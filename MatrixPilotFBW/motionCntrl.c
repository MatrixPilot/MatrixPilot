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

// earth horizontal turn acceleration in g
SHORT_FLOAT earth_turn_accn = { 0 };

// Rate in byte circular counts/s * 16
// Byte circular : 256 counts = 360 degrees.
// 360*16 = 1 rotation/ second
int earth_turn_rate = 0;

fractional earth_roll_angle = 0;
fractional earth_pitch_angle = 0;

// return tan of angle in short float with input in byte circular angle
extern SHORT_FLOAT tansf(signed char angle);

// Calculations for required motion before axis control are performed.
void motionCntrl(void)
{
	// Calculate earth based roll angle
	struct relative2D matrix_accum ;
	matrix_accum.x = rmat[8] ;
	matrix_accum.y = rmat[6] ;
	earth_roll_angle = rect_to_polar16(&matrix_accum) ;

	matrix_accum.x = rmat[8] ;
	matrix_accum.y = rmat[7] ;
	earth_pitch_angle = - rect_to_polar16(&matrix_accum) ;			// binary angle (0 to 65536 = 360 degrees)

	earth_turn_accn = tansf(earth_roll_angle >> 8) ;

	earth_turn_rate = calc_earth_turn_rate(earth_turn_accn , air_speed_3DIMU) ;
}

inline fractional get_earth_roll_angle(void) {return earth_roll_angle;}

inline fractional get_earth_pitch_angle(void) {return earth_pitch_angle;}

inline int get_earth_turn_rate(void) {return earth_turn_rate;}



// tan function returning a BYTE FLOAT 
//extern BYTE_FLOAT tanb(signed char angle);

// tan function returning a long integer fractional where lower word is the fraction
long tanli(signed char angle)
{
	signed char tempAngle = angle;
	int exponent;
	union longww temp = {0};

	if(angle == 0) 
		return 0;
	else if(angle == 64)
		return 0x7FFF;
	else if(angle == -64)
		return 0x8000;

	if(tempAngle > 64)
		tempAngle -= 128;
	else if(tempAngle < -64)
		tempAngle += 128;

	if(tempAngle >= 0)
	{
		temp._.W0 = (int) tan_table[tempAngle].mantissa << 12;
		exponent = (int) tan_table[tempAngle].exponent;
		temp.WW <<= exponent;
	}
	else
	{
		temp._.W0 = (int) -tan_table[-tempAngle].mantissa << 12;
		exponent = (int) tan_table[-tempAngle].exponent;
		temp.WW <<= exponent;
	}

	return temp.WW ;	
}

// tan function returning a short float
extern SHORT_FLOAT tansf(signed char angle)
{
	signed char tempAngle = angle;

	SHORT_FLOAT sf = {0,0};

	if(angle == 0) 
		return sf;
	else if(angle == 64)
	{
		sf.exponent = 127;
		sf.mantissa = -127;
		return sf;
	}
	else if(angle == -64)
	{
		sf.exponent = 127;
		sf.mantissa = -127;
		return sf;
	}

	if(tempAngle > 64)
		tempAngle -= 128;
	else if(tempAngle < -64)
		tempAngle += 128;

	if(tempAngle >= 0)
	{
		sf.mantissa = (int) tan_table[tempAngle].mantissa;
		sf.exponent = (int) tan_table[tempAngle].exponent;
	}
	else
	{
		sf.mantissa = (int) -tan_table[-tempAngle].mantissa;
		sf.exponent = (int) tan_table[-tempAngle].exponent;
	}

	return sf ;	
}


// turn accn in g
inline SHORT_FLOAT calc_turn_accn_from_angle(fractional bank_angle)
{
	SHORT_FLOAT tanx;
	tanx.mantissa = (bank_angle >> 8);
	tanx = tansf( (signed char) tanx.mantissa );
	// TODO, take care of out of range values at +-PI/2
	return tanx;
}


// turn accn in g
inline SHORT_FLOAT calc_turn_accn_from_rmat(fractional rmat)
{
	SHORT_FLOAT tanx = { 0 };
	return tanx;
}




// Calculate the estimated earth based turn rate in byte circular per second.
// This is based on airspeed and bank angle for level flight.
// Takes airspeed as cm/s
// returns byte circular*16
int calc_earth_turn_rate(SHORT_FLOAT earth_turn_g, int airspeed)
{
	union longww temp;

	// Convert from cm/s to m/s
	temp.WW = __builtin_mulss (airspeed , (RMAX * 0.01) ) ;
	temp.WW <<= 2;
	if(temp._.W0 & 0x8000)
		temp._.W1++;

	// If airspeed is zero, return zero
	if(temp._.W1 == 0)
		return 0;

	// Divide acceleration by airpseed to get angular rate
	temp._.W1 = __builtin_divsd ( ((fractional) earth_turn_g.mantissa) << 11, temp._.W1 ) ;
	temp._.W0 = 0x8000;
	int gain = (int) (earth_turn_g.exponent) - 1;
	if(gain < 0)
		temp.WW >>= -gain;
	else
		temp.WW <<= gain;
	// Shift by exponent - 1.  
	// This gives a little more maximum range to the turn rate. 11G turn at 8m/s is ok.

	// TODO: OVERFLOW RANGE CHECK ON POSITIVE EXPONENT.

	// Multiply by G acceleration and do a little ranging.
	// 10035 = INT(G/16) = RMAX*0.6125
	temp.WW = __builtin_mulss (temp._.W1 , 10035 ) ;
	temp.WW <<= 3;

	return temp._.W1;
};

// Calculate the pitch rate due to turning when banked
// bank angle in fractional Q14 from dcm.
// Turn rate in 16*byte circular per second.
int calc_turn_pitch_rate(fractional bank_angle, int turn_rate)
{
	union longww temp;
	temp.WW = __builtin_mulss (bank_angle , turn_rate ) ;
	temp.WW <<= 2;
	if(temp._.W0 & 0x8000)
		temp._.W1++;
	return temp._.W1; 	
}

int calc_turn_yaw_rate(fractional bank_angle, int turn_rate)
{
	union longww temp;
	temp.WW = __builtin_mulss (RMAX-bank_angle , turn_rate ) ;
	temp.WW <<= 2;
	if(temp._.W0 & 0x8000)
		temp._.W1++;
	return temp._.W1; 	
}


const BYTE_FLOAT tan_table[63] = {
{	0	,	0	},
{	13	,	-5	},
{	13	,	-4	},
{	9	,	-3	},
{	13	,	-3	},
{	8	,	-2	},
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
{	8	,	3	},
{	9	,	3	},
{	10	,	3	},
{	12	,	3	},
{	13	,	3	},
{	8	,	4	},
{	10	,	4	},
{	14	,	4	},
{	10	,	5	},
{	10	,	6	},

};
