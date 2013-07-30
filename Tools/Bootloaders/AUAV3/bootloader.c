/*********************************************************************
 *
 *                  dsPIC33E/PIC24E Bootloader
 *
 *********************************************************************
 * FileName:        bootloader.c
 * Dependencies:
 * Processor:       dsPIC33E/PIC24E
 *
 * Compiler:        MPLAB C30
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC3E/PIC24E Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip dsPIC3E/PIC24E Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************/
#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "bootloader.h"

#include "USB/usb.h"
#include "USB/usb_function_msd.h"
#include "MDD File System/FSIO.h"
#include "NVMem.h"


#define SWITCH_PRESSED                  0
#define AUX_FLASH_BASE_ADRS             (0x7FC000)
#define AUX_FLASH_END_ADRS              (0x7FFFFF)
#define DEV_CONFIG_REG_BASE_ADDRESS     (0xF80000)
#define DEV_CONFIG_REG_END_ADDRESS      (0xF80012)

#define REC_FLASHED                     0
#define REC_NOT_FOUND                   1
#define REC_FOUND_BUT_NOT_FLASHED       2

#define DATA_RECORD 		            0
#define END_OF_FILE_RECORD 	            1
#define EXT_SEG_ADRS_RECORD             2
#define EXT_LIN_ADRS_RECORD             4

typedef struct
{
    UINT8 *start;
    UINT8 len;
    UINT8 status;
} T_REC;

typedef struct 
{
	UINT8 RecDataLen;
	DWORD_VAL Address;
	UINT8 RecType;
	UINT8* Data;
	UINT8 CheckSum;	
	DWORD_VAL ExtSegAddress;
	DWORD_VAL ExtLinAddress;
} T_HEX_RECORD;	

static void file_flash(FSFILE* file);
static void ConvertAsciiToHex(UINT8* asciiRec, UINT8* hexRec);
static void WriteHexRecord2Flash(UINT8* HexRecord);
void AT45D_FormatFS(void);

T_REC record;
int MDD_AT45D_Write_Activity = 0;
UINT pointer = 0;
UINT8 hexRec[100];
UINT8 asciiRec[200];
UINT8 asciiBuffer[512+200]; // and entire sector read plus left over hex record from previous pass
                            // example matrixpilot hex records are 44+crlf bytes long
                            // however we are allowing for them to be up to 200 characters each

#define ERR_FS_INIT	     1
#define ERR_NV_ERASE     2
#define ERR_HEX_CSUM     3

void error(int code)
{
	RedLED = LED_OFF;
	BlueLED = LED_OFF;
	GreenLED = LED_OFF;

	switch (code)
	{
	case ERR_FS_INIT:
//		printf("FSInit failed\r\n");
		BlueLED = LED_ON;
		break;
	case ERR_NV_ERASE:
//		printf("NV flash erase failed\r\n");
		RedLED = LED_ON;
		break;
	case ERR_HEX_CSUM:
//		printf("HEX record checksum failed\r\n");
		GreenLED = LED_ON;
		break;
	}

	while (1)
	{
		BlinkOrangeLED();
	}
}

void bootloader(void)
{
	FSFILE* file;

	USBInitialize(0);

	if (!FSInit())
	{
		// File system failed - pretty much DISKmount didn't work
		AT45D_FormatFS();
		if (!FSInit())
		{
			error(ERR_FS_INIT);
		}
	}

	while (1)
	{
		USBTasks();
		BlinkBlueLED();

		// User Application USB tasks
		if ((USBDeviceState < CONFIGURED_STATE) || (USBSuspendControl == 1))
		{
			// do nothing
		}
		else
		{
			BlinkGreenLED();
			MSDTasks();
		}
		if (_T1IF)
		{
			_T1IF = 0;

			// check for MSD activity...
			if (MDD_AT45D_Write_Activity)
			{
				MDD_AT45D_Write_Activity = 0;
			}
			else
			{
				file = FSfopen("image.hex", "r");
				if (file != NULL)
				{
					file_flash(file);
					FSfclose(file);
					FSremove("image.hex");
					//AT45D_FormatFS();
					return;
				}
			}
		}
	}
}

