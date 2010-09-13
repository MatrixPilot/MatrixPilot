// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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

#if ( GPS_TYPE == GPS_STD )
#define GPS_DELAY 1.75

#elif ( GPS_TYPE == GPS_UBX_2HZ )
#define GPS_DELAY 1.25

#elif ( GPS_TYPE == GPS_UBX_4HZ )
#if ( HILSIM == 1 )
#define GPS_DELAY 0
#else
#define GPS_DELAY 1.25
#endif
#else
#define GPS_DELAY 1.25

#endif

#define DR_BUFFER_SIZE 2 + (int)(GPS_RATE*GPS_DELAY)
#define DR_PERIOD (int)(40/GPS_RATE)
#define DR_INTERPOLATION (int)(16.0*(GPS_RATE*GPS_DELAY - (int)(GPS_RATE*GPS_DELAY))) 

extern fractional accelEarth[] ;

#define ACCEL2DELTAV ((.025*GRAVITYM*4.0*RMAX)/GRAVITY)
#define VELOCITY2LOCATION (0.025*.01*4.0*RMAX)

#define TIMESTEP 0.025
#define MAX16 (4.0*RMAX)

int dead_reckon_clock = 0 ;

//      velocity, as estimated by the IMU
union longww IMUvelocityx =  { 0 }  ;
union longww IMUvelocityy =  { 0 }  ;
union longww IMUvelocityz =  { 0 }  ;

fractional locationErrorEarth[] = { 0 , 0 , 0 } ;

//      location, as estimated by the IMU
union longww IMUlocationx =  { 0 }  ;
union longww IMUlocationy =  { 0 }  ;
union longww IMUlocationz =  { 0 }  ;

int IMUvelocityxHistory[DR_BUFFER_SIZE] ;
int IMUvelocityyHistory[DR_BUFFER_SIZE] ;
int IMUvelocityzHistory[DR_BUFFER_SIZE] ;

int IMUlocationxHistory[DR_BUFFER_SIZE] ;
int IMUlocationyHistory[DR_BUFFER_SIZE] ;
int IMUlocationzHistory[DR_BUFFER_SIZE] ;

void shift_deadreckon_history(void)
{
	int buffer_index ;
	int next_slot ;
	for ( buffer_index = 0 ; buffer_index < DR_BUFFER_SIZE - 1 ; buffer_index++ )
	{
		next_slot = buffer_index+1 ; 
		IMUvelocityxHistory[buffer_index] = IMUvelocityxHistory[next_slot] ;
		IMUvelocityyHistory[buffer_index] = IMUvelocityyHistory[next_slot] ;
		IMUvelocityzHistory[buffer_index] = IMUvelocityzHistory[next_slot] ;

		IMUlocationxHistory[buffer_index] = IMUlocationxHistory[next_slot] ;
		IMUlocationyHistory[buffer_index] = IMUlocationyHistory[next_slot] ;
		IMUlocationzHistory[buffer_index] = IMUlocationzHistory[next_slot] ;
	}
	return ;
}

void snapshot_dead_reckon(void)
{

	IMUvelocityxHistory[DR_BUFFER_SIZE-1] = IMUvelocityx._.W1 ;
	IMUvelocityyHistory[DR_BUFFER_SIZE-1] = IMUvelocityy._.W1 ;
	IMUvelocityzHistory[DR_BUFFER_SIZE-1] = IMUvelocityz._.W1 ;

	IMUlocationxHistory[DR_BUFFER_SIZE-1] = IMUlocationx._.W1 ;
	IMUlocationyHistory[DR_BUFFER_SIZE-1] = IMUlocationy._.W1 ;
	IMUlocationzHistory[DR_BUFFER_SIZE-1] = IMUlocationz._.W1 ;

	return ;
}

