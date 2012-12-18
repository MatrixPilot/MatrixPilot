/*********************************************************************
 
 Helper Functions for Microchip TCPIP Stack
 
 FileName:      Helpers.c
 Dependencies:  See INCLUDES section
 Processor:     PIC18, PIC24, dsPIC, PIC32
 Compiler:      Microchip C18, C30, C32
 Company:       Microchip Technology, Inc.

 Software License Agreement

 Copyright (C) 2002-2011 Microchip Technology Inc.  All rights
 reserved.

 Microchip licenses to you the right to use, modify, copy, and
 distribute:
 (i)  the Software when embedded on a Microchip microcontroller or
      digital signal controller product ("Device") which is
      integrated into Licensee's product; or
 (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
		used in conjunction with a Microchip ethernet controller for
		the sole purpose of interfacing with the ethernet controller.

 You should refer to the license agreement accompanying this
 Software for additional information regarding your rights and
 obligations.

 THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.

 ********************************************************************
 File Description:
 
 Change History:
 
  Rev         Description
  ----------  -------------------------------------------------------
  1.0 - 5.31  Initial release; Rewritten CalcIPChecksum() to avoid
              multi-byte shift operation; Added hexatob(),
              btohexa_high(), and btohexa_low(); Optimized swapl();
              Added leftRotateDWORD()
  5.36        Updated compile time check for ultoa();

 ********************************************************************/
#define __HELPERS_C

#include <stdarg.h>
#include "TCPIP Stack/TCPIP.h"


// Default Random Number Generator seed. 0x41FE9F9E corresponds to calling LFSRSeedRand(1)
static DWORD dwLFSRRandSeed = 0x41FE9F9E;

/*****************************************************************************
  Function:
	DWORD LFSRSeedRand(DWORD dwSeed)

  Summary:
	Seeds the LFSR random number generator invoked by the LFSRRand() function.  
	The prior seed is returned.

  Description:
	Seeds the LFSR random number generator invoked by the LFSRRand() function.  
	The prior seed is returned.

  Precondition:
	None

  Parameters:
	wSeed - The new 32-bit seed value to assign to the LFSR.

  Returns:
  	The last seed in use.  This can be saved and restored by a subsequent call 
	to LFSRSeedRand() if you wish to use LFSRRand() in multiple contexts 
	without disrupting the random number sequence from the alternative 
	context.  For example, if App 1 needs a given sequence of random numbers 
	to perform a test, if you save and restore the seed in App 2, it is 
	possible for App 2 to not disrupt the random number sequence provided to 
	App 1, even if the number of times App 2 calls LFSRRand() varies.
  	
  Side Effects:
	None
	
  Remarks:
	Upon initial power up, the internal seed is initialized to 0x1.  Using a 
	dwSeed value of 0x0 will return the same sequence of random numbers as 
	using the seed of 0x1.
  ***************************************************************************/
DWORD LFSRSeedRand(DWORD dwSeed)
{
	DWORD dwOldSeed;
	BYTE i;

	// Save original seed to be returned later
	dwOldSeed = dwLFSRRandSeed;

	// Ensure zero isn't selected as a seed value, this would result in all 
	// 0x0000 output values from the LFSR
	if(dwSeed == 0u)
		dwSeed = 1;
		
	// Set the new seed
	dwLFSRRandSeed = dwSeed;
	
	// Run the LFSR a few times to get rid of obvious start up artifacts for 
	// seed values that don't have many set bits.
	for(i = 0; i < 16; i++)
		LFSRRand();
	
	// Return saved old seed
	return dwOldSeed;
}

/*****************************************************************************
  Function:
	WORD LFSRRand(void)

  Summary:
	Returns a pseudo-random 16-bit unsigned integer in the range from 0 
	to 65535 (0x0000 to 0xFFFF).

  Description:
	Returns a pseudo-random 16-bit unsigned integer in the range from 0 
	to 65535 (0x0000 to 0xFFFF).  The random number is generated using a 
	Linear Feedback Shift Register (LFSR) type pseudo-random number generator 
	algorithm.  The LFSR can be seeded by calling the LFSRSeedRand() function
	to generate the same sequence of random numbers as a prior string of calls.
	
	The internal LFSR will repeat after 2^32-1 iterations.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	Random 16-bit unsigned integer.
  	
  Side Effects:
	The internal LFSR seed is updated so that the next call to LFSRRand() 
	will return a different random number.
	
  Remarks:
	None
  ***************************************************************************/
WORD LFSRRand(void)
{
	BYTE i;
	
	// Taps: 32 31 29 1
	// Characteristic polynomial: x^32 + x^31 + x^29 + x + 1
	// Repeat 15 times to make the shift pattern less obvious
	for(i = 0; i < 15; i++)
		dwLFSRRandSeed = (dwLFSRRandSeed >> 1) ^ ((0ul - (dwLFSRRandSeed & 1ul)) & 0xD0000001ul);

	// Return 16-bits as pseudo-random number
	return (WORD)dwLFSRRandSeed;
}


/*****************************************************************************
  Function:
	DWORD GenerateRandomDWORD(void)

  Summary:
	Generates a random DWORD.

  Description:
	This function generates a random 32-bit integer.  It collects
	randomness by comparing the A/D converter's internal R/C oscillator
	clock with our main system clock.  By passing collected entropy to the
	LFSRSeedRand()/LFSRRand() functions, the output is normalized (deskewed) 
	in the hopes of meeting statistical randomness tests.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	Random 32-bit number.
  	
  Side Effects:
	This function uses the A/D converter (and so you must disable 
	interrupts if you use the A/D converted in your ISR).  The LFSRRand() 
	function will be reseeded, and Timer0 (PIC18) and Timer1 (PIC24, 
	dsPIC, and PIC32) will be used.  TMR#H:TMR#L will have a new value.
	Note that this is the same timer used by the Tick module.
	
  Remarks:
	This function times out after 1 second of attempting to generate the 
	random DWORD.  In such a case, the output may not be truly random.  
	Typically, this function executes in around 500,000 instruction cycles.
	
	The intent of this function is to produce statistically random and
	cryptographically secure random number.  Whether or not this is true on
	all (or any) devices/voltages/temperatures is not tested.
  ***************************************************************************/
