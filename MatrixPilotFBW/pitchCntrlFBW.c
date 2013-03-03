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
#include "autopilotCntrl.h"
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

fractional turn_rate_pitch_gain = RMAX*0.1;
fractional pos_error_rate_gain = RMAX*0.8;

int pitchrate ;
int navElevMix ;
int elevInput ;

int aspd_3DIMU_filtered = 0;

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

enum
{
	VECT_POSITION = 0,
	VECT_RATE,
};


fractional calc_pitch_error(void)
{
    struct relative2D pitchDemand = get_auto_pitchDemand();

    union longww dotprod ;
	union longww crossprod ;
	fractional actualX = rmat[8];
	fractional actualY = -rmat[7];
	fractional desiredX = pitchDemand.x ;
	fractional desiredY = pitchDemand.y ;

	dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
	crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
	crossprod.WW = crossprod.WW<<2 ; // at this point, we have 1/2 of the cross product
									// cannot go any higher than that, could get overflow
	if ( dotprod._.W1 > 0 )
	{
		desiredY = crossprod._.W1;
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			desiredY = RMAX ;
		}
		else
		{
			desiredY = -RMAX ;
		}
	}
	
//	dotprod.WW = __builtin_mulss( rmat[6] , desiredY ) << 2;
	return desiredY;
}

void normalPitchCntrl(void)
{
	// controls for position and rate
	int pitch_error;	
	union longww rateAccum;
	union longww posAccum;
	union longww accn;
	union longww temp;
	int Cl;
	int aoa;
	fractional output_gain[2];			// Gain from accumulator to output

	aspd_3DIMU_filtered >>= 1;
	aspd_3DIMU_filtered += air_speed_3DIMU >> 1;

// Do basic lift/acceleration feedforward calculation

	// Multiply Q16 scaled acceleration by GRAVITY
	// Divide first to get headroom for 16g
	temp.WW = get_earth_turn_accn() >> 4;
	temp.WW = limitRMAX(temp.WW);
	temp.WW = __builtin_mulss( temp._.W0 , GRAVITY ) << 4;

	// Find total lift acceleration by root sum squares of centripetal plus gravity
	posAccum.WW = __builtin_mulss( temp._.W1 , temp._.W1 );
	
	// GRAVITY multiplied by pitch rotation then squared
//	temp.WW = __builtin_mulss( GRAVITY , rmat[8] ) << 2;
//	posAccum.WW += __builtin_mulss( temp._.W1 , temp._.W1 );
	posAccum.WW +=	(GRAVITY * GRAVITY);

	accn.WW = (long) sqrt_long(posAccum.WW);

// Do rotation rate calculation
	rateAccum.WW = calc_turn_pitch_rate( get_earth_turn_rate(), rmat[6]);

	// Rescale rotation rate from RMAX to gyro scale
	rateAccum.WW = limitRMAX(rateAccum.WW);
	rateAccum.WW = __builtin_mulss( (5632.0/SCALEGYRO) , rateAccum._.W0 ) >> 14 ;

// Pitch error correction to pitch acceleration adjustment

	pitch_error = calc_pitch_error();

	// Gain for pitch error correction due to roll rotation
	temp.WW =  __builtin_mulss( rmat[6] , rmat[6] ) << 2;
	// rmat[6] is zero with no roll.  Can't use rmat 8 since rmat 8
	// is zero at zero pitch resulting in no pitch gain.
	posAccum.WW = __builtin_mulss( RMAX - temp._.W1 , pitch_error ) << 1 ;

	// position error to rate demand times user gain
	// User gain controls settling time of position error
	posAccum.WW = __builtin_mulss( posAccum._.W1 , pos_error_rate_gain ) << 2 ;
	accn.WW += posAccum._.W1;		// TODO - REMOVE
	accn.WW = limitRMAX(accn.WW);

	rateAccum.WW += posAccum._.W1;
	rateAccum.WW = limitRMAX(rateAccum.WW);

	rateAccum.WW += (long) omegagyro[0];
	rateAccum.WW = limitRMAX(rateAccum.WW);

// Turn rate error into a delta in acceleration by multiplying by airspeed
	rateAccum.WW = __builtin_mulss( rateAccum._.W0 , air_speed_3DIMU ) << 2 ;
	//scale result into accelerometer units of GRAVITY and m/s instead of cm/s

	// First divide by 2048 to range back to RMAX
	rateAccum.WW >>= 11;
	rateAccum.WW = limitRMAX(rateAccum.WW);
	rateAccum.WW = __builtin_mulss( rateAccum._.W0 , (1024.0 * GRAVITY / 100.0) ) << 2 ;
	rateAccum.WW = limitRMAX(rateAccum._.W1);

// Adjust required acceleration with the feedback
//	accn += rateAccum._.W0;

//	accn = GRAVITY;		//TODO - REMOVE THIS

	if( accn.WW > (GRAVITY * MAX_G_POSITIVE))
		accn.WW = (GRAVITY * MAX_G_POSITIVE);
	else if( accn.WW < (GRAVITY * -MAX_G_NEGATIVE))
		accn.WW = (GRAVITY * -MAX_G_NEGATIVE);

	if ( PITCH_STABILIZATION && mode_autopilot_enabled() )
	{
//		pitchAccum.WW =Get  __builtin_mulss( pitchAccum._.W0 , pitchgain ) << 2; 
//					  + __builtin_mulss( pitchkd , pitchrate ) << 2;
//		posAccum.WW = __builtin_mulss( pitchkd , pitchrate ) << 3;

		// Calculate the required angle of attack
		// TODO - TARGET OR ACTAL AIRSPEED???
		Cl = afrm_get_required_Cl(aspd_3DIMU_filtered , accn._.W0);
		aoa = afrm_get_required_alpha(aspd_3DIMU_filtered , Cl);

		posAccum._.W0 = loopkup_elevator_control( 250 - aoa );  	//546 // (AFRM_NEUTRAL_PITCH * 182 ) 182 = (RMAX / 90.0)
		posAccum.WW = -limitRMAX(posAccum._.W0);					// Output control is negative!
	}
	else
	{
		posAccum.WW = 0 ;
	}

	ap_cntrls[AP_CNTRL_PITCH] = posAccum._.W0;

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
