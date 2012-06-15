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

#if(ALTITUDE_GAINS_VARIABLE == 1)

#include "airspeedCntrl.h"

extern int desiredSpeed;

int 	airspeed		= 0;
int 	groundspeed		= 0;
int 	airspeedError	= 0;

int minimum_groundspeed		= MINIMUM_GROUNDSPEED * 100.0;
int minimum_airspeed		= MINIMUM_AIRSPEED * 100.0;
int maximum_airspeed		= MAXIMUM_AIRSPEED * 100.0;

// Calculate the airspeed.
// Note that this airspeed is a magnitude regardless of direction.
// It is not a calculation of forward airspeed.
void calc_airspeed(void)
{
	int speed_component ;
	long fwdaspd2;

	speed_component = IMUvelocityx._.W1 - estimatedWind[0] ;
	fwdaspd2 = __builtin_mulss ( speed_component , speed_component ) ;

	speed_component = IMUvelocityy._.W1 - estimatedWind[1] ;
	fwdaspd2 += __builtin_mulss ( speed_component , speed_component ) ;

	speed_component = IMUvelocityz._.W1 - estimatedWind[2] ;
	fwdaspd2 += __builtin_mulss ( speed_component , speed_component ) ;

	airspeed  = sqrt_long(fwdaspd2);
}

// Calculate the groundspeed
void calc_groundspeed(void) // computes (1/2gravity)*( actual_speed^2 - desired_speed^2 )
{
	long gndspd2;

	gndspd2 = __builtin_mulss ( IMUvelocityx._.W1 , IMUvelocityx._.W1 ) ;
	gndspd2 += __builtin_mulss ( IMUvelocityy._.W1 , IMUvelocityy._.W1 ) ;
	gndspd2 += __builtin_mulss ( IMUvelocityz._.W1 , IMUvelocityz._.W1 ) ;

	groundspeed 	= sqrt_long(gndspd2);
}

// Calculate the required airspeed in cm/s.  desiredSpeed is in dm/s
void calc_target_airspeed(void)
{
	union longww accum ;
	accum.WW = __builtin_mulsu ( desiredSpeed , 10 ) ;
	target_airspeed = accum._.W0 ;

	if(groundspeed < minimum_groundspeed)
		target_airspeed += (minimum_groundspeed - groundspeed);

	if(target_airspeed > maximum_airspeed)
		target_airspeed = maximum_airspeed;

	if(target_airspeed < minimum_airspeed)
		target_airspeed = minimum_airspeed;

	//Some airspeed error filtering
	airspeedError = airspeedError >>=1;
	airspeedError += ( (target_airspeed - airspeed) >> 1);
}

#endif		//(ALTITUDE_GAINS_VARIABLE == 1)
