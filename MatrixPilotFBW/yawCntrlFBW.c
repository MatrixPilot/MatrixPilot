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
#include "airframe.h"
#include "minifloat.h"


#define HOVERYOFFSET ((long)(HOVER_YAW_OFFSET*(RMAX/57.3)))

int yawkdrud 	= YAWKD_RUDDER*SCALEGYRO*RMAX ;
int rollkprud 	= ROLLKP_RUDDER*RMAX ;
int rollkdrud 	= ROLLKD_RUDDER*SCALEGYRO*RMAX ;
int hoveryawkp 	= HOVER_YAWKP*RMAX ;
int hoveryawkd 	= HOVER_YAWKD*SCALEGYRO*RMAX ;

void normalYawCntrl(void) ;
void hoverYawCntrl(void) ;

int yawkprud = YAWKP_RUDDER*RMAX ;

// Q16 gains in long type for telemetry
long yaw_rate_gain 	= (AFRM_Q16_SCALE*0.2);
long yaw_damping 	= (AFRM_Q16_SCALE* 30.0);
//long yaw_error 	= (AFRM_Q16_SCALE*0.2);

void yawCntrl(void)
{
//	if ( canStabilizeHover() && current_orientation == F_HOVER )
//	{
//		hoverYawCntrl() ;
//	}
//	else
//	{
		normalYawCntrl() ;
//	}
	
//	return ;
}


void normalYawCntrl(void)
{
	union longww temp;
	_Q16	Q16temp;
	minifloat yaw_rate_demand;
	minifloat yaw_rate_feedback;
	minifloat yaw_rate_error;
	minifloat yaw_moment;
	minifloat ClRudd;

	// Scale of radians/s per AD converter unit
	const minifloat gyro_radians_scale = ftomf(SCALEGYRO / 5632.0);

	int yawNavDeflection ;
	union longww rollStabilization ;
	union longww gyroYawFeedback ;

	// Mystery multipy by 2.  Don't know why
	Q16temp = get_earth_turn_rate() << 2;
	yaw_rate_demand = Q16tomf(Q16temp);

	// Multiply the expected earth rotation rate by sin(roll) to get yaw rate.
	// Results in rad/s
	yaw_rate_demand = mf_mult( yaw_rate_demand, RMAXtomf(rmat[6]) );

	// Scale gyro to radians/s in minifloat
	yaw_rate_feedback = ltomf(omegaAccum[2]);
	yaw_rate_feedback = mf_mult(gyro_radians_scale, yaw_rate_feedback);

	yaw_rate_error = mf_sub(yaw_rate_feedback, yaw_rate_demand);

	yaw_moment = mf_mult(yaw_rate_error , Q16tomf(yaw_damping) );

	// Find the lift coefficient for the rudder
	ClRudd = afrm_get_rudd_required_Cl(air_speed_3DIMU , yaw_moment);

	if ( mode_autopilot_enabled() )
	{
		ap_cntrls[AP_CNTRL_YAW] = lookup_rudder_control( ClRudd );
	}
	else
	{
		ap_cntrls[AP_CNTRL_YAW] = 0 ;
	}

	return ;
}


void hoverYawCntrl(void)
{
//	union longww yawAccum ;
//	union longww gyroYawFeedback ;
//	
//	if ( flags._.pitch_feedback )
//	{
//		gyroYawFeedback.WW = __builtin_mulss( hoveryawkd , omegaAccum[2] ) ;
//		
//		int yawInput = ( udb_flags._.radio_on == 1 ) ? REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, udb_pwIn[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) : 0 ;
//		int manualYawOffset = yawInput * (int)(RMAX/2000);
//		
//		yawAccum.WW = __builtin_mulss( rmat[6] + HOVERYOFFSET + manualYawOffset , hoveryawkp ) ;
//	}
//	else
//	{
//		gyroYawFeedback.WW = 0 ;
//		yawAccum.WW = 0 ;
//	}
//	
//	yaw_control = (long)yawAccum._.W1 - (long)gyroYawFeedback._.W1 ;
//	ap_cntrls[AP_CNTRL_YAW]			= PWM_to_frac(yaw_control		,0	, false);

	return ;
}
