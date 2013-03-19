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

#ifndef FBWCNTRL_H
#define FBWCNTRL_H

#include "inputCntrl.h"
#include <libq.h>

typedef enum
{
	FBW_ROLL_MODE_STABILISE,
	FBW_ROLL_MODE_RATE,
	FBW_ROLL_MODE_RATE_AND_STABILISE,
	FBW_ROLL_MODE_POSITION,
} FBW_ROLL_MODE;

typedef enum
{
	FBW_ASPD_MODE_STABILISE_CRUISE,
	FBW_ASPD_MODE_LOGO,
	FBW_ASPD_MODE_THROTTLE,
	FBW_ASPD_MODE_CAMBER,
	FBW_ASPD_MODE_CAMBER_AND_PITCH,
	FBW_ASPD_MODE_PITCH,
} FBW_ASPD_MODE;

typedef enum
{
	FBW_PITCH_MODE_NONE,
	FBW_PITCH_MODE_PITCH,
	FBW_PITCH_MODE_ASPD,
} FBW_PITCH_MODE;


typedef enum
{
	FBW_ALTITUDE_MODE_MANUAL,
	FBW_ALTITUDE_MODE_STANDARD,					// Use original matrixpilot flags
	FBW_ALTITUDE_MODE_THROTTLE_ALT_ABSOLUTE,	// Throttle is altitude
	FBW_ALTITUDE_MODE_THROTTLE_CLIMB_RATE,		// Throttle is climb rate
	FBW_ALTITUDE_MODE_PITCH_CLIMB_RATE,			// Pitch is climb rate
} FBW_ALTITUDE_MODE;


// This must only be included after the FBW modes are defined above.
#include "fbw_options.h"

// Must be defined after fbw_options.h
#define THROTTLE_DEADBAND_RMAX ( (int)( ( (long)RMAX * (long)THROTTLE_DEADBAND) / 100) )
#define THROTTLE_DEADBAND_GAIN ( (int)( ( (long)RMAX * (long)RMAX) / (RMAX - THROTTLE_DEADBAND_RMAX)) )

// Get modes of fbw operation
extern FBW_ROLL_MODE fbw_get_roll_mode(void);
extern FBW_ALTITUDE_MODE fbw_get_altitude_mode(void);
extern FBW_ASPD_MODE fbw_get_airspeed_mode(void);
extern FBW_PITCH_MODE fbw_get_pitch_mode(void);

// outputs of fbw demand.
extern inline _Q16 fbw_desiredRollPosition(void);
extern inline _Q16 fbw_desiredTurnRate(void);
extern inline _Q16 fbw_desiredPitchPosition(void);


// Get the roll demand from fly-by-wire
inline _Q16 fbwGetRollDemand(void);

// Get the demand altitude from user input
inline long get_fbw_demand_altitude(void);

// Process user input into fly by wire demands
extern void fbwDemandCntrl(void);

extern boolean fbwManualControlLockout(IN_CNTRL channel);

#endif

