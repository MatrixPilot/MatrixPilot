// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2015 MatrixPilot Team
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


#include "../MatrixPilot/defines.h"
#include "mavlink_options.h"

#if (USE_MAVLINK == 1)

#include "../MatrixPilot/MAVLink.h"
#include "../libUDB/serialIO.h"
#include "../MatrixPilot/telemetry_log.h"
#include <string.h>

#if (PX4 == 1)

int mavlink_fh = 0;

int16_t mavlink_serial_send(mavlink_channel_t UNUSED(chan), const uint8_t buf[], uint16_t len)
{
//int _write(int file, char *ptr, int len);
//	_write(mavlink_fh, buf, len);
}

#else

#define	SERIAL_BUFFER_SIZE  MAVLINK_MAX_PACKET_LEN

static int16_t sb_index = 0;
static int16_t end_index = 0;
static char serial_interrupt_stopped = 1;
static uint8_t serial_buffer[SERIAL_BUFFER_SIZE];


void udb_serial_callback_received_byte(uint8_t rxchar)
{
	mavlink_input_byte(rxchar);
}

int16_t udb_serial_callback_get_byte_to_send(void)
{
	if (sb_index < end_index && sb_index < SERIAL_BUFFER_SIZE) // ensure never end up racing thru memory.
	{
		uint8_t txchar = serial_buffer[sb_index++];
		return txchar;
	}
	else
	{
		serial_interrupt_stopped = 1;
	}
	return -1;
}

//int16_t mavlink_serial_send(mavlink_channel_t UNUSED(chan), uint8_t buf[], uint16_t len)
int16_t mavlink_serial_send(mavlink_channel_t UNUSED(chan), const uint8_t buf[], uint16_t len) // RobD
// Note: Channel Number, chan, is currently ignored.
{
	int16_t start_index;
	int16_t remaining;

#if (USE_TELELOG == 1)
//printf("calling log_telemetry with %u bytes\r\n", len);
	log_telemetry(buf, len);
#endif // USE_TELELOG

	// Note at the moment, all channels lead to the one serial port
	if (serial_interrupt_stopped == 1)
	{
		sb_index = 0;
		end_index = 0;
	}
	start_index = end_index;
	remaining = SERIAL_BUFFER_SIZE - start_index;

//	printf("%u\r\n", remaining);

	if (len > remaining)
	{
		// Chuck away the entire packet, as sending partial packet
		// will break MAVLink CRC checks, and so receiver will throw it away anyway.
		return (-1);
	}
	if (remaining > 1)
	{
		memcpy(&serial_buffer[start_index], buf, len);
		end_index = start_index + len;
	}
	if (serial_interrupt_stopped == 1)
	{
		serial_interrupt_stopped = 0;
		udb_serial_start_sending_data();
	}
	return (1);
}

#endif // (PX4 == 1)

#endif // (USE_MAVLINK == 1)