DWORD GenerateRandomDWORD(void)
{
	BYTE vBitCount;
	WORD w, wTime, wLastValue;
	DWORD dwTotalTime;
	union
	{
		DWORD	dw;
		WORD	w[2];
	} randomResult;

#if defined __18CXX	
{
	BYTE ADCON0Save, ADCON2Save;
	BYTE T0CONSave, TMR0HSave, TMR0LSave;

	// Save hardware SFRs
	ADCON0Save = ADCON0;
	ADCON2Save = ADCON2;
	T0CONSave = T0CON;
	TMR0LSave = TMR0L;
	TMR0HSave = TMR0H;

	// Set up Timer and A/D converter module
	ADCON0 = 0x01;	// Turn on the A/D module
	ADCON2 = 0x3F;	// 20 Tad acquisition, Frc A/D clock used for conversion
	T0CON = 0x88;	// TMR0ON = 1, no prescalar
	vBitCount = 0;
	dwTotalTime = 0;
	wLastValue = 0;
	randomResult.dw = LFSRRand();
	while(1)
	{
		// Time the duration of an A/D acquisition and conversion
		TMR0H = 0x00;
		TMR0L = 0x00;
		ADCON0bits.GO = 1;
		ClrWdt();
		while(ADCON0bits.GO);
		((BYTE*)&wTime)[0] = TMR0L;
		((BYTE*)&wTime)[1] = TMR0H;
		w = LFSRRand();
	
		// Wait no longer than 1 second obtaining entropy
		dwTotalTime += wTime;
		if(dwTotalTime >= GetInstructionClock())
		{
			randomResult.w[0] ^= LFSRRand();
			randomResult.w[1] ^= LFSRRand();
			break;
		}
	
		// Keep sampling if minimal entropy was likely obtained this round
		if(wLastValue == wTime)
			continue;
	
		// Add this entropy into the pseudo random number generator by reseeding
		LFSRSeedRand(w + (wLastValue - wTime));
		wLastValue = wTime;
	
		// Accumulate at least 32 bits of randomness over time
		randomResult.dw <<= 1;
		if(LFSRRand() & 0x0080)
			randomResult.w[0] |= 0x1;
	
		// See if we've collected a fair amount of entropy and can quit early
		if(++vBitCount == 0u)
			break;
	}

	// Restore hardware SFRs
	ADCON0 = ADCON0Save;
	ADCON2 = ADCON2Save;
	TMR0H = TMR0HSave;
	TMR0L = TMR0LSave;
	T0CON = T0CONSave;
}
#else
{
	WORD AD1CON1Save, AD1CON2Save, AD1CON3Save;
	WORD T1CONSave, PR1Save;

	// Save hardware SFRs
	AD1CON1Save = AD1CON1;
	AD1CON2Save = AD1CON2;
	AD1CON3Save = AD1CON3;
	T1CONSave = T1CON;
	PR1Save = PR1;

	// Set up Timer and A/D converter module
	AD1CON1 = 0x0000;		// Turn off the ADC so we can write to it
	AD1CON3 = 0x9F00;		// Frc A/D clock, 31 Tad acquisition
	AD1CON2 = 0x003F;		// Interrupt after every 16th sample/convert
	AD1CON1 = 0x80E4;		// Turn on the A/D module, auto-convert
	T1CON = 0x8000;			// TON = 1, no prescalar
	PR1 = 0xFFFF;			// Don't clear timer early
	vBitCount = 0;
	dwTotalTime = 0;
	wLastValue = 0;
	randomResult.dw = LFSRRand();
	while(1)
	{
		ClrWdt();
		#if defined(__C30__)
			while(!IFS0bits.AD1IF);
		#else
			while(!IFS1bits.AD1IF);
		#endif
		wTime = TMR1;
		TMR1 = 0x0000;

		#if defined(__C30__)
			IFS0bits.AD1IF = 0;
		#else
			IFS1CLR = _IFS1_AD1IF_MASK;
		#endif
		w = LFSRRand();
	
		// Wait no longer than 1 second obtaining entropy
		dwTotalTime += wTime;
		if(dwTotalTime >= GetInstructionClock())
		{
			randomResult.w[0] ^= LFSRRand();
			randomResult.w[1] ^= LFSRRand();
			break;
		}
	
		// Keep sampling if minimal entropy was likely obtained this round
		if(wLastValue == wTime)
			continue;
	
		// Add this entropy into the pseudo random number generator by reseeding
		LFSRSeedRand(w + (wLastValue - wTime));
		wLastValue = wTime;
	
		// Accumulate at least 32 bits of randomness over time
		randomResult.dw <<= 1;
		if(LFSRRand() & 0x0080)
			randomResult.w[0] |= 0x1;
	
		// See if we've collected a fair amount of entropy and can quit early
		if(++vBitCount == 0u)
			break;
	}


	// Restore hardware SFRs
	AD1CON1 = 0x0000;		// Turn off the ADC so we can write to it
	AD1CON3 = AD1CON3Save;
	AD1CON2 = AD1CON2Save;
	AD1CON1 = AD1CON1Save;
	T1CON = T1CONSave;
	PR1 = PR1Save;
}
#endif

	return randomResult.dw;
}


#if defined(STACK_USE_HTTP_SERVER)
/*****************************************************************************
  Function:
	void UnencodeURL(BYTE* URL)

  Summary:
	Decodes a URL-encoded string.

  Description:
	This function is deprecated except for use with HTTP Classic.  It
	attempts to decode a URL encoded string, converting all hex escape
	sequences into a literal byte.  However, it is inefficient over long
	strings and does not handle URL-encoded data strings ('&' and '=').

  Precondition:
	None

  Parameters:
	URL - the null-terminated string to decode

  Returns:
  	None
  ***************************************************************************/
void UnencodeURL(BYTE* URL)
{
	BYTE *Right, *Copy;
	WORD_VAL Number;

	while((Right = (BYTE*)strchr((char*)URL, '%')))
	{
		// Make sure the string is long enough
		if(Right[1] == '\0')
			break;
		if(Right[2] == '\0')
			break;

		// Update the string in place
		Number.v[0] = Right[2];
		Number.v[1] = Right[1];
		*Right++ = hexatob(Number);
		URL = Right;

		// Remove two blank spots by shifting all remaining characters right two
		Copy = Right + 2;
		while((*Right++ = *Copy++));
	}
}		    
#endif


/*****************************************************************************
  Function:
	BOOL StringToIPAddress(BYTE* str, IP_ADDR* IPAddress)

  Summary:
	Converts a string to an IP address

  Description:
	This function parses a dotted-quad decimal IP address string into an 
	IP_ADDR struct.  The output result is big-endian.
	
  Precondition:
	None

  Parameters:
	str - Pointer to a dotted-quad IP address string
	IPAddress - Pointer to IP_ADDR in which to store the result

  Return Values:
  	TRUE - an IP address was successfully decoded
  	FALSE - no IP address could be found, or the format was incorrect
  ***************************************************************************/
BOOL StringToIPAddress(BYTE* str, IP_ADDR* IPAddress)
{
	DWORD_VAL dwVal;
	BYTE i, charLen, currentOctet;

	charLen = 0;
	currentOctet = 0;
	dwVal.Val = 0;
	while((i = *str++))
	{
		if(currentOctet > 3u)
			break;

		i -= '0';
		

		// Validate the character is a numerical digit or dot, depending on location
		if(charLen == 0u)
		{
			if(i > 9u)
				return FALSE;
		}
		else if(charLen == 3u)
		{
			if(i != (BYTE)('.' - '0'))
				return FALSE;

			if(dwVal.Val > 0x00020505ul)
				return FALSE;

			IPAddress->v[currentOctet++] = dwVal.v[2]*((BYTE)100) + dwVal.v[1]*((BYTE)10) + dwVal.v[0];
			charLen = 0;
			dwVal.Val = 0;
			continue;
		}
		else
		{
			if(i == (BYTE)('.' - '0'))
			{
				if(dwVal.Val > 0x00020505ul)
					return FALSE;

				IPAddress->v[currentOctet++] = dwVal.v[2]*((BYTE)100) + dwVal.v[1]*((BYTE)10) + dwVal.v[0];
				charLen = 0;
				dwVal.Val = 0;
				continue;
			}
			if(i > 9u)
				return FALSE;
		}

		charLen++;
		dwVal.Val <<= 8;
		dwVal.v[0] = i;
	}

	// Make sure the very last character is a valid termination character 
	// (i.e., not more hostname, which could be legal and not an IP 
	// address as in "10.5.13.233.picsaregood.com"
	if(i != 0u && i != '/' && i != '\r' && i != '\n' && i != ' ' && i != '\t' && i != ':')
		return FALSE;

	// Verify and convert the last octet and return the result
	if(dwVal.Val > 0x00020505ul)
		return FALSE;

	IPAddress->v[3] = dwVal.v[2]*((BYTE)100) + dwVal.v[1]*((BYTE)10) + dwVal.v[0];

	return TRUE;
}

