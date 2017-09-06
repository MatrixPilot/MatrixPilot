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

#include "defines.h"

#define USE_TELEMETRY
#ifdef USE_TELEMETRY

#include "states.h"
#include "config.h"
#include "navigate.h"
#include "cameraCntrl.h"
#include "flightplan.h"
#include "flightplan_waypoints.h"
#include "altitudeCntrl.h"
#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif
#include "../libUDB/heartbeat.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/osd.h"
#include "options_magnetometer.h"
#include "options_osd.h"
//#if (SILSIM != 1) // this caused a build failure when SILSIM and SERIAL_UDB_EXTRA are both defined
#include "../libUDB/libUDB.h" // Needed for access to RCON
#include "../libUDB/ADchannel.h"
//#endif
#include "../libUDB/mcu.h"
//#include "../libDCM/libDCM_internal.h" // Needed for access to internal DCM values
#include "../libDCM/libDCM.h" // Needed for access to internal DCM value
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/estAltitude.h"
#include "../libDCM/estWind.h"
#include "../libDCM/rmat.h"
#include "../libDCM/mathlibNAV.h"   // Needed for SERIAL_ARDUSTATION
#include "../libUDB/magnetometer.h" // Needed for SERIAL_MAGNETOMETER 
#include <string.h>


////////////////////////////////////////////////////////////////////////////////
// Serial Output Format (Can be SERIAL_NONE, SERIAL_DEBUG, SERIAL_ARDUSTATION, SERIAL_UDB,
// SERIAL_UDB_EXTRA, SERIAL_CAM_TRACK, SERIAL_OSD_REMZIBI, or SERIAL_UDB_MAG)
// This determines the format of the output sent out the spare serial port.
// Note that SERIAL_OSD_REMZIBI only works with a ublox GPS.
// SERIAL_UDB_EXTRA will add additional telemetry fields to those of SERIAL_UDB.
// SERIAL_UDB_EXTRA can be used with the OpenLog without characters being dropped.
// SERIAL_UDB_EXTRA may result in dropped characters if used with the XBEE wireless transmitter.
// SERIAL_CAM_TRACK is used to output location data to a 2nd UDB, which will target its camera at this plane.
// SERIAL_UDB_MAG outputs the automatically calculated offsets and raw magnetometer data.

#ifndef SERIAL_OUTPUT_FORMAT
//#define SERIAL_OUTPUT_FORMAT                SERIAL_NONE    // TODO: this will have missing dependencies
#define SERIAL_OUTPUT_FORMAT                SERIAL_UDB_EXTRA
#endif // SERIAL_OUTPUT_FORMAT

////////////////////////////////////////////////////////////////////////////////
// MAVLINK is a bi-directional binary format for use with QgroundControl, HKGCS or MAVProxy (Ground Control Stations)
// Note that MAVLINK defaults to using a baud rate of 57600 baud (other formats default to 19200)
// enable MAVLink support with USE_MAVLINK in mavlink_options.h

////////////////////////////////////////////////////////////////////////////////
// Serial Output BAUD rate for either standard telemetry streams or MAVLink
//  19200, 38400, 57600, 115200, 230400, 460800, 921600 // yes, it really will work at this rate
//#define SERIAL_BAUDRATE                     19200


#if ((SERIAL_OUTPUT_FORMAT != SERIAL_NONE) && (SERIAL_OUTPUT_FORMAT != SERIAL_MAVLINK))

#if (FLY_BY_DATALINK_ENABLED == 1)
#include "fly_by_datalink.h"
#endif

#include <stdarg.h>


static union intbb voltage_milis = {0};
static union intbb voltage_temp;

static void sio_newMsg(uint8_t);
static void sio_voltage_low(uint8_t inchar);
static void sio_voltage_high(uint8_t inchar);

static void sio_fp_data(uint8_t inchar);
static void sio_fp_checksum(uint8_t inchar);

#if (CAM_USE_EXTERNAL_TARGET_DATA == 1)
static void sio_cam_data(uint8_t inchar);
static void sio_cam_checksum(uint8_t inchar);
#endif

#if (FLY_BY_DATALINK_ENABLED == 1)
static void sio_fbdl_data(unsigned char inchar);
#endif

static char fp_high_byte;
static uint8_t fp_checksum;

static void (*sio_parse)(uint8_t inchar) = &sio_newMsg;


#define SERIAL_BUFFER_SIZE 256
static char serial_buffer[SERIAL_BUFFER_SIZE+1];
static int16_t sb_index = 0;
static int16_t end_index = 0;

int16_t udb_serial_callback_get_byte_to_send(void);
void udb_serial_callback_received_byte(uint8_t rxchar);

