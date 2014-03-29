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
#include "../libUDB/heartbeat.h"
#include "../libUDB/magnetometer.h"
#include "../libUDB/barometer.h"
#include "../libUDB/ADchannel.h"
#include "estAltitude.h"
#include "mathlibNAV.h"
#include "rmat.h"
#include <math.h>


union dcm_fbts_word dcm_flags;

// Calibrate for 10 seconds before moving servos
#define CALIB_COUNT  400    // 10 seconds at 40 Hz
#define GPS_COUNT    1000   // 25 seconds at 40 Hz

void send_HILSIM_outputs(void);


void foo_test(union longww longfoo)
{
	int16_t a1, a0;

	a1 = longfoo._.W1;
	a0 = longfoo._.W0;
	printf("foo_test() W1 = %i, W0 = %i\r\n", a1, a0);
}
/*
void foo_test2(union longww longfoo)
{
#define _LOWORD(a) (a._.W0)
#define LOWORD(a) _LOWORD((int16_t)((union longww)a))
#define _HIWORD(a) (a._.W1)
#define HIWORD(a) _HIWORD((int16_t)((union longww)a))

	int16_t b, c;
	int32_t x = longfoo.WW;

	b = HIWORD(x);
	c = LOWORD(x);
	printf("foo_test() HIWORD = %i, LOWORD = %i\r\n", b, c);
}
 */
void vect_test(void)
{
	vect2_16t a;
	vect2_16t b;

	a.x = 5;
	a.y = 34;

	b = a;

	printf("vect_test() b.x %i b.y %i\r\n", b.x, b.y);

}

void dcm_init(void)
{
	union longww longfoo = { 0 };

	longfoo._.W1 = 1;
	longfoo._.W0 = 0;
	printf("longfoo W1:1 W0:0 = %li\r\n", longfoo.WW);
	foo_test(longfoo);

	longfoo._.W1 = 0;
	longfoo._.W0 = 1;
	printf("longfoo W1:0 W0:1 = %li\r\n", longfoo.WW);
	foo_test(longfoo);

	vect_test();

	dcm_flags.W = 0;
	dcm_flags._.first_mag_reading = 1;
	dcm_init_rmat();
}

void dcm_run_init_step(uint16_t count)
{
//	DPRINT("%u\r\n", count);

	if (count == CALIB_COUNT)
	{
		// Finish calibration
		DPRINT("calib_finished\r\n");
		dcm_flags._.calib_finished = 1;
		dcm_calibrate();
	}

	if (count <= GPS_COUNT)
	{
		gps_startup_sequence(GPS_COUNT - count); // Counts down from GPS_COUNT to 0
		if (count == GPS_COUNT)
		{
			DPRINT("init_finished\r\n");
			dcm_flags._.init_finished = 1;
		}
	}
}

#if (BAROMETER_ALTITUDE == 1)

// We want to be reading both the magnetometer and the barometer at 4Hz
// The magnetometer driver returns a new result via the callback on each call
// The barometer driver needs to be called several times to get a single 
//  result set via the callback. Also on first invocation the barometer driver
//  reads calibration data, and hence requires one extra call

void do_I2C_stuff(void)
{
	static int toggle = 0;
	static int counter = 0;

	if (toggle) {
		if (counter++ > 0) {
//#if (MAG_YAW_DRIFT == 1 && HILSIM != 1)
#if (MAG_YAW_DRIFT == 1)
//			printf("rxMag %u\r\n", udb_heartbeat_counter);
			rxMagnetometer(udb_magnetometer_callback);
#endif
			counter = 0;
			toggle = 0;
		}
	} else {
		rxBarometer(udb_barometer_callback);
		if (counter++ > 6) {
			counter = 0;
			toggle = 1;
		}
	}
}
#endif // BAROMETER_ALTITUDE

