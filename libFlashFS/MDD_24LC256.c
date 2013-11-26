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
#include "../Microchip/Include/MDD File System/FSIO.h"
#include "../Microchip/Include/MDD File System/FSDefs.h"
#include "FSconfig.h"
#include "MDD_AT45D.h"
#include "AT45D.h"
#include <stdint.h>
#include <stdio.h>


extern void Delayms(BYTE milliseconds);

static MEDIA_INFORMATION mediaInformation;


/******************************************************************************
 * Function:        BYTE MediaDetect(void)
 *
 * PreCondition:    InitIO() function has been executed.
 *
 * Output:          TRUE   - Card detected
 *                  FALSE   - No card detected
 *****************************************************************************/
BYTE MDD_AT45D_MediaDetect(void)
{
	return TRUE;
}

/******************************************************************************
 * Function:        WORD ReadSectorSize(void)
 *
 * PreCondition:    MediaInitialize() is complete
 *
 * Output:          WORD - size of the sectors for this physical media.
 *****************************************************************************/
WORD MDD_AT45D_ReadSectorSize(void)
{
	return MEDIA_SECTOR_SIZE;
}

/******************************************************************************
 * Function:        DWORD ReadCapacity(void)
 *
 * PreCondition:    MediaInitialize() is complete
 *
 * Output:          DWORD - size of the "disk" - 1 (in terms of sector count).  
 *                  Ex: In other words, this function returns the last valid 
 *                  LBA address that may be read/written to.
 *****************************************************************************/
DWORD MDD_AT45D_ReadCapacity(void)
{
	//The SCSI READ_CAPACITY command wants to know the last valid LBA address 
	//that the host is allowed to read or write to.  Since LBA addresses start
	//at and include 0, a return value of 0 from this function would mean the 
	//host is allowed to read and write the LBA == 0x00000000, which would be 
	//1 sector worth of capacity.
	//Therefore, the last valid LBA that the host may access is 
	//MDD_AT45D_FLASH_TOTAL_DISK_SIZE - 1.
	return (MDD_AT45D_FLASH_TOTAL_DISK_SIZE - 1); 
}

/******************************************************************************
 * Function:        BYTE InitIO(void)
 *
 * Output:          TRUE  - Card initialized
 *                  FALSE - Card not initialized
 *****************************************************************************/
BYTE MDD_AT45D_InitIO(void)
{
	return TRUE;
}

/******************************************************************************
 * Function:    BYTE MediaInitialize(void)
 *
 * Output:      Returns a pointer to a MEDIA_INFORMATION structure
 *
 * Overview:    MediaInitialize initializes the media card and supporting variables.
 *****************************************************************************/
MEDIA_INFORMATION * MDD_AT45D_MediaInitialize(void)
{
	mediaInformation.validityFlags.bits.sectorSize = TRUE;
//	mediaInformation.validityFlags.bits.maxLUN = TRUE;
	mediaInformation.sectorSize = MEDIA_SECTOR_SIZE;
//	mediaInformation.maxLUN =; // BYTE
	mediaInformation.errorCode = MEDIA_NO_ERROR;
	return &mediaInformation;
}

/******************************************************************************
 * Function:    BYTE SectorRead(DWORD sector_addr, BYTE *buffer)
 *
 * Input:       sector_addr - Sector address, each sector contains 512-byte
 *              buffer      - Buffer where data will be stored
 *
 * Output:      Returns TRUE if read successful, false otherwise
 *
 * Overview:    SectorRead reads 512 bytes of data from the card starting
 *              at the sector address specified by sector_addr and stores
 *              them in the location pointed to by 'buffer'.
 *
 * Note:        The device expects the address field in the command packet
 *              to be byte address. Therefore the sector_addr must first
 *              be converted to byte address. This is accomplished by
 *              shifting the address left 9 times.
 *****************************************************************************/
BYTE MDD_AT45D_SectorRead(DWORD sector_addr, BYTE* buffer)
{
//	printf("MDD_AT45D_SectorRead %u\r\n", (unsigned int)sector_addr);

	if (sector_addr >= MDD_AT45D_FLASH_TOTAL_DISK_SIZE)
	{
		return FALSE;
	}
	ReadSector((uint16_t)sector_addr, buffer);
	return TRUE;
}

/******************************************************************************
 * Function:    BYTE SectorWrite(DWORD sector_addr, BYTE *buffer, BYTE allowWriteToZero)
 *
 * Input:       sector_addr - Sector address, each sector contains 512-byte
 *              buffer - Buffer where data will be read
 *              allowWriteToZero - If true, writes to the MBR will be valid
 *
 * Output:      Returns TRUE if write successful, FALSE otherwise
 *
 * Overview:    SectorWrite sends 512 bytes of data from the location
 *              pointed to by 'buffer' to the card starting
 *              at the sector address specified by sector_addr.
 *
 * Note:        The sample device expects the address field in the command packet
 *              to be byte address. Therefore the sector_addr must first
 *              be converted to byte address. This is accomplished by
 *              shifting the address left 9 times.
 *****************************************************************************/
BYTE MDD_AT45D_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero)
{
//	printf("MDD_AT45D_SectorWrite %u\r\n", (unsigned int)sector_addr);

	if (sector_addr >= MDD_AT45D_FLASH_TOTAL_DISK_SIZE)
	{
		return FALSE;
	}
	WriteSector((uint16_t)sector_addr, buffer);
	return TRUE;
}

/******************************************************************************
 * Function:    BYTE WriteProtectState(void)
 *
 * Output:      BYTE - Returns the status of the "write enabled" pin
 *
 * Overview:    Determines if the card is write-protected
 *****************************************************************************/
BYTE MDD_AT45D_WriteProtectState(void)
{
	return FALSE;
}
