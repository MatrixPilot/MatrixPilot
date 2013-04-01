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


float floatvals[] = {0.0, 1.0, -1.0, 1.1, -1.1,
					63.0, 64.0, 65.0, 
					-63.0, -64.0, -65.0,
					4095.0, 4096.0, 4097.0,
					-4095.0, -4096.0, -4097.0,
					0.09, 0.1, 0.11,
					-0.09, -0.1, -0.11,
					0.00001, 0.000011, 0.0000099,
		 			-0.00001, -0.000011, -0.0000099,
					1E-10, -1E-10,
					1E10, -1E10 };

//float floatvals[] = {1.0, 63.0};

const int16_t count = sizeof(floatvals) / sizeof(float);

int16_t results[2048];

//	main program for testing the IMU.

int16_t Q16convtest(void);
int16_t floattest(void);

int16_t Q16multtest(void);
int16_t Q16divtest(void);
int16_t Q16addtest(void);
int16_t Q16sqrttest(void);

int16_t multtest(void);
int16_t divtest(void);
int16_t addtest(void);
int16_t sqrttest(void);

int16_t compare(float target, float result);
int16_t compare_tol(float target, float result, float tolh, float toll);
int16_t compare_offset(float target, float result, float offset);
int16_t check_mf(minifloat mf);

void clear_results(void)
{
	int16_t index = sizeof(results);
	for (index = 0; index < ((sizeof(results) / sizeof(int16_t))); index ++)
	{
		results[index] = -1;
	}
}

int32_t errorcount = 0;


int16_t main (void)
{
//	clear_results();
//	Q16convtest();
	clear_results();
	floattest();

	multtest();
	clear_results();
	divtest();
	clear_results();
	addtest();
	clear_results();
	sqrttest();
	return 0 ;
}



int16_t Q16convtest(void)
{
	float a;
	minifloat mfa;

	int16_t index;
	
	_Q16 tempQ16;

	
	// Q16 conversion test
	for(index = 0; index < count; index++)
	{
		a = floatvals[index];
		a *= 0x10000;
		tempQ16 = (int32_t) a;

		mfa = Q16tomf(tempQ16);
		
		tempQ16 = mftoQ16(mfa);

		a = (float) tempQ16;
		a /= 0x10000;

		results[index] = compare_tol(floatvals[index], a, 1.01, 0.99);
		if(results[index] != 1) 
					errorcount++;;

	}

	return errorcount;
}


int16_t Q16multtest(void)
{
	float a;
	float b;
	float target;
	float result;
	minifloat mfa;
	minifloat mfb;
	minifloat mf;

	int16_t index = 0;
	int16_t indexa;
	int16_t indexb;

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
			if(target > 32767)
				results[index] = 2;
			else if (target < -32767)
				results[index] = 2;
			else if ((target > 0) && (target < (256.0 / 65536) ))
					results[index] = 2;
			else if ((target < 0) && (target > -(256.0 / 65536)))
					results[index] = 2;
			else
			{	
				a *= 0x10000;
				tempQ16 = (int32_t) a;	
				mfa = Q16tomf(tempQ16);
	
				b *= 0x10000;
				tempQ16 = (int32_t) b;
				mfb = Q16tomf(tempQ16);
	
				mf = mf_mult(mfa, mfb);
				tempQ16 = mftoQ16(mf);
	
				result = (float) tempQ16;
				result /= 0x10000;
	
				results[index] = compare_tol(target, result, 1.02, 0.98);
				if(results[index] != 1) 
						errorcount++;;

			}
			index++;
		}
	}

	return errorcount;
}



int16_t multtest(void)
{
	float a;
	float b;
	float target;
	float result;
	minifloat mfa;
	minifloat mfb;
	minifloat mf;

	int16_t index = 0;
	int16_t indexa;
	int16_t indexb;

	for(indexa = 0; indexa < count; indexa++)
	{
		for(indexb = 0; indexb < count; indexb++)
		{
			a = floatvals[indexa];
			b = floatvals[indexb];
			target = a * b;

			// Check if valid test
			if(1 == 0)
				results[index] = 10;
			else
			{	
				mfa = ftomf(a);
				mfb = ftomf(b);
	
				mf = mf_mult(mfa, mfb);
				
				result = mftof(mf);
	
				results[index] = compare_tol(target, result, 1.02, 0.98);
				if(results[index] != 1) 
						errorcount++;;

			}
			index++;
		}
	}

	return errorcount;
}



