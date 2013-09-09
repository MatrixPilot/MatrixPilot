/*********************************************************************
 *
 *	Big Integer Library
 *  Library for Microchip TCP/IP Stack
 *	 - Provides support for integers larger than 32 bits
 *
 *********************************************************************
 * FileName:        BigInt.c
 * Dependencies:    BigInt_helper.asm (PIC18), BigInt_helper.S 
 *					(PIC24/dsPIC) or BigInt_helper_C32.S (PIC32)
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
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
 ********************************************************************/
#define __BIGINT_C

#include "TCPIPConfig.h"
#include "HardwareProfile.h"
#include "TCPIP Stack/SSLClientSize.h"

#if (defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)) && (!defined(ENC100_INTERFACE_MODE) || (SSL_RSA_CLIENT_SIZE > 1024))

#include "TCPIP Stack/TCPIP.h"

// External declarations for assembly helpers
#if defined(__C30__)
	extern __attribute__((__near__)) BIGINT_DATA_TYPE *_iA, *_iB, *_xA, *_xB, *_iR, _wC;
#else
	extern BIGINT_DATA_TYPE *_iA, *_iB, *_xA, *_xB, *_iR, _wC;
#endif
extern void _addBI(void);
extern void _subBI(void);
extern void _zeroBI(void);
extern void _msbBI(void);
extern void _mulBI(void);
extern void _copyBI(void);
extern void _sqrBI(void);
extern void _masBI(void);
extern void _addBIROM(void);
extern void _subBIROM(void);
extern void _mulBIROM(void);
extern void _masBIROM(void);


#if BIGINT_PROFILE
	DWORD addBICounter = 0;
	DWORD addBIROMCounter = 0;
	DWORD subBICounter = 0;
	DWORD subBIROMCounter = 0;
	DWORD zeroBICounter = 0;
	DWORD msbBICounter = 0;
	DWORD mulBICounter = 0;
	DWORD mulBIROMCounter = 0;
	DWORD sqrBICounter = 0;
	DWORD masBICounter = 0;
	DWORD masBIROMCounter = 0;
	DWORD copyBICounter = 0;
	
	#define	addBI()		{addBICounter -= TickGet(); _addBI(); addBICounter += TickGet();}
	#define	addBIROM()	{addBIROMCounter -= TickGet(); _addBIROM();addBIROMCounter += TickGet();}
	#define	subBI()		{subBICounter -= TickGet(); _subBI(); subBICounter += TickGet();}
	#define	subBIROM()	{subBIROMCounter -= TickGet(); _subBIROM(); subBIROMCounter += TickGet();}
	#define	zeroBI()	{zeroBICounter -= TickGet(); _zeroBI(); zeroBICounter += TickGet();}
	#define	msbBI()		{msbBICounter -= TickGet(); _msbBI(); msbBICounter += TickGet();}
	#define	mulBI()		{mulBICounter -= TickGet(); _mulBI(); mulBICounter += TickGet();}
	#define	mulBIROM()	{mulBIROMCounter -= TickGet(); _mulBIROM(); mulBIROMCounter += TickGet();}
	#define	sqrBI()		{sqrBICounter -= TickGet(); _sqrBI(); sqrBICounter += TickGet();}
	#define	masBI()		{masBICounter -= TickGet(); _masBI(); masBICounter += TickGet();}
	#define	masBIROM()	{masBIROMCounter -= TickGet(); _masBIROM(); masBIROMCounter += TickGet();}
	#define	copyBI()	{copyBICounter -= TickGet(); _copyBI(); copyBICounter += TickGet();}
#else
	#define	addBI()		_addBI()
	#define	addBIROM()	_addBIROM()
	#define	subBI()		_subBI()
	#define	subBIROM()	_subBIROM()
	#define	zeroBI()	_zeroBI()
	#define	msbBI()		_msbBI()
	#define	mulBI()		_mulBI()
	#define	mulBIROM()	_mulBIROM()
	#define	sqrBI()		_sqrBI()
	#define	masBI()		_masBI()
	#define	masBIROM()	_masBIROM()
	#define	copyBI()	_copyBI()