// Called at HEARTBEAT_HZ
void udb_heartbeat_callback(void)
{
#if (BAROMETER_ALTITUDE == 1)
	if (udb_heartbeat_counter % (HEARTBEAT_HZ / 40) == 0)
	{
		do_I2C_stuff(); // TODO: this should always be be called at 40Hz
	}
#else
//#if (MAG_YAW_DRIFT == 1 && HILSIM != 1)
#if (MAG_YAW_DRIFT == 1)
	// This is a simple counter to do stuff at 4hz
//	if (udb_heartbeat_counter % 10 == 0)
	if (udb_heartbeat_counter % (HEARTBEAT_HZ / 4) == 0)
	{
		rxMagnetometer(udb_magnetometer_callback);
	}
#endif
#endif // BAROMETER_ALTITUDE

//  when we move the IMU step to the MPU call back, to run at 200 Hz, remove this
	if (dcm_flags._.calib_finished)
	{
		dcm_run_imu_step();
	}

	dcm_heartbeat_callback();    // this was called dcm_servo_callback_prepare_outputs();

	if (!dcm_flags._.init_finished)
	{
		if (udb_heartbeat_counter % (HEARTBEAT_HZ / 40) == 0)
		{
			dcm_run_init_step(udb_heartbeat_counter / (HEARTBEAT_HZ / 40));
		}
	}

#if (HILSIM == 1)
	send_HILSIM_outputs();
#endif
}

// dcm_calibrate is called twice during the startup sequence.
// Firstly 10 seconds after startup, then immediately before the first waggle, which is 10 seconds after getting radio link.  
// This makes sure we get initialized when there's no radio, or when bench testing, 
// and 2nd time is at a time the user knows to keep the plane steady before a flight.
void dcm_calibrate(void)
{
	// Don't allow re/calibrating before the initial calibration period has finished
	if (dcm_flags._.calib_finished)
	{
		udb_a2d_record_offsets();
	}
}

void dcm_set_origin_location(int32_t o_lon, int32_t o_lat, int32_t o_alt)
{
	union longbbbb accum_nav;
	unsigned char lat_cir;

printf("o_lon %li o_lat %li o_alt %li\r\n", o_lon, o_lat, o_alt); // o_lon 113480854 o_lat 472580108 o_alt 57763
printf("longitude %f latitude %f altitude %f\r\n", (double)o_lon / 10000000, (double)o_lat / 10000000, (double)o_alt / 100); // o_lon 113480854 o_lat 472580108 o_alt 57763
	lat_origin.WW = o_lat;
	lon_origin.WW = o_lon;
	alt_origin.WW = o_alt;

	// scale the low 16 bits of latitude from GPS units to gentleNAV units
	accum_nav.WW = __builtin_mulss(LONGDEG_2_BYTECIR, lat_origin._.W1);
//printf("accum_nav.WW %i\r\n", accum_nav.WW);
	lat_cir = accum_nav.__.B2;  // effectively divides by 256
//printf("lat_cir %i\r\n", lat_cir);
	// estimate the cosine of the latitude, which is used later computing desired course
	cos_lat = cosine(lat_cir);
}
// o_lat 472581329       = 47.2 degrees
// accum_nav.WW 2199355
// lat_cir 33

extern fractional Float2Fract(float aVal);	/* Converts float into fractional */
                                            /* float value in range [-1, 1) */
extern float Fract2Float(fractional aVal);  /* Converts fractional into float */
                                            /* fract value in range {-1, 1-2^-15} */
