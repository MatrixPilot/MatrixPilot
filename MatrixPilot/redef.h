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

#define ROLL_STABILIZATION_AILERONS settings._.RollStabilizaionAilerons
#define ROLL_STABILIZATION_RUDDER settings._.RollStabilizationRudder
#define PITCH_STABILIZATION settings._.PitchStabilization
#define YAW_STABILIZATION_RUDDER settings._.YawStabilizationRudder
#define YAW_STABILIZATION_AILERON settings._.YawStabilizationAileron
#define AILERON_NAVIGATION settings._.AileronNavigation
#define RUDDER_NAVIGATION settings._.RudderNavigation
#define ALTITUDEHOLD_STABILIZED settings._.AltitudeholdStabilized
#define ALTITUDEHOLD_WAYPOINT settings._.AltitudeholdWaypoint
#define RACING_MODE settings._.RacingMode



#undef YAWKP_AILERON
#undef YAWKD_AILERON
#undef ROLLKP
#undef ROLLKD
#undef AILERON_BOOST

#undef PITCHGAIN
#undef PITCHKD
#undef RUDDER_ELEV_MIX
#undef ROLL_ELEV_MIX
#undef ELEVATOR_BOOST

#undef YAWKP_RUDDER
#undef YAWKD_RUDDER
#undef ROLLKP_RUDDER
#undef ROLLKD_RUDDER
#undef RUDDER_BOOST
#undef RTL_PITCH_DOWN

#undef THROTTLEKP
#undef THROTTLEKD
#undef THROTTLEKA
#undef THROTTLEKI

#undef HEIGHT_TARGET_MAX
#undef HEIGHT_TARGET_MIN
#undef ALT_HOLD_THROTTLE_MIN
#undef ALT_HOLD_THROTTLE_MAX

#undef ALT_HOLD_PITCH_MIN
#undef ALT_HOLD_PITCH_MAX
#undef ALT_HOLD_PITCH_HIGH

#undef HOVER_ROLLKP
#undef HOVER_ROLLKD
#undef HOVER_PITCHGAIN
#undef HOVER_PITCHKD
#undef HOVER_PITCH_OFFSET
#undef HOVER_YAWKP
#undef HOVER_YAWKD
#undef HOVER_YAW_OFFSET
#undef HOVER_PITCH_TOWARDS_WP
#undef HOVER_NAV_MAX_PITCH_RADIUS

#define YAWKP_AILERON gains.YawKPAileron
#define YAWKD_AILERON gains.YawKDAileron
#define ROLLKP gains.RollKP
#define ROLLKD gains.RollKD
#define AILERON_BOOST gains.AileronBoost
#define PITCHGAIN gains.Pitchgain
#define PITCHKD gains.PitchKD
#define RUDDER_ELEV_MIX gains.RudderElevMix
#define ROLL_ELEV_MIX gains.RollElevMix
#define ELEVATOR_BOOST gains.ElevatorBoost
#define YAWKP_RUDDER gains.YawKPRudder
#define YAWKD_RUDDER gains.YawKDRudder
#define ROLLKP_RUDDER gains.RollKPRudder
#define ROLLKD_RUDDER gains.RollKDRudder
#define RUDDER_BOOST gains.RudderBoost
#define THROTTLEKP gains.ThrottleKP
#define THROTTLEKD gains.ThrottleKD
#define THROTTLEKA gains.ThrottleKA
#define THROTTLEKI gains.ThrottleKI

#define RTL_PITCH_DOWN gains.RtlPitchDown
#define HEIGHT_TARGET_MAX gains.HeightTargetMax
#define HEIGHT_TARGET_MIN gains.HeightTargetMin
#define ALT_HOLD_THROTTLE_MIN gains.AltHoldThrottleMin
#define ALT_HOLD_THROTTLE_MAX gains.AltHoldThrottleMax
#define ALT_HOLD_PITCH_MIN gains.AltHoldPitchMin
#define ALT_HOLD_PITCH_MAX gains.AltHoldPitchMax
#define ALT_HOLD_PITCH_HIGH gains.AltHoldPitchHigh

#define HOVER_ROLLKP gains.HoverRollKP
#define HOVER_ROLLKD gains.HoverRollKD
#define HOVER_PITCHGAIN gains.HoverPitchGain
#define HOVER_PITCHKD gains.HoverPitchKD
#define HOVER_PITCH_OFFSET gains.HoverPitchOffset
#define HOVER_YAWKP gains.HoverYawKP
#define HOVER_YAWKD gains.HoverYawKD
#define HOVER_YAW_OFFSET gains.HoverYawOffset
#define HOVER_PITCH_TOWARDS_WP gains.HoverPitchTowardsWP
#define HOVER_NAV_MAX_PITCH_RADIUS gains.HoverNavMaxPitchRadius