#endif

#if BIGINT_DEBUG
	#if defined(__18CXX)
		void BigIntPrint(const BIGINT *a)
		{
		  	BIGINT_DATA_TYPE *ptr;
		  	
		  	for(ptr = a->ptrMSBMax; ptr >= a->ptrLSB; ptr--)
		  	{
				while(BusyUART());
				putcUART(btohexa_high(*ptr));
				while(BusyUART());
				putcUART(btohexa_low(*ptr));
			}
		}
		void BigIntPrintROM(BIGINT_ROM *a)
		{
		  	ROM BIGINT_DATA_TYPE *ptr;
		  	
		  	for(ptr = a->ptrMSB; ptr >= a->ptrLSB; ptr--)
		  	{
				while(BusyUART());
				putcUART(btohexa_high(*ptr));
				while(BusyUART());
				putcUART(btohexa_low(*ptr));
			}
		}
	#elif defined(__C30__)
		void BigIntPrint(const BIGINT *a)
		{
		  	WORD w;
		  	BYTE v;
		  	
		  	BIGINT_DATA_TYPE *ptr;
		  	
		  	for(ptr = a->ptrMSBMax; ptr >= a->ptrLSB; ptr--)
		  	{
			  	WORD_VAL wv;
			  	
			  	wv.Val = *ptr;
			  	
				while(BusyUART());
				putcUART(btohexa_high(wv.v[1]));
				while(BusyUART());
				putcUART(btohexa_low(wv.v[1]));
				while(BusyUART());
				putcUART(btohexa_high(wv.v[0]));
				while(BusyUART());
				putcUART(btohexa_low(wv.v[0]));
			}
		}
	#endif
	
	void putulhexUART(DWORD dw)
	{
		while(BusyUART());
		putcUART('0');
		while(BusyUART());
		putcUART('x');
		while(BusyUART());
		putcUART(btohexa_high(((BYTE*)&dw)[3]));
		while(BusyUART());
		putcUART(btohexa_low(((BYTE*)&dw)[3]));
		while(BusyUART());
		putcUART(btohexa_high(((BYTE*)&dw)[2]));
		while(BusyUART());
		putcUART(btohexa_low(((BYTE*)&dw)[2]));
		while(BusyUART());
		putcUART(btohexa_high(((BYTE*)&dw)[1]));
		while(BusyUART());
		putcUART(btohexa_low(((BYTE*)&dw)[1]));
		while(BusyUART());
		putcUART(btohexa_high(((BYTE*)&dw)[0]));
		while(BusyUART());
		putcUART(btohexa_low(((BYTE*)&dw)[0]));
	}
#endif


static BIGINT_DATA_TYPE* BigIntMSB(BIGINT *n);

/*********************************************************************
 * Function:        void BigInt(BIGINT *theInt, BIGINT_DATA_TYPE *data, WORD wWordLength)
 *
 * PreCondition:    None
 *
 * Input:           *theInt: the integer to create
 *					*data: a pointer to the data
 *					wWordLength: the number of words in the integer (a word is 1 byte on PIC18s, 2 bytes on PIC24/dsPIC)
 *
 * Output:          The BIGINT is ready to use
 *
 * Side Effects:    None
 *
 * Overview:        Call BigInt() to correctly set up the pointers.
 *
 * Note:            None
 ********************************************************************/
#if defined(BI_USE_CONSTRUCTOR)
void BigInt(BIGINT *theInt, BIGINT_DATA_TYPE *data, WORD wWordLength)
{
	theInt->ptrLSB = data;
	theInt->ptrMSB = data + wWordLength - 1;
	theInt->ptrMSBMax = theInt->ptrMSB;
	theInt->bMSBValid = 0;
}
#endif

