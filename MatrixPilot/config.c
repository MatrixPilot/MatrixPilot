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


#include "defines.h"
#include "config.h"
#include "altitude_config.h"
#include <stdio.h>

#if (USE_CONFIGFILE == 1)

#include "minIni.h"

union config_word config;
struct gains_variables gains;
union network_module_word nw_mod;

const char* strConfigFile = "config.ini";
const char* strNetwork = "NETWORK";
const char* strStabilise = "STABILISE";
const char* strNavigation = "NAVIGATION";
const char* strRoll = "ROLL";
const char* strPitch = "PITCH";
const char* strYaw = "YAW";
const char* strAltitude = "ALTITUDE";
const char* strRTL = "RTL";
const char* strHover = "HOVER";

char address[16];
char gateway[16];
char subnet[16];
char dhcp = 55;

/*
int   ini_getbool(const mTCHAR *Section, const mTCHAR *Key, int DefValue, const mTCHAR *Filename);
long  ini_getl(const mTCHAR *Section, const mTCHAR *Key, long DefValue, const mTCHAR *Filename);
int   ini_gets(const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *DefValue, mTCHAR *Buffer, int BufferSize, const mTCHAR *Filename);
int   ini_getsection(int idx, mTCHAR *Buffer, int BufferSize, const mTCHAR *Filename);
int   ini_getkey(const mTCHAR *Section, int idx, mTCHAR *Buffer, int BufferSize, const mTCHAR *Filename);
INI_REAL ini_getf(const mTCHAR *Section, const mTCHAR *Key, INI_REAL DefValue, const mTCHAR *Filename);
 */

void load_network(void)
{
	int port = 0;
	int result;

	result = ini_gets(strNetwork, "address", "10.10.10.10", address, sizeof(address), strConfigFile);
	result = ini_gets(strNetwork, "gateway", "10.1.1.1", gateway, sizeof(gateway), strConfigFile);
	result = ini_gets(strNetwork, "subnet", "255.0.0.0", subnet, sizeof(subnet), strConfigFile);
	port = ini_getl(strNetwork, "port", 21, strConfigFile);
	dhcp = ini_getbool(strNetwork, "dhcp", 1, strConfigFile);

	printf("IP address: %s\r\n", address);
	printf("IP gateway: %s\r\n", gateway);
	printf("IP subnet: %s\r\n", subnet);
	printf("IP port: %u\r\n", port);
	printf("DHCP: %u\r\n", dhcp);

#if (USE_NETWORK != 0)
	nw_mod._.uart1           = ini_getbool(strNetwork, "uart1", NETWORK_USE_UART1, strConfigFile);
	nw_mod._.uart2           = ini_getbool(strNetwork, "uart2", NETWORK_USE_UART2, strConfigFile);
	nw_mod._.flybywire       = ini_getbool(strNetwork, "flybywire", NETWORK_USE_FLYBYWIRE, strConfigFile);
	nw_mod._.mavlink         = ini_getbool(strNetwork, "mavlink", NETWORK_USE_MAVLINK, strConfigFile);
	nw_mod._.debug           = ini_getbool(strNetwork, "debug", NETWORK_USE_DEBUG, strConfigFile);
	nw_mod._.adsb            = ini_getbool(strNetwork, "adsb", NETWORK_USE_ADSB, strConfigFile);
	nw_mod._.logo            = ini_getbool(strNetwork, "logo", NETWORK_USE_LOGO, strConfigFile);
	nw_mod._.cam_tracking    = ini_getbool(strNetwork, "cam_tracking", NETWORK_USE_CAM_TRACKING, strConfigFile);
	nw_mod._.gpstest         = ini_getbool(strNetwork, "gpstest", NETWORK_USE_GPSTEST, strConfigFile);
	nw_mod._.pwmreport       = ini_getbool(strNetwork, "pwmreport", NETWORK_USE_PWMREPORT, strConfigFile);
	nw_mod._.xplane          = ini_getbool(strNetwork, "xplane", NETWORK_USE_XPLANE, strConfigFile);
	nw_mod._.telemetry_extra = ini_getbool(strNetwork, "telemetry_extra", NETWORK_USE_TELEMETRY_EXTRA, strConfigFile);
	nw_mod._.ground_station  = ini_getbool(strNetwork, "ground_station", NETWORK_USE_GROUND_STATION, strConfigFile);
#endif // USE_NETWORK
}

