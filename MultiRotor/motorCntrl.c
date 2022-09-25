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
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/rmat.h"
#include "../libDCM/mathlib.h"
#include "../libDCM/mathlibNAV.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/ADchannel.h"
#include "rotorLib.h"

// Used for serial debug output
#include <stdio.h>

#define MANUAL_DEADBAND 200 // amount of throttle before fly-by-wire controls engage
#define MAXIMUM_ERROR_INTEGRAL ((long int) 32768000 )
#define YAW_DEADBAND 5 // prevent Tx pulse variation from causing yaw drift

extern union longww IMUlocationx , IMUlocationy , IMUvelocityx , IMUvelocityy ;

int theta[3] ;
extern boolean didCalibrate ;
extern int commanded_tilt_gain ;
extern void compute_tilt_rmat( int [] , int , int );
extern void initialize_yaw_rmat(void);
void compute_altitude_control(void);
extern int lidar_pulses ;
int number_pulses = 0 ;

int roll_control ;
int pitch_control ;
int yaw_control ;
int altitude_control = 0;
uint16_t altitude = 0 ;
int previous_altitude = 0 ;
int32_t climb_rate = 0 ;
int accel_feedback ;
int theta_previous[2] = { 0 , 0 } ;
int theta_delta[2] ;
extern int IMU_climb ;
extern int IMU_altitude ;
int16_t target_climb_rate ;
int16_t target_altitude ;

int pwManual[NUM_INPUTS+1] ;
int commanded_roll ;
int commanded_pitch ;
int commanded_yaw ;
int x_velocity_feedback ;
int y_velocity_feedback ;
int16_t x_velocity_target , y_velocity_target ;
int16_t x_position_target = 0 , y_position_target = 0 ;
int16_t delta_position[3] ;

int roll_error ;
int pitch_error ;
int yaw_error ;

int roll_error_previous = 0 ;
int pitch_error_previous = 0 ;
int yaw_error_previous = 0 ;

union longww roll_error_integral = { 0 } ;
union longww pitch_error_integral = { 0 } ;
union longww yaw_error_integral = { 0 } ;

int target_rmat[9] = { RMAX , 0 , 0 , 0 , RMAX , 0 , 0 , 0 , RMAX } ;
int target_rmat_prev_transpose[9] = { RMAX , 0 , 0 , 0 , RMAX , 0 , 0 , 0 , RMAX } ;
int target_rmat_change[9] ;
int target_rate[3] ;

const uint16_t yaw_command_gain = ((long) MAX_YAW_RATE )*(4.8/PID_HZ) ;

//#define GGAIN_CONTROL SCALEGYRO*6*(RMAX*(1.0/PID_HZ)) // integration multiplier for gyros
//static fractional ggain_control[] =  { GGAIN_CONTROL, GGAIN_CONTROL, GGAIN_CONTROL };
int yaw_rmat[9] = { RMAX , 0 , 0 , 0 , RMAX , 0 , 0 , 0 , RMAX } ;
int tilt_rmat[9] ;

int16_t mult_Q2_14( int16_t x , int16_t y)
{
	int32_t product ;
	product = (__builtin_mulss(x,y))>>14 ;
	return (int16_t) product ;
}

extern union longww throttle_accum ;
void compute_target_climb_rate(void)
{
	int16_t altitude_error ;
	target_altitude = TARGET_MIN_ALTITUDE + IMU_ALT_DIVISOR*(THROTTLE_COMMAND-WEIGHT);
	altitude_error = IMU_altitude - target_altitude ;
	if(abs(altitude_error)<MAX_ALT_ERROR)
	{
		target_climb_rate = __builtin_divsd(__builtin_mulsu(-altitude_error,MAX_CLIMB_RATE ) , MAX_ALT_ERROR) ;
	}
	else
	{
		if (altitude_error > 0 )
		{
			target_climb_rate = - MAX_CLIMB_RATE ;
		}
		else
		{
			target_climb_rate =  MAX_CLIMB_RATE ;
		}
	}
}

boolean target_position_recorded = false ;

union longww throttle_accum = {0};

int16_t land_enable = 0 ;

