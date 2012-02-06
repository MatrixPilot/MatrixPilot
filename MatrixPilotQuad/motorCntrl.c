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
#define YAW_DEADBAND 5 // prevent Tx pulse variation from causing yaw drift

extern int theta[3] ;
extern boolean didCalibrate ;
extern void matrix_normalize ( int[] ) ;
extern void MatrixRotate( int[] , int[] ) ;
extern int commanded_tilt_gain ;

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
int yaw_error ;

int roll_error_previous = 0 ;
int pitch_error_previous = 0 ;
int yaw_error_previous = 0 ;

union longww roll_error_integral = { 0 } ;
union longww pitch_error_integral = { 0 } ;
union longww yaw_error_integral = { 0 } ;

int target_orientation[9] = { RMAX , 0 , 0 , 0 , RMAX , 0 , 0 , 0 , RMAX } ;

const int yaw_command_gain = ((long) MAX_YAW_RATE )*(0.03) ;

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

	int commanded_tilt[3] ;

	int roll_error_delta ;
	int pitch_error_delta ;
	int yaw_error_delta ;

	union longww long_accum ;
//	union longww accum ; // debugging temporary

	int yaw_step ;
	int yaw_vector[3] ;
	int target_orientation_transposed[9] ;
	int orientation_error_matrix[9] ;
	
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

		motor_A = motor_B = motor_C = motor_D = pwManual[THROTTLE_INPUT_CHANNEL] ;

		VectorCopy ( 9 , target_orientation , rmat ) ;

		commanded_roll =  ( pwManual[ROLL_INPUT_CHANNEL] 
						- udb_pwTrim[ROLL_INPUT_CHANNEL]) ;
		commanded_pitch = ( pwManual[PITCH_INPUT_CHANNEL] 
						- udb_pwTrim[PITCH_INPUT_CHANNEL] ) ;
		commanded_yaw = ( pwManual[YAW_INPUT_CHANNEL] 
						- udb_pwTrim[YAW_INPUT_CHANNEL] )  ;
#ifdef CONFIG_PLUS
		commanded_pitch_body_frame = commanded_pitch ;
		commanded_roll_body_frame = commanded_roll ;
#endif

#ifdef CONFIG_X
		commanded_pitch_body_frame =  3*(( commanded_pitch - commanded_roll )/4) ; // approximation to .707, not critcal
		commanded_roll_body_frame = 3*(( commanded_pitch + commanded_roll )/4) ; 
#endif

		motor_A += + commanded_yaw - commanded_pitch_body_frame ;
		motor_B += - commanded_yaw - commanded_roll_body_frame ;
		motor_C += + commanded_yaw + commanded_pitch_body_frame ;
		motor_D += - commanded_yaw + commanded_roll_body_frame ;

		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_D ) ;

	}
	else
	{
		commanded_roll =  ( pwManual[ROLL_INPUT_CHANNEL] 
						- udb_pwTrim[ROLL_INPUT_CHANNEL])*commanded_tilt_gain ;
		commanded_pitch = ( pwManual[PITCH_INPUT_CHANNEL] 
						- udb_pwTrim[PITCH_INPUT_CHANNEL] )*commanded_tilt_gain  ;
		commanded_yaw = ( pwManual[YAW_INPUT_CHANNEL] 
						- udb_pwTrim[YAW_INPUT_CHANNEL] )  ;

		if ( commanded_yaw >= YAW_DEADBAND )
		{
			commanded_yaw -= YAW_DEADBAND ;
		}
		else if ( commanded_yaw <= - YAW_DEADBAND )
		{
			commanded_yaw += YAW_DEADBAND ;
		}
		else
		{
			commanded_yaw = 0 ;
		}

//		adjust roll and pitch commands to prevent combined tilt from exceeding 90 degrees
		commanded_tilt[0] = commanded_roll ;
		commanded_tilt[1] = commanded_pitch ;
		commanded_tilt[2] = RMAX ;
		vector3_normalize( commanded_tilt , commanded_tilt ) ;
		commanded_roll = commanded_tilt[0] ;
		commanded_pitch = commanded_tilt[1] ;

#ifdef CONFIG_PLUS

		commanded_pitch_body_frame = commanded_pitch ;
		commanded_roll_body_frame = commanded_roll ;

#endif

#ifdef CONFIG_X

		commanded_pitch_body_frame =  3*(( commanded_pitch - commanded_roll )/4) ; // approximation to .707, not critcal
		commanded_roll_body_frame = 3*(( commanded_pitch + commanded_roll )/4) ; 

#endif



//		Compute the orientation of the virtual quad (which is used only for yaw control)
//		Set the earth vertical to match in both frames (since we are interested only in yaw)

		target_orientation[6] = rmat[6] ;
		target_orientation[7] = rmat[7] ;
		target_orientation[8] = rmat[8] ;

//		renormalize to align other two axes into the the plane perpendicular to the vertical
		matrix_normalize( target_orientation ) ;
		
//		Rotate the virtual quad around the earth vertical axis according to the commanded yaw rate
		yaw_step = commanded_yaw * yaw_command_gain ;
		VectorScale( 3 , yaw_vector , &target_orientation[6] , yaw_step ) ;
		VectorAdd( 3, yaw_vector , yaw_vector , yaw_vector ) ; // doubles the vector
		MatrixRotate( target_orientation , yaw_vector ) ;

//		Compute the misalignment between target and actaul
		MatrixTranspose( 3 , 3 , target_orientation_transposed , target_orientation )	;
		MatrixMultiply ( 3 , 3 , 3 , orientation_error_matrix , target_orientation_transposed , rmat ) ;

//		Compute orientation errors
		roll_error = commanded_roll_body_frame + rmat[6] ;
		pitch_error = commanded_pitch_body_frame - rmat[7] ;
		yaw_error = ( orientation_error_matrix[1] - orientation_error_matrix[3] )/2 ;

//		Compute the signals that are common to all 4 motors
		min_throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;
		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ACCEL_K ) , accelEarth[2] ) ;
		accel_feedback = long_accum._.W1 ;
		motor_A = motor_B = motor_C = motor_D = pwManual[THROTTLE_INPUT_CHANNEL] - accel_feedback ;


