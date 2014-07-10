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


#include "../MatrixPilot/defines.h" // TODO: remove, temporarily here for options to work correctly
#include "../MatrixPilot/navigate.h" // TODO: resolve this upwards include for  navigate_process_flightplan() declaration
#include "libDCM_internal.h"
#include "gpsParseCommon.h"
#include "estAltitude.h"
#include "mathlibNAV.h"
#include "rmat.h"
#include "../libUDB/interrupt.h"
#include <string.h>

// GPS parser modules variables
union longbbbb lat_gps_, lon_gps_;
union longbbbb alt_sl_gps_;
union longbbbb tow_;
//union intbb sog_gps_, cog_gps_, climb_gps_;
//union intbb nav_valid_, nav_type_, week_no_;
union intbb hdop_;
union longbbbb date_gps_, time_gps_;

extern void (*msg_parse)(uint8_t gpschar);

static const uint8_t* gps_out_buffer = 0;
static int16_t gps_out_buffer_length = 0;
static int16_t gps_out_index = 0;


int32_t get_gps_date(void)
{
	return date_gps_.WW;
}
int32_t get_gps_time(void)
{
	return time_gps_.WW;
}

void init_gps_std(void);
void init_gps_ubx(void);
void init_gps_mtek(void);
void init_gps_nmea(void);
void init_gps_none(void);

void gps_init(void)
{
#if (GPS_TYPE == GPS_STD)
	init_gps_std();
#elif (GPS_TYPE == GPS_UBX_2HZ || GPS_TYPE == GPS_UBX_4HZ)
	init_gps_ubx();
#elif (GPS_TYPE == GPS_MTEK)
	init_gps_mtek();
#elif (GPS_TYPE == GPS_NMEA)
	init_gps_nmea();
#elif (GPS_TYPE == GPS_NONE)
	init_gps_none();
#endif
}

#if (GPS_TYPE == GPS_NONE)
void init_gps_none(void) { }
boolean gps_nav_valid(void) { return 0; }
void gps_startup_sequence(int16_t gpscount) { }
void gps_commit_data(void) { }
void gps_parse_none(uint8_t gpschar) { }
void (*msg_parse)(uint8_t) = &gps_parse_none;
#endif // GPS_TYPE


void gpsoutbin(int16_t length, const uint8_t msg[]) // output a binary message to the GPS
{
	gps_out_buffer = 0; // clear the buffer pointer first, for safety, in case we're interrupted
	gps_out_index = 0;
	gps_out_buffer_length = length;
	gps_out_buffer = (uint8_t*)msg;

	udb_gps_start_sending_data();
}

void gpsoutline(const char *message) // output one NMEA line to the GPS
{
	gpsoutbin(strlen(message), (const uint8_t*)message);
}

int16_t udb_gps_callback_get_byte_to_send(void)
{
	if (gps_out_buffer != 0 && gps_out_index < gps_out_buffer_length)
	{
		// We have a byte to send
		return (uint8_t)(gps_out_buffer[gps_out_index++]);
	}
	else
	{
		// No byte to send, so clear the link to the buffer
		gps_out_buffer = 0;
	}
	return -1;
}

// Got a character from the GPS
void udb_gps_callback_received_byte(uint8_t rxchar)
{
	//bin_out(rxchar);      // binary out to the debugging USART
	(*msg_parse)(rxchar);   // parse the input byte
}

boolean gps_nav_capable_check_set(void)
{ // TODO: gps_data_age is not being set by NMEA or STD parsers?
	if (gps_data_age < GPS_DATA_MAX_AGE) gps_data_age++;
	dcm_flags._.nav_capable = (gps_data_age < GPS_DATA_MAX_AGE);
//	return (gps_data_age < GPS_DATA_MAX_AGE);
	return dcm_flags._.nav_capable;
}

