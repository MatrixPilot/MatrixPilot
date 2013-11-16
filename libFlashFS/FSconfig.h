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


#ifndef _FS_DEF_H_
#define _FS_DEF_H_

#include "../Microchip/HardwareProfile.h"

#ifdef USE_AT45D_FLASH
#include "MDD_AT45D.h"
#endif // USE_AT45D_FLASH


// Summary: A macro indicating the maximum number of concurrently open files
// Description: The FS_MAX_FILES_OPEN #define is only applicable when dynamic memory allocation is not used (FS_DYNAMIC_MEM is not defined).
//              This macro defines the maximum number of open files at any given time.  The amount of RAM used by FSFILE objects will
//              be equal to the size of an FSFILE object multipled by this macro value.  This value should be kept as small as possible
//              as dictated by the application.  This will reduce memory usage.
#define FS_MAX_FILES_OPEN       3


// Summary: A macro defining the size of a sector
// Description: The MEDIA_SECTOR_SIZE macro will define the size of a sector on the FAT file system.  This value must equal 512 bytes,
//              1024 bytes, 2048 bytes, or 4096 bytes.  The value of a sector will usually be 512 bytes.
#define MEDIA_SECTOR_SIZE       512



/* *******************************************************************************************************/
/************** Compiler options to enable/Disable Features based on user's application ******************/
/* *******************************************************************************************************/


// Summary: A macro to enable/disable file search functions.
// Description: The ALLOW_FILESEARCH definition can be commented out to disable file search functions in the library.  This will
//              prevent the use of the FindFirst and FindNext functions and reduce code size.
//#define ALLOW_FILESEARCH

// Summary: A macro to enable/disable write functionality
// Description: The ALLOW_WRITES definition can be commented out to disable all operations that write to the device.  This will
//              greatly reduce code size.
#define ALLOW_WRITES


// Summary: A macro to enable/disable format functionality
// Description: The ALLOW_FORMATS definition can be commented out to disable formatting functionality.  This will prevent the use of
//              the FSformat function.  If formats are enabled, write operations must also be enabled by uncommenting ALLOW_WRITES.
//#define ALLOW_FORMATS

// Summary: A macro to enable/disable directory operations.
// Description: The ALLOW_DIRS definition can be commented out to disable all directory functionality.  This will reduce code size.
//              If directories are enabled, write operations must also be enabled by uncommenting ALLOW_WRITES in order to use
//              the FSmkdir or FSrmdir functions.
//#define ALLOW_DIRS

// Summary: A macro to enable/disable the FSfprintf function.
// Description: The ALLOW_FSFPRINTF definition can be commented out to disable the FSfprintf function.  This will save code space.  Note that
//              if FSfprintf is enabled and the PIC18 architecture is used, integer promotions must be enabled in the Project->Build Options
//              menu.  Write operations must be enabled to use FSfprintf.
//#define ALLOW_FSFPRINTF

// Summary: A macro to enable/disable FAT32 support.
// Description: The SUPPORT_FAT32 definition can be commented out to disable support for FAT32 functionality.  This will save a small amount
//              of code space.
//#define SUPPORT_FAT32



/**************************************************************************************************/
// Select a method for updating file timestamps
/**************************************************************************************************/

// Summary: A macro to enable RTCC based timestamp generation
// Description: The USEREALTIMECLOCK macro will configure the code to automatically
//              generate timestamp information for files from the RTCC module. The user
//              must enable and configure the RTCC module before creating or modifying
//              files.                                                                 
//#define USEREALTIMECLOCK

// Summary: A macro to enable manual timestamp generation
// Description: The USERDEFINEDCLOCK macro will allow the user to manually set
//              timestamp information using the SetClockVars function. The user will
//              need to set the time variables immediately before creating or closing a
//              file or directory.                                                    
//#define USERDEFINEDCLOCK

// Summary: A macro to enable don't-care timestamp generation
// Description: The INCREMENTTIMESTAMP macro will set the create time of a file to a
//              static value and increment it when a file is updated. This timestamp
//              generation method should only be used in applications where file times
//              are not necessary.                                                    
#define INCREMENTTIMESTAMP

#ifndef USEREALTIMECLOCK
	#ifndef USERDEFINEDCLOCK
		#ifndef INCREMENTTIMESTAMP
			#error Please enable USEREALTIMECLOCK, USERDEFINEDCLOCK, or INCREMENTTIMESTAMP
		#endif
	#endif
#endif

/************************************************************************/
// Set this preprocessor option to '1' to use dynamic FSFILE object allocation.  It will
// be necessary to allocate a heap when dynamically allocating FSFILE objects.
// Set this option to '0' to use static FSFILE object allocation.
/************************************************************************/
#if 0
	// Summary: A macro indicating that FSFILE objects will be allocated dynamically
	// Description: The FS_DYNAMIC_MEM macro will cause FSFILE objects to be allocated from a dynamic heap.  If it is undefined,
	//              the file objects will be allocated using a static array.
	#define FS_DYNAMIC_MEM
	#ifdef USE_PIC18
		// Description: Function pointer to a dynamic memory allocation function
		#define FS_malloc   SRAMalloc
		// Description: Function pointer to a dynamic memory free function
		#define FS_free     SRAMfree
	#else
		#define FS_malloc   malloc
		#define FS_free     free
	#endif
#endif

//#define USE_AT45D_FLASH

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

#else

#error Must define a file system media interface

#endif

#endif // _FS_DEF_H_
