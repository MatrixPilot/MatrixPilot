/*
 * File:   ring_buffer.c
 * Author: markw
 *
 * Created on August 2, 2013, 8:12 AM
 */

//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2013 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.

#include "options.h"
#ifdef USE_RING_BUFFER
void udb_serial_start_sending_data(void);

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include "ring_buffer.h"

// ring buffer code ported from Arduino SerialPort Library (C) 2011 GPLV3 by William Greiman.
// there was a serious bug in put(char*, int), fixed here in ring_putn(char*, int).
// RINGLEN is the usable number of bytes, RINGSIZE is the actual sizeof(ring_buffer).
// ring_tail is modified by ring_get at IPL5 when transmitting data via UART2.
// ring_head is not modified by ISRs: Since this is a transmit buffer, data is added
// to the queue by calling one of the put methods at IPL0.
#define RINGLEN 5800

#define RINGSIZE (RINGLEN+1)
static volatile int16_t ring_head = 0;
static volatile int16_t ring_tail = 0;
__attribute__((far)) char ring_buffer[RINGSIZE];

#if (MAVLINK_MAX_PACKET_LEN > RINGLEN)
#error "ringbuffer length must be >= MAVLINK_MAX_PACKET_LEN"
#endif

// called by udb_serial_callback_get_byte_to_send at IPL5
// modifies ring_tail

bool ring_get(char* b)
{
	int16_t t = ring_tail;
	if (ring_head == t)
	{
		// buffer is empty
		return false;
	}
	*b = ring_buffer[t++];
	ring_tail = t < RINGSIZE ? t : 0;
	return true;
}

// insert 1 byte at head of buffer
// return number of bytes stored, zero if buffer is full

int16_t ring_put(char b)
{
	int16_t h = ring_head;
	// OK to store here even if ring is full
	ring_buffer[h++] = b;
	if (h >= RINGSIZE) h = 0;
	if (h == ring_tail) return 0; // buffer is full, didn't lose b (yet)
	ring_head = h;
	return 1;
}

// insert n bytes at head of buffer; modifies head at IPL5
// If space available is less than n, store only space bytes.
// return number of bytes stored, zero if buffer is full

int16_t ring_putn(const char* b, int16_t n)
{
	// disable U2TX interrupt to fetch ring_tail
	_U2TXIE = 0;

	int16_t t = ring_tail;

	// re-enable U2TX interrupt
	_U2TXIE = 1;

	int16_t h = ring_head;
	int16_t space;
	if (h < t)
	{
		space = t - h - 1;
	}
	else
	{
		space = RINGLEN - h + t;
	}
	if (n > space) n = space;
	if ((n + h) <= RINGSIZE)
	{
		memcpy(&ring_buffer[h], b, n);
	}
	else
	{
		int16_t n1 = RINGSIZE - h;
		memcpy(&ring_buffer[h], b, n1);
		memcpy(ring_buffer, &b[n1], n - n1);
	}
	h += n;
	ring_head = h < RINGSIZE ? h : h - RINGSIZE;
	return n;
}

// return number of bytes in buffer

int16_t ring_available()
{
	// disable U2TX interrupt to fetch ring_tail
	_U2TXIE = 0;

	int16_t n = ring_head - ring_tail;

	// re-enable U2TX interrupt
	_U2TXIE = 1;

	return n < 0 ? RINGSIZE + n : n;
}

// return space available in buffer (in bytes)

int16_t ring_space()
{
	int16_t space;
	// disable U2TX interrupt to fetch ring_tail
	_U2TXIE = 0;

	int16_t t = ring_tail;

	// re-enable U2TX interrupt
	_U2TXIE = 1;

	if (ring_head < t)
	{
		space = t - ring_head - 1;
	}
	else
	{
		space = RINGLEN - ring_head + t;
	}

	return space;
}

int16_t queue_data(const char* buff, int16_t nbytes)
{
	if (ring_space() > nbytes)
	{
		ring_putn(buff, nbytes);
		return(1);
	}
	else
	{
		return(-1);
	}
}

// send string out telemetry port

int16_t queue_string(const char* string)
{
	return queue_data(string, strlen(string));
}

// queue a string without null terminator

void queue_prepend(const char* buff, int16_t nbytes)
{
	if (ring_space() > nbytes - 1)
	{
		ring_putn(buff, nbytes - 1);
	}
}

#endif
