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
int grd_alt_calibrated = 0 ; 
long barometer_altitude_gnd = 0;  		// meters calibrated ground altitude
long barometer_pressure_gnd = 0;	  	// PA calibrated ground pressure
int barometer_temperature_gnd = 0;  	// celcius calibrated ground temperature
long ground_altitude = 0;

long barometer_pressure;	  		// PA realtime pressure
int barometer_temperature;	  		// celcius realtime temperature
long barometer_altitude_asl;	  	// cm above sea level realtime altitude 
long barometer_altitude_agl;  		// cm above ground level realtime altitude

inline long get_barometer_altitude_asl(void) { return barometer_altitude_asl; }
inline long get_barometer_pressure(void) { return barometer_pressure; }
inline int get_barometer_temperature(void) { return barometer_temperature; }
inline long get_barometer_altitude_agl(void) { return barometer_altitude_agl; }
inline long get_barometer_altitude_gnd(void) { return barometer_altitude_gnd; }

//  triggered once during poweron, from states.c, calibrate barometer initial ground temp, PA and ALT data
//    issue: if the board resets during flight.. can this be stored in a persistent data base??
void altimeter_calibrate(void)  		
{
	barometer_temperature_gnd = barometer_temperature;
	barometer_pressure_gnd = barometer_pressure;
	#if ( SONAR_ALTITUDE == 1 )    								// defined in options.h option to use sonar_altitude  
		barometer_altitude_gnd = (ground_altitude - ((sonar_altitude)/100));
	#else 
		barometer_altitude_gnd = (ground_altitude);
 	#endif
}

#if (USE_BAROMETER == 1)
	void udb_barometer_callback(long pressure, int temperature, char status)	
	{
	float runtime_altitude;
	#if (USE_PA_PRESSURE == 1)	  						//  option to use current sea level PA pressure
		const float p0 = (PA_PRESSURE);    				//    as defined in options.h  
		runtime_altitude = (float)44330 * (1 - pow(((float) pressure/p0), (1/5.255)));
		ground_altitude = (float)44330 * (1 - pow(((float) barometer_pressure_gnd/p0), (1/5.255)));
	#elif (USE_PA_PRESSURE == 2)    					// option to use current sea level use METAR's mercury pressure
		const float p0 = (MC_PRESSURE/0.0295333727112); // convert mercury to sea level PA pressure
		runtime_altitude = (float)44330 * (1 - pow(((float) pressure/p0), (1/5.255)));  // compute actual runtime ASL altitude in meters
		ground_altitude = (float)44330 * (1 - pow(((float) barometer_pressure_gnd/p0), (1/5.255)));
	#else  				// else, use user defined GROUND ALT to compute for SL hPA and use product to compute ASL altitude
		//float sea_lev_pressure;
		ground_altitude = ASL_GROUND_ALT;			//   use home ground alt in m, user defined at options.h 
		const float sea_lev_pressure = ((float)pressure / pow((1 - (ground_altitude/44330.0)), 5.255));
	 	runtime_altitude = (float)44330 * (1 - pow(((float) pressure/sea_lev_pressure), (1/5.255)));  // this is just the reverse of using PA pressure
	#endif
	barometer_pressure = pressure;
	barometer_temperature = (temperature/10);
	barometer_altitude_asl = runtime_altitude ;
	barometer_altitude_agl = (float)44330 * (1 - pow(((float) pressure/barometer_pressure_gnd), (1/5.255)));
	barometer_altitude_gnd = ground_altitude;
	}
#endif

		//runtime_altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.1902949571836346)); or def. rounded off to 0.190295
		//ground_altitude = (float)44330 * (1 - pow(((float) barometer_pressure_gnd/p0), 0.1902949571836346));
