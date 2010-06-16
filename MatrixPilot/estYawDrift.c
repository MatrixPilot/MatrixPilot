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


#include "p30f4011.h"
#include "defines.h"
#include <dsp.h>
#include "optionsRmat.h"
#include "definesRmat.h"

//	Compute actual and desired courses.
//	Actual course is simply the scaled GPS course over ground information.
//	Desired course is a "return home" course, which is simply the negative of the
//	angle of the vector from the origin to the location of the plane.

//	The origin is recorded as the location of the plane during power up of the control.

extern int estimatedWind[3];

fractional rmat1filt = 0 ;
fractional rmat4filt = RMAX ;

int velocity_magnitude = 0 ;
int forward_acceleration = 0 ;
int velocity_previous = 0 ;
int air_speed_magnitude = 0;

#define GPSTAU 3.0

#define GPSFILT (4.0/GPSTAU)*RMAX


void estYawDrift(void)
{
	union longbbbb accum ;
	union longww accum_velocity ;

	if ( gps_nav_valid() )
	{
		commit_gps_data() ;

	    // convert GPS course of 360 degrees to a binary model with 256	
		accum.WW = __builtin_muluu ( COURSEDEG_2_BYTECIR , cog_gps.BB ) + 0x00008000 ;
	    // re-orientate from compass (clockwise) to maths (anti-clockwise) with 0 degrees in East 
		actual_dir = -accum.__.B2 + 64 ;

		// Note that all these velocities are in centimeters / second
		velocity_magnitude = sog_gps.BB ;
		
		accum_velocity.WW = ( __builtin_mulss( cosine( actual_dir ) , velocity_magnitude) << 2) + 0x00008000 ;
		GPSvelocity.x = accum_velocity._.W1 ;
	
		accum_velocity.WW = (__builtin_mulss( sine( actual_dir ) , velocity_magnitude) << 2 ) + 0x00008000 ;
		GPSvelocity.y = accum_velocity._.W1 ;

		GPSvelocity.z = climb_gps.BB ;

		velocity_thru_air.y = GPSvelocity.y - estimatedWind[1] ;
		velocity_thru_air.x = GPSvelocity.x - estimatedWind[0] ;                                  
		calculated_heading  = rect_to_polar( &velocity_thru_air ) ;
		// veclocity_thru_air.x becomes air speed as a by product of CORDIC routine in rect_to_polar()
		air_speed_magnitude = velocity_thru_air.x; // in cm / sec

#if ( GPS_RATE == 4 )
		forward_acceleration = (velocity_magnitude - velocity_previous) << 2 ; // Ublox enters code 4 times per second
#elif ( GPS_RATE == 2 )
		forward_acceleration = (velocity_magnitude - velocity_previous) << 1 ; // Ublox enters code 2 times per second
#else
		forward_acceleration = velocity_magnitude - velocity_previous ; // EM406 standard GPS enters code once per second
#endif
	
		velocity_previous = velocity_magnitude ;
	}
	
	accum.WW = __builtin_mulss( GPSFILT , (rmat[1] - rmat1filt )) ;
	rmat1filt = rmat1filt + accum._.W1 ;
	accum.WW = __builtin_mulss( GPSFILT , (rmat[4] - rmat4filt )) ;
	rmat4filt = rmat4filt + accum._.W1 ;
	
#if ( GPS_RATE == 1 )
	dirovergndHRmat[0] = rmat1filt ;
	dirovergndHRmat[1] = rmat4filt ;
	dirovergndHRmat[2] = 0 ;
#else
	dirovergndHRmat[0] = rmat[1] ;
	dirovergndHRmat[1] = rmat[4] ;
	dirovergndHRmat[2] = 0 ;
#endif
	
	// Don't update Yaw Drift while hovering, since that doesn't work right yet
	if ( gps_nav_valid() && current_orientation != F_HOVER )
	{
		if ((estimatedWind[0] == 0) && (estimatedWind[1] == 0) || air_speed_magnitude < WIND_NAV_AIR_SPEED_MIN   )
		{
			dirovergndHGPS[0] = -cosine(actual_dir) ;
			dirovergndHGPS[1] = sine(actual_dir) ;
		}
		else
		{
			dirovergndHGPS[0] = -cosine(calculated_heading) ;
			dirovergndHGPS[1] = sine(calculated_heading) ;
		}
	}
	else
	{
		dirovergndHGPS[0] = dirovergndHRmat[0] ;
		dirovergndHGPS[1] = dirovergndHRmat[1] ;
	}
	dirovergndHGPS[2] = 0 ;
	flags._.yaw_req = 1 ;
	return ;
}