#if defined(__18CXX) && defined(BI_USE_CONSTRUCTOR_ROM)
void BigIntROM(BIGINT_ROM *theInt, ROM BIGINT_DATA_TYPE *data, WORD wWordLength)
{
	theInt->ptrLSB = data;
	theInt->ptrMSB = data + wWordLength - 1;

	// Find the MSB, which can never change
	while(*theInt->ptrMSB == 0u)
		theInt->ptrMSB--;
}
#endif

/*********************************************************************
 * Function:        void BigIntZero(BIGINT* theInt)
 *
 * PreCondition:    None
 *
 * Input:           *theInt: the integer to clear
 *
 * Output:          theInt = 0
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntZero() zero all data bytes in the BigInt
 *
 * Note:            None
 ********************************************************************/
#if defined(BI_USE_ZERO)
void BigIntZero(BIGINT *theInt)
{
	_iA = theInt->ptrLSB;
	_xA = theInt->ptrMSBMax;
	zeroBI();

	// Set up the new MSB pointer
	theInt->ptrMSB = theInt->ptrLSB;
	theInt->bMSBValid = 1;
}
#endif

/*********************************************************************
 * Function:        void BigIntMod(BIGINT *n, const BIGINT* m)
 *
 * PreCondition:    None
 *
 * Input:           *n: a pointer to the number
 *					*m: a pointer to the modulus
 *					
 * Output:          *n contains the modded number
 * 					i.e: *n = *n % *m
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntMod() to calculate the modulus of two
 *					really big numbers.
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/
#if defined(BI_USE_MOD)
void BigIntMod(BIGINT *n, BIGINT* m)
{
	BIGINT_DATA_TYPE *ptrMSBn, MSBm;
	BIGINT_DATA_TYPE_2 qHatInt;
	union
	{
		BIGINT_DATA_TYPE v[2];
		BIGINT_DATA_TYPE_2 Val;
	} topTwoWords;

	// Find the starting MSBs
	ptrMSBn = BigIntMSB(n);
	MSBm = *BigIntMSB(m);

	// Set up assembly pointers for m
	// _iB and _xB are limiters in the _mas function
	_iB = m->ptrLSB;
	_xB = BigIntMSB(m);

	// Find out how many bytes we need to shift and move the LSB up
	_iR = n->ptrLSB + (BigIntMagnitudeDifference(n, m) - 1);

	// This loops while the order of magnitude (in words) of n > m
	// Each iteration modulos off one word of magnitude from n
	while(_iR >= n->ptrLSB)
	{
		// Find qHat = MSBn:MSBn-1/MSBm
		topTwoWords.Val = *((BIGINT_DATA_TYPE_2*)(ptrMSBn - 1));
		qHatInt = topTwoWords.Val / MSBm;
		if(qHatInt > BIGINT_DATA_MAX)  
			qHatInt = BIGINT_DATA_MAX;

#if BIGINT_DEBUG
		putrsUART("\r\n\r\n    n = ");
		BigIntPrint(n);
		putrsUART("\r\n    m = ");
		BigIntPrint(m);
		putrsUART("\r\n    qHat (");
		putulhexUART(qHatInt);
		putrsUART(") = topTwo(");
		putulhexUART(topTwoWords.Val);
		putrsUART(") / (");
		putulhexUART(MSBm);
		putrsUART(") ");
#endif

		// Once qHat is determined, we multiply M by qHat, shift it up
		// as many bytes as possible, and subtract the result.
		// In essence, qHat is a rough estimation of the quotient, divided
		// by a power of 2^8 (PIC18) or 2^16 (PIC24/dsPIC) or 2^32 (PIC32)
		
		// This implementation multiplies and subtracts in the same step
		// using a _mas function which saves about 30% of clock cycles.

		// Save the old MSB and set up the ASM pointers
		_wC = (BIGINT_DATA_TYPE)qHatInt;

		// Do the multiply and subtract
		// Occassionally this results in underflow...this is solved below.
		masBI();

		// qHat may have been 1 or 2 greater than possible.  If so,
		// the new MSB will be greater than the old one, so we *add*
		// M back to N in the shifted position until overflow occurs
		// and this case corrects itself.
		while(topTwoWords.v[1] < *BigIntMSB(n))
//		while(((BIGINT_DATA_TYPE*)&topTwoWords)[1] < *BigIntMSB(n))
		{
			_iA = _iR;
			_xA = BigIntMSB(n);
			addBI();
		}

		// We should have modulated off a word (or two if we were lucky),
		// so move our MSB and LSB pointers as applicable
		while(*ptrMSBn == 0x0u)
		{
			_iR--;
			n->ptrMSB--;
			ptrMSBn--;
		}
	}

	// Iteration of the _mas function can only handle full-byte orders
	// of magnitude.  The result may still be a little larger, so this
	// cleans up the last few multiples with simple subtraction.
	while(BigIntCompare(n, m) >= 0)
	{
		_iA = n->ptrLSB;
		_xA = n->ptrMSB;
		subBI();
	
		// Invalidate MSB pointer
		n->bMSBValid = 0;
	}
}
#endif

#if defined(__18CXX) && defined(BI_USE_MOD_ROM)
void BigIntModROM(BIGINT *n, BIGINT_ROM* m)
{
	BIGINT_DATA_TYPE *ptrMSBn, MSBm;
	BIGINT_DATA_TYPE_2 qHatInt, topTwoWords;

	// Find the starting MSBs
	ptrMSBn = BigIntMSB(n);
	MSBm = *m->ptrMSB;

	// Set up assembly pointers for m
	// _iBr and _xBr are limiters in the _masROM function
	_iBr = m->ptrLSB;
	_xBr = m->ptrMSB;

	// Find out how many bytes we need to shift and move the LSB up
	_iR = n->ptrLSB + (BigIntMagnitudeDifferenceROM(n, m) - 1);

	// This loops while the order of magnitude (in words) of n > m
	// Each iteration modulos off one word of magnitude from n
	while(_iR >= n->ptrLSB)
	{
		// Find qHat = MSBn:MSBn-1/MSBm
		topTwoWords = *((BIGINT_DATA_TYPE_2*)(ptrMSBn - 1));
		qHatInt = topTwoWords / MSBm;
		if(qHatInt > BIGINT_DATA_MAX)  
			qHatInt = BIGINT_DATA_MAX;

#if BIGINT_DEBUG
		putrsUART("\r\n\r\n    n = ");
		BigIntPrint(n);
		putrsUART("\r\n    m = ");
		BigIntPrintROM(m);
		putrsUART("\r\n    qHat (");
		putulhexUART(qHatInt);
		putrsUART(") = topTwo(");
		putulhexUART(topTwoWords);
		putrsUART(") / (");
		putulhexUART(MSBm);
		putrsUART(") ");
#endif

		// Once qHat is determined, we multiply M by qHat, shift it up
		// as many bytes as possible, and subtract the result.
		// In essence, qHat is a rough estimation of the quotient, divided
		// by a power of 2^8 (PIC18) or 2^16 (PIC24/dsPIC) or 2^32 (PIC32)
		
		// This implementation multiplies and subtracts in the same step
		// using a _mas function which saves about 30% of clock cycles.

		// Save the old MSB and set up the ASM pointers
		_wC = (BIGINT_DATA_TYPE)qHatInt;

		// Do the multiply and subtract
		// Occassionally this results in underflow...this is solved below.
		masBIROM();

		// qHat may have been 1 or 2 greater than possible.  If so,
		// the new MSB will be greater than the old one, so we *add*
		// M back to N in the shifted position until overflow occurs
		// and this case corrects itself.
		while(((BIGINT_DATA_TYPE*)&topTwoWords)[1] < *BigIntMSB(n))
		{
			_iA = _iR;
			_xA = BigIntMSB(n);
			addBIROM();
		}

		// We should have modulated off a word (or two if we were lucky),
		// so move our MSB and LSB pointers as applicable
		while(*ptrMSBn == 0x0u)
		{
			_iR--;
			n->ptrMSB--;
			ptrMSBn--;
		}
	}

	// Iteration of the _mas function can only handle full-byte orders
	// of magnitude.  The result may still be a little larger, so this
	// cleans up the last few multiples with simple subtraction.
	while(BigIntCompareROM(n, m) >= 0)
	{
		_iA = n->ptrLSB;
		_xA = n->ptrMSB;
		subBIROM();
	
		// Invalidate MSB pointer
		n->bMSBValid = 0;
	}
}
#endif

/*********************************************************************
 * Function:        CHAR BigIntCompare(BIGINT *a, BIGINT *b)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to the first number
 *					*b: a pointer to the second number
 *
 * Output:          0 if a == b
 *					1 if a > b
 *					-1 if a < b
 *
 * Side Effects:    None
 *
 * Overview:        Determines if a > b, a < b, or a == b
 *
 * Note:            Supports at least 2048 bits.
 *					
 ********************************************************************/
