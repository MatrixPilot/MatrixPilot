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


#ifndef _FSCONFIG_H_
#define _FSCONFIG_H_

#include "HardwareProfile.h"

#ifdef USE_AT45D_FLASH
//#warning USE_AT45D_FLASH
#include "../libFlashFS/MDD_AT45D.h"
#elif defined USE_EEPROM_FLASH
//#warning USE_EEPROM_FLASH
#define USE_I2C1_DRIVER                 1
#include "../libFlashFS/MDD_EEPROM.h"
#elif defined(USE_SD_INTERFACE_WITH_SPI)
//#warning USE_SD_INTERFACE_WITH_SPI
#include "MDD-File-System/SD-SPI.h"
#else
#error Must define a file system media interface
#endif // USE_AT45D_FLASH


// The FS_MAX_FILES_OPEN #define is only applicable when Dynamic
// memeory allocation is not used (FS_DYNAMIC_MEM not defined).
// Defines how many concurent open files can exist at the same time.
// Takes up static memory. If you do not need to open more than one
// file at the same time, then you should set this to 1 to reduce
// memory usage
#define FS_MAX_FILES_OPEN       3

// The size of a sector
// Must be 512, 1024, 2048, or 4096
// 512 bytes is the value used by most cards
#define MEDIA_SECTOR_SIZE       512


// Uncomment this to use the FindFirst, FindNext, and FindPrev
#define ALLOW_FILESEARCH

// Comment this line out if you don't intend to write data to the card
#define ALLOW_WRITES

// Comment this line out if you don't intend to format your card
// Writes must be enabled to use the format function
//#define ALLOW_FORMATS

// Uncomment this definition if you're using directories
// Writes must be enabled to use directories
//#define ALLOW_DIRS

// Allows the use of the FSfprintf function
// Writes must be enabled to use the FSprintf function
//#define ALLOW_FSFPRINTF

// If FAT32 support required then uncomment the following
//#define SUPPORT_FAT32


// Select how you want the timestamps to be updated
// Use the Real-time clock peripheral to set the clock
// You must configure the RTC in your application code
//#define USEREALTIMECLOCK

// The user will update the timing variables manually using the SetClockVars function
// The user should set the clock before they create a file or directory (Create time),
// and before they close a file (last access time, last modified time)
//#define USERDEFINEDCLOCK

// Just increment the time- this will not produce accurate times and dates
#define INCREMENTTIMESTAMP

#ifndef USEREALTIMECLOCK
	#ifndef USERDEFINEDCLOCK
		#ifndef INCREMENTTIMESTAMP
			#error Please enable USEREALTIMECLOCK, USERDEFINEDCLOCK, or INCREMENTTIMESTAMP
		#endif
	#endif
#endif

/************************************************************************/
// Define FS_DYNAMIC_MEM to use malloc for allocating
// FILE structure space.  uncomment all three lines
/************************************************************************/
#if 0
	#define FS_DYNAMIC_MEM
	#ifdef USE_PIC18
		#define FS_malloc   SRAMalloc
		#define FS_free     SRAMfree
	#else
		#define FS_malloc   malloc
		#define FS_free     free
	#endif
#endif

// Function definitions
// Associate the physical layer functions with the correct physical layer

#ifdef USE_SD_INTERFACE_WITH_SPI       // SD-SPI.c and .h

// Description: Function pointer to the Media Initialize Physical Layer function
#define MDD_MediaInitialize     MDD_SDSPI_MediaInitialize

// Description: Function pointer to the Media Detect Physical Layer function
#define MDD_MediaDetect         MDD_SDSPI_MediaDetect

// Description: Function pointer to the Sector Read Physical Layer function
#define MDD_SectorRead          MDD_SDSPI_SectorRead

// Description: Function pointer to the Sector Write Physical Layer function
#define MDD_SectorWrite         MDD_SDSPI_SectorWrite

// Description: Function pointer to the I/O Initialization Physical Layer function
#define MDD_InitIO              MDD_SDSPI_InitIO

// Description: Function pointer to the Media Shutdown Physical Layer function
#define MDD_ShutdownMedia       MDD_SDSPI_ShutdownMedia

// Description: Function pointer to the Write Protect Check Physical Layer function
#define MDD_WriteProtectState   MDD_SDSPI_WriteProtectState

// Description: Function pointer to the Read Capacity Physical Layer function
#define MDD_ReadCapacity        MDD_SDSPI_ReadCapacity

// Description: Function pointer to the Read Sector Size Physical Layer Function
#define MDD_ReadSectorSize      MDD_SDSPI_ReadSectorSize

#elif defined USE_AT45D_FLASH

