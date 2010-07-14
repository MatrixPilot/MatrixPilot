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
#include "definesRmat.h"
#include "defines.h"

struct waypoint3D GPSlocation 		  = { 0 , 0 , 0 } ;
struct waypoint3D GPSvelocity 		  = { 0 , 0 , 0 } ;
struct waypoint3D view_location       = { 0 , 0 , 0 } ; 
struct relative2D velocity_thru_air   = { 0 , 0 } ;

signed char calculated_heading ; //calculated heading allows for wind velocity
		
#include "waypoints.h"

#define NUMBER_POINTS (( sizeof waypoints ) / sizeof ( struct waypointDef ))
#define NUMBER_RTL_POINTS (( sizeof rtlWaypoints ) / sizeof ( struct waypointDef ))

int waypointIndex = 0 ;							
struct waypointparameters goal ;
struct relative2D togoal = { 0 , 0 } ;
int tofinish_line  = 0 ;
int progress_to_goal = 0 ;
int crosstrack = 0 ;
signed char desired_dir_waypoint = 0 ;
signed char desired_bearing_over_ground  = 0 ;


struct waypointDef *currentWaypointSet = (struct waypointDef*)waypoints ;
int numPointsInCurrentSet = NUMBER_POINTS ;


// For a relative waypoint, wp_to_relative() just passes the relative
// waypoint location through unchanged.
// For an absolute waypoint, wp_to_relative() converts the waypoint's
// location from absolute to relative.
struct waypointDef wp_to_relative(struct waypointDef wp)
{
	if ( wp.flags & F_ABSOLUTE )
	{
		union longww accum_nav ;
		
		wp.loc.y = (wp.loc.y - lat_origin.WW)/90 ; // in meters
		
		accum_nav.WW = ((wp.loc.x - long_origin.WW)/90) ; // in meters
		accum_nav.WW = ((__builtin_mulss ( cos_lat , accum_nav._.W0 )<<2)) ;
		wp.loc.x = accum_nav._.W1 ;

		wp.viewpoint.y = (wp.viewpoint.y - lat_origin.WW)/90 ; // in meters
		
		accum_nav.WW = ((wp.viewpoint.x - long_origin.WW)/90) ; // in meters
		accum_nav.WW = ((__builtin_mulss ( cos_lat , accum_nav._.W0 )<<2)) ;
		wp.viewpoint.x = accum_nav._.W1 ;

		wp.flags -= F_ABSOLUTE ;
	}
	return wp;
}


void setup_origin_2D_location( void )
{
#if ( USE_FIXED_ORIGIN == 1 )
		struct waypoint2D origin = FIXED_ORIGIN_LOCATION ;
		lat_origin.WW = origin.y ;
		long_origin.WW = origin.x ;
#else
		lat_origin.WW = lat_gps.WW ;
		long_origin.WW = long_gps.WW ;
#endif
}


void set_goal( struct waypoint3D fromPoint , struct waypoint3D toPoint )
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


void set_camera_view( struct waypoint3D current_view )
{
	view_location.x = current_view.x ;
	view_location.y = current_view.y ;
	view_location.z = current_view.z ;
}


// In the future, we could include more than 2 waypoint sets...
void init_waypoints ( int waypointSetIndex )
{
	if ( waypointSetIndex == 1 ) // RTL waypoint set
	{
		currentWaypointSet = (struct waypointDef*)rtlWaypoints ;
		numPointsInCurrentSet = NUMBER_RTL_POINTS ;
	}
	else if ( waypointSetIndex == 0 ) // Main waypoint set
	{
		currentWaypointSet = (struct waypointDef*)waypoints ;
    	numPointsInCurrentSet = NUMBER_POINTS ;
    }
	
	waypointIndex = 0 ;
	struct waypointDef current_waypoint = wp_to_relative(currentWaypointSet[0]) ;
	set_goal( GPSlocation , current_waypoint.loc ) ;
	set_camera_view(current_waypoint.viewpoint) ;
	setBehavior(current_waypoint.flags) ;
	
	// IFS0bits.T3IF = 1 ;			// trigger navigation immediately
	
	return ;
}


