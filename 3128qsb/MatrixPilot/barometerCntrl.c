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


#include "../libDCM/libDCM.h"
#include "../libUDB/barometer.h"
#include "barometerCntrl.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*  REF. STD. ALTITUDE FORUMULAS:
const float p0 = 101325;     //  Pressure at sea level (Pa)
float altitude;

// after calculation and retreival of Pa pressure 
  altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));
  Serial.print("Altitude: ");
  Serial.print(altitude, 2);
  Serial.println(" m");

// CURRENT 
const float currentAltitude = 1580.08; // current altitude in METERS
const float ePressure = p0 * pow((1-currentAltitude/44330), 5.255);  // expected pressure (in Pa) at altitude
float weatherDiff;

*/

//	The origin is recorded as the altitude of the plane during power up of the control.

//1st CALIBRATION DATA
long bar_gndpressure_onecal = 0;  		// 1st cal home ground pressure
int bar_gndtemperature_onecal = 0;  	// 1st cal home ground temperature
long bar_sealvpressure_onecal = 0; 		// 1st cal sea level presure

//2nd CALIBRATION DATA
long bar_gndpressure_twocal = 0;  		// 2nd cal home ground pressure
int bar_gndtemperature_twocal = 0;  	// 2nd cal home ground temperature
long bar_sealvpressure_twocal = 0; 		// 2nd cal sea level presure

//VALIDATED HOME GROUND DATA
static long bar_gndpressure_valcal;  	// validated cal home ground pressure
static int bar_gndtemperature_valcal;  	// validated cal home ground temperature
static long bar_sealvpressure_valcal; 	// validated sea level pressure during calibration
static long gps_gndaltitude_valcal;  	// launch point GPS est. ground altitude;
static long bar_aslaltitude_valcal;		// launch point above sea level altitude;
static long bar_gndaltitude_valcal;		// launch point ground altitude;

//RUN TIME BAROMETER DATA
long barometer_pressure = 0;			// RT pressure
int barometer_temperature = 0;				// RT temperature
long barometer_aslaltitude = 0;	  			// RT long barometer_altitude;  above sea level altitude - ASL (millimeters)
long barometer_aglaltitude = 0;  			// RT long barometer_agl_altitude;  above ground level altitude - AGL
long barometer_gndaltitude = 0;  			// RT ground altitude

inline long get_barometer_aslaltitude(void) { return barometer_aslaltitude; }
inline long get_barometer_pressure(void) { return barometer_pressure; }
inline int get_barometer_temperature(void) { return barometer_temperature; }
inline long get_barometer_gndaltitude(void) { return barometer_gndaltitude; }
inline long get_barometer_aglaltitude(void) { return barometer_aglaltitude; }


// Following BarometerCalOne ~ Two TESTING trigger location of barometer (altimeter) calibration, 
//  with 0 (states.c) trigger is first at (barometerCntrl.c), 2nd (~CalTwo) in libDCM.c
//  0- states.c (def); 1- navigate.c; as set by #define ALTCAL_TRIG in options.h

// HOME GROUND/POWER ON CALIBRATIONS
//  Trigger options for running calibration functions: BarometerCalOne ~ Two, depends on def. of
//  ALTCAL_TRIG	(options.h): 0 triggers BarometerCalOne 1st in states.c then ~CalTwo in libDCM.c
//    and setting to 1, triggers both ~CalOne ~ Two in navigate.c:

void barometerCalOne(void)
{
	const int32_t gps_gndaltitudeone = alt_origin.WW / 100;		// in meters, derived from libDCM 
	bar_gndtemperature_onecal = barometer_temperature;			// Ground temperature in celsious 
	bar_gndpressure_onecal = barometer_pressure;				// PA ground pressure 
	bar_sealvpressure_onecal = ((float)barometer_pressure / powf((1 - (gps_gndaltitudeone/44330.0f)), 5.255f));  // PA sea level pressure 
}

