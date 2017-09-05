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
//#include "config-defaults.h"
#define INI_REAL float
#if ! (WIN == 1 || NIX == 1 || PX4 == 1 || NOFS == 1)
/* for floating-point support, define additional types and functions */
//#define INI_REAL float
#define ini_ftoa(string,value) sprintf((string),"%f",(value))
#define ini_atof(string) (INI_REAL)strtod((string),NULL)
#endif

#include "minIni.h"
#include "navigate.h"
#include "airspeedCntrl.h"


union settings_word settings;
struct gains_variables gains;
struct altit_variables altit;
struct hover_variables hover;
struct turns_variables turns;

static const char* strConfigFile = "config.ini";
//static const char* strNetwork = "NETWORK";
static const char* strStabilise = "STABILISE";
static const char* strNavigation = "NAVIGATION";
static const char* strRoll = "ROLL";
static const char* strPitch = "PITCH";
static const char* strYaw = "YAW";
static const char* strAltitude = "ALTITUDE";
static const char* strRTL = "RTL";
static const char* strHover = "HOVER";
static const char* strTurns = "TURNS";
static const char* strMode = "MODE";

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)

static const char* strNetwork = "NETWORK";
union network_module_word nw_mod;
char address[16];
char gateway[16];
char subnet[16];
char dhcp = 55;

static void load_network(void)
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
}

#endif // NETWORK_INTERFACE

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

static void load_settings(void)
{
//	printf("load_settings()\r\n");

	settings._.RollStabilizaionAilerons = ini_getbool(strStabilise, "roll_ail", ROLL_STABILIZATION_AILERONS, strConfigFile);
	settings._.RollStabilizationRudder = ini_getbool(strStabilise, "roll_rud", ROLL_STABILIZATION_RUDDER, strConfigFile);
	settings._.PitchStabilization = ini_getbool(strStabilise, "pitch", PITCH_STABILIZATION, strConfigFile);
	settings._.YawStabilizationRudder = ini_getbool(strStabilise, "yaw_rud", YAW_STABILIZATION_RUDDER, strConfigFile);
	settings._.YawStabilizationAileron = ini_getbool(strStabilise, "yaw_ail", YAW_STABILIZATION_AILERON, strConfigFile);

	settings._.AileronNavigation = ini_getbool(strNavigation, "ail", AILERON_NAVIGATION, strConfigFile);
	settings._.RudderNavigation = ini_getbool(strNavigation, "rud", RUDDER_NAVIGATION, strConfigFile);
	// = ini_getbool(strNavigation, "wind", WIND_GAIN_ADJUSTMENT, strConfigFile);

	settings._.AltitudeholdStabilized = ini_getl(strAltitude, "stabilised", ALTITUDEHOLD_STABILIZED, strConfigFile);
	settings._.AltitudeholdWaypoint = ini_getl(strAltitude, "waypoint", ALTITUDEHOLD_WAYPOINT, strConfigFile);
	settings._.RacingMode = ini_getbool(strMode, "racing", RACING_MODE, strConfigFile);
}

static void save_settings(void)
{
}

/*
[ALTITUDE]
# NONE = 0, FULL = 1, PITCH = 2
stabilised = 1
# NONE = 0, FULL = 1, PITCH = 2
waypoint = 1
 */


/*

new for helicalTurns:

#define FEED_FORWARD                         1.0
#define TURN_RATE_NAV                       30.0
#define TURN_RATE_FBW                       60.0
#define CRUISE_SPEED                        12.0
#define ANGLE_OF_ATTACK_NORMAL              -0.8
#define ANGLE_OF_ATTACK_INVERTED            -7.2
#define ELEVATOR_TRIM_NORMAL                -0.03
#define ELEVATOR_TRIM_INVERTED              -0.67

and we remove the following:
#define AILERON_BOOST                       0.8
#define RUDDER_ELEV_MIX                     0.2
#define ROLL_ELEV_MIX                       0.35

 */
static void load_turns(void)
{
	turns.FeedForward = ini_getf(strTurns, "feedfwd", FEED_FORWARD, strConfigFile);
	DPRINT("turns.FeedForward = %f\r\n", (double)turns.FeedForward);
	turns.TurnRateNav = ini_getf(strTurns, "ratenav", TURN_RATE_NAV, strConfigFile);
	turns.TurnRateFBW = ini_getf(strTurns, "ratefbw", TURN_RATE_FBW, strConfigFile);
	turns.RefSpeed = ini_getf(strTurns, "refspd", REFERENCE_SPEED, strConfigFile);
	turns.AngleOfAttackNormal = ini_getf(strTurns, "aoanorm", ANGLE_OF_ATTACK_NORMAL, strConfigFile);
	turns.AngleOfAttackInverted = ini_getf(strTurns, "aoainvt", ANGLE_OF_ATTACK_INVERTED, strConfigFile);
	turns.ElevatorTrimNormal = ini_getf(strTurns, "elenorm", ELEVATOR_TRIM_NORMAL, strConfigFile);
	turns.ElevatorTrimInverted = ini_getf(strTurns, "eleinvt", ELEVATOR_TRIM_INVERTED, strConfigFile);
}

