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
#include "options_servo_mix.h"
#include "servoPrepare.h"
#include "config.h"
#include "states.h"
#include "cameraCntrl.h"
#include "../libUDB/servoOut.h"

#if (AIRFRAME_TYPE == AIRFRAME_GLIDER)
#include "airspeedCntrl.h"
#include "altitudeCntrl.h"

static int16_t aileronLeftBrakeFactor;
static int16_t aileronLeftFlapsPosFactor;
static int16_t aileronLeftFlapsNegFactor;
static int16_t aileronLeftLpFlapsFactor;
static int16_t aileronLeftRpFlapsFactor;
static int16_t aileronLeftLpSpeedFlapsFactor;
static int16_t aileronLeftRpSpeedFlapsFactor;

static int16_t flapLeftBrakeFactor;
static int16_t flapLeftFlapsPosFactor;
static int16_t flapLeftFlapsNegFactor;
static int16_t flapLeftRpFlapsFactor;
static int16_t flapLeftLpFlapsFactor;
static int16_t flapLeftRpSpeedFlapsFactor;
static int16_t flapLeftLpSpeedFlapsFactor;

static int16_t flapRightBrakeFactor;
static int16_t flapRightFlapsPosFactor;
static int16_t flapRightFlapsNegFactor;
static int16_t flapRightRpFlapsFactor;
static int16_t flapRightLpFlapsFactor;
static int16_t flapRightRpSpeedFlapsFactor;
static int16_t flapRightLpSpeedFlapsFactor;

static int16_t aileronRightBrakeFactor;
static int16_t aileronRightFlapsPosFactor;
static int16_t aileronRightFlapsNegFactor;
static int16_t aileronRightLpFlapsFactor;
static int16_t aileronRightRpFlapsFactor;
static int16_t aileronRightLpSpeedFlapsFactor;
static int16_t aileronRightRpSpeedFlapsFactor;

static int16_t elevatorBrakeFactor;
static int16_t elevatorThrottleFactor;

static int16_t rudderFromAileronFactor;
static int16_t rudderFactor;

static int16_t throttleFactor;

static int32_t throttleSteps = 0;//range: 0..2000

#endif //AIRFRAME_GLIDER

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


