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
#include "../libUDB/heartbeat.h"
#include "estAltitude.h"
#include "../libDCM/deadReckoning.h"
#include <math.h>
#include <stdlib.h>

//#define USE_DEBUG_IO

// The origin is recorded as the altitude of the plane during power up.

long barometer_pressure_gnd = 0;
#if (USE_BAROMETER_ALTITUDE >0)
static int32_t barometer_altitude_gnd = 0;
static int32_t barometer_altitude_gnd0 = 0;
static int32_t barometer_altitude_bias = 0;
static int32_t barometer_agl_altitude_pred;    // previous above ground level altitude - AGL
#endif
int barometer_temperature_gnd = 0;

static long barometer_altitude;        // above sea level altitude - ASL (millimeters)
int32_t barometer_agl_altitude;
static int32_t barometer_agl_altitude_1;    // previous above ground level altitude - AGL

long barometer_pressure;
int barometer_temperature;  //Degrés Centigrades
float sea_level_pressure;
static int32_t lidar_altitude;        // previous above ground height (millimeters)
static int32_t lidar_altitude_1;        // previous above ground height (millimeters)
static int32_t lidar_altitude_pred;        // predicted above ground height (millimeters)
static int32_t IMU_altitude;        // previous above ground height (millimeters)
static int32_t IMU_altitude_1;        // previous above ground height (millimeters)
static int32_t IMU_altitude_pred;        // predicted above ground height (millimeters)
static int32_t vze_baro,vze_lidar,vze_IMU;    // Altitude speeds
static int32_t vze_IMU_Bias;                //Altitude speed bias
static uint16_t qual_baro,qual_lidar,qual_IMU;// altitudes qualities
uint32_t        alpha,beta;              // filter coefficients
static uint16_t val_baro,val_lidar,val_IMU;              // altitude validities
static uint16_t baro_confiance=2;            //Altitudes confidences
static uint16_t lidar_confiance=10;
static uint16_t IMU_confiance=0;
//uint16_t feedback=256*10;//=10 / PID_HZ * 10 * 1024; // IMU altitude feedback compensation to avoid drift
uint16_t feedback=16;//=0.1*65536/40/10; // IMU altitude feedback compensation to avoid accelero drift
int32_t fusion,vze_fusion;                // Altitude fusion output (in mm)
int32_t estimated_altitude;        //  (millimeters)

inline int16_t get_barometer_temperature(void)   { return barometer_temperature; }
inline long get_barometer_pressure(void)     { return barometer_pressure; }
inline long get_barometer_altitude(void)     { return barometer_altitude; }
inline int32_t get_barometer_agl_altitude(void) { return barometer_agl_altitude; }

int16_t barometerInterval = 0;
/**
 * @brief Ascertain a reference ambient barometric pressure & temperature
 */
void altimeter_calibrate(void)
{
    qual_IMU = 2;
    IMU_altitude_1 = 0;
    IMU_altitude = 0;
    vze_IMU = 0;
    IMU_altitude_pred = 0;
#if (USE_BAROMETER_ALTITUDE == 1)
	int ground_altitude = alt_origin.WW / 100;    // meters
	barometer_temperature_gnd = barometer_temperature;
	barometer_pressure_gnd = barometer_pressure;

	sea_level_pressure = ((float)barometer_pressure / powf((1 - (ground_altitude/44330.0)), 5.255));

	DPRINT("altimeter_calibrate: ground temp & pres set %i, %li\r\n", barometer_temperature_gnd, barometer_pressure_gnd);
#elif (USE_BAROMETER_ALTITUDE == 2)
	if (barometer_altitude_gnd0 == 0) barometer_altitude_gnd0 = barometer_altitude;
        else barometer_altitude_gnd0 = (15*barometer_altitude_gnd0 + barometer_altitude)/16;
        vze_baro = 0;
        qual_baro = 10;
        barometer_altitude_gnd = barometer_altitude_gnd0;
        barometer_agl_altitude_1 = 0;
        barometer_agl_altitude_pred = 0;
        barometer_agl_altitude = 0;
#endif
#if (USE_LIDAR_ALTITUDE >0)
       vze_lidar = 0;
#endif  //LIDAR_ALTITUDE > 0
    fusion = 0;
    vze_fusion = 0;
       qual_lidar = 2;
       lidar_altitude_1 = lidar_altitude;
       lidar_altitude_pred = lidar_altitude;

#ifdef USE_DEBUG_IO
	printf("altimeter_calibrate: ground temp & pres set %i, %li\r\n", barometer_temperature_gnd, barometer_pressure_gnd);
#endif
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
    }
#elif (USE_BAROMETER_ALTITUDE == 2)
        barometer_agl_altitude = barometer_altitude - barometer_altitude_gnd;
#ifdef USE_DEBUG_IO
		// estimate sea level pressure assuming we're still on the ground
		// print altitude
		printf("estAltitude %f\r\n", (double) barometer_agl_altitude);
