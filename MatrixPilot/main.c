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
#include "console.h"
#include "MAVLink.h"
#include "servoMix.h"
#include "telemetry.h"
#include "flightplan.h"
#include "flightplan-waypoints.h"
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

void init_tasks(void);

#if (USE_MAVLINK == 1)
void parameter_table_init(void);
#endif

static jmp_buf buf;

int matrixpilot_init(void)
{
#if (USE_TELELOG == 1)
	log_init();
#endif
#if (USE_USB == 1)
	preflight();    // perhaps this would be better called usb_init()
#endif
	gps_init();     // this sets function pointers so i'm calling it early for now
	udb_init();     // configure clocks and enables global interrupts

	filesys_init(); // attempts to mount a file system
	config_init();  // reads .ini files otherwise initialises with defaults

	dcm_init();
	flightplan_init();

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
#error here
	quad_init();
#else // AIRFRAME_TYPE
	servoMix_init();
	servoPrepare_init();
	init_states();
	init_behavior();
	telemetry_init();
	mavlink_init();
#endif // AIRFRAME_TYPE

#ifdef _MSC_VER
#if (USE_MAVLINK == 1)
	parameter_table_init();
#endif // (USE_MAVLINK == 1)
#endif // _MSC_VER
	return 0;
}

int matrixpilot_loop(void)
{
#if (USE_TELELOG == 1)
	telemetry_log_service();
#endif
#if (USE_USB == 1)
	USBPollingService();
#endif
#if (CONSOLE_UART != 0 && SILSIM == 0)
	console();
#endif
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
	quad_background_task(); // this was called run_background_task()
#endif // AIRFRAME_TYPE
	udb_run();
	return 0;
}

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
	matrixpilot_init();

	if (setjmp(buf))
	{
		// a processor exception occurred and we're resuming execution here
		DPRINT("longjmp'd\r\n");
	}

//	MatrixPilot();
//	dcm_fract_test(472580108);

//#undef USE_FREERTOS
#ifdef USE_FREERTOS
	DPRINT("Initialising RTOS\r\n");
	init_tasks();   // initialise the RTOS
	DPRINT("Starting Scheduler\r\n");
	vTaskStartScheduler();  // start the RTOS running, this function should never return
	return 0;
}

void idle_task(void)
{
#else
#endif

	while (1)
	{
		matrixpilot_loop();
	}
}

#if 0
void vApplicationMallocFailedHook(void)
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c, heap_2.c or heap_4.c are used, then the
	size of the heap available to pvPortMalloc() is defined by
	configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
	API function can be used to query the size of free heap space that remains
	(although it does not provide information on how the remaining heap might
	be fragmented). */
	DPRINT("vApplicationMallocFailedHook()\r\n");
//	taskDISABLE_INTERRUPTS();
	for( ;; );
}
#endif // 0