#if (AIRFRAME_TYPE == AIRFRAME_GLIDER)
	//convert fraction to x*32 integer;(0.03 resolution) 0.03 = 1 , 1.0 = 32 //only once, to save resources
	aileronLeftBrakeFactor = (signed int)(AILERON_LEFT_BRAKE_FACTOR * 32.0);//float to int 
	aileronLeftFlapsPosFactor = (signed int)(AILERON_LEFT_FLAPS_POS_FACTOR * 32.0);
	aileronLeftFlapsNegFactor = (signed int)(AILERON_LEFT_FLAPS_NEG_FACTOR * 32.0);
	aileronLeftLpFlapsFactor = (signed int)(AILERON_LEFT_LP_FLAPS_FACTOR * 32.0);
	aileronLeftRpFlapsFactor = (signed int)(AILERON_LEFT_RP_FLAPS_FACTOR * 32.0);
	aileronLeftLpSpeedFlapsFactor = (signed int)(AILERON_LEFT_LP_SPEED_FLAPS_FACTOR * 32.0);
	aileronLeftRpSpeedFlapsFactor = (signed int)(AILERON_LEFT_RP_SPEED_FLAPS_FACTOR * 32.0);
	
	flapLeftBrakeFactor = (signed int)(FLAP_LEFT_BRAKE_FACTOR * 32.0);
	flapLeftFlapsPosFactor = (signed int)(FLAP_LEFT_FLAPS_POS_FACTOR * 32.0);
	flapLeftFlapsNegFactor = (signed int)(FLAP_LEFT_FLAPS_NEG_FACTOR * 32.0);
	flapLeftRpFlapsFactor = (signed int)(FLAP_LEFT_RP_FLAPS_FACTOR * 32.0);
	flapLeftLpFlapsFactor = (signed int)(FLAP_LEFT_LP_FLAPS_FACTOR * 32.0);
	flapLeftRpSpeedFlapsFactor = (signed int)(FLAP_LEFT_RP_SPEED_FLAPS_FACTOR * 32.0);
	flapLeftLpSpeedFlapsFactor = (signed int)(FLAP_LEFT_RP_SPEED_FLAPS_FACTOR * 32.0);
	
	flapRightBrakeFactor = (signed int)(FLAP_RIGHT_BRAKE_FACTOR * 32.0);
	flapRightFlapsPosFactor = (signed int)(FLAP_RIGHT_FLAPS_POS_FACTOR * 32.0);
	flapRightFlapsNegFactor = (signed int)(FLAP_RIGHT_FLAPS_NEG_FACTOR * 32.0);
	flapRightRpFlapsFactor = (signed int)(FLAP_RIGHT_RP_FLAPS_FACTOR * 32.0);
	flapRightLpFlapsFactor = (signed int)(FLAP_RIGHT_LP_FLAPS_FACTOR * 32.0);
	flapRightRpSpeedFlapsFactor = (signed int)(FLAP_RIGHT_RP_SPEED_FLAPS_FACTOR * 32.0);
	flapRightLpSpeedFlapsFactor = (signed int)(FLAP_RIGHT_RP_SPEED_FLAPS_FACTOR * 32.0);
	
	aileronRightBrakeFactor = (signed int)(AILERON_RIGHT_BRAKE_FACTOR * 32.0);
	aileronRightFlapsPosFactor = (signed int)(AILERON_RIGHT_FLAPS_POS_FACTOR * 32.0);
	aileronRightFlapsNegFactor = (signed int)(AILERON_RIGHT_FLAPS_NEG_FACTOR * 32.0);
	aileronRightLpFlapsFactor = (signed int)(AILERON_RIGHT_LP_FLAPS_FACTOR * 32.0);
	aileronRightRpFlapsFactor = (signed int)(AILERON_RIGHT_RP_FLAPS_FACTOR * 32.0);
	aileronRightLpSpeedFlapsFactor = (signed int)(AILERON_RIGHT_LP_SPEED_FLAPS_FACTOR * 32.0);
	aileronRightRpSpeedFlapsFactor = (signed int)(AILERON_RIGHT_RP_SPEED_FLAPS_FACTOR * 32.0);
	
	elevatorBrakeFactor = (signed int)(ELEVATOR_BRAKE_FACTOR * 32.0);
	elevatorThrottleFactor = (signed int)(ELEVATOR_THROTTLE_FACTOR * 32.0);
	
	rudderFromAileronFactor = (signed int)(RUDDER_FROM_AILERON_FACTOR * 32.0);
	rudderFactor = (signed int)(RUDDER_FACTOR * 32.0);
	
	throttleFactor = (signed int)(THROTTLE_FACTOR * 32.0);
	
#endif //AIRFRAME_GLIDER
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

