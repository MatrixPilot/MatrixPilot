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

	if ( canStabilizeHover() && current_orientation == F_HOVER )
	{
		hoverRollCntrl() ;
	}
	else
	{
		normalRollCntrl() ;
	}
	
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
	if ( AILERON_NAVIGATION && flags._.GPS_steering )
	{
		rollAccum._.W1 = determine_navigation_deflection( 'a' ) ;
	}
	
#ifdef TestGains
	flags._.pitch_feedback = 1 ;
#endif
	
	if ( ROLL_STABILIZATION_AILERONS && flags._.pitch_feedback )
	{
		gyroRollFeedback.WW = __builtin_mulss( rollkd , omegaAccum[1] ) ;
		rollAccum.WW += __builtin_mulss( rmat6 , rollkp ) ;
	}
	else
	{
		gyroRollFeedback.WW = 0 ;
	}
	
	if ( YAW_STABILIZATION_AILERON && flags._.pitch_feedback )
	{
		gyroYawFeedback.WW = __builtin_mulss( yawkdail, omegaAccum2 ) ;
	}
	else
	{
		gyroYawFeedback.WW = 0 ;
	}
	
	roll_control = (long)rollAccum._.W1 - (long)gyroRollFeedback._.W1 - (long)gyroYawFeedback._.W1 ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}


void hoverRollCntrl(void)
{
	int rollNavDeflection ;
	union longww gyroRollFeedback ;
	
	if ( flags._.pitch_feedback )
	{
		if ( AILERON_NAVIGATION && flags._.GPS_steering )
		{
			rollNavDeflection = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS/2) ? determine_navigation_deflection( 'h' ) : 0 ;
		}
		else
		{
			rollNavDeflection = 0 ;
		}
		
		gyroRollFeedback.WW = __builtin_mulss( hoverrollkd , omegaAccum[1] ) ;
	}
	else
	{
		rollNavDeflection = 0 ;
		gyroRollFeedback.WW = 0 ;
	}
	
	roll_control = rollNavDeflection -(long)gyroRollFeedback._.W1 ;
	
	return ;
}
