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
#include "minifloat.h"

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


long rate_error_load_gain = (AFRM_Q16_SCALE*0.1);
long pitch_error_rate_gain = (AFRM_Q16_SCALE*20);

int pitchrate ;
int navElevMix ;
int elevInput ;

int aspd_3DIMU_filtered = 0;	/// Filtered airspeed for less lumpiness

minifloat calc_pitch_error(void); // Calculate pitch error in minifloat radians

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

void normalPitchCntrl(void)
{
	// controls for position and rate
	minifloat pitch_error;	

	// Rad/s demand rate 
	minifloat rate_demand;

	// Rate error in rad/s
	minifloat rate_error;

	union longww posAccum;

	minifloat load;				// Wing load in unity gravity units
	minifloat Cl;				// Wing coefficient of lift
	minifloat aoa;				// Wing angle of attack
	minifloat tail_aoa;			// Tail aoa
	minifloat tail_angle;		// Tail angle
	minifloat tempmf;			// temporary minifloat;

	minifloat pitch_error_mf;
	minifloat pitch_error_pitch_mf;	// Pitch error in aircraft pitch axis

	union longww temp;
	_Q16	Q16temp;

	// Scale of radians/s per AD converter unit
	const minifloat gyro_radians_scale = ftomf(SCALEGYRO / 5632.0);
	const minifloat airspeed_cm_m_scale = ftomf(0.01);

	aspd_3DIMU_filtered >>= 1;
	aspd_3DIMU_filtered += air_speed_3DIMU >> 1;

	minifloat aspmf_filtered = ltomf(aspd_3DIMU_filtered);
	tempmf = ftomf(0.01);
	aspmf_filtered = mf_mult(aspmf_filtered, tempmf);

// Do basic lift/acceleration feedforward calculation

	// SQRT( Earth turn acceleration^2 + GRAVITY^2)

	Q16temp = get_earth_turn_accn();
	load = Q16tomf(Q16temp);
	load = mf_mult(load, load);

	// Gravity is unity so no point squaring it.
	tempmf = ftomf(1.0);
	load = mf_add(load, tempmf);

	load = mf_sqrt(load);

	// Pitch error in radians
	pitch_error = calc_pitch_error();
//	pitch_error = ftomf(0);		// TODO remove

	// Gain for pitch error correction due to roll rotation
	temp.WW =  __builtin_mulss( rmat[6] , rmat[6] ) << 2;
	// rmat[6] is zero with no roll.  Can't use rmat 8 since rmat 8
	// is zero at zero pitch resulting in no pitch gain.
	tempmf = RMAXtomf(RMAX - temp._.W1);
	pitch_error_pitch_mf = mf_mult( tempmf , pitch_error );

// Rate error calculations
	// Do rotation rate calculation
	rate_demand = calc_turn_pitch_rate(get_earth_turn_rate_mf() , rmat[6] );

	// pitch error to rate demand times user gain
	// User gain controls settling time of position error
	tempmf = mf_mult(pitch_error_pitch_mf, Q16tomf(pitch_error_rate_gain) ); 
	rate_demand = mf_add(rate_demand , tempmf );

	// Scale gyro to radians in minifloat
	tempmf = ltomf(omegagyro[0]);
	tempmf = mf_mult(gyro_radians_scale, tempmf);

	// Add rate demand to rate feedback to get total rate error
	rate_error = mf_add( rate_demand, tempmf);
	rate_error = mf_mult(rate_error, Q16tomf(rate_error_load_gain) );

// Turn rate error into a delta in load by multiplying by airspeed in m/s
	tempmf = mf_mult(aspmf_filtered, rate_error);

// Adjust required load with the rate error feedback
	load = mf_add(load, tempmf);

// TODO - fix this	
//	if( accn.WW > (GRAVITY * MAX_G_POSITIVE))
//		accn.WW = (GRAVITY * MAX_G_POSITIVE);
//	else if( accn.WW < (GRAVITY * -MAX_G_NEGATIVE))
//		accn.WW = (GRAVITY * -MAX_G_NEGATIVE);

	if ( PITCH_STABILIZATION && mode_autopilot_enabled() )
	{
//		pitchAccum.WW =Get  __builtin_mulss( pitchApccum._.W0 , pitchgain ) << 2; 
//					  + __builtin_mulss( pitchkd , pitchrate ) << 2;
//		posAccum.WW = __builtin_mulss( pitchkd , pitchrate ) << 3;

		// Calculate the required angle of attack
		// TODO - TARGET OR ACTAL AIRSPEED???
		minifloat Clmf = afrm_get_required_Cl_mf( aspd_3DIMU_filtered , load);
		aoa = afrm_get_required_alpha_mf(aspd_3DIMU_filtered, Clmf);

		minifloat Clmf_tail = afrm_get_tail_required_Cl_mf(aoa);

		tail_aoa = afrm_get_tail_required_alpha(Clmf_tail);

		// calculate required tail angle as wing pitch - wing aoa - tail aoa
		tail_angle = mf_sub( ftomf(AFRM_NEUTRAL_PITCH) , aoa);
		tail_angle = mf_add( tail_angle , tail_aoa );

		posAccum._.W0 = lookup_elevator_control( tail_angle );
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



// Calculate pitch error in minifloat radians
minifloat calc_pitch_error(void)
{
	minifloat 	pitch_error;
	_Q16		Q16temp;

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

	if(desiredY > RMAX) desiredY = RMAX;
	if(desiredY < -RMAX) desiredY = RMAX;

	// Convert ratio to radians
	Q16temp = ((_Q16) desiredY) << 2;
	Q16temp = _Q16asin(desiredY);

	// convert to minifloat
	pitch_error = Q16tomf(Q16temp);
	return pitch_error;
}
