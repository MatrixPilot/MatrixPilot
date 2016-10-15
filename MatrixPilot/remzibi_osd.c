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
#include "flightplan_waypoints.h"
#include "../libDCM/rmat.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/mathlibNAV.h"
#include "../libDCM/deadReckoning.h"
#include "../libUDB/osd.h"
#include "options_osd.h"
#include "navigate.h"
#include "states.h"

#if (USE_OSD == OSD_REMZIBI)

#include <string.h>
#include "osd_layout_remzibi.h"

#if ((USE_OSD == OSD_REMZIBI) && (OSD_REMZIBI_CLS_TIME == 0))
	#error("OSD_REMZIBI_CLS_TIME must be greatner that zero")
#endif

enum MP_MODE
{
	MODE_UNDEFINED,
	MODE_UNKNOWN,
	MODE_MANUAL,
	MODE_STABLE,
	MODE_WAYPOINT,
	MODE_RTH_HOLD,
	MODE_RTH
};

enum PLANE_FLIGHT_MODE
{
	PLANE_ON_GROUND,
	PLANE_IN_FLIGHT,
	PLANE_LANDED
};

#if (GPS_TYPE == GPS_MTEK)
	extern union longbbbb date_gps_, time_gps_;
#endif

static FILE* fp = NULL;

static int16_t telemetry_counter = 0;
static int16_t home_saved = false;
static int16_t mp_mode = 0;

static int16_t max_dist = 0;
static int16_t max_height = 0;
static int16_t max_speed = 0;
static int16_t on_ground_cnt = 0;
static int32_t distance = 0;

static int32_t last_lat = 0;
static int32_t last_lon = 0;

static int16_t flight_mode = PLANE_ON_GROUND;
static int16_t in_flight_counter = 0;

static const char* mp_mode_name[] = {
	"?",
	"?",
	"M",
	"S",
	"W",
	"H",
	"R"
};

void rezibi_osd_init(FILE* _fp)
{
//	udb_serial_set_rate(19200);
	fp = _fp;
}

static void serial_send_coord(int32_t coord)
{
	int32_t n1;
	int32_t n2;

	coord /= 10;
	n1 = (coord / 1000000L);
	n2 = (coord % 1000000L);
	fprintf(fp, "%li.%06li", n1, n2);
}

static void serial_send_location(int16_t loc)
{
	//$M,x,y,L,E,text,<CRLF>
	//  where:
	//        x - is int8_t value of x coordinate (ex. 12 are ASCII two bytes 31,32 , or 5 is ASCII one byte 35)
	//        y - is int8_t value of y coordinate
	//        L - is int8_t value of Leading character (ex. 178 is ASCII bytes 31 37 38)
	//        E - is int8_t value of Ending character
	//     text - is text to type as normal ASCII chars
	//  notice that coma "," at end of message must be as well as after M
	int16_t row = loc / 30;
	int16_t col = loc % 30;
	if (row > 512)
	{
		row -= 512;
		col += 128;
	}
	fprintf(fp, "$M,%i,%i,", col, row);
}

static void serial_send_string(const char* str, uint8_t leading, uint8_t tailing)
{
	if (leading != 0)
	{
		fprintf(fp, "%i", leading);
	}
	fprintf(fp, ",");
	if (tailing != 0)
	{
		fprintf(fp, "%i", tailing);
	}
	fprintf(fp, ",");
	fprintf(fp, str);
	fprintf(fp, ",\r\n");
}

static void serial_send_int(int16_t num, uint8_t leading, uint8_t tailing)
{
	if (leading != 0)
	{
		fprintf(fp, "%i", leading);
	}
	fprintf(fp, ",");
	if (tailing != 0)
	{
		fprintf(fp, "%i", tailing);
	}
	fprintf(fp, ",");
	fprintf(fp, "%i", num);
	fprintf(fp, ",\r\n");
}

static void serial_send_long(int32_t num, uint8_t leading, uint8_t tailing)
{
	if (leading != 0)
	{
		fprintf(fp, "%i", leading);
	}
	fprintf(fp, ",");
	if (tailing != 0)
	{
		fprintf(fp, "%i", tailing);
	}
	fprintf(fp, ",");
	fprintf(fp, "%li", num);
	fprintf(fp, ",\r\n");
}

