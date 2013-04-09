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

// Options for navigaton

#ifndef NAV_OPTIONS_H
#define NAV_OPTIONS_H

#define DEFAULT_LOITER_RADIUS 		30.0

#define NAV_MAX_R_ANGLE				50.0

// Roll rate used during navigation
#define NAV_ROLL_RATE_DEFAULT		25.0

// Standard roll rate for comparison during navigation calculation
#define NAV_ROLL_RATE_STANDARD		25.0

#define NAV_ASPD_STANDARD			12.0

// Fiddle factor gain for navigation error to roll position
#define AUTO_NAV_ROLL_GAIN			0.45

#endif