void motorCntrl(void)
{
	int temp ;
	
	int min_throttle ;
	
	int motor_A ;
	int motor_B ;
	int motor_C ;
	int motor_D ;

	//int roll_error_delta ;
	//int pitch_error_delta ;
	//int yaw_error_delta ;

	union longww long_accum ;

	int yaw_step ;
	int yaw_vector[3] ;
	int rmat_transposed[9] ;
	int correction_matrix[9] ;
	int tilt_rmat[9] ;

	
	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];
	
	if ( (udb_pwIn[7]>1800)&&(udb_pwIn[7]<4200) )
	{
		if (udb_pwIn[7]<3000) // pre-flight and landing
		{
			if (land_enable==0)
			{
				THROTTLE_COMMAND = THROTTLE_COMMAND_IN ;
			}
			else
			{
				if (THROTTLE_COMMAND>THROTTLE_CUTOUT )
				{
					throttle_accum.WW+=__builtin_mulsu(-250,COMMAND_STEP_RATE_MULTIPLIER);	
				}
				else
				{
					THROTTLE_COMMAND = 2000 ;
					if (THROTTLE_COMMAND_IN<2200) land_enable = 0 ;
				}
			}
		}
		else
		{
			land_enable = 1 ;
			throttle_accum.WW += __builtin_mulsu(THROTTLE_COMMAND_IN-3000 , COMMAND_STEP_RATE_MULTIPLIER ) ;
			
			if (THROTTLE_COMMAND>4000)
			{
				THROTTLE_COMMAND = 4000 ;
			}
			if (THROTTLE_COMMAND<MIN_THROTTLE_COMMAND)
			{
				THROTTLE_COMMAND = MIN_THROTTLE_COMMAND ;
			}	
		}
	}
	else
	{
		THROTTLE_COMMAND = 0 ;
	}
	
	if (!didCalibrate)
	{
		// Leave at 0 (no PWM pulses) until calibrated.
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = 0 ;
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = 0 ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = 0 ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = 0 ;
	}
	else if (abs(THROTTLE_COMMAND-udb_pwTrim[THROTTLE_INPUT_CHANNEL])< MANUAL_DEADBAND )
	{
		initialize_yaw_rmat();
		motor_A = THROTTLE_COMMAND ;
		motor_B = THROTTLE_COMMAND ;
		motor_C = THROTTLE_COMMAND ;
		motor_D = THROTTLE_COMMAND ;

		commanded_roll =  ( pwManual[ROLL_INPUT_CHANNEL] 
						- udb_pwTrim[ROLL_INPUT_CHANNEL]) ;
		commanded_pitch = ( pwManual[PITCH_INPUT_CHANNEL] 
						- udb_pwTrim[PITCH_INPUT_CHANNEL] ) ;
		commanded_yaw = ( pwManual[YAW_INPUT_CHANNEL] 
						- udb_pwTrim[YAW_INPUT_CHANNEL] )  ;

		motor_A += + commanded_yaw - commanded_pitch ;
		motor_B += - commanded_yaw - commanded_roll ;
		motor_C += + commanded_yaw + commanded_pitch ;
		motor_D += - commanded_yaw + commanded_roll ;

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
		
		// update yaw matrix
		yaw_step = (commanded_yaw/4) * yaw_command_gain;
		yaw_vector[0] = 0 ;
		yaw_vector[1] = 0 ;
		yaw_vector[2] = yaw_step ;
		MatrixRotate( yaw_rmat , yaw_vector ) ;
		matrix_normalize( yaw_rmat ) ;
		
		if( dcm_flags._.fpv_tilt_req == 1 )
		// multiply commanded yaw matrix by commanded tilt matrix to get overall target matrix
		// this if for commanded tilt in body frame 
		{	
			compute_tilt_rmat( tilt_rmat , commanded_roll , commanded_pitch ) ;
			MatrixMultiply ( 3 , 3 , 3 , target_rmat , yaw_rmat , tilt_rmat ) ;
			MatrixAdd( 3 , 3 , target_rmat , target_rmat , target_rmat ) ;
		}
		
		if (dcm_flags._.position_hold_req == 1)
		{
			
			delta_position[0] =  - IMUlocationx._.W1 ;
			delta_position[1] =  - IMUlocationy._.W1 ;
			delta_position[2] = MAX_DISTANCE ;
		
			vector3_normalize(delta_position,delta_position) ;
					
			x_velocity_target = mult_Q2_14 ( delta_position[0], MAX_SPEED );
			y_velocity_target = mult_Q2_14 ( delta_position[1], MAX_SPEED );
		
			x_velocity_feedback = multiply_saturate ( x_velocity_target-IMUvelocityx._.W1 , LATERAL_RATE_GAIN , RMAX ) ;
			y_velocity_feedback = multiply_saturate ( y_velocity_target-IMUvelocityy._.W1 , LATERAL_RATE_GAIN , RMAX ) ;	
			
			compute_tilt_rmat( tilt_rmat , commanded_roll+x_velocity_feedback , commanded_pitch+y_velocity_feedback ) ;	
			MatrixMultiply ( 3 , 3 , 3 , target_rmat , tilt_rmat , yaw_rmat ) ;
			MatrixAdd( 3 , 3 , target_rmat , target_rmat , target_rmat ) ;
		}
		
		if ( dcm_flags._.earth_frame_tilt_req == 1 )
		{
			if (target_position_recorded == false )
			{
				x_position_target = IMUlocationx._.W1 ;
				y_position_target = IMUlocationy._.W1 ;
				target_position_recorded = true ;
			}
			delta_position[0] = x_position_target - IMUlocationx._.W1 ;
			delta_position[1] = y_position_target - IMUlocationy._.W1 ;
			delta_position[2] = MAX_DISTANCE ;
		
			vector3_normalize(delta_position,delta_position) ;
					
			x_velocity_target = mult_Q2_14 ( delta_position[0], MAX_SPEED );
			y_velocity_target = mult_Q2_14 ( delta_position[1], MAX_SPEED );
		
			x_velocity_feedback = multiply_saturate ( x_velocity_target-IMUvelocityx._.W1 , LATERAL_RATE_GAIN , RMAX ) ;
			y_velocity_feedback = multiply_saturate ( y_velocity_target-IMUvelocityy._.W1 , LATERAL_RATE_GAIN , RMAX ) ;	
			
			compute_tilt_rmat( tilt_rmat , commanded_roll+x_velocity_feedback , commanded_pitch+y_velocity_feedback ) ;	
			MatrixMultiply ( 3 , 3 , 3 , target_rmat , tilt_rmat , yaw_rmat ) ;
			MatrixAdd( 3 , 3 , target_rmat , target_rmat , target_rmat ) ;
		}
		else
		{
			target_position_recorded = false ;
		}
		
		// compute feed forward
		MatrixMultiply( 3 , 3 , 3 , target_rmat_change , target_rmat_prev_transpose , target_rmat ) ;
		target_rate[0] = ( target_rmat_change[7]-target_rmat_change[5])/2 ;
		target_rate[1] = ( target_rmat_change[2]-target_rmat_change[6])/2 ;
		target_rate[2] = ( target_rmat_change[3]-target_rmat_change[1])/2 ;
		
		MatrixTranspose( 3 , 3 , target_rmat_prev_transpose , target_rmat ) ;
		
		// form the transpose of rmat
		MatrixTranspose( 3 , 3 , rmat_transposed , rmat )	;
		
		// form the correction matrix from rmat_transposed times target rmat
		MatrixMultiply ( 3 , 3 , 3 , correction_matrix , rmat_transposed , target_rmat ) ;	
		MatrixAdd( 3 , 3 , correction_matrix , correction_matrix , correction_matrix ) ;

		roll_error = -( correction_matrix[6]- correction_matrix[2])/2 ;
		pitch_error = -( correction_matrix[5]- correction_matrix[7])/2 ;
		yaw_error = -( correction_matrix[1]- correction_matrix[3])/2 ;
		

//		Compute the signals that are common to all 4 motors
		min_throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;
		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*ACCEL_K ) , accelEarth[2] ) ;
		accel_feedback = long_accum._.W1 ;
		int16_t thrust =  THROTTLE_COMMAND - accel_feedback ;
		motor_A = thrust ;
		motor_B = thrust ;
		motor_C = thrust ;
		motor_D = thrust ;