void telemetry_init(void)
{
#if (SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI)
	dcm_flags._.nmea_passthrough = 1;
#endif

#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE 19200 // default
#warning SERIAL_BAUDRATE set to default value of 19200 bps
#endif

#if (CONSOLE_UART != 2)
	udb_init_USART(&udb_serial_callback_get_byte_to_send, &udb_serial_callback_received_byte);
#endif
	udb_serial_set_rate(SERIAL_BAUDRATE);
}

////////////////////////////////////////////////////////////////////////////////
// 
// Receive Serial Commands
//

void udb_serial_callback_received_byte(uint8_t rxchar)
{
	(*sio_parse)(rxchar); // parse the input byte
}

static void sio_newMsg(uint8_t inchar)
{
	switch (inchar)
	{
	case 'V':
		sio_parse = &sio_voltage_high;
		break;
	
#if (FLIGHT_PLAN_TYPE == FP_LOGO)
	case 'L':
#else
	case 'W':
#endif
		fp_high_byte = -1; // -1 means we don't have the high byte yet (0-15 means we do)
		fp_checksum = 0;
		sio_parse = &sio_fp_data;
		flightplan_live_begin();
		break;

#if (CAM_USE_EXTERNAL_TARGET_DATA == 1)
	case 'T':
		fp_high_byte = -1; // -1 means we don't have the high byte yet (0-15 means we do)
		fp_checksum = 0;
		sio_parse = &sio_cam_data;
		camera_live_begin();
		break;
#endif

#if (FLY_BY_DATALINK_ENABLED == 1)
	case 'F':
		fp_checksum = 'F';
		sio_parse = &sio_fbdl_data;
		fbdl_live_begin();
		break;
#endif

	default:
		// error ?
		break;
	} // switch
}

static void sio_voltage_high(uint8_t inchar)
{
	voltage_temp.BB = 0; // initialize our temp variable
	voltage_temp._.B1 = inchar;
	sio_parse = &sio_voltage_low;
}

static void sio_voltage_low(uint8_t inchar)
{
	voltage_temp._.B0 = inchar;
	voltage_temp.BB = voltage_temp.BB * 2; // convert to voltage
	voltage_milis.BB = voltage_temp.BB;
	sio_parse = &sio_newMsg;
}

static int8_t hex_char_val(uint8_t inchar)
{
	if (inchar >= '0' && inchar <= '9')
	{
		return (inchar - '0');
	}
	else if (inchar >= 'A' && inchar <= 'F')
	{
		return (inchar - 'A' + 10);
	}
	return -1;
}

// For UDB Logo instructions, bytes should be passed in using the following format
// (Below, an X represents a hex digit 0-F.  Mulit-digit values are MSB first.)
// L            begin remote Logo command
// XX   byte:   command
// XX   byte:   subcommand
// X    0-1:    do fly
// X    0-1:    use param
// XXXX word:   argument
// *            done with command data
// XX   byte:   checksum should equal the sum of the 10 bytes before the *, mod 256
//
// For example: "L0201000005*E8" runs:
// the DO command(02) for subroutine 01 with fly and param off(00) and an argument of 0005


// For classic Waypoints, bytes should be passed in using the following format
// (Below, an X represents a hex digit 0-F.  Mulit-digit values are MSB first.)
// W                    begin remote Waypoint command
// XXXXXXXX int32_t:    waypoint X value
// XXXXXXXX int32_t:    waypoint Y value
// XXXX     word:       waypoint Z value
// XXXX     word:       flags
// XXXXXXXX int32_t:    cam view X value
// XXXXXXXX int32_t:    cam view Y value
// XXXX     word:       cam view Z value
// *                    done with command data
// XX       byte:       checksum should equal the sum of the 44 bytes before the *, mod 256
//
// For example: "W0000006400000032000F020000000000000000000000*67" represents:
// the waypoint { {100, 50, 15}, F_INVERTED, {0, 0, 0} }
//

static void sio_fp_data(uint8_t inchar)
{
	if (inchar == '*')
	{
		fp_high_byte = -1;
		sio_parse = &sio_fp_checksum;
	}
	else
	{
		int8_t hexVal = hex_char_val(inchar);
		if (hexVal == -1)
		{
			sio_parse = &sio_newMsg;
			return;
		}
		else if (fp_high_byte == -1)
		{
			fp_high_byte = hexVal * 16;
		}
		else
		{
			flightplan_live_received_byte(fp_high_byte + hexVal);
			fp_high_byte = -1;
		}
		fp_checksum += inchar;
	}
}

static void sio_fp_checksum(uint8_t inchar)
{
	int8_t hexVal = hex_char_val(inchar);

	if (hexVal == -1)
	{
		sio_parse = &sio_newMsg;
	}
	else if (fp_high_byte == -1)
	{
		fp_high_byte = hexVal * 16;
	}
	else
	{
		uint8_t v = fp_high_byte + hexVal;
		if (v == fp_checksum)
		{
			flightplan_live_commit();
		}
		sio_parse = &sio_newMsg;
	}
}