int16_t divtest(void)
{
	float a;
	float b;
	float target;
	float result;
	minifloat mfa;
	minifloat mfb;
	minifloat mf;

	int16_t index = 0;
	int16_t indexa;
	int16_t indexb;
	
	for(indexa = 0; indexa < count; indexa++)
	{
		for(indexb = 0; indexb < count; indexb++)
		{
			a = floatvals[indexa];
			b = floatvals[indexb];
			if(b != 0)
			{
				target = a / b;

				// Check if valid test
				if(1 == 0)
					results[index] = 10;
				else
				{	
					mfa = ftomf(a);
					mfb = ftomf(b);
		
					mf = mf_div(mfa, mfb);
					
					result = mftof(mf);
		
					results[index] = compare_tol(target, result, 1.01, 0.99);
					if(results[index] != 1) 
						errorcount++;;
				}
			}
			else
				results[index] = 3;

			index++;
		}
	}

	return errorcount;
}


int16_t Q16divtest(void)
{
	float a;
	float b;
	float target;
	float result;
	minifloat mfa;
	minifloat mfb;
	minifloat mf;

	int16_t index = 0;
	int16_t indexa;
	int16_t indexb;
	
	_Q16 tempQ16;

	for(indexa = 0; indexa < count; indexa++)
	{
		for(indexb = 0; indexb < count; indexb++)
		{
			a = floatvals[indexa];
			b = floatvals[indexb];
			if(b != 0)
			{
				target = a / b;

				// Check if valid test
				if(target > 32767)
					results[index] = 2;
				else if (target < -32767)
					results[index] = 2;
				else if ((target > 0) && (target < (256.0 / 65536) ))
					results[index] = 2;
				else if ((target < 0) && (target > -(256.0 / 65536)))
					results[index] = 2;
				else
				{	
					a *= 0x10000;
					tempQ16 = (int32_t) a;	
					mfa = Q16tomf(tempQ16);
		
					b *= 0x10000;
					tempQ16 = (int32_t) b;
					mfb = Q16tomf(tempQ16);
		
					mf = mf_div(mfa, mfb);
					tempQ16 = mftoQ16(mf);
		
					result = (float) tempQ16;
					result /= 0x10000;
		
					results[index] = compare_tol(target, result, 1.01, 0.99);
					if(results[index] != 1) 
						errorcount++;;
				}
			}
			else
				results[index] = 3;

			index++;
		}
	}

	return errorcount;
}


int16_t Q16addtest(void)
{
	float a;
	float b;
	float target;
	float result;
	minifloat mfa;
	minifloat mfb;
	minifloat mf;

	int16_t index = 0;
	int16_t indexa;
	int16_t indexb;

	float amplitude;
	
	_Q16 tempQ16;
	
	// Q16 divide test
	for(indexa = 0; indexa < count; indexa++)
	{
		for(indexb = 0; indexb < count; indexb++)
		{
			a = floatvals[indexa];
			b = floatvals[indexb];
			if(b != 0)
			{
				target = a + b;

				amplitude = sqrt( (a*a)  + (b*b) );

				// Check if valid test
				if(target > 32767)
					results[index] = 2;
				else if (target < -32767)
					results[index] = 2;
				else if ((target > 0) && (target < (256.0 / 65536) ))
					results[index] = 2;
				else if ((target < 0) && (target > -(256.0 / 65536)))
					results[index] = 2;
				else
				{	
					a *= 0x10000;
					tempQ16 = (int32_t) a;	
					mfa = Q16tomf(tempQ16);
		
					b *= 0x10000;
					tempQ16 = (int32_t) b;
					mfb = Q16tomf(tempQ16);
		
					mf = mf_add(mfa, mfb);
					tempQ16 = mftoQ16(mf);
		
					result = (float) tempQ16;
					result /= 0x10000;
		
					results[index] = compare_offset(target, result, amplitude * 0.05);
					if(results[index] != 1) 
						errorcount++;;
				}
			}
			else
				results[index] = 3;

			index++;
		}
	}

	return errorcount;
}



