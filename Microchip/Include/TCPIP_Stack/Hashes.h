/*********************************************************************
 *
 *					Hash Function Library Headers
 *
 *********************************************************************
 * FileName:        Hashes.h
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
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
 * IMPORTANT:  The implementation and use of third party algorithms, 
 * specifications and/or other technology may require a license from 
 * various third parties.  It is your responsibility to obtain 
 * information regarding any applicable licensing obligations.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Elliott Wood         05/01/07	Original
 ********************************************************************/

#ifndef __HASHES_H
#define __HASHES_H

/****************************************************************************
  Section:
	Data Types
  ***************************************************************************/

// Type of hash being calculated
typedef enum
{
	HASH_MD5	= 0u,		// MD5 is being calculated
	HASH_SHA1				// SHA-1 is being calculated
} HASH_TYPE;

// Context storage for a hash operation
typedef struct
{
	DWORD h0;				// Hash state h0
	DWORD h1;				// Hash state h1
	DWORD h2;				// Hash state h2
	DWORD h3;				// Hash state h3
	DWORD h4;				// Hash state h4
	DWORD bytesSoFar;		// Total number of bytes hashed so far
	BYTE partialBlock[64];	// Beginning of next 64 byte block
	HASH_TYPE hashType;		// Type of hash being calculated
} HASH_SUM;


/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/

#if defined(STACK_USE_SHA1)
	void SHA1Initialize(HASH_SUM* theSum);
	void SHA1AddData(HASH_SUM* theSum, BYTE* data, WORD len);
	void SHA1Calculate(HASH_SUM* theSum, BYTE* result);
	#if defined(__18CXX)
		void SHA1AddROMData(HASH_SUM* theSum, ROM BYTE* data, WORD len);
	#else
		// Non-ROM variant for C30 / C32
		#define SHA1AddROMData(a,b,c)	SHA1AddData(a,(BYTE*)b,c)
	#endif
#endif

#if defined(STACK_USE_MD5)
	void MD5Initialize(HASH_SUM* theSum);
	void MD5AddData(HASH_SUM* theSum, BYTE* data, WORD len);
	void MD5Calculate(HASH_SUM* theSum, BYTE* result);
	#if defined(__18CXX)
		void MD5AddROMData(HASH_SUM* theSum, ROM BYTE* data, WORD len);
	#else
		// Non-ROM variant for C30 / C32
		#define MD5AddROMData(a,b,c)	MD5AddData(a,(BYTE*)b,c)
	#endif
#endif

void HashAddData(HASH_SUM* theSum, BYTE* data, WORD len);
#if defined(__18CXX)
	void HashAddROMData(HASH_SUM* theSum, ROM BYTE* data, WORD len);
#else
	// Non-ROM variant for C30 / C32
	#define HashAddROMData(a,b,c)	HashAddData(a,(BYTE*)b,c)
#endif

#endif