#if (CAM_USE_EXTERNAL_TARGET_DATA == 1)

static void sio_cam_data(uint8_t inchar)
{
	if (inchar == '*')
	{
		fp_high_byte = -1;
		sio_parse = &sio_cam_checksum;
	}
	else
	{
		int8_t hexVal = hex_char_val(inchar);
		if (hexVal == -1)
		{
			sio_parse = &sio_newMsg;
			return;
		}
		else if (fp_high_byte == -1)
		{
			fp_high_byte = hexVal * 16;
		}
		else
		{
			uint8_t combined = fp_high_byte + hexVal;
			camera_live_received_byte(combined);
			fp_high_byte = -1;
			fp_checksum += combined;
		}
	}
}

static void sio_cam_checksum(uint8_t inchar)
{
	int8_t hexVal = hex_char_val(inchar);

	if (hexVal == -1)
	{
		sio_parse = &sio_newMsg;
	}
	else if (fp_high_byte == -1)
	{
		fp_high_byte = hexVal * 16;
	}
	else
	{
		uint8_t v = fp_high_byte + hexVal;
		if (v == fp_checksum)
		{
			camera_live_commit();
		}
		sio_parse = &sio_newMsg;
	}
}
#endif // CAM_USE_EXTERNAL_TARGET_DATA

#if (FLY_BY_DATALINK_ENABLED == 1)
static void sio_fbdl_data(unsigned char inchar)
{
	if (get_fbdl_pos() < LENGTH_OF_PACKET)
	{
		fp_checksum += inchar;
		if (!fbdl_live_received_byte(inchar))
			fbdl_live_begin();
	}
	else if (get_fbdl_pos() == LENGTH_OF_PACKET)
	{
		// UART has an extra BYTE for checksum, IP doesn't need it.
		if (inchar == fp_checksum)
		{
			fbdl_live_commit();
		}
		sio_parse = &sio_newMsg;
		fbdl_live_begin();
	}
	else
	{
		sio_parse = &sio_newMsg;
		fbdl_live_begin();
	}
}
#endif // (FLY_BY_DATALINK_ENABLED == 1)

////////////////////////////////////////////////////////////////////////////////
// 
// Output Serial Data
//

#if (USE_TELELOG == 1)
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static void serial_output(const char* format, ...)
{
	int16_t len;
	int16_t start_index;
	int16_t remaining;
	char telebuf[200];

	va_list arglist;
	va_start(arglist, format);

	len = vsnprintf(telebuf, sizeof(telebuf), format, arglist);

//	static int maxlen = 0;
//	if (len > maxlen) {
//		maxlen = len;
//		DPRINT("maxlen %u\r\n", maxlen);
//	}

	start_index = end_index;
	remaining = (SERIAL_BUFFER_SIZE - start_index);
	if (remaining < len) {
		DPRINT("SERBUF discarding %u bytes\r\n", len - remaining);
	}
	if (remaining > 1)
	{
		strncpy((char*)(&serial_buffer[start_index]), telebuf, MIN(remaining, len));
		end_index = start_index + MIN(remaining, len);
		serial_buffer[end_index] = '\0';
	}
	if (sb_index == 0)
	{
		udb_serial_start_sending_data();
	}
	log_telemetry(telebuf, len);

	va_end(arglist);
}
#else
// add this text to the output buffer
static void serial_output(const char* format, ...)
{
	int16_t start_index;
	int16_t remaining;
	va_list arglist;

	va_start(arglist, format);

	start_index = end_index;
	remaining = SERIAL_BUFFER_SIZE - start_index;

	if (remaining > 1)
	{
		udb_serial_stop_sending_data();
		int16_t wrote = vsnprintf((char*)(&serial_buffer[start_index]), (size_t)remaining, format, arglist);
		end_index = start_index + wrote;
		udb_serial_start_sending_data();
	}
	va_end(arglist);
}
#endif // USE_TELELOG

int16_t udb_serial_callback_get_byte_to_send(void)
{
	uint8_t txchar = serial_buffer[ sb_index++ ];

	if (txchar)
	{
		return txchar;
	}
	else
	{
		sb_index = 0;
		end_index = 0;
	}
	return -1;
}

static int16_t telemetry_counter = 13;

void telemetry_restart(void)
{
	telemetry_counter = 13;
}

#if (SERIAL_OUTPUT_FORMAT == SERIAL_DEBUG)

void telemetry_output_8hz(void)
{
	serial_output("lat: %li, long: %li, alt: %li\r\nrmat: %i, %i, %i, %i, %i, %i, %i, %i, %i\r\n",
	    lat_gps.WW, lon_gps.WW, alt_sl_gps.WW,
	    rmat[0], rmat[1], rmat[2],
	    rmat[3], rmat[4], rmat[5],
	    rmat[6], rmat[7], rmat[8]);
}

