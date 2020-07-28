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
#include "gpsParseCommon.h"
#include "deadReckoning_lidar.h"
#include "mathlibNAV.h"
#include "estWind.h"
#include "gpsData.h"
#include "rmat.h"
#include "../libUDB/heartbeat.h"



// seconds
#define DR_TIMESTEP (1.0/HEARTBEAT_HZ)

// 1.0 in 0.16 format
#define MAX16 (4.0*RMAX)

// seconds
//#define DR_TAU 2.0

// GRAVITYM is 980 cm/sec^2, GRAVITY is 2000 counts
// XY: dx/dt^2 * ACCEL2DELTAV_XY = cm/sec
#define ACCEL2DELTAV_XY ((DR_TIMESTEP*GRAVITYM*MAX16)/GRAVITY)
// Z: dx/dt^2 * ACCEL2DELTAV_Z = mm/sec
#define ACCEL2DELTAV_Z ((DR_TIMESTEP*10.0*GRAVITYM*MAX16)/GRAVITY)

//  X and Y velocity in cm/sec, location in meters
#define VELOCITY2LOCATION_XY (DR_TIMESTEP*.01*MAX16*16.0)
// The factor of 16 is so that the gain is more precise.
// There is a subsequent right shift by 4 to cancel the multiply by 16.

// Z velocity in mm/sec,  Z location in mm
#define VELOCITY2LOCATION_Z (DR_TIMESTEP*MAX16*16.0)

// dimensionless
#define DR_FILTER_GAIN (int16_t)(DR_TIMESTEP*MAX16/DR_TAU)

// 1/seconds
#define ONE_OVER_TAU (uint16_t)(MAX16/DR_TAU)
#define DR_I_GAIN (DR_FILTER_GAIN/DR_TAU)

// velocity, as estimated by the IMU: high word is millimeters/sec
union longww IMUvelocityx = { 0 };
union longww IMUvelocityy = { 0 };
union longww IMUvelocityz = { 0 };

int16_t forward_ground_speed = 0 ;
int16_t IMU_altitude = 0 ;
int16_t IMU_climb = 0 ;
extern int16_t altitude ;
extern int16_t climb_rate ;
extern int16_t pwManual[NUM_INPUTS+1] ;
extern int16_t number_pulses ;

// location, as estimated by the IMU
// high word is millimeters, low word is fractional millimeters
union longww IMUlocationx = { 0 };
union longww IMUlocationy = { 0 };
union longww IMUlocationz = { 0 };

// integral of velocity error for acceleration bias compensation
union longww IMUintegralAccelerationx = { 0 };
union longww IMUintegralAccelerationy = { 0 };
union longww IMUintegralAccelerationz = { 0 };

uint16_t air_speed_3DIMU = 0;
int16_t total_energy = 0;

// GPSlocation - IMUlocation: millimeters
fractional locationErrorEarth[] = { 0, 0, 0 };
// GPSvelocity - IMUvelocity
fractional velocityErrorEarth[] = { 0, 0, 0 };

