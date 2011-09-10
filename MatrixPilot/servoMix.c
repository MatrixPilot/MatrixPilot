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


#include "defines.h"

//	Perform control based on the airframe type.
//	Use the radio to determine the baseline pulse widths if the radio is on.
//	Otherwise, use the trim pulse width measured during power up.
//
//	Mix computed roll and pitch controls into the output channels for the compiled airframe type


const int aileronbgain = (int)(8.0*AILERON_BOOST) ;
const int elevatorbgain = (int)(8.0*ELEVATOR_BOOST) ;
const int rudderbgain = (int)(8.0*RUDDER_BOOST) ;

extern int theta[3] ;

int roll_feedback ;
int pitch_feedback ;
int yaw_feedback ;
int theta_previous[2] = { 0 , 0 } ;
int theta_delta[2] ;

void servoMix( void )
{
	long temp ;
	int pwManual[NUM_INPUTS+1] ;
	
	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];
	
	
	// Apply boosts if in a stabilized mode
	if (udb_flags._.radio_on && flags._.pitch_feedback)
	{
		pwManual[AILERON_INPUT_CHANNEL] += ((pwManual[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]) * aileronbgain) >> 3 ;
		pwManual[ELEVATOR_INPUT_CHANNEL] += ((pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) * elevatorbgain) >> 3 ;
		pwManual[RUDDER_INPUT_CHANNEL] += ((pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) * rudderbgain) >> 3 ;
	}
	
	// Standard airplane airframe
	// Mix roll_control into ailerons
	// Mix pitch_control into elevators
	// Mix yaw control and waggle into rudder
#if ( AIRFRAME_TYPE == AIRFRAME_STANDARD )
		temp = pwManual[AILERON_INPUT_CHANNEL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle) ;
		udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = 3000 +
			REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, udb_pwOut[AILERON_OUTPUT_CHANNEL] - 3000) ;
		
		temp = pwManual[ELEVATOR_INPUT_CHANNEL] + REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control) ;
		udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		temp = pwManual[RUDDER_INPUT_CHANNEL] + REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control - waggle) ;
		udb_pwOut[RUDDER_OUTPUT_CHANNEL] =  udb_servo_pulsesat( temp ) ;
		
		if ( pwManual[THROTTLE_INPUT_CHANNEL] == 0 )
		{
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0 ;
		}
		else
		{	
			temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control) ;
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		}
#endif
	
	
	// V-Tail airplane airframe
	// Mix roll_control and waggle into ailerons
	// Mix pitch_control and yaw_control into both elevator and rudder
#if ( AIRFRAME_TYPE == AIRFRAME_VTAIL )
		long vtail_yaw_control = REVERSE_IF_NEEDED(ELEVON_VTAIL_SURFACES_REVERSED, yaw_control) ;
		
		temp = pwManual[AILERON_INPUT_CHANNEL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle) ;
		udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		//	Reverse the polarity of the secondary aileron if necessary
		udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = 3000 +
			REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, udb_pwOut[AILERON_OUTPUT_CHANNEL] - 3000) ;
		
		temp = pwManual[ELEVATOR_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control + vtail_yaw_control) ;
		udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		temp = pwManual[RUDDER_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, pitch_control - vtail_yaw_control) ;
		udb_pwOut[RUDDER_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		if ( pwManual[THROTTLE_INPUT_CHANNEL] == 0 )
		{
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0 ;
		}
		else
		{	
			temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control) ;
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		}
#endif
	
	
	// Delta-Wing airplane airframe
	// Mix roll_control, pitch_control, and waggle into aileron and elevator
	// Mix rudder_control into  rudder
#if ( AIRFRAME_TYPE == AIRFRAME_DELTA )
		long delta_roll_control = REVERSE_IF_NEEDED(ELEVON_VTAIL_SURFACES_REVERSED, roll_control) ;
		
		temp = pwManual[AILERON_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, -delta_roll_control + pitch_control - waggle) ;
		udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		temp = pwManual[ELEVATOR_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, delta_roll_control + pitch_control + waggle) ;
		udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		temp = pwManual[RUDDER_INPUT_CHANNEL] + REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control) ;
		udb_pwOut[RUDDER_OUTPUT_CHANNEL] =  udb_servo_pulsesat( temp ) ;
		
		if ( pwManual[THROTTLE_INPUT_CHANNEL] == 0 )
		{
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0 ;
		}
		else
		{	
			temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control) ;
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		}
#endif
	
	
	// Helicopter airframe
	// Mix half of roll_control and half of pitch_control into aileron channels
	// Mix full pitch_control into elevator
	// Ignore waggle for now
