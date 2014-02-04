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
#include "defines.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


//  altitude offset default
int32_t alt_offset=BAR_ALT_OFFSET;				// auto calibrated altitude offset: WIP

//	The origin is recorded as the altitude of the plane during power up of the control board.
//  ..Stored prior sampled and averaged runtime barometric data

//RUN-TIME-INLINE BAROMETER DATA
static float barometer_pressure				= 0;	// RT raw pressure
static int16_t barometer_temperature		= 0;	// RT raw temperature
static float barometer_pressure_origin		= 0;	// Origin pressure
static int16_t barometer_temperature_origin	= 0;	// Origin pressure
static int32_t barometer_altitude_origin	= 0;	// Origin altitude
static float  barometer_rtavepres			= 0;	// Runtime averaged-sampled pressure
static int16_t barometer_rtavetemp			= 0;	// Runtime averaged-sampled temperature
static int32_t barometer_aslaltitude		= 0;	// Runtime averaged-sampled above sea level altitude - ASL (millimeters)
//static int32_t barometer_aslaltitude_pvr	= 0;	// Runtime previous value, above sea level altitude - ASL (millimeters)

// calibration loop counter and constant data
static const int16_t room_temp = 15;
static int8_t d		= 0;
static int16_t dd	= 0;
static int8_t i		= 0;
static int8_t ii	= 0;
static int8_t ui	= 0;
static int8_t ei	= 0;
static int8_t ww	= 0;

//INLINES
inline float 	get_barometer_pressure(void)		{return barometer_pressure;}
inline int16_t 	get_barometer_temperature(void)		{return barometer_temperature;}
inline float 	get_barometer_pressureorgn(void)	{return barometer_pressure_origin;}
inline int16_t 	get_barometer_temperatureorgn(void)	{return barometer_temperature_origin;}
inline int32_t 	get_barometer_altitudeorgn(void)	{return barometer_altitude_origin;}
inline float 	get_barometer_rtavepressure(void)	{return barometer_rtavepres;}
inline int16_t 	get_barometer_rtavetemperature(void){return barometer_rtavetemp;}
inline int32_t 	get_barometer_aslaltitude(void)		{return barometer_aslaltitude;}

// Retreive barometer data, run calibration and estimation functions, populate inlines
#if (USE_BAROMETER==1&&HILSIM!=1)
	void udb_barometer_callback(int32_t pressure, int16_t temperature, int8_t status) 
	{
		barometer_pressure = (float)pressure;
		//  in units of 0.1 deg. C
		barometer_temperature = (int16_t)temperature;  
		//  runtime trigger location (defined in options.h ): 0- here, barometerCntrl.c (def); 
		//	1- gpsParseCommon.c, 2- libDCM.c
		#if (ALTRUN_TRIG == 0)
			if (flags._.barometer_calibrated == 1)
			{
				estBarometerAltitude();	
			}
		#endif
		#if (ALTCAL_TRIG == 0)
			if (flags._.barometer_calibrated!=1)
			{
				barometerCalibrate();	
			}
		#endif
	}
#endif


// POWER-ON CALIBRATIONS
//  Trigger location options for running calibration functions: 
//  ALTCAL_TRIG	(options.h): options 1- states.c  2- libDCM.c  both at 4Hz default with 
//  BAR_HZ_CYCLE at 10

