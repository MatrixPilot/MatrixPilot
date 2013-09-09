/*********************************************************************
 *
 *	ARCFOUR Cryptography Library
 *  Library for Microchip TCP/IP Stack
 *	 - Provides encryption and decryption capabilities for the ARCFOUR
 *     algorithm, typically used as a bulk cipher for SSL
 *   - Reference: http://tools.ietf.org/html/draft-kaukonen-cipher-arcfour-01
 *
 *********************************************************************
 * FileName:        ARCFOUR.c
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
 * Elliott Wood	        2/27/07		Original
 ********************************************************************/
#define __ARCFOUR_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)

#include "TCPIP Stack/TCPIP.h"

/*****************************************************************************
  Function:
	void ARCFOURInitialize(ARCFOUR_CTX* ctx, BYTE* key, WORD len)

  Summary:
	Initializes an ARCFOUR encryption stream.

  Description:
	This function initializes an ARCFOUR encryption stream.  Call this 
	function to set up the initial state of the encryption context and the
	S-box.  The stream will be initialized to its zero state with the 
	supplied key.
	
	This function can be used to initialize for encryption and decryption.

  Precondition:
	None

  Parameters:
	ctx - A pointer to the allocated encryption context structure
	key - A pointer to the key to be used
	len - The length of the data in key

  Returns:
	None

  Remarks:
	For security, the key should be destroyed after this call.
  ***************************************************************************/
void ARCFOURInitialize(ARCFOUR_CTX* ctx, BYTE* key, WORD len)
{
	BYTE temp, i, j, *Sbox;

	// Initialize the context indicies
	i = 0;
	j = 0;
	Sbox = ctx->Sbox;
	
	// Initialize each S-box element with its index
	do
	{
		Sbox[i] = i;
		i++;
	} while(i != 0u);

	// Fill in the S-box
	do
	{
		j = j + Sbox[i] + key[i % len];
		temp = Sbox[i];
		Sbox[i] = Sbox[j];
		Sbox[j] = temp;
		i++;
	} while(i != 0u);

	// Reset the context indicies
	ctx->i = 0;
	ctx->j = 0;

}

/*****************************************************************************
  Function:
	void ARCFOURCrypt(ARCFOUR_CTX* ctx, BYTE* data, WORD len)

  Summary:
	Processes an array of data with the ARCFOUR algorithm.

  Description:
	This function uses the current ARCFOUR context to either encrypt or
	decrypt data in place.  The algorithm is the same for both processes, 
	so this function can perform either procedure.

  Precondition:
	The encryption context ctx has been initialized with ARCFOURInitialize.

  Parameters:
	ctx - A pointer to the initialized encryption context structure
	data - The data to be encrypted or decrypted (in place)
	len - The length of data

  Returns:
	None
  ***************************************************************************/
void ARCFOURCrypt(ARCFOUR_CTX* ctx, BYTE* data, WORD len)
{
	BYTE temp, temp2, i, j, *Sbox;

	// Buffer context variables in local RAM for faster access
	i = ctx->i;
	j = ctx->j;
	Sbox = ctx->Sbox;

	// Loop over each byte.  Extract its key and XOR
	while(len--)
	{		
		i++;
		temp = Sbox[i];		
		j += temp;
		temp2 = Sbox[j];
		Sbox[i] = temp2;
		Sbox[j] = temp;
		temp += temp2;
		temp2 = Sbox[temp];

		*data++ ^= temp2;
	}
	
	// Save the new context
	ctx->i = i;
	ctx->j = j;

}

#endif //#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)

