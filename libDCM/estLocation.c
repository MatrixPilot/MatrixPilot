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


#include "libDCM.h"
#include "gpsData.h"
#include "gpsParseCommon.h"
#include "estLocation.h"
#include "estAltitude.h"  //USE_BAROMETER_ALTITUDE
#include "mathlibNAV.h"
#include "estWind.h"


#ifdef USE_EXTENDED_NAV
static void location_plane(int32_t* location)
{
	location[1] = ((lat_gps.WW - lat_origin.WW)/90); // in meters, range is about 20 miles
	location[0] = long_scale((lon_gps.WW - lon_origin.WW)/90, cos_lat);
#if (USE_BAROMETER_ALTITUDE == 1 ) 
#warning "using pressure altitude instead of GPS altitude"
	// division by 100 implies alt_origin is in centimeters; not documented elsewhere
	// longword result = (longword/10 - longword)/100 : range
	location[2] = ((get_barometer_altitude()/10) - alt_origin.WW)/100; // height in meters
#else
	location[2] = (alt_sl_gps.WW - alt_origin.WW)/100; // height in meters
#endif // USE_BAROMETER_ALTITUDE

}
#else // !USE_EXTENDED_NAV
static void location_plane(int16_t* location)
{
	union longbbbb accum_nav;

	accum_nav.WW = ((lat_gps.WW - lat_origin.WW)/90); // in meters, range is about 20 miles
	location[1] = accum_nav._.W0;
	accum_nav.WW = long_scale((lon_gps.WW - lon_origin.WW)/90, cos_lat);
	location[0] = accum_nav._.W0;
#if (USE_BAROMETER_ALTITUDE == 1 ) 
#warning "using pressure altitude instead of GPS altitude"
	// division by 100 implies alt_origin is in centimeters; not documented elsewhere
	// longword result = (longword/10 - longword)/100 : range
	accum_nav.WW = ((get_barometer_altitude()/10) - alt_origin.WW)/100; // height in meters
#else
	accum_nav.WW = (alt_sl_gps.WW - alt_origin.WW)/100; // height in meters
#endif // USE_BAROMETER_ALTITUDE
	location[2] = accum_nav._.W0;
}
#endif // USE_EXTENDED_NAV

void estLocation(void)
{
	static int8_t cog_previous = 64;
	static int16_t sog_previous = 0;
	static int16_t climb_rate_previous = 0;
	static uint16_t velocity_previous = 0;
	static int16_t location_previous[] = { 0, 0, 0 };

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

	location_plane(&location[0]);

	// convert GPS course of 360 degrees to a binary model with 256
	accum.WW = __builtin_muluu(COURSEDEG_2_BYTECIR, cog_gps.BB) + 0x00008000;
	// re-orientate from compass (clockwise) to maths (anti-clockwise) with 0 degrees in East
	cog_circular = -accum.__.B2 + 64;

	// compensate for GPS reporting latency.
	// The dynamic model of the EM406 and uBlox is not well known.
	// However, it seems likely much of it is simply reporting latency.
	// This section of the code compensates for reporting latency.
	// markw: what is the latency? It doesn't appear numerically or as a comment
	// in the following code. Since this method is called at the GPS reporting rate
	// it must be assumed to be one reporting interval?
#if (HILSIM != 1)
	if (dcm_flags._.gps_history_valid)
	{
		cog_delta = cog_circular - cog_previous;
		sog_delta = sog_gps.BB - sog_previous;
		climb_rate_delta = climb_gps.BB - climb_rate_previous;

		location_deltaXY.x = location[0] - location_previous[0];
		location_deltaXY.y = location[1] - location_previous[1];
		location_deltaZ    = location[2] - location_previous[2];
	}
	else
	{
		cog_delta = 0;
		sog_delta = 0;
		climb_rate_delta = 0;
		location_deltaXY.x = location_deltaXY.y = location_deltaZ = 0;
	}
#else
	cog_delta = 0;
	sog_delta = 0;
	climb_rate_delta = 0;
	location_deltaXY.x = location_deltaXY.y = location_deltaZ = 0;
#endif //#if (HILSIM != 1)
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

	rotate_2D(&location_deltaXY, cog_delta); // this is a key step to account for rotation effects!!

	GPSlocation.x = location[0] + location_deltaXY.x;
	GPSlocation.y = location[1] + location_deltaXY.y;
	GPSlocation.z = location[2] + location_deltaZ;

	location_previous[0] = location[0];
	location_previous[1] = location[1];
	location_previous[2] = location[2];

	velocity_thru_air.y = GPSvelocity.y - estimatedWind[1];
	velocity_thru_air.x = GPSvelocity.x - estimatedWind[0];
	velocity_thru_airz  = GPSvelocity.z - estimatedWind[2];

	air_speed_3DGPS = vector3_mag(velocity_thru_air.x, velocity_thru_air.y, velocity_thru_airz);

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
}

