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


#include "defines.h"
#include "../libUDB/libUDB.h"

//	Compute actual and desired courses.
//	Actual course is simply the scaled GPS course over ground information.
//	Desired course is a "return home" course, which is simply the negative of the
//	angle of the vector from the origin to the location of the plane.

//	The origin is recorded as the location of the plane during power up of the control.
#if (( SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK ) || ( GAINS_VARIABLE == 1 ))
	int yawkpail = YAWKP_AILERON*RMAX ;
	int yawkprud = YAWKP_RUDDER*RMAX ;
#else 
	const int yawkpail = YAWKP_AILERON*RMAX ;
	const int yawkprud = YAWKP_RUDDER*RMAX ;
#endif

struct waypointparameters goal ;
struct relative2D togoal = { 0 , 0 } ;
int tofinish_line  = 0 ;
int progress_to_goal = 0 ;
signed char desired_dir = 0;


static void setup_origin(void)
{
	if (use_fixed_origin())
	{
		struct absolute3D origin = get_fixed_origin() ;
		dcm_set_origin_location(origin.x, origin.y, origin.z ) ;
	}
	else
	{
		dcm_set_origin_location(long_gps.WW, lat_gps.WW, alt_sl_gps.WW) ;
	}
	flags._.f13_print_req = 1 ; // Flag telemetry output that the origin can now be printed.
	
	return ;
}


void dcm_callback_gps_location_updated(void)
{
	if ( flags._.save_origin )
	{
		//	capture origin information during power up. much of this is not actually used for anything,
		//	but is saved in case you decide to extend this code.
		flags._.save_origin = 0 ;
		setup_origin() ;
	}

	
//	Ideally, navigate should take less than one second. For MatrixPilot, navigation takes only
//	a few milliseconds.
	
//	If you rewrite navigation to perform some rather ambitious calculations, perhaps using floating
//	point, matrix inversions, Kalman filters, etc., you will not cause a stack overflow if you
//	take more than 1 second, the interrupt handler will simply skip some of the navigation passes.
	
	return ;
}


void set_goal( struct relative3D fromPoint , struct relative3D toPoint )
{
	struct relative2D courseLeg ;
	
	goal.x = toPoint.x ;
	goal.y = toPoint.y ;
	goal.height = toPoint.z ;
	goal.fromHeight = fromPoint.z ;
	
	courseLeg.x = toPoint.x - fromPoint.x ;
	courseLeg.y = toPoint.y - fromPoint.y ;
	
	goal.phi = rect_to_polar ( &courseLeg ) ;
	goal.legDist = courseLeg.x ;
	goal.cosphi = cosine( goal.phi ) ;
	goal.sinphi = sine( goal.phi ) ;
	
	return ;
}


void update_goal_alt( int z )
{
	goal.height = z ;
	return ;
}


void process_flightplan( void )
{
	if ( gps_nav_valid() && flags._.GPS_steering )
	{
		compute_bearing_to_goal() ;
		run_flightplan() ;
		compute_camera_view() ;
	}
	return ;
}