void load_settings(void)
{
}

/*
const char* strMode = "MODE";
const char* strFailSafe = "FAILSAFE";

	mode_low = ini_getl(strMode, "low", MODE_SWITCH_THRESHOLD_LOW, strConfigFile);
	mode_high = ini_getl(strMode, "high", MODE_SWITCH_THRESHOLD_HIGH, strConfigFile);
	dhcp = ini_getbool(strMod, "two_pos", MODE_SWITCH_TWO_POSITION, strConfigFile);

 = ini_getl(strFailSafe, "channel", FAILSAFE_INPUT_CHANNEL, strConfigFile); // THROTTLE_INPUT_CHANNEL
 = ini_getl(strFailSafe, "min", FAILSAFE_INPUT_MIN, strConfigFile);
 = ini_getl(strFailSafe, "max", FAILSAFE_INPUT_MAX, strConfigFile);
 = ini_getl(strFailSafe, "type", FAILSAFE_TYPE, strConfigFile); // FAILSAFE_RTL
 = ini_getl(strFailSafe, "hold", FAILSAFE_HOLD, strConfigFile);


	result = ini_gets(strMAVLink, "name", "255.0.0.0", subnet, sizeof(subnet), strConfigFile);
const char* strMAVLink = "MAVLINK";
sysid = 55
name = "Not Defined"
rego = "Not Defined"
pilot = "Not Defined"
url = "http://www.diydrones.com"
 */

void load_config(void)
{
/*
	config._.RollStabilizaionAilerons = ROLL_STABILIZATION_AILERONS;
	config._.RollStabilizationRudder = ROLL_STABILIZATION_RUDDER;
	config._.PitchStabilization = PITCH_STABILIZATION;
	config._.YawStabilizationRudder = YAW_STABILIZATION_RUDDER;
	config._.YawStabilizationAileron = YAW_STABILIZATION_AILERON;

	config._.AileronNavigation = AILERON_NAVIGATION;
	config._.RudderNavigation = RUDDER_NAVIGATION;

	config._.AltitudeholdStabilized = ALTITUDEHOLD_STABILIZED;
	config._.AltitudeholdWaypoint = ALTITUDEHOLD_WAYPOINT;
	config._.RacingMode = RACING_MODE;
 */
	config._.RollStabilizaionAilerons = ini_getbool(strStabilise, "roll_ail", ROLL_STABILIZATION_AILERONS, strConfigFile);
	config._.RollStabilizationRudder = ini_getbool(strStabilise, "roll_rud", ROLL_STABILIZATION_RUDDER, strConfigFile);
	config._.PitchStabilization = ini_getbool(strStabilise, "pitch", PITCH_STABILIZATION, strConfigFile);
	config._.YawStabilizationRudder = ini_getbool(strStabilise, "yaw_rud", YAW_STABILIZATION_RUDDER, strConfigFile);
	config._.YawStabilizationAileron = ini_getbool(strStabilise, "yaw_ail", YAW_STABILIZATION_AILERON, strConfigFile);

	config._.AileronNavigation = ini_getbool(strNavigation, "ail", AILERON_NAVIGATION, strConfigFile);
	config._.RudderNavigation = ini_getbool(strNavigation, "rud", RUDDER_NAVIGATION, strConfigFile);
	// = ini_getbool(strNavigation, "wind", WIND_GAIN_ADJUSTMENT, strConfigFile);

	config._.AltitudeholdStabilized = ini_getl(strAltitude, "stabilised", ALTITUDEHOLD_STABILIZED, strConfigFile);
	config._.AltitudeholdWaypoint = ini_getl(strAltitude, "waypoint", ALTITUDEHOLD_WAYPOINT, strConfigFile);
//	config._.RacingMode = ini_getbool(strMode, "racing", RACING_MODE, strConfigFile);
}
/*
[ALTITUDE]
# NONE = 0, FULL = 1, PITCH = 2
stabilised = 1
# NONE = 0, FULL = 1, PITCH = 2
waypoint = 1
 */

