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

#include "../libFAT32/thinfat32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libFAT32/fat32_ui.h"

#define NO_ERROR 0
#define FILE_OPEN_ERROR 1
#define DATA_READ_ERROR 2
#define DATA_WRITE_ERROR 3
#define DATA_MISMATCH_ERROR 4


#ifdef USE_DEBUG_IO
//#include "debug.h"
#include "uart1.h"
#endif

void testproc_loop(void);
void testproc_init(void);


#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#endif



/*
 * Open a file, write a string to it, return 0.
 * Return an appropriate error code if there's any problem.
 */
int test_basic_write(char *input_file, char *write_string) {
	TFFile *fp;
	int rc;

	fp = tf_fopen(input_file, "w");
	
	if(fp) {
		rc = tf_fwrite(write_string, 1, strlen(write_string), fp);
		if(rc) {
			tf_fclose(fp);
			return DATA_WRITE_ERROR;
		}
		else {
			tf_fclose(fp);
			return NO_ERROR;
		}
		
	}
	else {
		return FILE_OPEN_ERROR;
	}
}


//	main program for testing the IMU.

int main (void)
{
	TFFile *fp;
	int rc;

	udb_init() ;
	dcm_init() ;

	init_servoPrepare() ;
	init_states() ;
	init_behavior() ;
	init_serial() ;
	
	testproc_init();

	char * filename = "/test0.txt";
	fp = tf_fopen(filename, "w");

//	if(rc = test_basic_write("/test0.txt", "Hello, World!")) {
//		printf("[TEST] Basic 8.3 write test failed with error code 0x%x\n", rc);
//	}else { 
//		printf("[TEST] Basic 8.3 write test PASSED.\n"); 
//	}


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
