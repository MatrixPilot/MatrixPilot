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

#if (AIRFRAME_TYPE != AIRFRAME_QUAD)

#include "airspeedCntrl.h"

#if(ALTITUDE_GAINS_VARIABLE != 1)
// If mavlink is being used but the gains are not variable
// implement the malink parameter variables for airspeed here
#if(SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
	#include "airspeedCntrl.h"

	int16_t minimum_groundspeed		= MINIMUM_GROUNDSPEED * 100;
	int16_t minimum_airspeed		= MINIMUM_AIRSPEED * 100;
	int16_t maximum_airspeed		= MAXIMUM_AIRSPEED * 100;
	int16_t cruise_airspeed			= CRUISE_AIRSPEED * 100;
	
	int16_t airspeed_pitch_adjust_rate	= (AIRSPEED_PITCH_ADJ_RATE*(RMAX/(57.3 * 40.0)));
	
	int16_t airspeed_pitch_ki_limit	= (AIRSPEED_PITCH_KI_MAX*(RMAX/57.3));
	fractional airspeed_pitch_ki = (AIRSPEED_PITCH_KI * RMAX);
	
	int16_t airspeed_pitch_min_aspd = (AIRSPEED_PITCH_MIN_ASPD*(RMAX/57.3));
	int16_t airspeed_pitch_max_aspd = (AIRSPEED_PITCH_MAX_ASPD*(RMAX/57.3));
#endif	//SERIAL_MAVLINK


#else	//ALTITUDE_GAINS_VARIABLE == 1


#include "airspeedCntrl.h"

// Calculate the airspeed.
extern int16_t calc_airspeed(void);

// Calculate the groundspeed.
extern int16_t calc_groundspeed(void);

// Calculate the target airspeed in cm/s from desiredSpd in dm/s
extern int16_t calc_target_airspeed(int16_t desiredSpd);

// Calculate the airspeed error vs target airspeed including filtering
extern int16_t calc_airspeed_error(void);

// Calculate the airspeed error integral term with filtering and limits
extern int32_t calc_airspeed_int_error(int16_t aspdError, int32_t aspd_integral);

int16_t 	airspeed		= 0;
int16_t 	groundspeed		= 0;
int16_t 	airspeedError	= 0;
int16_t 	target_airspeed	= 0;

int16_t minimum_groundspeed		= MINIMUM_GROUNDSPEED * 100;
int16_t minimum_airspeed		= MINIMUM_AIRSPEED * 100;
int16_t maximum_airspeed		= MAXIMUM_AIRSPEED * 100;
int16_t cruise_airspeed			= CRUISE_AIRSPEED * 100;

int16_t airspeed_pitch_adjust_rate	= (AIRSPEED_PITCH_ADJ_RATE*(RMAX/(57.3 * 40.0)));

// Remember last adjustment to limit rate of adjustment.
fractional last_aspd_pitch_adj	= 0;

// Integral of airspeed error
// lower word is underflow.  Upper word is output in degrees.
union longww airspeed_error_integral = {0};

int16_t airspeed_pitch_ki_limit	= (AIRSPEED_PITCH_KI_MAX*(RMAX/57.3));
fractional airspeed_pitch_ki = (AIRSPEED_PITCH_KI * RMAX);

int16_t airspeed_pitch_min_aspd = (AIRSPEED_PITCH_MIN_ASPD*(RMAX/57.3));
int16_t airspeed_pitch_max_aspd = (AIRSPEED_PITCH_MAX_ASPD*(RMAX/57.3));

void airspeedCntrl(void)
{
	airspeed 		= calc_airspeed();
	groundspeed 	= calc_groundspeed();
	target_airspeed = calc_target_airspeed(desiredSpeed);
	airspeedError 	= calc_airspeed_error();
 	airspeed_error_integral.WW = calc_airspeed_int_error(airspeedError, airspeed_error_integral.WW);
	return;
}


