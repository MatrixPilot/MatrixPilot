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


#include "libUDB.h"
#include "oscillator.h"
#include "uart.h"
#include "options_ports.h"

#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif

#ifndef CONSOLE_UART
#error CONSOLE_UART must be defined
#endif

extern int __C30_UART;

#if (CONSOLE_UART != 0) && (CONSOLE_UART != 9)

#ifdef USE_BLUETOOTH_UART
#define BAUDRATEX       9600UL
#define BRGHX           0
#else
#define BAUDRATEX       115200UL
#define BRGHX           1
#endif

#if (BRGHX == 0)
	#define BRG_DIVX    16
#elif (BRGHX == 1)
	#define BRG_DIVX    4
#else
	#error Must select a valid BRGHX value
#endif

//******************************************************************************
// Constants
//******************************************************************************

//UBRG register value and baudrate mistake calculation

#if defined (__C30__)
	#if defined (__dsPIC33E__) || defined (__PIC24E__)
	#define BAUDRATEREG     (((GetPeripheralClock())/(BRG_DIVX * BAUDRATEX)) - 1)
	#else
	#define BAUDRATEREG     (((GetSystemClock()/2)+(BRG_DIVX/2*BAUDRATEX))/BRG_DIVX/BAUDRATEX-1)
	#endif
#elif defined (__PIC32MX__)
	#define BAUDRATEREG     ((GetPeripheralClock()+(BRG_DIVX/2*BAUDRATEX))/BRG_DIVX/BAUDRATEX-1)
#else
	#error Cannot calculate BAUDRATEREG value
#endif

#if defined (__C30__)
	#if defined (__dsPIC33E__)|| defined (__PIC24E__)
	#define BAUD_ACTUAL     ((GetPeripheralClock())/(BRG_DIVX * (BAUDRATEREG+1)))
	#else
	#define BAUD_ACTUAL     ((GetSystemClock()/2)/BRG_DIVX/(BAUDRATEREG+1))
	#endif
#elif defined (__PIC32MX__)
	#define BAUD_ACTUAL     (GetPeripheralClock()/BRG_DIVX/(BAUDRATEREG+1))
#else
	#error Cannot calculate actual baud rate
#endif

#define BAUD_ERROR          ((BAUD_ACTUAL > BAUDRATEX) ? BAUD_ACTUAL-BAUDRATEX : BAUDRATEX-BAUD_ACTUAL)
#define BAUD_ERROR_PERCENT  ((BAUD_ERROR*100+BAUDRATEX/2)/BAUDRATEX)

#if defined (__C30__)
	#if (BAUD_ERROR_PERCENT > 3)
		#error UART frequency error is worse than 3%
	#elif (BAUD_ERROR_PERCENT > 2)
		#warning UART frequency error is worse than 2%
	#endif
#endif // #if defined (__C30__)

#if defined (__dsPIC33F__)
#undef BAUDRATEREG
//#define UDB_BAUD(x) ((int16_t)((FREQOSC / CLK_PHASES) / ((int32_t)4 * x) - 1))
#define BAUDRATEREG ((int16_t)((FREQOSC / CLK_PHASES) / ((int32_t)4 * BAUDRATEX) - 1))
#endif

#define _INIT(x, y) \
void Init(void) \
{ \
	U##x##BRG = BAUDRATEREG; \
	U##x##MODE = 0; \
	U##x##MODEbits.BRGH = BRGHX; \
	U##x##STA = 0; \
	U##x##MODEbits.UARTEN = 1; \
	U##x##STAbits.UTXEN = 1; \
	IFS##y##bits.U##x##RXIF = 0; \
}

