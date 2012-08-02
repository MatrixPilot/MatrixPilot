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
#include "airspeedCntrl.h"
#include "fbw_options.h"
#include "fbwCntrl.h"
#include "../libDCM/libDCM.h"
#include "airframe.h"

#if(USE_FBW == 1)

#include "airspeedCntrl.h"

// Calculate the target airspeed in cm/s from desiredSpd in dm/s
extern int calc_target_airspeed(int desiredSpd, unsigned int airspeed, unsigned int groundspeed);

// Calculate the airspeed error vs target airspeed including filtering
extern int calc_airspeed_error(void);

// Calculate the airspeed error integral term with filtering and limits
extern long calc_airspeed_int_error(int aspdError, long aspd_integral);

int 	airspeed		= 0;
int 	groundspeed		= 0;
int 	airspeedError	= 0;
int 	target_airspeed	= 0;

int minimum_groundspeed		= MINIMUM_GROUNDSPEED * 100;
int minimum_airspeed		= MINIMUM_AIRSPEED * 100;
int maximum_airspeed		= MAXIMUM_AIRSPEED * 100;
int cruise_airspeed			= CRUISE_AIRSPEED * 100;

int airspeed_pitch_adjust_rate	= (AIRSPEED_PITCH_ADJ_RATE*(RMAX/(57.3 * 40.0)));

// Remember last adjustment to limit rate of adjustment.
fractional last_aspd_pitch_adj	= 0;

// Integral of airspeed error
// lower word is underflow.  Upper word is output in degrees.
union longww airspeed_error_integral = {0};

int airspeed_pitch_ki_limit	= (AIRSPEED_PITCH_KI_MAX*(RMAX/57.3));
fractional airspeed_pitch_ki = (AIRSPEED_PITCH_KI * RMAX);

int airspeed_pitch_min_aspd = (AIRSPEED_PITCH_MIN_ASPD*(RMAX/57.3));
int airspeed_pitch_max_aspd = (AIRSPEED_PITCH_MAX_ASPD*(RMAX/57.3));

void airspeedCntrl(void)
{
	// If the radio is not on, force the desired airspeed to RTL airspeed
	if (udb_flags._.radio_on == 0)
		desiredSpeed = RTL_AIRSPEED * 10.0;

	target_airspeed = calc_target_airspeed(desiredSpeed, air_speed_3DIMU, ground_speed_3DIMU);

	airspeedError 	= calc_airspeed_error();
 	airspeed_error_integral.WW = calc_airspeed_int_error(airspeedError, airspeed_error_integral.WW);
	return;
}


// Calculate the required airspeed in cm/s.  desiredSpeed is in dm/s
// airspeed and groundspeed in cm/s
int calc_target_airspeed(int desiredSpd, unsigned int airspeed, unsigned int groundspeed)
{
	union longww accum ;
	int target;

	accum.WW = __builtin_mulsu ( desiredSpd , 10 ) ;
	target = accum._.W0 ;

	if(groundspeed < minimum_groundspeed)
		target += (minimum_groundspeed - groundspeed);

	if(target > maximum_airspeed)
		target = maximum_airspeed;

	if(target < minimum_airspeed)
		target = minimum_airspeed;

	return target;
}


// Calculate the airspeed error vs target airspeed including filtering
int calc_airspeed_error(void)
{
	//Some airspeed error filtering
	airspeedError = airspeedError >> 1;
	airspeedError += ( (target_airspeed - airspeed) >> 1);

	return airspeedError;
}

// Calculate the airspeed error integral term with filtering and limits
long calc_airspeed_int_error(int aspdError, long aspd_integral)
{
	union longww airspeed_int = {aspd_integral};
	airspeed_int.WW += __builtin_mulss( airspeed_pitch_ki, airspeedError ) << 2;

	if(airspeed_int._.W1 > airspeed_pitch_ki_limit)
		airspeed_int._.W1 = airspeed_pitch_ki_limit;
	else if(airspeed_int._.W1 < -airspeed_pitch_ki_limit)
		airspeed_int._.W1 = -airspeed_pitch_ki_limit;

	return airspeed_int.WW;
}

//Calculate and return pitch target adjustment for target airspeed
fractional gliding_airspeed_pitch_adjust(void)
{
//	union longww accum ;
//
//	// linear interpolation between target airspeed and cruise airspeed.
//	// calculating demand airspeed to pitch feedforward
//	int aspd_tc_delta = target_airspeed - cruise_airspeed;
//	int aspd_tc_range;
//	int pitch_range = 0;
//	fractional aspd_pitch_adj = 0;
//	
//	if(aspd_tc_delta > 0)
//	{
//		aspd_tc_range = maximum_airspeed - cruise_airspeed;
//		pitch_range = airspeed_pitch_max_aspd;
//	}
//	else if(aspd_tc_delta < 0)
//	{
//		aspd_tc_range = cruise_airspeed - minimum_airspeed;
//		pitch_range = airspeed_pitch_min_aspd;
//		aspd_tc_delta = -aspd_tc_delta;
//	}
//	else
//	{
//		aspd_tc_range = 1;
//	}
//
//	accum.WW = 0;
//	accum._.W1 = aspd_tc_delta;
//	accum._.W1 = __builtin_divsd( accum.WW >> 2,  aspd_tc_range );
//	accum.WW = __builtin_mulss( accum._.W1, pitch_range ) << 2;
//	aspd_pitch_adj = accum._.W1;
//
//	aspd_pitch_adj -= airspeed_error_integral._.W1;
//
//	// Pitch adjust for airspeed on glide only.
//	if(throttle_control >= 100)
//	{
//		aspd_pitch_adj = 0;
//		airspeed_error_integral.WW = 0;
//	}
//
//	// limit the rate of the airspeed pitch adjustment
//	if(aspd_pitch_adj > last_aspd_pitch_adj)
//	{
//		if( (last_aspd_pitch_adj + airspeed_pitch_adjust_rate) < aspd_pitch_adj)
//			aspd_pitch_adj = (last_aspd_pitch_adj + airspeed_pitch_adjust_rate);
//	}
//	else
//	{
//		if( (last_aspd_pitch_adj - airspeed_pitch_adjust_rate) > aspd_pitch_adj)
//			aspd_pitch_adj = (last_aspd_pitch_adj - airspeed_pitch_adjust_rate);
//	}
//
//	last_aspd_pitch_adj = aspd_pitch_adj;

//	return aspd_pitch_adj;
	return 0;
}

#define ASPD_TCONST_GAIN (RMAX / ASPD_ADJ_TIME_CONSTANT)
#define POTENTIAL_RANGE_MAX	( (long) 327 << 16 )

//Calculate and return pitch target adjustment for target airspeed
// Kinetic error in cm
// Return pitch target in degrees
signed char airspeed_pitch_adjust(fractional throttle, int actual_aspd, int target_aspd, long aspd_potential_error)
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

	// limit airspeed value to 1m/s 
	// TODO, correct for negative airspeeds
	if(actual_aspd < 100)
		actual_aspd = 100;

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



#endif		//(ALTITUDE_GAINS_VARIABLE == 1)