//		Compute the error intetgrals
		roll_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*TILT_KI/PID_HZ), roll_error ))>>5) ;
		if ( roll_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			roll_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( roll_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			roll_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

		pitch_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*TILT_KI/PID_HZ), pitch_error ))>>5) ;
		if ( pitch_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			pitch_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( pitch_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			pitch_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

		yaw_error_integral.WW += ((__builtin_mulus ( (unsigned int ) (32.0*RMAX*YAW_KI/PID_HZ), yaw_error ))>>5) ;
		if ( yaw_error_integral.WW > MAXIMUM_ERROR_INTEGRAL )
		{
			yaw_error_integral.WW = MAXIMUM_ERROR_INTEGRAL ;
		}
		if ( yaw_error_integral.WW < - MAXIMUM_ERROR_INTEGRAL )
		{
			yaw_error_integral.WW =  - MAXIMUM_ERROR_INTEGRAL ;
		}

//		Compute the derivatives
		
	//	VectorMultiply(3, theta, omegagyro, ggain_control); // Scalegain of 2
	//	theta_delta[0] = theta[0] - theta_previous[0] ;
	//	theta_delta[1] = theta[1] - theta_previous[1] ;

	//	theta_previous[0] = theta[0] ;
	//	theta_previous[1] = theta[1] ;

	//	roll_error_delta = roll_error - roll_error_previous ;
	//	roll_error_previous = roll_error ;

	//	pitch_error_delta = pitch_error - pitch_error_previous ;
	//	pitch_error_previous = pitch_error ;

	//	yaw_error_delta = yaw_error - yaw_error_previous ;
	//	yaw_error_previous = yaw_error ;

//		Compute the PID(DD) signals
		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KP) , roll_error ) ;
		roll_control = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KD*SCALEGYRO/26.0) , -omegagyro[1] ) ;
		roll_control += long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_FF*SERVO_HZ/50.0) , target_rate[1] ) ;
		roll_control += long_accum._.W1 ;

		//long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KDD) , -theta_delta[1] ) << 2 ;
		//roll_control += long_accum._.W1 ;

		roll_control += roll_error_integral._.W1 + ROLL_TRIM ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KP) , pitch_error ) ;
		pitch_control = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KD*SCALEGYRO/26.0) , -omegagyro[0] ) ;
		pitch_control += long_accum._.W1 ;
		
		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_FF*SERVO_HZ/50.0) , target_rate[0] ) ;
		pitch_control += long_accum._.W1 + PITCH_TRIM ;

		//long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*TILT_KDD) , -theta_delta[0] ) << 2 ;
		//pitch_control += long_accum._.W1 ;

		pitch_control += pitch_error_integral._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*YAW_KP) , yaw_error ) ;
		yaw_control = long_accum._.W1 ;

		long_accum.WW = __builtin_mulus ( (unsigned int) (RMAX*YAW_KD*SCALEGYRO/26.0) , -omegagyro[2] ) ;
		yaw_control += long_accum._.W1 ;

		yaw_control += yaw_error_integral._.W1 + YAW_TRIM ;