static void load_gains(void)
{
// Aileron/Roll Control Gains
	gains.RollKP = ini_getf(strRoll, "rollkp", ROLLKP, strConfigFile);
	gains.RollKD = ini_getf(strRoll, "rollkd", ROLLKD, strConfigFile);
	gains.YawKPAileron = ini_getf(strRoll, "yawkp", YAWKP_AILERON, strConfigFile);
	gains.YawKDAileron = ini_getf(strRoll, "yawkd", YAWKD_AILERON, strConfigFile);
//	gains.AileronBoost = ini_getf(strRoll, "boost", AILERON_BOOST, strConfigFile);

// Elevator/Pitch Control Gains
	gains.Pitchgain = ini_getf(strPitch, "gain", PITCHGAIN, strConfigFile);
	gains.PitchKD = ini_getf(strPitch, "pitchkd", PITCHKD, strConfigFile);
//	gains.RudderElevMix = ini_getf(strPitch, "rudder", RUDDER_ELEV_MIX, strConfigFile);
//	gains.RollElevMix = ini_getf(strPitch, "roll", ROLL_ELEV_MIX, strConfigFile);
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
	altit.DesiredSpeed = ini_getf(strAltitude, "desired_speed", DESIRED_SPEED, strConfigFile);
	altit.HeightMargin = ini_getf(strAltitude, "height_margin", HEIGHT_MARGIN, strConfigFile);
	altit.HeightTargetMax = ini_getf(strAltitude, "height_max", HEIGHT_TARGET_MAX, strConfigFile);
	altit.HeightTargetMin = ini_getf(strAltitude, "height_min", HEIGHT_TARGET_MIN, strConfigFile);
	altit.AltHoldThrottleMin = ini_getf(strAltitude, "throt_min", ALT_HOLD_THROTTLE_MIN, strConfigFile);
	altit.AltHoldThrottleMax = ini_getf(strAltitude, "throt_max", ALT_HOLD_THROTTLE_MAX, strConfigFile);
	altit.AltHoldPitchMin = ini_getf(strAltitude, "pitch_min", ALT_HOLD_PITCH_MIN, strConfigFile);
	altit.AltHoldPitchMax = ini_getf(strAltitude, "pitch_max", ALT_HOLD_PITCH_MAX, strConfigFile);
	altit.AltHoldPitchHigh = ini_getf(strAltitude, "pitch_high", ALT_HOLD_PITCH_HIGH, strConfigFile);
	// = ini_getl(strAltitude, "margin", HEIGHT_MARGIN, strConfigFile);

// Return To Launch Pitch Down
	gains.RtlPitchDown = ini_getf(strRTL, "pitch", RTL_PITCH_DOWN, strConfigFile);

// Hover
    hover.HoverRollKP = ini_getf(strHover, "rollkp", HOVER_ROLLKP, strConfigFile);
    hover.HoverRollKD = ini_getf(strHover, "rollkd", HOVER_ROLLKD, strConfigFile);
    hover.HoverPitchGain = ini_getf(strHover, "gain", HOVER_PITCHGAIN, strConfigFile);
    hover.HoverPitchKD = ini_getf(strHover, "pitchkd", HOVER_PITCHKD, strConfigFile);
    hover.HoverPitchOffset = ini_getf(strHover, "pitch", HOVER_PITCH_OFFSET, strConfigFile);
    hover.HoverYawKP = ini_getf(strHover, "yawkp", HOVER_YAWKP, strConfigFile);
    hover.HoverYawKD = ini_getf(strHover, "yawkd", HOVER_YAWKD, strConfigFile);
    hover.HoverYawOffset = ini_getf(strHover, "yaw", HOVER_YAW_OFFSET, strConfigFile);
    hover.HoverPitchTowardsWP = ini_getf(strHover, "wp", HOVER_PITCH_TOWARDS_WP, strConfigFile);
    hover.HoverNavMaxPitchRadius = ini_getf(strHover, "radius", HOVER_NAV_MAX_PITCH_RADIUS, strConfigFile);
}