#if (AIRFRAME_TYPE == AIRFRAME_GLIDER)
	{
	static int16_t mixerSteps = 0;
	static int16_t aileronInput = 0;
	static int16_t ailInLeftPartFlapsNotSpeed=0;
	static int16_t ailInRightPartFlapsNotSpeed=0;
	static int16_t ailInLeftPartFlapsSpeed=0;
	static int16_t ailInRightPartFlapsSpeed=0;
	static int16_t brakeSelectedTarget;   //resulting brake selection after checking switch/slider, throttle and flight modes, no brake == 0, full brake trottle == 1700
	static int32_t brakeSelectedStep=0;
#if (FLAPS_INPUT_CHANNEL != 0 )
	static int16_t flapsSelectedTarget=0;   //resulting flap selection after checking switch/slider and flight modes, normal speed = 0, slow = -1000, high speed = 1000
#endif
	static int16_t flapsSelectedStep=0;
	static int16_t autopilotThrottleSelected=0;	//used for elevator trim in motorclimb

	// Apply boosts to elevator and rudder if in a controlled mode
	// It does not matter whether the radio is on or not
	if (state_flags._.pitch_feedback)
	{
		pwManual[AILERON_INPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL] ;// in fly by wire or navigate mode, manual input is accounted for in the turn control
		pwManual[ELEVATOR_INPUT_CHANNEL] += ((pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) * elevatorbgain) >> 3;
		pwManual[RUDDER_INPUT_CHANNEL] += ((pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) * rudderbgain) >> 3;
	}
	brakeSelectedTarget = 0;
#if ( BRAKE_THR_SEL_INPUT_CHANNEL == 0 )
	brakeSelectedTarget = ( ((signed int)pwManual[BRAKE_INPUT_CHANNEL]) - SERVOCENTER );
#else
	//left slider up allows throttle and autobrake function
	if ( pwManual[BRAKE_THR_SEL_INPUT_CHANNEL] > (SERVOCENTER + 333) )
	{
#if ( ALTITUDE_GAINS_VARIABLE == 1 )
		//airspeedCntrl.c: controls autopilotBrake. define SPEED_CONTROL 1, GAINS_VARIABLE 1 and  ALTITUDE_GAINS_VARIABLE 1
		if ( get_autopilotBrake() > 0 )
		{
			//assume 0 brake = 0, full brake trottle == 1700
			brakeSelectedTarget = get_autopilotBrake();
		}
#endif
	}
	else
	{
		//left slider controls brake function on throttle stick, below centre is autopilotBrake
		if ( pwManual[BRAKE_THR_SEL_INPUT_CHANNEL] < (SERVOCENTER - 333) )
		{
			brakeSelectedTarget = ( ( SERVOMAX - (signed int)pwManual[THROTTLE_INPUT_CHANNEL] ) );// no brake == 0, full brake trottle == 1700
		}
	}
#endif

	//braking by overspeed, defined in servoMix.c and set in airspeedCntrl.c 0 brake = 0, full brake == 1700
	//overspeedBrake overrules normal brakes if more
	if ( state_flags._.pitch_feedback && (get_overspeedBrake() > brakeSelectedTarget) )
		{
			//overspeedBrake overrules autopilotBrake
			brakeSelectedTarget = get_overspeedBrake() ;
		}
	
	if ( brakeSelectedTarget < 150 )  //remove offset from throttle channel
	{
		brakeSelectedTarget = 0;
	}
	if ( brakeSelectedTarget > 1700 ) //limit
	{
		brakeSelectedTarget = 1700;
	}
	//slow down brake movement - full travel 1700 in ~3.5 sec @200Hz = 12
	if ( brakeSelectedTarget <= ( brakeSelectedStep - 12 ) )
	{
		brakeSelectedStep = brakeSelectedStep - 12;
	}
	else if ( brakeSelectedTarget > ( brakeSelectedStep + 12 ) )
	{
		brakeSelectedStep = brakeSelectedStep + 12;
	}	
	else
	{
		brakeSelectedStep = brakeSelectedTarget;
	}

#if (FLAPS_INPUT_CHANNEL != 0 )
	flapsSelectedTarget = get_flapsSelected();
	//slow down flaps movement - full travel -1000 .. 1000 in ~3.5 sec @200Hz = 12
	if ( flapsSelectedTarget <= ( flapsSelectedStep - 12 ) )
	{
		flapsSelectedStep -= 12;
	}
	else if ( flapsSelectedTarget > ( flapsSelectedStep + 12 ) )
	{
		flapsSelectedStep += 12;
	}	
	else
	{
		flapsSelectedStep = flapsSelectedTarget;
	}
#else
	flapsSelectedStep = 0;
#endif //FLAPS_INPUT_CHANNEL
	aileronInput = pwManual[AILERON_INPUT_CHANNEL] + REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, roll_control + waggle);

	//Calculate ailInLeftPartFlapsNotSpeed, ailInRightPartFlapsNotSpeed, ailInLeftPartFlapsSpeed and ailInRightPartFlapsSpeed
	//only one can be >0 at a time
	//aileron left or right, with autopilotFlaps set to speed (no adverse yaw compensation) or not (adverse yaw compensation)

	aileronInput = aileronInput - SERVOCENTER;
	ailInLeftPartFlapsNotSpeed=0;
	ailInRightPartFlapsNotSpeed=0;
	ailInLeftPartFlapsSpeed=0;
	ailInRightPartFlapsSpeed=0;
	if ( flapsSelectedStep < 500 ) //normal speed slected
	{
		if ( aileronInput > 0 )
		{
			ailInRightPartFlapsNotSpeed=aileronInput;
		}
		else
		{
			ailInLeftPartFlapsNotSpeed=-aileronInput;
		}
	}
	else //speed
	{
		if ( aileronInput > 0 )
		{
			ailInRightPartFlapsSpeed=aileronInput;
		}
		else
		{
			ailInLeftPartFlapsSpeed=-aileronInput;
		}
	}
	mixerSteps = 0;
	mixerSteps += (brakeSelectedStep * aileronLeftBrakeFactor)>>5;
	if ( flapsSelectedStep > 0 ) //speed slected
	{
		mixerSteps += (flapsSelectedStep * aileronLeftFlapsPosFactor)>>5;
	}
	else
	{
		mixerSteps += (flapsSelectedStep * aileronLeftFlapsNegFactor)>>5;
	}	
	mixerSteps += (ailInRightPartFlapsNotSpeed * aileronLeftRpFlapsFactor)>>5; // integer*integer math, /32 to normal scale
	mixerSteps += (ailInLeftPartFlapsNotSpeed * aileronLeftLpFlapsFactor)>>5;
	mixerSteps += (ailInRightPartFlapsSpeed * aileronLeftRpSpeedFlapsFactor)>>5;
	mixerSteps += (ailInLeftPartFlapsSpeed * aileronLeftLpSpeedFlapsFactor)>>5;
	mixerSteps += REVERSE_IF_NEEDED(AILERON_LEFT_OFFSET_REVERSED,AILERON_LEFT_OUTPUT_OFFSET);//boolean and integer, add offset in correct direction
	mixerSteps = REVERSE_IF_NEEDED(AILERON_LEFT_DIR_REVERSED, mixerSteps);
	mixerSteps += SERVOCENTER;
	udb_pwOut[AILERON_LEFT_OUTPUT_CHANNEL] = udb_servo_pulsesat(mixerSteps);

