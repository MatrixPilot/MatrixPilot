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
#include "fbw_options.h"

//	Perform control based on the airframe type.
//	Use the radio to determine the baseline pulse widths if the radio is on.
//	Otherwise, use the trim pulse width measured during power up.
//
//	Mix computed roll and pitch controls into the output channels for the compiled airframe type

#if(USE_INPUT_CONTROL == 1)

void servoMix( void )
{
	long temp ;
		
	// Standard airplane airframe
	// Mix roll_control into ailerons
	// Mix pitch_control into elevators
	// Mix yaw control and waggle into rudder
#if ( AIRFRAME_TYPE == AIRFRAME_STANDARD )
		temp = out_cntrls[IN_CNTRL_ROLL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle) ;
		udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = 3000 +
			REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, udb_pwOut[AILERON_OUTPUT_CHANNEL] - 3000) ;
		
		temp = out_cntrls[IN_CNTRL_PITCH] + REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control) ;
		udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		
		temp = out_cntrls[IN_CNTRL_YAW] + REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control - waggle) ;
		udb_pwOut[RUDDER_OUTPUT_CHANNEL] =  udb_servo_pulsesat( temp ) ;
		
		if ( out_cntrls[IN_CNTRL_THROTTLE] == 0 )
		{
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0 ;
		}
		else
		{	
			temp = out_cntrls[IN_CNTRL_THROTTLE] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control) ;
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp ) ;
		}
#endif
	
	
	// V-Tail airplane airframe
	// Mix roll_control and waggle into ailerons
	// Mix pitch_control and yaw_control into both elevator and rudder
#if ( AIRFRAME_TYPE == AIRFRAME_VTAIL )
		#error("VTAIL NOT SUPPORTED")
#endif
		
	// Delta-Wing airplane airframe
	// Mix roll_control, pitch_control, and waggle into aileron and elevator
	// Mix rudder_control into  rudder
#if ( AIRFRAME_TYPE == AIRFRAME_DELTA )
		#error("DELTA NOT SUPPORTED")
#endif
	
	
#if ( AIRFRAME_TYPE == AIRFRAME_HELI )
		#error("HELI NOT SUPPORTED")
#endif
		
		udb_pwOut[PASSTHROUGH_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_A_INPUT_CHANNEL] ) ;
		udb_pwOut[PASSTHROUGH_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_B_INPUT_CHANNEL] ) ;
		udb_pwOut[PASSTHROUGH_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_C_INPUT_CHANNEL] ) ;
		udb_pwOut[PASSTHROUGH_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_D_INPUT_CHANNEL] ) ;
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


#endif //(USE_INPUT_CONTROL == 1)