void barometerCalTwo(void)
{
	const int32_t gps_gndaltitudetwo = alt_origin.WW / 100;		// in meters, derived from libDCM 
	bar_gndtemperature_twocal = barometer_temperature;			// Ground temperature in celsious
	bar_gndpressure_twocal = barometer_pressure;				// ground pressure PA 
	bar_sealvpressure_twocal = ((float)barometer_pressure / powf((1 - (gps_gndaltitudetwo/44330.0f)), 5.255f));  // PA sea level pressure 
	barometerCalVal();
}

void barometerCalVal(void) 	// VALIDATE/STORE HOME GROUND BAROMETRIC DATA FROM CALIBRATION for later use
{
	float rtime_pressure = 0;
	rtime_pressure = barometer_pressure;    // PA
	const int32_t gps_gndaltitude = alt_origin.WW; // / 100;		// in centimeters, derived from libDCM 
	if (bar_gndpressure_onecal != 0)
	{
		bar_gndpressure_valcal = bar_gndpressure_onecal;
		bar_gndtemperature_valcal = bar_gndtemperature_onecal;
		bar_sealvpressure_valcal = bar_sealvpressure_onecal;
		bar_aslaltitude_valcal = ((float)44330.0f * (1 - pow(((float) rtime_pressure/bar_sealvpressure_onecal),((float)1/5.255f))))*100;

	}
	else if  (bar_gndpressure_onecal == 0 && bar_gndpressure_twocal != 0)
	{
		bar_gndpressure_valcal = bar_gndpressure_twocal;
		bar_gndtemperature_valcal = bar_gndtemperature_twocal;
		bar_sealvpressure_valcal = bar_sealvpressure_twocal;
		bar_aslaltitude_valcal = ((float)44330.0f * (1 - pow(((float) rtime_pressure/bar_sealvpressure_twocal),((float)1/5.255f))))*100;
	}
	else 
	{	
		if (rtime_pressure != 0){bar_gndpressure_valcal = rtime_pressure;} else {bar_gndpressure_valcal = 0;}
		if (barometer_temperature >0){bar_gndtemperature_valcal = barometer_temperature;} else {bar_gndtemperature_valcal = 0;}
		if (USE_PA_PRESSURE == 1 &&  PA_PRESSURE !=0){bar_sealvpressure_valcal = PA_PRESSURE;} else {bar_sealvpressure_valcal = 0;}
		if (rtime_pressure != 0){bar_aslaltitude_valcal = ((float)44330.0f * (1 - pow(((float) rtime_pressure/PA_PRESSURE),((float)1/5.255f))))*100;} else
			{bar_aslaltitude_valcal = 0;}
	}
	gps_gndaltitude_valcal = gps_gndaltitude;
	bar_gndaltitude_valcal = (gps_gndaltitude + bar_aslaltitude_valcal)/2;

	// TODO:  create and call filtering and sampling algorithms/functions here
}



#if (USE_BAROMETER == 1  && HILSIM != 1)	
	void udb_barometer_callback(long pressure, int temperature, char status) // RAW DATA FROM BAROMETER SENSOR
	{
		barometer_temperature = temperature;
		barometer_pressure = pressure;
		// TESTING runtime trigger location: 0- here, barometerCntrl.c (def); 1- gpsParseCommon.c; 2- libDCM.c (at 4HZ) set in options.h 
		#if ( ALTRUN_TRIG == 0) 
			estBarometerAltitude();
		#endif
	}
#endif