static void gps_parse_common_callback(void)
{
	// TODO: determine what this is all about and place it in a meaningful function name in module rmat.c
	dirOverGndHrmat[0] = rmat[1];
	dirOverGndHrmat[1] = rmat[4];
	dirOverGndHrmat[2] = 0;

	if (gps_nav_valid())
	{
		gps_commit_data();
#if (AIRFRAME_TYPE != AIRFRAME_QUAD)
#else
		//FIXME: hack to turn on dead reckoning
		if (!dcm_flags._.dead_reckon_enable)
		{
			dcm_set_origin_location(lon_gps.WW, lat_gps.WW, alt_sl_gps.WW);
			dcm_flags._.dead_reckon_enable = 1;
			// initialize boxCar filter state
			init_boxCarState(boxCarLen, boxCarN, boxCarBuff, boxCarSum, &filterState);
		}
#endif // AIRFRAME_TYPE
		gps_data_age = 0;
		dcm_callback_gps_location_updated(); // TODO: this in implemented in the MatrixPilot layer

		estLocation();
		estWind(&estimatedWind, GPSvelocity);
		estAltitude();
		estYawDrift();

		dcm_flags._.yaw_req = 1;       // request yaw drift correction
		dcm_flags._.reckon_req = 1;    // request dead reckoning correction
		dcm_flags._.rollpitch_req = 1;
#if (AIRFRAME_TYPE != AIRFRAME_QUAD)
#else
		dcm_flags._.integrate_req = 1; // request cm precision position update
		sendGPS = true;                // send gps telemetry record
//		tailFlash = 1;
#endif // AIRFRAME_TYPE
#if (DEADRECKONING == 0)
		navigate_process_flightplan(); // TODO: this in implemented in the MatrixPilot layer
#endif
	}
	else
	{
		gps_data_age = GPS_DATA_MAX_AGE+1;

		// TODO: determine what this is all about and place it in a meaningful function name in module rmat.c
		dirOverGndHGPS[0] = dirOverGndHrmat[0];
		dirOverGndHGPS[1] = dirOverGndHrmat[1];
		dirOverGndHGPS[2] = 0;
		dcm_flags._.yaw_req = 1;            // request yaw drift correction
		dcm_flags._.gps_history_valid = 0;  // gps history has to be restarted
	}
}

// Received a full set of GPS messages
void gps_parse_common(void)
{
	// TODO: perhaps have a boolean variable to reset gps_data_age??
	udb_background_trigger(&gps_parse_common_callback);
}

#define MS_PER_DAY 86400000 // = (24 * 60 * 60 * 1000)
const uint8_t days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static uint8_t day_of_week;

int16_t calculate_week_num(int32_t date)
{
	uint8_t year;
	uint8_t month;
	int16_t day;
	uint8_t m;
	uint8_t y;
	int16_t c;

//	DPRINT("date %li\r\n", date);

	// Convert date from DDMMYY to week_num and day_of_week
	year = date % 100;
	date /= 100;
	month = date % 100;
	date /= 100;
	day = date % 100;

	// Wait until we have real date data
	if (day == 0 || month == 0) return 0;

	// Begin counting at May 1, 2011 since this 1st was a Sunday
	m = 5;                          // May
	y = 11;                         // 2011
	c = 0;                          // loop counter

	while (m < month || y < year)
	{
		day += days_in_month[m-1];          // (m == 1) means Jan, so use days_in_month[0]
		if ((m == 2) && (y % 4 == 0) && (y % 100 != 0))
		{
			day += 1;                       // Add leap day
		}
		m++;
		if (m == 13)
		{
			m = 1;
			y++;
		}
		if (++c > 1200) break;              // Emergency escape from this loop.  Works correctly until May 2111.
	}
	day_of_week = (day % 7) - 1;
	return (1634 + (day / 7));              // We started at week number 1634
}

int32_t calculate_time_of_week(int32_t time)
{
	int16_t ms;
	uint8_t s;
	uint8_t m;
	uint8_t h;
	
//	DPRINT("time %li\r\n", time);

	// Convert time from HHMMSSmil to time_of_week in ms
	ms = time % 1000;
	time /= 1000;
	s = time % 100;
	time /= 100;
	m = time % 100;
	time /= 100;
	h = time % 100;
	time = (((((int32_t)(h)) * 60) + m) * 60 + s) * 1000 + ms;
	return (time + (((int32_t)day_of_week) * MS_PER_DAY));
}
