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
#include "../libDCM/gpsParseCommon.h"
#include "config.h"

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


#if (SILSIM == 1)
int mp_argc;
char **mp_argv;
int main(int argc, char** argv)
{
	// keep these values available for later
	mp_argc = argc;
	mp_argv = argv;
#else
int main(void)
{
	mcu_init();
#endif

#if (USE_TELELOG == 1)
	log_init();
#endif
#if (USE_USB == 1)
	preflight();    // perhaps this would be better called usb_init()
#endif
	gps_init();     // this sets function pointers so i'm calling it early for now
	udb_init();
	dcm_init();
	init_config();  // this will need to be moved up in order to support runtime hardware options
	init_servoPrepare();
	init_states();
	init_behavior();
	init_serial();
	
	if (setjmp())
	{
		// a processor exception occurred and we're resuming execution here 
		DPRINT("longjmp'd\r\n");
	}

//	MatrixPilot();

//#undef USE_FREERTOS
#ifdef USE_FREERTOS
	DPRINT("Initialising RTOS\r\n");
	init_tasks();	// initialise the RTOS
	DPRINT("Starting Scheduler\r\n");
	vTaskStartScheduler();	// start the RTOS running, this function should never return
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
#if (USE_TELELOG == 1)
	telemetry_log();
#endif
#if (USE_USB == 1)
	USBPollingService();
#endif
#if (CONSOLE_UART != 0 && SILSIM == 0)
	console();
#endif
	udb_run();
}