#elif (SERIAL_OUTPUT_FORMAT == SERIAL_ARDUSTATION)

extern int16_t desiredHeight;

void telemetry_output_8hz(void)
{
	uint16_t mode;
	struct relative2D matrix_accum;
	union longbbbb accum;
	int16_t desired_dir_deg;    // desired_dir converted to a bearing (0-360)

	int32_t earth_pitch;        // pitch in binary angles (0-255 is 360 degreres)
	int32_t earth_roll;         // roll of the plane with respect to earth frame
//	int32_t earth_yaw;          // yaw with respect to earth frame

	accum.WW  = (desired_dir * BYTECIR_TO_DEGREE) + 32768;
	desired_dir_deg  = accum._.W1 - 90; // "Convert UAV DevBoad Earth" to Compass Bearing
	if (desired_dir_deg < 0) desired_dir_deg += 360; 

	if (state_flags._.GPS_steering == 0 && state_flags._.pitch_feedback == 0)
		mode = 1;
	else if (state_flags._.GPS_steering == 0 && state_flags._.pitch_feedback == 1)
		mode = 2;
	else if (state_flags._.GPS_steering == 1 && state_flags._.pitch_feedback == 1 && udb_flags._.radio_on == 1)
		mode = 3;
	else if (state_flags._.GPS_steering == 1 && state_flags._.pitch_feedback == 1 && udb_flags._.radio_on == 0)
		mode = 0;
	else
		mode = 99; // Unknown

	//  Roll
	//  Earth Frame of Reference
	matrix_accum.x = rmat[8];
	matrix_accum.y = rmat[6];
	earth_roll = rect_to_polar(&matrix_accum);              // binary angle (0 - 256 = 360 degrees)
	earth_roll = (-earth_roll * BYTECIR_TO_DEGREE) >> 16;   // switch polarity, convert to -180 - 180 degrees

	//  Pitch
	//  Earth Frame of Reference
	//  Note that we are using the matrix_accum.x
	//  left over from previous rect_to_polar in this calculation.
	//  so this Pitch calculation must follow the Roll calculation
	matrix_accum.y = rmat[7];
	earth_pitch = rect_to_polar(&matrix_accum);             // binary angle (0 - 256 = 360 degrees)
	earth_pitch = (-earth_pitch * BYTECIR_TO_DEGREE) >> 16; // switch polarity, convert to -180 - 180 degrees

	// Yaw
	// Earth Frame of Reference
	// Ardustation does not use yaw in degrees
	// matrix_accum.x = rmat[4];
	// matrix_accum.y = rmat[1];
	// earth_yaw = rect_to_polar(&matrix_accum);            // binary angle (0 - 256 = 360 degrees)
	// earth_yaw = (earth_yaw * BYTECIR_TO_DEGREE) >> 16;   // switch polarity, convert to -180 - 180 degrees

	// The Ardupilot GroundStation protocol is mostly documented here:
	//    http://diydrones.com/profiles/blogs/ardupilot-telemetry-protocol

	if (udb_pulse_counter % 40 == 0)        // Every 8 runs (5 heartbeat counts per 8Hz)
	{
		serial_output("!!!LAT:%li,LON:%li,SPD:%.2f,CRT:%.2f,ALT:%li,ALH:%i,CRS:%.2f,BER:%i,WPN:%i,DST:%i,BTV:%.2f***\r\n"
		              "+++THH:%i,RLL:%li,PCH:%li,STT:%i,***\r\n",
		    lat_gps.WW / 10, lon_gps.WW / 10, (float)(sog_gps.BB / 100.0), (float)(climb_gps.BB / 100.0),
		    (alt_sl_gps.WW - alt_origin.WW) / 100, desiredHeight, (float)(cog_gps.BB / 100.0), desired_dir_deg,
		    waypointIndex, tofinish_line, (float)(voltage_milis.BB / 100.0), 
		    (int16_t)((udb_pwOut[THROTTLE_OUTPUT_CHANNEL] - udb_pwTrim[THROTTLE_OUTPUT_CHANNEL])/20),
		    earth_roll, earth_pitch, mode);
	}
	else if (udb_pulse_counter % 10 == 0)   // Every 2 runs (5 heartbeat counts per 8Hz)
	{
		serial_output("+++THH:%i,RLL:%li,PCH:%li,STT:%i,***\r\n",
		    (int16_t)((udb_pwOut[THROTTLE_OUTPUT_CHANNEL] - udb_pwTrim[THROTTLE_OUTPUT_CHANNEL])/20),
		    earth_roll, earth_pitch, mode);
	}
}

#elif (SERIAL_OUTPUT_FORMAT == SERIAL_UDB_EXTRA)

