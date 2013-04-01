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
#include "fbwCntrl.h"
#include "fbw_options.h"
#include "autopilotCntrl.h"
#include "motionCntrl.h"
#include "airframe.h"
#include "minifloat.h"


//int yawkdail 		= YAWKD_AILERON*SCALEGYRO*RMAX ;
//int rollkp 			= ROLLKP*RMAX ;
//int rollkd 			= ROLLKD*SCALEGYRO*RMAX ;

//int hoverrollkp 	= HOVER_ROLLKP*SCALEGYRO*RMAX ;
//int hoverrollkd 	= HOVER_ROLLKD*SCALEGYRO*RMAX ;

int32_t roll_damping			= AFRM_Q16_SCALE * 3.0;
int32_t roll_error_rate_gain	= AFRM_Q16_SCALE * 10.0;

int16_t yawkpail = YAWKP_AILERON*RMAX ;


fractional calc_roll_position_error(void);


void rollCntrl(void)
{

	union longww rollAccum = { 0 } ;

	minifloat roll_pos_error;
	minifloat roll_rate_error = {0,0};
	minifloat roll_rate_control = {0,0};
	minifloat roll_rate_demand = {0,0};

	// Scale of radians/s per AD converter unit
	const minifloat gyro_radians_scale = ftomf(SCALEGYRO / 5632.0);

	// Get roll rate feedback and scale to rad/s
	minifloat roll_rate_feedback = ltomf((int32_t) omegaAccum[1]);
	roll_rate_feedback = mf_mult(roll_rate_feedback, gyro_radians_scale);

	// Get position error and scale to rad (ish!)
    roll_pos_error = RMAXtomf(calc_roll_position_error());

	// Feed position error into rate demand
	roll_rate_demand = mf_mult( Q16tomf(roll_error_rate_gain) , roll_pos_error);

	// Error is demand minus feedback
	roll_rate_error = mf_add(roll_rate_feedback, roll_rate_demand);

	// Control is rate error times damping gain
	roll_rate_control = mf_mult( roll_rate_error, Q16tomf(roll_damping) );

	if ( mode_autopilot_enabled() )
	{
		// Get the change in aoa required from each wing to acheive the roll rate.
		minifloat aoa_delta = afrm_get_roll_rate_required_aoa_delta(air_speed_3DIMU, roll_rate_control);

		// Get the aileron deflection required for the change in aoa
		minifloat ail_deflection = afrm_get_aileron_deflection(aoa_delta);

		rollAccum.WW = afrm_lookup_aileron_control(ail_deflection) ;
	
	}
	else
	{
		rollAccum.WW = 0 ;
	}
	

	rollAccum.WW = limitRMAX(rollAccum.WW);
	roll_control = rollAccum._.W0;

	ap_cntrls[AP_CNTRL_ROLL]	= roll_control;

	// Servo reversing is handled in servoMix.c
	
	return ;
}



fractional calc_roll_position_error(void)
{
    struct relative2D rollDemand = get_auto_rollDemand();

    union longww rollAccum ;
    union longww dotprod ;
	union longww crossprod ;
	fractional actualX = rmat[8];
	fractional actualY = rmat[6];
	fractional desiredX = rollDemand.x ;
	fractional desiredY = rollDemand.y ;

	dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
	crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
	crossprod.WW = crossprod.WW<<2 ; // at this point, we have 1/2 of the cross product
									// cannot go any higher than that, could get overflow
	if ( dotprod._.W1 > 0 )
	{
		rollAccum._.W0 = -crossprod._.W1;
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			rollAccum._.W0 = -RMAX ;
		}
		else
		{
			rollAccum._.W0 = RMAX ;
		}
	}  

	return rollAccum._.W0;
}


