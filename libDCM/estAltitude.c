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
#include "estAltitude.h"
#include <math.h>
#include <stdlib.h>

//#define USE_DEBUG_IO

// The origin is recorded as the altitude of the plane during power up.

long barometer_pressure_gnd = 0;
int barometer_temperature_gnd = 0;

static long barometer_altitude;        // above sea level altitude - ASL (millimeters)
long barometer_pressure;
int16_t barometer_temperature;
float sea_level_pressure;

inline int16_t get_barometer_temperature(void)   { return barometer_temperature; }
inline long get_barometer_pressure(void)     { return barometer_pressure; }
inline long get_barometer_altitude(void)     { return barometer_altitude; }

int16_t barometerInterval = 0;
/**
 * @brief Ascertain a reference ambient barometric pressure & temperature
 */
void altimeter_calibrate(void)
{
	int ground_altitude = alt_origin.WW / 100;    // meters
	barometer_temperature_gnd = barometer_temperature;
	barometer_pressure_gnd = barometer_pressure;

	sea_level_pressure = ((float)barometer_pressure / powf((1 - (ground_altitude/44330.0)), 5.255));

	DPRINT("altimeter_calibrate: ground temp & pres set %i, %li\r\n", barometer_temperature_gnd, barometer_pressure_gnd);
}

#if (USE_BAROMETER_ALTITUDE == 1)
void udb_barometer_callback(long pressure, int16_t temperature, char status)
{
	barometer_temperature = temperature; // units of 0.1 deg C
	barometer_pressure = pressure; // units are Pascals so this could be reduced to an uint16_t
}
#endif

/**
 * @brief Calculate an altitude estimate using barometer data
 * @param
 * @return
 */
void estAltitude(void)
{
#if (USE_BAROMETER_ALTITUDE == 1)
	float pressure_ambient = barometer_pressure;    // Pascals?
	float barometer_alt;

	if (barometer_pressure_gnd != 0)
	{
		
		barometerInterval++;
		if ((barometerInterval % GPS_RATE) == 0)
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
		}
	}
#endif // USE_BAROMETER_ALTITUDE
}

/*  rough-in draft of new algorithm adaption pending verification and revision of barometer data & functions
#if (USE_BAROMETER_ALTITUDE == 1)
	void udb_barometer_callback(long pressure, int temperature, char status)
	{
	#if (USE_PA_PRESSURE == 1)          // **** OPTION TO USE PRESSURE OR HOME POSITION ALTITUDE  in options.h   ****
		const float p0 = PA_PRESSURE;   // **** Current pressure at sea level (Pa) defined in options.h   ****
		altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
		#if (SONAR_ALTITUDE == 1) 
			barometer_ground_altitude = ((float)44330 * (1 - pow(((float) barometer_pressure/p0), 0.190295))-(sonar_altitude/100));
		#else 
			barometer_ground_altitude = (float)44330 * (1 - pow(((float) barometer_pressure/p0), 0.190295));
//		return;
		#endif
	#else
		const float ground_altitude = (ASL_GROUND_ALT/100); // **** defined HOME ASL GROUND ALTITUDE in options.h  ****
//		float altitude;
//		float sea_level_pressure;
		barometer_pressure = pressure / 100;
		barometer_temperature = temperature / 10;
		sea_level_pressure = ((float)pressure / powf((1 - (ground_altitude/44330.0)), 5.255));
		altitude = (float)44330 * (1 - pow(((float) pressure/sea_level_pressure), 0.190295));  // this is just the reverse of the sea_level_pressure algorithm for testing
	#endif
	}
#endif
 */

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

/*
Peter Hollands peter.hollands@gmail.com

8 May (13 days ago)

to uavdevboard
Hi Robert,

I think that over time all the scenarios will be catered for. So it's just a question of which one to do first.

1) Suitable for UDB3 and UDB4: Pilot wants an accurate landing back at the same place as the take off. 
Pilot does not want to take a computer or GCS to the airfiled. i.e. a simple easy flight.  
Solution defaults to option 1 on your list, and uses the barometer primarily for relative altitude. 
Some provision is made to sense check the barometer against the GPS. (another discussion).  
If the pilot has set the ORIGIN in waypoints.h then the barometer will calibrate to the Origin Height at startup.

2.)  Suitable only for UDB4: Pilot is using a GCS and wants proper and sophisticated control of altltitude, 
again primarily for landing purposes. For this scenario, the location of the Origin (including Altitude) 
is settable via MAVLink. This is  easy and quick to program because of  of Matt's work. (and the fact that 
no additonal programming is required in MAVLink GCS's for setting paramaters).  
Also, with Matt's USE_NV_MEMORY option, the altitude can be stored in the EEPROM. This is important for 
the reboot scenario, should it ever happen. The developer will add the origin parameters to the
parameters.xml file. (beautiful art and recommended reading for early adopters in the uav devboard group).

Best wishes, Pete
 */

/*
crashmatt uavflightdirector@gmail.com

8 May (12 days ago)

to uavdevboard
Robert,

mavlink has a specific calibration command for setting barometric offsets.  We need to link this to a function of your choosing.

These storage settings are easy to do.  For each variable you wish to set / store you need the following:

    MatrixPilot variable name
    The storage area to put it in.  Does it belong with some other variables or does should it be in a collection of its own?
    The variable type e.g. int, Q14
    The variable units for mavlink eg. degrees, m/s, meters
    The variable units in mavlink eg. int circular, cm/s, cenitmeters.
    The mavlink parameter name, preferably starting with the storage area keyword.  limited to 15 characters max.
    If the variable should load on startup, reboot, both or neither.
    If the variable is read only

The parameters are defined in ParameterDatabase.xml
http://code.google.com/p/gentlenav/source/browse/trunk/Tools/pyparam/ParameterDatabase.xml#37


Regards Matt
 */
/*
crashmatt uavflightdirector@gmail.com

9 May (12 days ago)

to uavdevboard
Hi all,

Your enthusiasm has got me interested in running the barometer to compare against GPS and IMU performance.  
The first thing to do before attaching the barometer to the IMU is to sanity check the results.  
For that we need to datalog barometer value.  Any values from any mavlink messages can be plotted in QGC, 
either real time or from datalogs recorded by mavlink or QGC.

There is a mavlink message available for reporting the sensor pressures for airspeed, barometer and temperature.
mavlink message raw pressure
We can ask this message to be sent at an xHz rate for datalogging.

I can't see a message that would carry the raw altitude data from the barometer.  These are the related message that I find:
raw gps data - altitude information but for gps
global position - altitude information for the IMU

There is a general debug message that you could put some data in:
debug message - Floating point value plus identifier.

If none of these work for you, we can define a new message for the data you want.  That would go somewhere in here:
matrixpilot mavlink message definitions

the pre-flight calibration of barometer is done through this defined command:
Preflight storage mavlink command

The message is processed and act on here:
mavlink.c preflight storage command processing
This needs modifying to add the barometer calibration.  This is reasonably easy.

Regards Matt
 */
