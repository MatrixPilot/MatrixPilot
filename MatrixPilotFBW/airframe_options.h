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


/** AIRCRAFT GEOMETERY INPUTS HERE **/

// Aircraft mass in kg
#define AFRM_AIRCRAFT_MASS	6.5

// Wing area in square meters
#define AFRM_WING_AREA		1.05

// The ratio of wing area that is effective for generating lift
#define AFRM_EFFECTIVE_AREA_RATIO 0.9

// Mean aerodynamic wing chord
#define AFRM_MAC				0.22

// Centre of gravity measured at wing root from leading edge
#define AFRM_COFGX			0.092

// Centre of gravity - Not used yet
#define AFRM_COFGY			0

// Distance between the aerodynamic centres of wing and tail
#define AFRM_TAIL_MOMENT		1.20

// Tail area in square meters
#define AFRM_TAIL_AREA		0.08

// Wing pitch relative to fuselage
#define AFRM_NEUTRAL_PITCH		2.5

// Distance between the aerodynamic centres of wing and fin
#define AFRM_FIN_MOMENT		1.20

// Tail area in square meters
#define AFRM_FIN_AREA		0.08

// Aileron aerodynamic span used to calculate delta aoa for roll.
#define AFRM_AILERON_AERODYNAMIC_SPAN	1.8

// The authority that the aileron has over the rest of the wing.
// Value is 1 for perfect ailerons on a completely stiff wing
#define AFRM_AILERON_AUTHORITY	0.5

// The ratio of aileron chord to wing chord
#define AFRM_AILERON_CHORD_RATIO	0.26

// Air density in kg/m^3
#define AFRM_AIR_DENSITY	1.225

// Number of points on each polar for a particular airspeed
#define AFRM_OPT_POINTS_PER_POLAR 		2
#define AFRM_OPT_POLARS_PER_CONDITION 	1

// Glide ratio when gliding
#define CRUISE_GLIDE_RATIO	20.0

// Maximum climb rate in m/s at maximum throttle
// Assumes linear relationship with throttle setting.
#define MAX_THROTTLE_CLIMB_RATE	10.0

// Max G including standard gravity.
#define MAX_G_POSITIVE			5.0
#define MAX_G_NEGATIVE			-2.0

#endif

