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
#include "airspeedCntrlFBW.h"
#include "airframe.h"
#include "fbw_options.h"
#include "inputCntrl.h"
#include "motionCntrl.h"

//	If the state machine selects pitch feedback, compute it from the pitch gyro and accelerometer.

#define ANGLE_90DEG (RMAX/(2*57.3))

//#define RTLKICK ((long)(RTL_PITCH_DOWN*(RMAX/57.3)))
#define INVNPITCH ((long)(INVERTED_NEUTRAL_PITCH*(RMAX/57.3)))
#define HOVERPOFFSET ((long)(HOVER_PITCH_OFFSET*(RMAX/57.3)))
#define HOVERPTOWP ((long)(HOVER_PITCH_TOWARDS_WP*(RMAX/57.3)))

int pitchgain = (int)(PITCHGAIN*RMAX) ;
int pitchkd = (int) (PITCHKD*SCALEGYRO*RMAX) ;
int hoverpitchgain = (int)(HOVER_PITCHGAIN*RMAX) ;
int hoverpitchkd = (int) (HOVER_PITCHKD*SCALEGYRO*RMAX) ;
int rudderElevMixGain = (int)(RMAX*RUDDER_ELEV_MIX) ;
int rollElevMixGain = (int)(RMAX*ROLL_ELEV_MIX) ;
	
int alt_hold_pitch_min = ALT_HOLD_PITCH_MIN*(RMAX/57.3);
int alt_hold_pitch_max = ALT_HOLD_PITCH_MAX*(RMAX/57.3);

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
	union longww rateAccum ;
	union longww pcntrl;

//	int aspd_adj ;
//	fractional aspd_err, aspd_diff ;
	
#ifdef TestGains
	flags._.GPS_steering = 0 ; // turn navigation off
	flags._.pitch_feedback = 1 ; // turn stabilization on
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

	// Calculate turn rate with airspeed and bank angle

			// binary angle (0 to 65536 = 360 degrees)

	int turnRate = calc_turn_pitch_rate( get_earth_turn_rate(), rmat[6]);
	
	navElevMix = 0 ;
//	if ( flags._.pitch_feedback )
//	{
//		if ( RUDDER_OUTPUT_CHANNEL != CHANNEL_UNUSED && RUDDER_INPUT_CHANNEL != CHANNEL_UNUSED ) {
//			pitchAccum.WW = __builtin_mulss( rmat6 , rudderElevMixGain ) << 1 ;
//			pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 ,
//				REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, udb_pwTrim[RUDDER_INPUT_CHANNEL] - udb_pwOut[RUDDER_OUTPUT_CHANNEL]) ) << 3 ;
//			navElevMix += pitchAccum._.W1 ;
//		}
//		
//		pitchAccum.WW = __builtin_mulss( rmat6 , rollElevMixGain ) << 1 ;
//		pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 , rmat[6] ) >> 3 ;
//		navElevMix += pitchAccum._.W1 ;
//	}

//	if ( flags._.pitch_feedback )
//	{
//		if(turnRate > 0)
//			pitchAccum.WW = __builtin_mulss( turnRate , rollElevMixGain ) ; // << 10 ;
//		else
//			pitchAccum.WW = __builtin_mulss( -turnRate , rollElevMixGain ) ; // << 10 ;
//
//		if(pitchAccum.WW > RMAX)
//			pitchAccum.WW = RMAX;
//		else if(pitchAccum.WW < -RMAX)
//			pitchAccum.WW = -RMAX;
//
//		navElevMix += pitchAccum._.W0 ;
//	}

	// cos(roll angle) * pitch gyro;
//	pitchAccum.WW = ( __builtin_mulss( rmat8 , omegagyro[0] ) ;
//					- __builtin_mulss( rmat6 , omegagyro[2] )) << 1


	
//	fractional pitch_rate_limit = RMAX * sqrt(2*PI()*g/v)

	// throttle_control used as a bodge because ap and manual are not mixed yet.  TODO.  Tidy this.
	fractional aspd_pitch_adj  = (fractional) airspeed_pitch_adjust(throttle_control, air_speed_3DIMU, target_airspeed, minimum_airspeed, get_speed_height());
	aspd_pitch_adj <<= 7;		// Scale byte circular up to fractional

	if(aspd_pitch_adj > alt_hold_pitch_max)
		aspd_pitch_adj = alt_hold_pitch_max;
	else if(aspd_pitch_adj < alt_hold_pitch_min)
		aspd_pitch_adj = alt_hold_pitch_min;

	pitchAccum.WW = (long) aspd_pitch_adj - (long) get_earth_pitch_angle();

	if(pitchAccum.WW > RMAX)
		pitchAccum.WW = RMAX;
	else if(pitchAccum.WW < -RMAX)
		pitchAccum.WW = -RMAX;


	rateAccum.WW = (long) turnRate;
	// Feed pitch error into pitch rate demand
	rateAccum.WW += __builtin_mulss( pitchAccum._.W0 , RMAX*0.25 ) >> 14 ;
	rateAccum.WW = limitRMAX(rateAccum.WW);

	// Now we have the pitch rate demand, use it to feedforward into pitch
	pitchAccum.WW += __builtin_mulss( rateAccum._.W0 , RMAX*0.5 ) >> 11 ;  // 10?
	pitchAccum.WW = limitRMAX(pitchAccum.WW);

	rateAccum.WW += (long) omegagyro[0];
	rateAccum.WW = limitRMAX(rateAccum.WW);

	pitchrate = rateAccum._.W0 ;
	

	if ( PITCH_STABILIZATION && mode_autopilot_enabled() )
	{
		pitchAccum.WW = __builtin_mulss( pitchAccum._.W0 , pitchgain ) 
					  + __builtin_mulss( pitchkd , pitchrate ) ;
	}
	else
	{
		pitchAccum.WW = 0 ;
	}
	

	pcntrl.WW = (long)pitchAccum._.W1 + (long) navElevMix ;

	if(pcntrl.WW > RMAX)
		pcntrl.WW = RMAX;
	else if(pcntrl.WW < -RMAX)
		pcntrl.WW = -RMAX;

	pitch_control = pcntrl._.W0 ;
	ap_cntrls[AP_CNTRL_PITCH]		= PWM_to_frac(pitch_control		,0	, false);

	return ;
}



void hoverPitchCntrl(void)
{
//	union longww pitchAccum ;
//	
//	if ( flags._.pitch_feedback )
//	{
//		pitchAccum.WW = ( __builtin_mulss( -rmat[7] , omegagyro[0] )
//						- __builtin_mulss( rmat[6] , omegagyro[1] )) << 1 ;
//		pitchrate = pitchAccum._.W1 ;
//		
//		int elevInput = ( udb_flags._.radio_on == 1 ) ? REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, udb_pwIn[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) : 0 ;
//		int manualPitchOffset = elevInput * (int)(RMAX/600);
//		
//		long pitchToWP ;
//		
//		if ( flags._.GPS_steering )
//		{
//			pitchToWP = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS) ? HOVERPTOWP : (HOVERPTOWP / HOVER_NAV_MAX_PITCH_RADIUS * tofinish_line) ;
//		}
//		else
//		{
//			pitchToWP = 0 ;
//		}
//		
//		pitchAccum.WW = __builtin_mulss( rmat[8] + HOVERPOFFSET - pitchToWP + manualPitchOffset , hoverpitchgain )
//					  + __builtin_mulss( hoverpitchkd , pitchrate ) ;
//	}
//	else
//	{
//		pitchAccum.WW = 0 ;
//	}
//	
//	pitch_control = (long)pitchAccum._.W1 ;
	
	return ;
}