#ifdef arduCopter
		if((udb_heartbeat_counter%(HEARTBEAT_HZ/SERVO_HZ))==0)
		{
			compute_altitude_control();
		}		
		// Mix in the yaw, pitch, and roll signals into the motors
		motor_A += + yaw_control - pitch_control ;
		motor_B += - yaw_control - roll_control ;
		motor_C += + yaw_control + pitch_control ;
		motor_D += - yaw_control + roll_control ;
#endif
		
#ifdef draganflier
		if((udb_heartbeat_counter%(HEARTBEAT_HZ/SERVO_HZ))==0)
		{
			compute_altitude_control();
		}
		thrust += altitude_control ;
		if(thrust > MAX_THRUST) thrust = MAX_THRUST ;
		if(thrust < MIN_THRUST) thrust = MIN_THRUST ;		
		// Mix in the yaw, pitch, and roll signals into the motors
		motor_A = thrust + yaw_control - pitch_control ;
		motor_B = thrust - yaw_control - roll_control ;
		motor_C = thrust + yaw_control + pitch_control ;
		motor_D = thrust - yaw_control + roll_control ;
#endif

#ifdef spedix
		if((udb_heartbeat_counter%(HEARTBEAT_HZ/SERVO_HZ))==0)
		{
			compute_altitude_control();
		}
		// Mix in the yaw, pitch, and roll signals into the motors
		thrust += altitude_control ;
		if(thrust > MAX_THRUST) thrust = MAX_THRUST ;
		if(thrust < MIN_THRUST) thrust = MIN_THRUST ;
		int16_t yaw_total = yaw_control + YAW_TRIM ;
		int16_t pitch_total = pitch_control + PITCH_TRIM ;
		int16_t roll_total = roll_control + ROLL_TRIM ;
		motor_A = thrust - yaw_total + ( - pitch_total + roll_total )/2 ;
		motor_B = thrust + yaw_total + ( - pitch_total - roll_total )/2 ;
		motor_C = thrust - yaw_total + ( + pitch_total - roll_total )/2 ;
		motor_D = thrust + yaw_total + ( + pitch_total + roll_total )/2 ;
