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


#include "libDCM_internal.h"

//	math libraray

#define RADIANTOCIRCULAR 10430 


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

unsigned int sqrt_int( unsigned int sqr )
{
	// based on Heron's algorithm
	unsigned int binary_point = 0 ;
	unsigned int result = 255 ; 
							
	int iterations = 3 ;		
	if ( sqr == 0 )
	{
		return 0 ;
	}
	while ( ( sqr & 0xC000 ) == 0 ) // shift left to get a 1 in the 2 MSbits
	{
		sqr = sqr*4 ; // shift 2 bits
		binary_point ++ ; // track half of the number of bits shifted
	}
	sqr = sqr/2 ; // for convenience, Herons formula is result = ( result + sqr/result ) / 2
	while ( iterations )
	{
		iterations -- ;
		result = result/2 + sqr/result ;
	}
	result = result >> binary_point ; // shift result right to account for shift left of sqr 
	return result ;
}

unsigned int sqrt_long( unsigned long int sqr )
{
	// based on Heron's algorithm
	unsigned int binary_point = 0 ;
	unsigned int result = 65535 ; // need to start high and work down to avoid overflow in divud

	int iterations = 3 ;	// thats all you need

	if ( sqr < 65536 )	// use the 16 bit square root
	{
		return sqrt_int( ( unsigned int ) sqr ) ;
	}
	while ( ( sqr & 0xC0000000 ) == 0 ) // shift left to get a 1 in the 2 MSbits
	{
		sqr = sqr<< 2 ;
		binary_point ++ ; // track half of the number of bits shifted
	}
	sqr = sqr>>1 ; // for convenience, Herons formula is result = ( result + sqr/result ) / 2
	while ( iterations )
	{
		iterations -- ;
		result = result/2 + __builtin_divud ( sqr , result ) ;
	}
	result = result >> binary_point ; // shift result right to account for shift left of sqr 
	return result ;
}

unsigned int vector2_mag( int x , int y )
{
	long unsigned int magsqr ;
	magsqr = __builtin_mulss( x , x ) + __builtin_mulss( y , y ) ;
	return sqrt_long( magsqr )	;
}

unsigned int vector3_mag( int x , int y , int z )
{
	long unsigned int magsqr ;
	magsqr = __builtin_mulss( x , x ) + __builtin_mulss( y , y ) + __builtin_mulss( z , z );
	return sqrt_long( magsqr )	;
}

 