static void serial_send_cls(void)
{
	fprintf(fp, "$CLS\r\n");
}

static void update_coords(void)
{
	int32_t date = 0;
	int32_t time = 0;
	int8_t earth_yaw;
	int16_t angle1;
	int16_t angle;
	struct relative2D toGoal;
	int16_t dist_to_home;
	int16_t dist_to_goal;

	uint16_t ground_speed_3DIMU = vector3_mag(IMUvelocityx._.W1,
	                                          IMUvelocityy._.W1,
	                                          IMUvelocityz._.W1);

	struct relative2D curHeading;
	curHeading.x = -rmat[1];
	curHeading.y = rmat[4];
	earth_yaw = rect_to_polar(&curHeading);  // -128 to 127 (0=East,  ccw)

	// convert to degrees
	angle1 = (earth_yaw * 180 + 64) >> 7;   // -180 to 180 (0=East,  ccw)
	if (angle1 < 0) // 0 - 360
	{
		angle1 += 360;
	}

	angle = 90 - angle1;    // course as degree - as integer , range 0 - 360, cw!! 0=North
	if (angle < 0)
	{
		angle += 360;
	}

	toGoal.x = 0 - IMUlocationx._.W1;
	toGoal.y = 0 - IMUlocationy._.W1;

	dist_to_home = toGoal.x;
	dist_to_goal;

	if (state_flags._.GPS_steering)
	{
		//dir_to_goal = desired_dir - earth_yaw;
		dist_to_goal = abs(tofinish_line);
	}
	else 
	{
		dist_to_goal = dist_to_home;
	}

#if (GPS_TYPE == GPS_MTEK)
//	date = date_gps_.WW;
	date = get_gps_date();
  #if OSD_REMZIBI_TIME_LONG_FMT
//	time = time_gps_.WW / 1000;
	time = get_gps_time() / 1000;
  #else
//	time = time_gps_.WW / 100000;
	time = get_gps_time() / 100000;
  #endif
#endif

	// GPS data:
	//
	// $A,lat,lng,numSV,[dist],alt,speed,course,[azimuth],gpsDate,gpsTime,<CRLF>
	//
	// Notice that comas must be after A and at end of message 
	// [dist] and [azimuth] can be empty , I mean  two empty comas ,,
	// Lat and lon format as float ex. -121.123456 
	// dist - as integer 
	// alt - as integer
	// course as degree - as integer , range -180 to 180 
	//
	fprintf(fp, "$A,");
	serial_send_coord(lat_gps.WW);
	fprintf(fp, ",");
	serial_send_coord(lon_gps.WW);

	fprintf(fp, ",%i,%i,%i,%i,%i,,%li,%li,\r\n",
		(int16_t)svs,
		dist_to_home,
		IMUlocationz._.W1,
		(int16_t)(ground_speed_3DIMU/51),   // OSD expects speed in knots !!  
		angle,                              // we could also put here angle to goal
		date,
		time
	);

#if (OSD_LOC_REMZIBI_DEBUG != OSD_LOC_DISABLED)
	serial_send_location(OSD_LOC_REMZIBI_DEBUG);
	fprintf(fp, ",,E %i A1 %i A %i\r\n", (int16_t)earth_yaw, angle1, angle);
#endif

#if (OSD_LOC_AP_MODE != OSD_LOC_DISABLED)
	serial_send_location(OSD_LOC_AP_MODE);
	//fprintf(fp, "$M,131,4,");
	serial_send_string(mp_mode_name[mp_mode], 0, 0);
#endif

#if (OSD_LOC_DIST_TO_GOAL != OSD_LOC_DISABLED)
	serial_send_location(OSD_LOC_DIST_TO_GOAL);
	if (mp_mode >= MODE_WAYPOINT)
	{
		serial_send_int(dist_to_goal, 0, 0x97);
	}
	else
	{
		serial_send_string("     ", 0, 0);
	}
#endif
	// save max_values
	if (max_speed < ground_speed_3DIMU)
	{
		max_speed = ground_speed_3DIMU;
	}
	if (max_height < IMUlocationz._.W1)
	{
		max_height = IMUlocationz._.W1;
	}
	if (max_dist < dist_to_home)
	{
		max_dist = dist_to_home;
	}
	// called every 0.25 sec
	// speed in cm/s
	// dist is stored in dm (0.1m = 10cm)
	if (flight_mode == PLANE_IN_FLIGHT)
	{
		distance += ground_speed_3DIMU / 40;
	}
}