/*****************************************************************************
  Function:
	BOOL ROMStringToIPAddress(ROM BYTE* str, IP_ADDR* IPAddress)

  Summary:
	Converts a string to an IP address

  Description:
	This function parses a dotted-quad decimal IP address string into an 
	IP_ADDR struct.  The output result is big-endian.
	
  Precondition:
	None

  Parameters:
	str - Pointer to a dotted-quad IP address string
	IPAddress - Pointer to IP_ADDR in which to store the result

  Return Values:
  	TRUE - an IP address was successfully decoded
  	FALSE - no IP address could be found, or the format was incorrect
  
  Remarks:
	This function is aliased to StringToIPAddress on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
BOOL ROMStringToIPAddress(ROM BYTE* str, IP_ADDR* IPAddress)
{
	DWORD_VAL dwVal;
	BYTE i, charLen, currentOctet;

	charLen = 0;
	currentOctet = 0;
	dwVal.Val = 0;
	while(i = *str++)
	{
		if(currentOctet > 3u)
			break;

		i -= '0';
		

		// Validate the character is a numerical digit or dot, depending on location
		if(charLen == 0u)
		{
			if(i > 9u)
				return FALSE;
		}
		else if(charLen == 3u)
		{
			if(i != (BYTE)('.' - '0'))
				return FALSE;

			if(dwVal.Val > 0x00020505ul)
				return FALSE;

			IPAddress->v[currentOctet++] = dwVal.v[2]*((BYTE)100) + dwVal.v[1]*((BYTE)10) + dwVal.v[0];
			charLen = 0;
			dwVal.Val = 0;
			continue;
		}
		else
		{
			if(i == (BYTE)('.' - '0'))
			{
				if(dwVal.Val > 0x00020505ul)
					return FALSE;

				IPAddress->v[currentOctet++] = dwVal.v[2]*((BYTE)100) + dwVal.v[1]*((BYTE)10) + dwVal.v[0];
				charLen = 0;
				dwVal.Val = 0;
				continue;
			}
			if(i > 9u)
				return FALSE;
		}

		charLen++;
		dwVal.Val <<= 8;
		dwVal.v[0] = i;
	}

	// Make sure the very last character is a valid termination character 
	// (i.e., not more hostname, which could be legal and not an IP 
	// address as in "10.5.13.233.picsaregood.com"
	if(i != 0u && i != '/' && i != '\r' && i != '\n' && i != ' ' && i != '\t')
		return FALSE;

	// Verify and convert the last octet and return the result
	if(dwVal.Val > 0x00020505ul)
		return FALSE;

	IPAddress->v[3] = dwVal.v[2]*((BYTE)100) + dwVal.v[1]*((BYTE)10) + dwVal.v[0];

	return TRUE;
}
#endif



/*****************************************************************************
  Function:
	WORD Base64Decode(BYTE* cSourceData, WORD wSourceLen, 
						BYTE* cDestData, WORD wDestLen)

  Description:
	Decodes a Base-64 array to its literal representation.
	
  Precondition:
	None

  Parameters:
	cSourceData - Pointer to a string of Base-64 encoded data
	wSourceLen	- Length of the Base-64 source data
	cDestData	- Pointer to write the decoded data
	wSourceLen	- Maximum length that can be written to cDestData

  Returns:
  	Number of decoded bytes written to cDestData.
  
  Remarks:
	This function is binary safe and will ignore invalid characters (CR, LF, 
	etc).  If cSourceData is equal to cDestData, the data will be converted
	in-place.  If cSourceData is not equal to cDestData, but the regions 
	overlap, the behavior is undefined.
	
	Decoded data is always at least 1/4 smaller than the source data.
  ***************************************************************************/
#if defined(STACK_USE_BASE64_DECODE)
WORD Base64Decode(BYTE* cSourceData, WORD wSourceLen, BYTE* cDestData, WORD wDestLen)
{
	BYTE i;
	BYTE vByteNumber;
	WORD wBytesOutput;

	vByteNumber = 0;
	wBytesOutput = 0;

	// Loop over all provided bytes
	while(wSourceLen--)
	{
		// Fetch a Base64 byte and decode it to the original 6 bits
		i = *cSourceData++;
		if(i >= 'A' && i <= 'Z')	// Regular data
			i -= 'A' - 0;
		else if(i >= 'a' && i <= 'z')
			i -= 'a' - 26;
		else if(i >= '0' && i <= '9')
			i -= '0' - 52;
		else if(i == '+' || i == '-')
			i = 62;
		else if(i == '/' || i == '_')
			i = 63;
		else 						// Skip all padding (=) and non-Base64 characters
			continue;


		// Write the 6 bits to the correct destination location(s)
		if(vByteNumber == 0u)
		{
			vByteNumber++;
			if(wBytesOutput >= wDestLen)
				break;
			wBytesOutput++;
			*cDestData = i << 2;
		}
		else if(vByteNumber == 1u)
		{
			vByteNumber++;
			*cDestData++ |= i >> 4;
			if(wBytesOutput >= wDestLen)
				break;
			wBytesOutput++;
			*cDestData = i << 4;
		}
		else if(vByteNumber == 2u)
		{
			vByteNumber++;
			*cDestData++ |= i >> 2;
			if(wBytesOutput >= wDestLen)
				break;
			wBytesOutput++;
			*cDestData = i << 6;
		}
		else
		{
			vByteNumber = 0;
			*cDestData++ |= i;
		}
	}

	return wBytesOutput;
}
#endif	// #if defined(STACK_USE_BASE64_DECODE)


/*****************************************************************************
  Function:
	WORD Base64Encode(BYTE* cSourceData, WORD wSourceLen,
						BYTE* cDestData, WORD wDestLen)

  Description:
	Encodes a binary array to Base-64.
	
  Precondition:
	None

  Parameters:
	cSourceData - Pointer to a string of binary data
	wSourceLen	- Length of the binary source data
	cDestData	- Pointer to write the Base-64 encoded data
	wSourceLen	- Maximum length that can be written to cDestData

  Returns:
  	Number of encoded bytes written to cDestData.  This will always be
  	a multiple of 4.
  
  Remarks:
	Encoding cannot be performed in-place.  If cSourceData overlaps with 
	cDestData, the behavior is undefined.
	
	Encoded data is always at least 1/3 larger than the source data.  It may
	be 1 or 2 bytes larger than that.
  ***************************************************************************/
#if defined(STACK_USE_BASE64_ENCODE) || defined(STACK_USE_SMTP_CLIENT) || defined(STACK_USE_DYNAMICDNS_CLIENT)
WORD Base64Encode(BYTE* cSourceData, WORD wSourceLen, BYTE* cDestData, WORD wDestLen)
{
	BYTE i, j;
	BYTE vOutput[4];
	WORD wOutputLen;

	wOutputLen = 0;
	while(wDestLen >= 4u)
	{
		// Start out treating the output as all padding
		vOutput[0] = 0xFF;
		vOutput[1] = 0xFF;
		vOutput[2] = 0xFF;
		vOutput[3] = 0xFF;

		// Get 3 input octets and split them into 4 output hextets (6-bits each) 
		if(wSourceLen == 0u)
			break;
		i = *cSourceData++;
		wSourceLen--;
		vOutput[0] = (i & 0xFC)>>2;
		vOutput[1] = (i & 0x03)<<4;
		if(wSourceLen)
		{
			i = *cSourceData++;
			wSourceLen--;
			vOutput[1] |= (i & 0xF0)>>4;
			vOutput[2] = (i & 0x0F)<<2;
			if(wSourceLen)
			{
				i = *cSourceData++;
				wSourceLen--;
				vOutput[2] |= (i & 0xC0)>>6;
				vOutput[3] = i & 0x3F;
			}
		}
	
		// Convert hextets into Base 64 alphabet and store result
		for(i = 0; i < 4u; i++)
		{
			j = vOutput[i];

			if(j <= 25u)
				j += 'A' - 0;
			else if(j <= 51u)
				j += 'a' - 26;
			else if(j <= 61u)
				j += '0' - 52;
			else if(j == 62u)
				j = '+';
			else if(j == 63u)
				j = '/';
			else				// Padding
				j = '=';

			*cDestData++ = j;
		}

		// Update counters
		wDestLen -= 4;
		wOutputLen += 4;
	}

	return wOutputLen;
}
#endif // #if defined(STACK_USE_BASE64_ENCODE) || defined(STACK_USE_SMTP) || defined(STACK_USE_DYNAMICDNS_CLIENT)


/*****************************************************************************
  Function:
	void uitoa(WORD Value, BYTE* Buffer)

  Summary:
	Converts an unsigned integer to a decimal string.
	
  Description:
	Converts a 16-bit unsigned integer to a null-terminated decimal string.
	
  Precondition:
	None

  Parameters:
	Value	- The number to be converted
	Buffer	- Pointer in which to store the converted string

  Returns:
  	None
  ***************************************************************************/