int16_t addtest(void)
{
	float a;
	float b;
	float target;
	float result;
	minifloat mfa;
	minifloat mfb;
	minifloat mf;

	int16_t index = 0;
	int16_t indexa;
	int16_t indexb;

	float amplitude;
	
	_Q16 tempQ16;
	
	// Q16 divide test
	for(indexa = 0; indexa < count; indexa++)
	{
		for(indexb = 0; indexb < count; indexb++)
		{
			a = floatvals[indexa];
			b = floatvals[indexb];
			if(b != 0)
			{
				target = a + b;

				amplitude = sqrt( (a*a)  + (b*b) );

				// Check if valid test
				if(1 == 0)
					results[index] = 10;
				else
				{	
					mfa = ftomf(a);
					mfb = ftomf(b);
		
					mf = mf_add(mfa, mfb);
					
					result = mftof(mf);
		
					results[index] = compare_offset(target, result, amplitude * 0.05);
					if(results[index] != 1) 
						errorcount++;;
				}
			}
			else
				results[index] = 3;

			index++;
		}
	}

	return errorcount;
}


int16_t Q16sqrttest(void)
{
	float a;
	float target;
	float result;
	minifloat mfa;
	minifloat mf;

	int16_t index = 0;

	float amplitude;
	
	_Q16 tempQ16;
	
	// Q16 divide test
	for(index = 0; index < count; index++)
	{
		a = floatvals[index];

		if(a >= 0)
		{
			target = sqrt(a) ;
			amplitude = target;

			// Check if valid test
			if(target > 32767)
				results[index] = 2;
			else if (target < -32767)
				results[index] = 2;
			else if ((target > 0) && (target < (256.0 / 65536) ))
				results[index] = 2;
			else if ((target < 0) && (target > -(256.0 / 65536)))
				results[index] = 2;
			else
			{	
				mfa = ftomf(a);
	
				mf = mf_sqrt(mfa);

				tempQ16 = mftoQ16(mf);		
				result = (float) tempQ16;
				result /= 0x10000;
	
				results[index] = compare_offset(target, result, amplitude * 0.01);
				if(results[index] != 1) 
					errorcount++;;
			}
		}
		else
			results[index] = 3;

	}

	return errorcount;
}


int16_t sqrttest(void)
{
	float a;
	float target;
	float result;
	minifloat mfa;
	minifloat mf;

	int16_t index = 0;

	float amplitude;
	
	_Q16 tempQ16;
	
	// Q16 divide test
	for(index = 0; index < count; index++)
	{
		a = floatvals[index];

		if(a >= 0)
		{
			target = sqrt(a) ;
			amplitude = target;

			// Check if valid test
			if(1 == 0)
					results[index] = 10;
			else
			{	
				mfa = ftomf(a);
		
				mf = mf_sqrt(mfa);
					
				result = mftof(mf);
	
				results[index] = compare_offset(target, result, amplitude * 0.01);
				if(results[index] != 1) 
					errorcount++;;
			}
		}
		else
			results[index] = 3;

	}

	return errorcount;
}


int16_t floattest(void)
{
	float a;
	minifloat mfa;

	int16_t index;
		
	// Q16 conversion test
	for(index = 0; index < count; index++)
	{
		a = floatvals[index];

		mfa = ftomf(a);
		
		a = mftof(mfa);

		results[index] = compare_tol(floatvals[index], a, 1.01, 0.99);
		if(results[index] != 1) 
					errorcount++;;

	}

	return errorcount;
}



int16_t compare_tol(float target, float result, float tolh, float toll)
{
	float b = target * tolh;	

	if(b > 0)
	{
		if(result > b)return 0;
	}
	else
	{
		if(result < b)return 0;
	}

	b = target * toll;

	if(b > 0)
	{
		if(result < b) return 0;
	}
	else
	{
		if(result > b) return 0;
	}
	return 1;
}


int16_t compare(float target, float result)
{
	return compare_tol(target, result, 1.05, 0.95);
}


int16_t compare_offset(float target, float result, float offset)
{
	float b = target + offset;	
	if(result > b)
		return 0;

	b = target - offset;
	if(result < b) 
		return 0;

	return 1;
}


int16_t check_mf(minifloat mf)
{
	if(mf.mant == 0) return 1;

	int16_t mant = mf.mant;
	if( (mant > 0) && (mant < 0x80))
		return 5;
	if( (mant < 0) && (mant > -0x80))
		return 5;
	return 1;
}