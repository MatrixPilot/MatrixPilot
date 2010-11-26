// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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

#define HOVERYOFFSET ((long)(HOVER_YAW_OFFSET*(RMAX/57.3)))

#if(GAINS_VARIABLE == 0)
	const int yawkdrud 	= YAWKD_RUDDER*SCALEGYRO*RMAX ;
	const int rollkprud 	= ROLLKP_RUDDER*RMAX ;
	const int hoveryawkp 	= HOVER_YAWKP*RMAX ;
	const int hoveryawkd 	= HOVER_YAWKD*SCALEGYRO*RMAX ;
#else
	int yawkdrud 	= YAWKD_RUDDER*SCALEGYRO*RMAX ;
	int rollkprud 	= ROLLKP_RUDDER*RMAX ;
	int hoveryawkp 	= HOVER_YAWKP*RMAX ;
	int hoveryawkd 	= HOVER_YAWKD*SCALEGYRO*RMAX ;
#endif


void normalYawCntrl(void) ;
void hoverYawCntrl(void) ;


void yawCntrl(void)
{
	if ( canStabilizeHover() && current_orientation == F_HOVER )
	{
		hoverYawCntrl() ;
	}
	else
	{
		normalYawCntrl() ;
	}
	
	return ;
}


void normalYawCntrl(void)
{
	int yawNavDeflection ;
	union longww rollStabilization ;
	union longww gyroYawFeedback ;
	int ail_rud_mix ;

#ifdef TestGains
	flags._.GPS_steering = 0 ; // turn off navigation
	flags._.pitch_feedback = 1 ; // turn on stabilization
#endif 
	if ( RUDDER_NAVIGATION && flags._.GPS_steering )
	{
		yawNavDeflection = determine_navigation_deflection( 'y' ) ;
		
		if ( canStabilizeInverted() && current_orientation == F_INVERTED )
		{
			yawNavDeflection = -yawNavDeflection ;
		}
	}
	else
	{
		yawNavDeflection = 0 ;
	}
	
	if ( YAW_STABILIZATION_RUDDER && flags._.pitch_feedback )
	{
		gyroYawFeedback.WW = __builtin_mulss( yawkdrud, omegaAccum[2] ) ;
	}
	else
	{
		gyroYawFeedback.WW = 0 ;
	}

	if ( ROLL_STABILIZATION_RUDDER && flags._.pitch_feedback && ( current_orientation == F_NORMAL ) )
	{
		rollStabilization.WW = __builtin_mulss( rmat[6] , rollkprud ) ;
	}
	else
	{
		rollStabilization.WW = 0 ;
	}
	
	if ( flags._.pitch_feedback )
	{
		int ail_offset = udb_pwIn[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL] ;
		ail_rud_mix = MANUAL_AILERON_RUDDER_MIX * REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, ail_offset) ;
		if ( canStabilizeInverted() && current_orientation == F_INVERTED ) ail_rud_mix = -ail_rud_mix ;
	}
	else
	{
		ail_rud_mix = 0 ;
	}
	
	yaw_control = (long)yawNavDeflection 
				- (long)gyroYawFeedback._.W1 
				+ (long)rollStabilization._.W1 
				+ ail_rud_mix ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}


void hoverYawCntrl(void)
{
	union longww yawAccum ;
	union longww gyroYawFeedback ;
	
	if ( flags._.pitch_feedback )
	{
		gyroYawFeedback.WW = __builtin_mulss( hoveryawkd , omegaAccum[2] ) ;
		
		int yawInput = ( udb_flags._.radio_on == 1 ) ? REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, udb_pwIn[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) : 0 ;
		int manualYawOffset = yawInput * (int)(RMAX/2000);
		
		yawAccum.WW = __builtin_mulss( rmat[6] + HOVERYOFFSET + manualYawOffset , hoveryawkp ) ;
	}
	else
	{
		gyroYawFeedback.WW = 0 ;
		yawAccum.WW = 0 ;
	}
	
	yaw_control = (long)yawAccum._.W1 - (long)gyroYawFeedback._.W1 ;
	
	return ;
}