#if (FLAP_LEFT_OUTPUT_CHANNEL != 0 )
	mixerSteps = 0;
	mixerSteps += (brakeSelectedStep * flapLeftBrakeFactor)>>5;
	if ( flapsSelectedStep > 0 ) //speed slected
	{
		mixerSteps += (flapsSelectedStep * flapLeftFlapsPosFactor)>>5;
	}
	else
	{
		mixerSteps += (flapsSelectedStep * flapLeftFlapsNegFactor)>>5;
	}	
	mixerSteps += (ailInRightPartFlapsNotSpeed * flapLeftRpFlapsFactor)>>5;// integer*integer math, /32 to normal scale
	mixerSteps += (ailInLeftPartFlapsNotSpeed * flapLeftLpFlapsFactor)>>5;
	mixerSteps += (ailInRightPartFlapsSpeed * flapLeftRpSpeedFlapsFactor)>>5;
	mixerSteps += (ailInLeftPartFlapsSpeed * flapLeftLpSpeedFlapsFactor)>>5;
	mixerSteps += REVERSE_IF_NEEDED(FLAP_LEFT_OFFSET_REVERSED,FLAP_LEFT_OUTPUT_OFFSET);//boolean and integer, add offset in correct direction
	mixerSteps = REVERSE_IF_NEEDED(FLAP_LEFT_DIR_REVERSED, mixerSteps);
	mixerSteps += SERVOCENTER;
	udb_pwOut[FLAP_LEFT_OUTPUT_CHANNEL] = udb_servo_pulsesat(mixerSteps);
#endif
#if (FLAP_RIGHT_OUTPUT_CHANNEL != 0 )
	mixerSteps = 0;
	mixerSteps += (brakeSelectedStep * flapRightBrakeFactor)>>5;
	if ( flapsSelectedStep > 0 ) //speed slected
	{
		mixerSteps += (flapsSelectedStep * flapLeftFlapsPosFactor)>>5;
	}
	else
	{
		mixerSteps += (flapsSelectedStep * flapLeftFlapsNegFactor)>>5;
	}	
	mixerSteps += (ailInRightPartFlapsNotSpeed * flapRightRpFlapsFactor)>>5;// integer*integer math, /32 to normal scale
	mixerSteps += (ailInLeftPartFlapsNotSpeed * flapRightLpFlapsFactor)>>5;	
	mixerSteps += (ailInRightPartFlapsSpeed * flapRightRpSpeedFlapsFactor)>>5;	
	mixerSteps += (ailInLeftPartFlapsSpeed * flapRightLpSpeedFlapsFactor)>>5;
	mixerSteps += REVERSE_IF_NEEDED(FLAP_RIGHT_OFFSET_REVERSED,FLAP_RIGHT_OUTPUT_OFFSET);//boolean and integer, add offset in correct direction
	mixerSteps = REVERSE_IF_NEEDED(FLAP_RIGHT_DIR_REVERSED, mixerSteps);
	mixerSteps += SERVOCENTER;
	udb_pwOut[FLAP_RIGHT_OUTPUT_CHANNEL] = udb_servo_pulsesat(mixerSteps);
