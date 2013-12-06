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
#include "defines.h"
#include <stdio.h>

//	The origin is recorded as the altitude of the plane during power up of the control.

long barometer_altitude_gnd;
long barometer_pressure_gnd;
int barometer_temperature_gnd;

long barometer_pressure;
int barometer_temperature;
long barometer_asl_altitude;	  // above sea level altitude 
long barometer_agl_altitude;  // mod: above ground level altitude 
long barometer_grd_altitude;  // mod: 
long barometer_agl_est_altitude;

inline long get_barometer_asl_altitude(void) { return barometer_asl_altitude; }
inline long get_barometer_pressure(void) { return barometer_pressure; }
inline int get_barometer_temperature(void) { return barometer_temperature; }
inline long get_barometer_grd_altitude(void) { return barometer_grd_altitude; }
inline long get_barometer_agl_altitude(void) { return barometer_agl_altitude; }

//  triggered from states.c, calibrate barometer initial ground temp, PA and ALT data
void altimeter_calibrate(void)  		
{
	barometer_temperature_gnd = barometer_temperature;
	barometer_pressure_gnd = barometer_pressure;
	#if ( SONAR_ALTITUDE == 1 )    								// defined in options.h option to use sonar_altitude  
		barometer_altitude_gnd = (barometer_grd_altitude - (sonar_altitude));
	#else 
		barometer_altitude_gnd = (barometer_grd_altitude);
 	#endif
}

// triggered from libDCM.c at 40/10hz, get dynamic pressure and temperature and 
//  compute for flight ASL and AGL altitudes
//   TODO: store landing field ground reference during autotakeoff in an array using RT GA
//  :H59,58:t22,p1475,ga24592,ag-27674,as-3082:
#if (USE_BAROMETER == 1)
	void udb_barometer_callback(long pressure, int temperature, char status)	
	{
	float asl_altitude;
	float ground_altitude;
	#if (USE_PA_PRESSURE == 1)	  // 102189	     				// defined in options.h option to use SL hPA current pressure  
		const float p0 = (PA_PRESSURE);    						//  Current pressure at sea level (SL hPA) defined in options.h  
 		#if ( USE_REALTIME_GRDPRES == 1)  
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

/*  DEBUG options deactivated but preserved for posterity
#if (EST_ALT == 1)
	void estAGLAltitude(void)    					// RAN FROM gpsParseCommon.c 
	{
		barometer_agl_est_altitude = (float)44330 * (1 - pow(((float) barometer_pressure/barometer_pressure_gnd), 0.190295));
		return;
	}
#else
	{
		barometer_agl_est_altitude = (float)44330 * (1 - pow(((float) barometer_pressure/barometer_pressure_gnd), 0.190295));
		return;
	}
#endif
*/

