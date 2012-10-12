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

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#endif


void testproc_loop(void);
void testproc_init(void);
int fs_test(void);


//	main program for testing the IMU.

int main (void)
{
	udb_init() ;
	dcm_init() ;
#ifdef MP_QUAD
	quad_init();
#else // !MP_QUAD
	init_servoPrepare() ;
	init_states() ;
	init_behavior() ;
	init_telemetry() ;
#endif // MP_QUAD
	
//	testproc_init();
//	fs_test();

#ifdef USE_FREERTOS
	// initialise the RTOS

	// start the RTOS running, this function should never return
	vTaskStartScheduler();
	// but in case it does
#endif

	while (1)
	{	
		udb_run() ;
#ifdef USE_DEBUG_IO
		testproc_loop();
#endif
	}	
	return 0 ;
}