void compute_bearing_to_goal( void )
{
	union longww temporary ;
	union longww crossWind ;
	signed char desired_dir_temp ;
	signed char desired_bearing_over_ground ;
	
	// compute the goal vector from present position to waypoint target in meters:
	
#if ( DEADRECKONING == 1 )
	togoal.x = goal.x - IMUlocationx._.W1 ;
	togoal.y = goal.y - IMUlocationy._.W1 ;
#else
	togoal.x = goal.x - GPSlocation.x ;
	togoal.y = goal.y - GPSlocation.y ;
#endif
	
	// project the goal vector onto the direction vector between waypoints
	// to get the distance to the "finish" line:
	
	temporary.WW = (  __builtin_mulss( togoal.x , goal.cosphi )
					+ __builtin_mulss( togoal.y , goal.sinphi ))<<2 ;
	


	tofinish_line = temporary._.W1 ;
	
	
	if ( desired_behavior._.cross_track )
	{
		// If using Cross Tracking
		
#define CTDEADBAND 0
#define CTMARGIN 16
#define CTGAIN 2
// note: CTGAIN*(CTMARGIN-CTDEADBAND) should equal 32
	
		// project the goal vector perpendicular to the desired direction vector
		// to get the crosstrack error
		
		temporary.WW = ( __builtin_mulss( togoal.y , goal.cosphi )
					   - __builtin_mulss( togoal.x , goal.sinphi ))<<2 ;
	
		int crosstrack = temporary._.W1 ;
		
		// crosstrack is measured in meters
		// angles are measured as an 8 bit signed character, so 90 degrees is 64 binary.
		
		if ( abs(crosstrack) < ((int)(CTDEADBAND)))
		{
			desired_bearing_over_ground = goal.phi ;
		}
		else if ( abs(crosstrack) < ((int)(CTMARGIN)))
		{
			if ( crosstrack > 0 )
			{
				desired_bearing_over_ground = goal.phi + ( crosstrack - ((int)(CTDEADBAND)) ) * ((int)(CTGAIN)) ;
			}
			else
			{
				desired_bearing_over_ground = goal.phi + ( crosstrack + ((int)(CTDEADBAND)) ) * ((int)(CTGAIN)) ;
			}
		}
		else
		{
			if ( crosstrack > 0 )
			{
				desired_bearing_over_ground = goal.phi + 32 ; // 45 degrees maximum
			}
			else
			{
				desired_bearing_over_ground = goal.phi - 32 ; // 45 degrees maximum
			}
		}
		
		if ((estimatedWind[0] == 0 && estimatedWind[1] == 0) || air_speed_magnitudeXY < WIND_NAV_AIR_SPEED_MIN)
			// last clause keeps ground testing results same as in the past. Small and changing GPS speed on the ground,
			// combined with small wind_estimation will change calculated heading 4 times / second with result
			// that ailerons start moving 4 times / second on the ground. This clause prevents this happening when not flying.
			// Once flying, the GPS speed settles down to a larger figure, resulting in a smooth calculated heading.
		{
			desired_dir_temp = desired_bearing_over_ground ;
		}
		else
		{
			// account for the cross wind:
			// compute the wind component that is perpendicular to the desired bearing:
			crossWind.WW = ( __builtin_mulss( estimatedWind[0] , sine( desired_bearing_over_ground ))
									- __builtin_mulss( estimatedWind[1] , cosine( desired_bearing_over_ground )))<<2 ;
			if (  air_speed_magnitudeXY > abs(crossWind._.W1) )
			{
				// the correction to the bearing is the arcsine of the ratio of cross wind to air speed
				desired_dir_temp = desired_bearing_over_ground
				+ arcsine( __builtin_divsd ( crossWind.WW , air_speed_magnitudeXY )>>2 ) ;
			}
			else
			{
				desired_dir_temp = desired_bearing_over_ground ;
			}
		}
	
	}
	else {
		// If not using Cross Tracking
		
		if ((estimatedWind[0] == 0 && estimatedWind[1] == 0) || air_speed_magnitudeXY < WIND_NAV_AIR_SPEED_MIN)
			// last clause keeps ground testing results same as in the past. Small and changing GPS speed on the ground,
			// combined with small wind_estimation will change calculated heading 4 times / second with result
			// that ailerons start moving 4 times / second on the ground. This clause prevents this happening when not flying.
			// Once flying, the GPS speed settles down to a larger figure, resulting in a smooth calculated heading.
		{
			desired_dir_temp = rect_to_polar( &togoal ) ;
		}
		else
		{
			desired_bearing_over_ground = rect_to_polar( &togoal ) ;
			
			// account for the cross wind:
			// compute the wind component that is perpendicular to the desired bearing:
			crossWind.WW = ( __builtin_mulss( estimatedWind[0] , sine( desired_bearing_over_ground ))
									- __builtin_mulss( estimatedWind[1] , cosine( desired_bearing_over_ground )))<<2 ;
			if (  air_speed_magnitudeXY > abs(crossWind._.W1) )
			{
				// the correction to the bearing is the arcsine of the ratio of cross wind to air speed
				desired_dir_temp = desired_bearing_over_ground
				+ arcsine( __builtin_divsd ( crossWind.WW , air_speed_magnitudeXY )>>2 ) ;
			}
			else
			{
				desired_dir_temp = desired_bearing_over_ground ;
			}
		}
	}
	
	if ( flags._.GPS_steering )
	{
		desired_dir = desired_dir_temp ;
		
		if (goal.legDist > 0)
		{
			// progress_to_goal is the fraction of the distance from the start to the finish of
			// the current waypoint leg, that is still remaining.  it ranges from 0 - 1<<12.
			progress_to_goal = (((long)goal.legDist - tofinish_line + ground_velocity_magnitudeXY/100)<<12) / goal.legDist ;
			if (progress_to_goal < 0) progress_to_goal = 0 ;
			if (progress_to_goal > (long)1<<12) progress_to_goal = (long)1<<12 ;
		}
		else
		{
			progress_to_goal = (long)1<<12 ;
		}
	}
	else
	{
		if (current_orientation != F_HOVER)
		{
			desired_dir = calculated_heading ;
		}
	}
}