#define MDD_MediaInitialize     MDD_AT45D_MediaInitialize
#define MDD_MediaDetect         MDD_AT45D_MediaDetect
#define MDD_SectorRead          MDD_AT45D_SectorRead
#define MDD_SectorWrite         MDD_AT45D_SectorWrite
#define MDD_InitIO              MDD_AT45D_InitIO
#define MDD_ShutdownMedia       MDD_AT45D_ShutdownMedia
#define MDD_WriteProtectState   MDD_AT45D_WriteProtectState
#define MDD_ReadSectorSize      MDD_AT45D_ReadSectorSize
#define MDD_ReadCapacity        MDD_AT45D_ReadCapacity

// ---------------------------------------------------------------------------------------
// The size (in number of sectors) of the desired usable data portion of the MSD volume
// ---------------------------------------------------------------------------------------
// Note1: Windows 7 appears to require a minimum capacity of at least 13 sectors.
// Note2: Windows will not be able to format a drive if it is too small.  The reason
//  for this, is that Windows will try to put a "heavyweight" (comparatively) filesystem
//  on the drive, which will consume ~18kB of overhead for the filesystem.  If the total
//  drive size is too small to fit the filesystem, then Windows will give an error.	
//  This also means that formatting the drive will "shrink" the usuable data storage
//  area, since the default FAT12 filesystem implemented in the Files.c data tables is very
//  lightweight, with very low overhead.
// Note3: It is important to make sure that no part of the MSD volume shares a flash
//  erase page with the firmware program memory.  This can be done by using a custom
//  modified linker script, or by carefully selecting the starting address and the 
//  total size of the MSD volume.  See also below code comments.
// Note4: It is also important to make sure that no part of the MSD volume shares
//  an erase page with the erase page that contains the microcontroller's configuration
//  bits (for microcontrollers that use flash for storing the configuration bits, 
//  see device datasheet). This can be accomplished by using a modified linker script,
//  which protects the flash page with the configuration bits (if applicable), or,
//  by carefully choosing the FILES_ADDRESS and MDD_AT45D_FLASH_DRIVE_CAPACITY,
//  to make sure the MSD volume does extend into the erase page with the configuration
//  bits.

//#define MDD_AT45D_FLASH_DRIVE_CAPACITY 14
//#define MDD_AT45D_FLASH_DRIVE_CAPACITY 42
#define MDD_AT45D_FLASH_DRIVE_CAPACITY 8192


// --------------------------------------------------------------------------
// Starting Address of the MSD Volume.
// --------------------------------------------------------------------------
// Note: Make certain that this starting address is aligned with the start
//  of a flash erase block.  It is important to make certain that no part of
//  the MSD volume overlaps any portion of a flash erase page which is used
//  for storing firmware program code.  When the host writes a sector to the 
//  MSD volume, the firmware must erase an entire page of flash in order to
//  do the write.  If the sector being written happened to share a flash erase
//  page with this firmware, unpredictable results would occur, since part of
//  firmware would also end up getting erased during the write to the MSD volume.

#define FILES_ADDRESS 0x2000


// --------------------------------------------------------------------------
// Maximum files supported
// --------------------------------------------------------------------------
// MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT must be a multiple of 16
// Note: Even if MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT is 16, this does not
//  necessarily mean the drive will always work with 16 files.  The drive will
//  suppport "up to" 16 files, but other limits could be hit first, even before
//  the drive is full.  The RootDirectory0[] sector could get full with less
//  files, especially if the files are using long filenames.  

//#define MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT 16
#define MDD_AT45D_FLASH_MAX_NUM_FILES_IN_ROOT 64

//#define AT45D_FLASH_WRITE_PROTECT

#elif defined USE_EEPROM_FLASH

#define MDD_MediaInitialize     MDD_EEPROM_MediaInitialize
#define MDD_MediaDetect         MDD_EEPROM_MediaDetect
#define MDD_SectorRead          MDD_EEPROM_SectorRead
#define MDD_SectorWrite         MDD_EEPROM_SectorWrite
#define MDD_InitIO              MDD_EEPROM_InitIO
#define MDD_ShutdownMedia       MDD_EEPROM_ShutdownMedia
#define MDD_WriteProtectState   MDD_EEPROM_WriteProtectState
#define MDD_ReadSectorSize      MDD_EEPROM_ReadSectorSize
#define MDD_ReadCapacity        MDD_EEPROM_ReadCapacity

//#define MDD_EEPROM_FLASH_DRIVE_CAPACITY 14
#define MDD_EEPROM_FLASH_MAX_NUM_FILES_IN_ROOT 16

#else

#error Must define a file system media interface

#endif

#endif // _FSCONFIG_H_
