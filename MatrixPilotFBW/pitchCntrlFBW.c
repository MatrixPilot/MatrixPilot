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

//#define RTLKICK ((int32_t)(RTL_PITCH_DOWN*(RMAX/57.3)))
//#define INVNPITCH ((int32_t)(INVERTED_NEUTRAL_PITCH*(RMAX/57.3)))
//#define HOVERPOFFSET ((int32_t)(HOVER_PITCH_OFFSET*(RMAX/57.3)))
//#define HOVERPTOWP ((int32_t)(HOVER_PITCH_TOWARDS_WP*(RMAX/57.3)))

//int pitchgain = (int16_t)(PITCHGAIN*RMAX) ;
//int pitchkd = (int16_t) (PITCHKD*SCALEGYRO*RMAX) ;
//int hoverpitchgain = (int16_t)(HOVER_PITCHGAIN*RMAX) ;
//int hoverpitchkd = (int16_t) (HOVER_PITCHKD*SCALEGYRO*RMAX) ;
//int rudderElevMixGain = (int16_t)(RMAX*RUDDER_ELEV_MIX) ;
//int rollElevMixGain = (int16_t)(RMAX*ROLL_ELEV_MIX) ;


int32_t rate_error_load_gain = (AFRM_Q16_SCALE*0.1);
int32_t pitch_error_rate_gain = (AFRM_Q16_SCALE*20);

int16_t pitchrate ;
int16_t navElevMix ;
int16_t elevInput ;

minifloat total_load = {128,1};

minifloat wing_aoa;				// Wing angle of attack
minifloat wing_Cl;				// Wing angle of attack

// elevator pitch correction, rates and conditions
int32_t aoa_offset_correction = 0;
int32_t aoa_offset_correction_rate = 0;

int32_t aoa_offset_correction_max_load = (AFRM_Q16_SCALE*1.1);
int32_t aoa_offset_correction_min_load = (AFRM_Q16_SCALE*0.9);

int32_t aoa_offset_correction_max_Cl = (AFRM_Q16_SCALE*0.23);
int32_t aoa_offset_correction_min_Cl = (AFRM_Q16_SCALE*0.12);

minifloat calc_pitch_error(void); // Calculate pitch error in minifloat radians

void normalPitchCntrl(void) ;
void hoverPitchCntrl(void) ;

minifloat loadCalc(void);
void loadErrorCorrection(minifloat measured_load, minifloat calculated_load, minifloat Cl);


void pitchCntrl(void)
{
	minifloat actual_load = loadCalc();
	loadErrorCorrection(actual_load, total_load, wing_Cl);
	normalPitchCntrl() ;
}
//
//enum
//{
//	VECT_POSITION = 0,
//	VECT_RATE,
//};
//

minifloat loadCalc(void)
{
	minifloat load;
	const minifloat accn_scale = ftomf(SCALEGYRO / 5632.0);
	load = ltomf(gplane_raw[2]);
	load = mf_mult(load, accn_scale);
	return load;
}

void loadErrorCorrection(minifloat measured_load, minifloat calculated_load, minifloat Cl)
{
	_Q16 temp = mftoQ16(Cl);
	if(temp > aoa_offset_correction_max_Cl)
		return;
	if(temp < aoa_offset_correction_min_Cl)
		return;

	temp = mftoQ16(calculated_load);
	if(temp > aoa_offset_correction_max_load)
		return;
	if(temp < aoa_offset_correction_min_load)
		return;

	minifloat gain = Q16tomf(aoa_offset_correction_rate);
	minifloat error = mf_add(mf_inv(calculated_load), measured_load);
	error = mf_mult(error, gain);
	aoa_offset_correction += mftoQ16(error);
}

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
	minifloat tail_aoa;			// Tail aoa
	minifloat tail_angle;		// Tail angle
	minifloat tempmf;			// temporary minifloat;

	minifloat pitch_error_pitch_mf;	// Pitch error in aircraft pitch axis

	union longww temp;
	_Q16	Q16temp;

	// Scale of radians/s per AD converter unit
	const minifloat gyro_radians_scale = ftomf(SCALEGYRO / 5632.0);

	int16_t aspd_3DIMU_filtered = get_filtered_airspeed();

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

// Limit the requested load to the safe aircraft limit.
	load = afrm_load_limit(load);

	if ( PITCH_STABILIZATION && mode_autopilot_enabled() )
	{
//		pitchAccum.WW =Get  __builtin_mulss( pitchApccum._.W0 , pitchgain ) << 2; 
//					  + __builtin_mulss( pitchkd , pitchrate ) << 2;
//		posAccum.WW = __builtin_mulss( pitchkd , pitchrate ) << 3;

		// Calculate the required angle of attack
		// TODO - TARGET OR ACTAL AIRSPEED???
		wing_Cl = afrm_get_required_Cl_mf( aspd_3DIMU_filtered , load);
		wing_aoa = afrm_get_required_alpha_mf(aspd_3DIMU_filtered, wing_Cl);

		minifloat Clmf_tail = afrm_get_tail_required_Cl_mf(wing_aoa);

		tail_aoa = afrm_get_tail_required_alpha(Clmf_tail);

		// calculate required tail angle as wing pitch - wing aoa - tail aoa
		tail_angle = mf_sub( ftomf(AFRM_NEUTRAL_PITCH) , wing_aoa);
		tail_angle = mf_add( tail_angle , tail_aoa );
		tail_angle = mf_add( tail_angle, Q16tomf(aoa_offset_correction) );

		minifloat tail_load = afrm_calc_tail_load(aspd_3DIMU_filtered, Clmf_tail);
		total_load = mf_add(load, tail_load);

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
//		int16_t elevInput = ( udb_flags._.radio_on == 1 ) ? REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, udb_pwIn[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) : 0 ;
//		int16_t manualPitchOffset = elevInput * (int16_t)(RMAX/600);
//		
//		int32_t pitchToWP ;
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
//	pitch_control = (int32_t)pitchAccum._.W1 ;
	
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
