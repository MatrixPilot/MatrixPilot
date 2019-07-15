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


#ifndef OSCILLATOR_H
#define OSCILLATOR_H


#define MIPS                    32

// clock-frequecy in Hz with suffix LL (64-bit-long), eg. 32000000LL for 32MHz
#if (MIPS == 16)
#define FREQOSC                 32000000LL  // 16 MIPS
#elif (MIPS == 32)
#define FREQOSC                 64000000LL  // 32 MIPS
#elif (MIPS == 40)
#define FREQOSC                 80000000LL  // 40 MIPS
#elif (MIPS == 64)
#define FREQOSC                 128000000LL // 64 MIPS
#elif (MIPS == 70)
#define FREQOSC                 140000000LL // 70 MIPS
#else
#error Invalid MIPS setting, must be 16, 32, 40, 64 or 70
#endif // MIPS

#define FOSC                    FREQOSC
#define CLK_PHASES              2
#define FCY                     (FOSC/CLK_PHASES)   // MCU is running at FCY MIPS

// Define the clock macros expected by the Microchip Application Libraries
#define GetSystemClock()        (FCY)
#define GetInstructionClock()   (FCY)   // Normally GetSystemClock()/2 for PIC24/dsPIC
#define GetPeripheralClock()    (FCY)

#define delay_us(x) __delay32(((((long long)x)*FCY)/1000000L))  // delays x us
#define delay_ms(x) __delay32(((((long long)x)*FCY)/1000L))     // delays x ms

void __delay32(unsigned long cycles);


#endif // OSCILLATOR_H