void telemetry_output_8hz(void)
{
	int16_t i;
	static int toggle = 0;
	static boolean f13_print_prepare = false;
	// F2: SERIAL_UDB_EXTRA format is printed out every other time, although it is being called at 8Hz, this
	//     version will output at 4Hz.
	static int16_t pwIn_save[NUM_INPUTS + 1];
	static int16_t pwOut_save[NUM_OUTPUTS + 1];
	
	switch (telemetry_counter)
	{
		case 13:
			serial_output("F22:Sensors=%i,%i,%i,%i,%i,%i\r\n",
				UDB_XACCEL.value, UDB_YACCEL.value,
				UDB_ZACCEL.value + (Z_GRAVITY_SIGN ((int16_t)(2*GRAVITY))),
				udb_xrate.value, udb_yrate.value, udb_zrate.value);
			break;
		case 12: 
			serial_output("F21:Offsets=%i,%i,%i,%i,%i,%i\r\n",
				UDB_XACCEL.offset, UDB_YACCEL.offset, UDB_ZACCEL.offset,
				udb_xrate.offset, udb_yrate.offset, udb_zrate.offset);
			break;
		case 11:
			serial_output("F15:IDA=");
			serial_output(ID_VEHICLE_MODEL_NAME);
			serial_output(":IDB=");
			serial_output(ID_VEHICLE_REGISTRATION);
			serial_output(":\r\n");
			break;
		case 10:
			serial_output("F16:IDC=");
			serial_output(ID_LEAD_PILOT);
			serial_output(":IDD=");
			serial_output(ID_DIY_DRONES_URL);
			serial_output(":\r\n");
			break;
		case 9:
			serial_output("F17:FD_FWD=%5.3f:TR_NAV=%5.3f:TR_FBW=%5.3f:\r\n",
			    turns.FeedForward, turns.TurnRateNav, turns.TurnRateFBW);
			break;
		case 8:
			serial_output("F18:AOA_NRM=%5.3f:AOA_INV=%5.3f:EL_TRIM_NRM=%5.3f:EL_TRIM_INV=%5.3f:CRUISE_SPD=%5.3f:\r\n",
			    turns.AngleOfAttackNormal, turns.AngleOfAttackInverted, turns.ElevatorTrimNormal,
			    turns.ElevatorTrimInverted, turns.RefSpeed);
			break;
		case 7:
			serial_output("F19:AIL=%i,%i:ELEV=%i,%i:THROT=%i,%i:RUDD=%i,%i:\r\n",
			    AILERON_OUTPUT_CHANNEL, AILERON_CHANNEL_REVERSED, ELEVATOR_OUTPUT_CHANNEL,ELEVATOR_CHANNEL_REVERSED,
			    THROTTLE_OUTPUT_CHANNEL, THROTTLE_CHANNEL_REVERSED, RUDDER_OUTPUT_CHANNEL,RUDDER_CHANNEL_REVERSED );
			break;
		case 6:
			serial_output("F14:WIND_EST=%i:GPS_TYPE=%i:DR=%i:BOARD_TYPE=%i:AIRFRAME=%i:"
			              "RCON=0x%X:TRAP_FLAGS=0x%X:TRAP_SOURCE=0x%lX:ALARMS=%i:"
			              "CLOCK=%i:FP=%d:\r\n",
			    WIND_ESTIMATION, GPS_TYPE, DEADRECKONING, BOARD_TYPE, AIRFRAME_TYPE,
			    get_reset_flags(), trap_flags, trap_source, osc_fail_count,
			    CLOCK_CONFIG, FLIGHT_PLAN_TYPE);
			break;
		case 5:
			serial_output("F4:R_STAB_A=%i:R_STAB_RD=%i:P_STAB=%i:Y_STAB_R=%i:Y_STAB_A=%i:AIL_NAV=%i:RUD_NAV=%i:AH_STAB=%i:AH_WP=%i:RACE=%i:\r\n",
			    settings._.RollStabilizaionAilerons, settings._.RollStabilizationRudder, settings._.PitchStabilization, settings._.YawStabilizationRudder, settings._.YawStabilizationAileron,
			    settings._.AileronNavigation, settings._.RudderNavigation, settings._.AltitudeholdStabilized, settings._.AltitudeholdWaypoint, settings._.RacingMode);
			break;
		case 4:
			serial_output("F5:YAWKP_A=%5.3f:YAWKD_A=%5.3f:ROLLKP=%5.3f:ROLLKD=%5.3f:A_BOOST=%5.3f:A_BOOST=NULL\r\n",
			    gains.YawKPAileron, gains.YawKDAileron, gains.RollKP, gains.RollKD);
			break;
		case 3:
			serial_output("F6:P_GAIN=%5.3f:P_KD=%5.3f:RUD_E_MIX=NULL:ROL_E_MIX=NULL:E_BOOST=%3.1f:\r\n",
			    gains.Pitchgain, gains.PitchKD, gains.ElevatorBoost);
			break;
		case 2:
			serial_output("F7:Y_KP_R=%5.4f:Y_KD_R=%5.3f:RLKP_RUD=%5.3f:RLKD_RUD=%5.3f:RUD_BOOST=%5.3f:RTL_PITCH_DN=%5.3f:\r\n",
			    gains.YawKPRudder, gains.YawKDRudder, gains.RollKPRudder, gains.RollKDRudder, gains.RudderBoost, gains.RtlPitchDown);
			break;
		case 1:
			serial_output("F8:H_MAX=%6.1f:H_MIN=%6.1f:MIN_THR=%3.2f:MAX_THR=%3.2f:PITCH_MIN_THR=%4.1f:PITCH_MAX_THR=%4.1f:PITCH_ZERO_THR=%4.1f:\r\n",
			    altit.HeightTargetMax, altit.HeightTargetMin, altit.AltHoldThrottleMin, altit.AltHoldThrottleMax,
			    altit.AltHoldPitchMin, altit.AltHoldPitchMax, altit.AltHoldPitchHigh);
			break;
		default:
		{
			// F2 below means "Format Revision 2: and is used by a Telemetry parser to invoke the right pattern matching
			// F2 is a compromise between easy reading of raw data in an ascii file and minimising extraneous data in the stream.
			toggle = !toggle;

			if (state_flags._.f13_print_req == 1)
			{
				if (toggle && !f13_print_prepare)
				{
					f13_print_prepare = true;
					return;  //wait for next run
				}
 			}
			if (!f13_print_prepare)
			{
				if (toggle)
				{
					serial_output("F2:T%li:S%d%d%d:N%li:E%li:A%li:W%i:"
					              "a%i:b%i:c%i:d%i:e%i:f%i:g%i:h%i:i%i:"
					              "c%u:s%i:cpu%u:"
					              "as%u:wvx%i:wvy%i:wvz%i:ma%i:mb%i:mc%i:svs%i:hd%i:",
					    tow.WW, udb_flags._.radio_on, dcm_flags._.nav_capable, state_flags._.GPS_steering,
					    lat_gps.WW, lon_gps.WW, alt_sl_gps.WW, waypointIndex,
					    rmat[0], rmat[1], rmat[2],
					    rmat[3], rmat[4], rmat[5],
					    rmat[6], rmat[7], rmat[8],
					    (uint16_t)cog_gps.BB, sog_gps.BB, (uint16_t)udb_cpu_load(), 
					    air_speed_3DIMU,
				    estimatedWind[0], estimatedWind[1], estimatedWind[2],
#if (MAG_YAW_DRIFT == 1)
				    magFieldEarth[0], magFieldEarth[1], magFieldEarth[2],
#else
				    (uint16_t)0, (uint16_t)0, (uint16_t)0,
#endif // MAG_YAW_DRIFT
				    svs, hdop);

					// Approximate time passing between each telemetry line, even though
					// we may not have new GPS time data each time through.
					if (tow.WW > 0) tow.WW += 250; 

					// Save  pwIn and PwOut buffers for printing next time around
					for (i = 0; i <= NUM_INPUTS; i++)
						pwIn_save[i] = udb_pwIn[i];
					for (i = 0; i <= NUM_OUTPUTS; i++)
						pwOut_save[i] = udb_pwOut[i];
				}
				else
				{
					int16_t i;
					vect3_16t goal;
					navigate_get_goal(&goal);
					for (i= 1; i <= NUM_INPUTS; i++)
						serial_output("p%ii%i:",i,pwIn_save[i]);
					for (i= 1; i <= NUM_OUTPUTS; i++)
						serial_output("p%io%i:",i,pwOut_save[i]);
					serial_output("imx%i:imy%i:imz%i:lex%i:ley%i:lez%i:fgs%X:ofc%i:tx%i:ty%i:tz%i:G%d,%d,%d:AF%i,%i,%i:",IMUlocationx._.W1,IMUlocationy._.W1,IMUlocationz._.W1,
					    locationErrorEarth[0], locationErrorEarth[1], locationErrorEarth[2],
					    state_flags.WW, osc_fail_count,
					    IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1, goal.x, goal.y, goal.z, aero_force[0], aero_force[1], aero_force[2]);
#if (USE_BAROMETER_ALTITUDE == 1)
					serial_output("tmp%i:prs%li:alt%li:",
					    get_barometer_temperature(), get_barometer_pressure(), 
					    get_barometer_altitude());
#endif
					
					serial_output("bmv%i:mA%i:mAh%i:",
#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	                battery_voltage._.W1,
#else
	                (int16_t)0,
#endif
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)                        
					battery_current._.W1, battery_mAh_used._.W1);
#else
					(int16_t)0, (int16_t)0);                    
#endif
					serial_output("DH%i:",desiredHeight);
#if (RECORD_FREE_STACK_SPACE == 1)
					extern uint16_t maxstack;
					serial_output("stk%d:", (int16_t)(4096-maxstack));
#endif // RECORD_FREE_STACK_SPACE
					serial_output("\r\n");
					serial_output("F23:G%i:V%i:\r\n",gps_parse_errors,vdop);
				}
			}
			if (state_flags._.f13_print_req == 1)
			{
				// The F13 line of telemetry is printed when origin has been captured and in between F2 lines in SERIAL_UDB_EXTRA
				if (!f13_print_prepare)
				{
					return;
				}
				else
				{
					f13_print_prepare = false;
				}
				serial_output("F13:week%i:origN%li:origE%li:origA%li:\r\n", week_no, lat_origin.WW, lon_origin.WW, alt_origin);
				serial_output("F20:NUM_IN=%i:TRIM=",NUM_INPUTS);
				for (i = 1; i <= NUM_INPUTS; i++)
				{
					serial_output("%i,",udb_pwTrim[i]);
				}
				serial_output(":\r\n");
				state_flags._.f13_print_req = 0;
			}
			break;
		}
	}
	if (telemetry_counter)
	{
		telemetry_counter--;
	}