void barometerCalibrate(void)   		
{
    // Let the barometer settle for a minimum of 3-4 seconds after power on. Depending on ambient 
	// temperature, it reads quite a long way off w/in the first 2-3 seconds, leading to about 
	// >2 M and 100 C of error if there's no wait.  Depending on cycle speed setting defined in 
	// options.h, auto adjust delay: 10 for 4Hz (def), 8 for 5Hz, 5 for 8Hz, 4 for 10Hz and 2 for 
	// Finaly, auto adjust delay timer to ensure 2 second delay for warm and about 4 seconds 
	//  20hz, for cold or lower than typical room temperature of 15 C:
	if(flags._.d_init != 1)
	{
		if(d < CAL_WU_TD && d++ < CAL_WU_TD){}  // initial warmup time for improved temperature reading accuracy
		else if(d >= CAL_WU_TD) 
		{
			if (barometer_temperature/10 >= room_temp){ww = 1;}else{ww = 2;} 
			if (CAL_HZ_CYCLE >= 20){dd = 3*ww;} 
			else if (CAL_HZ_CYCLE >= 10){dd = 4*ww;} 
			else if (CAL_HZ_CYCLE >= 8){dd = 5*ww;} 
			else if (CAL_HZ_CYCLE >= 5){dd = 8*ww;}
			else if (CAL_HZ_CYCLE >= 4){dd = 10*ww;} 
			else if (CAL_HZ_CYCLE >= 2){dd = 20*ww;}
			else if (CAL_HZ_CYCLE < 2){dd = 40*ww;}
			flags._.d_init=1;
		} 
	}

	//  Then, let's get the initial barometer sensor feed, validate and store these values for final calibration
	if((flags._.d_init==1)&&(flags._.i_init!=1))
	{
	    if(i < dd && i++ < dd){} // warm-up time delay loop
		else if(i >= dd)
		{
			flags._.i_init=1;
		}
	} 	

    // Use BAR_CALSAMPLING value for the final calibration of point of origin pressure (Pa) and temperature(0.1 deg. C)
	if((flags._.d_init==1)&&(flags._.i_init==1))
	{
	    if(ii<BAR_CALSAMPLING&&ii++<BAR_CALSAMPLING)
		{
	        if((udb_flags._.healthy==true)&&(barometer_pressure>0)&&(barometer_pressure>=(int32_t)(PA_PRESSURE*BAR_PAPCT_THRESH))
				&&(barometer_temperature>0)&&(barometer_temperature<BAR_TEMP_THRESH))
			{
				if (flags._.o_initrun != 1)
				{ 
					barometer_pressure_origin = (float)barometer_pressure;
					barometer_temperature_origin = (int16_t)barometer_temperature;
					flags._.o_initrun = 1;
				}
				barometer_pressure_origin = (float)(barometer_pressure_origin*0.8f + barometer_pressure*0.2f);
				barometer_temperature_origin = (int16_t)(barometer_temperature_origin*0.8f + barometer_temperature*0.2f);
			} 
			else 
			{
				if(ii>0){ii--;}
			}
	    }
		else if(ii>=BAR_CALSAMPLING)
		{
			// TODO: see if moving this to states.c can improve bar origin data accuracy 
			barometer_altitude_origin = setBarOriginAlt(barometer_pressure,barometer_temperature); 
 
			if (barometer_altitude_origin <0)		// re-run 3rd stage calibration if asl alt value is not positive 
			{
				flags._.barometer_calibrated=0; 
				flags._.o_initrun = 0;
				initOriginVars();
			} 
			else
			{  
				flags._.barometer_calibrated=1; 
			}
		}
	}
}		//  barometerCalibrate()

//  Triggered from states.c, UPDATE calibration upon GPS lock for improving accuracy of ground barometric data
//  TODO: update only if the difference is significant (> 500 Pa or newtons/sq. M) and  
//  everything is go-green

void barometerCalibrationUpdate(void) 						
{
	if(ui<BAR_CALSAMPLING&&ui++<BAR_CALSAMPLING)
	{
	    if((udb_flags._.healthy==true)&&(barometer_pressure>0)&&((float)barometer_pressure>=(float)(PA_PRESSURE*BAR_PAPCT_THRESH))
		&&(barometer_temperature>0)&&(barometer_temperature<BAR_TEMP_THRESH))
		{
			barometer_pressure_origin = (float)(barometer_pressure_origin*0.8f + barometer_pressure*0.2f);
			barometer_temperature_origin = (int16_t)(barometer_temperature_origin*0.8f + barometer_temperature*0.2f);
		} 
		else 
		{
			if(ui>0){ui--;}  // reject any filtered invalid data
			// TODO: add an error management or health recovery algo here later
		}
	}
	else if(ui>=BAR_CALSAMPLING)
	{
		// TODO: see if moving this somewhere in states.c can improve bar origin data accuracy 
		barometer_altitude_origin = setBarOriginAlt(barometer_pressure,barometer_temperature);

			if (barometer_altitude_origin <0)		// re-run calibration update if alt value is not positive
			{
				flags._.barometer_calib_updated = 0; 
			} 
			else
			{  
				flags._.barometer_calib_updated = 1; 
			}
	}
}

