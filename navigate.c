#include "p30f4011.h"
#include "defines.h"
#include "definesRmat.h"

//	Compute actual and desired courses.
//	Actual course is simply the scaled GPS course over ground information.
//	Desired course is a "return home" course, which is simply the negative of the
//	angle of the vector from the origin to the location of the plane.

//	The origin is recorded as the location of the plane during power up of the control.

const int yawkpail = YAWKP_AILERON*RMAX ;
const int yawkprud = YAWKP_RUDDER*RMAX ;


int height = 0 ;
union longww heightlong = { 0 } ;

//signed char GPS_pitch = 0 ;

struct relative2D vector_to_origin ;

extern signed char desired_dir_waypoint ;


void navigate( void )
{
	union longbbbb accum_nav ;
	union longww accum_velocity ;
	
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
	
	accum_nav.WW = ((lat_gps.WW - lat_origin.WW)/90) ; // in meters, range is about 20 miles
	vector_to_origin.y = - accum_nav._.W0 ;
	GPSlocation.y = accum_nav._.W0 ;
	
	heightlong.WW = ( alt_sl_gps.WW - alt_origin.WW)/100 ; // height in meters
	height = heightlong._.W0 ;
	GPSlocation.z = height ;
	
	//	multiply the longitude delta by the cosine of the latitude
	accum_nav.WW = ((long_gps.WW - long_origin.WW)/90) ; // in meters
	accum_nav.WW = ((__builtin_mulss ( cos_lat , accum_nav._.W0 )<<2)) ;
	vector_to_origin.x = - accum_nav._.W1 ;
	GPSlocation.x = accum_nav._.W1 ;
	
	//	convert to polar to produce
	bearing_to_origin = rect_to_polar( &vector_to_origin ) ;
	
	estimateWind() ;
	
	return ;
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
	
	int yawkp = (navType == 'y') ? yawkprud : yawkpail ;
	
#ifdef TestGains
	desiredX = -cosine ( (navType == 'y') ? 0 : 64 ) ;
	desiredY = sine ( (navType == 'y') ? 0 : 64 ) ;
#else
	desiredX = -cosine( desired_dir ) ;
	desiredY = sine( desired_dir ) ;
#endif
	actualX = (navType == 'h') ? rmat[2] : rmat[1] ;
	actualY = (navType == 'h') ? rmat[5] : rmat[4] ;
	dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
	crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
	crossprod.WW = crossprod.WW<<2 ;
	if ( dotprod._.W1 > 0 )
	{
		deflectionAccum.WW = __builtin_mulss( crossprod._.W1 , yawkp ) ;
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			deflectionAccum._.W1 = yawkpail/4 ;
		}
		else
		{
			deflectionAccum._.W1 = -yawkpail/4 ;
		}
	}
	
	if (navType == 'h') deflectionAccum.WW = -deflectionAccum.WW ;
	
	return deflectionAccum._.W1 ;
}
