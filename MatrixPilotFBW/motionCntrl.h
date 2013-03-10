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

#include <libq.h>
#include "minifloat.h"

extern _Q16 get_earth_turn_rate(void);
extern _Q16 get_earth_turn_accn(void);

extern inline signed char get_airspeed_pitch_adjustment(void);

// Calculations for required motion before axis control are performed.
void motionCntrl(void);

// Calculate acceration 
int calc_reqd_centripetal_accn(int airspeed, int rotation_rate);

// Calculate the rate of pitch due to turning when aircraft is banked
// Input is g centripetal acceleration into the turn in horizontal earth frame
extern _Q16 calc_earth_turn_rate(_Q16 earth_turn_g, int airspeed);

// Calculate the rate of expected pitching around horizontal earth frame
// in the aircraft pitch axis.
// Inputs are turn rate (byte circular * 16) and roll position (frac rmat)
extern minifloat calc_turn_pitch_rate(_Q16 earth_turn_rate, fractional roll_pos);



#endif

