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
#include "../libUDB/barometer.h"
#include "estAltitude.h"
#include <stdio.h>

//	The origin is recorded as the altitude of the plane during power up of the control.

long barometer_pressure_gnd = 0;
int barometer_temperature_gnd = 0;

long barometer_altitude;	  // above sea level altitude 
long barometer_agl_altitude;  // above ground level altitude 
long barometer_pressure;
int barometer_temperature;
const float ground_altitude = 132.0;  //home ground altitude
//  db mods
long est_barometer_altitude;
long barometer_ground_altitude;
//float altitude;
//float sea_level_pressure;

//   (1)   Extract barometer data from barometer_udb4.c via udb_barometer_callback
inline int get_barometer_temperature(void) { return barometer_temperature; }
inline long get_barometer_pressure(void) { return barometer_pressure; }
inline long get_barometer_altitude(void) { return barometer_altitude; }

//   (2)   Process the data and pass them into ~~gnd variables
void altimeter_calibrate(void)  											//   **** ORIGINALY RAN FROM states.c 
{
	barometer_temperature_gnd = barometer_temperature;
	barometer_pressure_gnd = barometer_pressure;
}

//   (3)   Start data feed into variables from I2C2 and calculate
#if (USE_BAROMETER == 1)            
void udb_barometer_callback(long pressure, int temperature, char status)  	//  ****  RAN FROM libDCM.c to acquire data from barometer_udb4.c
	{
	// const float ground_altitude = 132.0;	// Home altitude
	//	const float p0 = 101325;     // Pressure at sea level (Pa)  -- standard
	//	const float p0 = 101660;     // Pressure at sea level (Pa)  -- currently according to BMCC weather station
	float altitude;
	float sea_level_pressure;

	barometer_temperature = temperature / 10;
	barometer_pressure = pressure / 100;

	sea_level_pressure = ((float)pressure / powf((1 - (ground_altitude/44330.0)), 5.255));

	// 	altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
 	altitude = (float)44330 * (1 - pow(((float) pressure/sea_level_pressure), 0.190295));  // this is just the reverse of the sea_level_pressure algorithm
	}
#endif


//   (4)   Process the data and pass them into ~~gnd variables for debugging/testing
void estAltitude(void)    													// **** RAN FROM gpsParseCommon.c 
{
	barometer_altitude = (float)44330 * (1 - pow(((float) barometer_pressure/barometer_pressure_gnd), 0.190295));
	barometer_agl_altitude = (barometer_altitude - (ground_altitude * 100.0)) ;  //  compute above ground altitude
	return ;
}


//  TEST LOGs: 

//  original sequence and run sources-  (1), (2), (3) and (4):
//  1: vanilla
//  Sonar 31,30 (cm AGL altitude) 
//  Barometer: barometer_temperature_gnd 16409,  barometer_pressure_gnd 26214, barometer_temperature 16749, barometer_pressure 26214, 
//  barometer_ground_altitude 16832, est_barometer_altitude 0, barometer_altitude 17593
//  2:
//  raw excerpt: H44,42:btg16403,bpg13107,bt16750,bp5243,bga16824,eba0,BA17594,Bag0:
//  Sonar: H 40 and 38 (latter, tilt compensated with rmat8)
//  btg is barometer ground temperature - 16409 ; bpg - barometer ground preassure 13107  ;  
//  bt barometer temperature 16750 ;  bp barometer preassure 5243; bga barometer ground altitude 16824 ;  
//  eba estimaged barometer altitude 0  ;  BA barometer altitude  17594  ;  Bag barometer above ground level altitude 
