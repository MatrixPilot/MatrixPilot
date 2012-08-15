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

// Responsible for synthesizing motion demands according to inputs and limits

#ifndef MOTIONCNTLR_H
#define MOTIONCNTLR_H

// Control demand of position, rate and accelration
//typedef CNTRL_DEMAND fractional[3];

typedef struct tagBYTE_FLOAT
{
	unsigned int 	mantissa	: 4;
	int 			exponent	: 4;
} BYTE_FLOAT;

typedef struct tagSHORT_FLOAT
{
	signed char	 	mantissa;
	signed char	 	exponent;
} SHORT_FLOAT;

extern const BYTE_FLOAT tan_table[63];

// tan function returning a long fractional where lower word is the fraction
//extern long tanli(signed char angle);

// tan function returning a SHORT_FLOAT
//extern BYTE_FLOAT tansf(signed char angle);

// Calculations for required motion before axis control are performed.
void motionCntrl(void);

// Calculate the turn rate in byte circular per second.
// Can be multiple of byte to represent > 180deg per second. Max 127 rotations / sec.
// Takes airspeed as cm/s
//extern int calc_turn_rate(fractional bank_angle, fractional airspeed);

// Calculate the rate of pitch due to turning when aircraft is banked
// Input is g centripetal acceleration into the turn in horizontal earth frame
extern int calc_earth_turn_rate(SHORT_FLOAT earth_turn_g, int airspeed);

// Calculate the rate of expected pitching around horizontal earth frame
// in the aircraft pitch axis.
// Inputs are turn rate (byte circular * 16) and roll position (frac rmat)
extern int calc_turn_pitch_rate(int earth_turn_rate, fractional roll_pos);

extern inline int get_earth_turn_rate(void);

extern inline fractional get_earth_roll_angle(void);
extern inline fractional get_earth_pitch_angle(void);


#endif

