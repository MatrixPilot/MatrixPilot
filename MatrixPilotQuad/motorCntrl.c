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

#define MANUAL_DEADBAND 200 // amount of throttle before fly-by-wire controls engage
#define MAXIMUM_ERROR_INTEGRAL ((long int) 32768000 )

extern int theta[3] ;
extern boolean didCalibrate ;

int roll_control ;
int pitch_control ;
int yaw_control ;
int accel_feedback ;
int theta_previous[2] = { 0 , 0 } ;
int theta_delta[2] ;

int pwManual[NUM_INPUTS+1] ;
int commanded_roll ;
int commanded_pitch ;
int commanded_yaw ;

int roll_error ;
int pitch_error ;

int roll_error_previous = 0 ;
int pitch_error_previous = 0 ;

union longww roll_error_integral = { 0 } ;
union longww pitch_error_integral = { 0 } ;

void motorCntrl(void)
{
	int temp ;
	
	int min_throttle ;
	
	int motor_A ;
	int motor_B ;
	int motor_C ;
	int motor_D ;

	int commanded_roll_body_frame ;
	int commanded_pitch_body_frame ;

	int roll_error_delta ;
	int pitch_error_delta ;

	union longww long_accum ;
	
	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];
	
	
	if (!didCalibrate)
	{
		// Leave at 0 (no PWM pulses) until calibrated.
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = 0 ;
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = 0 ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = 0 ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = 0 ;
	}
	else if (abs(pwManual[THROTTLE_INPUT_CHANNEL]-udb_pwTrim[THROTTLE_INPUT_CHANNEL])< MANUAL_DEADBAND )
	{
		commanded_roll =  ( pwManual[ROLL_INPUT_CHANNEL] 
						- udb_pwTrim[ROLL_INPUT_CHANNEL]) >> 2 ;
		commanded_pitch = ( pwManual[PITCH_INPUT_CHANNEL] 
						- udb_pwTrim[PITCH_INPUT_CHANNEL] ) >> 2 ;
		commanded_yaw = ( pwManual[YAW_INPUT_CHANNEL] 
						- udb_pwTrim[YAW_INPUT_CHANNEL] ) >> 1 ;
		motor_A = motor_B = motor_C = motor_D = pwManual[THROTTLE_INPUT_CHANNEL] ;

#ifdef CONFIG_PLUS

		motor_A += - commanded_pitch + commanded_yaw ;
		motor_B += - commanded_roll - commanded_yaw ;
		motor_C += + commanded_pitch + commanded_yaw ;
		motor_D += + commanded_roll - commanded_yaw ;

#endif

#ifdef CONFIG_X

		motor_A += ( - commanded_pitch + commanded_roll )/2 + commanded_yaw ;
		motor_B += ( - commanded_roll - commanded_pitch )/2 - commanded_yaw ;
		motor_C += ( + commanded_pitch  - commanded_roll )/2 + commanded_yaw ;
		motor_D += ( + commanded_roll + commanded_pitch )/2 - commanded_yaw ;

#endif


		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_D ) ;
	}
	else
	{
		commanded_roll =  ( pwManual[ROLL_INPUT_CHANNEL] 
						- udb_pwTrim[ROLL_INPUT_CHANNEL]) << 3 ;
		commanded_pitch = ( pwManual[PITCH_INPUT_CHANNEL] 
						- udb_pwTrim[PITCH_INPUT_CHANNEL] ) << 3 ;
		commanded_yaw = ( pwManual[YAW_INPUT_CHANNEL] 
						- udb_pwTrim[YAW_INPUT_CHANNEL] ) >> 1 ;

#ifdef CONFIG_PLUS

		commanded_pitch_body_frame = commanded_pitch ;
		commanded_roll_body_frame = commanded_roll ;

#endif

#ifdef CONFIG_X

		commanded_pitch_body_frame =  ( commanded_pitch - commanded_roll )/2 ;
		commanded_roll_body_frame = ( commanded_pitch + commanded_roll )/2 ;

#endif

		min_throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ACCEL_K ) , accelEarth[2] ) ;
		accel_feedback = long_accum._.W1 ;

		motor_A = motor_B = motor_C = motor_D = pwManual[THROTTLE_INPUT_CHANNEL] - accel_feedback ;

		roll_error = commanded_roll_body_frame + rmat[6] ;
		pitch_error = commanded_pitch_body_frame - rmat[7] ;

		roll_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*ROLL_KI/40.), roll_error ))>>5) ;


		if ( roll_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			roll_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( roll_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			roll_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

		pitch_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*PITCH_KI/40.), pitch_error ))>>5) ;


		if ( pitch_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			pitch_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( pitch_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			pitch_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

		theta_delta[0] = theta[0] - theta_previous[0] ;
		theta_delta[1] = theta[1] - theta_previous[1] ;

		theta_previous[0] = theta[0] ;
		theta_previous[1] = theta[1] ;

		roll_error_delta = roll_error - roll_error_previous ;
		roll_error_previous = roll_error ;

		pitch_error_delta = pitch_error - pitch_error_previous ;
		pitch_error_previous = pitch_error ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ROLL_KP) , roll_error ) ;
		roll_control = long_accum._.W1 ;

//		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ROLL_KD) , -theta[1] ) ;
//		roll_control += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ROLL_KD) , roll_error_delta ) ;
		roll_control += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ROLL_KDD) , -theta_delta[1] ) << 2 ;
		roll_control += long_accum._.W1 ;

		roll_control += roll_error_integral._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*PITCH_KP) , pitch_error ) ;
		pitch_control = long_accum._.W1 ;

//		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*PITCH_KD) , -theta[0] ) ;
//		pitch_control += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*PITCH_KD) , pitch_error_delta ) ;
		pitch_control += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*PITCH_KDD) , -theta_delta[0] ) << 2 ;
		pitch_control += long_accum._.W1 ;

		pitch_control += pitch_error_integral._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*YAW_KD) , -theta[2] ) ;
		yaw_control = long_accum._.W1 ;
		yaw_control += commanded_yaw ;

#ifndef CONFIG_PLUS
#ifndef CONFIG_X
#error ("You have not selected a configuration in options.h, select either CONFIG_PLUS or CONFIG_X.")
#endif
#endif

#ifdef CONFIG_PLUS
#ifdef CONFIG_X
#error ("You have selected both CONFIG_PLUS and CONFIG_X in options.h. Select just one of them."
#endif
#endif

		motor_A += + yaw_control - pitch_control ;
		motor_B += - yaw_control - roll_control ;
		motor_C += + yaw_control + pitch_control ;
		motor_D += - yaw_control + roll_control ;

		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_D ) ;
	}
}
