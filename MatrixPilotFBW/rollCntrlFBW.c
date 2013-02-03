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

#if(USE_FBW == 1)


#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || (GAINS_VARIABLE == 1)
	int yawkdail 		= YAWKD_AILERON*SCALEGYRO*RMAX ;
	int rollkp 			= ROLLKP*RMAX ;
	int rollkd 			= ROLLKD*SCALEGYRO*RMAX ;
#else 
	const int yawkdail 	= YAWKD_AILERON*SCALEGYRO*RMAX ;

	const int rollkp 	= ROLLKP*RMAX ;
	const int rollkd 	= ROLLKD*SCALEGYRO*RMAX ;
#endif	

#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || (GAINS_VARIABLE == 1)
	int hoverrollkp 	= HOVER_ROLLKP*SCALEGYRO*RMAX ;
	int hoverrollkd 	= HOVER_ROLLKD*SCALEGYRO*RMAX ;
#else
	const int hoverrollkp = HOVER_ROLLKP*SCALEGYRO*RMAX ;
	const int hoverrollkd = HOVER_ROLLKD*SCALEGYRO*RMAX ;
#endif

void normalRollCntrl(void) ;
void hoverRollCntrl(void) ;

void rollCntrl(void)
{

//	if ( canStabilizeHover() && current_orientation == F_HOVER )
//	{
//		hoverRollCntrl() ;
//	}
//	else
//	{
		normalRollCntrl() ;
//	}
//	
	return ;
}


void normalRollCntrl(void)
{
	union longww rollAccum = { 0 } ;
	union longww gyroRollFeedback ;
	union longww gyroYawFeedback ;
	
	fractional rmat6 ;
	fractional omegaAccum2 ;
	
	if ( !canStabilizeInverted() || !desired_behavior._.inverted )
	{
		rmat6 = rmat[6] ;
		omegaAccum2 = omegaAccum[2] ;
	}
	else
	{
		rmat6 = -rmat[6] ;
		omegaAccum2 = -omegaAccum[2] ;
	}
	
#ifdef TestGains
	flags._.GPS_steering = 0 ; // turn off navigation
#endif

	if ( mode_navigation_enabled() )
	{
		rollAccum.WW = (long) determine_navigation_deflection( 'a' ) << 4;
	}
	else
	{
		if(get_flightmode() == FLIGHT_MODE_ASSISTED)
		{
			if(fbw_roll_mode == FBW_ROLL_MODE_POSITION)
			{
				rollAccum.WW = (long) get_desiredRollPosition();
			}
			else
			{
				rollAccum.WW = 0;
			}
		}
		else
		{
			rollAccum.WW = 0;
		}
	}
	
#ifdef TestGains
	flags._.pitch_feedback = 1 ;
#endif
	
	if ( ROLL_STABILIZATION_AILERONS && mode_autopilot_enabled() )
	{
		rollAccum.WW += (long) get_earth_roll_angle();
		rollAccum.WW = limitRMAX(rollAccum.WW);

		rollAccum.WW += __builtin_mulss( rollAccum._.W0 , rollkp ) >> 10;

		// Feed roll error into roll rate demand
		gyroRollFeedback.WW  =  (long) omegaAccum[1];
		gyroRollFeedback.WW -= __builtin_mulss( rollAccum._.W0 , RMAX*0.25 ) >> 14 ;

		gyroRollFeedback.WW = limitRMAX(gyroRollFeedback.WW);
		gyroRollFeedback.WW = __builtin_mulss( rollkd , gyroRollFeedback._.W0 ) >> 10 ;
	
	}
	else
	{
		rollAccum.WW = 0 ;
		gyroRollFeedback.WW = 0 ;
	}
	
//	if ( YAW_STABILIZATION_AILERON && flags._.pitch_feedback )
//	{
//		gyroYawFeedback.WW = __builtin_mulss( yawkdail, omegaAccum2 ) ;
//	}
//	else
//	{
		gyroYawFeedback.WW = 0 ;
//	}

	gyroRollFeedback.WW = limitRMAX(gyroRollFeedback.WW);
	gyroYawFeedback.WW = limitRMAX(gyroYawFeedback.WW);

	rollAccum.WW = rollAccum.WW - (long) gyroRollFeedback._.W1 - (long) gyroYawFeedback._.W1 ;
	rollAccum.WW = limitRMAX(rollAccum.WW);
	roll_control = rollAccum._.W0;

	ap_cntrls[AP_CNTRL_ROLL]	= roll_control;

	// Servo reversing is handled in servoMix.c
	
	return ;
}


//void hoverRollCntrl(void)
//{
//	int rollNavDeflection ;
//	union longww gyroRollFeedback ;
//	
//	if ( flags._.pitch_feedback )
//	{
//		if ( AILERON_NAVIGATION && flags._.GPS_steering )
//		{
//			rollNavDeflection = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS/2) ? determine_navigation_deflection( 'h' ) : 0 ;
//		}
//		else
//		{
//			rollNavDeflection = 0 ;
//		}
//		
//		gyroRollFeedback.WW = __builtin_mulss( hoverrollkd , omegaAccum[1] ) ;
//	}
//	else
//	{
//		rollNavDeflection = 0 ;
//		gyroRollFeedback.WW = 0 ;
//	}
//	
//	roll_control = rollNavDeflection -(long)gyroRollFeedback._.W1 ;
//	ap_cntrls[AP_CNTRL_ROLL]		= PWM_to_frac(roll_control		,0	, false);
//	
//	return ;
//}

#endif	//(USE_FBW == 1)