#if defined(BI_USE_COMPARE)
CHAR BigIntCompare(BIGINT *a, BIGINT *b)
{
	PTR_BASE				magA, magB;
	BIGINT_DATA_TYPE 		valA, valB;
	BIGINT_DATA_TYPE 		*ptrA;
	BIGINT_DATA_TYPE 		*ptrB;
	
	magA = BigIntMSB(a) - a->ptrLSB;
	magB = BigIntMSB(b) - b->ptrLSB;

	#if BIGINT_DEBUG_COMPARE
		putrsUART("\r\n    Compared Magnitudes |a|:");
		putulhexUART(w1);
		putrsUART(" |b|:");
		putulhexUART(w2);
		putrsUART(" diff:");
		putulhexUART(w1-w2);
	#endif

	if(magA > magB)
	{
		#if BIGINT_DEBUG_COMPARE
			putrsUART(" a > b");
		#endif
		
		return 1;
	}
	else if(magA < magB)
	{
		#if BIGINT_DEBUG_COMPARE
			putrsUART(" a < b");
		#endif
		
		return -1;
	}

	#if BIGINT_DEBUG_COMPARE
		putrsUART(" Checking further bytes...");
	#endif
	
	// Loop through all words, looking for a non-equal word
	ptrA = BigIntMSB(a);
	ptrB = BigIntMSB(b);
	while(ptrA >= a->ptrLSB)	// Magnitude is same, no need to check ptrB bounds
	{
		valA = *ptrA--;
		valB = *ptrB--;
		
		if(valA > valB)
		{
			#if BIGINT_DEBUG_COMPARE
				putrsUART(" a > b");
			#endif
			
			return 1;
		}
		else if(valA < valB)
		{
			#if BIGINT_DEBUG_COMPARE
				putrsUART(" a < b");
			#endif
			
			return -1;
		}	
	}

	// All words were exactly identical, return match
	return 0;
}
#endif