void uitoa(WORD Value, BYTE* Buffer)
{
	BYTE i;
	WORD Digit;
	WORD Divisor;
	BOOL Printed = FALSE;

	if(Value)
	{
		for(i = 0, Divisor = 10000; i < 5u; i++)
		{
			Digit = Value/Divisor;
			if(Digit || Printed)
			{
				*Buffer++ = '0' + Digit;
				Value -= Digit*Divisor;
				Printed = TRUE;
			}
			Divisor /= 10;
		}
	}
	else
	{
		*Buffer++ = '0';
	}

	*Buffer = '\0';
}			    

/*****************************************************************************
  Function:
	void ultoa(DWORD Value, BYTE* Buffer)

  Summary:
	Converts an unsigned integer to a decimal string.
	
  Description:
	Converts a 32-bit unsigned integer to a null-terminated decimal string.
	
  Precondition:
	None

  Parameters:
	Value	- The number to be converted
	Buffer	- Pointer in which to store the converted string

  Returns:
  	None
  ***************************************************************************/
// HI-TECH PICC-18 PRO 9.63, C30 v3.25, and C32 v1.12 already have a ultoa() library function
// C18 already has a ultoa() function that more-or-less matches this one
// C32 < 1.12 and C30 < v3.25 need this function
#if (defined(__PIC32MX__) && (__C32_VERSION__ < 112)) || (defined (__C30__) && (__C30_VERSION__ < 325)) || defined(__C30_LEGACY_LIBC__) || defined(__C32_LEGACY_LIBC__)
void ultoa(DWORD Value, BYTE* Buffer)
{
	BYTE i;
	DWORD Digit;
	DWORD Divisor;
	BOOL Printed = FALSE;

	if(Value)
	{
		for(i = 0, Divisor = 1000000000; i < 10; i++)
		{
			Digit = Value/Divisor;
			if(Digit || Printed)
			{
				*Buffer++ = '0' + Digit;
				Value -= Digit*Divisor;
				Printed = TRUE;
			}
			Divisor /= 10;
		}
	}
	else
	{
		*Buffer++ = '0';
	}

	*Buffer = '\0';
}
#endif

/*****************************************************************************
  Function:
	BYTE hexatob(WORD_VAL AsciiChars)

  Summary:
	Converts a hex string to a single byte.
	
  Description:
	Converts a two-character ASCII hex string to a single packed byte.
	
  Precondition:
	None

  Parameters:
	AsciiChars - WORD_VAL where .v[0] is the ASCII value for the lower nibble
					and .v[1] is the ASCII value for the upper nibble.  Each
					must range from '0'-'9', 'A'-'F', or 'a'-'f'.

  Returns:
  	Resulting packed byte 0x00 - 0xFF.
  ***************************************************************************/
BYTE hexatob(WORD_VAL AsciiChars)
{
	// Convert lowercase to uppercase
	if(AsciiChars.v[1] > 'F')
		AsciiChars.v[1] -= 'a'-'A';
	if(AsciiChars.v[0] > 'F')
		AsciiChars.v[0] -= 'a'-'A';

	// Convert 0-9, A-F to 0x0-0xF
	if(AsciiChars.v[1] > '9')
		AsciiChars.v[1] -= 'A' - 10;
	else
		AsciiChars.v[1] -= '0';

	if(AsciiChars.v[0] > '9')
		AsciiChars.v[0] -= 'A' - 10;
	else
		AsciiChars.v[0] -= '0';

	// Concatenate
	return (AsciiChars.v[1]<<4) |  AsciiChars.v[0];
}

/*****************************************************************************
  Function:
	BYTE btohexa_high(BYTE b)

  Summary:
	Converts the upper nibble of a binary value to a hexadecimal ASCII byte.

  Description:
	Converts the upper nibble of a binary value to a hexadecimal ASCII byte.
	For example, btohexa_high(0xAE) will return 'A'.

  Precondition:
	None

  Parameters:
	b - the byte to convert

  Returns:
  	The upper hexadecimal ASCII byte '0'-'9' or 'A'-'F'.
  ***************************************************************************/
BYTE btohexa_high(BYTE b)
{
	b >>= 4;
	return (b>0x9u) ? b+'A'-10:b+'0';
}

/*****************************************************************************
  Function:
	BYTE btohexa_high(BYTE b)

  Summary:
	Converts the lower nibble of a binary value to a hexadecimal ASCII byte.

  Description:
	Converts the lower nibble of a binary value to a hexadecimal ASCII byte.
	For example, btohexa_high(0xAE) will return 'E'.

  Precondition:
	None

  Parameters:
	b - the byte to convert

  Returns:
  	The lower hexadecimal ASCII byte '0'-'9' or 'A'-'F'.
  ***************************************************************************/
BYTE btohexa_low(BYTE b)
{
	b &= 0x0F;
	return (b>9u) ? b+'A'-10:b+'0';
}

/*****************************************************************************
  Function:
	signed char stricmppgm2ram(BYTE* a, ROM BYTE* b)

  Summary:
	Case-insensitive comparison of a string in RAM to a string in ROM.

  Description:
	Performs a case-insensitive comparison of a string in RAM to a string
	in ROM.  This function performs identically to strcmppgm2ram, except that
	the comparison is not case-sensitive.

  Precondition:
	None

  Parameters:
	a - Pinter to tring in RAM
	b - Pointer to string in ROM

  Return Values:
  	\-1 - a < b
  	0	- a = b
  	1	- a > b
  ***************************************************************************/
signed char stricmppgm2ram(BYTE* a, ROM BYTE* b)
{
	BYTE cA, cB;
	
	// Load first two characters
	cA = *a;
	cB = *b;
	
	// Loop until one string terminates
	while(cA != '\0' && cB != '\0')
	{
		// Shift case if necessary
		if(cA >= 'a' && cA <= 'z')
			cA -= 'a' - 'A';
		if(cB >= 'a' && cB <= 'z')
			cB -= 'a' - 'A';
			
		// Compare
		if(cA > cB)
			return 1;
		if(cA < cB)
			return -1;
		
		// Characters matched, so continue
		a++;
		b++;
		cA = *a;
		cB = *b;
	}
	
	// See if one string terminated first
	if(cA > cB)
		return 1;
	if(cA < cB)
		return -1;
		
	// Strings match
	return 0;
}

/*****************************************************************************
  Function:
	WORD swaps(WORD v)

  Description:
	Swaps the endian-ness of a WORD.

  Precondition:
	None

  Parameters:
	v - the WORD to swap

  Returns:
	The swapped version of v.
  ***************************************************************************/
WORD swaps(WORD v)
{
	WORD_VAL t;
	BYTE b;

	t.Val   = v;
	b       = t.v[1];
	t.v[1]  = t.v[0];
	t.v[0]  = b;

	return t.Val;
}

/*****************************************************************************
  Function:
	DWORD swapl(DWORD v)

  Description:
	Swaps the endian-ness of a DWORD.

  Precondition:
	None

  Parameters:
	v - the DWORD to swap

  Returns:
	The swapped version of v.
  ***************************************************************************/
#if defined(__C32__)
DWORD   __attribute__((nomips16)) swapl(DWORD v)
#else
DWORD swapl(DWORD v)
#endif
{
	// Swap bytes 0 and 3
	((DWORD_VAL*)&v)->v[0] ^= ((DWORD_VAL*)&v)->v[3];
	((DWORD_VAL*)&v)->v[3] ^= ((DWORD_VAL*)&v)->v[0];
	((DWORD_VAL*)&v)->v[0] ^= ((DWORD_VAL*)&v)->v[3];

	// Swap bytes 1 and 2
	((DWORD_VAL*)&v)->v[1] ^= ((DWORD_VAL*)&v)->v[2];
	((DWORD_VAL*)&v)->v[2] ^= ((DWORD_VAL*)&v)->v[1];
	((DWORD_VAL*)&v)->v[1] ^= ((DWORD_VAL*)&v)->v[2];

	return v;
}