extern boolean origin_recorded ;
extern int16_t dead_reckon_clock ;
void dead_reckon(void)
{
	int16_t air_speed_x, air_speed_y, air_speed_z;
	{
		// compute location and velocity errors
		// for Z use LIDAR
		// wait until takeoff to use altitude information, LIDAR does not work on the ground	
		if (abs(pwManual[THROTTLE_INPUT_CHANNEL]-udb_pwTrim[THROTTLE_INPUT_CHANNEL])< 200 )
		{
			// sitting on the ground
			IMUlocationz.WW = 0 ;
			IMUvelocityz.WW = 0 ;
			IMUintegralAccelerationz.WW = 0 ;
		}
		else
		{		
			if (number_pulses>=MIN_LIDAR_PULSE_THRESH)
			{				
				// use LIDAR for altitude
				locationErrorEarth[2] = altitude - IMUlocationz._.W1;
				velocityErrorEarth[2] = climb_rate - IMUvelocityz._.W1;
			}
			else
			{
				// true dead reckoning, ignore the LIDAR
				locationErrorEarth[2] = 0 ;
				velocityErrorEarth[2] = 0 ;
			}
			// compensate for velocity error ;
			IMUintegralAccelerationz.WW += __builtin_mulss(((int16_t)(DR_I_GAIN)), velocityErrorEarth[2]);

			// integrate the raw acceleration
			IMUvelocityz.WW += __builtin_mulss(((int16_t)(ACCEL2DELTAV_Z)), accelEarth[2]);
		
			// apply the proportional term for the acceleration bias compensation
			IMUvelocityz.WW += __builtin_mulss(2*DR_FILTER_GAIN, velocityErrorEarth[2]);
		
			// apply the integral term for the acceleration bias compensation
			IMUvelocityz.WW += __builtin_mulss(DR_TIMESTEP*MAX16,IMUintegralAccelerationz._.W1);
		
			// integrate IMU velocity to update the IMU location	
			IMUlocationz.WW += (__builtin_mulss(((int16_t)(VELOCITY2LOCATION_Z)), IMUvelocityz._.W1)>>4);

			// apply the location bias compensation
			IMUlocationz.WW += __builtin_mulss(DR_FILTER_GAIN, locationErrorEarth[2]);
		}
	
		// use GPS for X and Y
		if(origin_recorded&&(dead_reckon_clock>0))
		{
			dead_reckon_clock--;
			if (dcm_flags._.reckon_req)
			{
				dcm_flags._.reckon_req = 0 ;
				locationErrorEarth[0] = GPSlocation.x - IMUlocationx._.W1;
				locationErrorEarth[1] = GPSlocation.y - IMUlocationy._.W1;
				velocityErrorEarth[0] = GPSvelocity.x - IMUvelocityx._.W1;
				velocityErrorEarth[1] = GPSvelocity.y - IMUvelocityy._.W1;
			}
			// compute the integral term for the acceleration bias compensation
			IMUintegralAccelerationx.WW += __builtin_mulss(((int16_t)(DR_I_GAIN)), velocityErrorEarth[0]);
			IMUintegralAccelerationy.WW += __builtin_mulss(((int16_t)(DR_I_GAIN)), velocityErrorEarth[1]);

			// integrate the raw acceleration
			IMUvelocityx.WW += __builtin_mulss(((int16_t)(ACCEL2DELTAV_XY)), accelEarth[0]);
			IMUvelocityy.WW += __builtin_mulss(((int16_t)(ACCEL2DELTAV_XY)), accelEarth[1]);
		
			// apply the proportional term for the acceleration bias compensation
			IMUvelocityx.WW += __builtin_mulss(2*DR_FILTER_GAIN, velocityErrorEarth[0]);
			IMUvelocityy.WW += __builtin_mulss(2*DR_FILTER_GAIN, velocityErrorEarth[1]);
		
			// apply the integral term for the acceleration bias compensation
			IMUvelocityx.WW += __builtin_mulss(DR_TIMESTEP*MAX16,IMUintegralAccelerationx._.W1);
			IMUvelocityy.WW += __builtin_mulss(DR_TIMESTEP*MAX16,IMUintegralAccelerationy._.W1);
		
			// integrate IMU velocity to update the IMU location	
			IMUlocationx.WW += (__builtin_mulss(((int16_t)(VELOCITY2LOCATION_XY)), IMUvelocityx._.W1)>>4);
			IMUlocationy.WW += (__builtin_mulss(((int16_t)(VELOCITY2LOCATION_XY)), IMUvelocityy._.W1)>>4);

			// apply the location bias compensation
			IMUlocationx.WW += __builtin_mulss(DR_FILTER_GAIN, locationErrorEarth[0]);
			IMUlocationy.WW += __builtin_mulss(DR_FILTER_GAIN, locationErrorEarth[1]);
		}
		else
		{
			IMUlocationx.WW += (__builtin_mulss(((int16_t)(VELOCITY2LOCATION_XY)), IMUvelocityx._.W1)>>4);
			IMUlocationy.WW += (__builtin_mulss(((int16_t)(VELOCITY2LOCATION_XY)), IMUvelocityy._.W1)>>4);
		}
	}
	
	// the following used to include an adjustment for the wind
	air_speed_x = IMUvelocityx._.W1 ;
	air_speed_y = IMUvelocityy._.W1 ;
	air_speed_z = IMUvelocityz._.W1 ;
	IMU_climb = air_speed_z ;
	IMU_altitude = IMUlocationz._.W1 ;
}
