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


#include "../matrixpilot/defines.h"
#include "../libUDB/libUDB.h"
#include "navigateFBW.h"

//	Compute actual and desired courses.
//	Actual course is simply the scaled GPS course over ground information.
//	Desired course is a "return home" course, which is simply the negative of the
//	angle of the vector from the origin to the location of the plane.



#define DEFAULT_LOITER_RADIUS 80

unsigned int16_t loiter_radius = DEFAULT_LOITER_RADIUS;

struct waypointparameters goal ;
struct relative2D togoal = { 0 , 0 } ;
int tofinish_line  = 0 ;
int progress_to_goal = 0 ;

// Legacy desired direction only used for reporting to telemetry
signed char desired_dir = 0;

_Q16 desired_dir_q16 = 0;

fractional nav_pitch_gain = 0;
fractional nav_roll_gain = 0;
fractional nav_yaw_gain = 0;

struct relative2D nav_actual_heading;
struct relative2D compute_actual_heading( void );

int desiredHeight ;

inline fractional get_pitch_gain( void ) {return nav_pitch_gain;}
inline fractional get_yaw_gain( void ) {return nav_yaw_gain;}
inline fractional get_roll_gain( void ) {return nav_roll_gain;}
inline struct relative2D get_actual_heading( void ) {return nav_actual_heading;};

inline void setTargetAltitude(int16_t targetAlt)
{
	desiredHeight = targetAlt ;
	return ;
}

// Get the desired altitude for guided mode only.
inline int32_t get_guided_desired_altitude(void);

void setup_origin(void)
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


void update_goal_alt( int16_t z )
{
	goal.height = z ;
	return ;
}



void process_flightplan( void )
{
	switch( get_flightmode())
	{
	case FLIGHT_MODE_STABILIZED:
	case FLIGHT_MODE_MANUAL:
	case FLIGHT_MODE_ASSISTED: 
		break;
	case FLIGHT_MODE_NO_RADIO: 
	case FLIGHT_MODE_AUTONOMOUS:
		if ( gps_nav_valid() )
		{
			nav_actual_heading = compute_actual_heading();
			compute_bearing_to_goal() ;
			run_flightplan() ;
			compute_camera_view() ;
		};
		break;
	}
	return ;
}

void calc_navigation_gains( void )
{
	union longww temp;
	// Pitch gain which is RMAX*sin(pitch)^2
	temp.WW = __builtin_mulss( rmat[7] , rmat[7] ) << 2;
	nav_pitch_gain = temp._.W1;

	// roll gain which is RMAX*sin(roll)^2
	temp.WW = __builtin_mulss( rmat[6] , rmat[6] ) << 2;
	nav_roll_gain = temp._.W1;

	// Yaw gain which is (2*RMAX*cos(pitch)*sin(pitch))^2
	temp.WW = __builtin_mulss( rmat[7] , rmat[8] ) << 3;
	temp.WW = __builtin_mulss( temp._.W1, temp._.W1) << 2;
	nav_yaw_gain = temp._.W1;
}




struct relative2D compute_actual_heading( void )
{
	struct relative2D heading;
	union longww temp;

        calc_navigation_gains( );
	fractional pitch_inv_gain = RMAX - nav_pitch_gain;

	temp.WW = __builtin_mulss( rmat[1] , pitch_inv_gain ) << 2;	// actualX normal
	temp.WW += __builtin_mulss( rmat[2] , nav_pitch_gain ) << 2;	// actualX normal
	heading.x = temp._.W1;

	temp.WW = __builtin_mulss( rmat[4] , pitch_inv_gain ) << 2; ; // actualY normal
	temp.WW += __builtin_mulss( rmat[5] , nav_pitch_gain ) << 2; ; // actualY normal
	heading.y = temp._.W1;

	return heading;
}


