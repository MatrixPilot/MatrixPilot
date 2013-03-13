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
#include "minifloat.h"
#include <libq.h> /* include fixed point library */

// earth horizontal turn acceleration in Q16 g
_Q16 earth_turn_accn = 0;

// Predicted earth turn rate to achieve a balanced turn given a bank angle
minifloat earth_turn_rate = {0,0};

fractional earth_roll_angle = 0;
fractional earth_pitch_angle = 0;

// The pitch adjustment required to achieve setpoint airspeed.
fractional aspd_pitch_adj = 0;


#define EARTH_ROLL_90DEG_LIM (64 << 8)

// Acess functions
inline signed char get_airspeed_pitch_adjustment(void) {return aspd_pitch_adj;}

inline _Q16 get_earth_turn_rate(void) {return 0;};

inline _Q16 get_earth_turn_accn(void) {return earth_turn_accn;};

inline minifloat get_earth_turn_rate_mf(void) {return earth_turn_rate;};



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
	earth_turn_accn = _Q16tan( temp.WW);

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
// returns miniflaot rad/s
minifloat calc_earth_turn_rate(_Q16 earth_turn_g, int airspeed)
{
	union longww temp;
	minifloat aspd_mf;
	minifloat rate_mf = {0,0};

	// Constant to convert from cm/s to m/s and from g to m/s^2
	const minifloat scale_const = ftomf( (float) (0.01 * (1/9.81) ) );

	if(airspeed < 100)
		return rate_mf;

	aspd_mf = ltomf(airspeed);
	aspd_mf = mf_mult(aspd_mf, scale_const);

	rate_mf = Q16tomf(earth_turn_g);
	rate_mf = mf_div( rate_mf, aspd_mf);
	
	return rate_mf;
};

// Calculate the pitch rate due to turning when banked
// bank angle in fractional Q14 from dcm. Normally rmat[6]
// Turn rate in 16*byte circular per second.
minifloat calc_turn_pitch_rate(minifloat earth_turn_rate, fractional bank_angle)
{
	union longww temp;
	minifloat bank_mf;
	minifloat rate_mf;

	// Convert RMAX fractional sin(bank_angle) to minifloat
	bank_mf = RMAXtomf(bank_angle);
	
	rate_mf = mf_mult(earth_turn_rate , bank_mf );

	return rate_mf;
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
