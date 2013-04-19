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

#include <stdint.h>
#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "FSconfig.h"
#include "MDD_AT45D.h"


//#if !defined(MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT)
//    #define MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT 64
//#endif

//Note: If only 1 FAT sector is used, assuming 12-bit (1.5 byte) FAT entry size 
//(ex: FAT12 filesystem), then the total FAT entries that can fit in a single 512 
//byte FAT sector is (512 bytes) / (1.5 bytes/entry) = 341 entries.  This allows 
//the FAT table to reference up to 341*512 = ~174kB of space.  Therfore, more 
//FAT sectors are needed if creating an MSD volume bigger than this.
#define MDD_AT45D_FLASH_NUM_RESERVED_SECTORS 1
#define MDD_AT45D_FLASH_NUM_VBR_SECTORS 1
#define MDD_AT45D_FLASH_NUM_FAT_SECTORS 32
#define MDD_AT45D_FLASH_NUM_ROOT_DIRECTORY_SECTORS ((MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT+15)/16) //+15 because the compiler truncates
/*
//#define MDD_AT45D_FLASH_OVERHEAD_SECTORS (\
//            MDD_AT45D_FLASH_NUM_RESERVED_SECTORS + \
//            MDD_AT45D_FLASH_NUM_VBR_SECTORS + \
//            MDD_AT45D_FLASH_NUM_ROOT_DIRECTORY_SECTORS + \
//            MDD_AT45D_FLASH_NUM_FAT_SECTORS)

//#define MDD_AT45D_FLASH_TOTAL_DISK_SIZE (\
//            MDD_AT45D_FLASH_OVERHEAD_SECTORS + \
//            MDD_AT45D_FLASH_DRIVE_CAPACITY)
 */
//#define MDD_AT45D_FLASH_PARTITION_SIZE (DWORD)(MDD_AT45D_FLASH_TOTAL_DISK_SIZE - 1)  //-1 is to exclude the sector used for the MBR 
#define MDD_AT45D_FLASH_PARTITION_SIZE (DWORD)(MDD_AT45D_FLASH_TOTAL_DISK_SIZE)

//------------------------------------------------------------------------------
//First FAT sector at LBA = 0
//------------------------------------------------------------------------------
ROM BYTE BootSector[] =
{
0xEB, 0x3C, 0x90,										// Jump instruction
'M', 'S', 'D', 'O', 'S', '5', '.', '0',					// OEM Name "MSDOS5.0"
(MEDIA_SECTOR_SIZE&0xFF), (MEDIA_SECTOR_SIZE>>8),		// Bytes per sector (MEDIA_SECTOR_SIZE)
0x01,													// Sectors per cluster
//0x02,													// Sectors per cluster
MDD_AT45D_FLASH_NUM_RESERVED_SECTORS, 0x00,				// Reserved sector count (usually 1 for FAT12 or FAT16)
0x01,													// Number of FATs 
MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT, 0x00,			// Max number of root directory entries
//0x00, 0x00,												// Total sectors (0x0000 means: use the 4 byte field at offset 0x20 instead)
(BYTE)MDD_AT45D_FLASH_PARTITION_SIZE, (BYTE)(MDD_AT45D_FLASH_PARTITION_SIZE >> 8),	// Total sectors (0x0000 means: use the 4 byte field at offset 0x20 instead)
0xF8,													// Media Descriptor
MDD_AT45D_FLASH_NUM_FAT_SECTORS, 0x00,					// Sectors per FAT
0x3F, 0x00,												// Sectors per track
0xFF, 0x00,												// Number of heads
//0x01, 0x00, 0x00, 0x00,									// Hidden sectors
0x00, 0x00, 0x00, 0x00,									// Hidden sectors
//(BYTE)MDD_AT45D_FLASH_PARTITION_SIZE, (BYTE)(MDD_AT45D_FLASH_PARTITION_SIZE >> 8), (BYTE)(MDD_AT45D_FLASH_PARTITION_SIZE >> 16), (BYTE)(MDD_AT45D_FLASH_PARTITION_SIZE >> 24), // Total sectors (when WORD value at offset 20 is 0x0000)
0x00, 0x00, 0x00, 0x00,									// Total sectors (when WORD value at offset 20 is 0x0000)
//0x00,													// Physical drive number
0x80,													// Physical drive number
0x00,													// Reserved("current head")
0x29,													// Signature
0x32, 0x67, 0x94, 0xC4,									// ID (serial number)
'N', 'O', ' ', 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' ',	// Volume Label (11 bytes) - "NO NAME    "
'F', 'A', 'T', '1', '2', ' ', ' ', ' ',					// FAT system "FAT12   "
// Operating system boot code
//
// ....
//
//0x55, 0xAA											// End of sector (0x55AA)
};

//------------------------------------------------------------------------------
//First FAT sector at LBA = 1
//------------------------------------------------------------------------------
//Please see:  http://technet.microsoft.com/en-us/library/cc938438.aspx
//For short summary on how this table works.
//Note: This table consists of a series of 12-bit entries, and are fully packed 
//(no pad bits).  This means every other byte is a "shared" byte, that is split
//down the middle and is part of two adjacent 12-bit entries.  
//The entries are in little endian format.