#if defined(__18CXX) && defined(BI_USE_COMPARE_ROM)
CHAR BigIntCompareROM(BIGINT *a, BIGINT_ROM *b)
{
	PTR_BASE				magA, magB;
	BIGINT_DATA_TYPE		valA, valB;
	BIGINT_DATA_TYPE		*ptrA;
	ROM BIGINT_DATA_TYPE	*ptrB;
	
	magA = BigIntMSB(a) - a->ptrLSB;
	magB = b->ptrMSB - b->ptrLSB;

	#if BIGINT_DEBUG_COMPARE
		putrsUART("\r\n    Compared Magnitudes |a|:");
		putulhexUART(w1);
		putrsUART(" |b|:");
		putulhexUART(w2);
		putrsUART(" diff:");
		putulhexUART(s);
	#endif

	if(magA > magB)
	{
		#if BIGINT_DEBUG_COMPARE
			putrsUART(" a > b");
		#endif
		
		return 1;
	}
	else if(magA < magB)
	{
		#if BIGINT_DEBUG_COMPARE
			putrsUART(" a < b");
		#endif
		
		return -1;
	}

	#if BIGINT_DEBUG_COMPARE
		putrsUART(" Checking further bytes...");
	#endif
	
	// Loop through all words, looking for a non-equal word
	ptrA = BigIntMSB(a);
	ptrB = b->ptrMSB;
	while(ptrA >= a->ptrLSB)	// Magnitude is same, no need to check ptrB bounds
	{
		valA = *ptrA--;
		valB = *ptrB--;
		
		if(valA > valB)
		{
			#if BIGINT_DEBUG_COMPARE
				putrsUART(" a > b");
			#endif
			
			return 1;
		}
		else if (valA < valB)
		{
			#if BIGINT_DEBUG_COMPARE
				putrsUART(" a < b");
			#endif
			
			return -1;
		}	
	}

	// All words were exactly identical, return match
	return 0;
}
#endif


