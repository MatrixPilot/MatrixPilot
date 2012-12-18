/*********************************************************************
 *
 *	Random Number Generator
 *  Library for Microchip TCP/IP Stack
 *	 - Provides a cryptographically secure method for generating
 *	   random data
 *
 *********************************************************************
 * FileName:        Random.c
 * Dependencies:    StackTsk.c
 *                  Tick.c
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Elliott Wood	     	5/09/07     Original        (Rev 1.0)
********************************************************************/

#define __RANDOM_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)

#include "TCPIP Stack/TCPIP.h"

static HASH_SUM randHash;
static BYTE output[20];
static BYTE bCount;

/*********************************************************************
 * Function:        void RandomInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Random number generator is initialized.
 *
 * Side Effects:    None
 *
 * Overview:        Sets up the random generator structure.
 *
 * Note:            Data may not be secure until several packets have
 *					been received.
 ********************************************************************/
void RandomInit(void)
{
	unsigned char i;
	unsigned long dw;
	
	SHA1Initialize(&randHash);
	
	// Add some starting entropy to the pool.  This is slow.
	for(i = 0; i < 5; i++)
	{
		dw = GenerateRandomDWORD();
		RandomAdd(((BYTE*)&dw)[0]);
		RandomAdd(((BYTE*)&dw)[1]);
		RandomAdd(((BYTE*)&dw)[2]);
		RandomAdd(((BYTE*)&dw)[3]);
	}
		
	bCount = 20;
}

/*********************************************************************
 * Function:        BYTE RandomGet(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          A random byte is generated
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
BYTE RandomGet(void)
{
	if(bCount >= 20u)
	{//we need to get new random bytes
		SHA1Calculate(&randHash, output);
		RandomAdd(output[0]);
		bCount = 0;
	}
	
	//return the random byte
	return output[bCount++];
}


/*********************************************************************
 * Function:        void RandomAdd(BYTE data)
 *
 * PreCondition:    None
 *
 * Input:           a random byte to add to the seed
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Hashes the byte and an asynchronous timer value
 *
 * Note:            None
 ********************************************************************/
void RandomAdd(BYTE data)
{
	DWORD dTemp;
	
	SHA1AddData(&randHash, &data, 1);
	dTemp = TickGet();
	SHA1AddData(&randHash, (BYTE*)&dTemp, 1);
	
	bCount = 20;
}

#endif	//#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)
