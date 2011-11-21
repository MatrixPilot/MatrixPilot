// This file is part of the MatrixPilotQuad firmware.
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

#include "../libDCM/libDCM.h"


extern int theta[3] ;


int roll_feedback ;
int pitch_feedback ;
int yaw_feedback ;
int accel_feedback ;
int theta_previous[2] = { 0 , 0 } ;
int theta_delta[2] ;


void motorCntrl(void)
{
	int pwManual[NUM_INPUTS+1] ;
	
	int commanded_roll ;
	int commanded_pitch ;
	int commanded_yaw ;


	int min_throttle ;

	int motor_A ;
	int motor_B ;
	int motor_C ;
	int motor_D ;

	union longww long_accum ;
	
	if (!dcm_flags._.calib_finished)
	{
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = 3000 ;
	}
	else
	{
		commanded_roll =  ( pwManual[ROLL_INPUT_CHANNEL] 
						- udb_pwTrim[ROLL_INPUT_CHANNEL]) >> 2 ;
		commanded_pitch = ( pwManual[PITCH_INPUT_CHANNEL] 
						- udb_pwTrim[PITCH_INPUT_CHANNEL] ) >> 2 ;
		commanded_yaw = ( pwManual[YAW_INPUT_CHANNEL] 
						- udb_pwTrim[YAW_INPUT_CHANNEL] ) >> 1 ;

		min_throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ACCEL_K ) , accelEarth[2] ) ;
		accel_feedback = long_accum._.W1 ;

		motor_A = motor_B = motor_C = motor_D = pwManual[THROTTLE_INPUT_CHANNEL] - accel_feedback ;

		theta_delta[0] = theta[0] - theta_previous[0] ;
		theta_delta[1] = theta[1] - theta_previous[1] ;

		theta_previous[0] = theta[0] ;
		theta_previous[1] = theta[1] ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ROLL_KP) , -rmat[6] ) ;
		roll_feedback = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ROLL_KD) , theta[1] ) ;
		roll_feedback += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ROLL_KDD) , theta_delta[1] ) << 2 ;
		roll_feedback += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*PITCH_KP) , rmat[7] ) ;
		pitch_feedback = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*PITCH_KD) , theta[0] ) ;
		pitch_feedback += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*PITCH_KDD) , theta_delta[0] ) << 2 ;
		pitch_feedback += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*YAW_KD) , theta[2] ) ;
		yaw_feedback = long_accum._.W1 ;

		motor_A += commanded_pitch + commanded_yaw - pitch_feedback - yaw_feedback ;  // forward  CCW
		motor_B += commanded_roll - commanded_yaw - roll_feedback + yaw_feedback ;    // blue      CW
		motor_C += -commanded_pitch + commanded_yaw + pitch_feedback - yaw_feedback ; // yellow   CCW
		motor_D += -commanded_roll - commanded_yaw + roll_feedback + yaw_feedback ;   // green     CW

		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_D ) ;
	}
}