/*********************************************************************
 * Function:        int BigIntMagnitudeDifference(const BIGINT *n)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to the first number
 *					*b: a pointer to the second number
 *					
 * Output:          Returns the magnitude of difference in zero-bytes
 *
 * Side Effects:    None
 *
 * Overview:        Helps to quickly determine a byte shift for operations
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/
#if defined(BI_USE_MAG_DIFF)
int BigIntMagnitudeDifference(BIGINT *a, BIGINT *b)
{
	return BigIntMagnitude(a) - BigIntMagnitude(b);
}
#endif

#if defined(__18CXX) && defined(BI_USE_MAG_DIFF_ROM)
int BigIntMagnitudeDifferenceROM(BIGINT *a, BIGINT_ROM *b)
{
	return BigIntMagnitude(a) - BigIntMagnitudeROM(b);
}
#endif

/*********************************************************************
 * Function:        WORD BigIntMagnitude(BIGINT *n)
 *
 * PreCondition:    None
 *
 * Input:           *n: a pointer to the number
 *					
 * Output:          Returns the number of significant words in the data, less one (ex: 0x12, has zero magnitude)
 *
 * Side Effects:    None
 *
 * Overview:        Helps to quickly determine the magnitude of the number
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/
#if defined(BI_USE_MAG)
WORD BigIntMagnitude(BIGINT *n)
{
	return BigIntMSB(n) - n->ptrLSB;
}
#endif

#if defined(__18CXX) && defined(BI_USE_MAG_ROM)
WORD BigIntMagnitudeROM(BIGINT_ROM *n)
{
	return n->ptrMSB - n->ptrLSB;
}
#endif

/*********************************************************************
 * Function:        static BIGINT_DATA_TYPE* BigIntMSB(const BIGINT *n)
 *
 * PreCondition:    None
 *
 * Input:           *n: a pointer to the number
 *					
 * Output:          n->ptrMSB points to the MSB of n
 *
 * Side Effects:    None
 *
 * Overview:        Updates the ptrMSB.  Use after an operation in which
 *					the new MSB cannot be estimated.
 *
 * Note:            Supports at least 2048 bits
 ********************************************************************/
#if defined(BI_USE_MSB)
static BIGINT_DATA_TYPE* BigIntMSB(BIGINT *n)
{
	BIGINT_DATA_TYPE *iASave, *xASave;
	
	// If cached value is valid, use it
	if(n->bMSBValid)
		return n->ptrMSB;

	// Otherwise find a new MSB and save it
	iASave = _iA;
	xASave = _xA;
	_iA = n->ptrLSB;
	_xA = n->ptrMSBMax;
	msbBI();
	n->ptrMSB = _xA;
	n->bMSBValid = 1;

	_iA = iASave;
	_xA = xASave;

	return n->ptrMSB;
}
#endif

