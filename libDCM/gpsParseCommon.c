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


#include "libDCM_internal.h"
#include "gpsParseCommon.h"
#include "estAltitude.h"
#include "mathlibNAV.h"
#include <string.h>


// GPS modules global variables
#ifdef USE_EXTENDED_NAV
struct relative3D_32 GPSlocation = { 0, 0, 0 };
#else
struct relative3D GPSlocation = { 0, 0, 0 };
#endif // USE_EXTENDED_NAV
struct relative3D GPSvelocity = { 0, 0, 0 };

union longbbbb lat_origin, lon_origin, alt_origin;
union longbbbb lat_gps, lon_gps, alt_sl_gps;        // latitude, longitude, altitude
union intbb sog_gps, climb_gps, week_no;            // speed over ground, climb
union uintbb cog_gps;                               // course over ground, units: degrees * 100, range [0-35999]
union intbb week_no;
union intbb as_sim;
union longbbbb tow;
//union longbbbb xpg, ypg, zpg;                     // gps x, y, z position
//union intbb    xvg, yvg, zvg;                     // gps x, y, z velocity
//uint8_t mode1, mode2;                             // gps mode1, mode2
uint8_t hdop;                                       // horizontal dilution of precision
uint8_t svs;                                        // number of satellites
int16_t cos_lat = 0;
int16_t gps_data_age;
const uint8_t* gps_out_buffer = 0;
int16_t gps_out_buffer_length = 0;
int16_t gps_out_index = 0;

// GPS parser modules variables
union longbbbb lat_gps_, lon_gps_;
union longbbbb alt_sl_gps_;
union longbbbb tow_;
//union intbb sog_gps_, cog_gps_, climb_gps_;
//union intbb nav_valid_, nav_type_, week_no_;
union intbb hdop_;
union longbbbb date_gps_, time_gps_;

int32_t get_gps_date(void)
{
	return date_gps_.WW;
}
int32_t get_gps_time(void)
{
	return time_gps_.WW;
}

void gps_assert(void)
{
	printf("gps_assert:" " __FILE__" " __LINE__" "\r\n");
}

void (*msg_parse)(uint8_t gpschar) = (void (*)(uint8_t))&gps_assert;
void (*gps_startup_sequence)(int16_t gpscount) = (void (*)(int16_t))&gps_assert;
boolean (*gps_nav_valid)(void) = (boolean (*)(void))&gps_assert;
void (*gps_commit_data)(void) = &gps_assert;

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

int8_t actual_dir;
uint16_t ground_velocity_magnitudeXY = 0;
int16_t forward_acceleration = 0;
uint16_t air_speed_magnitudeXY = 0;
uint16_t air_speed_3DGPS = 0;
int8_t calculated_heading;

static int8_t cog_previous = 64;
static int16_t sog_previous = 0;
static int16_t climb_rate_previous = 0;
static int16_t location_previous[] = { 0, 0, 0 };
static uint16_t velocity_previous = 0;

