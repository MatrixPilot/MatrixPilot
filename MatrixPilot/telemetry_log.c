// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
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


#include "defines.h"
#include "../libUDB/heartbeat.h"
#include "telemetry_log.h"
#if (WIN == 1 || NIX == 1)
#include <stdio.h>
#include "SIL-filesystem.h"
#else
#include "MDD File System/FSIO.h"
#include "AT45D.h"
#endif
#include <string.h>
#include <stdarg.h>


#if (WIN == 1 || NIX == 1)
#define LOGFILE_ENABLE_PIN 0
#else
#if defined( __dsPIC33E__ )
//#define LOGFILE_ENABLE_PIN PORTBbits.RB0  // PGD
//#define LOGFILE_ENABLE_PIN PORTBbits.RB1  // PGC
#define LOGFILE_ENABLE_PIN PORTAbits.RA6  // DIG2
#elif defined( __dsPIC33F__ )
//#define LOGFILE_ENABLE_PIN PORTAbits.RA5
#define LOGFILE_ENABLE_PIN 1 // don't force logfile open
#else
#error unknown processor family
#endif

#endif

boolean log_enabled(void)
{
	return (LOGFILE_ENABLE_PIN ? false : true);
}

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define LOGBUF_BUFFER_SIZE 512

static char logbuf1[LOGBUF_BUFFER_SIZE];
static char logbuf2[LOGBUF_BUFFER_SIZE];
static volatile int lb1_end_index = 0;
static volatile int lb2_end_index = 0;
static volatile int lb_in_use = 1;
static volatile int lb_flush = 0;
static char logfile_name[13];
static FSFILE* fsp = NULL;

int log_file_counter = 0;
#if 0
static int16_t log_append(char* logbuf, int index, const uint8_t* data, int len, int dbg)
{
	int16_t end_index = 0;

	log_file_counter += len;

//	assert(len <= (LOGBUF_BUFFER_SIZE - index));

	if (len <= (LOGBUF_BUFFER_SIZE - index))
	{
	}
	else
	{
		printf("%u: index %u, len %u, remaining %i\r\n", dbg, index, len, (LOGBUF_BUFFER_SIZE - index));
		return end_index; // dump characters to avoid buffer overflow
	}
	strncpy((char*)(&logbuf[index]), data, len);
	end_index = index + len;
	logbuf[end_index] = '\0'; // TODO: i don't think we really want this...
	return end_index;
}
#else
static int16_t log_append(char* logbuf, int index, const uint8_t* data, int len, int dbg)
{
//	int16_t end_index = 0;

	log_file_counter += len;

//	assert(len <= (LOGBUF_BUFFER_SIZE - index));

	if (len <= (LOGBUF_BUFFER_SIZE - index))
	{
//		strncpy((char*)(&logbuf[index]), data, len);
		memcpy(&logbuf[index], data, len);
//		end_index = index + len;
//		logbuf[index + len] = '\0'; // TODO: i don't think we really want this...
		return (index + len);
	}
	printf("%u: index %u, len %u, remaining %i\r\n", dbg, index, len, (LOGBUF_BUFFER_SIZE - index));
	// dump data to avoid buffer overflow
	return index;
}
#endif
// called from telemetry module at interrupt level to buffer new log data
/*
void log_telemetry(char* data, int len)
{
	int16_t remaining;

	if (fsp == NULL)
	{
		return;
	}

	if (lb_in_use == 1)
	{
		remaining = LOGBUF_BUFFER_SIZE - lb1_end_index;
		if (remaining <= len) { // TODO: just changed this to debug earlier problem, maybe need to revert to only <
			lb1_end_index = log_append(logbuf1, lb1_end_index, data, remaining, 1);
			data += remaining;
			len -= remaining;
			lb_in_use = 2;
			if (lb_flush != 0)
			{
				printf("ERROR: buffer %u still busy\r\n", lb_flush);
			}
			lb_flush = 1;
			lb2_end_index = log_append(logbuf2, lb2_end_index, data, len, 2);
		} else {
			lb1_end_index = log_append(logbuf1, lb1_end_index, data, len, 3);
		}
	}
	else
	{
		remaining = LOGBUF_BUFFER_SIZE - lb2_end_index;
		if (remaining <= len) {
			lb2_end_index = log_append(logbuf2, lb2_end_index, data, remaining, 4);
			data += remaining;
			len -= remaining;
			lb_in_use = 1;
			if (lb_flush != 0)
			{
				printf("ERROR: buffer %u still busy\r\n", lb_flush);
			}
			lb_flush = 2;
			lb1_end_index = log_append(logbuf1, lb1_end_index, data, len, 5);
		} else {
			lb2_end_index = log_append(logbuf2, lb2_end_index, data, len, 6);
		}
	}
}
 */