#if (USE_TELELOG == 1)
	log_swapbuf();
#endif
}

#elif (SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI)

#warning SERIAL_OSD_REMZIBI undergoing merge to trunk

void telemetry_output_8hz(void)
{
	// TODO: Output interesting information for OSD.
	// But first we'll have to implement a buffer for passthrough characters to avoid
	// output corruption, or generate NMEA ourselves here.
}

#elif (SERIAL_OUTPUT_FORMAT == SERIAL_MAGNETOMETER)


#define OFFSETSHIFT 1

extern int16_t I2ERROR;
extern int16_t I2messages;
extern int16_t I2interrupts;

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD )
#define I2CCONREG I2C2CON
#define I2CSTATREG I2C2STAT
#else
#define I2CCONREG I2CCON
#define I2CSTATREG I2CSTAT
#endif
/*
void telemetry_output_8hz(void)
{
	serial_output("MagMessage: %i\r\nI2CCON: %X, I2CSTAT: %X, I2ERROR: %X\r\nMessages: %i\r\nInterrupts: %i\r\n\r\n",
	    magMessage,
	    I2CCONREG,  I2CSTATREG, I2ERROR,
	    I2messages, I2interrupts);
}
 */

void telemetry_output_8hz(void)
{
	if (udb_pulse_counter % (HEARTBEAT_HZ / 4) == 0) 
	{
		serial_output(" MagOffset,%i,%i,%i,"
		              " MagBody,%i,%i,%i,"
		              " MagEarth,%i,%i,%i,"
		              " MagGain,%i,%i,%i,"
		              " Calib,%i,%i,%i,"
		              " MagMessage,%i,"
		              " TotalMsg,%i,"
			      " I2CCON,0x%X, I2CSTAT,0x%X"
		              "\r\n",
#ifdef MAG_STATIC_OFFSETS
		    
		    udb_staticMagOffset[0], udb_staticMagOffset[1], udb_staticMagOffset[2],
#else
		    udb_magOffset[0]>>OFFSETSHIFT, udb_magOffset[1]>>OFFSETSHIFT, udb_magOffset[2]>>OFFSETSHIFT,
#endif
		    udb_magFieldBody[0], udb_magFieldBody[1], udb_magFieldBody[2],
		    magFieldEarth[0], magFieldEarth[1], magFieldEarth[2],
		    magGain[0], magGain[1], magGain[2],
		    rawMagCalib[0], rawMagCalib[1], rawMagCalib[2],
		    magMessage,
		    I2messages,
		    //I2CCONREG, I2CSTATREG, I2ERROR); // PDH I2ERROR no where to be found.
		    I2CCONREG, I2CSTATREG);
	}
}