/*****************************************************************************
  Function:
	WORD CalcIPChecksum(BYTE* buffer, WORD count)

  Summary:
	Calculates an IP checksum value.

  Description:
	This function calculates an IP checksum over an array of input data.  The
	checksum is the 16-bit one's complement of one's complement sum of all 
	words in the data (with zero-padding if an odd number of bytes are 
	summed).  This checksum is defined in RFC 793.

  Precondition:
	buffer is WORD aligned (even memory address) on 16- and 32-bit PICs.

  Parameters:
	buffer - pointer to the data to be checksummed
	count  - number of bytes to be checksummed

  Returns:
	The calculated checksum.
	
  Internal:
	This function could be improved to do 32-bit sums on PIC32 platforms.
  ***************************************************************************/
WORD CalcIPChecksum(BYTE* buffer, WORD count)
{
	WORD i;
	WORD *val;
	union
	{
		WORD w[2];
		DWORD dw;
	} sum;

	i = count >> 1;
	val = (WORD*)buffer;

	// Calculate the sum of all words
	sum.dw = 0x00000000ul;
	while(i--)
		sum.dw += (DWORD)*val++;

	// Add in the sum of the remaining byte, if present
	if(count & 0x1)
		sum.dw += (DWORD)*(BYTE*)val;

	// Do an end-around carry (one's complement arrithmatic)
	sum.dw = (DWORD)sum.w[0] + (DWORD)sum.w[1];

	// Do another end-around carry in case if the prior add 
	// caused a carry out
	sum.w[0] += sum.w[1];

	// Return the resulting checksum
	return ~sum.w[0];
}


/*****************************************************************************
  Function:
	char* strupr(char* s)

  Summary:
	Converts a string to uppercase.

  Description:
	This function converts strings to uppercase on platforms that do not
	already have this function defined.  All lower-case characters are
	converted, an characters not included in 'a'-'z' are left as-is.

  Precondition:
	None

  Parameters:
	s - the null-terminated string to be converted.

  Returns:
	Pointer to the initial string.
  ***************************************************************************/
#if !defined(__18CXX) || defined(HI_TECH_C)
char* strupr(char* s)
{
	char c;
	char *t;

	t = s;
	while( (c = *t) )
	{
		if(c >= 'a' && c <= 'z')
		{
			*t -= ('a' - 'A');
		}
		t++;
	}
	return s;
}
#endif

#if defined(__18CXX)
// Make this variable global for the following function.
// Hi-Tech PICC18 cannot access local function variables from inline asm.
DWORD_VAL toRotate; 
#endif

/*****************************************************************************
  Function:
	DWORD leftRotateDWORD(DWORD val, BYTE bits)

  Summary:
	Left-rotates a DWORD.

  Description:
	This function rotates the bits in a 32-bit DWORD left by a specific 
	number of bits.

  Precondition:
	None

  Parameters:
	val		- the DWORD to be rotated
	bits	- the number of bits by which to shift

  Returns:
	Rotated DWORD value.
	
  Remarks:
	This function is only implemented on 8-bit platforms for now.  The 
	8-bit compilers generate excessive code for this function, while C30
	and C32 already generate compact code.  Those compilers are served
	by a macro defined in Helpers.h.
  ***************************************************************************/
#if defined(__18CXX)
DWORD leftRotateDWORD(DWORD val, BYTE bits)
{
	BYTE i, t;
	//DWORD_VAL toRotate;
	toRotate.Val = val;
	
	for(i = bits; i >= 8u; i -= 8)
	{
		t = toRotate.v[3];
		toRotate.v[3] = toRotate.v[2];
		toRotate.v[2] = toRotate.v[1];
		toRotate.v[1] = toRotate.v[0];
		toRotate.v[0] = t;
	}
	
	
	#if defined(HI_TECH_C)
	for(; i != 0; i--)
	{
		asm("movlb (_toRotate)>>8");
		//asm("bcf _STATUS,0,C");
		asm("bcf 0xFD8,0,C");		// HI-TECH PICC-18 PRO 9.63PL1 doesn't define _STATUS
		asm("btfsc (_toRotate)&0ffh+3,7,B");
		//asm("bsf _STATUS,0,C");
		asm("bsf 0xFD8,0,C");		// HI-TECH PICC-18 PRO 9.63PL1 doesn't define _STATUS
		asm("rlcf (_toRotate)&0ffh+0,F,B");
		asm("rlcf (_toRotate)&0ffh+1,F,B");
		asm("rlcf (_toRotate)&0ffh+2,F,B");
		asm("rlcf (_toRotate)&0ffh+3,F,B");
	}
	#else
	for(; i != 0u; i--)
	{
		_asm
		movlb toRotate
		bcf STATUS,0,0
		btfsc toRotate+3,7,1
		bsf STATUS,0,0
		rlcf toRotate+0,1,1
		rlcf toRotate+1,1,1
		rlcf toRotate+2,1,1
		rlcf toRotate+3,1,1
		_endasm
	}
	#endif
	
	return toRotate.Val;
}
#endif

/*****************************************************************************
  Function:
	void FormatNetBIOSName(BYTE Name[])

  Summary:
	Formats a string to a valid NetBIOS name.

  Description:
	This function formats a string to a valid NetBIOS name.  Names will be
	exactly 16 characters, as defined by the NetBIOS spec.  The 16th 
	character will be a 0x00 byte, while the other 15 will be the 
	provided string, padded with spaces as necessary.

  Precondition:
	None

  Parameters:
	Name - the string to format as a NetBIOS name.  This parameter must have
	  at least 16 bytes allocated.

  Returns:
	None
  ***************************************************************************/
void FormatNetBIOSName(BYTE Name[])
{
	BYTE i;

	Name[15] = '\0';
	strupr((char*)Name);
	i = 0;
	while(i < 15u)
	{
		if(Name[i] == '\0')
		{
			while(i < 15u)
			{
				Name[i++] = ' ';
			}
			break;
		}
		i++;
	}
}

/*****************************************************************************
  Function:
	char * strnchr(const char *searchString, size_t count, char c)

  Summary:
	Searches a string up to a specified number of characters for a specific 
	character.

  Description:
	Searches a string up to a specified number of characters for a specific 
	character.  The string is searched forward and the first occurance 
	location is returned.  If the search character is not present in the 
	string, or if the maximum character count is reached first, then a NULL 
	pointer is returned.

  Precondition:
	None

  Parameters:
	searchString - Pointer to a null terminated string to search.  If count is 
		less than the string size, then the string need not be null terminated.
	count - Maximum number of characters to search before aborting.
	c - Character to search for
	
  Returns:
	Pointer to the first occurance of the character c in the string 
	searchString.  If the character is not found or the maximum count is 
	reached, a NULL pointer is returned.
  ***************************************************************************/
char * strnchr(const char *searchString, size_t count, char c)
{
	char c2;
	
	while(count--)
	{
		c2  = *searchString++;
		if(c2 == 0u)
			return NULL;
		if(c2 == c)
			return (char*)--searchString;
	}
	return NULL;
}


/*****************************************************************************
  Function:
	char* strncpy_m(char* destStr, size_t destSize, int nStrings, ...)

  Summary:
	Copies multiple strings to a destination

  Description:
	Copies multiple strings to a destination
    but doesn't copy more than destSize characters.
    Useful where the destination is actually an array and an extra \0
    won't be appended to overflow the buffer
    
  Precondition:
	- valid string pointers
    - destSize should be > 0

  Parameters:
	destStr - Pointer to a string to be initialized with the multiple strings provided as arguments.

    destSize    - the maximum size of the destStr field, that cannot be exceeded.
                  An \0 won't be appended if the resulting size is > destSize

    nStrings    - number of string parameters to be copied into destStr

    ...         - variable number of arguments
    
	
  Returns:
	Length of the destination string, terminating \0 (if exists) not included
  ***************************************************************************/
size_t strncpy_m(char* destStr, size_t destSize, int nStrings, ...)
{
    va_list     args;
    const char* str;
    char*       end;
    size_t      len;

    destStr[0] = '\0';
    end = destStr + destSize - 1;
    *end = '\0';
    len = 0;
    
    va_start( args, nStrings );
    
    while(nStrings--)
    {
        if(*end)
        {   // if already full don't calculate strlen outside the string area
            len = destSize;
            break;
        }
        
        str = va_arg(args, const char*);
        strncpy(destStr + len, str, destSize - len);
        len += strlen(str);
    }

    va_end( args );
    
    return len;
}


