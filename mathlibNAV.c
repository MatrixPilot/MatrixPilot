#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	math libraray

//	sine table for angles from zero to pi/2 with an increment of pi/128 radian.
//  sine values are multiplied by 2**14
const int sintab[] =	
	{0,	402,	804,	1205,	1606,	2006,	2404,	2801,	3196,	3590,	3981,
		4370,	4756,	5139,	5520,	5897,	6270,	6639,	7005,	7366,	7723,	
		8076,	8423,	8765,	9102,	9434,	9760,	10080,	10394,	10702,	11003,	
		11297,	11585,	11866,	12140,	12406,	12665,	12916,	13160,	13395,	13623,	
		13842,	14053,	14256,	14449,	14635,	14811,	14978,	15137,	15286,	15426,	
		15557,	15679,	15791,	15893,	15986,	16069,	16143,	16207,	16261,	16305,	
		16340,	16364,	16379,	16384}	;

int sine ( signed char angle )
//	returns (2**14)*sine(angle), angle measured in units of pi/128 ratians
{
	int angle_int ;
	angle_int = angle ;
	if ( angle_int >= 0 )
	{
		if ( angle_int > 64 )
		{
			return( sintab[128-angle_int] ) ;
		}
		else
		{
			return( sintab[angle_int] ) ;
		}
	}
	else
	{
		angle_int = - angle_int ;
		if ( angle_int > 64 )
		{
			return( -sintab[128-angle_int] ) ;
		}
		else
		{
			return( -sintab[angle_int]) ;
		}
	}
}

signed char arcsine ( int y )
// returns the inverse sine of y
// y is in Q2.14 format, 16384 is maximum value
// returned angle is a byte circular
{
	signed char angle = 32 ;
	signed char doubleangle = 64 ;
	signed char step = 32 ;
	signed char sign ;
	if ( y > 0 )
	{
		sign = 1 ;
	}
	else
	{
		sign = - 1 ;
		y = - y ;
	}
	if ( y == 16384 )
	{
		return sign*64 ;
	}
	while ( step > 0 )
	{
		angle = doubleangle>>1 ;
		if ( y == sine( angle ) )
		{
			return sign*angle ;
		}
		else if ( y >  (( sine( angle )+ sine( angle - 1 ))>>1 ) )
		{
			doubleangle += step ;
		}
		else
		{
			doubleangle -= step ;
		}
		step = step>>1 ;
	}
	return sign*(doubleangle>>1) ;
}

int cosine ( signed char angle )
{
	return ( sine ( angle+64 ) ) ;
}

void rotate( struct relative2D *xy , signed char angle )
{
	//	rotates xy by angle, measured in a counter clockwise sense.
	//	A mathematical angle of plus or minus pi is represented digitally as plus or minus 128.
	int cosang , sinang , newx , newy ;
	union longww accum ;
	sinang = sine( angle ) ;
	cosang = cosine( angle ) ;
	accum.WW = ((__builtin_mulss( cosang , xy->x) - __builtin_mulss( sinang , xy->y ))<<2) ;
	newx = accum._.W1 ;
	accum.WW = ((__builtin_mulss( sinang , xy->x) + __builtin_mulss( cosang , xy->y ))<<2) ;
	newy = accum._.W1 ;
	xy->x = newx ;
	xy->y = newy ;
	return ;	
}

signed char rect_to_polar ( struct relative2D *xy )
{
	//	Convert from rectangular to polar coordinates using "CORDIC" arithmetic, which is basically
	//	a binary search for the angle.
	//	As a by product, the xy is rotated onto the x axis, so that y is driven to zero,
	//	and the magnitude of the vector winds up as the x component.

	signed char theta = 0 ;
	signed char delta_theta = 64 ;
	signed char theta_rot ;
	signed char steps = 7 ;
	int scaleShift ;

	if ( 	( ( xy-> x ) < 255 ) && 
			( ( xy-> x ) > -255 ) && 
			( ( xy-> y ) < 255 ) && 
			( ( xy-> y ) > -255 )) 
	{
		scaleShift = 6 ;
		xy->x = ( xy->x << 6 ) ;
		xy->y = ( xy->y << 6 ) ;
	}
	else
	{
		scaleShift = 0 ;
	}

	while ( steps > 0 )
	{
		theta_rot = delta_theta ;
		if ( xy->y  > 0 ) theta_rot = -theta_rot ;
		rotate ( xy , theta_rot ) ;
		theta += theta_rot ;
		delta_theta = (delta_theta>>1) ;
		steps--;
	}
	if ( xy->y > 0 ) theta-- ;

	xy->x = ( xy->x >> scaleShift ) ;
	xy->y = ( xy->y >> scaleShift ) ;

	return (-theta ) ;
}

#define RADIANTOCIRCULAR 10430 

int rect_to_polar16 ( struct relative2D *xy )
{
	//	Convert from rectangular to polar coordinates using "CORDIC" arithmetic, which is basically
	//	a binary search for the angle.
	//	As a by product, the xy is rotated onto the x axis, so that y is driven to zero,
	//	and the magnitude of the vector winds up as the x component.
	//  Returns a value as a 16 bit "circular" so that 180 degrees yields 2**15
	int scaleShift ;
	int theta16 ;
	signed char theta = 0 ;
	signed char delta_theta = 64 ;
	signed char theta_rot ;
	signed char steps = 7 ;

	if ( 	( ( xy-> x ) < 255 ) && 
			( ( xy-> x ) > -255 ) && 
			( ( xy-> y ) < 255 ) && 
			( ( xy-> y ) > -255 )) 
	{
		scaleShift = 6 ;
		xy->x = ( xy->x << 6 ) ;
		xy->y = ( xy->y << 6 ) ;
	}
	else
	{
		scaleShift = 0 ;
	}

	while ( steps > 0 )
	{
		theta_rot = delta_theta ;
		if ( xy->y  > 0 ) theta_rot = -theta_rot ;
		rotate ( xy , theta_rot ) ;
		theta += theta_rot ;
		delta_theta = (delta_theta>>1) ;
		steps--;
	}
	theta = -theta ;
	theta16 = theta<<8 ;

	if ( xy->x > 0 ) 
	{
		theta16 += __builtin_divsd ( __builtin_mulss( RADIANTOCIRCULAR , xy->y ) , xy->x ) ;
	}

	xy->x = ( xy->x >> scaleShift ) ;
	xy->y = ( xy->y >> scaleShift ) ;

	return ( theta16 ) ;
}
