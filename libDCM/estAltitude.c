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
#include "../libUDB/barometer.h"
#include "../libUDB/lidar.h"
#include "estAltitude.h"
#include <math.h>
#include <stdlib.h>

//#define USE_DEBUG_IO

// The origin is recorded as the altitude of the plane during power up.

long barometer_pressure_gnd = 0;
long barometer_altitude_gnd = 0;
long barometer_altitude_gnd0 = 0;
int32_t barometer_agl_altitude;
int barometer_temperature_gnd = 0;

static long barometer_altitude;        // above sea level altitude - ASL (millimeters)
long barometer_pressure;
int16_t barometer_temperature;
float sea_level_pressure;
static int32_t lidar_altitude;        // lidar above ground height (millimeters)
int32_t estimated_altitude;        //  (millimeters)

inline int16_t get_barometer_temperature(void)   { return barometer_temperature; }
inline long get_barometer_pressure(void)     { return barometer_pressure; }
inline long get_barometer_altitude(void)     { return barometer_altitude; }

int16_t barometerInterval = 0;
/**
 * @brief Ascertain a reference ambient barometric pressure & temperature
 */
void altimeter_calibrate(void)
{
#if (USE_BAROMETER_ALTITUDE == 1)
	int ground_altitude = alt_origin.WW / 100;    // meters
	barometer_temperature_gnd = barometer_temperature;
	barometer_pressure_gnd = barometer_pressure;

	sea_level_pressure = ((float)barometer_pressure / powf((1 - (ground_altitude/44330.0)), 5.255));

	DPRINT("altimeter_calibrate: ground temp & pres set %i, %li\r\n", barometer_temperature_gnd, barometer_pressure_gnd);
#elif (USE_BAROMETER_ALTITUDE == 2)
	if (barometer_altitude_gnd0 == 0) barometer_altitude_gnd0 = barometer_altitude;
        else barometer_altitude_gnd0 = (15*barometer_altitude_gnd0 + barometer_altitude)/16;
        barometer_altitude_gnd = barometer_altitude_gnd0;
#endif
#if (USE_LIDAR_ALTITUDE >0)
	if (lidar0 == 0) lidar0 = lidar_altitude;
        else lidar0 = (15*lidar0 + lidar_altitude)/16;
#endif  //LIDAR_ALTITUDE > 0
}

#if (USE_BAROMETER_ALTITUDE == 1)
void udb_barometer_callback(long pressure, int16_t temperature, char status)
{
	barometer_temperature = temperature; // units of 0.1 deg C
	barometer_pressure = pressure; // units are Pascals so this could be reduced to an uint16_t
}
#elif (USE_BAROMETER_ALTITUDE == 2)
void udb_barometer_callback(long altitude, int16_t temperature, char status)
{
	barometer_temperature = temperature; // units of 0.1 deg C
	barometer_altitude = altitude;//Altitude in mm
}
#endif

/**
 * @brief Calculate an altitude estimate using barometer data
 * @param
 * @return
 */
void udb_lidar_callback(long altitude)
{
	lidar_altitude = altitude;//Altitude in mm
}

//Estimate the best altitude from all four sensors IMU, Baro, Lidar or sonanr and GPS

void estAltitude(void)
{
#if (USE_BAROMETER_ALTITUDE == 1)
	float pressure_ambient = barometer_pressure;    // Pascals?
	float barometer_alt;

	if (barometer_pressure_gnd != 0)
	{		
		barometer_alt = 44330.0f * ((1-pow((pressure_ambient/sea_level_pressure),(1/5.255f)))); // Meters
		barometer_altitude = (long)(barometer_alt * 1000); // millimeters
#ifdef USE_DEBUG_IO
		// estimate sea level pressure assuming we're still on the ground
		int ground_altitude = alt_origin.WW / 100; // meters
		sea_level_pressure = ((float) barometer_pressure / powf((1 - (ground_altitude / 44330.0)), 5.255));
		// print pressure altitude, pressure and current SLP estimate
		printf("estAltitude %f, pressure %f, sea level pressure %f\r\n", (double) barometer_alt, (double) (.01 * pressure_ambient), (double) (.01 * sea_level_pressure));
#endif
#elif (USE_BAROMETER_ALTITUDE == 2)
        barometer_agl_altitude = barometer_altitude - barometer_altitude_gnd;
#ifdef USE_DEBUG_IO
		// estimate sea level pressure assuming we're still on the ground
		// print altitude
		printf("estAltitude %f\r\n", (double) barometer_agl_altitude);
#endif
#endif // USE_BAROMETER_ALTITUDE
#if (USE_BAROMETER_ALTITUDE > 0)
        estimated_altitude = lidar_altitude;        // barometer altitude (millimeters)
#endif
#if (USE_LIDAR_ALTITUDE >0)
        if (lidar_altitude <10000) estimated_altitude = lidar_altitude;        // lidar altitude (millimeters)

#endif  //LIDAR_ALTITUDE > 0
#if (USE_SONAR_INPUT != 0)
	calculate_sonar_height_above_ground();
    if (sonar_height_to_ground <10000) estimated_altitude = sonar_height_to_ground;        // lidar altitude (millimeters)
#endif
}
