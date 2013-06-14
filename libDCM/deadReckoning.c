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


#include "libDCM_internal.h"

#define DR_PERIOD (int16_t)((40/GPS_RATE)+4 )

#define DR_TIMESTEP 0.025
#define MAX16 (4.0*RMAX)
#define DR_TAU 2.5

#define ACCEL2DELTAV ((DR_TIMESTEP*GRAVITYM*MAX16)/GRAVITY)
#define VELOCITY2LOCATION (DR_TIMESTEP*.01*MAX16*16.0)
//	The factor of 16 is so that the gain is more precise.
//	There is a subsequent right shift by 4 to cancel the multiply by 16.

#define DR_FILTER_GAIN (int16_t) (DR_TIMESTEP*MAX16/DR_TAU)
#define ONE_OVER_TAU (uint16_t) (MAX16/DR_TAU)

int16_t dead_reckon_clock = DR_PERIOD ;

//      velocity, as estimated by the IMU
union longww IMUvelocityx =  { 0 }  ;
union longww IMUvelocityy =  { 0 }  ;
union longww IMUvelocityz =  { 0 }  ;

//      location, as estimated by the IMU
union longww IMUlocationx =  { 0 }  ;
union longww IMUlocationy =  { 0 }  ;
union longww IMUlocationz =  { 0 }  ;

//		velocity, as estimated by dead-reckoning (virtual GPS)
int16_t DRvelocityx = 0 ;
int16_t DRvelocityy = 0 ;
int16_t DRvelocityz = 0 ;

//		location, as estimated by dead-reckoning (virtual GPS)
union longww DRlocationx =  { 0 } ;
union longww DRlocationy =  { 0 } ;
union longww DRlocationz =  { 0 } ;

//	integral of acceleration
union longww IMUintegralAccelerationx = { 0 } ;
union longww IMUintegralAccelerationy = { 0 } ;
union longww IMUintegralAccelerationz = { 0 } ;

uint16_t air_speed_3DIMU = 0 ;
int16_t total_energy = 0 ;

//	GPSlocation - IMUlocation
fractional locationErrorEarth[] = { 0 , 0 , 0 } ;
//	GPSvelocity - IMUvelocity
fractional velocityErrorEarth[] = { 0 , 0 , 0 } ;

extern int16_t errorYawground[] ;


// determine whether or not to use the virtual GPS
// instead of the real one. For now, for testing purposes,
// control this by turning the radio on/off.
// later, this is where the logic will be for assessing GPS health
boolean use_virtual_gps()
{
	if ( udb_flags._.radio_on == 1 )
	{
		return false ;
	}
	else
	{
		return true ;
	}
}

void compute_virtual_gps()
{
	union longww accum ;
	if ( use_virtual_gps() ){

		accum.WW = - ( __builtin_mulus ( air_speed_3DGPS , rmat[1] ) << 2 ) ;
		DRvelocityx = accum._.W1 + estimatedWind[0] ;

		accum.WW =  ( __builtin_mulus ( air_speed_3DGPS , rmat[4] ) << 2 ) ;
		DRvelocityy = accum._.W1 + estimatedWind[1] ;

		//	integrate DR horizontal velocity to update the DR horizontal location	
		DRlocationx.WW += ( __builtin_mulss( ((int16_t)(VELOCITY2LOCATION)) ,  DRvelocityx )>>4 ) ;
		DRlocationy.WW += ( __builtin_mulss( ((int16_t)(VELOCITY2LOCATION)) ,  DRvelocityy )>>4 ) ;
	
		DRlocationz._.W1 = GPSlocation.z ;
		DRlocationz._.W0 = 0 ;
		DRvelocityz = GPSvelocity.z ;
	}
	else {
		DRlocationx._.W1 = GPSlocation.x ;
		DRlocationy._.W1 = GPSlocation.y ;
		DRlocationz._.W1 = GPSlocation.z ;

		DRlocationx._.W0 = 0 ;
		DRlocationy._.W0 = 0 ;
		DRlocationz._.W0 = 0 ;

		DRvelocityx = GPSvelocity.x ;
		DRvelocityy = GPSvelocity.y ;
		DRvelocityz = GPSvelocity.z ;
	}
}

int16_t gps_imu_location_offset[3] = { 0 , 0 , 0 } ;
int16_t gps_imu_location_offset_previous[3] = { 0 , 0 , 0 } ; 
int16_t gps_imu_location_offset_rate[3] = { 0 , 0 , 0 } ; 
int16_t gps_imu_location_offset_rate_previous[3] = { 0 , 0 , 0 } ; 

