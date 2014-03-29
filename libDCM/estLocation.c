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
//#include "../MatrixPilot/navigate.h" // TODO: resolve this upwards include for  navigate_process_flightplan() declaration
#include "libDCM_internal.h"
#include "gpsParseCommon.h"
//#include "estAltitude.h"
#include "mathlibNAV.h"
//#include "rmat.h"
//#include "../libUDB/interrupt.h"
//#include <string.h>


//#if (AIRFRAME_TYPE != AIRFRAME_QUAD)
//#else
#include "../libUDB/filters.h"

// multiplier for GPS x,y units (1/90 originally => 1 LSB / meter)
#define LATLON2CM (10/9)

struct relative3D GPSloc_cm = {0, 0, 0};
// boxcar integrator buffer
const int boxCarN = 3;
const int boxCarLen = 10;
struct boxCarState filterState;
int boxCarBuff[30];
long boxCarSum[3];

extern boolean sendGPS;
extern int tailFlash;

//#endif // AIRFRAME_TYPE



#ifdef USE_EXTENDED_NAV
static void location_plane(vect3_32t* gpsloc)
{
	gpsloc->y = ((lat_gps.WW - lat_origin.WW)/90); // in meters, range is about 20 miles
	gpsloc->x = long_scale((lon_gps.WW - lon_origin.WW)/90, cos_lat);
	gpsloc->z = (alt_sl_gps.WW - alt_origin.WW)/100; // height in meters
}
#else // !USE_EXTENDED_NAV
static void location_plane(vect3_16t* gpsloc)
{
	union longbbbb accum_nav;

		accum_nav.WW = ((lat_gps.WW - lat_origin.WW)/90); // in meters, range is about 20 miles
		gpsloc->y = accum_nav._.W0;
		accum_nav.WW = long_scale((lon_gps.WW - lon_origin.WW)/90, cos_lat);
		gpsloc->x = accum_nav._.W0;
#ifdef USE_PRESSURE_ALT
#warning "using pressure altitude instead of GPS altitude"
		// division by 100 implies alt_origin is in centimeters; not documented elsewhere
		// longword result = (longword/10 - longword)/100 : range
		accum_nav.WW = ((get_barometer_altitude()/10) - alt_origin.WW)/100; // height in meters
#else
		accum_nav.WW = (alt_sl_gps.WW - alt_origin.WW)/100; // height in meters
#endif // USE_PRESSURE_ALT
		gpsloc->z = accum_nav._.W0;
}
#endif // USE_EXTENDED_NAV

#ifdef USE_EXTENDED_NAV
static void location_quad(vect3_32t* gpsloc)
#else // !USE_EXTENDED_NAV
static void location_quad(vect3_16t* gpsloc)
#endif // USE_EXTENDED_NAV
{
	// convert from degrees of latitude to meters; spherical earth circumference is 360 degrees ~= 40030km
	// => 1 degree ~= 111km => 1.11e5 m/deg
	// lat_gps is degrees * 1e7 and we want centimeters = degrees * 1.11e5: 1.11e5 ~= 1e7 * 100 / 90
	// 32 bit result <- delta degrees*1E7 * 100 / 90
//	long loc_cm_sum[3];
	static vect3_16t loc_cm_avg;

	union longbbbb accum_nav;
	vect3_16t loc_cm;

		accum_nav.WW = ((lat_gps.WW - lat_origin.WW) * LATLON2CM) ; // in centimeters
//		GPSloc_cm.y = accum_nav._.W0 ;  // low 16 bits of result, range is +/-327 meters
		loc_cm.y = accum_nav._.W0 ;  // low 16 bits of result, range is +/-327 meters
		accum_nav.WW *= .01 ;		   // meters
		gpsloc->y = accum_nav._.W0 ;  // low 16 bits of result, range is about 20 miles

		// multiply the longitude delta by the cosine of the latitude
		accum_nav.WW = ((lon_gps.WW - lon_origin.WW) * LATLON2CM); // in centimeters
		accum_nav.WW = ((__builtin_mulss(cos_lat, accum_nav._.W0) << 2));
//		GPSloc_cm.x = accum_nav._.W1 ;
		loc_cm.x = accum_nav._.W1 ;  // high 16 bits of result, range is +/-327 meters
		accum_nav.WW *= .01 ;
		gpsloc->x = accum_nav._.W1 ;

		// alt_sl_gps is meters * 100
		accum_nav.WW = (alt_sl_gps.WW - alt_origin.WW) ; // altitude (above origin) in centimeters
//		GPSloc_cm.z = accum_nav._.W0;
		loc_cm.z = accum_nav._.W0 ;  // low 16 bits of result, range is +/-327 meters
		accum_nav.WW *= .01 ;		   // meters
		gpsloc->z = accum_nav._.W0;

		// run boxcar filter on new position
		boxcar(&loc_cm, &filterState, &loc_cm_avg);
		// copy results back to GPSloc_cm
		GPSloc_cm.x = loc_cm_avg.x;
		GPSloc_cm.y = loc_cm_avg.y;
		GPSloc_cm.z = loc_cm_avg.z;
}