/*********************************************************************
 * Function:        void BigIntMultiply(const BIGINT *a, const BIGINT *b, BIGINT *res)
 *
 * PreCondition:    res->ptrMSBMax - res->ptrLSB + 1 >= a->ptrMSBMax - a->ptrLSB + 1 + b->ptrMSBMax - b->ptrLSB + 1, &res != &[a|b]
 *
 * Input:           *a: a pointer to the first number
 *					*b: a pointer to the second number
 *					*res: a pointer to memory to store the result
 *
 * Output:          *res contains the result of a * b
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntMultiply() to multiply two really big numbers.
 *
 * Note:            Supports at least 2048 result bits.
 *					This essentially implements elementary school long
 *					multiplication in base 256 (PIC18) or 65536 (PIC24/dsPIC).  
 *					This is the fastest	algorithm until you pass about 
 *					1024 bits.  This is O(n^2).
 *					res CANNOT be A or B.
 ********************************************************************/
#if defined(BI_USE_MULTIPLY)
void BigIntMultiply(BIGINT *a, BIGINT *b, BIGINT *res)
{
	// Clear out the result
	BigIntZero(res);

	// Load the start and stop pointers
	_iA = a->ptrLSB;
	_xA = BigIntMSB(a);
	_iB = b->ptrLSB;
	_xB = BigIntMSB(b);
	_iR = res->ptrLSB;

	// Perform the multiplication
	mulBI();

	// Invalidate the MSB ptr
	res->bMSBValid = 0;
}
#endif

#if defined(__18CXX) && defined(BI_USE_MULTIPLY_ROM)
void BigIntMultiplyROM(BIGINT *a, BIGINT_ROM *b, BIGINT *res)
{
	//clear out the result
	BigIntZero(res);

	// Load the start and stop pointers
	_iA = a->ptrLSB;
	_xA = BigIntMSB(a);
	_iBr = b->ptrLSB;
	_xBr = b->ptrMSB;
	_iR = res->ptrLSB;

	// Perform the multiplication
	mulBIROM();

	// Invalidate the MSB ptr
	res->bMSBValid = 0;
}
#endif

/*********************************************************************
 * Function:        void BigIntSquare(const BIGINT *a, BIGINT *res)
 *
 * PreCondition:    res->size >= 2 * a->size, &res != &a
 *
 * Input:           *a: a pointer to the number
 *					*res: a pointer to memory to store the result
 *
 * Output:          *res contains the result of a * a
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntSquare() to square two really big numbers.
 *
 * Note:            Supports at least 2048 result bits.
 *					Functionally equivalent to BigIntMultiply, except
 *					an optimization is made for the case of square that
 *					allows us to skip ~1/2 the iterations.
 *					res CANNOT be A.
 ********************************************************************/
#if defined(BI_USE_SQUARE)
void BigIntSquare(BIGINT *a, BIGINT *res)
{
	BigIntZero(res);
	_iA = a->ptrLSB;
	_xA = BigIntMSB(a);
	_iR = res->ptrLSB;
	sqrBI();

	// Invalidate the MSB ptr
	res->bMSBValid = 0;
}
#endif

/*********************************************************************
 * Function:        void BigIntAdd(BIGINT *a, const BIGINT *b)
 *
 * PreCondition:    a->ptrMSBMax - a->ptrLSB must be >= b->ptrMSB - b->ptrLSB
 *
 * Input:           *a: a pointer to the first number
 *					*b: a pointer to the second number
 *
 * Output:          a = a + b
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntAdd() to add two really big numbers
 *
 * Note:            Supports at least 2048 result bits.
 ********************************************************************/
#if defined(BI_USE_ADD)
void BigIntAdd(BIGINT *a, BIGINT *b)
{
	_iA = a->ptrLSB;
	_xA = a->ptrMSBMax;
	_iB = b->ptrLSB;
	_xB = BigIntMSB(b);
	addBI();

	// Invalidate MSB pointer
	a->bMSBValid = 0;
}
#endif

#if defined(__18CXX) && defined(BI_USE_ADD_ROM)
// Note: This function is not required by either RSA operation
void BigIntAddROM(BIGINT *a, BIGINT_ROM *b)
{
	_iA = a->ptrLSB;
	_xA = a->ptrMSBMax;
	_iBr = b->ptrLSB;
	_xBr = b->ptrMSB;
	addBIROM();

	//invalidate MSB pointer
	a->bMSBValid = 0;
}
#endif

