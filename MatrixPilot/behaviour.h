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

#ifndef _BEHAVIOUR_H_
#define _BEHAVIOUR_H_

struct behavior_flag_bits {
	uint16_t takeoff        : 1;    // disable altitude interpolation for faster climbout
	uint16_t inverted       : 1;    // fly iverted
	uint16_t hover          : 1;    // hover the plane
	uint16_t rollLeft       : 1;    // unimplemented
	uint16_t rollRight      : 1;    // unimplemented
	uint16_t trigger        : 1;    // trigger action
	uint16_t loiter         : 1;    // stay on the current waypoint
	uint16_t land           : 1;    // throttle off
	uint16_t absolute       : 1;    // absolute waypoint
	uint16_t altitude       : 1;    // climb/descend to goal altitude
	uint16_t cross_track    : 1;    // use cross-tracking navigation
	uint16_t unused         : 5;
};

union bfbts_word { struct behavior_flag_bits _; int16_t W; };

#define F_NORMAL               0
#define F_TAKEOFF              1
#define F_INVERTED             2
#define F_HOVER                4
#define F_ROLL_LEFT            8
#define F_ROLL_RIGHT          16
#define F_TRIGGER             32
#define F_LOITER              64
#define F_LAND               128
#define F_ABSOLUTE           256
#define F_ALTITUDE_GOAL      512
#define F_CROSS_TRACK       1024

#define TRIGGER_TYPE_NONE      0
#define TRIGGER_TYPE_SERVO     1
#define TRIGGER_TYPE_DIGITAL   2

#define TRIGGER_PULSE_HIGH     4
#define TRIGGER_PULSE_LOW      8
#define TRIGGER_TOGGLE        16
#define TRIGGER_REPEATING     32

extern int16_t current_orientation;
extern union bfbts_word desired_behavior;

void init_behavior(void);
void setBehavior(int16_t newBehavior);
void updateBehavior(void);
void updateTriggerAction(void);
boolean canStabilizeInverted(void);
boolean canStabilizeHover(void);

#endif // _BEHAVIOUR_H_
