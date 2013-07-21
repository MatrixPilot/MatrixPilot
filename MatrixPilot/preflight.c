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
#if (CONSOLE_UART != 0)
#include "console.h"
#endif
#if (USE_TELELOG != 0)
#include "telemetry_log.h"
#endif

#if (BOARD_TYPE == AUAV3_BOARD)

#include "preflight.h"
#include "USB/usb.h"
#include "USB/usb_function_msd.h"
#include <stdio.h>
#include "usb_cdc.h"
#include "delay.h"

#if (HILSIM_USB != 1)

void preflight(void)
{
	printf("Initialising USB\r\n");
	USBDeviceInit();        //usb_device.c.  Initializes USB module SFRs and firmware variables to known states.
	#if defined(USB_INTERRUPT)
		USBDeviceAttach();
	#endif
//	delayMs(100);
	delay_ms(100);

	printf("Preflight setup\r\n");
	while (U1OTGSTATbits.VBUSVD)
	{
		#if defined(USB_POLLING)
		// Check bus status and service USB interrupts.
		USBDeviceTasks();   // Interrupt or polling method.  If using polling, must call
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
#if (USE_MSD != 0)
			MSDTasks();
#endif
			CDCTasks();
		}
#if (CONSOLE_UART != 0)
		console();
#endif
	}
	LED_BLUE = LED_OFF;
	LED_RED = LED_OFF;
	LED_GREEN = LED_OFF;
	LED_ORANGE = LED_OFF;
	printf("Preflight complete\r\n");
}

#else

void preflight(void)
{
	printf("Initialising USB\r\n");

	USBDeviceInit();    //usb_device.c.  Initializes USB module SFRs and firmware variables to known states.
#if defined(USB_INTERRUPT)
	USBDeviceAttach();
#endif
}

#endif // HILSIM_USB

void USBPollingService(void)
{
	if (U1OTGSTATbits.VBUSVD)   // If we detect the USB power has returned, assume an end-of-flight condition
	{
#if (USE_TELELOG != 0)
		log_close();        // Close the datalog file
#endif // USE_TELELOG
/*
		#if defined(USB_POLLING)
		// Check bus status and service USB interrupts.
		USBDeviceTasks();   // Interrupt or polling method.  If using polling, must call
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
		#endif // USB_POLLING

		// User Application USB tasks
		if ((USBDeviceState < CONFIGURED_STATE)||(USBSuspendControl==1))
		{
			// do nothing
		}
		else
		{
#if (USE_MSD != 0)
			MSDTasks();
#endif
			CDCTasks();
		}
 */
	}
}

#endif // (BOARD_TYPE == AUAV3_BOARD)
