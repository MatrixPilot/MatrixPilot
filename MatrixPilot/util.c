/* utility calls for RTD Serial Recovery (rtdsr)
 *
 * copyright (c) 2011 Pete B. <xtreamerdev@gmail.com>
 *
 * based on util.c from Realtek bootloader set_pll, not copyrighted
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "util.h"

/************************************************************************
 *  Definitions for UART0
 ************************************************************************/

//#define USE_HANDSHAKE                  /* uncomment to use RTS/CTS    */
#define POLLSIZE                0x405    /* 1029 */
#define HW_LIMIT_STOP   (POLLSIZE-64)    /* RTS OFF when 64 chars in buf */
#define HW_LIMIT_START  (POLLSIZE-32)    /* RTS  ON when 32 chars in buf */
#define SERIAL_MCR_DTR           0x01    /* Data Terminal Ready         */
#define SERIAL_MCR_RTS           0x02    /* Request To Send             */
#define SERIAL_LSR_DR            0x01    /* Data ready                  */
#define SERIAL_LSR_THRE          0x20    /* Transmit Holding empty      */
#define SERIAL_MSR_CTS           0x10    /* Clear to send               */


/************************************************************************
 *  Timings for Xtreamer
 ************************************************************************/

/* Estimated number of for(i=0;;i++); loops we can issue in a second */
#define FOR_LOOP_CYCLE 200000
/* Estimated number of calls to serial read we can issue in a second */
#define SERIAL_CYCLE 48000

#if 0

/************************************************************************
 *  Static variables
 ************************************************************************/
static UINT16  recv_buffer[POLLSIZE];
static UINT16* putptr = &recv_buffer[0];
static UINT16* getptr = &recv_buffer[0];

void sync(void)
{
//	asm(".set mips3");
//	asm("sync");
//	asm(".set mips1");
}

static int serial_poll(void)
{
	UINT32 lstat;
	UINT32 rdata;

	for (lstat = REG32(0xb801b214); lstat & SERIAL_LSR_DR; lstat = REG32(0xb801b214))
	{
		rdata = REG32(0xb801b200) & 0xff;
#ifdef USE_HANDSHAKE
        if (room <= HW_LIMIT_STOP * sizeof(*putptr))
        {
            REG32(0xb801b210) &= ~SERIAL_MCR_RTS;
        }
#endif
		*putptr = (lstat << 8) | rdata;
		/* increase putptr to its future position */
		if (++putptr >= &recv_buffer[POLLSIZE])
			putptr = &recv_buffer[0];
		if (putptr == getptr)
		{
			if (putptr == &recv_buffer[0])
				putptr = &recv_buffer[POLLSIZE-1];
			else
				putptr--;
		}
	}
	return lstat;
}

void serial_write(UINT8* p_param)
{
	UINT32 x;
#ifdef USE_HANDSHAKE
	UINT32 y;
#endif

	for (;;)
	{
		/* OBS: LSR_OE, LSR_PE, LSR_FE and LSR_BI are cleared on read */
		x = serial_poll();
		x = x & SERIAL_LSR_THRE;
#ifdef USE_HANDSHAKE
		y = REG32(0xb801b218);
		y = y & SERIAL_MSR_CTS;
		if (x && y)
#else
		if (x)
#endif
		break;
	}
	REG32(0xb801b200) = *p_param;
}

int serial_read()
{
	int c = -1;

	serial_poll();
	if (getptr != putptr)
	{
		c = *getptr & 0xFF;
		if (++getptr >= &recv_buffer[POLLSIZE])
		{
			getptr = &recv_buffer[0];
		}
#ifdef USE_HANDSHAKE
		if (((REG32(0xb801b210) & SERIAL_MCR_RTS) == 0) &&
			(((UINT32)getptr - (UINT32)putptr) &
			((POLLSIZE - 1) * sizeof(*getptr))
			>= HW_LIMIT_START * sizeof(*getptr)))
		{
				REG32(0xb801b210) |= SERIAL_MCR_RTS;
		}
#endif
	}
	return c;
}

/* Get a character with a timeout in seconds. Negative timeout means infinite */
int _getchar(int timeout)
{
	int i, c;

	for (i = 0; (timeout >= 0) ? (i < timeout * SERIAL_CYCLE) : (1); i++)
	{
		c = serial_read();
		if (c >= 0)
		{
			return c;
		}
	}
	return -1;
}

void _putchar(int c)
{
	unsigned char b = c & 0xFF;
	serial_write(&b);
}

void _memset(void *dst, UINT8 value, UINT32 size)
{
	UINT32 i;
	for (i = 0; i < size; i++)
		REG8(((UINT32)dst) + i) = value;
}

void _memcpy(void *dst, void *src, UINT32 size)
{
	UINT32 i;
	for (i = 0; i < size; i++)
		REG8(((UINT32)dst) + i) = REG8(((UINT32)src) + i);
}

#endif

void _sleep(int seconds)
{
	unsigned long i;
	/* This seems to produce the approximative results on Xtreamer Pro */
	for (i = 0; i < FOR_LOOP_CYCLE * seconds; i++);
}

int _getchar(int timeout)
{
	return -1;
}

void _putchar(int c)
{
	unsigned char b = c & 0xFF;
//	serial_write(&b);
}

char* _strdup(const char* str)
{
	int n;
	char* result = 0;

	if (str)
	{
		n = strlen(str) + 1;
		result = malloc(n);
		if (result)
		{
			memcpy(result, str, n);
		}
	}
	return result;
}