#endif

#ifdef desktest		
		// debugging
		/*compute_altitude_control();
		motor_A = udb_pwIn[1];
		motor_B = udb_pwIn[2];
		motor_C = udb_pwIn[3];
		motor_D = udb_pwIn[4];
		udb_pwOut[5] = udb_servo_pulsesat( udb_pwIn[5]/10 + 2000 ) ;
		udb_pwOut[6] = udb_servo_pulsesat( udb_pwIn[5] ) ;*/
		
		long_accum.WW = __builtin_mulss(roll_error, 4000);
		motor_A=(3000 + long_accum._.W1);	
		long_accum.WW = __builtin_mulss(pitch_error, 4000);
		motor_B=(3000 + long_accum._.W1);	
		long_accum.WW = __builtin_mulss(yaw_error, 4000);
		motor_C=(3000 + long_accum._.W1);	
		motor_D = 3000 ;
		
#endif

//		Send the signals out to the motors
		udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_D ) ;
	}
}

/*
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
*/

void compute_altitude_control(void)
{
	int altitude_change ;
	int rate_control ;
	int proportional_control ;
	number_pulses = lidar_pulses ;
	lidar_pulses = 0 ;
	if (number_pulses>=MIN_LIDAR_PULSE_THRESH)
	{
		// compute LIDAR altitude in millimeters. account for tilt
		altitude = __builtin_divsd( __builtin_mulss( udb_pwIn[5] , rmat[8] ) , RMAX)/2 ;
		altitude_change = altitude - previous_altitude ;
		previous_altitude = altitude ;
		climb_rate = __builtin_mulsu ( altitude_change , SERVO_HZ ) ;
		// perform saturation to block the noise during takeoff and landing
		if ( climb_rate > 3000 ) climb_rate = 3000 ;
		if ( climb_rate < -3000 ) climb_rate = -3000 ;
	}
	compute_target_climb_rate();
	rate_control = (target_climb_rate - IMU_climb)/IMU_CLIMB_RATE_DIVISOR ;
	proportional_control = (TARGET_MIN_ALTITUDE-IMU_altitude)/IMU_ALT_DIVISOR ; 
	if(rate_control>MAX_ALT_RATE_CONTROL)
	{
		rate_control = MAX_ALT_RATE_CONTROL ;
	}
	if(rate_control< -MAX_ALT_RATE_CONTROL)
	{
		rate_control = -MAX_ALT_RATE_CONTROL ;
	}
		
	if(proportional_control>MAX_ALT_PROP_CONTROL)
	{
		proportional_control = MAX_ALT_PROP_CONTROL ;
	}
	if(proportional_control< -MAX_ALT_PROP_CONTROL)
	{
		proportional_control = -MAX_ALT_PROP_CONTROL ;
	}
#if  ( USE_LIDAR == 1 )		
	if((IMU_altitude > 0 )&&(THROTTLE_COMMAND>2300))
	{
		altitude_control = rate_control + proportional_control ;
		//altitude_control = 0 ;
	}
	else
	{
		altitude_control = 0 ;
	}
#else
	altitude_control = 0 ;
#endif // USE_LIDAR
}

#if  (( ( int ) + MAX_YAW_RATE   < 25 ) || ( ( int ) + MAX_YAW_RATE > 250 ))
#error ("MAX_YAW_RATE must be between 25.0 and 250.0 degrees/second.")
#endif

#if (((int) + MAX_TILT) > 45)
#error ("MAX_TILT mus be less than or equal to 45 degrees."
#endif