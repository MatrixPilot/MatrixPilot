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

#ifdef USE_DEBUG_IO
//#include "debug.h"
#include "uart1.h"
#endif

void testproc_loop(void);
void testproc_init(void);


#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#endif

//	main program for testing the IMU.

int main (void)
{
	udb_init() ;
	dcm_init() ;
	init_servoPrepare() ;
	init_states() ;
	init_behavior() ;
	init_serial() ;
	
	testproc_init();

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