void load_gains(void)
{
/*
	gains.YawKPAileron = YAWKP_AILERON;
	gains.YawKDAileron = YAWKD_AILERON;
	gains.RollKP = ROLLKP;
	gains.RollKD = ROLLKD;
	gains.AileronBoost = AILERON_BOOST;
	gains.Pitchgain = PITCHGAIN;
	gains.PitchKD = PITCHKD;
	gains.RudderElevMix = RUDDER_ELEV_MIX;
	gains.RollElevMix = ROLL_ELEV_MIX;
	gains.ElevatorBoost = ELEVATOR_BOOST;
	gains.YawKPRudder = YAWKP_RUDDER;
	gains.YawKDRudder = YAWKD_RUDDER;
	gains.RollKPRudder = ROLLKP_RUDDER;
	gains.RollKDRudder = ROLLKD_RUDDER;
	gains.RudderBoost = RUDDER_BOOST;
	gains.RtlPitchDown = RTL_PITCH_DOWN;
	gains.HeightTargetMax = HEIGHT_TARGET_MAX;
	gains.HeightTargetMin = HEIGHT_TARGET_MIN;
	gains.AltHoldThrottleMin = ALT_HOLD_THROTTLE_MIN;
	gains.AltHoldThrottleMax = ALT_HOLD_THROTTLE_MAX;
	gains.AltHoldPitchMin = ALT_HOLD_PITCH_MIN;
	gains.AltHoldPitchMax = ALT_HOLD_PITCH_MAX;
	gains.AltHoldPitchHigh = ALT_HOLD_PITCH_HIGH;
 */
// Aileron/Roll Control Gains
	gains.RollKP = ini_getf(strRoll, "rollkp", ROLLKP, strConfigFile);
	gains.RollKD = ini_getf(strRoll, "rollkd", ROLLKD, strConfigFile);
	gains.YawKPAileron = ini_getf(strRoll, "yawkp", YAWKP_AILERON, strConfigFile);
	gains.YawKDAileron = ini_getf(strRoll, "yawkd", YAWKD_AILERON, strConfigFile);
	gains.AileronBoost = ini_getf(strRoll, "boost", AILERON_BOOST, strConfigFile);

// Elevator/Pitch Control Gains
	gains.Pitchgain = ini_getf(strPitch, "gain", PITCHGAIN, strConfigFile);
	gains.PitchKD = ini_getf(strPitch, "pitchkd", PITCHKD, strConfigFile);
	gains.RudderElevMix = ini_getf(strPitch, "rudder", RUDDER_ELEV_MIX, strConfigFile);
	gains.RollElevMix = ini_getf(strPitch, "roll", ROLL_ELEV_MIX, strConfigFile);
	gains.ElevatorBoost = ini_getf(strPitch, "boost", ELEVATOR_BOOST, strConfigFile);
	// = ini_getf(strPitch, "invert", INVERTED_NEUTRAL_PITCH, strConfigFile);

// Rudder/Yaw Control Gains
	gains.YawKPRudder = ini_getf(strYaw, "yawkp", YAWKP_RUDDER, strConfigFile);
	gains.YawKDRudder = ini_getf(strYaw, "yawkd", YAWKD_RUDDER, strConfigFile);
	gains.RollKPRudder = ini_getf(strYaw, "rollkp", ROLLKP_RUDDER, strConfigFile);
	gains.RollKDRudder = ini_getf(strYaw, "rollkd", ROLLKD_RUDDER, strConfigFile);
	// = ini_getf(strYaw, "mix", MANUAL_AILERON_RUDDER_MIX, strConfigFile);
	gains.RudderBoost = ini_getf(strYaw, "boost", RUDDER_BOOST, strConfigFile);

// Altitude Hold
	gains.HeightTargetMax = ini_getf(strAltitude, "height_max", HEIGHT_TARGET_MAX, strConfigFile);
	gains.HeightTargetMin = ini_getf(strAltitude, "height_min", HEIGHT_TARGET_MIN, strConfigFile);
	gains.AltHoldThrottleMin = ini_getf(strAltitude, "throt_min", ALT_HOLD_THROTTLE_MIN, strConfigFile);
	gains.AltHoldThrottleMax = ini_getf(strAltitude, "throt_max", ALT_HOLD_THROTTLE_MAX, strConfigFile);
	gains.AltHoldPitchMin = ini_getf(strAltitude, "pitch_min", ALT_HOLD_PITCH_MIN, strConfigFile);
	gains.AltHoldPitchMax = ini_getf(strAltitude, "pitch_max", ALT_HOLD_PITCH_MAX, strConfigFile);
	gains.AltHoldPitchHigh = ini_getf(strAltitude, "pitch_high", ALT_HOLD_PITCH_HIGH, strConfigFile);
	// = ini_getl(strAltitude, "margin", HEIGHT_MARGIN, strConfigFile);

// Return To Launch Pitch Down
	gains.RtlPitchDown = ini_getf(strRTL, "pitch", RTL_PITCH_DOWN, strConfigFile);

// Hover
    gains.HoverRollKP = ini_getf(strHover, "rollkp", HOVER_ROLLKP, strConfigFile);
    gains.HoverRollKD = ini_getf(strHover, "rollkd", HOVER_ROLLKD, strConfigFile);
    gains.HoverPitchGain = ini_getf(strHover, "gain", HOVER_PITCHGAIN, strConfigFile);
    gains.HoverPitchKD = ini_getf(strHover, "pitchkd", HOVER_PITCHKD, strConfigFile);
    gains.HoverPitchOffset = ini_getf(strHover, "pitch", HOVER_PITCH_OFFSET, strConfigFile);
    gains.HoverYawKP = ini_getf(strHover, "yawkp", HOVER_YAWKP, strConfigFile);
    gains.HoverYawKD = ini_getf(strHover, "yawkd", HOVER_YAWKD, strConfigFile);
    gains.HoverYawOffset = ini_getf(strHover, "yaw", HOVER_YAW_OFFSET, strConfigFile);
    gains.HoverPitchTowardsWP = ini_getf(strHover, "wp", HOVER_PITCH_TOWARDS_WP, strConfigFile);
    gains.HoverNavMaxPitchRadius = ini_getf(strHover, "radius", HOVER_NAV_MAX_PITCH_RADIUS, strConfigFile);

}

#endif // USE_CONFIGFILE

void init_config(void)
{
#if (USE_CONFIGFILE == 1)
	load_settings();
	load_config();
	load_gains();
#endif // USE_CONFIGFILE

	init_yawCntrl();
	init_rollCntrl();
	init_pitchCntrl();
	init_navigation();
	init_airspeedCntrl();
	init_altitudeCntrl();
	init_altitudeCntrlVariable();
}

/*
int   ini_putl(const mTCHAR *Section, const mTCHAR *Key, long Value, const mTCHAR *Filename);
int   ini_puts(const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, const mTCHAR *Filename);
#if defined INI_REAL
int   ini_putf(const mTCHAR *Section, const mTCHAR *Key, INI_REAL Value, const mTCHAR *Filename);
 */
