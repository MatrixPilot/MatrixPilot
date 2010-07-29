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

//	If the state machine selects pitch feedback, compute it from the pitch gyro and accelerometer.

#define RTLKICK ((long)(RTL_PITCH_DOWN*(RMAX/57.3)))
#define INVNPITCH ((long)(INVERTED_NEUTRAL_PITCH*(RMAX/57.3)))
#define HOVERPOFFSET ((long)(HOVER_PITCH_OFFSET*(RMAX/57.3)))
#define HOVERPTOWP ((long)(HOVER_PITCH_TOWARDS_WP*(RMAX/57.3)))

const int pitchgain = (int)(PITCHGAIN*RMAX) ;
const int pitchkd = (int) (PITCHKD*SCALEGYRO*RMAX) ;

const int hoverpitchgain = (int)(HOVER_PITCHGAIN*RMAX) ;
const int hoverpitchkd = (int) (HOVER_PITCHKD*SCALEGYRO*RMAX) ;

const int rudderElevMixGain = (int)(RMAX*RUDDER_ELEV_MIX) ;
const int rollElevMixGain = (int)(RMAX*ROLL_ELEV_MIX) ;

int pitchrate ;
int navElevMix ;
int elevInput ;

void normalPitchCntrl(void) ;
void hoverPitchCntrl(void) ;


void pitchCntrl(void)
{
	if ( canStabilizeHover() && desired_behavior._.hover )
	{
		hoverPitchCntrl() ;
	}
	else
	{
		normalPitchCntrl() ;
	}
	
	return ;
}


void normalPitchCntrl(void)
{
	union longww pitchAccum ;
	int rtlkick ;
	
#ifdef TestGains
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
#endif
	
	fractional rmat6 ;
	fractional rmat7 ;
	fractional rmat8 ;
	
	if ( !canStabilizeInverted() || current_orientation != F_INVERTED )
	{
		rmat6 = rmat[6] ;
		rmat7 = rmat[7] ;
		rmat8 = rmat[8] ;
	}
	else
	{
		rmat6 = -rmat[6] ;
		rmat7 = -rmat[7] ;
		rmat8 = -rmat[8] ;
		pitchAltitudeAdjust = -pitchAltitudeAdjust - INVNPITCH ;
	}
	
	navElevMix = 0 ;
	if ( flags._.pitch_feedback )
	{
		if ( RUDDER_OUTPUT_CHANNEL != CHANNEL_UNUSED && RUDDER_INPUT_CHANNEL != CHANNEL_UNUSED ) {
			pitchAccum.WW = __builtin_mulss( rmat6 , rudderElevMixGain ) << 1 ;
			pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 ,
				REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, udb_pwTrim[RUDDER_INPUT_CHANNEL] - udb_pwOut[RUDDER_OUTPUT_CHANNEL]) ) << 3 ;
			navElevMix += pitchAccum._.W1 ;
		}
		
		pitchAccum.WW = __builtin_mulss( rmat6 , rollElevMixGain ) << 1 ;
		pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 , rmat[6] ) >> 3 ;
		navElevMix += pitchAccum._.W1 ;
	}

	pitchAccum.WW = ( __builtin_mulss( rmat8 , omegagyro[0] )
					- __builtin_mulss( rmat6 , omegagyro[2] )) << 1 ;
	pitchrate = pitchAccum._.W1 ;
	
	if ( !udb_flags._.radio_on && flags._.GPS_steering )
	{
		rtlkick = RTLKICK ;
	}
	else
	{
		rtlkick = 0 ;
	}
	
	if ( PITCH_STABILIZATION && flags._.pitch_feedback )
	{
		pitchAccum.WW = __builtin_mulss( rmat7 - rtlkick + pitchAltitudeAdjust, pitchgain ) 
					  + __builtin_mulss( pitchkd , pitchrate ) ;
	}
	else
	{
		pitchAccum.WW = 0 ;
	}
	
	pitch_control = (long)pitchAccum._.W1 + navElevMix ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}


void hoverPitchCntrl(void)
{
	union longww pitchAccum ;
	
	if ( flags._.pitch_feedback )
	{
		pitchAccum.WW = ( __builtin_mulss( -rmat[7] , omegagyro[0] )
						- __builtin_mulss( rmat[6] , omegagyro[1] )) << 1 ;
		pitchrate = pitchAccum._.W1 ;
		
		int elevInput = ( udb_flags._.radio_on == 1 ) ? REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, udb_pwIn[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) : 0 ;
		int manualPitchOffset = elevInput * (int)(RMAX/600);
		
		long pitchToWP ;
		
		if ( flags._.GPS_steering )
		{
			pitchToWP = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS) ? HOVERPTOWP : (HOVERPTOWP / HOVER_NAV_MAX_PITCH_RADIUS * tofinish_line) ;
		}
		else
		{
			pitchToWP = 0 ;
		}
		
		pitchAccum.WW = __builtin_mulss( rmat[8] + HOVERPOFFSET - pitchToWP + manualPitchOffset , hoverpitchgain )
					  + __builtin_mulss( hoverpitchkd , pitchrate ) ;
	}
	else
	{
		pitchAccum.WW = 0 ;
	}
	
	pitch_control = (long)pitchAccum._.W1 ;
	
	return ;
}

