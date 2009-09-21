#include "p30f4011.h"
#include "defines.h"
#include "definesRmat.h"

//	Compute actual and desired courses.
//	Actual course is simply the scaled GPS course over ground information.
//	Desired course is a "return home" course, which is simply the negative of the
//	angle of the vector from the origin to the location of the plane.

//	The origin is recorded as the location of the plane during power up of the control.

int height = 0 ;
union longww heightlong = { 0 };

//signed char GPS_pitch = 0 ;

int velocity_magnitude = 0 ;
int forward_acceleration = 0 ;
int velocity_previous = 0 ;

extern signed char desired_dir_waypoint ;

void navigate(void)
{
	union longbbbb accum_nav ;
	union longww accum_velocity ;
	struct relative2D vector_to_origin ;
	signed char bearing_to_origin ;
	if ( flags._.save_origin )
	{
		//	capture origin information during power up. much of this is not actually used for anything,
		//	but is saved in case you decide to extend this code.
		flags._.save_origin = 0 ;
		lat_origin = lat_gps ;
		long_origin = long_gps ;
		alt_origin = alt_sl_gps ;
		x_origin = xpg ;
		y_origin = ypg ;
		z_origin = zpg ;
		//	scale the latitude from GPS units to gentleNAV units
		accum_nav.WW = __builtin_mulss( LONGDEG_2_BYTECIR , lat_origin._.W1 ) ;
		lat_cir = accum_nav.__.B2 ;
		//	estimate the cosine of the latitude, which is used later computing desired course
		cos_lat = cosine ( lat_cir ) ;
	}

	//	Subtract the origin latitude, longitude, and altitude from present lat, long, alt.
	//	Then flip the sign.
	//	(Yes, it would have been simpler to subtract present from the origin!)
	

	accum_nav.WW = ((lat_gps.WW - lat_origin.WW)/90) ;  // in meters
	vector_to_origin.y = - accum_nav._.W0 ;
	GPSlocation.y = accum_nav._.W0 ;

	heightlong.WW = ( alt_sl_gps.WW - alt_origin.WW)/100 ; // height in meters
	height = heightlong._.W0 ;
	GPSlocation.z = height ;

	//	multiply the longitude delta by the cosine of the latitude
	accum_nav.WW = ((long_gps.WW - long_origin.WW)/90) ;  // in meters
	accum_nav.WW = ((__builtin_mulss ( cos_lat , accum_nav._.W0 )<<2)) ;
	vector_to_origin.x = - accum_nav._.W1 ;
	GPSlocation.x = accum_nav._.W1 ;

	//	convert to polar to produce
	bearing_to_origin = rect_to_polar( &vector_to_origin ) ;
	if ( flags._.use_waypoints == 1 )
	{
		desired_dir = desired_dir_waypoint ;
	}
	else
	{
		desired_dir = bearing_to_origin ;
	}

	//	convert course over ground from CW GPS units to mathematical CCW units
	accum_nav.WW = __builtin_mulss ( COURSEDEG_2_BYTECIR , cog_gps.BB ) ;
	actual_dir = -accum_nav.__.B2 + 64 ;

	velocity_magnitude = sog_gps.BB ;
	forward_acceleration = velocity_magnitude - velocity_previous ;
	velocity_previous = velocity_magnitude ;

	accum_velocity.WW = __builtin_mulss( cosine( actual_dir ) , velocity_magnitude) << 2 ;
	GPSvelocity.x = accum_velocity._.W1 ;

	accum_velocity.WW = __builtin_mulss( sine( actual_dir ) , velocity_magnitude) << 2 ;
	GPSvelocity.y = accum_velocity._.W1 ;

	GPSvelocity.z = climb_gps.BB ;

	return ;
}
