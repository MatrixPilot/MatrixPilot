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
#include "behaviour.h"
#include "servoPrepare.h"
#include "../libDCM/gpsParseCommon.h"
#include "config.h"
#include "states.h"
#include "flightplan-waypoints.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mavlink_options.h"

#include <setjmp.h>

#include "../libFlashFS/filesys.h"

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (CONSOLE_UART != 0)
#include "console.h"
#endif

void TaskInit_Create(void);
void vApplicationIdleHook(void);
void init_serial(void);

static jmp_buf buf;

#if (SILSIM == 1)
int mp_argc;
char** mp_argv;
int main(int argc, char** argv)
{
	// keep these values available for later
	mp_argc = argc;
	mp_argv = argv;
#else
int main(void)
{
	mcu_init();     // initialise the processor specific registers
#endif

//#if (USE_TELELOG == 1)
//	log_init();
//#endif
#if (USE_USB == 1)
	preflight();    // perhaps this would be better called usb_init()
#endif
	gps_init();     // this sets function pointers so i'm calling it early for now
	udb_init();     // configure clocks and enables global interrupts
	filesys_init(); // attempts to mount a file system
	config_init();  // reads .ini files otherwise initialises with defaults
	dcm_init();
//	init_waypoints();
	init_servoPrepare();
	init_states();
	init_behavior();
	init_serial();

	if (setjmp(buf))
	{
		// a processor exception occurred and we're resuming execution here 
		DPRINT("longjmp'd\r\n");
	}

//	MatrixPilot();

//#undef USE_FREERTOS
#ifdef USE_FREERTOS
	DPRINT("Initialising RTOS\r\n");
	TaskInit_Create();   // initialise the RTOS
	DPRINT("Starting Scheduler\r\n");
	vTaskStartScheduler();  // start the RTOS running, this function should never return
#else
	while (1)
	{
		vApplicationIdleHook();
	}
#endif // USE_FREERTOS
	return 0;
}

void vApplicationIdleHook(void)
{
//	static int i = 0;
//*
#if (USE_TELELOG == 1)
	telemetry_log();
#endif
#if (USE_USB == 1)
	USBPollingService();
#endif
#if (CONSOLE_UART != 0 && SILSIM == 0)
	console();
#endif
//#ifndef (USE_FREERTOS)
	udb_run();
//#endif
// */
}

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
	DPRINT("Stack: %s\r\n", pcTaskName);
}

void vApplicationMallocFailedHook(void)
{
	DPRINT("Malloc Failed\r\n");
}

uint16_t rtos_ticks = 0;

#ifdef USE_FREERTOS
void vApplicationTickHook(void) // 1000 Hz
{
	static int16_t i = 0;

	rtos_ticks++;

//	if (++i > (configTICK_RATE_HZ / HEARTBEAT_HZ)) // 40 Hz
//	{
//		i = 0;
//		// heartbeat() is now registered as a callback with the MPU6000 driver @ 200Hz
//	}
}
#endif // USE_FREERTOS