unsigned int wind_gain_adjustment( void )
{
#if ( WIND_GAIN_ADJUSTMENT == 1 )
	unsigned int horizontal_air_speed ;
	unsigned int horizontal_ground_speed_over_2 ;
	unsigned int G_over_2A ;
	unsigned int G_over_2A_sqr ;
	unsigned long temporary_long ;
	horizontal_air_speed = vector2_mag( IMUvelocityx._.W1 - estimatedWind[0] , 
										IMUvelocityy._.W1 - estimatedWind[1]) ;
	horizontal_ground_speed_over_2 = vector2_mag( IMUvelocityx._.W1  , 
										IMUvelocityy._.W1 ) >> 1;

	if ( horizontal_ground_speed_over_2 >= horizontal_air_speed )  
	{
		return 0xFFFF ;
	}
	else if ( horizontal_air_speed > 0 )
	{
		temporary_long = ((unsigned long ) horizontal_ground_speed_over_2 ) << 16 ;
		G_over_2A = __builtin_divud ( temporary_long , horizontal_air_speed ) ;
		temporary_long = __builtin_muluu ( G_over_2A , G_over_2A ) ;
		G_over_2A_sqr = temporary_long >> 16 ;
		if ( G_over_2A_sqr > 0x4000 )
		{
			return ( G_over_2A_sqr ) ;
		}
		else
		{
			return ( 0x4000 ) ;
		}
	}
	else
	{
		return 0x4000 ;
	}
#else
	return 0x4000;
#endif
}

// Values for navType:
// 'y' = yaw/rudder, 'a' = aileron/roll, 'h' = aileron/hovering
int determine_navigation_deflection(char navType)
{
	union longww deflectionAccum ;
	union longww dotprod ;
	union longww crossprod ;
	int desiredX ;
	int desiredY ;
	int actualX ;
	int actualY ;
	unsigned int yawkp ;
	
	if (navType == 'y')
	{
		yawkp =  yawkprud  ;
		actualX = rmat[1] ;
		actualY = rmat[4] ;
	}
	else if (navType == 'a')
	{
		yawkp =  yawkpail ;
		actualX = rmat[1] ;
		actualY = rmat[4] ;
	}
	else if (navType == 'h')
	{
		yawkp = yawkpail ;
		actualX = rmat[2] ;
		actualY = rmat[5] ;
	}
	else
	{
		return 0 ;
	}
	
#ifdef TestGains
	desiredX = -cosine ( (navType == 'y') ? 0 : 64 ) ;
	desiredY = sine ( (navType == 'y') ? 0 : 64 ) ;
#else
	desiredX = -cosine( desired_dir ) ;
	desiredY = sine( desired_dir ) ;
#endif
	
	dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
	crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
	crossprod.WW = crossprod.WW<<3 ; // at this point, we have 1/2 of the cross product
									// cannot go any higher than that, could get overflow
	if ( dotprod._.W1 > 0 )
	{
		deflectionAccum.WW = __builtin_mulsu( crossprod._.W1 , yawkp ) ;
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			deflectionAccum._.W1 = (yawkp/2) ;
		}
		else
		{
			deflectionAccum._.W1 = -(yawkp/2) ; // yawkp is unsigned, must divide and then negate
		}
	}
	
	if (navType == 'h') deflectionAccum.WW = -deflectionAccum.WW ;

	// multiply by wind gain adjustment, and multiply by 2
	deflectionAccum.WW = ( __builtin_mulsu ( deflectionAccum._.W1 , wind_gain )<<1 ) ; 
	return deflectionAccum._.W1 ;
}