static void get_mp_mode(void)
{
	if (!home_saved)
		mp_mode =  MODE_UNDEFINED;
	else if (!state_flags._.pitch_feedback)
		mp_mode =  MODE_MANUAL;
	else if (!state_flags._.GPS_steering)
		mp_mode =  MODE_STABLE;
	else if (udb_flags._.radio_on && !state_flags._.rtl_hold)
		mp_mode =  MODE_WAYPOINT;
	else if (state_flags._.rtl_hold && udb_flags._.radio_on)
		mp_mode =  MODE_RTH_HOLD;   // H : RTL Hold, has signal
	else if (!udb_flags._.radio_on)
		mp_mode =  MODE_RTH;
	else
		mp_mode =  MODE_UNKNOWN;    // Unknown
}

static void update_flight_time(void)
{
#if (OSD_LOC_FLIGHT_TIME != OSD_LOC_DISABLED)
	serial_send_location(OSD_LOC_FLIGHT_TIME);
	fprintf(fp, "232,,");
	int16_t mm = (in_flight_counter / 2) / 60;
	int16_t ss = (in_flight_counter / 2) % 60;
	fprintf(fp, "%02i:%02i,\r\n", mm, ss);
#endif

#if (OSD_LOC_DISTANCE != OSD_LOC_DISABLED)
	serial_send_location(OSD_LOC_DISTANCE);

	#if OSD_USE_METRIC_UNITS
		serial_send_long(distance / 10, 0xA8, 0xB1);
	#else
		int32_t dist = 33 * distance;
		serial_send_long(dist / 100, 0xA8, 0xAA);
	#endif

#endif

#if (OSD_LOC_OSD_LOC_WP_NO != OSD_LOC_DISABLED)
	serial_send_location(OSD_LOC_WP_NO);
	if (mp_mode >= MODE_WAYPOINT)
	{
		serial_send_int(waypointIndex, 0, 0);
	} 
	else
	{
		serial_send_string("  ", 0, 0);
	}
#endif
}

static void show_rssi(void)
{
#if (OSD_LOC_RSSI != OSD_LOC_DISABLED && ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	serial_send_location(OSD_LOC_RSSI);
	fprintf(fp, "179,,%i,\r\n", rc_signal_strength);
#endif
}

static void serial_show_AH(void)
{
#if OSD_SHOW_HORIZON
	struct relative2D matrix_accum;
	int32_t earth_pitch;    // pitch in binary angles ( 0-255 is 360 degreres)
	int32_t earth_roll;     // roll of the plane with respect to earth frame
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
	// Artificial Horizon data should be look like that :
	//   $I,roll,pitch,<CRLF>   
	//	
	//	ex .  $I,23,-112,CRLF          CRLF -are two bytes termination of line (dec 13 10) (hex 0D 0A) 
	//	Support graphical artificial horizon and pitch presentation , roll and pitch as integer type as degrees (-180,180)
	//
	fprintf(fp, "$I,%li,%li,\r\n", earth_roll, earth_pitch);
#endif // OSD_SHOW_HORIZON
}

static void init_sequence(void)
{
	static int16_t init_counter = -1;

	if (init_counter == -1)
	{
		init_counter = 0;
		serial_send_cls();
	}
	if (init_counter == 2)
	{
		update_coords();
	}
	if (init_counter == 6)
	{
		serial_send_cls();
	}
	if (init_counter == 10)
	{
		update_coords();
	}
	if (init_counter == 14)
	{
		serial_send_cls();

		//  Save Home command
		//  $SH<CRLF>

		fprintf(fp, "$SH\r\n");
		home_saved = true;
		telemetry_counter = 0;  // used to run initial cls
	}
	++init_counter;
}