#endif
#endif // USE_BAROMETER_ALTITUDE
#if (USE_SONAR_INPUT != 0)
	calculate_sonar_height_above_ground();
    if (sonar_height_to_ground <10000) estimated_altitude = sonar_height_to_ground;        // lidar altitude (millimeters)
#endif

// Beginning alti_fusion gfm
      /*======================================================================*/
      /*          Altimeter fusion   (all altitudes in mm)                    */
      /*======================================================================*/
      // IMU altitude filter and offset compensation using IMUvelocityz (in mm/s)
      IMU_altitude = IMUlocationz.WW >> 6;
//      vze_IMU = __builtin_mulus(feedback,(fusion - IMU_altitude) );
//      vze_IMU = vze_IMU + IMUintegralAccelerationz.WW ;
//      vze_IMU = (int32_t)(__builtin_divud( vze_IMU, 164) );
//     #define VELOCITY2LOCATION (1.0 / PID_HZ *10.0*4.0*RMAX)// mm/s
//      vze_IMU = (__builtin_mulss(((int16_t)(VELOCITY2LOCATION)), IMUvelocityz._.W1));

      // Update qualities
      if (abs(IMU_altitude - fusion)<1000)
      {         
          if (qual_IMU <4 ) qual_IMU=qual_IMU+1;
      }
      else
      {        
          if (qual_IMU>2)  qual_IMU=qual_IMU-1;
      }
       
      if (abs(barometer_agl_altitude - fusion)<1000)
      {          
          if (qual_baro <60 ) qual_baro=qual_baro+1;
      }
      else
      {          
          if (qual_baro>10) qual_baro=qual_baro-1;
      }
      if (abs(lidar_altitude - fusion)<500)
      {          
          if (qual_lidar <2 ) qual_lidar=qual_lidar+1;
      }
      else
      {         
          if (qual_lidar>2) qual_lidar=qual_lidar-1;
      }

// Lidar confidence when altitude is lower than 12 m 
      if (IMU_altitude<12000) {         
         baro_confiance=0;
         lidar_confiance=10;
      }
      else if (IMU_altitude<16000) { //between 12 and 16 m, weighting between Lidar and Baro          
          baro_confiance=6*(IMU_altitude-12000)/12000;
          lidar_confiance=10-30*(IMU_altitude-12000)/12000;
            }
          else
          {     //No more Lidar confidence above 16 m, Baro only is used
          baro_confiance=2;
          lidar_confiance=0;
          }
      
      // Compute filter alpha beta parameters
//       alpha := 2.0*(2.0*qual_IMU-1)/qual_IMU/(qual_IMU+1) ;
//      beta := 6.0/qual_IMU/(qual_IMU+1) ;
     alpha = __builtin_muluu(2048,(2*qual_IMU-1));
      alpha = __builtin_divud(alpha,qual_IMU);
      alpha =__builtin_divud(alpha,(qual_IMU+1)) ;
//  (beta* 10 / PID_HZ), factor ten to transform cm/s in mm/s
      beta = __builtin_divud(1536,qual_IMU);
      beta = __builtin_divud(beta,(qual_IMU+1)) ;
 //     beta = __builtin_divud(beta,PID_HZ) ;

      vze_IMU = IMUvelocityz._.W1 * 10 / PID_HZ ;
//      Drift correction due to residual accelero offset
         if (fusion<10000) 
          vze_IMU_Bias=(63*vze_IMU_Bias+(vze_lidar - (IMUvelocityz.WW >> 8)))>>6;//40/10/1024=1/256
          else
          vze_IMU_Bias=(63*vze_IMU_Bias+(vze_fusion - (IMUvelocityz.WW >> 8)))>>6;
//     vze_IMU=vze_IMU+vze_IMU_Bias;
      
      vze_IMU = vze_IMU + __builtin_mulus(beta, (int16_t)(IMU_altitude-IMU_altitude_1)) ;
      IMU_altitude_pred =  IMU_altitude_pred +vze_IMU ;
      IMU_altitude_pred =  IMU_altitude_pred + ((__builtin_mulus(alpha,(int16_t)(IMU_altitude-IMU_altitude_1)))) ;
      IMU_altitude_1 = IMU_altitude_pred >> 10;
//      IMU_altitude_pred :=  IMU_altitude_pred + ((alpha*(IMU_altitude-IMU_altitude_1))) +vze_IMU;
//      IMU_altitude_1 := IMU_altitude_pred  div 1024;// One more time division by 1024 for renormalisation
#if USE_BAROMETER_ALTITUDE>0
      // Compute filter alpha beta parameters
      alpha = __builtin_muluu(2048,(2*qual_baro-1));
      alpha = __builtin_divud(alpha,qual_baro);
      alpha =__builtin_divud(alpha,(qual_baro+1)) ;
      beta = __builtin_divud(1536,qual_baro);
      beta = __builtin_divud(beta,(qual_baro+1)) ;

      vze_baro = vze_baro +  __builtin_mulus(beta, (int16_t)(barometer_agl_altitude-barometer_agl_altitude_1) );
