// This file is part of the MatrixPilot LedTest demo.
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


#include "../../libUDB/libUDB.h"


#define NUM_PINS_IN_GROUP_1	22
#define NUM_PINS_IN_GROUP_2	19
#define PIN_IN_GROUP(g, p)	((((g)-1)*NUM_PINS_IN_GROUP_1)+(p))

#define OUTPUT	0
#define INPUT	1


boolean didFail = 0 ;


void setPinState(int pinIndex, char val)
{
	switch(pinIndex) {
		case  0:  _TRISD8 = val; break;
		case  1:  _TRISD10 = val; break;
		case  2:  _TRISD13 = val; break;
		case  3:  _TRISD15 = val; break;
		case  4:  _TRISD0 = val; break;
		case  5:  _TRISD1 = val; break;
		case  6:  _TRISD3 = val; break;
		case  7:  _TRISD5 = val; break;
		case  8:  _TRISD7 = val; break;
		case  9:  _TRISF13 = val; break;
		case 10:  _TRISF5 = val; break;
		case 11:  _TRISA3 = val; break;
		case 12:  _TRISA12 = val; break;
		case 13:  _TRISB2 = val; break;
		case 14:  _TRISF8 = val; break;
		case 15:  _TRISF6 = val; break;
		case 16:  _TRISB15 = val; break;
		case 17:  _TRISC2 = val; break;
		case 18:  _TRISA5 = val; break;
		case 19:  _TRISE0 = val; break;
		case 20:  _TRISF1 = val; break;
		case 21:  _TRISF3 = val; break;

		case 22:  _TRISD9 = val; break;
		case 23:  _TRISD11 = val; break;
		case 24:  _TRISD12 = val; break;
		case 25:  _TRISD14 = val; break;
		case 26:  _TRISD2 = val; break;
		case 27:  _TRISD4 = val; break;
		case 28:  _TRISD6 = val; break;
		case 29:  _TRISF4 = val; break;
		case 30:  _TRISA2 = val; break;
		case 31:  _TRISF12 = val; break;
		case 32:  _TRISA13 = val; break;
		case 33:  _TRISF7 = val; break;
		case 34:  _TRISC1 = val; break;
		case 35:  _TRISC3 = val; break;
		case 36:  _TRISA7 = val; break;
		case 37:  _TRISF0 = val; break;
		case 38:  _TRISA1 = val; break;
		case 39:  _TRISA4 = val; break;
		case 40:  _TRISF2 = val; break;
	}
}


void setPinValue(int pinIndex, char val)
{
	switch(pinIndex) {
		case  0:  _LATD8 = val; break;
		case  1:  _LATD10 = val; break;
		case  2:  _LATD13 = val; break;
		case  3:  _LATD15 = val; break;
		case  4:  _LATD0 = val; break;
		case  5:  _LATD1 = val; break;
		case  6:  _LATD3 = val; break;
		case  7:  _LATD5 = val; break;
		case  8:  _LATD7 = val; break;
		case  9:  _LATF13 = val; break;
		case 10:  _LATF5 = val; break;
		case 11:  _LATA3 = val; break;
		case 12:  _LATA12 = val; break;
		case 13:  _LATB2 = val; break;
		case 14:  _LATF8 = val; break;
		case 15:  _LATF6 = val; break;
		case 16:  _LATB15 = val; break;
		case 17:  _LATC2 = val; break;
		case 18:  _LATA5 = val; break;
		case 19:  _LATE0 = val; break;
		case 20:  _LATF1 = val; break;
		case 21:  _LATF3 = val; break;

		case 22:  _LATD9 = val; break;
		case 23:  _LATD11 = val; break;
		case 24:  _LATD12 = val; break;
		case 25:  _LATD14 = val; break;
		case 26:  _LATD2 = val; break;
		case 27:  _LATD4 = val; break;
		case 28:  _LATD6 = val; break;
		case 29:  _LATF4 = val; break;
		case 30:  _LATA2 = val; break;
		case 31:  _LATF12 = val; break;
		case 32:  _LATA13 = val; break;
		case 33:  _LATF7 = val; break;
		case 34:  _LATC1 = val; break;
		case 35:  _LATC3 = val; break;
		case 36:  _LATA7 = val; break;
		case 37:  _LATF0 = val; break;
		case 38:  _LATA1 = val; break;
		case 39:  _LATA4 = val; break;
		case 40:  _LATF2 = val; break;
	}
}