void compute_camera_view (void)
{

#if ( DEADRECKONING == 1 )
	camera_view.x = view_location.x - IMUlocationx._.W1 ;
	camera_view.y = view_location.y - IMUlocationy._.W1 ;
	camera_view.z = view_location.z - IMUlocationz._.W1 ;
#else
	camera_view.x = view_location.x - GPSlocation.x ;
	camera_view.y = view_location.y - GPSlocation.y ;
	camera_view.z = view_location.z - GPSlocation.z ;
#endif

	return ;
}


void compute_waypoint ( void )
{
	union longww temporary ;
	union longww crossWind ;
	// compute the goal vector from present position to waypoint target in meters:
	
#if ( DEADRECKONING == 1 )	
	togoal.x =  goal.x  - IMUlocationx._.W1  ;
	togoal.y =  goal.y  - IMUlocationy._.W1  ;
#else
	togoal.x =  goal.x  - GPSlocation.x  ;
	togoal.y =  goal.y  - GPSlocation.y  ;
#endif
	
	// project the goal vector onto the direction vector between waypoints
	// to get the distance to the "finish" line:
	
	temporary.WW = (  __builtin_mulss( togoal.x , goal.cosphi )
					+ __builtin_mulss( togoal.y , goal.sinphi ))<<2 ;
	
	tofinish_line = temporary._.W1 ;


	
#if ( USE_CROSSTRACKING == 1 )
#define CTDEADBAND 0
#define CTMARGIN 32
#define CTGAIN 1
#define MAXCROSSANGLE 32
// note: CTGAIN*(CTMARGIN-CTDEADBAND) should equal 32

	temporary.WW = ( __builtin_mulss( togoal.y , goal.cosphi )
				   - __builtin_mulss( togoal.x , goal.sinphi ))<<2 ;
	
	crosstrack = temporary._.W1 ;
		
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
			desired_bearing_over_ground = goal.phi + ( crosstrack - ((int)(CTDEADBAND) )) * ((int) (CTGAIN)) ;
		}
		else
		{
			desired_bearing_over_ground = goal.phi + ( crosstrack + ((int)(CTDEADBAND) )) * ((int) (CTGAIN)) ;
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

	if ((estimatedWind[0] == 0 && estimatedWind[1] == 0) || air_speed_magnitude < WIND_NAV_AIR_SPEED_MIN   )
		// clause keeps ground testing results same as in the past. Small and changing GPS speed on the ground,
		// combined with small wind_estimation will change calculated heading 4 times / second with result
		// that ailerons start moving 4 times / second on the ground. This clause prevents this happening when not flying.
		// Once flying, the GPS speed settles down to a larger figure, resulting in a smooth calculated heading.
		{
			desired_dir_waypoint = desired_bearing_over_ground ;
		}
		else
		{
			// account for the cross wind:
			// compute the wind component that is perpendicular to the desired bearing:
			crossWind.WW = ( __builtin_mulss( estimatedWind[0] , sine( desired_bearing_over_ground ))
						- __builtin_mulss( estimatedWind[1] , cosine( desired_bearing_over_ground )))<<2 ;
			if (  air_speed_magnitude > abs(crossWind._.W1) )
			{
				// the correction to the bearing is the arcsine of the ratio of cross wind to air speed
				desired_dir_waypoint = desired_bearing_over_ground
				+ arcsine( __builtin_divsd ( crossWind.WW , air_speed_magnitude )>>2 ) ;
			}
			else
			{
				desired_dir_waypoint = desired_bearing_over_ground ;
			}

		}
	
#else
	
	if ((estimatedWind[0] == 0 && estimatedWind[1] == 0) || air_speed_magnitude < WIND_NAV_AIR_SPEED_MIN   )
	// clause keeps ground testing results same as in the past. Small and changing GPS speed on the ground,
	// combined with small wind_estimation will change calculated heading 4 times / second with result
	// that ailerons start moving 4 times / second on the ground. This clause prevents this happening when not flying.
	// Once flying, the GPS speed settles down to a larger figure, resulting in a smooth calculated heading.
	{
		desired_dir_waypoint = rect_to_polar ( & togoal ) ;
	}
	else
	{
		desired_bearing_over_ground = rect_to_polar( &togoal) ;
		
		// account for the cross wind:
		// compute the wind component that is perpendicular to the desired bearing:
		crossWind.WW = ( __builtin_mulss( estimatedWind[0] , sine( desired_bearing_over_ground ))
					- __builtin_mulss( estimatedWind[1] , cosine( desired_bearing_over_ground )))<<2 ;
		if (  air_speed_magnitude > abs(crossWind._.W1) )
		{
			// the correction to the bearing is the arcsine of the ratio of cross wind to air speed
			desired_dir_waypoint = desired_bearing_over_ground
			+ arcsine( __builtin_divsd ( crossWind.WW , air_speed_magnitude )>>2 ) ;
		}
		else
		{
			desired_dir_waypoint = desired_bearing_over_ground ;
		}
	}
#endif
}