#elif (SERIAL_OUTPUT_FORMAT == SERIAL_MAG_CALIBRATE)

static int16_t mag_x_axis_max = 0;
static int16_t mag_x_axis_min = 0;
static int16_t mag_y_axis_max = 0;
static int16_t mag_y_axis_min = 0;
static int16_t mag_z_axis_max = 0;
static int16_t mag_z_axis_min = 0;
static boolean first_time_through = true;


void telemetry_output_8hz(void)
{
	if (udb_pulse_counter % (HEARTBEAT_HZ / 4) == 0) 
	{
		if (first_time_through)
		{
			first_time_through = false;
			mag_x_axis_max = (udb_magFieldBody[0] + (udb_staticMagOffset[0]));
			mag_x_axis_min = (udb_magFieldBody[0] + (udb_staticMagOffset[0]));
			 mag_y_axis_max = (udb_magFieldBody[1] +(udb_staticMagOffset[1]));
			mag_y_axis_min = (udb_magFieldBody[1] + (udb_staticMagOffset[1]));
			mag_z_axis_max = (udb_magFieldBody[2] + (udb_staticMagOffset[2]));
			mag_z_axis_min = (udb_magFieldBody[2] + (udb_staticMagOffset[2]));
		}
		else
		{
			if (mag_x_axis_max > (udb_magFieldBody[0] + (udb_staticMagOffset[0]))) { mag_x_axis_max = (udb_magFieldBody[0] + (udb_staticMagOffset[0])); }
			if (mag_x_axis_min < (udb_magFieldBody[0] + (udb_staticMagOffset[0]))) { mag_x_axis_min = (udb_magFieldBody[0] + (udb_staticMagOffset[0])); }
			if (mag_y_axis_max > (udb_magFieldBody[1] + (udb_staticMagOffset[1]))) { mag_y_axis_max = (udb_magFieldBody[1] + (udb_staticMagOffset[1])); }
			if (mag_y_axis_min < (udb_magFieldBody[1] + (udb_staticMagOffset[1]))) { mag_y_axis_min = (udb_magFieldBody[1] + (udb_staticMagOffset[1])); }
			if (mag_z_axis_max > (udb_magFieldBody[2] + (udb_staticMagOffset[2]))) { mag_z_axis_max = (udb_magFieldBody[2] + (udb_staticMagOffset[2])); }
			if (mag_z_axis_min < (udb_magFieldBody[2] + (udb_staticMagOffset[2]))) { mag_z_axis_min = (udb_magFieldBody[2] + (udb_staticMagOffset[2])); }
		}
		serial_output("Mag X,%i, Mag Y,%i, Mag Z,%i, Offset X,%i, Offset Y,%i, Offset Z,%i\r\n",
		// We add in the udb_static_MagOffset in case user is re-calibrating and has left an old one defined.
		udb_magFieldBody[0] + (udb_staticMagOffset[0]),
		udb_magFieldBody[1] + (udb_staticMagOffset[1]),
		udb_magFieldBody[2] + (udb_staticMagOffset[2]),
		(mag_x_axis_max + mag_x_axis_min)  / 2,
		(mag_y_axis_max + mag_y_axis_min)  / 2,
		(mag_z_axis_max + mag_z_axis_min)  / 2
		);
		
	}
}


