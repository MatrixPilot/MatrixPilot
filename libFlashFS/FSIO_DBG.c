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
#include "AT45D.h"
#include "FSIO_DBG.h"
#include <string.h>
#include <stdio.h>


void AT45D_FormatFS(void);

static char logfile_name[13];
//static FSFILE * fp_log;

extern DISK gDiskData;         // Global structure containing device information.


void DisplayFS(void)
{
	printf("firsts %u\r\n", (unsigned int)gDiskData.firsts);
	printf("fat %u\r\n", (unsigned int)gDiskData.fat );
	printf("root %u\r\n", (unsigned int)gDiskData.root );
	printf("data %u\r\n", (unsigned int)gDiskData.data );
	printf("maxroot %u\r\n", (unsigned int)gDiskData.maxroot );
	printf("maxcls %u\r\n", (unsigned int)gDiskData.maxcls );
	printf("sectorSize %u\r\n", (unsigned int)gDiskData.sectorSize );
	printf("fatsize %u\r\n", (unsigned int)gDiskData.fatsize );
	printf("fatcopy %u\r\n", (unsigned int)gDiskData.fatcopy );
	printf("SecPerClus %u\r\n", (unsigned int)gDiskData.SecPerClus );
	printf("type %u\r\n", (unsigned int)gDiskData.type );
	printf("mount %u\r\n", (unsigned int)gDiskData.mount );
/*
    BYTE    *   buffer;         // Address of the global data buffer used to read and write file information
    DWORD       firsts;         // Logical block address of the first sector of the FAT partition on the device
    DWORD       fat;            // Logical block address of the FAT
    DWORD       root;           // Logical block address of the root directory
    DWORD       data;           // Logical block address of the data section of the device.
    WORD        maxroot;        // The maximum number of entries in the root directory.
    DWORD       maxcls;         // The maximum number of clusters in the partition.
    DWORD       sectorSize;     // The size of a sector in bytes
    DWORD       fatsize;        // The number of sectors in the FAT
    BYTE        fatcopy;        // The number of copies of the FAT in the partition
    BYTE        SecPerClus;     // The number of sectors per cluster in the data region
    BYTE        type;           // The file system type of the partition (FAT12, FAT16 or FAT32)
    BYTE        mount;          // Device mount flag (TRUE if disk was mounted successfully, FALSE otherwise)
 */
}

static int fs_nextlog(char* filename)
{
	FSFILE* fp;
	int i;

	for (i = 0; i < 99; i++) {
		sprintf(filename, "log%02u.txt", i);
		fp = FSfopen(filename, "r");
		if (fp != NULL) {
			FSfclose(fp);
		} else {
			return 1;
		}
	}
	return 0;
}

void log_init(void)
{
	init_dataflash();

	if (!FSInit()) {
		AT45D_FormatFS();
		if (!FSInit()) {
			printf("File system initialisation failed\r\n");
			return;
		}
	}
	printf("File system initalised\r\n");

	if (!fs_nextlog(logfile_name)) {
		strcpy(logfile_name, "fp_log.txt");
	}
	printf("Logging to file %s\r\n", logfile_name);
}

void fs_telelog(char* str, int len)
{
	FSFILE* fsp;
//	static FSFILE* fsp;
//	int len = strlen(str);

//	if (!fsp) {
//		fsp = FSfopen(logfile_name, "a");
//	}
	fsp = FSfopen(logfile_name, "a");
	if (fsp) {
//    unsigned char str_put_n_chars (FSFILE * handle, unsigned char n, char c);
		if (FSfwrite(str, 1, len, fsp) != len) {
			printf("ERROR: fs_telelog() - FSfwrite\r\n");
		}
		FSfclose(fsp);
		fsp = NULL;
	}
	else
	{
		printf("ERROR: fs_telelog() - FSfopen\r\n");
	}
}

///////////////////////////////////////////////////////////////////////////////
//		fs_openconfig("config.txt");
//		openconfig("config.txt");

//		fs_openlog("fp_log.txt");
//		fs_log("this is a test string\r\n");
//		fs_closelog();
/*
int fs_log(char* str)
{
	int len = strlen(str);

	if (fp_log != NULL) {
		if (FSfwrite(str, 1, strlen(str), fp_log) != len) {
			printf("ERROR: fswrite failed\n");
			return -1;
		}
	}
	return 0;
}

int fs_openlog(char* filename)
{
//	fp_log = FSfopen(filename, "w");
	fp_log = FSfopen(filename, "a");
	if (fp_log != NULL) {
//		printf("%s opened\n", filename);
	} else {
		printf("ERROR: FSfopen(%s) failed\r\n", filename);
		return -1;
	}
	return 0;
}

int fs_closelog(void)
{
	FSfclose(fp_log);
	return 0;
}
 */

//int FSfeof(FSFILE* stream);
//size_t FSfread(void* ptr, size_t size, size_t n, FSFILE* stream);
/*
int fs_showconfig(FSFILE* file)
{
	char line[2]; // or other suitable maximum line size
	int i;
	
	if (file != NULL) {
	    while (!FSfeof(file)) {
//			i = FSfread(line, sizeof(char), sizeof(line), file);
			i = FSfread(line, sizeof(char), 1, file);
			line[i] = '\0';
    	    printf("%s", line);
		}
//	    while (fscanf(file, "%s", line)!=EOF) {
//		while (fgets(line, sizeof(line), file) != NULL) {
//    	    printf("%s", line);
//		}
//    	fclose(file);
	}
	return 0;
}

int fs_openconfig(char* filename)
{
	FSFILE* cfile;

	cfile = FSfopen(filename, "r");
	if (cfile != NULL) {
		printf("%s opened\n", filename);
		fs_showconfig(cfile);
	} else {
		printf("ERROR: FSfopen(%s) failed\r\n", filename);
		return -1;
	}
	return 0;
}
 */
/*
int openconfig(char* filename)
{
	char line[128]; // or other suitable maximum line size
	FILE* file;

	file = fopen(filename, "r");
	if (file != NULL) {
		printf("%s opened\n", filename);
//	    while (fscanf(file, "%s", line)!=EOF) {
		while (fgets(line, sizeof(line), file) != NULL) {
    	    printf("%s", line);
		}
    	fclose(file);
	} else {
		printf("ERROR: fopen %s failed\r\n", filename);
		return -1;
	}
	return 0;
}
 */