static void file_flash(FSFILE* file)
{
	UINT readBytes;
	UINT i;

	// Erase Flash (Block Erase the program Flash)
	if (NVMemBlockErase() != 0)
	{
		error(ERR_NV_ERASE);
	}

	record.status = REC_NOT_FOUND;      // Initialize the state-machine to read the records.

	while (1)
	{
		USBTasks();
		BlinkBlueLED();
		BlinkOrangeLED();

		// For a faster read, fetch 512 bytes at a time and buffer it.
		readBytes = FSfread((void*)&asciiBuffer[pointer], 1, 512, file);

		if (readBytes == 0)
		{
			// Nothing to read. Come out of this loop
			// break;
			// Jump to start of application
			// Disable all enabled interrupts (only USB)
			// before jumping to the application code.
			IEC5bits.USB1IE = 0;
//			JumpToApp();
			return;
		}

		for (i = 0; i < (readBytes + pointer); i ++)
		{
			// This state machine seperates-out the valid hex records from the read 512 bytes.
			switch (record.status)
			{
			case REC_FLASHED:
			case REC_NOT_FOUND:
				if (asciiBuffer[i] == ':')
				{
					// We have a record found in the 512 bytes of data in the buffer.
					record.start = &asciiBuffer[i];
					record.len = 0;
					record.status = REC_FOUND_BUT_NOT_FLASHED;
				}
				break;
			case REC_FOUND_BUT_NOT_FLASHED:
				if ((asciiBuffer[i] == 0x0A) || (asciiBuffer[i] == 0xFF))
				{
					// We have got a complete record. (0x0A is new line feed and 0xFF is End of file)
					// Start the hex conversion from element
					// 1. This will discard the ':' which is
					// the start of the hex record.
					ConvertAsciiToHex(&record.start[1], hexRec);
					WriteHexRecord2Flash(hexRec);
					record.status = REC_FLASHED;
				}
				break;
			}
			record.len ++;  // Move to next byte in the buffer.
		}

		if (record.status == REC_FOUND_BUT_NOT_FLASHED)
		{
			// We still have a half read record in the buffer. The next half part of the record is read 
			// when we read 512 bytes of data from the next file read. 
			memcpy(asciiBuffer, record.start, record.len);
			pointer = record.len;
			record.status = REC_NOT_FOUND;
		}
		else
		{
			pointer = 0;
		}
	}
}

/****************************************************************************
  Function:
    BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size)

  Description:
    This is the application event handler.  It is called when the stack has
    an event that needs to be handled by the application layer rather than
    by the client driver.  If the application is able to handle the event, it
    returns TRUE.  Otherwise, it returns FALSE.

  Parameters:
    BYTE address    - Address of device where event occurred
    USB_EVENT event - Identifies the event that occured
    void *data      - Pointer to event-specific data
    DWORD size      - Size of the event-specific data

  Return Values:
    TRUE    - The event was handled
    FALSE   - The event was not handled

  Remarks:
    The application may also implement an event handling routine if it
    requires knowledge of events.  To do so, it must implement a routine that
    matches this function signature and define the USB_HOST_APP_EVENT_HANDLER
    macro as the name of that function.
  ***************************************************************************/

BOOL USB_ApplicationEventHandler(BYTE address, USB_EVENT event, void *data, DWORD size)
{
	switch (event)
	{
		case EVENT_VBUS_REQUEST_POWER:
			// The data pointer points to a byte that represents the amount of power
			// requested in mA, divided by two.  If the device wants too much power,
			// we reject it.
			return TRUE;

		case EVENT_VBUS_RELEASE_POWER:
			// Turn off Vbus power.
			// The PIC24F with the Explorer 16 cannot turn off Vbus through software.
			// This means that the device was removed
			return TRUE;
			break;

		case EVENT_HUB_ATTACH:
			return TRUE;
			break;

		case EVENT_UNSUPPORTED_DEVICE:
			return TRUE;
			break;

		case EVENT_CANNOT_ENUMERATE:
			//UART2PrintString( "\r\n***** USB Error - cannot enumerate device *****\r\n" );
			return TRUE;
			break;

		case EVENT_CLIENT_INIT_ERROR:
			//UART2PrintString( "\r\n***** USB Error - client driver initialization error *****\r\n" );
			return TRUE;
			break;

		case EVENT_OUT_OF_MEMORY:
			//UART2PrintString( "\r\n***** USB Error - out of heap memory *****\r\n" );
			return TRUE;
			break;

		case EVENT_UNSPECIFIED_ERROR:   // This should never be generated.
			//UART2PrintString( "\r\n***** USB Error - unspecified *****\r\n" );
			return TRUE;
			break;

		default:
			break;
	}
	return FALSE;
}

static void ConvertAsciiToHex(UINT8* asciiRec, UINT8* hexRec)
{
	UINT8 i = 0;
	UINT8 k = 0;
	UINT8 hex;

	while ((asciiRec[i] >= 0x30) && (asciiRec[i] <= 0x66))
	{
		// Check if the ascci values are in alpha numeric range.
		if (asciiRec[i] < 0x3A)
		{
			// Numerical reperesentation in ASCII found.
			hex = asciiRec[i] & 0x0F;
		}
		else
		{
			// Alphabetical value.
			hex = 0x09 + (asciiRec[i] & 0x0F);
		}

		// Following logic converts 2 bytes of ASCII to 1 byte of hex.
		k = i % 2;

		if (k)
		{
			hexRec[i/2] |= hex;
			
		}
		else
		{
			hexRec[i/2] = (hex << 4) & 0xF0;
		}
		i++;
	}
}

