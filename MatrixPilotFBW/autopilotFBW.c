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

// Values for navType:
// 'y' = yaw/rudder, 'a' = aileron/roll, 'h' = aileron/hovering

// Outputs an RMAX scaled value RMAX*SIN(A) where A is the angle difference
// between two angles
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
		deflectionAccum.WW = crossprod._.W1;
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			deflectionAccum._.W1 = RMAX ;
		}
		else
		{
			deflectionAccum._.W1 = -RMAX ; // yawkp is unsigned, must divide and then negate
		}
	}
	
	if (navType == 'h') deflectionAccum.WW = -deflectionAccum.WW ;

	// multiply by wind gain adjustment, and multiply by 2
//	deflectionAccum.WW = ( __builtin_mulsu ( deflectionAccum._.W1 , wind_gain )<<1 ) ; 
	return deflectionAccum._.W1 ;
}
