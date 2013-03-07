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


#include "../MatrixPilotFBW/minifloat.h"
//#include <maths.h>


float floatvals[] = {1.0, -1.0, 1.1, -1.1}; /*, 
					63.0, 64.0 65.0, 
					-63.0, -64.0, -65.0,
					4095.0, 4096.0, 4097.0,
					-4095.0, -4096.0, -4097.0,
					0.09, 0.1, 0.11,
					-0.09, -0.1, -0.11 }; */

const int count = sizeof(floatvals) / sizeof(float);

//	main program for testing the IMU.

int main (void)
{
	float a;
	float b;
	minifloat mfa;
	minifloat mfb;

	int index;
	int indexa;
	int indexb;
	
	long errorcount = 0;

	_Q16 tempQ16;

	for(index = 0; index < count; index++)
	{
		a = floatvals[index];
		a *= 0x10000;
		tempQ16 = (long) a;

		mfa = Q16tomf(tempQ16);
		
		tempQ16 = mftoQ16(mfa);

		a = (float) tempQ16;
		a /= 0x10000;

		if(a != floatvals[index])
		{
			errorcount++;
		}
	}
	
	return 0 ;
}