// called every 0.5 sec
static void update_in_flight(void)
{
	static int16_t counter = 0;

	// simple state machine
	//   flight_mode = PLANE_ON_GROUND -> if moved 7 meters and throttle > 50% -> PLANE_IN_FLIGHT

	int32_t lat = lat_gps.WW >> 6;  // cut off unecessary digits, it's about 7 meters
	int32_t log = lat_gps.WW >> 6;
	int16_t throttleIn = udb_pwIn[THROTTLE_INPUT_CHANNEL];

	if (flight_mode == PLANE_ON_GROUND || flight_mode == PLANE_LANDED)  // we are on ground before or after flight
	{                                                   // we need 7 meters of movement
		if (last_lon != log && last_lat != lat)         // we are moving 
		{
			if (throttleIn < OSD_REMZIBI_MID_THROTTLE)  // but without throttle
			{
				last_lon = log;                         // save
				last_lat = lat;
			}
			else                                        // with throttle so we are flying
			{
				flight_mode = PLANE_IN_FLIGHT;
				serial_send_cls();                      // clear AH
			}
		}
	}
	else if (flight_mode == PLANE_IN_FLIGHT)
	{
		++in_flight_counter;
		if (last_lon != log && last_lat != lat)         // we are moving
		{
			last_lon = log;                             // save
			last_lat = lat;
			counter = 0;
		}
		else
		{
			counter++;
		}
		if (counter > 20 && throttleIn < OSD_REMZIBI_MIN_THROTTLE)  // no location change for 10 seconds and throttle low
		{
			on_ground_cnt = 0;
			in_flight_counter -= 20;
			flight_mode = PLANE_LANDED;
			serial_send_cls();                          // clear AH
		}
	}
	if (flight_mode == PLANE_LANDED)
	{
		++on_ground_cnt;                                // counter for maxes
	}
}

void remzibi_osd_8hz(void)
{
	if (fp != NULL)
	{
		if (home_saved)
		{
			if (flight_mode == PLANE_ON_GROUND || flight_mode == PLANE_IN_FLIGHT)
			{
				serial_show_AH();
			}
			if (telemetry_counter & 1)
			{
				update_coords();
			}
			if (telemetry_counter % 4 == 0)
			{
				get_mp_mode();
				update_in_flight();
				show_rssi();
			}
			if (telemetry_counter % 8 == 0)
			{
				update_flight_time();
			}
		}
		else
		{
			if (gps_nav_valid())
			{
				init_sequence();
			}
			fprintf(fp, "$M,11,8,,,MATRIXPILOT,\r\n");
			fprintf(fp, "$M,136,7,,,WAITING FOR GPS %i,\r\n", (int16_t)svs);
		}
	
		if (telemetry_counter == 24 || telemetry_counter % (OSD_REMZIBI_CLS_TIME * 8) == 0)	// 3 secs after home save or about every OSD_REMZIBI_CLS_TIME secs
		{
			serial_send_cls();
		}
	
		if (on_ground_cnt > (OSD_REMZIBI_SUMMARY_DELAY * 2))    // seconds * 2 ticks per second
		{
			// flight summary
			fprintf(fp, "$M,2,7,,,MAX,\r\n");
			fprintf(fp, "$M,130,8,167,177,%i,\r\n", max_dist);
			fprintf(fp, "$M,130,9,166,177,%i,\r\n", max_height);
#if OSD_USE_METRIC_UNITS
			fprintf(fp, "$M,130,10,168,222,%i,\r\n", max_speed /28);
			fprintf(fp, "$M,130,11,168,177,%li,\r\n", distance / 10);
#else
			fprintf(fp, "$M,130,10,168,223,%i,\r\n", max_speed / 51);
			fprintf(fp, "$M,130,11,168,170,%i,\r\n",(distance * 33) / 100);
#endif
			int16_t mm = (in_flight_counter / 2) / 60;
			int16_t ss = (in_flight_counter / 2) % 60;
			fprintf(fp, "$M,130,12,232,,%02i:%02i,\r\n", mm, ss);
		}
		++telemetry_counter;
	}
}

#endif // (USE_OSD == OSD_REMZIBI)
