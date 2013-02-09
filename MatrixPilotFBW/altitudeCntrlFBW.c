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

#include "../MatrixPilot/defines.h"
#include "fbw_options.h"
#include "fbwCntrl.h"
#include "inputCntrl.h"
#include "airspeedCntrlFBW.h"
#include "motionCntrl.h"

long speed_height_error = 0 ;

// Excess energy height.  Positive is too fast.
long excess_energy_height(int targetAspd, int actualAspd) // computes (1/2gravity)*( actual_speed^2 - desired_speed^2 )
{
	union longww accum;
	union longww height ;

	// targetAspd * 6 / 10 
	// 1/10 to scale from cm/s to dm/s
	// 6 is ~1/(2*g) with adjustments for x8 multiply later

	accum.WW = __builtin_mulsu ( actualAspd , 37877 ) ;
	height.WW = __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( targetAspd , 37877 ) ;
	height.WW -= (__builtin_mulss ( accum._.W1 , accum._.W1 )) ;

	return height.WW;
}


void altitudeCntrl(void)
{
	speed_height_error = excess_energy_height(target_airspeed, air_speed_3DIMU);
	return ;
}


inline long get_speed_height_error(void)
{
	return speed_height_error;
}


#define ASPD_TCONST_GAIN (RMAX / ASPD_ADJ_TIME_CONSTANT)
#define POTENTIAL_RANGE_MAX	( (long) 327 << 16 )


//Calculate and return pitch target adjustment for target airspeed
// Kinetic error in cm
// Return pitch target in degrees
signed char airspeed_pitch_adjust(fractional throttle, int actual_aspd, int target_aspd, int min_airspeed, long aspd_potential_error)
{
	union longww temp;

	int glideRate = expected_glide_descent_rate(actual_aspd);

	int climbRate = feedforward_climb_rate(throttle, glideRate, actual_aspd);

	temp.WW = aspd_potential_error;

	// Limit the potential error to the +-327m or +-32700cm range.
	// Then multiply by 100 to get error in cm
	if(temp.WW > POTENTIAL_RANGE_MAX)
		temp.WW = (POTENTIAL_RANGE_MAX * 100);
	else if(temp.WW < -POTENTIAL_RANGE_MAX)
		temp.WW = -(POTENTIAL_RANGE_MAX * 100);
	else
		temp.WW *= 100;
		//TODO optimise this

	// Multiply the aspd_potential_error in cm by the time constant gain
	temp.WW = __builtin_mulsu( temp._.W1, ASPD_TCONST_GAIN);
//	temp.WW <<= 1;

	// Add the kinetic adjust * gain to the required climb rate.	
	climbRate += temp._.W1;

	// limit airspeed value to the minimum airspeed
	if(actual_aspd <  min_airspeed)
		actual_aspd =  min_airspeed;

	// limit airspeed value to 1m/s minimum to avoid divide by zero error
	if(actual_aspd <  100)
		actual_aspd =  100;

	if(climbRate > actual_aspd)
		climbRate = actual_aspd;
	if(climbRate < -actual_aspd)
		climbRate = actual_aspd;

	// Divide climbRate by airspeed
	temp.WW = 0;
	temp._.W1 = climbRate;
	temp.WW >>= 2;
	climbRate  =	__builtin_divsd( temp.WW , actual_aspd );
//	climbRate >>= 2;

	// Return angle of climb
	return arcsine(climbRate);
}
