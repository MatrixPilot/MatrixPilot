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


#include "options_airspeed.h"

extern int16_t airspeed;
extern int16_t groundspeed;

extern uint16_t target_airspeed;
extern int16_t airspeedError;

extern uint16_t minimum_airspeed;
extern uint16_t maximum_airspeed;
extern uint16_t cruise_airspeed;

// Feedforward values of pitch 
extern int16_t airspeed_pitch_min_aspd;
extern int16_t airspeed_pitch_max_aspd;

extern uint16_t minimum_groundspeed;
extern int16_t airspeed_pitch_adjust_rate;
extern int16_t airspeed_pitch_ki_limit;
extern fractional airspeed_pitch_ki;
extern fractional airspeed_pitch_kp;


void init_airspeedCntrl(void);
#if (AIRFRAME_TYPE == AIRFRAME_GLIDER)
inline int16_t get_overspeedBrake(void);
inline int16_t get_flapsSelected(void);
#endif

// Run all airspeed control calculations and filters
void airspeedCntrl(void);

//Calculate and return pitch target adjustment for target airspeed
// return value is in dcm scale angle 
fractional gliding_airspeed_pitch_adjust(void);
// gfm speed control
fractional quad_airspeed_pitch_adjust(void);


#endif // AIRSPEEDCNTRL_H
