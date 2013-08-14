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


#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "USB/usb.h"
#include "USB/usb_device.h"
#include "bootloader.h"
#include "mcu.h"

BOOL ValidAppPresent(void)
{
	volatile DWORD AppPtr;

	TBLPAG = 0x00;

	AppPtr = ((DWORD)__builtin_tblrdh(USER_APP_RESET_ADDRESS) << 16);
	AppPtr = AppPtr | ((DWORD)__builtin_tblrdl(USER_APP_RESET_ADDRESS));
	if (AppPtr == 0xFFFFFF)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int main(void)
{
	mcu_init();

	USBDeviceInit();    //usb_device.c.  Initializes USB module SFRs and firmware variables to known states.
#if defined(USB_INTERRUPT)
	USBDeviceAttach();
#endif

	// Enter firmware upgrade mode if there is a trigger or if the application is not valid
	if ((ReadSwitchStatus() == SWITCH_PRESSED) || !ValidAppPresent())
	{
		bootloader();
	}

	((void (*)(void))USER_APP_RESET_ADDRESS)();

	return 0;
}
