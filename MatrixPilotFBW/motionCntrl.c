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

// Motion control module
// Responsible for calculating aircraft movements

// trigonomentry lookup tables are at the bottom.

#include "../MatrixPilot/defines.h"
#include "motionCntrl.h"


//long tan(signed char angle)
//{
//	union longww temp = {0};
//	fractional sina = sine(angle);
//	fractional cosa = cosine(angle);
//
//	if(cosa == 0)
//	{
//		if(angle > 0)
//			return 0x7FFF;
//		else
//			return -0x8000;
//	}
//
//	temp.WW = __builtin_divss(temp.WW, cosa);
//}
//

// tan function returning a BYTE FLOAT 
//extern BYTE_FLOAT tanb(signed char angle);

// tan function returning a long integer fractional where lower word is the fraction
extern long tanli(signed char angle)
{
	signed char tempAngle = angle;
	int mant;
	int exp;

	if(angle == 0) 
		return 0;
	else if(angle == 64)
		return 0x7FFF;
	else if(angle == -64)
		return 0x8000;

	if(tempAngle > 64)
		tempAngle -= 128;

	if(tempAngle >= 0)
	{
		mant = (int) tan_table[tempAngle].mantissa;
		exp = (int) tan_table[tempAngle].exponent;
	}
	else
	{
		mant = (int) -tan_table[-tempAngle].mantissa;
		exp = (int) tan_table[-tempAngle].exponent;
	}
	
	return ( (long) mant) << exp ;
	
}

// Calculate the estimated earth based turn rate in byte circular per second.
// This is based on airspeed and bank angle for level flight.
// Can be multiple of byte to represent > 180deg per second. Max 127 rotations / sec.
// Takes airspeed as cm/s
fractional calc_turn_rate(fractional bank_angle, int airspeed)
{
	union longww temp;
	// Convert to dm
	temp.WW = __builtin_mulss (airspeed , (RMAX * 0.1) ) ;
	temp.WW <<= 1;
	if(temp._.W0 & 0x8000)
		temp._.W1++;

	// airspeed^2
	temp.WW = __builtin_mulss (temp._.W1 , temp._.W1 ) ;
	
	union longww tanx;
	tanx.WW = tanli( (signed char) bank_angle >> 8);

	// Divide acceleration by airpseed^2
	temp._.W1 = __builtin_divsd (tanx.WW , temp._.W1 ) ;
	return temp._.W1;
};



const BYTE_FLOAT tan_table[63] = {
{	0	,	0	},
{	13	,	-5	},
{	13	,	-4	},
{	9	,	-3	},
{	13	,	-3	},
{	16	,	-3	},
{	9	,	-2	},
{	11	,	-2	},
{	13	,	-2	},
{	14	,	-2	},
{	8	,	-1	},
{	9	,	-1	},
{	10	,	-1	},
{	11	,	-1	},
{	11	,	-1	},
{	12	,	-1	},
{	13	,	-1	},
{	14	,	-1	},
{	15	,	-1	},
{	8	,	0	},
{	9	,	0	},
{	9	,	0	},
{	10	,	0	},
{	10	,	0	},
{	11	,	0	},
{	11	,	0	},
{	12	,	0	},
{	12	,	0	},
{	13	,	0	},
{	14	,	0	},
{	15	,	0	},
{	15	,	0	},
{	8	,	0	},
{	8	,	1	},
{	9	,	1	},
{	9	,	1	},
{	10	,	1	},
{	10	,	1	},
{	11	,	1	},
{	11	,	1	},
{	12	,	1	},
{	13	,	1	},
{	13	,	1	},
{	14	,	1	},
{	15	,	1	},
{	11	,	1	},
{	8	,	2	},
{	9	,	2	},
{	10	,	2	},
{	10	,	2	},
{	11	,	2	},
{	12	,	2	},
{	13	,	2	},
{	14	,	2	},
{	16	,	2	},
{	9	,	3	},
{	10	,	3	},
{	12	,	3	},
{	13	,	3	},
{	8	,	4	},
{	10	,	4	},
{	14	,	4	},
{	10	,	5	},
{	10	,	6	},
{	15	,	15	},

};