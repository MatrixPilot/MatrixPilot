/*********************************************************************
 *
 *					Big Integer Class Headers
 *
 *********************************************************************
 * FileName:        BigInt.h
 * Dependencies:    Compiler.h, GenericTypeDefs.h
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
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Elliott Wood	        2/15/07		Original
 ********************************************************************/

#ifndef __BIGINT_H
#define __BIGINT_H

#define BIGINT_DEBUG			0
#define BIGINT_DEBUG_COMPARE 	0
#define RSAEXP_DEBUG			0

#if defined(__18CXX)
	#define BIGINT_DATA_SIZE	8ul	//bits
	#define BIGINT_DATA_TYPE	BYTE
	#define BIGINT_DATA_MAX		0xFFu
	#define BIGINT_DATA_TYPE_2	WORD
#elif defined(__C30__)
	#define BIGINT_DATA_SIZE	16ul	//bits
	#define BIGINT_DATA_TYPE	WORD
	#define BIGINT_DATA_MAX		0xFFFFu
	#define BIGINT_DATA_TYPE_2	DWORD
#elif defined(__C32__)
	#define BIGINT_DATA_SIZE	32ul	//bits
	#define BIGINT_DATA_TYPE	DWORD
	#define BIGINT_DATA_MAX		0xFFFFFFFFu
	#define BIGINT_DATA_TYPE_2	QWORD
#endif

typedef struct
{
	BIGINT_DATA_TYPE *ptrLSB;		// Pointer to the least significant byte/word (lowest memory address)
	BIGINT_DATA_TYPE *ptrMSB;		// Pointer to the first non-zero most significant byte/word (higher memory address) if bMSBValid set
	BIGINT_DATA_TYPE *ptrMSBMax;	// Pointer to the maximum memory address that ptrMSB could ever be (highest memory address)
	BOOL bMSBValid; 
} BIGINT;

#if defined(__18CXX)
	typedef struct _BIGINT_ROM
	{
		ROM BIGINT_DATA_TYPE *ptrLSB;
		ROM BIGINT_DATA_TYPE *ptrMSB;
	} BIGINT_ROM;
#else
	#define BIGINT_ROM	BIGINT
#endif


void BigInt(BIGINT *theInt, BIGINT_DATA_TYPE *data, WORD wWordLength);
void BigIntMod(BIGINT*, BIGINT*);
void BigIntMultiply(BIGINT*, BIGINT*, BIGINT*);

void BigIntAdd(BIGINT*, BIGINT*);
void BigIntSubtract(BIGINT*, BIGINT*);
void BigIntSubtractROM(BIGINT*, BIGINT_ROM*);
void BigIntCopy(BIGINT*, BIGINT*);
void BigIntSquare(BIGINT *a, BIGINT *res);
void BigIntZero(BIGINT *theInt);

int BigIntMagnitudeDifference(BIGINT *a, BIGINT *b);
int BigIntMagnitudeDifferenceROM(BIGINT *a, BIGINT_ROM *b);
CHAR BigIntCompare(BIGINT*, BIGINT*);
WORD BigIntMagnitude(BIGINT *n);

void BigIntSwapEndianness(BIGINT *a);

void BigIntPrint(const BIGINT *a);


#if defined(__18CXX)	
	void BigIntROM(BIGINT_ROM *theInt, ROM BIGINT_DATA_TYPE *data, WORD wWordLength);
	void BigIntModROM(BIGINT*, BIGINT_ROM*);
	void BigIntMultiplyROM(BIGINT*, BIGINT_ROM*, BIGINT*);
	void BigIntAddROM(BIGINT*, BIGINT_ROM*);
	void BigIntCopyROM(BIGINT*, BIGINT_ROM*);
	CHAR BigIntCompareROM(BIGINT*, BIGINT_ROM*);
	WORD BigIntMagnitudeROM(BIGINT_ROM *n);
	
	extern ROM BIGINT_DATA_TYPE *_iBr, *_xBr;
	
	void BigIntPrintROM(BIGINT_ROM*);
#else
	#define BigIntROM(a,b,c)			BigInt(a,((BIGINT_DATA_TYPE*)(b)),c)
	#define BigIntModROM(a,b)			BigIntMod(a,b)
	#define BigIntMultiplyROM(a,b,c)	BigIntMultiply(a,b,c)
	#define BigIntAddROM(a,b)			BigIntAdd(a,b)
	#define BigIntCopyROM(a,b)			BigIntCopy(a,b)
	#define BigIntCompareROM(a,b)		BigIntCompare(a,b)
	#define BigIntMagnitudeROM(a)		BigIntMagnitude(a)
#endif


#endif