void dcm_fract_test(int32_t o_lat)
{
/*
	union longbbbb lat_origin;
	union longbbbb accum_nav;
	unsigned char lat_cir;

printf("-------------------------------------------------------------------------------\r\n");

// convert degrees to radians   radians = degrees * pi / 180
// convert radians to degrees   degrees = radians * 180 / pi

	lat_origin.WW = o_lat;

	// scale the latitude from GPS units to gentleNAV units
	accum_nav.WW = __builtin_mulss(LONGDEG_2_BYTECIR, lat_origin._.W1);
//#define LONGDEG_2_BYTECIR   305 // = (256/360)*((256)**4)/(10**7)

	lat_cir = accum_nav.__.B2;
	// estimate the cosine of the latitude, which is used later computing desired course

// returns(2**14)*cosine(angle), angle measured in units of pi/128 radians
	cos_lat = cosine(lat_cir);

	// A mathematical angle of plus or minus pi is represented digitally as plus or minus 128.

printf("lat_cir %i\r\n", lat_cir);
printf("cos_lat %i\r\n", cos_lat);
printf("lat_cir %f\r\n", Fract2Float(lat_cir));
printf("cos_lat %f\r\n", Fract2Float(cos_lat));

//	cos_lat = Float2Fract(cos(Fract2Float(lat_cir)*128));
//printf("cos_lat %f\r\n", Fract2Float(cos_lat));

//	    v = _Q15cosPI(0);
//printf("cos_lat %i\r\n", cos_lat);
  {
	  float degrees;
	  float radians;
	  float cos_lat_rad;

	  degrees = o_lat / 10000000.0;
	  radians = degrees * 3.1415926 / 180;
	  cos_lat_rad = cos(radians);
	  printf("degrees %f radians %f cos_lat_rad %f\r\n", degrees, radians, cos_lat_rad);

	  o_lat = 900000000;
	  degrees = o_lat / 10000000.0;
	  radians = degrees * 3.1415926 / 180;
	  cos_lat_rad = cos(radians);
	  printf("degrees %f radians %f cos_lat_rad %f\r\n", degrees, radians, cos_lat_rad);

	  o_lat = 2147483647;
	  degrees = o_lat / 10000000.0;
	  radians = degrees * 3.1415926 / 180;
	  cos_lat_rad = cos(radians);
	  printf("degrees %f radians %f cos_lat_rad %f\r\n", degrees, radians, cos_lat_rad);

//	fractional r1 = Float2Fract(0.5);
//	float f2 = Fract2Float(r1);
//	printf("f2 %f\r\n", f2);
  }

	printf("-------------------------------------------------------------------------------\r\n");
 */
}

struct relative3D dcm_absolute_to_relative(struct waypoint3D absolute)
{
	struct relative3D rel;

	rel.z = absolute.z;
	rel.y = (absolute.y - lat_origin.WW) / 90; // in meters
	rel.x = long_scale((absolute.x - lon_origin.WW) / 90, cos_lat);
	return rel;
}

#ifdef USE_EXTENDED_NAV
struct relative3D_32 dcm_absolute_to_relative_32(struct waypoint3D absolute)
{
	struct relative3D_32 rel;

	rel.z = absolute.z;
	rel.y = (absolute.y - lat_origin.WW) / 90; // in meters
	rel.x = long_scale((absolute.x - lon_origin.WW) / 90, cos_lat);
	return rel;
}
#endif // USE_EXTENDED_NAV

vect3_32t dcm_rel2abs(vect3_32t rel)
{
	vect3_32t abs;

	abs.z = rel.z;
	abs.y = (rel.y * 90) + lat_origin.WW;
	abs.x = (rel.x * 90) + lon_origin.WW;
	abs.x = long_scale((rel.x * 90), cos_lat) + lon_origin.WW;

// didn't work )-:
//unsigned int inv_cos_lat = __builtin_divsd(1, cos_lat);
//	abs.x = long_scale((rel.x * 90), inv_cos_lat) + lon_origin.WW;

	return abs;
}
/*
  In going from absolute to relative, you multiply ((absolute.x - lon_origin.WW) / 90) by cos_lat.
  In going from relative to absolute, you divide (rel.x*90) by cos_lat.
  Basically, you will have to implement a function "long_divide" such that long_divide( ( long_scale ( X , cos_lat)  ) , cos_lat ) = X
 */
