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

#ifndef AIRSPEEDCNTRL_H
#define AIRSPEEDCNTRL_H

#include "../matrixpilot/airspeed_options.h"
#include "airframe.h"

typedef struct tagCondition
{
	aero_data airdata;
	int16_t airspeed_target;
} sCondition;

extern int16_t 		target_airspeed;
extern int16_t 		airspeedError;

extern int16_t 		minimum_airspeed;
extern int16_t 		maximum_airspeed;
extern int16_t 		cruise_airspeed;

// Feedforward values of pitch 
extern int16_t 		airspeed_pitch_min_aspd;
extern int16_t 		airspeed_pitch_max_aspd;

// Run all airspeed control calculations and filters
// Do this before running any of the functions below
extern void airspeedCntrl(void);

extern int32_t get_speed_height_error(void);

inline int16_t get_filtered_airspeed( void );

//Calculate and return pitch target adjustment for target airspeed
// return value is in dcm scale angle
// For gliding only based on PI control.
//extern fractional gliding_airspeed_pitch_adjust(void);

//Calculate and return pitch target adjustment for target airspeed
// return value is Q16 radians. Positive is pitch up.
// Based on total energy control
extern _Q16 airspeed_pitch_adjust(fractional throttle, int16_t actual_aspd, int16_t target_aspd, int16_t min_airspeed, int32_t aspd_potential_error);


#endif

