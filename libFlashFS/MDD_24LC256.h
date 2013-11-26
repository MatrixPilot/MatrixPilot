// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2013 MatrixPilot Team
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


#ifndef _MDD_AT45D_H_
#define _MDD_AT45D_H_


#include "../Microchip/Include/GenericTypeDefs.h"
#include "FSconfig.h"
#include "../Microchip/Include/MDD File System/FSDefs.h"

#define FALSE   0
#define TRUE    !FALSE

#define MDD_24LC256_TOTAL_DISK_SIZE MDD_24LC256_DRIVE_CAPACITY

#define INITIALIZATION_VALUE            0x55

BYTE MDD_24LC256_InitIO(void);
BYTE MDD_24LC256_MediaDetect(void);
MEDIA_INFORMATION * MDD_24LC256_MediaInitialize(void);
BYTE MDD_24LC256_SectorRead(DWORD sector_addr, BYTE* buffer);
BYTE MDD_24LC256_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero);
WORD MDD_24LC256_ReadSectorSize(void);
DWORD MDD_24LC256_ReadCapacity(void);
BYTE MDD_24LC256_WriteProtectState(void);


#endif // _MDD_AT45D_H_