#define _ISPRESSED(x) \
char IsPressed(void) \
{ \
	if (U##x##STAbits.URXDA) return 1; \
	return 0; \
}

#define _GETCHAR(x) \
char GetChar(void) \
{ \
	char Temp; \
	while (!IsPressed()); \
	Temp = U##x##RXREG; \
	ClrError(); \
	return Temp; \
}

#define _PUTCHAR(x) \
void PutChar(char ch) \
{ \
	U##x##TXREG = ch; \
	while (U##x##STAbits.TRMT == 0); \
}

#define _CLRERROR(x) \
void ClrError(void) \
{ \
	if (U##x##STAbits.OERR) U##x##STAbits.OERR = 0; \
}

// define the 'pastor' macros
#define _Init(x, y)   _INIT(x, y)
#define _IsPressed(x) _ISPRESSED(x)
#define _GetChar(x)   _GETCHAR(x)
#define _PutChar(x)   _PUTCHAR(x)
#define _ClrError(x)  _CLRERROR(x)

#if (CONSOLE_UART == 1)
_Init(CONSOLE_UART, 0);
#elif (CONSOLE_UART == 2)
_Init(CONSOLE_UART, 1);
#elif (CONSOLE_UART == 3 || CONSOLE_UART == 4)
_Init(CONSOLE_UART, 5);
#endif // CONSOLE_UART

_IsPressed(CONSOLE_UART);
_GetChar(CONSOLE_UART);
_PutChar(CONSOLE_UART);
_ClrError(CONSOLE_UART);

void init_uart(void)
{
	__C30_UART = CONSOLE_UART;
	Init();
}

/*******************************************************************************
Function: GetBaudError()

Precondition:
	None.

Overview:
	This routine checks the UART baud rate error percentage and returns it.

Input: None.

Output: Returns the baud rate error in percent.

*******************************************************************************/
char GetBaudError(void)
{
	unsigned int errorPercent = 0;

	errorPercent = ((BAUD_ERROR*100+BAUDRATEX/2)/BAUDRATEX);
	return (char)errorPercent;
}


/*******************************************************************************
Function: PrintString(char *str)

Precondition:
	Init must be called prior to calling this routine.

Overview:
	This function prints a string of characters to the UART.

Input: Pointer to a null terminated character string.

Output: None.

*******************************************************************************/
void PrintString(const char *str)
{
	unsigned char c;

	while ((c = *str++))
		PutChar(c);
}

/*******************************************************************************
Function: PutDec(unsigned char dec)

Precondition:
	Init must be called prior to calling this routine.

Overview:
	This function converts decimal data into a string and outputs it to UART.

Input: Binary data.

Output: None.

*******************************************************************************/
void PutDec(unsigned char dec)
{
	unsigned char res;
	unsigned char printed_already = 0;

	res = dec;

	if (res/100)
	{
		PutChar(res/100 + '0');
		printed_already = 1;
	}
	res = res - (res/100)*100;

	if ((res/10) || (printed_already == 1))
	{
		PutChar(res/10 + '0');
	}
	res = res - (res/10)*10;

	PutChar(res + '0');
}

/*******************************************************************************
Function: PutHex

Precondition:
	Init must be called prior to calling this routine.

Overview:
	This function converts hex data into a string and outputs it to UART.

Input: Binary data.

Output: None.

*******************************************************************************/

const unsigned char CharacterArray[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//extern const unsigned char CharacterArray[];

void PutHex(int toPrint)
{
	int printVar;

	printVar = toPrint;
	toPrint = (toPrint>>4) & 0x0F;
	PutChar(CharacterArray[toPrint]);

	toPrint = printVar & 0x0F;
	PutChar(CharacterArray[toPrint]);
}

/*******************************************************************************
Function: PutHexWord(unsigned int toPrint)

Precondition:
	Init must be called prior to calling this routine.

Overview:
	This function converts hex data into a string and outputs it to UART.

Input: Binary data.

Output: None.

*******************************************************************************/
#if defined(__C30__) || defined(__PIC32MX__)
void PutHexWord(unsigned int toPrint)
{
	unsigned int printVar;

	printVar = (toPrint>>12) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>8) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>4) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = toPrint & 0x0F;
	PutChar(CharacterArray[printVar]);
}

void PutHexDWord(unsigned long toPrint)
{
	unsigned long printVar;

	printVar = (toPrint>>28) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>24) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>20) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>16) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>12) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>8) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = (toPrint>>4) & 0x0F;
	PutChar(CharacterArray[printVar]);

	printVar = toPrint & 0x0F;
	PutChar(CharacterArray[printVar]);
}

#endif

/*********************************************************************
Function: char Char2Hex(char ch)

PreCondition: none

Input: ASCII to be converted

Output: number

Side Effects: none

Overview: converts ASCII coded digit into number

Note: none

********************************************************************/
char Char2Hex(char ch)
{
	// Wrong char
	if (ch > 102)
		return 0;

	// From a to f
	if (ch > 96)
		return (ch - 87);

	// Wrong char
	if (ch > 70)
		return 0;

	// From A to F
	if (ch > 64)
		return (ch - 55);

	// Wrong char
	if (ch > 57)
		return 0;

	// From 0 - 9
	if (ch > 47)
		return (ch - 48);
	else // Wrong char
		return 0;
}

/*********************************************************************
Function: char Hex2Char(char hex)

PreCondition: none

Input: number

Output: ASCII code

Side Effects: none

Overview: converts low nibble into ASCII coded digit

Note: none

********************************************************************/
char Hex2Char(char hex)
{
	char h;
	h = hex & 0x0f;

	// From 0xa to 0xf
	if (h > 9)
		return (h + 55);
	else
		return (h + 48);
}

#else

void init_uart(void)
{
#if (CONSOLE_UART == 9)
	__C30_UART = CONSOLE_UART;
#endif
}

#endif // CONSOLE_UART