void log_telemetry(const uint8_t* data, int len)
{
	int16_t remaining;

	if (fsp == NULL)
	{
		return;
	}

	if (lb_in_use == 1)
	{
		remaining = LOGBUF_BUFFER_SIZE - lb1_end_index;
		if (remaining <= len) { // TODO: just changed this to debug earlier problem, maybe need to revert to only <
			lb1_end_index = log_append(logbuf1, lb1_end_index, data, remaining, 1);
			data += remaining;
			len -= remaining;
			lb_in_use = 2;
			if (lb_flush != 0) // logbuf2 is still being written?
			{
				printf("ERROR: buffer %u still busy\r\n", lb_flush);
			}
			lb_flush = 1;
			lb2_end_index = log_append(logbuf2, lb2_end_index, data, len, 2);
		} else {
			lb1_end_index = log_append(logbuf1, lb1_end_index, data, len, 3);
		}
	}
	else
	{
		remaining = LOGBUF_BUFFER_SIZE - lb2_end_index;
		if (remaining <= len) {
			lb2_end_index = log_append(logbuf2, lb2_end_index, data, remaining, 4);
			data += remaining;
			len -= remaining;
			lb_in_use = 1;
			if (lb_flush != 0) // logbuf1 is still being written?
			{
				printf("ERROR: buffer %u still busy\r\n", lb_flush);
			}
			lb_flush = 2;
			lb1_end_index = log_append(logbuf1, lb1_end_index, data, len, 5);
		} else {
			lb2_end_index = log_append(logbuf2, lb2_end_index, data, len, 6);
		}
	}
}

static int fs_nextlog(char* filename)
{
	FSFILE* fp;
	int i;

	for (i = 0; i < 99; i++)
	{
		sprintf(filename, "log%02u.txt", i);
		fp = FSfopen(filename, "r");
		if (fp != NULL)
		{
			FSfclose(fp);
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

// called at startup to initialise the telemetry log system
void log_init(void)
{
//	init_dataflash(); // this should now be getting device specific called from lower layers via FSInit()
	if (!FSInit())
	{
#ifdef USE_AT45D_FLASH
		AT45D_FormatFS();
#elif (WIN == 1) || (NIX == 1)
#else
#warning No Mass Storage Device Format Function Defined
#endif // USE_AT45D_FLASH
		if (!FSInit())
		{
			printf("File system initialisation failed\r\n");
			return;
		}
	}
	printf("File system initalised\r\n");
}

void restart_telemetry(void);

static void log_open(void)
{
	static uint8_t log_error = 0;

	if (log_error) return;

	log_close();            // just in case the calling code is dumb..
	if (!fs_nextlog(logfile_name))
	{
		strcpy(logfile_name, "fp_log.txt");
	}
	fsp = FSfopen(logfile_name, "a");
	if (fsp != NULL)
	{
		lb1_end_index = 0;  // purge the logfile ping-pong buffers
		lb2_end_index = 0;
		restart_telemetry();// signal telemetry to send startup data again
		printf("%s opened\r\n", logfile_name);
	}
	else
	{
		printf("%s failed\r\n", logfile_name);
		log_error = 1;      // don't allow further attempts to open logfile
	}
}

// this may be called at interrupt or background level
void log_close(void)
{
	FSFILE* fp = fsp;   // make a copy of our file pointer

	if (fsp)
	{
		fsp = NULL;     // close the door to any further writes
		FSfclose(fp);   // and close up the file
		printf("%s closed\r\n", logfile_name);
	}
}

void restart_telemetry(void);
boolean inflight_state(void);

static void log_check(void)
{
	static uint16_t debounce = 0;

	if (debounce)
	{
		debounce--;
		return;
	}
	if (fsp)
	{
		if (LOGFILE_ENABLE_PIN == 1 && !inflight_state())
		{
			debounce = 5000;// arbitrary number
			log_close();
		}
	}
	else
	{
		if (LOGFILE_ENABLE_PIN == 0 || inflight_state())
		{
			debounce = 5000;// arbitrary number
			log_open();
		}
	}
}

static void log_write(char* str, int len)
{
//	printf("log_write() %u bytes\r\n", len);
	if (fsp)
	{
		led_on(LED_BLUE);
		if (FSfwrite(str, 1, len, fsp) != len)
		{
			DPRINT("ERROR: FSfwrite\r\n");
			log_close();
		}
	}
}

// called from mainloop at background priority to write telemetry log data to log file
void telemetry_log_service(void)
{
	log_check();

//	if (lb_flush == 1)
	if (lb1_end_index >= LOGBUF_BUFFER_SIZE)
	{
		log_write(logbuf1, lb1_end_index);
		lb1_end_index = 0;
		lb_flush = 0;
	}

//	if (lb_flush == 2)
	if (lb2_end_index >= LOGBUF_BUFFER_SIZE)
	{
		log_write(logbuf2, lb2_end_index);
		lb2_end_index = 0;
		lb_flush = 0;
	}
}
