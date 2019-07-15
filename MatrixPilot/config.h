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

// TODO: bad header file name, implies it has editable options, which it does not

////////////////////////////////////////////////////////////////////////////////
// config.h
// 
// This file includes all of the structures which represent the user 
// configurable runtime options. 
// By default they are set to default values as defined in options.h, 
// however they can be overridden by fields in an optional config.ini file on
// one of the filesystems.
// 

struct network_module_bits {
	uint16_t unused                     : 3;
	uint16_t uart1                      : 1;
	uint16_t uart2                      : 1;
	uint16_t flybywire                  : 1;
	uint16_t mavlink                    : 1;
	uint16_t debug                      : 1;
	uint16_t adsb                       : 1;
	uint16_t logo                       : 1;
	uint16_t cam_tracking               : 1;
	uint16_t gpstest                    : 1;
	uint16_t pwmreport                  : 1;
	uint16_t xplane                     : 1;
	uint16_t telemetry_extra            : 1;
	uint16_t ground_station             : 1;
};

union network_module_word { struct network_module_bits _; int16_t W; };

/*
struct option_bits {
	uint16_t unused                     : 5;
	uint16_t IsDirty                    : 1;
};
 */

struct settings_bits {
	uint16_t unused                     : 3;
	uint16_t IsDirty                    : 1;
	uint16_t RollStabilizaionAilerons   : 1;
	uint16_t RollStabilizationRudder    : 1;
	uint16_t PitchStabilization         : 1;
	uint16_t YawStabilizationRudder     : 1;
	uint16_t YawStabilizationAileron    : 1;
	uint16_t AileronNavigation          : 1;
	uint16_t RudderNavigation           : 1;
	uint16_t AltitudeholdStabilized     : 2;
	uint16_t AltitudeholdWaypoint       : 2;
	uint16_t RacingMode	                : 1;
};

union settings_word { struct settings_bits _; int16_t W; };


struct gains_variables {
	uint16_t IsDirty;
	float YawKPAileron;
	float YawKDAileron;
	float RollKP;
	float RollKD;
	float RollKA;
	float AileronBoost;
	float Pitchgain;
	float PitchKD;
	float PitchKA;
	float RudderElevMix;
	float RollElevMix;
	float ElevatorBoost;
	float YawKPRudder;
	float YawKDRudder;
	float YawKARudder;
	float RollKPRudder;
	float RollKDRudder;
	float RudderBoost;
	float RtlPitchDown;
         float ThrottleKP;
         float ThrottleKD;
         float ThrottleKA;
         float ThrottleKI;
};
	
struct altit_variables {
	float DesiredSpeed;
	float HeightMargin;
	float HeightTargetMax;
	float HeightTargetMin;
	float AltHoldThrottleMin;
	float AltHoldThrottleMax;
	float AltHoldPitchMin;
	float AltHoldPitchMax;
	float AltHoldPitchHigh;
};

struct hover_variables {
	float HoverRollKP;
	float HoverRollKD;
	float HoverPitchGain;
	float HoverPitchKD;
	float HoverPitchOffset;
	float HoverYawKP;
	float HoverYawKD;
	float HoverYawOffset;
	float HoverPitchTowardsWP;
	float HoverNavMaxPitchRadius;
};

struct turns_variables {
//	float FeedFwd;
//	float RateNav;
//	float RateFbw;
//	float CrseSpd;
//	float AoANorm;
//	float AoAInvt;
//	float ElvNorm;
//	float ElvInvt;
	float FeedForward;
	float TurnRateNav;
	float TurnRateFBW;
	float RefSpeed;
	float AngleOfAttackNormal;
	float AngleOfAttackInverted;
	float ElevatorTrimNormal;
	float ElevatorTrimInverted;
};

extern struct gains_variables gains;
extern struct altit_variables altit;
extern struct hover_variables hover;
extern struct turns_variables turns;
extern union settings_word settings;
//extern union network_module_word network_modules;

void config_load(void);
void config_save(void);
void config_init(void);