static void save_gains(void)
{
// Aileron/Roll Control Gains
	ini_putf(strRoll, "rollkp", gains.RollKP, strConfigFile);
	ini_putf(strRoll, "rollkd", gains.RollKD, strConfigFile);
	ini_putf(strRoll, "yawkp", gains.YawKPAileron, strConfigFile);
	ini_putf(strRoll, "yawkd", gains.YawKDAileron, strConfigFile);
	ini_putf(strRoll, "boost", gains.AileronBoost, strConfigFile);

// Elevator/Pitch Control Gains
	ini_putf(strPitch, "gain", gains.Pitchgain, strConfigFile);
	ini_putf(strPitch, "pitchkd", gains.PitchKD, strConfigFile);
	ini_putf(strPitch, "rudder", gains.RudderElevMix, strConfigFile);
	ini_putf(strPitch, "roll", gains.RollElevMix, strConfigFile);
	ini_putf(strPitch, "boost", gains.ElevatorBoost, strConfigFile);
	// = ini_putf(strPitch, "invert", gains.INVERTED_NEUTRALPitch, strConfigFile);

// Rudder/Yaw Control Gains
	ini_putf(strYaw, "yawkp", gains.YawKPRudder, strConfigFile);
	ini_putf(strYaw, "yawkd", gains.YawKDRudder, strConfigFile);
	ini_putf(strYaw, "rollkp", gains.RollKPRudder, strConfigFile);
	ini_putf(strYaw, "rollkd", gains.RollKDRudder, strConfigFile);
	// = ini_putf(strYaw, "mix", gains.MANUAL_AILERON_Rudder_MIX, strConfigFile);
	ini_putf(strYaw, "boost", gains.RudderBoost, strConfigFile);

// Altitude Hold
	ini_putf(strAltitude, "desired_speed", altit.DesiredSpeed, strConfigFile);
	ini_putf(strAltitude, "height_margin", altit.HeightMargin, strConfigFile);
	ini_putf(strAltitude, "height_max", altit.HeightTargetMax, strConfigFile);
	ini_putf(strAltitude, "height_min", altit.HeightTargetMin, strConfigFile);
	ini_putf(strAltitude, "throt_min", altit.AltHoldThrottleMin, strConfigFile);
	ini_putf(strAltitude, "throt_max", altit.AltHoldThrottleMax, strConfigFile);
	ini_putf(strAltitude, "pitch_min", altit.AltHoldPitchMin, strConfigFile);
	ini_putf(strAltitude, "pitch_max", altit.AltHoldPitchMax, strConfigFile);
	ini_putf(strAltitude, "pitch_high", altit.AltHoldPitchHigh, strConfigFile);
	// = ini_getl(strAltitude, "margin", altit.Height_MARGIN, strConfigFile);

// Return To Launch Pitch Down
	ini_putf(strRTL, "pitch", gains.RtlPitchDown, strConfigFile);

// Hover
	ini_putf(strHover, "rollkp", hover.HoverRollKP, strConfigFile);
	ini_putf(strHover, "rollkd", hover.HoverRollKD, strConfigFile);
	ini_putf(strHover, "gain", hover.HoverPitchGain, strConfigFile);
	ini_putf(strHover, "pitchkd", hover.HoverPitchKD, strConfigFile);
	ini_putf(strHover, "pitch", hover.HoverPitchOffset, strConfigFile);
	ini_putf(strHover, "yawkp", hover.HoverYawKP, strConfigFile);
	ini_putf(strHover, "yawkd", hover.HoverYawKD, strConfigFile);
	ini_putf(strHover, "yaw", hover.HoverYawOffset, strConfigFile);
	ini_putf(strHover, "wp", hover.HoverPitchTowardsWP, strConfigFile);
	ini_putf(strHover, "radius", hover.HoverNavMaxPitchRadius, strConfigFile);
}

static void save_turns(void)
{
	ini_putf(strTurns, "feedfwd", turns.FeedForward, strConfigFile);
	ini_putf(strTurns, "ratenav", turns.TurnRateNav, strConfigFile);
	ini_putf(strTurns, "ratefbw", turns.TurnRateFBW, strConfigFile);
	ini_putf(strTurns, "refspd", turns.RefSpeed, strConfigFile);
	ini_putf(strTurns, "aoanorm", turns.AngleOfAttackNormal, strConfigFile);
	ini_putf(strTurns, "aoainvt", turns.AngleOfAttackInverted, strConfigFile);
	ini_putf(strTurns, "elenorm", turns.ElevatorTrimNormal, strConfigFile);
	ini_putf(strTurns, "eleinvt", turns.ElevatorTrimInverted, strConfigFile);
}

void config_load(void)
{
	load_settings();
	load_gains();
	load_turns();

	init_yawCntrl();
	init_rollCntrl();
	init_pitchCntrl();

	init_navigation();
	init_airspeedCntrl();
	init_altitudeCntrl();
	init_altitudeCntrlVariable();
}

void config_save(void)
{
	save_yawCntrl();
	save_rollCntrl();
	save_pitchCntrl();

//	save_navigation();
//	save_airspeedCntrl();
#if (ALTITUDE_GAINS_VARIABLE == 1)
	save_altitudeCntrlVariable();
#else
	save_altitudeCntrl();
#endif

	save_settings();
	save_gains();
	save_turns();
}

void config_init(void)
{
	config_load();
}
