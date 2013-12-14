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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* DEVELOPER'S TECH. NOTES:

   per gpsParseCommon.c funtions:
		alt_sl_gps.WW  	ambient realtime altitude GPS reading  in meters
		alt_origin.WW    	power on calibrated altitude in cm
*/

// These origin barometer_~ is calibrated below during power on or at point of origin.
int32_t barometer_cal_gndpressure;			// calibrated ground pressure
int16_t barometer_cal_gndtemperature;		// calibrated ground temperature 
float barometer_cal_slpressure;    			// calibrated sea level pressure	

// Ambient runtime barometer values:

int32_t barometer_pressure;    							// RT barometer preassure in hPA
int32_t barometer_gndpressure;    						// launch point barometer preassure in hPA
int32_t barometer_rawaltitude;    						// RT inflight, from above sea level (ASL) altitude in meters
int32_t barometer_aslaltitude;        					// RT inflight, from above sea level (ASL) altitude in centimeters
int32_t barometer_aglaltitude;    						// RT from above ground level (AGL) altitude in centimeters
int32_t barometer_gndaltitude;    						// RT terrain ground altitude estimate in centimeters >>> theoretical <<<<
int16_t barometer_temperature;    						// RT barometer temperature in degrees celcius
int16_t barometer_gndtemperature; 						// launch point barometer temperature in degrees celcius

inline int32_t get_barometer_pressure(void)  { return barometer_pressure; }  // RT inflight barometer preassure in hPA
inline int32_t get_barometer_gnd_pressure(void) { return barometer_gndpressure; }  // launch point barometer ground level preassure in hPA
inline int32_t get_barometer_raw_altitude(void) { return barometer_rawaltitude; }  // RT inflight, from above sea level (ASL) altitude in m
inline int32_t get_barometer_asl_altitude(void) { return barometer_aslaltitude; }  // RT inflight, from above sea level (ASL) altitude 
inline int32_t get_barometer_agl_altitude(void) { return barometer_aglaltitude; }  // RT above ground level (AGL) altitude in centimeters
inline int32_t get_barometer_gnd_altitude(void) { return barometer_gndaltitude; }  // RT inlfight terrain ground level altitude estimate in centimeters
inline int16_t get_barometer_temperature(void) { return barometer_temperature; }   // RT barometer inflight temperature realtime in degrees celcius
inline int32_t get_barometer_gnd_temperature(void) { return barometer_gndtemperature; }  // launch point barometer temperature 

//  altimeter_calibrate() function calibrates and stores baromerter_~~ values
//    calibration triggered from navigate.c, when USE_BAROMETER == 1
void altimeter_calibrate(void)						    
{
	const int32_t gps_orig_gndaltitude = (alt_origin.WW / 100);    	// GPS estimated ground altitude reading in meters at power-on or point of origin in meters
	barometer_cal_gndtemperature = barometer_temperature;
	barometer_cal_gndpressure = barometer_pressure;
	barometer_cal_slpressure = ((float)barometer_pressure / powf((1 - (gps_orig_gndaltitude/44330.0)), 5.255));  // calibrate sea_level_pressure
}

//  runtime barometer feeds
void udb_barometer_callback(int32_t pressure, int16_t temperature, char status)
{
	barometer_temperature = temperature;
	barometer_pressure = pressure;
}
	
void estAltitude(void)
{
	
	if (barometer_pressure != 0)
	{
		#if (USE_PA_PRESSURE == 1)	  			     				// defined in options.h option to use SL hPA current pressure  
        	const float p0 = (PA_PRESSURE);    						// Current pressure at sea level (SL hPA) defined in options.h  
		#elif (USE_PA_PRESSURE == 2)    							// use mercury value
			const float p0 = (MC_PRESSURE/0.0295333727112); 		// Current pressure at sea level (SL METAR inch mercury) defined in options.h  
        #endif

		const int32_t barometer_ambientpressure = barometer_pressure;

        // ASL raw altitude in Meters
		barometer_rawaltitude = 44330.0f * ((1-pow((barometer_ambientpressure/barometer_cal_slpressure),(1/5.255f)))); 

        // ASL raw altitude in centimeters; millimeters, use (~~)x1000
		barometer_aslaltitude = (int32_t)(barometer_rawaltitude*100); 

        //  AGL altitude in centimeters; millimeters, use (~~)x1000
  		barometer_aglaltitude = (int32_t)(44330.0f*((1-pow((((float)barometer_ambientpressure)/((float)barometer_cal_gndpressure)),(1/5.255f)))))*100; 

        //  inflight >>> THEORETICAL <<< ground-terrain ASL altitude in centimeters; millimeters, use (~~)x1000
		barometer_gndaltitude = ((float)44330 * (1 - pow(((float) barometer_ambientpressure/p0), 0.190295)))*100;

		//  launch point  ground temperature
		barometer_gndtemperature = barometer_cal_gndtemperature;

		//  launch point  ground hPA pressure 
		barometer_gndpressure = barometer_cal_gndpressure;
	}
	else
	{
		// something to filter an error read or glitch
		barometer_rawaltitude = -99;
		barometer_aslaltitude = -99;
		barometer_aglaltitude = -99;
		barometer_gndaltitude = -99;
	}
}


/*  EXPERIMENTAL ROUGH-IN DBERROYA 12/19/2013

#if (USE_BAROMETER == 1)
	void udb_barometer_callback(long pressure, int temperature, char status)	
	{
	float asl_altitude;
	float ground_altitude;
	#if (USE_PA_PRESSURE == 1)	  // 102189	     				// defined in options.h option to use SL hPA current pressure  
		const float p0 = (PA_PRESSURE);    						//  Current pressure at sea level (SL hPA) defined in options.h  
 		#if ( USE_REALTIME_GRDPRES == 1)     					//  Option enabled in options.h  
			ground_altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
		#else
			 // ga24592
			ground_altitude = (float)44330 * (1 - pow(((float) barometer_pressure_gnd/p0), 0.190295));
		#endif
		//  as-3082:
		asl_altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
	#elif (USE_PA_PRESSURE == 2)    // use mercury value
		const float p0 = (MC_PRESSURE/0.0295333727112);    	// Current pressure at sea level (SL METAR in. mercury) defined in options.h  
 		#if ( USE_REALTIME_GRDPRES == 1)  // TODO: store in an array landing field ground reference during auto take off using RT GA
			ground_altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
		#else
			 // ga24592
			ground_altitude = (float)44330 * (1 - pow(((float) barometer_pressure_gnd/p0), 0.190295));
		#endif
		//  as-3082:
		asl_altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
	#else  							// else, use user defined GROUND ALT to compute for SL hPA and use product to compute ASL altitude
		float seal_pressure;
		ground_altitude = (ASL_GROUND_ALT);			//   use home GROUND ALTITUDE, USER DEFINED in options.h 
		seal_pressure = ((float)pressure / pow((1 - (ground_altitude/44330.0)), 5.255));
	 	asl_altitude = (float)44330 * (1 - pow(((float) pressure/seal_pressure), 0.190295));  // this is just the reverse of using PA pressure
	#endif
	barometer_pressure = (pressure/100);
	barometer_temperature = (temperature/10);
	barometer_asl_altitude = (asl_altitude) ;
	#if ( USE_REALTIME_GRDPRES == 1)
		barometer_grd_altitude = ground_altitude ;
		barometer_agl_altitude = (asl_altitude - ground_altitude) ;
	#else
		barometer_grd_altitude = barometer_pressure_gnd ; // ERROR
		barometer_agl_altitude = (asl_altitude - barometer_grd_altitude) ;
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