#endif

	mixerSteps = 0;
	mixerSteps += (brakeSelectedStep * aileronRightBrakeFactor)>>5;
	if ( flapsSelectedStep > 0 ) //speed slected
	{
		mixerSteps += (flapsSelectedStep * aileronRightFlapsPosFactor)>>5;
	}
	else
	{
		mixerSteps += (flapsSelectedStep * aileronRightFlapsNegFactor)>>5;
	}	
	mixerSteps += (ailInRightPartFlapsNotSpeed * aileronRightRpFlapsFactor)>>5;// integer*integer math, /32 to normal scale
	mixerSteps += (ailInLeftPartFlapsNotSpeed * aileronRightLpFlapsFactor)>>5;
	mixerSteps += (ailInRightPartFlapsSpeed * aileronRightRpSpeedFlapsFactor)>>5;
	mixerSteps += (ailInLeftPartFlapsSpeed * aileronRightLpSpeedFlapsFactor)>>5;
	mixerSteps += REVERSE_IF_NEEDED(AILERON_RIGHT_OFFSET_REVERSED,AILERON_RIGHT_OUTPUT_OFFSET);//boolean and integer, add offset in correct direction
	mixerSteps = REVERSE_IF_NEEDED(AILERON_RIGHT_DIR_REVERSED, mixerSteps);
	mixerSteps += SERVOCENTER;
	udb_pwOut[AILERON_RIGHT_OUTPUT_CHANNEL] = udb_servo_pulsesat(mixerSteps);


#if (THROTTLE_INPUT_CHANNEL != 0 )
	mixerSteps = 0;

	if (pwManual[THROTTLE_INPUT_CHANNEL] == 0)
	{
		udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = 0;
		autopilotThrottleSelected = 0;
	}
	else
	{
		temp = pwManual[THROTTLE_INPUT_CHANNEL] + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle_control);
		//one channel selects throttle or autopilotBrake function on left stick

		if ( (BRAKE_THR_SEL_INPUT_CHANNEL != 0) && ( pwManual[BRAKE_THR_SEL_INPUT_CHANNEL] < (SERVOCENTER + 333)) )
		{
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat(2000);//throttle off, but keep signal on ESC
			autopilotThrottleSelected = 0;
		}
		else
		{
			throttleSteps = temp - SERVOMIN;
			throttleSteps = (throttleSteps * throttleFactor)>>5;
			autopilotThrottleSelected = throttleSteps;// 0 = 0, full = 2000
			throttleSteps += SERVOMIN;
			temp = (signed int)throttleSteps;
			udb_pwOut[THROTTLE_OUTPUT_CHANNEL] = udb_servo_pulsesat(temp);
		}
	}
#endif  //THROTTLE_INPUT_CHANNEL

	temp = pwManual[RUDDER_INPUT_CHANNEL] + REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, yaw_control - waggle);
	mixerSteps = temp - SERVOCENTER;
	mixerSteps += (aileronInput * rudderFromAileronFactor)>>5;
	mixerSteps += REVERSE_IF_NEEDED(RUDDER_OFFSET_REVERSED,RUDDER_OUTPUT_OFFSET);
	mixerSteps += (mixerSteps * rudderFactor)>>5;
	mixerSteps = REVERSE_IF_NEEDED(RUDDER_DIR_REVERSED, mixerSteps);
	mixerSteps += SERVOCENTER;
	udb_pwOut[RUDDER_OUTPUT_CHANNEL] = udb_servo_pulsesat(mixerSteps);

	temp = pwManual[ELEVATOR_INPUT_CHANNEL] + REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pitch_control);
	mixerSteps = temp - SERVOCENTER;
	mixerSteps += (brakeSelectedStep * elevatorBrakeFactor)>>5;
	mixerSteps += (autopilotThrottleSelected * elevatorThrottleFactor)>>5;
	mixerSteps += REVERSE_IF_NEEDED(ELEVATOR_OFFSET_REVERSED,ELEVATOR_OUTPUT_OFFSET);
	mixerSteps = REVERSE_IF_NEEDED(ELEVATOR_DIR_REVERSED, mixerSteps);
	mixerSteps += SERVOCENTER;
	udb_pwOut[ELEVATOR_OUTPUT_CHANNEL] = udb_servo_pulsesat(mixerSteps);

#if (BRAKE_OUTPUT_CHANNEL != 0 )
	//Brake control or logging
	mixerSteps = brakeSelectedStep;
	mixerSteps += SERVOMIN;
	udb_pwOut[BRAKE_OUTPUT_CHANNEL] = udb_servo_pulsesat( mixerSteps );
#endif

#if (FLAPS_OUTPUT_CHANNEL != 0 )
	//Flaps control or logging
	mixerSteps = flapsSelectedStep;
	mixerSteps += SERVOMIN;
	udb_pwOut[FLAPS_OUTPUT_CHANNEL] = udb_servo_pulsesat( mixerSteps );
#endif

	}
#endif // AIRFRAME_GLIDER


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

	//	Reverse the polarity of the secondary aileron if necessary
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
