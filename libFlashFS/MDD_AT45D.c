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
#include "MDD File System/FSIO.h"
#include "MDD File System/FSDefs.h"
#include "FSconfig.h"
#include "MDD_AT45D.h"
#include "AT45D.h"

#include <stdint.h>
#include <stdio.h>


extern void Delayms(BYTE milliseconds);

static MEDIA_INFORMATION mediaInformation;

void AT45D_WipeFS(void)
{
#ifndef USE_DMA
	int i;

	printf("AT45D_WipeFS() ");
	PageToBuffer(50, 2);
	for (i = 0; i < 500; i++) {
		printf("%u ", i);
		BufferToPage(2, i);
}
	printf("done.\r\n");
#else
	printf("AT45D_WipeFS() - not yet supported.\r\n");
#endif
}

/******************************************************************************
 * Function:        BYTE MediaDetect(void)
 *
 * PreCondition:    InitIO() function has been executed.
 *
 * Output:          TRUE   - Card detected
 *                  FALSE   - No card detected
 *****************************************************************************/
BYTE MDD_AT45D_MediaDetect()
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
BYTE MDD_AT45D_InitIO (void)
{
    return  TRUE;
}

/******************************************************************************
 * Function:        BYTE MediaInitialize(void)
 *
 * Output:          Returns a pointer to a MEDIA_INFORMATION structure
 *
 * Overview:        MediaInitialize initializes the media card and supporting variables.
 *****************************************************************************/
MEDIA_INFORMATION * MDD_AT45D_MediaInitialize(void)
{
    mediaInformation.validityFlags.bits.sectorSize = TRUE;
//    mediaInformation.validityFlags.bits.maxLUN = TRUE;
    mediaInformation.sectorSize = MEDIA_SECTOR_SIZE;
//    mediaInformation.maxLUN = ; // BYTE
	mediaInformation.errorCode = MEDIA_NO_ERROR;
	return &mediaInformation;
}

int AT45D_WriteSector(unsigned int sector);
int AT45D_ReadSector(unsigned int sector);
void AT45D_GetBuffer(uint8_t* buffer);
void AT45D_PutBuffer(uint8_t* buffer);

/******************************************************************************
 * Function:        BYTE SectorRead(DWORD sector_addr, BYTE *buffer)
 *
 * Input:           sector_addr - Sector address, each sector contains 512-byte
 *                  buffer      - Buffer where data will be stored, see
 *                                'ram_acs.h' for 'block' definition.
 *                                'Block' is dependent on whether internal or
 *                                external memory is used
 *
 * Output:          Returns TRUE if read successful, false otherwise
 *
 * Overview:        SectorRead reads 512 bytes of data from the card starting
 *                  at the sector address specified by sector_addr and stores
 *                  them in the location pointed to by 'buffer'.
 *
 * Note:            The device expects the address field in the command packet
 *                  to be byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
//The flash memory is organized differently on the different microcontroller
//families.  Therefore, multiple versions of this function are implemented.
BYTE MDD_AT45D_SectorRead(DWORD sector_addr, BYTE* buffer)
{
//	printf("SectorRead %u\r\n", (unsigned int)sector_addr);

    //Error check.  Make sure the host is trying to read from a legitimate
    //address, which corresponds to the MSD volume (and not some other program
    //memory region beyond the end of the MSD volume).
	if (sector_addr >= MDD_AT45D_FLASH_TOTAL_DISK_SIZE)
	{
		return FALSE;
	}
#ifndef USE_DMA
//void PageToBuffer (uint16_t PageAdr, uint8_t BufferNo)
//void BufferReadStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
	PageToBuffer((uint16_t)sector_addr, 2);
	BufferReadStr(2, 0, 512, buffer);
#else
	AT45D_ReadSector((uint16_t)sector_addr);
	AT45D_GetBuffer(buffer);
#endif
    return TRUE;
}    

/******************************************************************************
 * Function:        BYTE SectorWrite(DWORD sector_addr, BYTE *buffer, BYTE allowWriteToZero)
 *
 * Input:           sector_addr - Sector address, each sector contains 512-byte
 *                  buffer      - Buffer where data will be read
 *                  allowWriteToZero - If true, writes to the MBR will be valid
 *
 * Output:          Returns TRUE if write successful, FALSE otherwise
 *
 * Overview:        SectorWrite sends 512 bytes of data from the location
 *                  pointed to by 'buffer' to the card starting
 *                  at the sector address specified by sector_addr.
 *
 * Note:            The sample device expects the address field in the command packet
 *                  to be byte address. Therefore the sector_addr must first
 *                  be converted to byte address. This is accomplished by
 *                  shifting the address left 9 times.
 *****************************************************************************/
BYTE MDD_AT45D_SectorWrite(DWORD sector_addr, BYTE* buffer, BYTE allowWriteToZero)
{
//	printf("SectorWrite %u\r\n", (unsigned int)sector_addr);

    //First, error check the resulting address, to make sure the MSD host isn't trying 
    //to erase/program illegal LBAs that are not part of the designated MSD volume space.
    if (sector_addr >= MDD_AT45D_FLASH_TOTAL_DISK_SIZE)
    {
        return FALSE;
    }
#ifndef USE_DMA
//void BufferWriteStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
//void BufferToPage (uint8_t BufferNo, uint16_t PageAdr)
	BufferWriteStr(1, 0, 512, buffer);
	BufferToPage(1, (uint16_t)sector_addr);
#else
	AT45D_PutBuffer(buffer);
	AT45D_WriteSector((uint16_t)sector_addr);
#endif
    return TRUE;
}    

/******************************************************************************
 * Function:        BYTE WriteProtectState(void)
 *
 * Output:          BYTE    - Returns the status of the "write enabled" pin
 *
 * Overview:        Determines if the card is write-protected
 *****************************************************************************/
BYTE MDD_AT45D_WriteProtectState(void)
{
	return FALSE;
}
