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


#include "../libUDB/libUDB.h" // for DPRINT
#include "filesys.h"
#ifndef WIN32
#include "MDD-File-System/FSIO.h"
#include "AT45D.h"
#include "EEPROM.h"


void filesys_chkdsk(void)
{
	printf("filesys_chkdsk() - TODO\r\n");
}

/*
// Summary: A structure used for searching for files on a device.
// Description: The SearchRec structure is used when searching for file on a device.  It contains parameters that will be loaded with
//              file information when a file is found.  It also contains the parameters that the user searched for, allowing further
//              searches to be perfomed in the same directory for additional files that meet the specified criteria.
typedef struct
{
	char            filename[FILE_NAME_SIZE_8P3 + 2];   // The name of the file that has been found
	unsigned char   attributes;                     // The attributes of the file that has been found
	unsigned long   filesize;                       // The size of the file that has been found
	unsigned long   timestamp;                      // The last modified time of the file that has been found (create time for directories)
	#ifdef SUPPORT_LFN
		BOOL               AsciiEncodingType;       // Ascii file name or Non-Ascii file name indicator
		unsigned short int *utf16LFNfound;          // Pointer to long file name found in UTF16 format
		unsigned short int utf16LFNfoundLength;     // LFN Found length in terms of words including the NULL word at the last.
	#endif
	unsigned int    entry;                          // The directory entry of the last file found that matches the specified attributes. (Internal use only)
	char            searchname[FILE_NAME_SIZE_8P3 + 2]; // The 8.3 format name specified when the user began the search. (Internal use only)
	unsigned char   searchattr;                     // The attributes specified when the user began the search. (Internal use only)
	unsigned long   cwdclus;                        // The directory that this search was performed in. (Internal use only)
	unsigned char   initialized;                    // Check to determine if the structure was initialized by FindFirst (Internal use only)
} SearchRec;
 */
void filesys_dir(char* arg)
{
	SearchRec rec;

	if (arg == NULL) {
		arg = "*.*";
	}
	DPRINT("filesys_dir %s\r\n", arg);
	if (FindFirst(arg, ATTR_MASK, &rec) != -1) {
		do {
			printf("%s\t%lu\r\n", rec.filename, rec.filesize);
		} while (FindNext(&rec) != -1);
	}
}

void filesys_cat(char* arg)
{
	char buf[2];
	FSFILE* fp;

	DPRINT("filesys_cat(%s)\r\n", arg);

	fp = FSfopen(arg, "r");
	if (fp != NULL) {
		while (FSfread(buf, 1, sizeof(char), fp) == 1) {
			printf("%c", buf[0]);
		}
		FSfclose(fp);
	} else {
		DPRINT("failed to open %s\r\n", arg);
	}
}

void filesys_format(void)
{
	printf("filesys_format\r\n");
#ifdef USE_AT45D_FLASH
	AT45D_FormatFS();
#elif defined USE_EEPROM_FLASH
	EEPROM_FormatFS();
#elif (WIN == 1) || (NIX == 1)
#else
#warning No Mass Storage Device Format Function Defined
#endif // USE_AT45D_FLASH
}

int filesys_init(void)
{
#if (USE_FILESYS == 1)
	DPRINT("filesys_init()\r\n");
//	init_dataflash(); // this should now be getting device specific called from lower layers via FSInit()

	MDD_InitIO();
	if (!MDD_MediaDetect())
	{
		printf("no media detected\r\n");
		return 0;
	}

	if (!FSInit())
	{
		printf("fs_init() return zero\r\n");
		filesys_format();
		if (!FSInit())
		{
			printf("File system initialisation failed\r\n");
			return 0;
		}
	}
	printf("File system initalised\r\n");
#endif // USE_FILESYS
	return 1;
}

#else // WIN32

void filesys_chkdsk(void)
{
	printf("filesys_chkdsk() - WIN32 TODO\r\n");
}

void filesys_dir(char* arg)
{
	if (arg == NULL) {
		arg = "*.*";
	}
	printf("filesys_dir %s - WIN32 TODO\r\n", arg);
}

//size_t FSfread(void *ptr, size_t size, size_t n, FSFILE *stream);
void filesys_cat(char* arg)
{
	printf("filesys_cat(%s) - WIN32 TODO\r\n", arg);
}

void filesys_format(void)
{
}

int filesys_init(void)
{
	printf("filesys_init() - nothing to do on WIN32\r\n");
	return 1;
}

#endif // WIN32

