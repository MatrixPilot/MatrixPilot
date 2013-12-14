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
#include "USB/usb_function_msd.h"
#include "MDD_AT45D.h"


//The LUN variable definition is critical to the MSD function driver.  This
//  array is a structure of function pointers that are the functions that 
//  will take care of each of the physical media.  For each additional LUN
//  that is added to the system, an entry into this array needs to be added
//  so that the stack can know where to find the physical layer functions.
//  In this example the media initialization function is named 
//  "MediaInitialize", the read capacity function is named "ReadCapacity",
//  etc.

#if 0

LUN_FUNCTIONS LUN[MAX_LUN + 1] = 
{
	{
		&MDD_AT45D_MediaInitialize,
		&MDD_AT45D_ReadCapacity,
		&MDD_AT45D_ReadSectorSize,
		&MDD_AT45D_MediaDetect,
		&MDD_AT45D_SectorRead,
		&MDD_AT45D_WriteProtectState,
		&MDD_AT45D_SectorWrite
	}
};

#endif

// Standard Response to INQUIRY command stored in ROM
const ROM InquiryResponse inq_resp = {
	0x00,		// peripheral device is connected, direct access block device
	0x80,		// removable
	0x04,		// version = 00=> does not conform to any standard, 4=> SPC-2
	0x02,		// response is in format specified by SPC-2
	0x20,		// n-4 = 36-4=32= 0x20
	0x00,		// sccs etc.
	0x00,		// bque=1 and cmdque=0,indicates simple queueing 00 is obsolete,
				// but as in case of other device, we are just using 00
	0x00,		// 00 obsolete, 0x80 for basic task queueing
	{'M','i','c','r','o','c','h','p'
	},
	// this is the T10 assigned Vendor ID
	{'M','a','s','s',' ','S','t','o','r','a','g','e',' ',' ',' ',' '
	},
	{'0','0','0','1'
	}
};