//		Compute the error intetgrals
		roll_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*TILT_KI/40.), roll_error ))>>5) ;
		if ( roll_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			roll_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( roll_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			roll_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

		pitch_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*TILT_KI/40.), pitch_error ))>>5) ;
		if ( pitch_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			pitch_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( pitch_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			pitch_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

		yaw_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*YAW_KI/40.), yaw_error ))>>5) ;
		if ( yaw_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			yaw_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( yaw_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			yaw_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

//		Compute the derivatives
		theta_delta[0] = theta[0] - theta_previous[0] ;
		theta_delta[1] = theta[1] - theta_previous[1] ;

		theta_previous[0] = theta[0] ;
		theta_previous[1] = theta[1] ;

		roll_error_delta = roll_error - roll_error_previous ;
		roll_error_previous = roll_error ;

		pitch_error_delta = pitch_error - pitch_error_previous ;
		pitch_error_previous = pitch_error ;

		yaw_error_delta = yaw_error - yaw_error_previous ;
		yaw_error_previous = yaw_error ;

//		Compute the PID(DD) signals
		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KP) , roll_error ) ;
		roll_control = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KD) , roll_error_delta ) ;
		roll_control += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KDD) , -theta_delta[1] ) << 2 ;
		roll_control += long_accum._.W1 ;

		roll_control += roll_error_integral._.W1 ;


		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KP) , pitch_error ) ;
		pitch_control = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KD) , pitch_error_delta ) ;
		pitch_control += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KDD) , -theta_delta[0] ) << 2 ;
		pitch_control += long_accum._.W1 ;

		pitch_control += pitch_error_integral._.W1 ;


		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*YAW_KP) , yaw_error ) ;
		yaw_control = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*YAW_KD) , yaw_error_delta ) ;
		yaw_control += long_accum._.W1 ;

		yaw_control += yaw_error_integral._.W1 ;

//		Mix in the yaw, pitch, and roll signals into the motors
		motor_A += + yaw_control - pitch_control ;
		motor_B += - yaw_control - roll_control ;
		motor_C += + yaw_control + pitch_control ;
		motor_D += - yaw_control + roll_control ;

//		Send the signals out to the motors
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_D ) ;

	}
}


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


#if  (( ( int ) + MAX_YAW_RATE   < 50 ) || ( ( int ) + MAX_YAW_RATE > 500 ))
#error ("MAX_YAW_RATE must be between 50.0 and 500.0 degrees/second.")
#endif

#if (((int) + MAX_TILT) > 45)
#error ("MAX_TILT mus be less than or equal to 45 degrees."
#endif