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
#include "../libDCM/libDCM_internal.h"
#include "../libDCM/gpsParseCommon.h"
#include "osd_config.h"
#include <stdlib.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#if (USE_OSD == OSD_MINIM)

#include <string.h>
#include "osd_layout_remzibi.h"

extern int16_t waypointIndex;

static int16_t telemetry_counter = 0;

//void init_serial(void)
//{
//	udb_serial_set_rate(57600);
//}

static void update_coords(void)
{
	uint16_t ground_speed_3DIMU = vector3_mag(IMUvelocityx._.W1,
	                                          IMUvelocityy._.W1,
	                                          IMUvelocityz._.W1);

	struct relative2D curHeading;
	curHeading.x = -rmat[1];
	curHeading.y = rmat[4];
	int8_t earth_yaw = rect_to_polar(&curHeading);  // -128 to 127 (0=East,  ccw)

	// convert to degrees
	int16_t angle1 = (earth_yaw * 180 + 64) >> 7;   // -180 to 180 (0=East,  ccw)
	if (angle1 < 0)                                 // 0 - 360
	{
		angle1 += 360;
	}
	int16_t angle = 90 - angle1;                    // course as degree - as integer, range 0 - 360, cw!! 0=North
	if (angle < 0)
	{
		angle += 360;
	}
	struct relative2D toGoal;
	toGoal.x = 0 - IMUlocationx._.W1;
	toGoal.y = 0 - IMUlocationy._.W1;
	int16_t dist_to_home = toGoal.x;
	int16_t gps_fix_type;
	if (gps_nav_valid())
		gps_fix_type = 3;
	else
		gps_fix_type = 0;
	//
	// GPS data:
	//
	//   $A,lat,lng,numSV,alt,speed,course,fix,<CRLF>
	//
	serial_output("$A,%li,%li,%i,%i,%i,%i,%i,%i,\r\n",
		lat_gps.WW,
		lon_gps.WW,
		(int16_t)svs,
		IMUlocationz._.W1,
		(int16_t)(ground_speed_3DIMU/100),  // speed in m/s
		(int16_t)(air_speed_3DIMU/100),     // speed in m/s
		angle,
		gps_fix_type
	);
}

static void update_mp_mode(void)
{
/***
    From minimOSD code
        if (osd_mode == 0) mode_str = "manu"; //Manual
        else if (osd_mode == 1) mode_str = "circ"; //CIRCLE
        else if (osd_mode == 2) mode_str = "stab"; //Stabilize
        else if (osd_mode == 5) mode_str = "fbwa"; //FLY_BY_WIRE_A
        else if (osd_mode == 6) mode_str = "fbwb"; //FLY_BY_WIRE_B
        else if (osd_mode == 10) mode_str = "auto"; //AUTO
        else if (osd_mode == 11) mode_str = "retl"; //Return to Launch
        else if (osd_mode == 12) mode_str = "loit"; //Loiter
        else if (osd_mode == 15) mode_str = "guid"; //GUIDED 
***/

	int16_t mp_mode = 0;

	// $P,mode,CRLF
	if (!flags._.pitch_feedback)
		mp_mode = 0;
	else if (!flags._.GPS_steering)
		mp_mode = 2;
	else if (udb_flags._.radio_on && !flags._.rtl_hold)
		mp_mode = 5;
	else if (flags._.rtl_hold && udb_flags._.radio_on)
		mp_mode = 15;                       // H : RTL Hold, has signal
	else if (!udb_flags._.radio_on)
		mp_mode = 11;
	else
		mp_mode = 10;                       // Unknown

	serial_output("$P,%i,\r\n", mp_mode);
}

static void update_wp(void)
{
	struct relative2D curHeading;
	curHeading.x = -rmat[1];
	curHeading.y = rmat[4];
	int16_t earth_yaw = rect_to_polar(&curHeading);// 0-255 (0=East,  ccw)
	int16_t dir_to_goal = desired_dir - earth_yaw;

	// wp_target_bearing, wp_dist, wp_number
	serial_output("$M,%li,%li,%i,\r\n", dir_to_goal, abs(tofinish_line), waypointIndex); 
}

