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
#include "defines.h"
#include "../libUDB/barometer.h"
#include "estAltitude.h"
#include <stdio.h>

//	The origin is recorded as the altitude of the plane during power up of the control.

long barometer_pressure_gnd = 0;
int barometer_temperature_gnd = 0;

long barometer_altitude;
long est_barometer_altitude;
long barometer_ground_altitude;
long barometer_pressure;
int barometer_temperature;
float altitude;
float sea_level_pressure;

inline long get_barometer_altitude(void) { return barometer_altitude; }
inline long get_barometer_pressure(void) { return barometer_pressure; }
inline int get_barometer_temperature(void) { return barometer_temperature; }
// added MODs 
inline long get_barometer_ground_altitude(void) { return barometer_ground_altitude; }
inline long get_est_barometer_altitude(void) { return est_barometer_altitude; }


void altimeter_calibrate(void)
{
	barometer_pressure_gnd = barometer_pressure;
	barometer_temperature_gnd = barometer_temperature;
	#ifdef USE_DEBUG_IO
		printf( "altimeter_calibrate: ground temp & pres set %.1f, %.2f\r\n", (double)barometer_temperature_gnd / 10.0, (double)barometer_pressure_gnd / 100.0);
	#endif
}

#if (USE_BAROMETER == 1)
	void udb_barometer_callback(long pressure, int temperature, char status)
	{
	#if (USE_PA_PRESSURE == 1)		     							// **** OPTION TO USE PRESSURE OR HOME POSITION ALTITUDE  in options.h   ****
		const float p0 = PA_PRESSURE;    							// **** Current pressure at sea level (Pa) defined in options.h   ****
		altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
		#if ( SONAR_ALTITUDE == 1 ) 
			barometer_ground_altitude = ((float)44330 * (1 - pow(((float) barometer_pressure/p0), 0.190295))-(sonar_altitude/100));
		#else 
			barometer_ground_altitude = (float)44330 * (1 - pow(((float) barometer_pressure/p0), 0.190295));
		//return ;
		#endif
	#else
		const float ground_altitude = (ASL_GROUND_ALT/100);			// **** defined HOME ASL GROUND ALTITUDE in options.h  ****
	//	float altitude;
		float sea_level_pressure;
		barometer_pressure = pressure / 100;
		barometer_temperature = temperature / 10;
		sea_level_pressure = ((float)pressure / powf((1 - (ground_altitude/44330.0)), 5.255));
	 	altitude = (float)44330 * (1 - pow(((float) pressure/sea_level_pressure), 0.190295));  // this is just the reverse of the sea_level_pressure algorithm for testing
	#endif
	#ifdef USE_DEBUG_IO
	//	printf( "T = %.1f C, P = %.2f mB, A = %.2f m\r\n", (double)temperature / 10.0, (double)pressure / 100.0, (double)altitude);
		printf( "barom %.1f, %.2f, %.2f, slp %.2f\r\n", (double)temperature / 10.0, (double)pressure / 100.0, (double)altitude, (double)sea_level_pressure / 100.0);
	#endif
	}
#endif

void estAltitude(void)
{
	est_barometer_altitude = (float)44330 * (1 - pow(((float) barometer_pressure/barometer_pressure_gnd), 0.190295));
	barometer_altitude = altitude ;
	barometer_ground_altitude = barometer_ground_altitude;

	return ;
}



// This will never work as the very GPS update that calls this uses the debug_io serial port...
//#ifdef USE_DEBUG_IO
//	printf( "estAltitude %.2f\r\n", (double)barometer_altitude);
//#endif
// MAVLINK_MESSAGE_INFO_SCALED_PRESSURE
/*
// MESSAGE SCALED_PRESSURE PACKING

#define MAVLINK_MSG_ID_SCALED_PRESSURE 29

typedef struct __mavlink_scaled_pressure_t
{
 uint32_t time_boot_ms; ///< Timestamp (microseconds since UNIX epoch or microseconds since system boot)
 float press_abs; ///< Absolute pressure (hectopascal)
 float press_diff; ///< Differential pressure 1 (hectopascal)
 int16_t temperature; ///< Temperature measurement (0.01 degrees celsius)
} mavlink_scaled_pressure_t;

#define MAVLINK_MSG_ID_SCALED_PRESSURE_LEN 14
#define MAVLINK_MSG_ID_29_LEN 14

 */