void estBarometerAltitude(void)  	// validate and store HOME GROUND barometric estimates for later use
{
#if (USE_BAROMETER == 1  && HILSIM != 1)
	#ifndef EST_BAROMETER_ALLDATA
		float rtime_pressure = 0;
		rtime_pressure =barometer_pressure;    // PA
	
		if (bar_gndpressure_onecal != 0)
		{
			barometer_aslaltitude = ((float)44330.0f * (1 - pow(((float) rtime_pressure/bar_sealvpressure_onecal),((float)1/5.255f))))*100;
		}
		else if  (bar_gndpressure_onecal == 0 && bar_gndpressure_twocal != 0)
		{
			barometer_aslaltitude = ((float)44330.0f * (1 - pow(((float) rtime_pressure/bar_sealvpressure_twocal),((float)1/5.255f))))*100;
		}
		else 
		{
			barometer_aslaltitude = -99 ;  // error or no read
		}
		barometer_gndaltitude = alt_origin.WW ;  // ground ASL altitude
		barometer_aglaltitude = barometer_aslaltitude - alt_origin.WW ;  //AGL altitude in cm

	#else
		float asl_altitude=0;
		float ground_altitude=0;
		// unecessary but just for observance of a common nomenclature pattern
		float rtime_pressure = 0;
		rtime_pressure =barometer_pressure;    // PA
		float rtime_temperature = 0;
		rtime_temperature =barometer_temperature;    // PA

		#if (USE_PA_PRESSURE == 1)	  // 102189	     				// defined in options.h option to use SL hPa current pressure  
			const float p0 = (PA_PRESSURE);    						//  Current pressure at sea level (SL hPa) defined in options.h  
	 		#if ( USE_REALTIME_GRDPRES == 1)  
				ground_altitude = (float)44330 * (1 - pow(((float) rtime_pressure/p0), 0.190295));
			#else
				 // ga24592
				ground_altitude = (float)44330 * (1 - pow(((float) bar_gndpressure_valcal/p0), 0.190295));
			#endif
			asl_altitude = (float)44330 * (1 - pow(((float) rtime_pressure/p0), 0.190295));
		#elif (USE_PA_PRESSURE == 2)    // use mercury value
			const float p0 = (MC_PRESSURE/0.0295333727112);    	// Current pressure at sea level (SL METAR in. mercury) defined in options.h  
	 		#if ( USE_REALTIME_GRDPRES == 1)  					// TODO: store in an array landing field ground reference during auto take off using RT GA
				ground_altitude = (float)44330 * (1 - pow(((float) rtime_pressure/p0), 0.190295));
			#else
				 // ga24592
				ground_altitude = (float)44330 * (1 - pow(((float) bar_gndpressure_valcal/p0), 0.190295));
			#endif
			//  as-3082:
			asl_altitude = (float)44330 * (1 - pow(((float) rtime_pressure/p0), 0.190295));
		#else  							// else, use user defined GROUND ALT to compute for SL hPA and use product to compute ASL altitude
			float seal_pressure;
			ground_altitude = (ASL_GROUND_ALT);			//   use home GROUND ALTITUDE, USER DEFINED in options.h 
			seal_pressure = ((float)rtime_pressure / pow((1 - (ground_altitude/44330.0)), 5.255));
		 	asl_altitude = (float)44330 * (1 - pow(((float) rtime_pressure/seal_pressure), 0.190295));  // this is just the reverse of using PA pressure
		#endif
		barometer_pressure = (rtime_pressure);
		barometer_temperature = (rtime_temperature/10);
		barometer_aslaltitude = (asl_altitude) ;
		#if ( USE_REALTIME_GRDPRES == 1)
			barometer_gndaltitude = ground_altitude;
			barometer_aglaltitude = (asl_altitude - ground_altitude);
		#else
			barometer_gndaltitude =  bar_gndaltitude_valcal; 
			barometer_aglaltitude = (asl_altitude - barometer_gndaltitude);
		#endif
	#endif 
#endif 
}

///	 GOOFY'S AREA:


		//float barometer_alt;
		/*
		if (bar_gndpressure_onecal != 0)
		{
			barometer_aslaltitude = ((float)44330.0f * (1 - pow(((float) rtime_pressure/bar_sealvpressure_onecal),((float)1/5.255f))))*100;
		}
		else if  (bar_gndpressure_onecal == 0 && bar_gndpressure_twocal != 0)
		{
			barometer_aslaltitude = ((float)44330.0f * (1 - pow(((float) rtime_pressure/bar_sealvpressure_twocal),((float)1/5.255f))))*100;
		}
		else 
		{
			barometer_aslaltitude = -99 ;  // error or no read
		}
		*/

		//barometer_aslaltitude = ((44330.0f * (1-pow(rtime_pressure/bar_sealvpressure_onecal), (1/5.255f)))))*100); // ASL altitude in cm
		//barometer_aslaltitude = //((44330.0f * (1-pow(rtime_pressure/bar_sealvpressure_onecal)(1/5.255f))))*100; // ASL altitude in cm