/*********************************************************************
 * Function:        void BigIntSubtract(BIGINT *a, const BIGINT *b)
 *
 * PreCondition:    a->ptrMSBMax - a->ptrLSB must be >= b->ptrMSB - b->ptrLSB
 *
 * Input:           *a: a pointer to the first number
 *					*b: a pointer to the second number
 *
 * Output:          a = a - b
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntSubtract() to subtract two really big numbers
 *
 * Note:            Supports at least 2048 result bits.
 ********************************************************************/
#if defined(BI_USE_SUBTRACT)
void BigIntSubtract(BIGINT *a, BIGINT *b)
{
	_iA = a->ptrLSB;
	_xA = a->ptrMSBMax;
	_iB = b->ptrLSB;
	_xB = BigIntMSB(b);
	subBI();

	// Invalidate MSB pointer
	a->bMSBValid = 0;
}
#endif

#if defined(__18CXX) && defined(BI_USE_SUBTRACT_ROM)
// Note: This function is not required by either RSA operation
void BigIntSubtractROM(BIGINT *a, BIGINT_ROM *b)
{
	_iA = a->ptrLSB;
	_xA = a->ptrMSBMax;
	_iBr = b->ptrLSB;
	_xBr = b->ptrMSB;
	subBIROM();

	//invalidate MSB pointer
	a->bMSBValid = 0;
}
#endif

/*********************************************************************
 * Function:        void BigIntCopy(BIGINT *a, const BIGINT *b)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to a BIGINT to copy into
 *					*b: a pointer to the data
 *
 * Output:          a = b
 *
 * Side Effects:    None
 *
 * Overview:        Call BigIntCopy() copy one BIGINT to another
 *
 * Note:            Supports at least 2048 bits.  Only data is copied, so
 *					if sizeof(b) > sizeof(a), only the least significant 
 *					sizeof(a) bytes are copied.
 ********************************************************************/
#if defined(BI_USE_COPY)
void BigIntCopy(BIGINT *a, BIGINT *b)
{
	_iA = a->ptrLSB;
	_xA = a->ptrMSBMax;
	_iB = b->ptrLSB;
	_xB = b->ptrMSBMax;
	copyBI();

	// Invalidate MSB pointer
	a->bMSBValid = 0;
}
#endif

#if defined(__18CXX) && defined(BI_USE_COPY_ROM)
void BigIntCopyROM(BIGINT *a, BIGINT_ROM *b)
{
	BIGINT_DATA_TYPE *pa;
	ROM BIGINT_DATA_TYPE *pb;
	
	for(pa = a->ptrLSB, pb = b->ptrLSB; (pa <= a->ptrMSBMax) && (pb <= b->ptrMSB); pa++, pb++)
		*pa = *pb;
	
	// Zero fill remainder
	while(pa <= a->ptrMSBMax)
	{
		*pa = 0;
		pa++;
	}

	// Invalidate MSB pointer
	a->bMSBValid = 0;
}
#endif	//#if defined(__18CXX)

/*********************************************************************
 * Function:        void BigIntSwapEndianness(BIGINT *a)
 *
 * PreCondition:    None
 *
 * Input:           *a: a pointer to the BigInt
 *
 * Output:          *a: same value, with endianness swapped
 *
 * Side Effects:    None
 *
 * Overview:        Converts a big-endian data array to little-endian,
 *					or a little-endian data array to big-endian.
 *
 * Note:            None
 ********************************************************************/
void BigIntSwapEndianness(BIGINT *a)
{
	BYTE temp, *front, *end;
	
	// Set up the pointers
	front = (BYTE*)a->ptrLSB;
	end = (BYTE*)a->ptrMSBMax + (sizeof(BIGINT_DATA_TYPE) - 1);
	
	// Swap all data elements
	while(front < end)
	{
		temp = *front;
		*front = *end;
		*end = temp;
		front++;
		end--;
	}
			
}

#endif // #if (defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)) && !defined(ENC100_INTERFACE_MODE)