/*****************************************************************************
  Function:
	BYTE ExtractURLFields(BYTE *vURL, 
						  PROTOCOLS *protocol, 
						  BYTE *vUsername, WORD *wUsernameLen, 
						  BYTE *vPassword, WORD *wPasswordLen, 
						  BYTE *vHostname, WORD *wHostnameLen, 
						  WORD *wPort, 
						  BYTE *vFilePath, WORD *wFilePathLen)

  Summary:
	Extracts all parameters from an URL string (ex: 
	"http://admin:passwd@www.microchip.com:8080/myfile.gif" is split into 
	{PROTOCOL_HTTP, "admin", "passwd", "www.microchip.com", 8080, "/myfile.gif"}.

  Description:
	Extracts all parameters from an URL string (ex: 
	"http://admin:passwd@www.microchip.com:8080/myfile.gif" is split into 
	{PROTOCOL_HTTP, "admin", "passwd", "www.microchip.com", 8080, "/myfile.gif"}.
	
	The URL string can be null terminated, or alternatively could be terminated 
	by a carriage return or line feed.
	
	If the protocol is unrecognized or the protocol is recognized but the URL 
	is malformed, than an error is safely returned.  For more information on 
	URL/URI interpretation see RFC 2396.

  Precondition:
	This function is commented out by default to save code space because 
	it is not used by any current stack features.  However, if you want to use 
	it, go ahead and uncomment it.  It has been tested, so it (should) work 
	correctly.

  Parameters:
	vURL -	Pointer to null terminated URL to decode and extract from.  This 
		parameter is required and needs to have the minimum RFC 1738 components 
		in it (protocol and hostname).
		
	protocol - Optional pointer to a PROTOCOLS enum to retrieve the decoded 
		protocol type.  If this parameter is unneeded, specify a NULL pointer.  
		The protocol is a required part of the URL, so it must always be 
		present.  The protocol also determines what scheme all other parameters 
		are decoded using, so the function will fail if an unrecognized 
		protocol is provided.  The PROTOCOLS enum members show all of the 
		currently supported protocols for this function.
		
		<p>For the example URL provided in the function description, 
		PROTOCOL_HTTP would be returned for this field.
		
	vUsername - Optional pointer to a buffer to write the decoded username 
		portion of the URL.  If the URL does not contain a username or a NULL 
		pointer is supplied, then this field is ignored.

		<p>For the example URL provided in the function description, "admin" 
		would be returned for this field.
		
	wUsernameLen -
		On call\: Optional pointer to a WORD specifying the maximum length of 
		the vUsername buffer, including the null terminator character.
		
		<p>Upon return\: If wUsernameLen and vUsername are non-NULL, the 
		*wUsernameLen WORD is updated with the actual number of characters 
		written to the vUsername buffer, including the null terminator 
		character.  If vUsername is NULL but wUsernameLen is non-NULL, then no 
		characters are copied, but *wUsernameLen will return the number of 
		characters required to fit the full username string.  If wUsernameLen 
		is NULL, then the username field in the URL, if present, is ignored and 
		the vUsername pointer is not used.
		
		<p>If zero characters were written, this indicates that the URL did not 
		contain a username field.  If one character was written, this indicates 
		that a username field was present, but was a zero character string 
		(ex\: "").
		 
		<p>For the example URL provided in the function description, 6 (0x0006) 
		would be returned for this field.
		
	vPassword - Optional pointer to a buffer to write the decoded password 
		portion of the URL.  If the URL does not contain a password or a NULL 
		pointer is supplied, then this field is ignored.

		<p>For the example URL provided in the function description, "passwd" 
		would be returned for this field.
		
	wPasswordLen -
		On call\: Optional pointer to a WORD specifying the maximum length of 
		the vPassword buffer, including the null terminator character.
		
		<p>Upon return\: If wPasswordLen and vPassword are non-NULL, the 
		*wPasswordLen WORD is updated with the actual number of characters 
		written to the vPassword buffer, including the null terminator 
		character.  If vPassword is NULL but wPasswordLen is non-NULL, then no 
		characters are copied, but *wPasswordLen will return the number of 
		characters required to fit the full password string.  If wPasswordLen 
		is NULL, then the password field in the URL, if present, is ignored and 
		the vPassword pointer is not used.
		
		<p>If zero characters were written, this indicates that the URL did not 
		contain a password field.  If one character was written, this indicates 
		that a password field was present, but was a zero character string 
		(ex\: "").
		 
		<p>For the example URL provided in the function description, 7 (0x0007) 
		would be returned for this field.
		
	vHostname - Optional pointer to a buffer to write the decoded hostname 
		portion of the URL.  All Internet URLs must contain a hostname or IP 
		address, however, if a NULL pointer is supplied, then this field is 
		ignored.

		<p>For the example URL provided in the function description, 
		"www.microchip.com" would be returned for this field.  If the URL was 
		"http://192.168.0.1", then this field would be returned as 
		"192.168.0.1".	The IP address would not be decoded to a DWORD (use the 
		StringToIPAddress() helper function to do this).
		
	wHostnameLen -
		On call\: Optional pointer to a WORD specifying the maximum length of 
		the vHostname buffer, including the null terminator character.
		
		<p>Upon return\: If wHostnameLen and vHostname are non-NULL, the 
		*wHostnameLen WORD is updated with the actual number of characters 
		written to the vHostname buffer, including the null terminator 
		character.  If vHostname is NULL but wHostnameLen is non-NULL, then no 
		characters are copied, but *wHostnameLen will return the number of 
		characters required to fit the full hostname string.  If wHostnameLen 
		is NULL, then the hostname field in the URL, is ignored and the 
		vHostname pointer is not used.
		
		<p>For the example URL provided in the function description, 
		18 (0x0012) would be returned for this field.  If the URL was 
		"http://192.168.0.1", then this field would be returned as 12 (0x000C).
		
	wPort - Optional pointer to a WORD specifying the TCP or UDP port that the 
		server is listening on.  If the port field is absent from the URL, then 
		this parameter will specify the default port for the protocol.  For 
		example, "http://www.microchip.com" would result in 80 being return as 
		the specified port.
		 
		<p>If the wPort pointer is NULL, then the port field in the URL 
		is ignored, if present.
		
	vFilePath - Optional pointer to a buffer to write the decoded file path 
		portion of the URL.  If a NULL pointer is supplied, then this field is 
		ignored.  If a file path is not present in the URL, then "/" will be 
		returned in this field.  

		<p>For the example URL provided in the function description, 
		"/myfile.gif" would be returned for this field.
		
	wFilePathLen -
		On call\: Optional pointer to a WORD specifying the maximum length of 
		the vFilePath buffer, including the null terminator character.
		
		<p>Upon return\: If wFilePathLen and vFilePath are non-NULL, the 
		*wFilePathLen WORD is updated with the actual number of characters 
		written to the vFilePath buffer, including the null terminator 
		character.  If vFilePath is NULL but wFilePathLen is non-NULL, then no 
		characters are copied, but *wFilePathLen will return the number of 
		characters required to fit the full file path string.  If wFilePathLen 
		is NULL, then the file path field in the URL, if present, is ignored and 
		the vFilePath pointer is not used.
		
		<p>This function always returns "/" if no file path is present, so
		*wFilePathLen will also be at least 2 characters ('/' and null 
		terminator) if the pointer is non-NULL.
	
		<p>For the example URL provided in the function description, 12 (0x000C) 
		would be returned for this field.
		
  Returns:
	Zero on success.  Nonzero indicates an error code.  If a nonzero error code 
	is returned, none of the returned buffers or pointer values should be 
	treated as valid, but some of them may have been written to.  The following 
	are all possible return values.
	<table>
		0   No error
		1   Protocol unknown (additional code needs to be added to 
			 ExtractURLFields() and the PROTOCOLS enum needs to be updated if 
			 you want to decode URLs of this protocol type.
		2   URL malformed. Illegal or unknown URL format encountered.
		3   Buffer too small.  One of the input buffer sizes is too small to 
			 contain the URL parameter.
	</table>
  ***************************************************************************/