static void WriteHexRecord2Flash(UINT8* HexRecord)
{
	static T_HEX_RECORD HexRecordSt;
	UINT8 Checksum = 0;
	UINT i;
	UINT32 WrData;
	UINT32 ProgAddress;
	UINT Result;

	HexRecord = &HexRecord[0];
	HexRecordSt.RecDataLen = HexRecord[0];
	HexRecordSt.RecType = HexRecord[3];
	HexRecordSt.Data = &HexRecord[4];

	Checksum = 0;       // Hex Record checksum check.
	for (i = 0; i < HexRecordSt.RecDataLen + 5; i++)
	{
		Checksum += HexRecord[i];
	}

	if (Checksum != 0)
	{
		// Error. Hex record Checksum mismatch.
		error(ERR_HEX_CSUM);
	}
	else
	{
		// Hex record checksum OK.
		switch (HexRecordSt.RecType)
		{
			case DATA_RECORD:   // Record Type 00, data record.
				HexRecordSt.Address.byte.MB = 0;
				HexRecordSt.Address.byte.UB = 0;
				HexRecordSt.Address.byte.HB = HexRecord[1];
				HexRecordSt.Address.byte.LB = HexRecord[2];

				// Derive the address.
				HexRecordSt.Address.Val = HexRecordSt.Address.Val + HexRecordSt.ExtLinAddress.Val + HexRecordSt.ExtSegAddress.Val;

				while (HexRecordSt.RecDataLen) // Loop till all bytes are done.
				{
					// Convert the Physical address to Virtual address.
					ProgAddress = (HexRecordSt.Address.Val / 2);

					// Make sure we are not writing boot area and device configuration bits.
					if (((ProgAddress < AUX_FLASH_BASE_ADRS) ||
					     (ProgAddress > AUX_FLASH_END_ADRS)) &&
					    ((ProgAddress < DEV_CONFIG_REG_BASE_ADDRESS) ||
					     (ProgAddress > DEV_CONFIG_REG_END_ADDRESS)))
					{
						if (HexRecordSt.RecDataLen < 4)
						{
							// Sometimes record data length will not be in multiples of 4. Appending 0xFF will make sure that..
							// we don't write junk data in such cases.
							WrData = 0xFFFFFFFF;
							memcpy(&WrData, HexRecordSt.Data, HexRecordSt.RecDataLen);
						}
						else
						{
							memcpy(&WrData, HexRecordSt.Data, 4);
						}
						// Write the data into flash.
						Result = NVMemWriteWord(ProgAddress, WrData);
						// Assert on error. This must be caught during debug phase.
						while (Result!=0);
					}

					// Increment the address.
					HexRecordSt.Address.Val += 4;
					// Increment the data pointer.
					HexRecordSt.Data += 4;
					// Decrement data len.
					if (HexRecordSt.RecDataLen > 3)
					{
						HexRecordSt.RecDataLen -= 4;
					}
					else
					{
						HexRecordSt.RecDataLen = 0;
					}
				}
				break;

			case EXT_SEG_ADRS_RECORD:   // Record Type 02, defines 4th to 19th bits of the data address.
				HexRecordSt.ExtSegAddress.byte.MB = 0;
				HexRecordSt.ExtSegAddress.byte.UB = HexRecordSt.Data[0];
				HexRecordSt.ExtSegAddress.byte.HB = HexRecordSt.Data[1];
				HexRecordSt.ExtSegAddress.byte.LB = 0;
				// Reset linear address.
				HexRecordSt.ExtLinAddress.Val = 0;
				break;

			case EXT_LIN_ADRS_RECORD:   // Record Type 04, defines 16th to 31st bits of the data address. 
				HexRecordSt.ExtLinAddress.byte.MB = HexRecordSt.Data[0];
				HexRecordSt.ExtLinAddress.byte.UB = HexRecordSt.Data[1];
				HexRecordSt.ExtLinAddress.byte.HB = 0;
				HexRecordSt.ExtLinAddress.byte.LB = 0;
				// Reset segment address.
				HexRecordSt.ExtSegAddress.Val = 0;
				break;

			case END_OF_FILE_RECORD:    // Record Type 01, defines the end of file record.
			default:
				HexRecordSt.ExtSegAddress.Val = 0;
				HexRecordSt.ExtLinAddress.Val = 0;
				break;
		}
	}
}