void estLocation(void)
{
	static int8_t cog_previous = 64;
	static int16_t sog_previous = 0;
	static int16_t climb_rate_previous = 0;
	static uint16_t velocity_previous = 0;
	static vect3_16t gpsloc_previous = { 0, 0, 0 };

	union longbbbb accum;
	union longww accum_velocity;
	int8_t cog_circular;
	int8_t cog_delta;
	int16_t sog_delta;
	int16_t climb_rate_delta;
#ifdef USE_EXTENDED_NAV
	vect3_32t gpsloc;
#else
	vect3_16t gpsloc;
#endif // USE_EXTENDED_NAV
	int16_t location_deltaZ;
	struct relative2D location_deltaXY;
	struct relative2D velocity_thru_air;
	int16_t velocity_thru_airz;

#if (AIRFRAME_TYPE != AIRFRAME_QUAD)
	location_plane(&gpsloc);
#else // AIRFRAME_QUAD
	location_quad(&gpsloc);
#endif // AIRFRAME_TYPE

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

		if (dcm_flags._.gps_history_valid)
		{
			cog_delta = cog_circular - cog_previous;
			sog_delta = sog_gps.BB - sog_previous;
			climb_rate_delta = climb_gps.BB - climb_rate_previous;

			location_deltaXY.x = gpsloc.x - gpsloc_previous.x;
			location_deltaXY.y = gpsloc.y - gpsloc_previous.y;
			location_deltaZ    = gpsloc.z - gpsloc_previous.z;
		}
		else
		{
			cog_delta = 0;
			sog_delta = 0;
			climb_rate_delta = 0;
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

		rotate_2D(&location_deltaXY, cog_delta); // this is a key step to account for rotation effects!!

		GPSlocation.x = gpsloc.x + location_deltaXY.x;
		GPSlocation.y = gpsloc.y + location_deltaXY.y;
		GPSlocation.z = gpsloc.z + location_deltaZ;

		gpsloc_previous.x = gpsloc.x;
		gpsloc_previous.y = gpsloc.y;
		gpsloc_previous.z = gpsloc.z;

		velocity_thru_air.y = GPSvelocity.y - estimatedWind.y;
		velocity_thru_air.x = GPSvelocity.x - estimatedWind.x;
		velocity_thru_airz  = GPSvelocity.z - estimatedWind.z;

#if (HILSIM == 1)
		air_speed_3DGPS = hilsim_airspeed.BB; // use Xplane as a pitot
#else
		air_speed_3DGPS = vector3_mag(velocity_thru_air.x, velocity_thru_air.y, velocity_thru_airz);
#endif

		calculated_heading = rect_to_polar(&velocity_thru_air);
//////////////
//	DPRINT("gpsParseCommon:rect_to_polar(velocity_thru_air.x %i, .y %i) returned %i:\r\n", velocity_thru_air.x, velocity_thru_air.y, calculated_heading);
//////////////
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

