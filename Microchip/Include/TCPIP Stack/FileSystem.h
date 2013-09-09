/*********************************************************************
 *
 *     File system access interface layer header File
 *
 *********************************************************************
 * FileName:        FileSystem.h
 * Description:     File system access interface layer
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.00 or higher
 *					Microchip C30 v3.01 or higher
 *					Microchip C18 v3.20 or higher
 *					HI-TECH PICC-18 STD 9.50PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2008 Microchip Technology Inc.  All rights 
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and 
 * distribute: 
 * (i)  the Software when embedded on a Microchip microcontroller or 
 *      digital signal controller product ("Device") which is 
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and 
 *      ENC28J60.h ported to a non-Microchip device used in 
 *      conjunction with a Microchip ethernet controller for the 
 *      sole purpose of interfacing with the ethernet controller. 
 *
 * You should refer to the license agreement accompanying this 
 * Software for additional information regarding your rights and 
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT 
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL 
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS 
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE 
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER 
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT 
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Aseem Swalah         7/31/08  	Original
 * Amit Shirbhate	      7/18/09  	Modified
 ********************************************************************/
#ifndef _FILE_SYSTEM_HEADER_FILE
#define _FILE_SYSTEM_HEADER_FILE
 
#if defined STACK_USE_MPFS2
    #include "TCPIP Stack/MPFS2.h"
    typedef MPFS_HANDLE FILE_HANDLE;
    #define INVALID_FILE_HANDLE MPFS_INVALID_HANDLE
#elif defined STACK_USE_MDD 
    #include "MDD File System/FSIO.h"
    typedef FSFILE * FILE_HANDLE;
    #define INVALID_FILE_HANDLE NULL
#endif

// Indicates the method for MPFSSeek
	typedef enum
	{
		FILE_SEEK_START		= 0u,	// Seek forwards from the front of the file
		FILE_SEEK_FORWARD,			// Seek forward from the current position
		FILE_SEEK_END,				// Seek backwards from the end of the file
		FILE_SEEK_REWIND			// See backwards from the current position
	} FILE_SEEK_MODE;

int FileSystemInit(void);

FILE_HANDLE FileOpen(const char * fileName, const char *mode);

FILE_HANDLE FileOpenROM(const char * fileName, const char *mode);

int FileClose(FILE_HANDLE fh);

size_t FileRead(void *ptr, size_t size, size_t n, FILE_HANDLE stream);

int FileSeek(FILE_HANDLE stream, long offset, int whence);

long FileTell(FILE_HANDLE fh);

int FileEOF(FILE_HANDLE stream);

int FileFormat(char mode, long int serialNumber, char *volumeID);

size_t FileWrite(const void *ptr, size_t size, size_t n, FILE_HANDLE stream);

size_t FileReadUInt32(DWORD *ptr, FILE_HANDLE stream);

size_t FileReadUInt16(WORD *ptr, FILE_HANDLE stream);



#endif