char getPinValue(int pinIndex)
{
	switch(pinIndex) {
		case  0:  return _RD8; break;
		case  1:  return _RD10; break;
		case  2:  return _RD13; break;
		case  3:  return _RD15; break;
		case  4:  return _RD0; break;
		case  5:  return _RD1; break;
		case  6:  return _RD3; break;
		case  7:  return _RD5; break;
		case  8:  return _RD7; break;
		case  9:  return _RF13; break;
		case 10:  return _RF5; break;
		case 11:  return _RA3; break;
		case 12:  return _RA12; break;
		case 13:  return _RB2; break;
		case 14:  return _RF8; break;
		case 15:  return _RF6; break;
		case 16:  return _RB15; break;
		case 17:  return _RC2; break;
		case 18:  return _RA5; break;
		case 19:  return _RE0; break;
		case 20:  return _RF1; break;
		case 21:  return _RF3; break;

		case 22:  return _RD9; break;
		case 23:  return _RD11; break;
		case 24:  return _RD12; break;
		case 25:  return _RD14; break;
		case 26:  return _RD2; break;
		case 27:  return _RD4; break;
		case 28:  return _RD6; break;
		case 29:  return _RF4; break;
		case 30:  return _RA2; break;
		case 31:  return _RF12; break;
		case 32:  return _RA13; break;
		case 33:  return _RF7; break;
		case 34:  return _RC1; break;
		case 35:  return _RC3; break;
		case 36:  return _RA7; break;
		case 37:  return _RF0; break;
		case 38:  return _RA1; break;
		case 39:  return _RA4; break;
		case 40:  return _RF2; break;
	}
	return 0;
}


void delay(long d)
{
	for (; d>0; d--) {
		Nop();
	}
}


void failPin(int pinIndex, char testNum)
{
	if (didFail) return ;

	didFail = 1 ;

	// Look at pinIndex and testNum in the debugger to see why it failed.
	// If we add serial output, we'll send out info about each failed test.

	_TRISE1 = _TRISE4 = 0 ;
	_LATE1 = _LATE4 = 0 ;

	int i;
	for (i=0; i<32; i++) {
		_LATE1 = _LATE4 = (i%2 == 0) ;
		delay(200000) ;
	} ;
}


void IOTest(void)
{
	int p ;

	PMD1bits.AD1MD = 1;
	PMD3bits.AD2MD = 1;

	// Set as inputs
	for (p=0; p<NUM_PINS_IN_GROUP_1+NUM_PINS_IN_GROUP_2; p++) {
		setPinState(p, INPUT) ;
	}
	delay(1000) ;

	// Test that we're all pulled high
	for (p=0; p<NUM_PINS_IN_GROUP_1+NUM_PINS_IN_GROUP_2; p++) {
		if (getPinValue(p) == 0) failPin(p, 1);
	}

	int testPin;
	for (testPin=0; testPin<NUM_PINS_IN_GROUP_1; testPin++) {
		// Set testPin in group 1 to output low
		setPinState(PIN_IN_GROUP(1,testPin), OUTPUT);
		setPinValue(PIN_IN_GROUP(1,testPin), 0);
		delay(1000) ;

		// Test that group 1 is low, and 2 is high
		for (p=0; p<NUM_PINS_IN_GROUP_1; p++) {
			if (p == testPin) continue;
			if (getPinValue(PIN_IN_GROUP(1,p)) == 1) failPin(PIN_IN_GROUP(1,p), 2);
		}
		for (p=0; p<NUM_PINS_IN_GROUP_2; p++) {
			if (getPinValue(PIN_IN_GROUP(2,p)) == 0) failPin(PIN_IN_GROUP(2,p), 3);
		}
		setPinState(PIN_IN_GROUP(1,testPin), INPUT);
	}

	for (testPin=0; testPin<NUM_PINS_IN_GROUP_2; testPin++) {
		// Set testPin in group 2 to output low
		setPinState(PIN_IN_GROUP(2,testPin), OUTPUT);
		setPinValue(PIN_IN_GROUP(2,testPin), 0);
		delay(1000) ;

		// Test that group 1 is high, and 2 is low
		for (p=0; p<NUM_PINS_IN_GROUP_1; p++) {
			if (getPinValue(PIN_IN_GROUP(1,p)) == 0) failPin(PIN_IN_GROUP(1,p), 4);
		}
		for (p=0; p<NUM_PINS_IN_GROUP_2; p++) {
			if (p == testPin) continue;
			if (getPinValue(PIN_IN_GROUP(2,p)) == 1) failPin(PIN_IN_GROUP(2,p), 5);
		}
		setPinState(PIN_IN_GROUP(2,testPin), INPUT);
	}

	PMD1bits.AD1MD = 0;
	PMD3bits.AD2MD = 0;
}