// Calculate the airspeed.
// Note that this airspeed is a magnitude regardless of direction.
// It is not a calculation of forward airspeed.
int16_t calc_airspeed(void)
{
	int16_t speed_component ;
	int32_t fwdaspd2;

	speed_component = IMUvelocityx._.W1 - estimatedWind[0] ;
	fwdaspd2 = __builtin_mulss ( speed_component , speed_component ) ;

	speed_component = IMUvelocityy._.W1 - estimatedWind[1] ;
	fwdaspd2 += __builtin_mulss ( speed_component , speed_component ) ;

	speed_component = IMUvelocityz._.W1 - estimatedWind[2] ;
	fwdaspd2 += __builtin_mulss ( speed_component , speed_component ) ;

	airspeed  = sqrt_long(fwdaspd2);

	return airspeed;
}

// Calculate the groundspeed in cm/s
int16_t calc_groundspeed(void) // computes (1/2gravity)*( actual_speed^2 - desired_speed^2 )
{
	int32_t gndspd2;
	gndspd2 = __builtin_mulss ( IMUvelocityx._.W1 , IMUvelocityx._.W1 ) ;
	gndspd2 += __builtin_mulss ( IMUvelocityy._.W1 , IMUvelocityy._.W1 ) ;
	gndspd2 += __builtin_mulss ( IMUvelocityz._.W1 , IMUvelocityz._.W1 ) ;

	return sqrt_long(gndspd2);
}


// Calculate the required airspeed in cm/s.  desiredSpeed is in dm/s
int16_t calc_target_airspeed(int16_t desiredSpd)
{
	union longww accum ;
	int16_t target;

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
int16_t calc_airspeed_error(void)
{
	//Some airspeed error filtering
	airspeedError = airspeedError >> 1;
	airspeedError += ( (target_airspeed - airspeed) >> 1);

	return airspeedError;
}

// Calculate the airspeed error integral term with filtering and limits
int32_t calc_airspeed_int_error(int16_t aspdError, int32_t aspd_integral)
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
	union longww accum ;

	// linear interpolation between target airspeed and cruise airspeed.
	// calculating demand airspeed to pitch feedforward
	int16_t aspd_tc_delta = target_airspeed - cruise_airspeed;
	int16_t aspd_tc_range;
	int16_t pitch_range = 0;
	fractional aspd_pitch_adj = 0;
	
	if(aspd_tc_delta > 0)
	{
		aspd_tc_range = maximum_airspeed - cruise_airspeed;
		pitch_range = airspeed_pitch_max_aspd;
	}
	else if(aspd_tc_delta < 0)
	{
		aspd_tc_range = cruise_airspeed - minimum_airspeed;
		pitch_range = airspeed_pitch_min_aspd;
		aspd_tc_delta = -aspd_tc_delta;
	}
	else
	{
		aspd_tc_range = 1;
	}

	accum.WW = 0;
	accum._.W1 = aspd_tc_delta;
	accum._.W1 = __builtin_divsd( accum.WW >> 2,  aspd_tc_range );
	accum.WW = __builtin_mulss( accum._.W1, pitch_range ) << 2;
	aspd_pitch_adj = accum._.W1;

	aspd_pitch_adj -= airspeed_error_integral._.W1;

	// Pitch adjust for airspeed on glide only.
	if(throttle_control >= 100)
	{
		aspd_pitch_adj = 0;
		airspeed_error_integral.WW = 0;
	}

	// limit the rate of the airspeed pitch adjustment
	if(aspd_pitch_adj > last_aspd_pitch_adj)
	{
		if( (last_aspd_pitch_adj + airspeed_pitch_adjust_rate) < aspd_pitch_adj)
			aspd_pitch_adj = (last_aspd_pitch_adj + airspeed_pitch_adjust_rate);
	}
	else
	{
		if( (last_aspd_pitch_adj - airspeed_pitch_adjust_rate) > aspd_pitch_adj)
			aspd_pitch_adj = (last_aspd_pitch_adj - airspeed_pitch_adjust_rate);
	}

	last_aspd_pitch_adj = aspd_pitch_adj;

	return aspd_pitch_adj;
}


#endif		//(ALTITUDE_GAINS_VARIABLE == 1)

#endif // AIRFRAME_TYPE
