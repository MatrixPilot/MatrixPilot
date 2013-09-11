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


#if defined(__dsPIC33F__)
#include "p33Fxxxx.h"
#elif defined(__dsPIC33E__)
#include "p33Exxxx.h"
#elif defined(__PIC24E__)
#include "p24Exxxx.h"
#endif
#include <stdint.h>
#include <stdio.h>
#include "interrupt.h"

//#define USE_SETJMP

#define TRAP_SRC_OSCFAIL    1
#define TRAP_SRC_MATHERR    2
#define TRAP_SRC_RESERVED   4
#define TRAP_SRC_STACKERR   8
#define TRAP_SRC_ADDRSERR   16
#define TRAP_SRC_DEFAULTINT 32
#define TRAP_SRC_DMACERR    64

extern volatile uint16_t trap_flags;
extern volatile uint32_t trap_source;
extern volatile uint16_t osc_fail_count;
extern volatile uint16_t stack_ptr;

uint32_t getErrLoc(void);   // Get Address Error Loc

void __attribute__((__interrupt__)) _OscillatorFail(void);
void __attribute__((__interrupt__)) _AddressError(void);
void __attribute__((__interrupt__)) _StackError(void);
void __attribute__((__interrupt__)) _MathError(void);
void __attribute__((__interrupt__)) _DMACError(void);

extern volatile uint16_t JmpAddrLow;
extern volatile uint16_t JmpAddrHgh;
extern volatile uint16_t JmpStckPtr;

void restart(uint16_t flags, uint32_t addrs)
{
	trap_flags = flags;
	trap_source = addrs;

#ifdef USE_SETJMP
	if (flags < TRAP_SRC_RESERVED)
#else
	if (1)
#endif
	{
		// for certain exceptions a full reset seems like the best approach
		asm("reset");
	}
	else
	{
		// for less 'serious' exceptions just take the longjmp solution
		asm("mov   _JmpStckPtr,w15");
		asm("push  _JmpAddrLow");
		asm("push  _JmpAddrHgh");
		asm("mov   #1,w0");
		asm("retfie");
	}
}

void __attribute__((interrupt, no_auto_psv)) _OscillatorFail(void)
{
	INTCON1bits.OSCFAIL = 0;        // Clear the trap flag
	osc_fail_count++;
	restart(TRAP_SRC_OSCFAIL, getErrLoc());
}

void __attribute__((interrupt, no_auto_psv)) _AddressError(void)
{
	INTCON1bits.ADDRERR = 0;        // Clear the trap flag
	restart(TRAP_SRC_ADDRSERR, getErrLoc());
}

void __attribute__((interrupt, no_auto_psv)) _StackError(void)
{
	INTCON1bits.STKERR = 0;         // Clear the trap flag
	restart(TRAP_SRC_STACKERR, getErrLoc());
}

void __attribute__((interrupt, no_auto_psv)) _MathError(void)
{
	INTCON1bits.MATHERR = 0;        // Clear the trap flag
	restart(TRAP_SRC_MATHERR, getErrLoc());
}

unsigned int dmaErrFlag = 0, dmaPWErrLoc = 0, dmaRWErrLoc;

void __attribute__((interrupt, no_auto_psv)) _DMACError(void)
{
	INTCON1bits.DMACERR = 0;        // Clear the trap flag
#if defined(__dsPIC33E__)
//	errLoc = getErrLoc();
	dmaErrFlag = DMAPWC;
	//dmaErrFlag = INTCON3bits.DAE;

// Peripheral Write Collision Error Location
	if (dmaErrFlag &0x1)
		dmaPWErrLoc = DMA0STAL;

	dmaErrFlag=	DMARQC;

// DMA RAM Write Collision Error Location
	if (dmaErrFlag & 0x2)
		dmaRWErrLoc = DMA1STAL;
	DMARQC = 0;                     // Clear the DMA Request Collision Flag Bit
	DMAPWC = 0;                     // Clear the Peripheral Write Collision Flag Bit
#endif // __dsPIC33E__
	restart(TRAP_SRC_DMACERR, getErrLoc());
}
