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


const int16_t aileronbgain = (int16_t)(8.0*AILERON_BOOST) ;
const int16_t elevatorbgain = (int16_t)(8.0*ELEVATOR_BOOST) ;
const int16_t rudderbgain = (int16_t)(8.0*RUDDER_BOOST) ;


void servoMix( void )
{
  int8_t i;
	int16_t pwManual[NUM_INPUTS+1] ;
  int32_t aileron, aileronSecondary = 0, rudder, elevator, throttle;

	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (i = 0; i <= NUM_INPUTS; i++)
  {
		if (udb_flags._.radio_on)
			pwManual[i] = udb_pwIn[i];
		else
			pwManual[i] = udb_pwTrim[i];
  }
  
	// Apply boosts if in a stabilized mode
	if (udb_flags._.radio_on && flags._.pitch_feedback)
	{
		pwManual[AILERON_INPUT_CHANNEL] += ((pwManual[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]) * aileronbgain) >> 3 ;
		pwManual[ELEVATOR_INPUT_CHANNEL] += ((pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) * elevatorbgain) >> 3 ;
		pwManual[RUDDER_INPUT_CHANNEL] += ((pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) * rudderbgain) >> 3 ;
	}
	
    if ( pwManual[THROTTLE_INPUT_CHANNEL] == 0 )    // throttle pwm input value of zero should never occur
                                                    // and pwm output of zero will result in no output
    {
        throttle = 0 ;
    }
#if (CATAPULT_LAUNCH_INPUT_CHANNEL != CHANNEL_UNUSED)
    else if ( flags._.disable_throttle )    // disable throttle, but keep generating valid PWM signal
    {
        throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
    }
#endif
    else
    {
        throttle = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control) ;
    }

#if ( AIRFRAME_TYPE == AIRFRAME_STANDARD )
    // Standard airplane airframe
	// Mix roll_control into ailerons
	// Mix pitch_control into elevators
	// Mix yaw control and waggle into rudder
		aileron = pwManual[AILERON_INPUT_CHANNEL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle) ;
		
		aileronSecondary = 3000 +	REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, aileron - 3000) ;
		
		elevator = pwManual[ELEVATOR_INPUT_CHANNEL] + REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control) ;
		
		rudder = pwManual[RUDDER_INPUT_CHANNEL] + REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control - waggle) ;
	
	
#elif ( AIRFRAME_TYPE == AIRFRAME_VTAIL )
	// V-Tail airplane airframe
	// Mix roll_control and waggle into ailerons
	// Mix pitch_control and yaw_control into both elevator and rudder
		int32_t vtail_yaw_control = REVERSE_IF_NEEDED(ELEVON_VTAIL_SURFACES_REVERSED, yaw_control) ;
		
		aileron = pwManual[AILERON_INPUT_CHANNEL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle) ;
		
		//	Reverse the polarity of the secondary aileron if necessary
		aileronSecondary = 3000 + REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, aileron - 3000) ;
		
		elevator = pwManual[ELEVATOR_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control + vtail_yaw_control) ;
		
		rudder = pwManual[RUDDER_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, pitch_control - vtail_yaw_control) ;
	
	
#elif ( AIRFRAME_TYPE == AIRFRAME_DELTA )
	// Delta-Wing airplane airframe
	// Mix roll_control, pitch_control, and waggle into aileron and elevator
	// Mix rudder_control into  rudder
		int32_t delta_roll_control = REVERSE_IF_NEEDED(ELEVON_VTAIL_SURFACES_REVERSED, roll_control) ;
		
		aileron = pwManual[AILERON_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, -delta_roll_control + pitch_control - waggle) ;
		
		elevator = pwManual[ELEVATOR_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, delta_roll_control + pitch_control + waggle) ;
		
		rudder = pwManual[RUDDER_INPUT_CHANNEL] + REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control) ;
	
	
#elif ( AIRFRAME_TYPE == AIRFRAME_HELI )
	// Helicopter airframe
	// Mix half of roll_control and half of pitch_control into aileron channels
	// Mix full pitch_control into elevator
	// Ignore waggle for now
		aileron = pwManual[AILERON_INPUT_CHANNEL] +
			REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control/2 + pitch_control/2) ;
		
		elevator = pwManual[ELEVATOR_INPUT_CHANNEL] + REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control) ;
		
		aileronSecondary = pwManual[AILERON_SECONDARY_OUTPUT_CHANNEL] +
			REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, -roll_control/2 + pitch_control/2) ;
		
		rudder = pwManual[RUDDER_INPUT_CHANNEL] /*+ REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control)*/ ;
		
		if ( pwManual[THROTTLE_INPUT_CHANNEL] == 0 )
		{
			throttle = 0 ;
		}
		else
		{	
			throttle = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control) ;
		}
#endif

#if (CATAPULT_LAUNCH_INPUT_CHANNEL != CHANNEL_UNUSED)
    if ( isLauncherLaunching() )    // while launching, add up-trim PWM signal
    {
      aileron = REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, aileron + LAUNCH_TRIM_PITCH_UP);
    }
#endif

    udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat(aileron);
    udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = udb_servo_pulsesat(aileronSecondary);
    udb_pwOut[RUDDER_OUTPUT_CHANNEL] = udb_servo_pulsesat(rudder);
    udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat(elevator);
    udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat(throttle);
		udb_pwOut[PASSTHROUGH_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_A_INPUT_CHANNEL] ) ;
		udb_pwOut[PASSTHROUGH_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_B_INPUT_CHANNEL] ) ;
		udb_pwOut[PASSTHROUGH_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_C_INPUT_CHANNEL] ) ;
		udb_pwOut[PASSTHROUGH_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( pwManual[PASSTHROUGH_D_INPUT_CHANNEL] ) ;
}


void cameraServoMix( void )
{
	int32_t temp ;
	int16_t pwManual[NUM_INPUTS+1] ;
	
	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
  {
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];
  }
  
	temp = ( pwManual[CAMERA_PITCH_INPUT_CHANNEL] - 3000 ) + REVERSE_IF_NEEDED(CAMERA_PITCH_CHANNEL_REVERSED, 
					cam_pitch_servo_pwm_delta ) ;
	temp = cam_pitchServoLimit(temp) ;
	udb_pwOut[CAMERA_PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat_cam( temp + 3000 ) ;

	temp = ( pwManual[CAMERA_YAW_INPUT_CHANNEL] - 3000 ) + REVERSE_IF_NEEDED(CAMERA_YAW_CHANNEL_REVERSED, 
					cam_yaw_servo_pwm_delta ) ;
	temp = cam_yawServoLimit(temp) ;
	udb_pwOut[CAMERA_YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat_cam( temp + 3000 ) ;
}