#elif (SERIAL_OUTPUT_FORMAT == SERIAL_CAM_TRACK)

void telemetry_output_8hz(void)
{
	uint8_t checksum = 0;
	checksum += ((union intbb)(IMUlocationx._.W1))._.B0 + ((union intbb)(IMUlocationx._.W1))._.B1;
	checksum += ((union intbb)(IMUlocationy._.W1))._.B0 + ((union intbb)(IMUlocationy._.W1))._.B1;
	checksum += ((union intbb)(IMUlocationz._.W1))._.B0 + ((union intbb)(IMUlocationz._.W1))._.B1;

	// Send location as TXXXXYYYYZZZZ*CC, at 8Hz
	// Where T marks this as a camera Tracking message
	// XXXX is the relative X location in meters as a HEX value
	// YYYY is the relative Y location in meters as a HEX value
	// ZZZZ is the relative Z location in meters as a HEX value
	// And *CC is an asterisk followed by the checksum byte in HEX.
	// The checksum is just the sum of the previous 6 bytes % 256.

	serial_output("T%04X%04X%04X*%02X\r\n",
	    IMUlocationx._.W1, IMUlocationy._.W1, IMUlocationz._.W1,
	    checksum);
}
#endif //(SERIAL_OUTPUT_FORMAT == SERIAL_DEBUG)

#else //((SERIAL_OUTPUT_FORMAT != SERIAL_NONE) && (SERIAL_OUTPUT_FORMAT != SERIAL_MAVLINK))

#if (USE_OSD != OSD_MINIM) && (USE_OSD != OSD_REMZIBI)
void telemetry_output_8hz(void)
{
}
#endif // USE_OSD

int16_t udb_serial_callback_get_byte_to_send(void)
{
	return -1;
}
void udb_serial_callback_received_byte(uint8_t rxchar)
{
}
void telemetry_restart(void)
{
}
void telemetry_init(void)
{
}
#endif //((SERIAL_OUTPUT_FORMAT != SERIAL_NONE) && (SERIAL_OUTPUT_FORMAT != SERIAL_MAVLINK))
#else  // USE_TELEMETRY

int16_t udb_serial_callback_get_byte_to_send(void)
{
	return -1;
}
void udb_serial_callback_received_byte(uint8_t rxchar)
{
}
void telemetry_restart(void)
{
}
void telemetry_output_8hz(void)
{
}
void telemetry_init(void)
{
}

#endif // USE_TELEMETRY
