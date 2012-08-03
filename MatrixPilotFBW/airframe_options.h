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

// Options for fly-by-wire

#ifndef AIRFRAME_OPTIONS_H
#define AIRFRAME_OPTIONS_H

// Glide ratio when gliding
#define CRUISE_GLIDE_RATIO	20.0

// Maximum climb rate in m/s at maximum throttle
// Assumes linear relationship with throttle setting.
#define MAX_THROTTLE_CLIMB_RATE	14.0

// Max G including standard gravity.
#define MAX_G_POSITIVE			2.0
#define MAX_G_NEGATIVE			2.0

#endif