//  estimate runtime barometric pressure, temperature and altitude
void estBarometerAltitude(void)  	
{
#if (USE_BAROMETER==1&&HILSIM!=1)
    if(ei<BAR_RTSAMPLING&&ei++<BAR_RTSAMPLING)
	{

	    if((udb_flags._.healthy==true)&&(barometer_pressure>0)&&((float)barometer_pressure >= (float)(PA_PRESSURE*BAR_PAPCT_THRESH))
			&&(barometer_temperature>0)&&(barometer_temperature<BAR_TEMP_THRESH))
		{
			if (flags._.e_initrun != 1)  // after calibration, use the initial values from calibration for the 1st cycle
			{
				barometer_rtavepres=(float)barometer_pressure_origin;
				barometer_rtavetemp=(int16_t)barometer_temperature_origin;
				flags._.e_initrun=1;
			}
			barometer_rtavepres=(float)(barometer_rtavepres * 0.8f + barometer_pressure * 0.2f);
			barometer_rtavetemp=(int16_t)(barometer_rtavetemp * 0.8f + barometer_temperature * 0.2f);
		} 
		else 
		{
			if(ei>0){ei--;}  // reject any filtered-out invalid data
			// TODO: add an error management or health recovery algo here later
		}
	}
	else if(ei>=BAR_RTSAMPLING)
	{
		//  calculate ASL altitude
		barometer_aslaltitude = (int32_t)(calcBarASLAlt((float)barometer_rtavepres,(int16_t)barometer_rtavetemp,
			(float)barometer_pressure_origin,(int16_t)barometer_temperature_origin));						
		ei=0;
	}
#endif
}

//  save launch point or origin barometer altitude
int32_t setBarOriginAlt(float barpres,int16_t bartemp)
{
	static int32_t origin_altitude = 0;
	origin_altitude = (int32_t)((calcBarASLAlt((float)barpres,(int16_t)bartemp,
			(float)barometer_pressure_origin,(int16_t)barometer_temperature_origin)));	// converted to Cm
	return (int32_t)origin_altitude;
}

// simple altitude computation, less exact, faster, less demanding calculation
int32_t calcBarASLAlt(float barpres,int16_t bartemp,float orig_barpres,int16_t orig_bartemp)  	
{
	static int32_t asl_altitude = 0;
	#if  (EST_BARALT_SCALED==0)
		asl_altitude=(int32_t)calcNScBarASLAlt((float)barpres);
	#elif  (EST_BARALT_SCALED==1)
		asl_altitude=(int32_t)calcSMABarASLAlt((float)barpres,(int16_t)bartemp,(float)orig_barpres,(int16_t)orig_bartemp);
	#elif  (EST_BARALT_SCALED==2)
		asl_altitude=(int32_t)calcSHABarASLAlt((float)barpres,(int16_t)bartemp,(float)orig_barpres,(int16_t)orig_bartemp);
	#endif
	return (int32_t)asl_altitude;
}

// Temperature scaled, this is an accurate barometric altitude computation, technicaly, +-2.5 M of the standard 
// atmosphere tables in the troposphere (up to 11,000 m amsl), however requires faster cpus 
int32_t calcSHABarASLAlt(float barpres,int16_t bartemp,float gnd_barpres,int16_t gnd_bartemp)  	
{
  	static int32_t scaling = 0, temp = 0;
	static int32_t asl_altitude = 0;

   	scaling				=((float)barpres/(float)gnd_barpres);
 	temp				=((float)gnd_bartemp)+273.15f;
   	// This is an exact calculation that is within +-2m of the standard atmospheric tables
   	// in the troposphere (up to 11,000 M AMSL).  
	asl_altitude=(float)(153.8462f*temp*(1.0f-expf(0.190259f*logf(scaling))));

	return (int32_t)(asl_altitude+alt_offset);
}

