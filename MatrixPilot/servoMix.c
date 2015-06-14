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
#include "servoMix.h"
#include "servoPrepare.h"
#include "config.h"
#include "states.h"
#include "cameraCntrl.h"
#include "../libUDB/servoOut.h"

// Perform control based on the airframe type.
// Use the radio to determine the baseline pulse widths if the radio is on.
// Otherwise, use the trim pulse width measured during power up.
//
// Mix computed roll and pitch controls into the output channels for the compiled airframe type.

static int16_t elevatorbgain = 0;
static int16_t rudderbgain   = 0;

void servoMix_init(void)
{
	elevatorbgain = (int16_t)(8.0*gains.ElevatorBoost);
	rudderbgain   = (int16_t)(8.0*gains.RudderBoost);
}

void servoMix(void)
{
	int32_t temp;
	int16_t pwManual[NUM_INPUTS+1];

	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
	{
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];
	}

	// Standard airplane airframe
	// Mix roll_control into ailerons
	// Mix pitch_control into elevators
	// Mix yaw control and waggle into rudder
#if (AIRFRAME_TYPE == AIRFRAME_STANDARD)
	// Apply boosts to elevator and rudder if in a controlled mode
	// It does not matter whether the radio is on or not
	if (state_flags._.pitch_feedback)
	{
		pwManual[AILERON_INPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL] ; // in fly by wire or navigate mode, manual input is accounted for in the turn control
		pwManual[ELEVATOR_INPUT_CHANNEL] += ((pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) * elevatorbgain) >> 3;
		pwManual[RUDDER_INPUT_CHANNEL] += ((pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) * rudderbgain) >> 3;
	}
	temp = pwManual[AILERON_INPUT_CHANNEL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle);
	udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL] +
	    REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, udb_pwOut[AILERON_OUTPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]);

	temp = pwManual[ELEVATOR_INPUT_CHANNEL] + REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control);
	udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	temp = pwManual[RUDDER_INPUT_CHANNEL] + REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control - waggle);
	udb_pwOut[RUDDER_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	if (pwManual[THROTTLE_INPUT_CHANNEL] == 0)
	{
		udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0;
	}
	else
	{
		temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control);
		udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);
	}
#endif // AIRFRAME_STANDARD

	// V-Tail airplane airframe
	// Mix roll_control and waggle into ailerons
	// Mix pitch_control and yaw_control into both elevator and rudder
#if (AIRFRAME_TYPE == AIRFRAME_VTAIL)
	{
	int16_t rudderInput;
	int16_t elevatorInput;
	int16_t pitchInput;
	int16_t yawInput;
	int16_t pitchCommand;
	int16_t yawCommand;
	int32_t vtail_yaw_control;

	// Unmix the vtail
	rudderInput  = REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, (pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]));
	elevatorInput = REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, (pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]));
	pitchInput = ((rudderInput+elevatorInput)>>1);
	yawInput = ((-rudderInput+elevatorInput)>>1);

	if (state_flags._.pitch_feedback)
	{
		// Apply boost in FBW or navigate mode
		pitchCommand = ((elevatorbgain + 8) * pitchInput) >> 3;
		yawCommand   = ((rudderbgain + 8)   * yawInput)   >> 3;
		// in fly by wire or navigate mode, manual input is accounted for in the turn control
		pwManual[AILERON_INPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL];
	}
	else
	{
		pitchCommand = pitchInput;
		yawCommand = yawInput;
	}
	
		vtail_yaw_control = REVERSE_IF_NEEDED(ELEVON_VTAIL_SURFACES_REVERSED, yaw_control);

	// In fly by wire mode, ailerons are controlled indirectly by helical turn control
	temp = pwManual[AILERON_INPUT_CHANNEL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle);
	udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	// Reverse the polarity of the secondary aileron if necessary
	udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL] +
	    REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, udb_pwOut[AILERON_OUTPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]);

	temp = udb_pwTrim[ELEVATOR_INPUT_CHANNEL] +
	    REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitchCommand + pitch_control + yawCommand + vtail_yaw_control);
	udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	temp = udb_pwTrim[RUDDER_INPUT_CHANNEL] +
	    REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, pitchCommand + pitch_control - yawCommand - vtail_yaw_control);
	udb_pwOut[RUDDER_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	if (pwManual[THROTTLE_INPUT_CHANNEL] == 0)
	{
		udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0;
	}
	else
	{
		temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control);
		udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);
	}
	}
#endif // AIRFRAME_VTAIL

	// Delta-Wing airplane airframe
	// Mix roll_control, pitch_control, and waggle into aileron and elevator
	// Mix rudder_control into  rudder
