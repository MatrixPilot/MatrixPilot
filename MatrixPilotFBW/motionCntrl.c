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

// Motion control module
// Responsible for calculating aircraft movements

// trigonomentry lookup tables are at the bottom.


#include "../MatrixPilot/defines.h"
#include "motionCntrl.h"
#include "airspeedCntrlFBW.h"
#include "airframe.h"
#include <libq.h> /* include fixed point library */

// earth horizontal turn acceleration in g
_Q16 earth_turn_accn = 0;

// Predicted earth turn rate to achieve a balanced turn given a bank angle
// Angular rate in Q16
_Q16 earth_turn_rate = 0;

fractional earth_roll_angle = 0;
fractional earth_pitch_angle = 0;

// The pitch adjustment required to achieve setpoint airspeed.
fractional aspd_pitch_adj = 0;


#define EARTH_ROLL_90DEG_LIM (64 << 8)

// Calculations for required motion before axis control are performed.
void motionCntrl(void)
{
	// Calculate earth based roll angle
	struct relative2D matrix_accum ;
	matrix_accum.x = rmat[8] ;
	matrix_accum.y = rmat[6] ;
	earth_roll_angle = rect_to_polar16(&matrix_accum) ;

	matrix_accum.x = rmat[8] ;
	matrix_accum.y = rmat[7] ;
	earth_pitch_angle = - rect_to_polar16(&matrix_accum) ;			// binary angle (0 to 65536 = 360 degrees)

	union longww temp = {0};
	temp.WW = __builtin_mulss ( RMAX * PI/2 , earth_roll_angle ) >> 12;	
	temp.WW = _Q16tan( temp.WW);

//        // Correct direction of rotation when inverted
//        if(earth_roll_angle > EARTH_ROLL_90DEG_LIM )
//            earth_turn_accn = tansf( (EARTH_ROLL_90DEG_LIM - earth_roll_angle) >> 8) ;
//        else if(earth_roll_angle < -EARTH_ROLL_90DEG_LIM )
//            earth_turn_accn = tansf( (EARTH_ROLL_90DEG_LIM + earth_roll_angle) >> 8) ;
//        else
//            earth_turn_accn = tansf(earth_roll_angle >> 8) ;

	earth_turn_rate = calc_earth_turn_rate(earth_turn_accn , air_speed_3DIMU) ;

	// throttle_control used as a bodge because ap and manual are not mixed yet.  TODO.  Tidy this.
	aspd_pitch_adj = airspeed_pitch_adjust(throttle_control, air_speed_3DIMU, target_airspeed, minimum_airspeed, get_speed_height_error());

}

inline signed char get_airspeed_pitch_adjustment(void) {return aspd_pitch_adj;}

//inline fractional get_earth_roll_angle(void) {return earth_roll_angle;}
//inline fractional get_earth_pitch_angle(void) {return earth_pitch_angle;}

inline _Q16 get_earth_turn_rate(void) {return earth_turn_rate;};

inline _Q16 get_earth_turn_accn(void) {return earth_turn_accn;};


// Centripetal accelration for given airspeed and rotation
// airspeed in cm/s
// rotation rate in RMAX/PI() rad/s
int calc_reqd_centripetal_accn(int airspeed, int rotation_rate)
{
	union longww temp;
	// Convert from cm/s to m/s
	temp.WW = __builtin_mulss (airspeed , (RMAX * 0.01) ) ;
	temp.WW <<= 2;
	if(temp._.W0 & 0x8000)
		temp._.W1++;

	// Multiply airspeed by acceleration scaling
	temp.WW = __builtin_mulss ( (GRAVITY / AFRM_GRAVITY) , temp._.W1 ) ;

	temp.WW = __builtin_mulss ( rotation_rate , temp._.W0 ) << 2;	
	
	return temp._.W1;
}


// Calculate the estimated earth based turn rate in byte circular per second.
// This is based on airspeed and bank angle for level flight.
// Takes airspeed as cm/s and acceleration as Q16 scale / g
// returns byte circular*16
_Q16 calc_earth_turn_rate(_Q16 earth_turn_g, int airspeed)
{
	union longww temp;

	// Convert from cm/s to dm/s
	temp.WW = __builtin_mulss (airspeed , (RMAX * 0.1) ) ;
	temp.WW <<= 2;
	if(temp._.W0 & 0x8000)
		temp._.W1++;

	// If airspeed is zero, return zero
	if(temp._.W1 == 0)
		return 0;

	// Divide acceleration by airpseed to get angular rate
	// Angular rate result is scaled by RMAX/(g*10)
	// Maximum scale is 20g
	temp._.W1 = __builtin_divsd ( earth_turn_g, temp._.W1 ) ;
	temp._.W0 = 0x8000;

	// Multiply by G acceleration and rerange for dm to m airspeed
	temp.WW = __builtin_mulss (temp._.W1 , (9.81 * 10.0) );

	return temp.WW;
};

// Calculate the pitch rate due to turning when banked
// bank angle in fractional Q14 from dcm. Normally rmat[6]
// Turn rate in 16*byte circular per second.
int calc_turn_pitch_rate(_Q16 turn_rate, fractional bank_angle)
{
	union longww temp;

	// Divide turn rate to get some more range
	temp.WW = turn_rate >> 1;
	temp.WW = limitRMAX(temp.WW);

	temp.WW = __builtin_mulss (bank_angle , temp._.W0 ) ;
	temp.WW <<= 3;
	if(temp._.W0 & 0x8000)
		temp._.W1++;
	return temp._.W1;
}


// Calculate yaw rate due to turning when banked
// bank angle is normally rmat[8]
// TODO - fix this, it might be wrong to use rmat[8]
int calc_turn_yaw_rate(fractional bank_angle, int turn_rate)
{
	union longww temp;
	temp.WW = __builtin_mulss (bank_angle , turn_rate ) ;
	temp.WW <<= 2;
	if(temp._.W0 & 0x8000)
		temp._.W1++;
}

// return the RMAX scale control requried for an required elevator pitch
fractional loopkup_elevator_control( fractional elev_pitch )
{
	int index;
	// Make sure that if the angle is out of bounds then the limits are returned
	if(elev_pitch < elevator_angles[0].surface_deflection)
		return elevator_angles[0].ap_control;

	if(elev_pitch > elevator_angles[elevator_angle_points - 1].surface_deflection)
		return elevator_angles[elevator_angle_points - 1].ap_control;

	index = elevator_angle_points - 1;
	while(elev_pitch < elevator_angles[index - 1].surface_deflection)
	{
		index--;
	}

	return successive_interpolation(elev_pitch, 
			elevator_angles[index-1].surface_deflection,
			elevator_angles[index].surface_deflection, 
			elevator_angles[index-1].ap_control,
			elevator_angles[index].ap_control);
}