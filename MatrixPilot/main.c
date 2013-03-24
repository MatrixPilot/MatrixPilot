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

#include <stdio.h>
#include <stdint.h>
#include "../libFlashFS/USB/usb.h"
#include "../libFlashFS/USB/usb_function_msd.h"
#include "../libFlashFS/AT45D.h"
#include "../libFlashFS/FSIO.h"

//	main program for testing the IMU.

void mcu_init(void);
void DisplayFS(void);
void TestFS(void);
int fs_openconfig(char*);
int openconfig(char* filename);
int fs_openlog(char* filename);
int fs_log(char* str);
int fs_closelog(void);
void write_logbuf(void);

#if (SILSIM == 1)
int mp_argc;
char **mp_argv;
int main(int argc, char** argv)
{
	// keep thees values available for later
	mp_argc = argc;
	mp_argv = argv;
#else
int main (void)
{
#endif
	mcu_init();
	udb_init() ;
	dcm_init() ;
	init_servoPrepare() ;
	init_states() ;
	init_behavior() ;
	init_serial() ;
    init_dataflash();
//	AT45D_DisplaySettings();
//	AT45D_WipeFS();
	if (FSInit()) {
		printf("File system initalised\r\n");
		DisplayFS();
//		TestFS();

//		fs_openconfig("config.txt");
//		openconfig("config.txt");

		fs_openlog("logfile.txt");
		fs_log("this is a test string\r\n");
		fs_closelog();
	} else {
		printf("File system failed\r\n");
	}
	
	printf("\r\nInitialising USB\r\n");	
    USBDeviceInit();	//usb_device.c.  Initializes USB module SFRs and firmware
    					//variables to known states.
    #if defined(USB_INTERRUPT)
        USBDeviceAttach();
    #endif


	printf("MatrixPilot running...\r\n");
    while (1)
    {
        #if defined(USB_POLLING)
		// Check bus status and service USB interrupts.
        USBDeviceTasks(); // Interrupt or polling method.  If using polling, must call
        				  // this function periodically.  This function will take care
        				  // of processing and responding to SETUP transactions 
        				  // (such as during the enumeration process when you first
        				  // plug in).  USB hosts require that USB devices should accept
        				  // and process SETUP packets in a timely fashion.  Therefore,
        				  // when using polling, this function should be called 
        				  // regularly (such as once every 1.8ms or faster** [see 
        				  // inline code comments in usb_device.c for explanation when
        				  // "or faster" applies])  In most cases, the USBDeviceTasks() 
        				  // function does not take very long to execute (ex: <100 
        				  // instruction cycles) before it returns.
        #endif

	    // User Application USB tasks
    	if ((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1)) {
			// do nothing
		} else {
			MSDTasks();    
		}
	}
	while (1) {
		udb_run() ;
	}
	
	return 0 ;
}