#if (AIRFRAME_TYPE == AIRFRAME_DELTA)
	{
	int16_t aileronInput;
	int16_t elevatorInput;
	int16_t pitchInput;
	int16_t rollInput;
	int16_t pitchCommand;
	int16_t rollCommand;
	int32_t delta_roll_control;
	// unmix the inputs, note this will produce zeros during radio off
	aileronInput  = REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, (pwManual[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]));
	elevatorInput = REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, (pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]));
	pitchInput = (elevatorInput+aileronInput)>>1;
	rollInput = (elevatorInput-aileronInput)>>1;

	if (state_flags._.pitch_feedback)
	{
		pitchCommand = ((elevatorbgain + 8) * pitchInput ) >> 3;
		pwManual[RUDDER_INPUT_CHANNEL] += ((pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) * rudderbgain) >> 3;
		rollCommand = 0;
	}
	else
	{
		pitchCommand = pitchInput;
		rollCommand = rollInput;
	}
	delta_roll_control = REVERSE_IF_NEEDED(ELEVON_VTAIL_SURFACES_REVERSED, roll_control);

	temp = udb_pwTrim[AILERON_INPUT_CHANNEL] +
	    REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, -rollCommand -delta_roll_control + pitchCommand + pitch_control - waggle);
	udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	temp = udb_pwTrim[ELEVATOR_INPUT_CHANNEL] +
	    REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, rollCommand + delta_roll_control + pitchCommand + pitch_control + waggle);
	udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

	temp = pwManual[RUDDER_INPUT_CHANNEL] +
	    REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control - waggle);
	udb_pwOut[RUDDER_OUTPUT_CHANNEL] =  udb_servo_pulsesat(temp);
	
	if (pwManual[THROTTLE_INPUT_CHANNEL] == 0)
	{
		udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0;
	}
	else
	{
		temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control);
		udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);
	}
	}
#endif // AIRFRAME_DELTA

	// Helicopter airframe
	// Mix half of roll_control and half of pitch_control into aileron channels
	// Mix full pitch_control into elevator
	// Ignore waggle for now
#if (AIRFRAME_TYPE == AIRFRAME_HELI)
		temp = pwManual[AILERON_INPUT_CHANNEL] +
		    REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control/2 + pitch_control/2);
		udb_pwOut[AILERON_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

		temp = pwManual[ELEVATOR_INPUT_CHANNEL] +
		    REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control);
		udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

		temp = pwManual[AILERON_SECONDARY_OUTPUT_CHANNEL] + 
		    REVERSE_IF_NEEDED(AILERON_SECONDARY_CHANNEL_REVERSED, -roll_control/2 + pitch_control/2);
		udb_pwOut[AILERON_SECONDARY_OUTPUT_CHANNEL] = temp;

		temp = pwManual[RUDDER_INPUT_CHANNEL] /*+ REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control)*/;
		udb_pwOut[RUDDER_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);

		if (pwManual[THROTTLE_INPUT_CHANNEL] == 0)
		{
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0;
		}
		else
		{
			temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control);
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);
		}
#endif // AIRFRAME_HELI

		udb_pwOut[PASSTHROUGH_A_OUTPUT_CHANNEL] = udb_servo_pulsesat(pwManual[PASSTHROUGH_A_INPUT_CHANNEL]);
		udb_pwOut[PASSTHROUGH_B_OUTPUT_CHANNEL] = udb_servo_pulsesat(pwManual[PASSTHROUGH_B_INPUT_CHANNEL]);
		udb_pwOut[PASSTHROUGH_C_OUTPUT_CHANNEL] = udb_servo_pulsesat(pwManual[PASSTHROUGH_C_INPUT_CHANNEL]);
		udb_pwOut[PASSTHROUGH_D_OUTPUT_CHANNEL] = udb_servo_pulsesat(pwManual[PASSTHROUGH_D_INPUT_CHANNEL]);
}

void cameraServoMix(void)
{
	int32_t temp;
	int16_t pwManual[NUM_INPUTS+1];

	// TODO: why is this code from above repeated here? - RobD

	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
	{
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];
	}

	temp = (pwManual[CAMERA_PITCH_INPUT_CHANNEL] - 3000) 
	     + REVERSE_IF_NEEDED(CAMERA_PITCH_CHANNEL_REVERSED, cam_pitch_servo_pwm_delta);
	temp = cam_pitchServoLimit(temp);
	udb_pwOut[CAMERA_PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp + 3000);

	temp = (pwManual[CAMERA_YAW_INPUT_CHANNEL] - 3000) 
	     + REVERSE_IF_NEEDED(CAMERA_YAW_CHANNEL_REVERSED, cam_yaw_servo_pwm_delta);
	temp = cam_yawServoLimit(temp);
	udb_pwOut[CAMERA_YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp + 3000);
}