#if 0	
BYTE ExtractURLFields(BYTE *vURL, PROTOCOLS *protocol, BYTE *vUsername, WORD *wUsernameLen, BYTE *vPassword, WORD *wPasswordLen, BYTE *vHostname, WORD *wHostnameLen, WORD *wPort, BYTE *vFilePath, WORD *wFilePathLen)
{
	// These two arrays must exactly match up each other and the PROTOCOLS enum 
	// elements.  The protocol name strings must also be specified in all 
	// lowercase.
	static ROM char * ROM	vProtocolNames[] = {"http", "https", "mms", "rtsp"};
	static ROM WORD 		wProtocolPorts[] = { 80,     443,     1755,  554};
	WORD w, w2;
	BYTE i, j;
	PROTOCOLS prot;
	BYTE *temp, *temp2;
	WORD wURLLen;
	WORD wLocalPort;
	
	
	// Calculate how long this URL is
	wURLLen = strlen((char*)vURL);
	temp = (BYTE*)strnchr((char*)vURL, wURLLen, '\r');
	if(temp)
		wURLLen = temp - vURL;
	temp = (BYTE*)strnchr((char*)vURL, wURLLen, '\n');
	if(temp)
		wURLLen = temp - vURL;
	

	// Parse starting protocol field
	// Find out how long the protocol name field is
	temp = (BYTE*)strnchr((char*)vURL, wURLLen, ':');
	if(temp == NULL)
		return 2;
	
	// Search protocol list to see if this is a recognized protocol
	for(prot = 0; (BYTE)prot < sizeof(wProtocolPorts)/sizeof(wProtocolPorts[0]); prot++)
	{
		w = strlenpgm(vProtocolNames[prot]);
		if((WORD)(temp - vURL) == w)
		{
			w2 = 0;
			temp2 = vURL;
			while(w)
			{
				i = *temp2++;
				if((i >= 'A') && (i <= 'Z'))
					i += 'a' - 'A';
				if(i != (BYTE)vProtocolNames[prot][w2++])
					break;
				w--;
			}
			if(w == 0u)
			{
				if(protocol)
					*protocol = prot;
				break;
			}
		}
	}

	// If we've search the whole list and didn't find a match, then 
	// this protocol is unknown and this URL cannot be parsed.
	if((BYTE)prot >= sizeof(wProtocolPorts)/sizeof(wProtocolPorts[0]))
		return 1;
	
	w = temp - vURL + 1;
	vURL += w;
	wURLLen -= w;

	// Protocols using the authority field all must have a double 
	// slash "//" prefix
	if(wURLLen < 2u)
		return 2;
	for(j = 0; j < 2u; j++)
	{
		i = *vURL++;
		if(i != '/')
			return 2;
	}
	wURLLen -= 2;
	

	// Parse username and password fields
	// See if there is a @ sign, indicating that there is at 
	// least a username and possibly a password in this URL
	temp = (BYTE*)strnchr((char*)vURL, wURLLen, '@');
	if(temp == NULL)
	{
		if(wUsernameLen)
			*wUsernameLen = 0;
		if(wPasswordLen)
			*wPasswordLen = 0;
	}
	else
	{
		// If we get down here, there is a user name present, let's 
		// see if a password is also present by searching for a 
		// colon between the current string position and the @ 
		// symbol.
		temp2 = (BYTE*)strnchr((char*)vURL, temp - vURL, ':');
		
		// Calculate username length and password length, including 
		// null terminator (if the field exists)
		if(temp2 == NULL)
		{
			w = temp - vURL + 1;	// Username
			w2 = 0;					// Password
		}
		else
		{
			w = temp2 - vURL + 1;	// Username
			w2 = temp - temp2;		// Password
		}
		
		if(wUsernameLen)
		{
			if(vUsername)
			{
				if(*wUsernameLen < w)
					return 3;
				memcpy((void*)vUsername, (void*)vURL, w - 1);
				vUsername[w-1] = 0;
			}
			*wUsernameLen = w;
		}
	
		if(wPasswordLen)
		{
			if(vPassword)
			{
				if(*wPasswordLen < w2)
					return 3;
				if(w2)
				{
					memcpy((void*)vPassword, (void*)temp2+1, w2 - 1);
					vPassword[w2-1] = 0;
				}
			}
			*wPasswordLen = w2;
		}
	
		vURL += w;
		wURLLen -= w;
		if(w2)
		{
			vURL += w2;
			wURLLen -= w2;
		}
	}


	// Parse hostname field
	// Find the length of the hostname, including NULL 
	// terminator
	temp = (BYTE*)strnchr((char*)vURL, wURLLen, ':');
	temp2 = (BYTE*)strnchr((char*)vURL, wURLLen, '/');
	if(temp && temp2)
	{
		if(temp > temp2)
			temp = NULL;
	}
	if(temp == NULL)
	{
		temp = temp2;
		if(temp2 == NULL)
			temp = vURL + wURLLen;
	}
	w = temp - vURL + 1;
	if(wHostnameLen)
	{
		if(vHostname)
		{
			if(*wHostnameLen < w)
				return 3;
			memcpy((void*)vHostname, (void*)vURL, w - 1);
			vHostname[w-1] = 0;
		}
		*wHostnameLen = w;
	}
	vURL += w - 1;
	wURLLen -= w - 1;


	// Parse port field
	if(*vURL == ':')
	{
		vURL++;
		wURLLen--;
		wLocalPort = 0;
		w = wURLLen;
		temp = (BYTE*)strnchr((char*)vURL, wURLLen, '/');
		if(temp != NULL)
			w = temp - vURL;
		w2 = w;
		if(wPort)
		{
			while(w--)
			{
				wLocalPort *= 10;
				wLocalPort += *vURL++ - '0';
			}
			*wPort = wLocalPort;
		}
		else
			vURL += w2;
		wURLLen -= w2;
	}
	else if(wPort)
		*wPort = wProtocolPorts[prot];


	// Parse file path field
	if(wFilePathLen)
	{
		w = ++wURLLen;
		if(wURLLen == 1u)
			w = 2;
		if(vFilePath)
		{
			if(*wFilePathLen < w)
				return 3;
			if(wURLLen == 1u)
				vFilePath[0] = '/';
			else
				memcpy((void*)vFilePath, (void*)vURL, wURLLen - 1);
			vFilePath[w - 1] = 0;
			*wFilePathLen = w;
			return 0;
		}
		*wFilePathLen = w;
	}
	return 0;
}
#endif


/*****************************************************************************
  Function:
	SHORT Replace(BYTE *vExpression, ROM BYTE *vFind, ROM BYTE *vReplacement, 
				  WORD wMaxLen, BOOL bSearchCaseInsensitive)

  Summary:
	Replaces all instances of a particular substring with a new string

  Description:
	Searches a string (vExpression) and replaces all instances of a particular 
	substring (vFind) with a new string (vReplacement).  The start offset to 
	being searching and a maximum number of replacements can be specified.  The 
	search can be performed in a case sensitive or case insensitive manner.

  Precondition:
	This function is commented out by default to save code space because 
	it is not used by any current stack features.  However, if you want to use 
	it, go ahead and uncomment it.  It has been tested, so it (should) work 
	correctly.

  Parameters:
	vExpression - Null terminated string to search and make replacements within.
	vFind - Null terminated string to search for.
	vReplacement - Null terminated string to replace all instances of vFind with.
	wMaxLen - Maximum length of the output vExpression string if string 
		expansion is going to occur (replacement length is longer than find 
		length).  If the replacements will cause this maximum string length to 
		be exceeded, then no replacements will be made and a negative result 
		will be returned, indicating failure.  If the replacement length is 
		shorter or equal to the search length, then this parameter is ignored.
	bSearchCaseInsensitive - Boolean indicating if the search should be 
		performed in a case insensitive manner.  Specify TRUE for case 
		insensitive searches (slower) or FALSE for case sensitive 
		searching (faster).

  Remarks:
	If the replacement string length is shorter than or equal to the search 
	string length and the search string occurs in multiple overlapping 
	locations (ex\: expression is "aaa", find is "aa", and replacement is "bb") 
	then the first find match occuring when searching from left to right will 
	be replaced.  (ex\: output expression will be "bba").
	
	However, if the replacement string length is longer than the search string 
	length, the search will occur starting from the end of the string and 
	proceed to the beginning (right to left searching).  In this case if the 
	expression was "aaa", find was "aa", and replacement was "bbb", then the 
	final output expression will be "abbb".  

  Returns:
	If zero or greater, indicates the count of how many replacements were made.  
	If less than zero (negative result), indicates that wMaxLen was too small 
	to make the necessary replacements.  In this case, no replacements were 
	made.
  ***************************************************************************/