#if ( AIRFRAME_TYPE == AIRFRAME_HELI )
		temp = pwManual[AILERON_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control/2 + pitch_control/2) ;
		udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		temp = pwManual[ELEVATOR_INPUT_CHANNEL] + REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control) ;
		udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		temp = pwManual[AILERON_SECONDARY_OUTPUT_CHANNEL] + 
			REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, -roll_control/2 + pitch_control/2) ;
		udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = temp ;
		
		temp = pwManual[RUDDER_INPUT_CHANNEL] /*+ REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control)*/ ;
		udb_pwOut[RUDDER_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		if ( pwManual[THROTTLE_INPUT_CHANNEL] == 0 )
		{
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0 ;
		}
		else
		{	
			temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control) ;
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		}
#endif

		int commanded_roll ;
		int commanded_pitch ;
		int commanded_yaw ;


		int min_throttle ;

		int motor_A ;
		int motor_B ;
		int motor_C ;
		int motor_D ;

		union longww long_accum ;

//#define ROLL_KP 0.05
//#define PITCH_KP 0.05
//#define ROLL_KD 1.0
//#define PITCH_KD 1.0
//#define YAW_KD 1.0

//#define ROLL_KP 0.05
//#define PITCH_KP 0.05
//#define ROLL_KD 0.5
//#define PITCH_KD 0.5
//#define YAW_KD 0.5

//#define ROLL_KP 0.025
//#define PITCH_KP 0.025
//#define ROLL_KD 1.5
//#define PITCH_KD 1.5
//#define YAW_KD 1.5

#define ROLL_KP 0.05
#define PITCH_KP 0.05
#define ROLL_KD 1.0
#define PITCH_KD 1.0
#define YAW_KD 1.0
#define ROLL_KDD 1.0
#define PITCH_KDD 1.0

//		udb_pwOut[PASSTHROUGH_A_OUTPUT_CHANNEL] = 2000 ;
//		udb_pwOut[PASSTHROUGH_B_OUTPUT_CHANNEL] = 2000 ;
//		udb_pwOut[PASSTHROUGH_C_OUTPUT_CHANNEL] = 2000 ;
//		udb_pwOut[PASSTHROUGH_D_OUTPUT_CHANNEL] = 2000 ;

		commanded_roll =  ( pwManual[PASSTHROUGH_A_INPUT_CHANNEL] 
						- udb_pwTrim[PASSTHROUGH_A_INPUT_CHANNEL]) >> 2 ;
		commanded_pitch = ( pwManual[PASSTHROUGH_B_INPUT_CHANNEL] 
						- udb_pwTrim[PASSTHROUGH_B_INPUT_CHANNEL] ) >> 2 ;
		commanded_yaw = ( pwManual[PASSTHROUGH_D_INPUT_CHANNEL] 
						- udb_pwTrim[PASSTHROUGH_D_INPUT_CHANNEL] ) >> 1 ;

		min_throttle = udb_pwTrim[PASSTHROUGH_C_INPUT_CHANNEL] ;

		motor_A = motor_B = motor_C = motor_D = pwManual[PASSTHROUGH_C_INPUT_CHANNEL] ;

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

		motor_A += commanded_pitch + commanded_yaw - pitch_feedback - yaw_feedback ;
		motor_B += commanded_roll - commanded_yaw - roll_feedback + yaw_feedback ;
		motor_C += -commanded_pitch + commanded_yaw + pitch_feedback - yaw_feedback ;
		motor_D += -commanded_roll - commanded_yaw + roll_feedback + yaw_feedback ;

//		if ( motor_A < min_throttle ) motor_A = min_throttle ;
//		if ( motor_B < min_throttle ) motor_B = min_throttle ;
//		if ( motor_C < min_throttle ) motor_C = min_throttle ;
//		if ( motor_D < min_throttle ) motor_D = min_throttle ;

		udb_pwOut[PASSTHROUGH_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[PASSTHROUGH_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[PASSTHROUGH_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[PASSTHROUGH_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( motor_D ) ;

}


void cameraServoMix( void )
{
	long temp ;
	int pwManual[NUM_INPUTS+1] ;
	
	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];

	temp = ( pwManual[CAMERA_PITCH_INPUT_CHANNEL] - 3000 ) + REVERSE_IF_NEEDED(CAMERA_PITCH_CHANNEL_REVERSED, 
					cam_pitch_servo_pwm_delta ) ;
	temp = cam_pitchServoLimit(temp) ;
	udb_pwOut[CAMERA_PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp + 3000 ) ;

	temp = ( pwManual[CAMERA_YAW_INPUT_CHANNEL] - 3000 ) + REVERSE_IF_NEEDED(CAMERA_YAW_CHANNEL_REVERSED, 
					cam_yaw_servo_pwm_delta ) ;
	temp = cam_yawServoLimit(temp) ;
	udb_pwOut[CAMERA_YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp + 3000 ) ;
}
