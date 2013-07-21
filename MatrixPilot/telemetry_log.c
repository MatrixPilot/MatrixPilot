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
#include "../libUDB/oscillator.h"
#include "../libUDB/heartbeat.h"
#include "telemetry_log.h"
#include "MDD File System/FSIO.h"
#include "AT45D.h"
#include <string.h>
#include <stdarg.h>


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define LOGBUF_BUFFER_SIZE 512

static char logbuf1[LOGBUF_BUFFER_SIZE];
static char logbuf2[LOGBUF_BUFFER_SIZE];
static int lb1_end_index = 0;
static int lb2_end_index = 0;
static int lb_in_use = 1;
static char logfile_name[13];
static FSFILE* fsp = NULL;


static int16_t add_to_log(char* logbuf, int index, char* data, int len)
{
	int16_t end_index = 0;
	int16_t remaining = LOGBUF_BUFFER_SIZE - index;

	if (remaining < len) {
//		printf("LOGBUF discarding %u bytes\r\n", len - remaining);
	}
	if (remaining > 1)
	{
//		printf("start_index %u, remaining %u, len %u min %u\r\n", start_index, remaining, len, MIN(remaining, len));
		strncpy((char*)(&logbuf[index]), data, MIN(remaining, len));
		end_index = index + MIN(remaining, len);
		logbuf[end_index] = '\0';

	}
	return end_index;
}

// called from telemetry module at interrupt level to buffer new log data
void log_telemetry(char* data, int len)
{
	if (lb_in_use == 1)
	{
		lb1_end_index = add_to_log(logbuf1, lb1_end_index, data, len);
	}
	else
	{
		lb2_end_index = add_to_log(logbuf2, lb2_end_index, data, len);
	}
}

// called from telemetry module at interrrupt level to manage the log data buffers
void log_swapbuf(void)
{
	if (lb_in_use == 1)
	{
		lb_in_use = 2;
	}
	else
	{
		lb_in_use = 1;
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
	init_dataflash(MIPS);

	if (!FSInit())
	{
		AT45D_FormatFS();
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
		lb1_end_index = 0;  // empty the logfile ping-pong buffers
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
	FSFILE* fp = fsp;       // make a copy of our file pointer

	if (fsp)
	{
		fsp = NULL;         // close the door to any further writes
		FSfclose(fp);       // and close up the file
		printf("%s closed\r\n", logfile_name);
	}
}

//#define LOGFILE_ENABLE_PIN PORTBbits.RB0  // PGD
//#define LOGFILE_ENABLE_PIN PORTBbits.RB1  // PGC
#define LOGFILE_ENABLE_PIN PORTAbits.RA6  // DIG2

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
	if (fsp)
	{
		LED_BLUE = LED_ON;
		if (FSfwrite(str, 1, len, fsp) != len)
		{
//			printf("ERROR: FSfwrite\r\n");
			log_close();
		}
	}
}

// called from mainloop at background priority to write telemetry log data to log file
void telemetry_log(void)
{
	if (lb_in_use == 1)
	{
		if (lb2_end_index)
		{
			log_write(logbuf2, lb2_end_index);
			lb2_end_index = 0;
		}
	}
	else
	{
		if (lb1_end_index)
		{
			log_write(logbuf1, lb1_end_index);
			lb1_end_index = 0;
		}
	}
	log_check();
}
