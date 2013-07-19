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


#define PIN_IN_GROUP(g,p) ((g)*4+(p))
#define OUTPUT	0
#define INPUT	1

boolean didFail = 0;


void setPinState(int pinIndex, char val)
{
	switch (pinIndex) {
		case 0:  _TRISC1 = val; break;
		case 1:  _TRISC2 = val; break;
		case 2:  _TRISC3 = val; break;
		case 3:  _TRISG6 = val; break;

		case 4:  _TRISG7 = val; break;
		case 5:  _TRISG8 = val; break;
		case 6:  _TRISG9 = val; break;
		case 7:  _TRISA12 = val; break;

		case 8:  _TRISA13 = val; break;
		case 9:  _TRISB2 = val; break;
		case 10: _TRISA1 = val; break;
		case 11: _TRISF13 = val; break;

		case 12: _TRISF12 = val; break;
		case 13: _TRISB15 = val; break;
		case 14: _TRISD14 = val; break;
		case 15: _TRISD15 = val; break;

		case 16: _TRISF4 = val; break;
		case 17: _TRISF5 = val; break;
		case 18: _TRISF2 = val; break;
		case 19: _TRISF8 = val; break;

		case 20: _TRISF7 = val; break;
		case 21: _TRISF6 = val; break;
		case 22: _TRISA2 = val; break;
		case 23: _TRISA3 = val; break;

		case 24: _TRISA4 = val; break;
		case 25: _TRISA5 = val; break;
		case 26: _TRISA14 = val; break;
		case 27: _TRISD8 = val; break;

		case 28: _TRISD9 = val; break;
		case 29: _TRISD10 = val; break;
		case 30: _TRISD11 = val; break;
		case 31: _TRISD0 = val; break;

		case 32: _TRISD1 = val; break;
		case 33: _TRISD2 = val; break;
		case 34: _TRISD3 = val; break;
		case 35: _TRISD12 = val; break;

		case 36: _TRISD13 = val; break;
		case 37: _TRISD4 = val; break;
		case 38: _TRISD5 = val; break;
		case 39: _TRISD6 = val; break;

		case 40: _TRISD7 = val; break;
		case 41: _TRISF0 = val; break;
		case 42: _TRISF1 = val; break;
		case 43: _TRISA7 = val; break;

		case 44: _TRISE0 = val; break;
	}
}

void setPinValue(int pinIndex, char val)
{
	switch (pinIndex) {
		case 0:  _LATC1 = val; break;
		case 1:  _LATC2 = val; break;
		case 2:  _LATC3 = val; break;
		case 3:  _LATG6 = val; break;

		case 4:  _LATG7 = val; break;
		case 5:  _LATG8 = val; break;
		case 6:  _LATG9 = val; break;
		case 7:  _LATA12 = val; break;

		case 8:  _LATA13 = val; break;
		case 9:  _LATB2 = val; break;
		case 10: _LATA1 = val; break;
		case 11: _LATF13 = val; break;

		case 12: _LATF12 = val; break;
		case 13: _LATB15 = val; break;
		case 14: _LATD14 = val; break;
		case 15: _LATD15 = val; break;

		case 16: _LATF4 = val; break;
		case 17: _LATF5 = val; break;
		case 18: _LATF2 = val; break;
		case 19: _LATF8 = val; break;

		case 20: _LATF7 = val; break;
		case 21: _LATF6 = val; break;
		case 22: _LATA2 = val; break;
		case 23: _LATA3 = val; break;

		case 24: _LATA4 = val; break;
		case 25: _LATA5 = val; break;
		case 26: _LATA14 = val; break;
		case 27: _LATD8 = val; break;

		case 28: _LATD9 = val; break;
		case 29: _LATD10 = val; break;
		case 30: _LATD11 = val; break;
		case 31: _LATD0 = val; break;

		case 32: _LATD1 = val; break;
		case 33: _LATD2 = val; break;
		case 34: _LATD3 = val; break;
		case 35: _LATD12 = val; break;

		case 36: _LATD13 = val; break;
		case 37: _LATD4 = val; break;
		case 38: _LATD5 = val; break;
		case 39: _LATD6 = val; break;

		case 40: _LATD7 = val; break;
		case 41: _LATF0 = val; break;
		case 42: _LATF1 = val; break;
		case 43: _LATA7 = val; break;

		case 44: _LATE0 = val; break;
	}
}