/*
#if (USE_BAROMETER == 1 && HILSIM != 1)
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
 		#if ( USE_REALTIME_GRDPRES == 1)  					// TODO: store in an array landing field ground reference during auto take off using RT GA
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
	barometer_aslaltitude = (asl_altitude) ;
	#if ( USE_REALTIME_GRDPRES == 1)
		barometer_gndaltitude = ground_altitude;
		barometer_aglaltitude = (asl_altitude - ground_altitude);
	#else
		barometer_gndaltitude =  ground_altitude; // SUSPECT 
		barometer_aglaltitude = (asl_altitude - barometer_gndaltitude);
	#endif
	}
#endif
*/

// triggered from libDCM.c at 40/10hz, get dynamic pressure and temperature and 
//  compute for flight ASL and AGL altitudes
//   TODO: store landing field ground reference during autotakeoff in an array using RT GA
//  :H59,58:t22,p1475,ga24592,ag-27674,as-3082:
//		barometer_altitude = (long)(44330.0f*((1-pow((((float)barometer_pressure)/((float)barometer_pressure_gnd)),(1/5.255f)))))*1000; // millimeters
//  rough-in draft of new algorithm adaption pending verification and revision of barometer data & functions
/*
#ifdef USE_DEBUG_IO
	printf("BarometerCalOne: ground temp & pres set %i, %li\r\n", bar_gndtemperature_1cal, bar_gndpressure_1cal);
#endif
#ifdef USE_DEBUG_IO
	printf("BarometerCalTwo: ground temp & pres set %i, %li\r\n", bar_gndtemperature_2cal, bar_gndpressure_2cal);
#endif
#if (BAROMETER_ALTITUDE == 1)
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


/*
		//barometer_alt = 44330.0f * ((1-pow((rtime_pressure/bar_sealvpressure_1cal),(1/5.255f)))); // Meters
		//barometer_aslaltitude = (long)(barometer_alt * 1000); // millimeters
		barometer_aslaltitude  = (long)(44330.0f*((1-pow((((float)barometer_pressure)/((float)bar_gndpressure_1cal)),(1/5.255f)))))*1000; // millimeters

#ifdef USE_DEBUG_IO
		// estimate sea level pressure assuming we're still on the ground
		int ground_altitude = alt_origin.WW / 100; // meters
		sea_level_pressure = ((float) barometer_pressure / powf((1 - (ground_altitude / 44330.0)), 5.255));
		// print pressure altitude, pressure and current SLP estimate
		printf("estAltitude %f, pressure %f, sea level pressure %f\r\n", (double) barometer_alt, (double) (.01 * pressure_ambient), (double) (.01 * sea_level_pressure));
#endif
*/
//  triggered from states.c, calibrate barometer initial ground temp, PA and ALT data
//void calBarometer(void)  		
//{
//	barometer_temperature_gnd = barometer_temperature;
//	barometer_pressure_gnd = barometer_pressure;
//    /*if (barometer_pressure_gnd > 0)
//		barometer_calflag = 1;
//	else 
//		barometer_calflag = 0;
//	*/
//}


/*
#if (USE_BAROMETER == 1 && HILSIM != 1)
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
	barometer_aslaltitude = (asl_altitude) ;
	#if ( USE_REALTIME_GRDPRES == 1)
		barometer_gndaltitude = ground_altitude;
		barometer_aglaltitude = (asl_altitude - ground_altitude);
	#else
		barometer_gndaltitude =  ground_altitude; // SUSPECT 
		barometer_aglaltitude = (asl_altitude - barometer_gndaltitude);
	#endif
	}
#endif

*/