void adjust_dead_reckon(void)
{
	union longww accumulator ;

	// velocities

	accumulator.WW = __builtin_mulss(  ( IMUvelocityxHistory[1] - IMUvelocityxHistory[0] ) , DR_INTERPOLATION ) << 12 ;
	accumulator._.W1 += - IMUvelocityxHistory[1] + GPSvelocity.x ;
	IMUvelocityx.WW += accumulator.WW >> 1 ;

	accumulator.WW = __builtin_mulss(  ( IMUvelocityyHistory[1] - IMUvelocityyHistory[0] ) , DR_INTERPOLATION ) << 12 ;
	accumulator._.W1 += - IMUvelocityyHistory[1] + GPSvelocity.y ;
	IMUvelocityy.WW += accumulator.WW >> 1;

	accumulator.WW = __builtin_mulss(  ( IMUvelocityzHistory[1] - IMUvelocityzHistory[0] ) , DR_INTERPOLATION ) << 12 ;
	accumulator._.W1 += - IMUvelocityzHistory[1] + GPSvelocity.z ;
	IMUvelocityz.WW += accumulator.WW >> 1;

	accumulator.WW = __builtin_mulss(  ( IMUlocationxHistory[1] - IMUlocationxHistory[0] ) , DR_INTERPOLATION ) << 12 ;
	accumulator._.W1 += - IMUlocationxHistory[1] + GPSlocation.x ;
	IMUlocationx.WW += accumulator.WW >> 1 ;
	locationErrorEarth[0] = accumulator._.W1 ;

	accumulator.WW = __builtin_mulss(  ( IMUlocationyHistory[1] - IMUlocationyHistory[0] ) , DR_INTERPOLATION ) << 12 ;
	accumulator._.W1 += - IMUlocationyHistory[1] + GPSlocation.y ;
	IMUlocationy.WW += accumulator.WW >> 1 ;
	locationErrorEarth[1] = accumulator._.W1 ;

	accumulator.WW = __builtin_mulss(  ( IMUlocationzHistory[1] - IMUlocationzHistory[0] ) , DR_INTERPOLATION ) << 12 ;
	accumulator._.W1 += - IMUlocationzHistory[1] + GPSlocation.z ;
	IMUlocationz.WW += accumulator.WW >> 1  ;
	locationErrorEarth[2] = accumulator._.W1 ;

	return ;
}

void dead_reckon(void)
{

	if ( dcm_flags._.dead_reckon_enable == 1 )  // wait for startup of GPS
	{
		
		IMUvelocityx.WW += __builtin_mulss( ((int)(ACCEL2DELTAV)) ,  accelEarth[0] ) ;
		IMUvelocityy.WW += __builtin_mulss( ((int)(ACCEL2DELTAV)) ,  accelEarth[1] ) ;
		IMUvelocityz.WW += __builtin_mulss( ((int)(ACCEL2DELTAV)) ,  accelEarth[2] ) ;
	
		IMUlocationx.WW += __builtin_mulss( ((int)(VELOCITY2LOCATION)) ,  IMUvelocityx._.W1 ) ;
		IMUlocationy.WW += __builtin_mulss( ((int)(VELOCITY2LOCATION)) ,  IMUvelocityy._.W1 ) ;
		IMUlocationz.WW += __builtin_mulss( ((int)(VELOCITY2LOCATION)) ,  IMUvelocityz._.W1 ) ;
		if ( dead_reckon_clock++ > (int)(DR_PERIOD+5) )  // GPS watchdog
		{
			shift_deadreckon_history() ;
			snapshot_dead_reckon() ;
			dead_reckon_clock = 0 ;
		}
		else if ( gps_nav_valid() && ( dcm_flags._.reckon_req == 1 ) )
		{
			dcm_flags._.reckon_req = 0 ;
			dead_reckon_clock = 0 ;
			shift_deadreckon_history() ;
			snapshot_dead_reckon() ;  // this is needed for the case of a delay less than 1 tick
			adjust_dead_reckon() ;	
			snapshot_dead_reckon() ;
		}
	}
	else
	{
		IMUvelocityx.WW = 0 ;
		IMUvelocityy.WW = 0 ;
		IMUvelocityz.WW = 0 ;
	
		IMUlocationx.WW = 0 ;
		IMUlocationy.WW = 0 ;
		IMUlocationz.WW = 0 ;
	}
	return ;
}