// on slower CPUs, use a less exact, faster, and less demanding calculation
int32_t calcSMABarASLAlt(float barpres,int16_t bartemp,float gnd_barpres,int16_t gnd_bartemp)  	
{
  	static int32_t scaling = 0, temp = 0;
	static int32_t asl_altitude = 0;
   	scaling				=((float)gnd_barpres)/(float)barpres;
 	temp				=((float)gnd_bartemp)+273.15f;
	asl_altitude=(float)(logf(scaling)*temp*29.271267f);

	return (int32_t)(asl_altitude+alt_offset);
}

// simple altitude computation, less exact, faster, less demanding calculation
int32_t calcNScBarASLAlt(float barpres)  	
{
	static int32_t asl_altitude=0;
	#if (USE_PA_PRESSURE==1)				     				// eg. 101325 defined in options.h option, average or current SL Pa (newton / sq. M) 
		static const float p0=(float)PA_PRESSURE;    					// Current pressure at sea level (SL Pa) defined in options.h  
	#elif (USE_PA_PRESSURE==2)    								// use mercury value
		static const float p0=(float)(MC_PRESSURE/0.0295333727112); 	// Current pressure at sea level (SL METAR  mercury in.) defined in options.h  
	#elif (USE_PA_PRESSURE==3)    								// fuse Pa and mercury value
		static const float p0=(float)((PA_PRESSURE*0.5f)+((MC_PRESSURE/0.0295333727112)*0.5f)); 
	#endif
	asl_altitude=(float)44330*(1-pow(((float)barpres/p0),0.190295));

	return (int32_t)(asl_altitude+alt_offset);
}


// initialize to zero all runtime variables
void initOriginVars(void)  	
{
	barometer_pressure_origin=0;
	barometer_temperature_origin=0; 
	barometer_altitude_origin=0; 
	return;
}

///	 GOOFY'S PARKING AREA:
/*
		//union longbbbb barpress;
		//union longbbbb bartemp;
				//barpress.WW = __builtin_mulss((int32_t)bar_gndpressure_pvr,(int32_t)barometer_pressure);
				//bar_gndpressure_pvr=(int32_t)(barpress._.W1<<1);
				//bartemp.WW = __builtin_mulss((int16_t)bar_gndtemperature_pvr,(int16_t)barometer_temperature);
				//bar_gndtemperature_pvr=(int16_t)(bartemp._.W1<<1);
			//barpress.WW = __builtin_mulss((int32_t)rtime_pressure_pvr,(int32_t)barometer_pressure);
			//rtime_pressure_pvr=(int32_t)(barpress._.W1<<1);
			//bartemp.WW = __builtin_mulss((int16_t)rtime_temperature_pvr,(int16_t)barometer_temperature);
			//rtime_temperature_pvr=(int16_t)(bartemp._.W1<<1);
#define PA_PRESSURE							101325   	// in Pa is the typical average pressure at sea level
#define MC_PRESSURE							3016     	// 30.16 in. mercury (METAR), set USE_PA_PRESSURE to 2
#define ASL_GROUND_ALT						13200		// above sea level (ASL) altitude in centimeters or 132 m ASL


		//union longww barpress;
		//union longww bartemp;

			//barpress.WW = __builtin_mulss(barometer_pressure,rtime_pressure_pvr);
			//rtime_pressure_pvr=(float)barpress.WW;
			//bartemp.WW = __builtin_mulss(barometer_temperature,rtime_temperature_pvr);
			//rtime_temperature_pvr=(int16_t)(bartemp.WW<<1);
*/