void compute_bearing_to_goal(void )
{
	union longww temporary ;
	union longww crossWind ;
	_Q16 desired_dir_temp ;
	
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

	// Distance to the waypoint
	temporary.WW = (  __builtin_mulss( togoal.x , togoal.x )) ;
    temporary.WW += (  __builtin_mulss( togoal.y , togoal.y )) ;
	
	int16_t waypoint_dist = (uint16_t)sqrt_long( (uint32_t) temporary.WW);      

    _Q16 radius_angle = 57;		// 90 degrees
		
    if(loiter_radius < waypoint_dist )
    {
        // radius_angle = 65536 * loiter_radius / distance to radius
        temporary._.W1 = __builtin_divsd( RMAX , waypoint_dist);
        temporary.WW = __builtin_mulss( temporary._.W1 , loiter_radius ) << 2;
		radius_angle = _Q16asin( temporary.WW );
	}
	else
		radius_angle = 102944; 		//_Q16asin(Q16PI / 2);


	struct relative2D tempgoal = togoal ;
	_Q16 goal_angle = rect_to_polar16( &tempgoal );
	goal_angle <<= 2;

// 		tempgoal = get_actual_heading()
//        tempgoal.x = rmat[1] ;
//        tempgoal.y = rmat[4] ;
//        temporary._.W0 = rect_to_polar(&tempgoal);
//

// Dot cross product and +- decision here.

	desired_dir_temp = goal_angle + radius_angle;
//
//		temporary._.W0 = 0;
//		tempgoal = togoal ;
//		desired_dir_temp = rect_to_polar( &togoal ) ;

//		desired_dir_temp = 0;

//	if ( desired_behavior._.cross_track )
//	{
//		// If using Cross Tracking
//
//#define CTDEADBAND 0
//#define CTMARGIN 16
//#define CTGAIN 2
//// note: CTGAIN*(CTMARGIN-CTDEADBAND) should equal 32
//
//		// project the goal vector perpendicular to the desired direction vector
//		// to get the crosstrack error
//
//		temporary.WW = ( __builtin_mulss( togoal.y , goal.cosphi )
//					   - __builtin_mulss( togoal.x , goal.sinphi ))<<2 ;
//
//		int16_t crosstrack = temporary._.W1 ;
//
//		// crosstrack is measured in meters
//		// angles are measured as an 8 bit signed character, so 90 degrees is 64 binary.
//
//		if ( abs(crosstrack) < ((int16_t)(CTDEADBAND)))
//		{
//			desired_bearing_over_ground = goal.phi ;
//		}
//		else if ( abs(crosstrack) < ((int16_t)(CTMARGIN)))
//		{
//			if ( crosstrack > 0 )
//			{
//				desired_bearing_over_ground = goal.phi + ( crosstrack - ((int16_t)(CTDEADBAND)) ) * ((int16_t)(CTGAIN)) ;
//			}
//			else
//			{
//				desired_bearing_over_ground = goal.phi + ( crosstrack + ((int16_t)(CTDEADBAND)) ) * ((int16_t)(CTGAIN)) ;
//			}
//		}
//		else
//		{
//			if ( crosstrack > 0 )
//			{
//				desired_bearing_over_ground = goal.phi + 32 ; // 45 degrees maximum
//			}
//			else
//			{
//				desired_bearing_over_ground = goal.phi - 32 ; // 45 degrees maximum
//			}
//		}
//
//		if ((estimatedWind[0] == 0 && estimatedWind[1] == 0) || air_speed_magnitudeXY < WIND_NAV_AIR_SPEED_MIN)
//			// last clause keeps ground testing results same as in the past. Small and changing GPS speed on the ground,
//			// combined with small wind_estimation will change calculated heading 4 times / second with result
//			// that ailerons start moving 4 times / second on the ground. This clause prevents this happening when not flying.
//			// Once flying, the GPS speed settles down to a larger figure, resulting in a smooth calculated heading.
//		{
//			desired_dir_temp = desired_bearing_over_ground ;
//		}
//		else
//		{
//			// account for the cross wind:
//			// compute the wind component that is perpendicular to the desired bearing:
//			crossWind.WW = ( __builtin_mulss( estimatedWind[0] , sine( desired_bearing_over_ground ))
//									- __builtin_mulss( estimatedWind[1] , cosine( desired_bearing_over_ground )))<<2 ;
//			if (  air_speed_magnitudeXY > abs(crossWind._.W1) )
//			{
//				// the correction to the bearing is the arcsine of the ratio of cross wind to air speed
//				desired_dir_temp = desired_bearing_over_ground
//				+ arcsine( __builtin_divsd ( crossWind.WW , air_speed_magnitudeXY )>>2 ) ;
//			}
//			else
//			{
//				desired_dir_temp = desired_bearing_over_ground ;
//			}
//		}
//
//	}
//	else {
//		// If not using Cross Tracking
//
//		if ((estimatedWind[0] == 0 && estimatedWind[1] == 0) || air_speed_magnitudeXY < WIND_NAV_AIR_SPEED_MIN)
//			// last clause keeps ground testing results same as in the past. Small and changing GPS speed on the ground,
//			// combined with small wind_estimation will change calculated heading 4 times / second with result
//			// that ailerons start moving 4 times / second on the ground. This clause prevents this happening when not flying.
//			// Once flying, the GPS speed settles down to a larger figure, resulting in a smooth calculated heading.
//		{
//			desired_dir_temp = rect_to_polar( &togoal ) ;
//		}
//		else
//		{
//			desired_bearing_over_ground = rect_to_polar( &togoal ) ;
//
//			// account for the cross wind:
//			// compute the wind component that is perpendicular to the desired bearing:
//			crossWind.WW = ( __builtin_mulss( estimatedWind[0] , sine( desired_bearing_over_ground ))
//									- __builtin_mulss( estimatedWind[1] , cosine( desired_bearing_over_ground )))<<2 ;
//			if (  air_speed_magnitudeXY > abs(crossWind._.W1) )
//			{
//				// the correction to the bearing is the arcsine of the ratio of cross wind to air speed
//				desired_dir_temp = desired_bearing_over_ground
//				+ arcsine( __builtin_divsd ( crossWind.WW , air_speed_magnitudeXY )>>2 ) ;
//			}
//			else
//			{
//				desired_dir_temp = desired_bearing_over_ground ;
//			}
//		}
////	}


	if(mode_autopilot_enabled())
	{
		desired_dir_q16 = desired_dir_temp ;
		
		if (goal.legDist > 0)
		{
			// progress_to_goal is the fraction of the distance from the start to the finish of
			// the current waypoint leg, that is still remaining.  it ranges from 0 - 1<<12.
			progress_to_goal = (((int32_t)goal.legDist - tofinish_line + ground_velocity_magnitudeXY/100)<<12) / goal.legDist ;
			if (progress_to_goal < 0) progress_to_goal = 0 ;
			if (progress_to_goal > (int32_t)1<<12) progress_to_goal = (int32_t)1<<12 ;
		}
		else
		{
			progress_to_goal = (int32_t)1<<12 ;
		}
	}
	else
	{
		if (current_orientation != F_HOVER)
		{
			desired_dir_q16 = calculated_heading ;
		}
	}

}

unsigned int16_t wind_gain_adjustment( void )
{
#if ( WIND_GAIN_ADJUSTMENT == 1 )
	uint16_t horizontal_air_speed ;
	uint16_t horizontal_ground_speed_over_2 ;
	uint16_t G_over_2A ;
	uint16_t G_over_2A_sqr ;
	uint32_t temporary_long ;
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
		temporary_long = ((uint32_t ) horizontal_ground_speed_over_2 ) << 16 ;
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


inline int32_t get_guided_desired_altitude(void)
{
	if ( desired_behavior._.takeoff || desired_behavior._.altitude )
	{
		return goal.height ;
	}
	else
	{
		return ( goal.fromHeight + (((goal.height - goal.fromHeight) * (int32_t)progress_to_goal)>>12) ) ;
	}
}