int16_t gps_imu_velocity_offset[3] = { 0 , 0 , 0 } ;
int16_t gps_imu_velocity_offset_previous[3] = { 0 , 0 , 0 } ;

int16_t gps_location_noise[3] = { 0 , 0 , 0 } ;
int16_t gps_velocity_noise[3] = { 0 , 0 , 0 } ;
int16_t gps_location_noise_total = 0 ;
int16_t gps_velocity_noise_total = 0 ;

void dead_reckon(void)
{
	if ( dcm_flags._.dead_reckon_enable == 1 )  // wait for startup of GPS
	{
		//	integrate the accelerometers to update IMU velocity
		IMUintegralAccelerationx.WW += __builtin_mulss( ((int16_t)(ACCEL2DELTAV)) ,  accelEarth[0] ) ;
		IMUintegralAccelerationy.WW += __builtin_mulss( ((int16_t)(ACCEL2DELTAV)) ,  accelEarth[1] ) ;
		IMUintegralAccelerationz.WW += __builtin_mulss( ((int16_t)(ACCEL2DELTAV)) ,  accelEarth[2] ) ;

		//	integrate IMU velocity to update the IMU location	
		IMUlocationx.WW += ( __builtin_mulss( ((int16_t)(VELOCITY2LOCATION)) ,  IMUintegralAccelerationx._.W1 )>>4 ) ;
		IMUlocationy.WW += ( __builtin_mulss( ((int16_t)(VELOCITY2LOCATION)) ,  IMUintegralAccelerationy._.W1 )>>4 ) ;
		IMUlocationz.WW += ( __builtin_mulss( ((int16_t)(VELOCITY2LOCATION)) ,  IMUintegralAccelerationz._.W1 )>>4 ) ;

		compute_virtual_gps() ;

		if ( dead_reckon_clock > 0 )
		//	apply drift adjustments only while valid GPS data is in force.
		//  This is done with a countdown clock that gets reset each time new data comes in.
		{
			dead_reckon_clock -- ;

			IMUintegralAccelerationx.WW += __builtin_mulss( DR_FILTER_GAIN ,  velocityErrorEarth[0] ) ;
			IMUintegralAccelerationy.WW += __builtin_mulss( DR_FILTER_GAIN ,  velocityErrorEarth[1] ) ;
			IMUintegralAccelerationz.WW += __builtin_mulss( DR_FILTER_GAIN ,  velocityErrorEarth[2] ) ;
	
			IMUlocationx.WW += __builtin_mulss( DR_FILTER_GAIN ,  locationErrorEarth[0] ) ;
			IMUlocationy.WW += __builtin_mulss( DR_FILTER_GAIN ,  locationErrorEarth[1] ) ;
			IMUlocationz.WW += __builtin_mulss( DR_FILTER_GAIN ,  locationErrorEarth[2] ) ;

			IMUvelocityx.WW = IMUintegralAccelerationx.WW +
								__builtin_mulus( ONE_OVER_TAU , 100*locationErrorEarth[0] ) ;
			IMUvelocityy.WW = IMUintegralAccelerationy.WW +
								__builtin_mulus( ONE_OVER_TAU , 100*locationErrorEarth[1] ) ;
			IMUvelocityz.WW = IMUintegralAccelerationz.WW +
								__builtin_mulus( ONE_OVER_TAU , 100*locationErrorEarth[2] ) ;

		}
		else  // GPS has gotten disconnected
		{
			errorYawground[0] = errorYawground[1] = errorYawground[2] = 0 ; // turn off yaw drift
			dcm_flags._.gps_history_valid = 0 ; // restart GPS history variables
			IMUvelocityx.WW = IMUintegralAccelerationx.WW ;
			IMUvelocityy.WW = IMUintegralAccelerationy.WW ;
			IMUvelocityz.WW = IMUintegralAccelerationz.WW ;
		}
	
		if ( gps_nav_valid() && ( dcm_flags._.reckon_req == 1 ) )
		{
			//	compute error indications and restart the dead reckoning clock to apply them
			dcm_flags._.reckon_req = 0 ;
			dead_reckon_clock = DR_PERIOD ;
	
			locationErrorEarth[0] = DRlocationx._.W1 - IMUlocationx._.W1 ;
			locationErrorEarth[1] = DRlocationy._.W1 - IMUlocationy._.W1 ;
			locationErrorEarth[2] = DRlocationz._.W1 - IMUlocationz._.W1 ;

			velocityErrorEarth[0] = DRvelocityx - IMUintegralAccelerationx._.W1 ;
			velocityErrorEarth[1] = DRvelocityy - IMUintegralAccelerationy._.W1 ;
			velocityErrorEarth[2] = DRvelocityz - IMUintegralAccelerationz._.W1 ;

//			GPS health check computations

			gps_imu_location_offset[0] = GPSlocation.x - IMUlocationx._.W1 ;
			gps_imu_location_offset[1] = GPSlocation.y - IMUlocationy._.W1 ;
			gps_imu_location_offset[2] = GPSlocation.z - IMUlocationz._.W1 ;

			VectorSubtract( 3 , gps_imu_location_offset_rate , gps_imu_location_offset , gps_imu_location_offset_previous ) ;

			gps_location_noise[0] += ( gps_imu_location_offset_rate[0] - gps_imu_location_offset_rate_previous[0] ) - ( ( gps_location_noise[0] ) >> 2 ) ;
			gps_location_noise[1] += ( gps_imu_location_offset_rate[1] - gps_imu_location_offset_rate_previous[1] ) - ( ( gps_location_noise[1] ) >> 2 ) ;
			gps_location_noise[2] += ( gps_imu_location_offset_rate[2] - gps_imu_location_offset_rate_previous[2] ) - ( ( gps_location_noise[2] ) >> 2 ) ;

			VectorCopy ( 3 , gps_imu_location_offset_previous , gps_imu_location_offset ) ;
			VectorCopy ( 3 , gps_imu_location_offset_rate_previous , gps_imu_location_offset_rate ) ;

			gps_imu_velocity_offset[0] = GPSvelocity.x - IMUvelocityx._.W1 ;
			gps_imu_velocity_offset[1] = GPSvelocity.y - IMUvelocityy._.W1 ;
			gps_imu_velocity_offset[2] = GPSvelocity.z - IMUvelocityz._.W1 ;

			gps_velocity_noise[0] += ( gps_imu_velocity_offset[0] - gps_imu_velocity_offset_previous[0] ) - ( ( gps_velocity_noise[0] ) >> 2 ) ;
			gps_velocity_noise[1] += ( gps_imu_velocity_offset[1] - gps_imu_velocity_offset_previous[1] ) - ( ( gps_velocity_noise[1] ) >> 2 ) ;
			gps_velocity_noise[2] += ( gps_imu_velocity_offset[2] - gps_imu_velocity_offset_previous[2] ) - ( ( gps_velocity_noise[2] ) >> 2 ) ;	

			VectorCopy ( 3 , gps_imu_velocity_offset_previous , gps_imu_velocity_offset ) ;

			gps_location_noise_total = abs( gps_location_noise[0] ) + abs( gps_location_noise[1] ) +abs( gps_location_noise[2] ) ;
			gps_velocity_noise_total = abs( gps_velocity_noise[0] ) + abs( gps_velocity_noise[1] ) +abs( gps_velocity_noise[2] ) ;
		
		}
	}
	else
	{
		IMUintegralAccelerationx.WW = 0 ;
		IMUintegralAccelerationy.WW = 0 ;
		IMUintegralAccelerationz.WW = 0 ;

		IMUvelocityx.WW = 0 ;
		IMUvelocityy.WW = 0 ;
		IMUvelocityz.WW = 0 ;
	
		IMUlocationx.WW = 0 ;
		IMUlocationy.WW = 0 ;
		IMUlocationz.WW = 0 ;
	}

	int16_t air_speed_x , air_speed_y , air_speed_z ;

	air_speed_x = IMUvelocityx._.W1 - estimatedWind[0] ;
	air_speed_y = IMUvelocityy._.W1 - estimatedWind[1] ;
	air_speed_z = IMUvelocityz._.W1 - estimatedWind[2] ;

#if ( HILSIM == 1 )
	air_speed_3DIMU = as_sim.BB ; // use Xplane as a pitot
#else
	air_speed_3DIMU = 
					vector3_mag ( 	air_speed_x , air_speed_y , air_speed_z ) ;
#endif

	union longww accum ;
	union longww energy ;

	accum.WW = __builtin_mulsu ( air_speed_x , 37877 ) ;
	energy.WW = __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( air_speed_y , 37877 ) ;
	energy.WW += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( air_speed_z , 37877 ) ;
	energy.WW += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	energy.WW += IMUlocationz.WW ;
	total_energy = energy._.W1 ;
	
}