char getPinValue(int pinIndex)
{
	switch (pinIndex) {
		case 0:  return _RC1; break;
		case 1:  return _RC2; break;
		case 2:  return _RC3; break;
		case 3:  return _RG6; break;

		case 4:  return _RG7; break;
		case 5:  return _RG8; break;
		case 6:  return _RG9; break;
		case 7:  return _RA12; break;

		case 8:  return _RA13; break;
		case 9:  return _RB2; break;
		case 10: return _RA1; break;
		case 11: return _RF13; break;

		case 12: return _RF12; break;
		case 13: return _RB15; break;
		case 14: return _RD14; break;
		case 15: return _RD15; break;

		case 16: return _RF4; break;
		case 17: return _RF5; break;
		case 18: return _RF2; break;
		case 19: return _RF8; break;

		case 20: return _RF7; break;
		case 21: return _RF6; break;
		case 22: return _RA2; break;
		case 23: return _RA3; break;

		case 24: return _RA4; break;
		case 25: return _RA5; break;
		case 26: return _RA14; break;
		case 27: return _RD8; break;

		case 28: return _RD9; break;
		case 29: return _RD10; break;
		case 30: return _RD11; break;
		case 31: return _RD0; break;

		case 32: return _RD1; break;
		case 33: return _RD2; break;
		case 34: return _RD3; break;
		case 35: return _RD12; break;

		case 36: return _RD13; break;
		case 37: return _RD4; break;
		case 38: return _RD5; break;
		case 39: return _RD6; break;

		case 40: return _RD7; break;
		case 41: return _RF0; break;
		case 42: return _RF1; break;
		case 43: return _RA7; break;

		case 44: return _RE0; break;
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
	if (didFail) return;

	didFail = 1;

	// Look at pinIndex and testNum in the debugger to see why it failed.
	// If we add serial output, we'll send out info about each failed test.

	_TRISE1 = _TRISE4 = 0;
	_LATE1 = _LATE4 = 0;

	int i;
	for (i=0; i<32; i++) {
		_LATE1 = _LATE4 = (i%2 == 0);
		delay(200000);
	}
}

void IOTest(void)
{
	int g;

	PMD1bits.AD1MD = 1;
	PMD3bits.AD2MD = 1;

	for (g=0; g<=10; g++) {
		// Set as inputs
		setPinState(PIN_IN_GROUP(g,0), INPUT);
		setPinState(PIN_IN_GROUP(g,1), INPUT);
		setPinState(PIN_IN_GROUP(g,2), INPUT);
		setPinState(PIN_IN_GROUP(g,3), INPUT);
		if (g == 10) setPinState(PIN_IN_GROUP(g,4), INPUT);
		delay(1000);

		// Fail if low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 0) failPin(PIN_IN_GROUP(g,0), 0);
		if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 0);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 0) failPin(PIN_IN_GROUP(g,2), 0);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 0);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 0) failPin(PIN_IN_GROUP(g,4), 0);

		// Set pin A as output low
		setPinState(PIN_IN_GROUP(g,0), OUTPUT);
		setPinValue(PIN_IN_GROUP(g,0), 0);
		delay(1000);

		// Fail if C is high or if B or D is low
		if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 1);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 1) failPin(PIN_IN_GROUP(g,2), 1);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 1);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 1) failPin(PIN_IN_GROUP(g,4), 1);

		// Set pin B as output low
		setPinState(PIN_IN_GROUP(g,0), INPUT);
		setPinState(PIN_IN_GROUP(g,1), OUTPUT);
		setPinValue (PIN_IN_GROUP(g,1), 0);
		delay(1000);

		// Fail if D is high or if A or C is low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 0) failPin(PIN_IN_GROUP(g,0), 2);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 0) failPin(PIN_IN_GROUP(g,2), 2);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 1) failPin(PIN_IN_GROUP(g,3), 2);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 0) failPin(PIN_IN_GROUP(g,4), 2);

		// Set pin C as output low
		setPinState(PIN_IN_GROUP(g,1), INPUT);
		setPinState(PIN_IN_GROUP(g,2), OUTPUT);
		setPinValue (PIN_IN_GROUP(g,2), 0);
		delay(1000);

		// Fail if A is high or if B or D is low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 1) failPin(PIN_IN_GROUP(g,0), 3);
		if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 3);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 3);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 1) failPin(PIN_IN_GROUP(g,4), 3);

		// Set pin D as output low
		setPinState(PIN_IN_GROUP(g,2), INPUT);
		setPinState(PIN_IN_GROUP(g,3), OUTPUT);
		setPinValue (PIN_IN_GROUP(g,3), 0);
		delay(1000);

		// Fail if B is high or if A or C is low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 0) failPin(PIN_IN_GROUP(g,0), 4);
		if (getPinValue(PIN_IN_GROUP(g,1)) == 1) failPin(PIN_IN_GROUP(g,1), 4);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 0) failPin(PIN_IN_GROUP(g,2), 4);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 0) failPin(PIN_IN_GROUP(g,4), 4);

		setPinState(PIN_IN_GROUP(g,3), INPUT);

		// Test the bonus G10P1 pin
		if (g == 10) {
			// Set the bonus G10P1 pin as output low
			setPinState(PIN_IN_GROUP(g,4), OUTPUT);
			setPinValue (PIN_IN_GROUP(g,4), 0);
			delay(1000);

			// Fail if A or C is high or if B or D is low
			if (getPinValue(PIN_IN_GROUP(g,0)) == 1) failPin(PIN_IN_GROUP(g,0), 4);
			if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 4);
			if (getPinValue(PIN_IN_GROUP(g,2)) == 1) failPin(PIN_IN_GROUP(g,2), 4);
			if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 4);

			setPinState(PIN_IN_GROUP(g,4), INPUT);
		}
	}
	PMD1bits.AD1MD = 0;
	PMD3bits.AD2MD = 0;
}
