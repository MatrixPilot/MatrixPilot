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


#ifndef _UART_H_
#define _UART_H_

char GetBaudError(void);
char GetChar(void);
void PutChar(char ch);
void Init(void);
char IsPressed(void);
void PrintString(char* str);
void PutDec(unsigned char dec);
void PutHex(int toPrint);

#if defined(__C30__) || defined(__PIC32MX__)
void PutHexWord(unsigned int toPrint);
void PutHexDWord(unsigned long int toPrint);
#endif

char Char2Hex(char ch);
char Hex2Char(char hex);
void ClrError(void);
int DataReceived(void);

#define kbhit IsPressed
#define getch GetChar
#define putch PutChar


#endif // _UART_H_