//ROM BYTE PARTITION_ATTRIBUTES(FAT0_ADDRESS) FAT0[MEDIA_SECTOR_SIZE] =
ROM BYTE FAT0[] =
{
    0xF8,0xFF,0xFF   // Copy of the media descriptor 0xFF8
};

//Optional additional FAT space here, only needed for drives > ~174kB.
//#if(MDD_AT45D_FLASH_NUM_FAT_SECTORS > 1)
//ROM BYTE PARTITION_ATTRIBUTES(FATx_ADDRESS) FATx[MEDIA_SECTOR_SIZE*(MDD_AT45D_FLASH_NUM_FAT_SECTORS - 1)];
//#endif

ROM BYTE RootDirectory0[] =
{
    //Root
//    'D','r','i','v','e',' ','N','a','m','e',' ',// Drive Name (11 characters, padded with spaces)
    'A','U','A','V','3',' ','D','a','t','a',' ',// Drive Name (11 characters, padded with spaces)
    0x08,										// Specify this entry as a volume label
    0x00,								// Reserved
    0x00, 0x00, 0x00, 0x00, 0x11,		// Create time
    0x00, 0x11,							// Last Access
    0x00, 0x00,							// EA-index
    0x00, 0x00, 0x00, 0x11,				// Last modified time
    0x00, 0x00,							// First FAT cluster
    0x00, 0x00, 0x00, 0x00,				// File Size (number of bytes)
//    'F','I','L','E',' ',' ',' ',' ',    //File name (exactly 8 characters)
//    'T','X','T',                        //File extension (exactly 3 characters)
//    0x20, //specify this entry as a volume label
//    0x00, //Reserved
//    0x06, 0x28, 0x78, 0xDE, 0x38, //Create time
//    0xDE, 0x38,             //Last Access
//    0x00, 0x00,             //EA-index
//    0x04, 0x77, 0xDE, 0x38, //Last modified
//    0x02, 0x00,             //First FAT cluster
//    0x04, 0x00, 0x00, 0x00, //File Size (number of bytes)
};

/*
void FormatFS(void)
{
	printf("FormatFS()\r\n");
	{
//		printf("\tcopying master boot record\r\n");
//		BufferWriteStr(2, 0, 512, &MasterBootRecord[0]);
//		BufferToPage(2, 0);
	}
	{
		printf("\tcopying boot sector\r\n");
		BufferWriteStr(2, 0, 512, &BootSector[0]);
		BufferToPage(2, 1);
	}
	{
		printf("\tcopying FAT0\r\n");
		BufferWriteStr(2, 0, 512, &FAT0[0]);
		BufferToPage(2, 2);
	}
	{
		printf("\tcopying root directory\r\n");
		BufferWriteStr(2, 0, 512, &RootDirectory0[0]);
		BufferToPage(2, 3);
	}
	printf("FormatFS() complete\r\n");
}
 */
int AT45D_WriteSector(unsigned int sector);
int AT45D_ReadSector(unsigned int sector);
void AT45D_GetBuffer(uint8_t* buffer);
void AT45D_PutBuffer(uint8_t* buffer);


//void * memcpy ( void * destination, const void * source, size_t num );
//void * memset ( void * ptr, int value, size_t num );

void AT45D_FormatFS(void)
{
unsigned char buf[520];
	int i;

	printf("AT45D_FormatFS()\r\n");

	printf("\tcopying boot sector\r\n");
	memcpy(buf, BootSector, sizeof(BootSector));
	memset(buf + sizeof(BootSector), '\0', 512 - sizeof(BootSector));
	buf[510] = 0x55;
	buf[511] = 0xAA;
	AT45D_PutBuffer(buf);
	AT45D_WriteSector((uint16_t)0);

	printf("\tcopying FAT0\r\n");
	memcpy(buf, FAT0, sizeof(FAT0));
	memset(buf + sizeof(FAT0), '\0', 512 - sizeof(FAT0));
	AT45D_PutBuffer(buf);
	AT45D_WriteSector((uint16_t)1);
	memset(buf, '\0', 512);
	for (i = 2; i < (MDD_AT45D_FLASH_NUM_FAT_SECTORS + 1); i++) {
		AT45D_PutBuffer(buf);
		AT45D_WriteSector((uint16_t)i);
	}

	printf("\tcopying root directory\r\n");
	memcpy(buf, RootDirectory0, sizeof(RootDirectory0));
	memset(buf + sizeof(RootDirectory0), '\0', 512 - sizeof(RootDirectory0));
	AT45D_PutBuffer(buf);
	AT45D_WriteSector((uint16_t)(MDD_AT45D_FLASH_NUM_FAT_SECTORS+1));
	memset(buf, '\0', 512);
	for (i = (MDD_AT45D_FLASH_NUM_FAT_SECTORS+1+1); i < ((MDD_AT45D_FLASH_NUM_FAT_SECTORS+1) + MDD_AT45D_FLASH_NUM_ROOT_DIRECTORY_SECTORS); i++) {
		AT45D_PutBuffer(buf);
		AT45D_WriteSector((uint16_t)i);
	}

	printf("AT45D_FormatFS() complete\r\n");
}
