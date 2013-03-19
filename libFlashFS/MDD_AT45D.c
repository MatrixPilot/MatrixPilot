#include <stdint.h>
#include <stdio.h>

#include "FSIO.h"
#include "FSDefs.h"
#include "FSconfig.h"
#include "MDD_AT45D.h"
#include "AT45D.h"

static MEDIA_INFORMATION mediaInformation;

extern void Delayms(BYTE milliseconds);

void AT45D_WipeFS(void)
{
	printf("AT45D_WipeFS()\r\n");

	PageToBuffer(50, 2);
	BufferToPage(2, 0);
	BufferToPage(2, 1);
	BufferToPage(2, 2);
	BufferToPage(2, 3);
	BufferToPage(2, 4);
	BufferToPage(2, 5);
	BufferToPage(2, 6);
	BufferToPage(2, 7);
}

void AT45D_DisplaySettings(void)
{
	printf("MDD_AT45D_FLASH_NUM_RESERVED_SECTORS %u\r\n", MDD_AT45D_FLASH_NUM_RESERVED_SECTORS);
	printf("MDD_AT45D_FLASH_NUM_VBR_SECTORS %u\r\n", MDD_AT45D_FLASH_NUM_VBR_SECTORS);
	printf("MDD_AT45D_FLASH_NUM_FAT_SECTORS %u\r\n", MDD_AT45D_FLASH_NUM_FAT_SECTORS);
	printf("MDD_AT45D_FLASH_NUM_ROOT_DIRECTORY_SECTORS %u\r\n", MDD_AT45D_FLASH_NUM_ROOT_DIRECTORY_SECTORS);
	printf("MDD_AT45D_FLASH_OVERHEAD_SECTORS %u\r\n", MDD_AT45D_FLASH_OVERHEAD_SECTORS);
	printf("MDD_AT45D_FLASH_TOTAL_DISK_SIZE %u\r\n", MDD_AT45D_FLASH_TOTAL_DISK_SIZE);
	printf("MDD_AT45D_FLASH_PARTITION_SIZE %u\r\n", (int)MDD_AT45D_FLASH_PARTITION_SIZE);
	printf("\r\n");
	printf("MDD_AT45D_FLASH_DRIVE_CAPACITY %u\r\n", MDD_AT45D_FLASH_DRIVE_CAPACITY);
	printf("MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT %u\r\n", MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT);
	printf("FILES_ADDRESS %u\r\n", FILES_ADDRESS);
	printf("\r\n");
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
//printf("MDD_AT45D_ReadCapacity %u\r\n", MDD_AT45D_FLASH_TOTAL_DISK_SIZE);
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
//	printf("MDD_AT45D_SectorRead %u\r\n", (unsigned int)sector_addr);

    //Error check.  Make sure the host is trying to read from a legitimate
    //address, which corresponds to the MSD volume (and not some other program
    //memory region beyond the end of the MSD volume).
	if (sector_addr >= MDD_AT45D_FLASH_TOTAL_DISK_SIZE)
	{
		return FALSE;
	}
//void PageToBuffer (uint16_t PageAdr, uint8_t BufferNo)
	PageToBuffer((uint16_t)sector_addr, 2);
//void BufferReadStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
	BufferReadStr(2, 0, 512, buffer);
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
//	printf("MDD_AT45D_SectorWrite %u\r\n", (unsigned int)sector_addr);

    //First, error check the resulting address, to make sure the MSD host isn't trying 
    //to erase/program illegal LBAs that are not part of the designated MSD volume space.
    if (sector_addr >= MDD_AT45D_FLASH_TOTAL_DISK_SIZE)
    {
        return FALSE;
    }
//void BufferWriteStr (uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
	BufferWriteStr(1, 0, 512, buffer);
//void BufferToPage (uint8_t BufferNo, uint16_t PageAdr)
	BufferToPage(1, (uint16_t)sector_addr);
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
