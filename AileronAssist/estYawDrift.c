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

fractional rmat1filt = 0 ;
fractional rmat4filt = RMAX ;

long previous_lat ;
long previous_long ;
signed char compute_cog(void)
{
	int delta_lat ;
	int delta_long ;
	union longbbbb temporary ;
	struct xypair delta_gps ;
	delta_lat = (int)(lat_gps.WW - previous_lat) ;
	delta_long = (int)(long_gps.WW - previous_long) ;
	previous_lat = lat_gps.WW ;
	previous_long = long_gps.WW ;
	temporary.WW = ((__builtin_mulss ( cos_lat , delta_long)<<2 )) ;
	delta_long = temporary._.W1 ;
	delta_gps.x = delta_long ;
	delta_gps.y = delta_lat ;
	return rect_to_polar( & delta_gps );
}

signed char computed_cog ;

#define GPSTAU 12.0

#define GPSFILT (4.0/GPSTAU)*RMAX


void estYawDrift(void)
{
	union longbbbb accum ;
	accum.WW = __builtin_mulss ( COURSEDEG_2_BYTECIR , cog_gps.BB ) ;
	actual_dir = -accum.__.B2 + 64 ;
	computed_cog = compute_cog() ;

	accum.WW = __builtin_mulss( GPSFILT , (rmat[1] - rmat1filt )) ;
	rmat1filt = rmat1filt + accum._.W1 ;
	accum.WW = __builtin_mulss( GPSFILT , (rmat[4] - rmat4filt )) ;
	rmat4filt = rmat4filt + accum._.W1 ;

	dirovergndHRmat[0] = rmat1filt ;
	dirovergndHRmat[1] = rmat4filt ;
	dirovergndHRmat[2] = 0 ;

	if ( nav_valid_.BB == 0 )
	{
		dirovergndHGPS[0] = -cosine (computed_cog ) ;
		dirovergndHGPS[1] = sine ( computed_cog ) ;
	}
	else
	{
		dirovergndHGPS[0] = rmat1filt ;
		dirovergndHGPS[1] = rmat4filt ;
	}
	dirovergndHGPS[2] = 0 ;
	flags._.yaw_req = 1 ;
	return ;
}
