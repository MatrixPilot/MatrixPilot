/*********************************************************************
 *
 *     File system access interface layer Source File
 *
 *********************************************************************
 * FileName:        FileSystem.c
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
 * Amit Shirbhate	     7/18/09  	Modified
 ********************************************************************/
#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_MDD)

int FileSystemInit(void)
{
    #if defined STACK_USE_MPFS2
        MPFSInit();
    #elif defined STACK_USE_MDD 
        return FSInit();
    #endif
    
    return TRUE;
}


FILE_HANDLE FileOpen(const char * fileName, const char *mode)
{
    #if defined STACK_USE_MPFS2
        return MPFSOpen((BYTE*)fileName);
    #elif defined STACK_USE_MDD 
        return FSfopen(fileName, mode);
    #endif
}

FILE_HANDLE FileOpenROM(const char * fileName, const char *mode)
{
    #if defined STACK_USE_MPFS2
        return MPFSOpenROM((BYTE*)fileName);
    #elif defined STACK_USE_MDD 
        return FSfopen(fileName, mode);
    #endif
}

int FileClose(FILE_HANDLE fh)
{
    #if defined STACK_USE_MPFS2
        MPFSClose(fh);
    #elif defined STACK_USE_MDD 
        return FSfclose(fh);
    #endif

    return 0;
}

size_t FileRead(void *ptr, size_t size, size_t n, FILE_HANDLE stream)
{
    #if defined STACK_USE_MPFS2
        WORD length;
        length = size * n;
        return MPFSGetArray(stream, (BYTE*)ptr, length);
    #elif defined STACK_USE_MDD 
        if(ptr == NULL)
        {
            return 0;
        }
        else
        {
            return FSfread(ptr, size, n, stream);
        }
    #endif
}

int FileSeek(FILE_HANDLE stream, long offset, int whence)
{
    #if defined STACK_USE_MPFS2
        BOOL status;
        status = MPFSSeek(stream, offset, whence);
        if(status == TRUE)
            return 0;
        else
            return -1;
        
    #elif defined STACK_USE_MDD 
        return FSfseek(stream, offset, whence);
    #endif
}

long FileTell(FILE_HANDLE fh)
{
    #if defined STACK_USE_MPFS2
        return MPFSGetPosition(fh);
    #elif defined STACK_USE_MDD 
        return FSftell(fh);
    #endif
}

int FileEOF(FILE_HANDLE stream)
{
    #if defined STACK_USE_MPFS2
        return MPFSGetBytesRem(stream);
    #elif defined STACK_USE_MDD 
        return FSfeof(stream);
    #endif
}

int FileFormat(char mode, long int serialNumber, char *volumeID)
{
    #if defined STACK_USE_MPFS2
        int status;
        status = MPFSFormat();
        if(status == MPFS_INVALID_HANDLE)
            return -1;
        else
            return 0;
    #elif defined STACK_USE_MDD 
        return FSformat(mode, serialNumber, volumeID);
    #endif
}

size_t FileWrite(const void *ptr, size_t size, size_t n, FILE_HANDLE stream)
{
    #if defined STACK_USE_MPFS2
        WORD length;
        length = size * n;
        return MPFSPutArray(stream, (BYTE*)ptr, length);
    #elif defined STACK_USE_MDD 
        return FSfwrite(ptr, size, n, stream);
    #endif
}


size_t FileReadUInt32(DWORD *ptr, FILE_HANDLE stream)
{
	BYTE databuff[4];
	*ptr=0x00000000;

	#if defined STACK_USE_MPFS2
        WORD retVal;

		retVal=MPFSGetArray(stream, (BYTE*)ptr, 4);
	
		if(retVal == 4)//Number of Uints of 4 bytes each Read
		{
	
			((BYTE*)ptr)[3] = databuff[3];
			((BYTE*)ptr)[2] = databuff[2];
			((BYTE*)ptr)[1] = databuff[1];
			((BYTE*)ptr)[0] = databuff[0];

			return 4;//Number of bytes read
		}
		else
			return 0;
		
    #elif defined STACK_USE_MDD 
		size_t retVal; 
       
		retVal= FSfread(databuff, 4, 1, stream);

		if(retVal == 1)//Number of Uints of 4 bytes each Read
		{
	
			((BYTE*)ptr)[3] = databuff[3];
			((BYTE*)ptr)[2] = databuff[2];
			((BYTE*)ptr)[1] = databuff[1];
			((BYTE*)ptr)[0] = databuff[0];

			return 4;//Number of bytes read
		}
		else
			return 0;
        
    #endif
}

size_t FileReadUInt16(WORD *ptr, FILE_HANDLE stream)
{
	BYTE databuff[2];
	*ptr=0x0000;

	#if defined STACK_USE_MPFS2
        WORD retVal;

		retVal=MPFSGetArray(stream, (BYTE*)ptr, 2);

		if(retVal == 2)//Number of bytes read
		{
			((BYTE*)ptr)[1]= databuff[1];
			((BYTE*)ptr)[0]= databuff[0];
			return 2;//Number of bytes read
		}
		else
			return 0;
		
    #elif defined STACK_USE_MDD 
		size_t retVal; 
      
		retVal= FSfread(databuff, 2, 1, stream);

		if(retVal == 1)//Number of Uints of 4 bytes each Read
		{
			((BYTE*)ptr)[1]= databuff[1];
			((BYTE*)ptr)[0]= databuff[0];
			return 2;//Number of bytes read
		}
		else
			return 0;
        
    #endif
}


#endif //#if defined(STACK_USE_MDD)