void next_waypoint ( void ) 
{
	waypointIndex++ ;
	
	if ( waypointIndex >= numPointsInCurrentSet ) waypointIndex = 0 ;
	
	if ( waypointIndex == 0 )
	{
		if (numPointsInCurrentSet > 1)
		{
			struct waypointDef previous_waypoint = wp_to_relative( currentWaypointSet[numPointsInCurrentSet-1] ) ;
			struct waypointDef current_waypoint  = wp_to_relative( currentWaypointSet[0] ) ;
			set_goal( previous_waypoint.loc, current_waypoint.loc ) ;
			set_camera_view( current_waypoint.viewpoint ) ;

		}
		else
		{
			struct waypointDef current_waypoint = wp_to_relative( currentWaypointSet[0] ) ;
			set_goal( GPSlocation, current_waypoint.loc ) ;
			set_camera_view( current_waypoint.viewpoint ) ;
		}
		setBehavior( currentWaypointSet[0].flags ) ;
	}
	else
	{
		struct waypointDef previous_waypoint = wp_to_relative( currentWaypointSet[waypointIndex-1] ) ;
		struct waypointDef current_waypoint = wp_to_relative( currentWaypointSet[waypointIndex] ) ;
		set_goal( previous_waypoint.loc, current_waypoint.loc ) ;
		set_camera_view( current_waypoint.viewpoint ) ;
		setBehavior( current_waypoint.flags ) ;
	}

#if	( DEADRECKONING == 0 )
	compute_waypoint() ;
	compute_camera_view() ;
#endif
	return ;
}


void processwaypoints(void)
{
	if ( gps_nav_valid() && flags._.GPS_steering )
	{
		// steering is based on cross track error.
	 	// waypoint arrival is detected computing distance to the "finish line".
		
		// note: locations are measured in meters
		//		 velocities are in centimeters per second
		
		// locations have a range of +-32000 meters (20 miles) from origin
		
		compute_waypoint() ;
		compute_camera_view() ;
		
		if ( desired_behavior._.altitude )
		{	
			if ( abs(IMUheight - goal.height) < ((int) HEIGHT_MARGIN ))
				next_waypoint() ;
		}
		else
		{
#if ( USE_CROSSTRACKING == 1 )
			if ( tofinish_line < WAYPOINT_RADIUS ) // crossed the finish line
			{
				if ( desired_behavior._.loiter )
					set_goal( GPSlocation, wp_to_relative(currentWaypointSet[waypointIndex]).loc ) ;
				else
					next_waypoint() ;
			}
#else
			if ( (tofinish_line < WAYPOINT_RADIUS) || (togoal.x < WAYPOINT_RADIUS) ) // crossed the finish line
			{
				if ( desired_behavior._.loiter )
					set_goal( GPSlocation, wp_to_relative(currentWaypointSet[waypointIndex]).loc ) ;
				else
					next_waypoint() ;
			}
#endif
		}
	}
	
	if ( flags._.GPS_steering )
	{
		desired_dir = desired_dir_waypoint ;
		
		if (goal.legDist > 0)
		{
			// progress_to_goal is the fraction of the distance from the start to the finish of
			// the current waypoint leg, that is still remaining.  it ranges from 0 - 1<<12.
			progress_to_goal = (((long)goal.legDist - tofinish_line + velocity_magnitude/100)<<12) / goal.legDist ;
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
		desired_dir = calculated_heading ;
	}
	
	return ;
}