static void serial_show_AH(void)
{
	struct relative2D matrix_accum;
	int32_t earth_pitch;                    // pitch in binary angles ( 0-255 is 360 degreres)
	int32_t earth_roll;                     // roll of the plane with respect to earth frame

	//  Roll
	//  Earth Frame of Reference
	matrix_accum.x = rmat[8];
	matrix_accum.y = rmat[6];
	earth_roll = rect_to_polar(&matrix_accum);              // binary angle (0 - 256 = 360 degrees)
	earth_roll = (-earth_roll * BYTECIR_TO_DEGREE) >> 16;   // switch polarity, convert to -180 - 180 degrees
	earth_roll /= OSD_HORIZON_ROLL_SCALE;
#if OSD_HORIZON_ROLL_REVERSED
	earth_roll = -earth_roll;
#endif

	// Pitch
	// Earth Frame of Reference
	// Note that we are using the matrix_accum.x
	// left over from previous rect_to_polar in this calculation.
	// so this Pitch calculation must follow the Roll calculation
	matrix_accum.y = rmat[7];
	earth_pitch = rect_to_polar(&matrix_accum);             // binary angle (0 - 256 = 360 degrees)
	earth_pitch = (-earth_pitch * BYTECIR_TO_DEGREE) >> 16; // switch polarity, convert to -180 - 180 degrees
	earth_pitch /= OSD_HORIZON_PITCH_SCALE;
#if OSD_HORIZON_PITCH_REVERSED
	earth_pitch = -earth_pitch;
#endif

	// Artificial Horizon data should be look like that:
	//
	// $I,roll,pitch,<CRLF>
	//
	//  ex .  $I,23,-112,CRLF          CRLF -are two bytes termination of line (dec 13 10) (hex 0D 0A) 
	//  Support graphical artificial horizon and pitch presentation, roll and pitch as integer type as degrees (-180,180)
	//
	serial_output("$I,%li,%li,\r\n", earth_roll, earth_pitch);
}

static void update_climb_rate(void)
{
	// $Z,rate,CRLF
	// called every 1 sec so difference in height is climg rate
	static int16_t alt = 0;

	serial_output("$Z,%i,\r\n", IMUlocationz._.W1 - alt);
	alt = IMUlocationz._.W1;
}

static void update_battery(void)
{
	// $U,volts,CRLF

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	serial_output("$U,%li,\r\n", battery_voltage._.W1 * 10);
#else
	serial_output("$U,1200,\r\n");
#endif
}

static void update_channels(void)
{
	int16_t i;
	int16_t rssi = 100;

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	rssi = (rc_signal_strength * 255) / 100;
#endif
	// $C,ch1,.....,ch8,rssi,CRLF
	serial_output("$C,%i,", (unsigned)(udb_pwOut[THROTTLE_OUTPUT_CHANNEL]>>1));
	for (i = 5; i <= MIN(NUM_INPUTS, 8); i++)
	{
		serial_output("%i,", (unsigned)(udb_pwIn[i]>>1));
    }
	// if NUM_INPUTS < 8 fill remaining channels
	for (i = NUM_INPUTS+1; i <= 8; i++)
	{
		serial_output("1500,");
    }
	serial_output("%i,\r\n", rssi);
}

void serial_output_8hz(void)
{
	serial_show_AH();

	if (telemetry_counter & 1)
	{
		update_coords();
    }
	if (telemetry_counter % 4 == 1)
	{
		update_mp_mode();
		update_wp();
		update_channels();
    }
	if (telemetry_counter % 8 == 0)
	{
		update_battery();
		update_climb_rate();
    }
	++telemetry_counter;
}

#endif // (USE_OSD == OSD_MINIM)
