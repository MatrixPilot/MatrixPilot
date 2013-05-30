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

#include "USB/usb.h"
#include "USB/usb_function_cdc.h"

void BlinkUSBStatus(void);

char USB_In_Buffer[CDC_DATA_OUT_EP_SIZE];
char USB_Out_Buffer[CDC_DATA_OUT_EP_SIZE];
USB_HANDLE USBOutHandle = 0;  // Needs to be initialized to 0 at startup.
USB_HANDLE USBInHandle = 0;   // Needs to be initialized to 0 at startup.
BOOL blinkStatusValid = TRUE;


void CDCTasks(void)
{
	BYTE numBytesRead;

	// Blink the LEDs according to the USB device status
	if (blinkStatusValid)
	{
		BlinkUSBStatus();
	}

	// User Application USB tasks
	if ((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1))
	{
		return;
	}

#if (USE_HILSIM_USB == 1)

	numBytesRead = getsUSBUSART(USB_In_Buffer, sizeof(USB_In_Buffer));
	if (numBytesRead != 0)
	{
		int i = 0;
		while (i < numBytesRead)
		{
			udb_gps_callback_received_byte(USB_In_Buffer[i++]);
		}
	}

	if (mUSBUSARTIsTxTrfReady())
	{
		int i = 0;
		int txchar;
		while ((i < sizeof(USB_Out_Buffer))) && ((txchar = udb_gps_callback_get_byte_to_send()) != -1))
		{
			USB_Out_Buffer[i++] = txchar;
		}
		if (i > 0)
		{
			putUSBUSART(USB_Out_Buffer, i);
		}
	}

#else
	if (mUSBUSARTIsTxTrfReady())
	{
		numBytesRead = getsUSBUSART(USB_Out_Buffer, sizeof(USB_Out_Buffer));
		if (numBytesRead != 0)
		{
			BYTE i;

			for (i = 0; i < numBytesRead; i++)
			{
				switch (USB_Out_Buffer[i])
				{
					case 0x0A:
					case 0x0D:
						USB_In_Buffer[i] = USB_Out_Buffer[i];
						break;
					default:
						USB_In_Buffer[i] = USB_Out_Buffer[i] + 1;
						break;
				}
			}
			putUSBUSART(USB_In_Buffer, numBytesRead);
		}
	}
#endif
	CDCTxService();
}

#define LED_BLUE			LATBbits.LATB2
#define LED_ORANGE			LATBbits.LATB5

#define LED_ON				0
#define LED_OFF				1

#define led_toggle(x)		((x) = !(x))

#define mLED_Both_Off()		{LED_BLUE = LED_OFF; LED_ORANGE = LED_OFF;}
#define mLED_Both_On()		{LED_BLUE = LED_ON;  LED_ORANGE = LED_ON;}
#define mLED_Only_1_On()	{LED_BLUE = LED_ON;  LED_ORANGE = LED_OFF;}
#define mLED_Only_2_On()	{LED_BLUE = LED_OFF; LED_ORANGE = LED_ON;}

#define mLED_1_Toggle()		{led_toggle(LED_BLUE);}
#define mLED_2_Toggle()		{led_toggle(LED_ORANGE);}

#define mLED_1_On()			{LED_BLUE = LED_ON;}
#define mLED_2_On()			{LED_ORANGE = LED_ON;}
#define mLED_1_Off()		{LED_BLUE = LED_OFF;}
#define mLED_2_Off()		{LED_ORANGE = LED_OFF;}

#define mGetLED_1()			LED_BLUE
#define mGetLED_2()			LED_ORANGE

//    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
//    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
//    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
//    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}

void BlinkUSBStatus(void)
{
    static WORD led_count = 0;
    
    if (led_count == 0) led_count = 10000U;
    led_count--;

    if (USBSuspendControl == 1)
    {
        if (led_count == 0)
        {
            mLED_1_Toggle();
            if (mGetLED_1())
            {
                mLED_2_On();
            }
            else
            {
                mLED_2_Off();
            }
        }
    }
    else
    {
        if (USBDeviceState == DETACHED_STATE)
        {
            mLED_Both_Off();
        }
        else if (USBDeviceState == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if (USBDeviceState == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if (USBDeviceState == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if (USBDeviceState == ADDRESS_STATE)
        {
            if (led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }
        }
        else if (USBDeviceState == CONFIGURED_STATE)
        {
            if (led_count == 0)
            {
                mLED_1_Toggle();
                if (mGetLED_1())
                {
                    mLED_2_Off();
                }
                else
                {
                    mLED_2_On();
                }
            }
        }
    }
}

#if defined(USB_CDC_SET_LINE_CODING_HANDLER)
void mySetLineCodingHandler(void)
{
    //If the request is not in a valid range
    if (cdc_notice.GetLineCoding.dwDTERate.Val > 115200)
    {
        //NOTE: There are two ways that an unsupported baud rate could be
        //handled.  The first is just to ignore the request and don't change
        //the values.  That is what is currently implemented in this function.
        //The second possible method is to stall the STATUS stage of the request.
        //STALLing the STATUS stage will cause an exception to be thrown in the 
        //requesting application.  Some programs, like HyperTerminal, handle the
        //exception properly and give a pop-up box indicating that the request
        //settings are not valid.  Any application that does not handle the
        //exception correctly will likely crash when this requiest fails.  For
        //the sake of example the code required to STALL the status stage of the
        //request is provided below.  It has been left out so that this demo
        //does not cause applications without the required exception handling
        //to crash.
        //---------------------------------------
        //USBStallEndpoint(0,1);
    }
    else
    {
        //DWORD_VAL dwBaud;

        //Update the baudrate info in the CDC driver
        CDCSetBaudRate(cdc_notice.GetLineCoding.dwDTERate.Val);
        
        //Update the baudrate of the UART
//        #if defined(__18CXX)
//            dwBaud.Val = (GetSystemClock()/4)/line_coding.dwDTERate.Val-1;
//            SPBRG = dwBaud.v[0];
//            SPBRGH = dwBaud.v[1];
//        #elif defined(__C30__) || defined __XC16__
//            dwBaud.Val = (((GetPeripheralClock()/2)+(BRG_DIV2/2*line_coding.dwDTERate.Val))/BRG_DIV2/line_coding.dwDTERate.Val-1);
//            U2BRG = dwBaud.Val;
//        #elif defined(__C32__)
//            U2BRG = ((GetPeripheralClock()+(BRG_DIV2/2*line_coding.dwDTERate.Val))/BRG_DIV2/line_coding.dwDTERate.Val-1);
//            //U2MODE = 0;
//            U2MODEbits.BRGH = BRGH2;
//            //U2STA = 0;
//        #endif
    }
}
#endif // USB_CDC_SET_LINE_CODING_HANDLER