#if 0
SHORT Replace(BYTE *vExpression, ROM BYTE *vFind, ROM BYTE *vReplacement, WORD wMaxLen, BOOL bSearchCaseInsensitive)
{
	WORD wExpressionLen, wFindLen, wFindLenMinusOne, wReplacementLen;
	WORD wFindCount, wReplacementsLeft;
	BYTE i, j;
	BYTE vFirstFindChar;
	WORD wBytesLeft;
	BYTE *vDest;
	BYTE *vExpressionCompare;
	ROM BYTE *vFindCompare;
	WORD w;

	wFindLen = strlenpgm((ROM char*)vFind);
	if(wFindLen == 0u)
		return 0;
	
	wExpressionLen = strlen((char*)vExpression);
	wReplacementLen = strlenpgm((ROM char*)vReplacement);

	wFindCount = 0;
	wFindLenMinusOne = wFindLen - 1;
	vFirstFindChar = *vFind++;
	if(bSearchCaseInsensitive)	// Convert to all lowercase if needed
		if((vFirstFindChar >= (BYTE)'A') && (vFirstFindChar <= (BYTE)'Z'))
			vFirstFindChar += 'a' - 'A';

	// If the replacement string is the same length as the search string, then 
	// we can immediately do the needed replacements inline and return.
	if(wFindLen == wReplacementLen)
	{
		for(wBytesLeft = wExpressionLen; wBytesLeft; wBytesLeft--)
		{
			i = *vExpression++;
			if(bSearchCaseInsensitive)
			{
				if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
					i += 'a' - 'A';
				if(i != vFirstFindChar)
					continue;
				vExpressionCompare = vExpression;
				vFindCompare = vFind;
				w = wFindLenMinusOne;
				while(w)
				{
					i = *vExpressionCompare++;
					j = *vFindCompare++;
					if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
						i += 'a' - 'A';
					if((j >= (BYTE)'A') && (j <= (BYTE)'Z'))
						j += 'a' - 'A';
					if(i != j)
						break;
					w--;
				}
				if(w)
					continue;
			}
			else
			{
				if(i != vFirstFindChar)
					continue;
				if(memcmppgm2ram((void*)vExpression, (ROM void*)vFind, wFindLenMinusOne))
					continue;
			}
	
			memcpypgm2ram((void*)vExpression-1, (ROM void*)vReplacement, wReplacementLen);
			wFindCount++;
			vExpression += wFindLenMinusOne;
			wBytesLeft -= wFindLenMinusOne;
		}
		return wFindCount;
	}
	
	
	// If the replacement string is shorter than the search string, then we can 
	// search from left to right and move the string over as we find occurrences.
	if(wFindLen > wReplacementLen)
	{
		vDest = vExpression;
		for(wBytesLeft = wExpressionLen; wBytesLeft; wBytesLeft--)
		{
			i = *vExpression++;
			*vDest++ = i;
			if(bSearchCaseInsensitive)
			{
				if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
					i += 'a' - 'A';
				if(i != vFirstFindChar)
					continue;
				vExpressionCompare = vExpression;
				vFindCompare = vFind;
				w = wFindLenMinusOne;
				while(w)
				{
					i = *vExpressionCompare++;
					j = *vFindCompare++;
					if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
						i += 'a' - 'A';
					if((j >= (BYTE)'A') && (j <= (BYTE)'Z'))
						j += 'a' - 'A';
					if(i != j)
						break;
					w--;
				}
				if(w)
					continue;
			}
			else
			{
				if(i != vFirstFindChar)
					continue;
				if(memcmppgm2ram((void*)vExpression, (ROM void*)vFind, wFindLenMinusOne))
					continue;
			}
	
			memcpypgm2ram((void*)vDest-1, (ROM void*)vReplacement, wReplacementLen);
			vDest += wReplacementLen-1;
			wFindCount++;
			vExpression += wFindLenMinusOne;
			wBytesLeft -= wFindLenMinusOne;
		}
		*vDest = 0x00;	// Write new null terminator since the string may have shrunk
		return wFindCount;
	}
	
	// If the replacement string is longer than the search string, then we will 
	// take a two pass approach.  On the first pass, we will merely count how 
	// many replacements to make.  With this we can calculate how long the 
	// final string is going to be.  On the second pass, we will search from 
	// right to left and expand the string as needed.

	// Pass 1: count how many occurrences of vFind are in vExpression
	for(wBytesLeft = wExpressionLen; wBytesLeft; wBytesLeft--)
	{
		i = *vExpression++;
		if(bSearchCaseInsensitive)
		{
			if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
				i += 'a' - 'A';
			if(i != vFirstFindChar)
				continue;
			vExpressionCompare = vExpression;
			vFindCompare = vFind;
			w = wFindLenMinusOne;
			while(w)
			{
				i = *vExpressionCompare++;
				j = *vFindCompare++;
				if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
					i += 'a' - 'A';
				if((j >= (BYTE)'A') && (j <= (BYTE)'Z'))
					j += 'a' - 'A';
				if(i != j)
					break;
				w--;
			}
			if(w)
				continue;
		}
		else
		{
			if(i != vFirstFindChar)
				continue;
			if(memcmppgm2ram((void*)vExpression, (ROM void*)vFind, wFindLenMinusOne))
				continue;
		}

		wFindCount++;
		vExpression += wFindLenMinusOne;
		wBytesLeft -= wFindLenMinusOne;
	}
	
	// Return immediately if no replacements are needed
	if(wFindCount == 0u)
		return 0;

	// Pass 2: make replacements and move string over
	vDest = vExpression + wFindCount * (wReplacementLen - wFindLen);
	if(vDest > vExpression - wExpressionLen + wMaxLen)
		return -1;
	*vDest-- = 0x00;	// Write new null terminator
	vExpression -= 1;
	vFind -= 1;
	vFirstFindChar = vFind[wFindLenMinusOne];
	if(bSearchCaseInsensitive)	// Convert to all lowercase if needed
		if((vFirstFindChar >= (BYTE)'A') && (vFirstFindChar <= (BYTE)'Z'))
			vFirstFindChar += 'a' - 'A';
	wReplacementsLeft = wFindCount;
	while(wReplacementsLeft)
	{
		i = *vExpression--;
		*vDest-- = i;
		if(bSearchCaseInsensitive)
		{
			if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
				i += 'a' - 'A';
			if(i != vFirstFindChar)
				continue;
			vExpressionCompare = vExpression;
			vFindCompare = &vFind[wFindLenMinusOne-1];
			w = wFindLenMinusOne;
			while(w)
			{
				i = *vExpressionCompare--;
				j = *vFindCompare--;
				if((i >= (BYTE)'A') && (i <= (BYTE)'Z'))
					i += 'a' - 'A';
				if((j >= (BYTE)'A') && (j <= (BYTE)'Z'))
					j += 'a' - 'A';
				if(i != j)
					break;
				w--;
			}
			if(w)
				continue;
		}
		else
		{
			if(i != vFirstFindChar)
				continue;
			if(memcmppgm2ram((void*)vExpression-wFindLenMinusOne, (ROM void*)vFind, wFindLenMinusOne))
				continue;
		}
		memcpypgm2ram((void*)vDest-wReplacementLen+2, (ROM void*)vReplacement, wReplacementLen);
		vDest -= wReplacementLen-1;

		vExpression -= wFindLenMinusOne;
		wBytesLeft -= wFindLenMinusOne;
		wReplacementsLeft--;
	}
	return wFindCount;
}
#endif