//      if (val_baro>0) barometer_altitude_gnd = barometer_altitude_gnd + __builtin_mulus(feedback,(fusion - barometer_agl_altitude));
      barometer_agl_altitude_pred =  barometer_agl_altitude_pred + __builtin_mulus(alpha , (int16_t)(barometer_agl_altitude-barometer_agl_altitude_1)) +vze_baro;
      barometer_agl_altitude_1 = barometer_agl_altitude_pred >> 10;
#endif
      // Compute filter alpha beta parameters (multiplied by 1024)
      alpha = __builtin_muluu(2048,(2*qual_lidar-1));
      alpha = __builtin_divud(alpha,qual_lidar);
      alpha =__builtin_divud(alpha,(qual_lidar+1)) ;
      beta = __builtin_divud(1536,qual_lidar);
      beta = __builtin_divud(beta,(qual_lidar+1)) ;

      vze_lidar = vze_lidar + __builtin_mulus(beta , (int16_t)(lidar_altitude-lidar_altitude_1)) ;
      lidar_altitude_pred =  lidar_altitude_pred + __builtin_mulus(alpha,(int16_t)( lidar_altitude-lidar_altitude_1))+vze_lidar ;
      lidar_altitude_1 = lidar_altitude_pred >> 10;

      // update confidences
      val_IMU = __builtin_muluu(qual_IMU , IMU_confiance)<<1  ;//val_IMU max = 4*5*2=40
      if (( IMU_altitude < -1000) || (abs(IMU_altitude - fusion)>4000)) val_IMU = 0;
#if USE_BAROMETER_ALTITUDE>0
      val_baro = __builtin_muluu(qual_baro , baro_confiance);   //val_baro max = 60
      if (( barometer_agl_altitude_1 < -4000) || (abs(barometer_agl_altitude_1 - fusion)>10000)) val_baro = 0;
#else
      val_baro = 0;
#endif
      val_lidar = __builtin_muluu(qual_lidar , lidar_confiance)>>1;// val_lidar max = 8*10/2=40
      if ((abs(lidar_altitude_1 - fusion)>6000)) val_lidar = 0;
     // update confidences
//      val_GPS := qual_GPS * GPS_Confiance;
//      if (( GPS = 0.0) OR (abs(GPS - Alti)>0.5)) then val_GPS := 0.0;
    

      // compute fusion
      if ((val_baro>0.0) || (val_lidar>0.0) || (val_IMU>0.0))
        {
//        fusion = (fusion+barometer_agl_altitude_pred*val_baro+lidar_altitude_pred*val_lidar + IMU_altitude*val_IMU ) ;
//        fusion = (fusion << 7) +__builtin_mulus(val_baro,barometer_agl_altitude_1);
        fusion = fusion + __builtin_mulus(val_baro,(int16_t)barometer_agl_altitude_1);
        fusion = fusion +__builtin_mulus(val_lidar,(int16_t)lidar_altitude_1);
        fusion = fusion +__builtin_mulus(val_IMU,(int16_t)(IMU_altitude_1))  ;
        fusion = __builtin_divsd(fusion ,(1+val_baro+val_lidar+val_IMU));
//        fusion = __builtin_divsd(fusion ,(128+val_baro+val_lidar+val_IMU));
       }
      else
      {
      fusion = IMU_altitude;
      }
        // If Lidar is validated, then filter vze_fusion and finally compense eventual drift of Vz by Vz=(1-alpha)Vz+alpha*vze_fusion with alpha=1/164
//        vze_fusion = vze_IMU>>8; // en cm/s
//        val_IMU=val_IMU>>3;
//        val_lidar=val_lidar>>3;
//        vze_fusion =__builtin_divsd((5*vze_fusion + vze_IMU*val_IMU+ vze_lidar*val_lidar) , (5+val_IMU+val_lidar));
//            IMUintegralAccelerationz.WW=__builtin_mulus(65136,IMUintegralAccelerationz._.W1)+vze_fusion;// IMUvelocityz._.W1 in cm/s est positive vers le haut
      if ((val_baro>0.0) || (val_lidar>0.0))
      {
          vze_fusion = vze_lidar>>8; // en cm/s
//          IMUintegralAccelerationz.WW=vze_lidar*256;// IMUvelocityz._.W1 in cm/s est positive vers le haut 164=65536/400
        }
            else
            {
//            if (val_IMU>5) vze_fusion = (16*vze_fusion + vze_baro*val_baro+ vze_IMU*val_IMU) / (16+val_baro+val_IMU);
//             IMUintegralAccelerationz.WW=__builtin_mulus(65136,IMUintegralAccelerationz._.W1)+vze_fusion;// IMUvelocityz._.W1 in cm/s est positive vers le haut
         vze_fusion =IMUintegralAccelerationz._.W1;
       }

//        IMUlocationz.WW = fusion <<6;
//        IMUintegralAccelerationz.WW +=  __builtin_mulus(feedback,(fusion - IMU_altitude));
          IMUintegralAccelerationz.WW +=  vze_IMU_Bias <<4;
    estimated_altitude =  fusion ;  // Altitude in mm, positive upward
// fin alti_fusion gfm
}
