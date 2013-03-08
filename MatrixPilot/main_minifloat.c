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


float floatvals[] = {1.0, -1.0, 1.1, -1.1,
					63.0, 64.0, 65.0, 
					-63.0, -64.0, -65.0,
					4095.0, 4096.0, 4097.0,
					-4095.0, -4096.0, -4097.0,
					0.09, 0.1, 0.11,
					-0.09, -0.1, -0.11 };

//float floatvals[] = {-1.0};

const int count = sizeof(floatvals) / sizeof(float);

int results[512];

//	main program for testing the IMU.

int Q16convtest(void);
int Q16multtest(void);

int compare(float target, float result);

long errorcount = 0;


int main (void)
{
	Q16convtest();
	Q16multtest();

	
	return 0 ;
}



int Q16convtest(void)
{
	float a;
	float b;
	minifloat mfa;

	int index;
	
	_Q16 tempQ16;

	
	// Q16 conversion test
	for(index = 0; index < count; index++)
	{
		a = floatvals[index];
		a *= 0x10000;
		tempQ16 = (long) a;

		mfa = Q16tomf(tempQ16);
		
		tempQ16 = mftoQ16(mfa);

		a = (float) tempQ16;
		a /= 0x10000;

		results[index] = compare(floatvals[index], a);
	}

	return errorcount;
}


int Q16multtest(void)
{
	float a;
	float b;
	float target;
	float result;
	minifloat mfa;
	minifloat mfb;
	minifloat mf;

	int index = 0;
	int indexa;
	int indexb;

	_Q16 tempQ16;
	
	// Q16 multiplication test
	for(indexa = 0; indexa < count; indexa++)
	{
		for(indexb = 0; indexb < count; indexb++)
		{
			a = floatvals[indexa];
			b = floatvals[indexb];
			target = a * b;

			// Check if valid test
			if(target > 65535)
				results[index] = 2;
			else if (target < -65535)
				results[index] = 2;
			else
			{	
				a *= 0x10000;
				tempQ16 = (long) a;	
				mfa = Q16tomf(tempQ16);
	
				b *= 0x10000;
				tempQ16 = (long) b;
				mfb = Q16tomf(tempQ16);
	
				mf = mf_mult(mfa, mfb);
				tempQ16 = mftoQ16(mf);
	
				result = (float) tempQ16;
				result /= 0x10000;
	
				results[index] = compare(target, result);
			}
			index++;
		}
	}

	return errorcount;
}

int compare(float target, float result)
{
	float b = target * 1.05;	

	if(b > 0)
	{
		if(result > b)
		{
			errorcount++;
			return 0;
		}
	}
	else
	{
		if(result < b)
		{
			errorcount++;
			return 0;
		}
	}

	b = target * 0.95;

	if(b > 0)
	{
		if(result < b)
		{
			errorcount++;
			return 0;
		}
	}
	else
	{
		if(result > b)
		{
			errorcount++;
			return 0;
		}
	}
	return 1;
}
