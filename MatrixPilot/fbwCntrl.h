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

// Process user input into fly by wire demands
extern void fbwDemandCntrl(void);


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
	FBW_ASPD_MODE_CAMBER_AND_ELEVATOR,
	FBW_ASPD_MODE_ELEVATOR,
} FBW_ASPD_MODE;


// This must only be included after the FBW modes are defined above.
#include "fbw_options.h"

extern int fbw_roll_mode;
extern int fbw_altitude_mode;
extern int fbw_yaw_mode;
extern int fbw_airspeed_mode;

#endif