// Received a full set of GPS messages
void udb_background_callback_triggered(void)
{
	union longbbbb accum_nav;
	union longbbbb accum;
	union longww accum_velocity;
	int8_t cog_circular;
	int8_t cog_delta;
	int16_t sog_delta;
	int16_t climb_rate_delta;
#ifdef USE_EXTENDED_NAV
	int32_t location[3];
#else
	int16_t location[3];
#endif // USE_EXTENDED_NAV
	int16_t location_deltaZ;
	struct relative2D location_deltaXY;
	struct relative2D velocity_thru_air;
	int16_t velocity_thru_airz;

	dirovergndHRmat[0] = rmat[1];
	dirovergndHRmat[1] = rmat[4];
	dirovergndHRmat[2] = 0;

	if (gps_nav_valid())
	{
		gps_commit_data();

		gps_data_age = 0;

		dcm_callback_gps_location_updated();

#ifdef USE_EXTENDED_NAV
		location[1] = ((lat_gps.WW - lat_origin.WW)/90); // in meters, range is about 20 miles
		location[0] = long_scale((lon_gps.WW - lon_origin.WW)/90, cos_lat);
		location[2] = (alt_sl_gps.WW - alt_origin.WW)/100; // height in meters
#else
		accum_nav.WW = ((lat_gps.WW - lat_origin.WW)/90); // in meters, range is about 20 miles
		location[1] = accum_nav._.W0;
		accum_nav.WW = long_scale((lon_gps.WW - lon_origin.WW)/90, cos_lat);
		location[0] = accum_nav._.W0;
#ifdef USE_PRESSURE_ALT
#warning "using pressure altitude instead of GPS altitude"
		// division by 100 implies alt_origin is in centimeters; not documented elsewhere
		// longword result = (longword/10 - longword)/100 : range
		accum_nav.WW = ((get_barometer_altitude()/10) - alt_origin.WW)/100; // height in meters
#else
		accum_nav.WW = (alt_sl_gps.WW - alt_origin.WW)/100; // height in meters
#endif // USE_PRESSURE_ALT
		location[2] = accum_nav._.W0;
#endif // USE_EXTENDED_NAV

		// convert GPS course of 360 degrees to a binary model with 256
		accum.WW = __builtin_muluu (COURSEDEG_2_BYTECIR, cog_gps.BB) + 0x00008000;
		// re-orientate from compass (clockwise) to maths (anti-clockwise) with 0 degrees in East
		cog_circular = -accum.__.B2 + 64;

		// compensate for GPS reporting latency.
		// The dynamic model of the EM406 and uBlox is not well known.
		// However, it seems likely much of it is simply reporting latency.
		// This section of the code compensates for reporting latency.
		// markw: what is the latency? It doesn't appear numerically or as a comment
		// in the following code. Since this method is called at the GPS reporting rate
		// it must be assumed to be one reporting interval?

		if (dcm_flags._.gps_history_valid)
		{
			cog_delta = cog_circular - cog_previous;
			sog_delta = sog_gps.BB - sog_previous;
			climb_rate_delta = climb_gps.BB - climb_rate_previous;

			location_deltaXY.x = location[0] - location_previous[0];
			location_deltaXY.y = location[1] - location_previous[1];
			location_deltaZ = location[2] - location_previous[2];
		}
		else
		{
			cog_delta = sog_delta = climb_rate_delta = 0;
			location_deltaXY.x = location_deltaXY.y = location_deltaZ = 0;
		}
		dcm_flags._.gps_history_valid = 1;
		actual_dir = cog_circular + cog_delta;
		cog_previous = cog_circular;

		// Note that all these velocities are in centimeters / second

		ground_velocity_magnitudeXY = sog_gps.BB + sog_delta;
		sog_previous = sog_gps.BB;

		GPSvelocity.z = climb_gps.BB + climb_rate_delta;
		climb_rate_previous = climb_gps.BB;

		accum_velocity.WW = (__builtin_mulss(cosine(actual_dir), ground_velocity_magnitudeXY) << 2) + 0x00008000;
		GPSvelocity.x = accum_velocity._.W1;

		accum_velocity.WW = (__builtin_mulss(sine(actual_dir), ground_velocity_magnitudeXY) << 2) + 0x00008000;
		GPSvelocity.y = accum_velocity._.W1;

		rotate(&location_deltaXY, cog_delta); // this is a key step to account for rotation effects!!

		GPSlocation.x = location[0] + location_deltaXY.x;
		GPSlocation.y = location[1] + location_deltaXY.y;
		GPSlocation.z = location[2] + location_deltaZ;

		location_previous[0] = location[0];
		location_previous[1] = location[1];
		location_previous[2] = location[2];

		velocity_thru_air.y = GPSvelocity.y - estimatedWind[1];
		velocity_thru_air.x = GPSvelocity.x - estimatedWind[0];
		velocity_thru_airz  = GPSvelocity.z - estimatedWind[2];

#if (HILSIM == 1)
		air_speed_3DGPS = as_sim.BB; // use Xplane as a pitot
#else
		air_speed_3DGPS = vector3_mag(velocity_thru_air.x, velocity_thru_air.y, velocity_thru_airz);
#endif

		calculated_heading  = rect_to_polar(&velocity_thru_air);
		// veclocity_thru_air.x becomes XY air speed as a by product of CORDIC routine in rect_to_polar()
		air_speed_magnitudeXY = velocity_thru_air.x; // in cm / sec

#if (GPS_RATE == 4)
		forward_acceleration = (air_speed_3DGPS - velocity_previous) << 2; // Ublox enters code 4 times per second
#elif (GPS_RATE == 2)
		forward_acceleration = (air_speed_3DGPS - velocity_previous) << 1; // Ublox enters code 2 times per second
#else
		forward_acceleration = (air_speed_3DGPS - velocity_previous);      // EM406 standard GPS enters code once per second
#endif

		velocity_previous = air_speed_3DGPS;

		estimateWind();
		estAltitude();
		estYawDrift();
		dcm_flags._.yaw_req = 1;       // request yaw drift correction
		dcm_flags._.reckon_req = 1;    // request dead reckoning correction
		dcm_flags._.rollpitch_req = 1;
#if (DEADRECKONING == 0)
		process_flightplan();
#endif
	}
	else
	{
		gps_data_age = GPS_DATA_MAX_AGE+1;
		dirovergndHGPS[0] = dirovergndHRmat[0];
		dirovergndHGPS[1] = dirovergndHRmat[1];
		dirovergndHGPS[2] = 0;
		dcm_flags._.yaw_req = 1;            // request yaw drift correction
		dcm_flags._.gps_history_valid = 0;  // gps history has to be restarted
	}
}

#define MS_PER_DAY 86400000 // = (24 * 60 * 60 * 1000)
const uint8_t days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static uint8_t day_of_week;

int16_t calculate_week_num(int32_t date)
{
//	DPRINT("date %li\r\n", date);

	// Convert date from DDMMYY to week_num and day_of_week
	uint8_t year = date % 100;
	date /= 100;
	uint8_t month = date % 100;
	date /= 100;
	int16_t day = date % 100;

	// Wait until we have real date data
	if (day == 0 || month == 0) return 0;

	// Begin counting at May 1, 2011 since this 1st was a Sunday
	uint8_t m = 5;                          // May
	uint8_t y = 11;                         // 2011
	int16_t c = 0;                          // loop counter

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
//	DPRINT("time %li\r\n", time);

	// Convert time from HHMMSSmil to time_of_week in ms
	int16_t ms = time % 1000;
	time /= 1000;
	uint8_t s = time % 100;
	time /= 100;
	uint8_t m = time % 100;
	time /= 100;
	uint8_t h = time % 100;
	time = (((((int32_t)(h)) * 60) + m) * 60 + s) * 1000 + ms;
	return (time + (((int32_t)day_of_week) * MS_PER_DAY));
}
