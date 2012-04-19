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
long 	airspeed2 		= 0;
int 	groundspeed		= 0;
long 	groundspeed2 	= 0;

int minimum_groundspeed		= MINIMUM_GROUNDSPEED;
int minimum_airspeed		= MINIMUM_AIRSPEED;
int maximum_airspeed		= MAXIMUM_AIRSPEED;

void calc_airspeed(void)
{
	int speed_component ;
	union longww accum ;
	long fwdapsd2 = 0;

	speed_component = IMUvelocityx._.W1 - estimatedWind[0] ;
	accum.WW = __builtin_mulsu ( speed_component , 37877 ) ;
	fwdapsd2 += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	speed_component = IMUvelocityy._.W1 - estimatedWind[1] ;
	accum.WW = __builtin_mulsu ( speed_component , 37877 ) ;
	fwdapsd2 += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	speed_component = IMUvelocityz._.W1 - estimatedWind[2] ;
	accum.WW = __builtin_mulsu ( speed_component , 37877 ) ;
	fwdapsd2 += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	airspeed  = sqrt_long(fwdapsd2);
	airspeed2 = fwdapsd2;
}

void calc_groundspeed(void) // computes (1/2gravity)*( actual_speed^2 - desired_speed^2 )
{
	long ground_speed2_ = 0;
	union longww accum ;

	accum.WW = __builtin_mulsu ( IMUvelocityx._.W1 , 37877 ) ;
	ground_speed2_ += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( IMUvelocityy._.W1 , 37877 ) ;
	ground_speed2_ += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( IMUvelocityz._.W1 , 37877 ) ;
	ground_speed2_ += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	groundspeed 	= sqrt(ground_speed2_);
	groundspeed2 	= ground_speed2_;
}

void calc_target_airspeed(void)
{
	target_airspeed = desiredSpeed;

	if(groundspeed < minimum_groundspeed)
		target_airspeed += (minimum_groundspeed - groundspeed);

	if(target_airspeed > maximum_airspeed)
		target_airspeed = maximum_airspeed;

	if(target_airspeed < minimum_airspeed)
		target_airspeed = minimum_airspeed;
}

#endif		//(ALTITUDE_GAINS_VARIABLE == 1)