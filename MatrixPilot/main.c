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
//#include "options.h"
//#include "delay.h"
//#include "debug.h"

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (USE_CONFIGFILE == 1)
#include "config.h"
#endif

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#define THIS_IS_STACK_APPLICATION
#include "MyIpNetwork.h"
#endif

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#endif


void testproc_loop(void);
void testproc_init(void);

int fs_init(void);
int fs_openlog(void);

int fs_test(void);
//int thinfat32_test(void);


//	main program for testing the IMU.

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
	preflight();
#endif
	udb_init();
	dcm_init();
#if (USE_CONFIGFILE == 1)
	init_config();
#endif
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
	quad_init();
#else // AIRFRAME_TYPE

#if !(BOARD_TYPE & AUAV2_BOARD)
	init_servoPrepare();
	init_states();
	init_behavior();
	init_serial();
#endif // (BOARD_TYPE & AUAV2_BOARD)

#endif // AIRFRAME_TYPE
/*
	printf("Initialising Filesystem\r\n");
//	for (;;) {}
	
	fs_init();
	delay_ms(100);
	fs_openlog();
	delay_ms(100);

//	testproc_init();
//	fs_test();
//	thinfat32_test();
 */
	printf("MatrixPilot Initialised\r\n");

#ifdef USE_FREERTOS
	// initialise the RTOS

	// start the RTOS running, this function should never return
	vTaskStartScheduler();
	// but in case it does
#endif

	while (1)
	{
		udb_run();
#ifdef USE_DEBUG_IO
//		testproc_loop();
#endif
	}
	return 0;
}
