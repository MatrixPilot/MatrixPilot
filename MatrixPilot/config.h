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


////////////////////////////////////////////////////////////////////////////////
// config.h
// 
// This file includes all of the user configurable runtime options. By default
// they are set to default values as defined in options.h, however they can be 
// overridden by fields in an optional config.ini file on the flash filesystem.
// 

struct network_module_bits {
	uint16_t unused			: 3;
	uint16_t uart1			: 1;
	uint16_t uart2			: 1;
	uint16_t flybywire		: 1;
	uint16_t mavlink		: 1;
	uint16_t debug			: 1;
	uint16_t adsb			: 1;
	uint16_t logo			: 1;
	uint16_t cam_tracking	: 1;
	uint16_t gpstest		: 1;
	uint16_t pwmreport		: 1;
	uint16_t xplane			: 1;
	uint16_t telemetry_extra: 1;
	uint16_t ground_station	: 1;
};

union network_module_word { struct network_module_bits _; int16_t W; };

extern union network_module_word network_modules;

/*
struct dcm_flag_bits {
	uint16_t unused					: 4;
	uint16_t rollpitch_req			: 1;
	uint16_t gps_history_valid		: 1;
	uint16_t dead_reckon_enable		: 1;
	uint16_t reckon_req				: 1;
	uint16_t first_mag_reading		: 1;
	uint16_t mag_drift_req			: 1;
	uint16_t yaw_req				: 1;
	uint16_t skip_yaw_drift			: 1;
	uint16_t nav_capable			: 1;
	uint16_t nmea_passthrough		: 1; // only used by ublox
	uint16_t init_finished			: 1;
	uint16_t calib_finished			: 1;
};

//extern union dcm_fbts_word { struct dcm_flag_bits _; int16_t W; } 

union dcm_fbts_word dcm_flags;

 */
/*
struct option_bits {
	uint16_t unused						: 5;
	uint16_t IsDirty					: 1;
};
 */

struct config_bits {
	uint16_t unused						: 3;
	uint16_t IsDirty					: 1;
	uint16_t RollStabilizaionAilerons	: 1;
	uint16_t RollStabilizationRudder	: 1;
	uint16_t PitchStabilization			: 1;
	uint16_t YawStabilizationRudder		: 1;
	uint16_t YawStabilizationAileron	: 1;
	uint16_t AileronNavigation			: 1;
	uint16_t RudderNavigation			: 1;
	uint16_t AltitudeholdStabilized		: 2;
	uint16_t AltitudeholdWaypoint		: 2;
	uint16_t RacingMode					: 1;
};

union config_word { struct config_bits _; int16_t W; };

extern union config_word config;

void init_config(void);


struct gains_variables {
	uint16_t IsDirty;
	float YawKPAileron;
	float YawKDAileron;
	float RollKP;
	float RollKD;
	float AileronBoost;
	float Pitchgain;
	float PitchKD;
	float RudderElevMix;
	float RollElevMix;
	float ElevatorBoost;
	float YawKPRudder;
	float YawKDRudder;
	float RollKPRudder;
	float RollKDRudder;
	float RudderBoost;
	float RtlPitchDown;
	float HeightTargetMax;
	float HeightTargetMin;
	float AltHoldThrottleMin;
	float AltHoldThrottleMax;
	float AltHoldPitchMin;
	float AltHoldPitchMax;
	float AltHoldPitchHigh;

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

extern struct gains_variables gains;

void init_gains(void);
