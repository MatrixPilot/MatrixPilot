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

#ifndef FBW_OPTIONS_H
#define FBW_OPTIONS_H

// Use the input control module which converts all RC inputs to Q14 scaled with standardised direction.
// Also activates output control.
#define USE_INPUT_CONTROL 	1

#define USE_FBW				1

// Linear multiplex of controls instead of boost
// At neutral manual input AP output is 100%, manual is 0%
// At maximum manual input AP output is 0%, manual is 100%
// AP and manual outputs are scaled to this
#define OUTPUT_CONTROL_GAIN_MUX		1

// When doing output gain mux, optionally mix manual and autopilot commands.
// Note that this does not do reversing.
#define OUT_CNTRL_AP_MAN_PREMIX		1

// Mixer outputs are not directly into udb_pwOut but in mixer_outputs
// post_pix will then safely copy the outputs to udb_pwOut
#define MIXER_OUTPUTS_TO_UDB		1

// Overide any previous throttle mixing with safe hard coded mixing.
#define DO_SAFE_THROTTLE_MIXING		1

#define DEFAULT_FBW_AIRSPEED_MODE	FBW_ASPD_MODE_CAMBER_AND_PITCH
#define DEFAULT_FBW_ROLL_MODE		FBW_ROLL_MODE_POSITION
#define DEFAULT_FBW_ALTITUDE_MODE	FBW_ALTITUDE_MODE_STANDARD
#define DEFAULT_FBW_PITCH_MODE		FBW_PITCH_MODE_ASPD

// Maximum roll angle for maximum roll input in degrees
#define FBW_ROLL_POSITION_MAX		60.0

// Maximum and minimum pitch demands for fly by wire
#define FBW_PITCH_MAX				60.0
#define FBW_PITCH_MIN				-60.0

// Deadband as a percentage of the throttle range 
#define THROTTLE_DEADBAND			5

#endif

