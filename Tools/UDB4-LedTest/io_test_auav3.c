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


boolean didFail = 0 ;


void setPinState(int pinIndex, char val)
{
}


void setPinValue(int pinIndex, char val)
{
}


char getPinValue(int pinIndex)
{
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
	
}


void IOTest(void)
{
	int g ;
	
	PMD1bits.AD1MD = 1;
	PMD3bits.AD2MD = 1;
	
	for (g=0; g<=10; g++) {
		// Set as inputs
		setPinState(PIN_IN_GROUP(g,0), INPUT) ;
		setPinState(PIN_IN_GROUP(g,1), INPUT) ;
		setPinState(PIN_IN_GROUP(g,2), INPUT) ;
		setPinState(PIN_IN_GROUP(g,3), INPUT) ;
		if (g == 10) setPinState(PIN_IN_GROUP(g,4), INPUT) ;
		delay(1000) ;
		
		// Fail if low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 0) failPin(PIN_IN_GROUP(g,0), 0);
		if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 0);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 0) failPin(PIN_IN_GROUP(g,2), 0);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 0);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 0) failPin(PIN_IN_GROUP(g,4), 0);
		
		// Set pin A as output low
		setPinState(PIN_IN_GROUP(g,0), OUTPUT) ;
		setPinValue(PIN_IN_GROUP(g,0), 0) ;
		delay(1000) ;
		
		// Fail if C is high or if B or D is low
		if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 1);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 1) failPin(PIN_IN_GROUP(g,2), 1);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 1);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 1) failPin(PIN_IN_GROUP(g,4), 1);
		
		// Set pin B as output low
		setPinState(PIN_IN_GROUP(g,0), INPUT) ;
		setPinState(PIN_IN_GROUP(g,1), OUTPUT) ;
		setPinValue (PIN_IN_GROUP(g,1), 0) ;
		delay(1000) ;
		
		// Fail if D is high or if A or C is low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 0) failPin(PIN_IN_GROUP(g,0), 2);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 0) failPin(PIN_IN_GROUP(g,2), 2);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 1) failPin(PIN_IN_GROUP(g,3), 2);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 0) failPin(PIN_IN_GROUP(g,4), 2);
		
		
		// Set pin C as output low
		setPinState(PIN_IN_GROUP(g,1), INPUT) ;
		setPinState(PIN_IN_GROUP(g,2), OUTPUT) ;
		setPinValue (PIN_IN_GROUP(g,2), 0) ;
		delay(1000) ;
		
		// Fail if A is high or if B or D is low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 1) failPin(PIN_IN_GROUP(g,0), 3);
		if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 3);
		if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 3);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 1) failPin(PIN_IN_GROUP(g,4), 3);
		
		
		// Set pin D as output low
		setPinState(PIN_IN_GROUP(g,2), INPUT) ;
		setPinState(PIN_IN_GROUP(g,3), OUTPUT) ;
		setPinValue (PIN_IN_GROUP(g,3), 0) ;
		delay(1000) ;
		
		// Fail if B is high or if A or C is low
		if (getPinValue(PIN_IN_GROUP(g,0)) == 0) failPin(PIN_IN_GROUP(g,0), 4);
		if (getPinValue(PIN_IN_GROUP(g,1)) == 1) failPin(PIN_IN_GROUP(g,1), 4);
		if (getPinValue(PIN_IN_GROUP(g,2)) == 0) failPin(PIN_IN_GROUP(g,2), 4);
		if (g == 10 && getPinValue(PIN_IN_GROUP(g,4)) == 0) failPin(PIN_IN_GROUP(g,4), 4);
		
		setPinState(PIN_IN_GROUP(g,3), INPUT) ;
		
		
		// Test the bonus G10P1 pin
		if (g == 10) {
			// Set the bonus G10P1 pin as output low
			setPinState(PIN_IN_GROUP(g,4), OUTPUT) ;
			setPinValue (PIN_IN_GROUP(g,4), 0) ;
			delay(1000) ;
			
			// Fail if A or C is high or if B or D is low
			if (getPinValue(PIN_IN_GROUP(g,0)) == 1) failPin(PIN_IN_GROUP(g,0), 4);
			if (getPinValue(PIN_IN_GROUP(g,1)) == 0) failPin(PIN_IN_GROUP(g,1), 4);
			if (getPinValue(PIN_IN_GROUP(g,2)) == 1) failPin(PIN_IN_GROUP(g,2), 4);
			if (getPinValue(PIN_IN_GROUP(g,3)) == 0) failPin(PIN_IN_GROUP(g,3), 4);
			
			setPinState(PIN_IN_GROUP(g,4), INPUT) ;
		}
	}
	
	PMD1bits.AD1MD = 0;
	PMD3bits.AD2MD = 0;
}

