// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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


#undef ROLL_STABILIZATION_AILERONS
#undef ROLL_STABILIZATION_RUDDER
#undef PITCH_STABILIZATION
#undef YAW_STABILIZATION_RUDDER
#undef YAW_STABILIZATION_AILERON
#undef AILERON_NAVIGATION
#undef RUDDER_NAVIGATION
#undef ALTITUDEHOLD_STABILIZED
#undef ALTITUDEHOLD_WAYPOINT
#undef RACING_MODE

#define ROLL_STABILIZATION_AILERONS config._.RollStabilizaionAilerons
#define ROLL_STABILIZATION_RUDDER config._.RollStabilizationRudder
#define PITCH_STABILIZATION config._.PitchStabilization
#define YAW_STABILIZATION_RUDDER config._.YawStabilizationRudder
#define YAW_STABILIZATION_AILERON config._.YawStabilizationAileron
#define AILERON_NAVIGATION config._.AileronNavigation
#define RUDDER_NAVIGATION config._.RudderNavigation
#define ALTITUDEHOLD_STABILIZED config._.AltitudeholdStabilized
#define ALTITUDEHOLD_WAYPOINT config._.AltitudeholdWaypoint
#define RACING_MODE config._.RacingMode

