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

// outputCntrl responsible for:
// Mode control of data inputs to the mixer.
// Pre mixing of ap and manual controls to the mixer.
// Scaliing of data from ap and to mixer.

#include "defines.h"
#include "fbw_options.h"
#include "fbwCntrl.h"
#include "inputCntrl.h"

// RMAX scaled output commands to the mixer
fractional out_cntrls[IN_CNTRL_MAX];

// rmax scaled autopilot controls to the mixer
fractional ap_cntrls[AP_CNTRL_MAX];

// mixer outputs before safety and limit checks
int mixer_outputs[NUM_OUTPUTS + 1];

// Do linear manual overide of autopilot controls.
// AP has full authority and manual no authority with no manual at control centre
// AP has zero authority and manual full authority with full manual input deflection
// Modifies values in both out_cntrls and ap_cntrls
void linear_mux_overide(IN_CNTRL in_control, AP_CNTRL ap_control);

// Safe copying of radio inputs to control outputs
// If radio is off, sets all values to zero.
inline void safe_radio_inputs_to_outputs(void);

// Copy AP controls to 
// Scale AP controls to RMAX if needed
inline void scale_ap_controls_to_outputs(void);

// When requried, lockout manual control.
// Used for fly-by-wire modes
inline void manual_control_lockouts(void);

// pre-mix manual and autopilot controls.
// Autopilot has full authority when there is no control deflection
// Autopilot has zero authority at full control deflection
inline void control_pre_mixing(void);

// Scale and place outputs to that required by the mixer
inline void output_mixer_format(void);


// Change autopilot output and manual input into safe formatted mixer input.
// Also optionally do pre-mixing of manual and autopilot commands.
void pre_mix(void)
{
	safe_radio_inputs_to_outputs();
	scale_ap_controls_to_outputs();
	manual_control_lockouts();
	control_pre_mixing();
	output_mixer_format();
}	


void post_mix(void)
{
	int16_t throttle;

#if(MIXER_OUTPUTS_TO_UDB == 1)
 #if(DO_SAFE_THROTTLE_MIXING == 1)
	
	// Only mixes autopilot throttle when not in manual mode.
	if( get_flightmode() != FLIGHT_MODE_MANUAL )
		throttle = ap_cntrls[AP_CNTRL_THROTTLE] + out_cntrls[IN_CNTRL_THROTTLE];
	else
		throttle = out_cntrls[IN_CNTRL_THROTTLE];

	throttle = frac_to_PWM(throttle, udb_pwTrim[THROTTLE_INPUT_CHANNEL],  
											THROTTLE_CHANNEL_REVERSED,	true);
 #else
	throttle = mixer_outputs[THROTTLE_OUTPUT_CHANNEL];
 #endif  // (DO_SAFE_THROTTLE_MIXING == 1)


	// Limit throttle channel depending on it being reversed. 
	// Prevents ESCs with automatic minimum throttle from getting confused
 #if(THROTTLE_CHANNEL_REVERSED == 1)
	if(throttle > udb_pwTrim[THROTTLE_INPUT_CHANNEL])
		throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
 #else
	if(throttle < udb_pwTrim[THROTTLE_INPUT_CHANNEL])
		throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
	

	// Forces no throttle output before rc PWM input has been recieved
	if(udb_pwIn[THROTTLE_INPUT_CHANNEL] == 0)
		throttle = 0;

	mixer_outputs[THROTTLE_OUTPUT_CHANNEL] = throttle;

 	int16_t index;
	for(index = 0; index <= NUM_OUTPUTS; index++)
		if(index == THROTTLE_OUTPUT_CHANNEL)
			udb_pwOut[index] = throttle;
		else
			udb_pwOut[index] = udb_servo_pulsesat(mixer_outputs[index]);
		
 #endif // (THROTTLE_CHANNEL_REVERSED == 1)

#endif  // (MIXER_OUTPUTS_TO_UDB == 1)
}


inline void safe_radio_inputs_to_outputs(void)
{
	int32_t temp ;

	// If radio is off, fix control values to zero
	if (udb_flags._.radio_on)
	{
		for (temp = 0; temp < IN_CNTRL_MAX; temp++)
			out_cntrls[temp] = in_cntrls[temp];
	}
	else
	{
		for (temp = 0; temp < IN_CNTRL_MAX; temp++)
			out_cntrls[temp] = 0;
	}
}


inline void scale_ap_controls_to_outputs(void)
{
	// Convert waggle in PWM units to fractional
	ap_cntrls[AP_CNTRL_WAGGLE]		= PWM_to_frac(waggle			,0	, false);

	// Controls zero until this control is implemented
	ap_cntrls[AP_CNTRL_CAMBER]		= 0;
	ap_cntrls[AP_CNTRL_BRAKE]		= 0;
	ap_cntrls[AP_CNTRL_FLAP]		= 0;
}

inline void manual_control_lockouts(void)
{
#if(USE_FBW == 1)
	if( get_flightmode() == FLIGHT_MODE_ASSISTED)
	{
		if(fbwManualControlLockout(IN_CNTRL_ROLL) == true)
			out_cntrls[IN_CNTRL_ROLL] = 0;
		if(fbwManualControlLockout(IN_CNTRL_PITCH) == true)
			out_cntrls[IN_CNTRL_PITCH] = 0;
	}

	if( mode_autopilot_enabled() )
	{
		if(get_throttle_manual_lockout() == true)
			out_cntrls[IN_CNTRL_THROTTLE] = 0;
	}

#endif //(USE_FBW == 1)	
}


inline void control_pre_mixing(void)
{
#if(OUTPUT_CONTROL_GAIN_MUX == 1)
	// Only do linear mux in non manual modes
	if( get_flightmode() != FLIGHT_MODE_MANUAL )
	{
		linear_mux_overide(IN_CNTRL_ROLL, AP_CNTRL_ROLL);
		linear_mux_overide(IN_CNTRL_PITCH, AP_CNTRL_PITCH);
		linear_mux_overide(IN_CNTRL_YAW, AP_CNTRL_YAW);
	
	 #if(OUT_CNTRL_AP_MAN_PREMIX == 1)
		out_cntrls[IN_CNTRL_ROLL] 		= 	out_cntrls[IN_CNTRL_ROLL] 	+ ap_cntrls[AP_CNTRL_ROLL];
		out_cntrls[IN_CNTRL_PITCH] 		= 	out_cntrls[IN_CNTRL_PITCH] 	+ ap_cntrls[AP_CNTRL_PITCH];
		out_cntrls[IN_CNTRL_YAW] 		= 	out_cntrls[IN_CNTRL_YAW] 	+ ap_cntrls[AP_CNTRL_YAW];
	 #endif //(OUT_CNTRL_AP_MAN_PREMIX == 1)
	}

#endif	//(OUTPUT_CONTROL_GAIN_MUX == 1)
}


inline void output_mixer_format(void)
{
#if(OUTPUT_CONTROL_IN_PWM_UNITS == 1)
	out_cntrls[IN_CNTRL_ROLL] 		= frac_to_PWM(out_cntrls[IN_CNTRL_ROLL]		, udb_pwTrim(ROLL_INPUT_CHANNEL)	, ROLL_CHANNEL_REVERSED,	false);
	out_cntrls[IN_CNTRL_PITCH] 		= frac_to_PWM(out_cntrls[IN_CNTRL_PITCH]	, udb_pwTrim(PITCH_INPUT_CHANNEL)	, PITCH_CHANNEL_REVERSED,	false);
	out_cntrls[IN_CNTRL_YAW] 		= frac_to_PWM(out_cntrls[IN_CNTRL_YAW]		, udb_pwTrim(YAW_INPUT_CHANNEL)		, YAW_CHANNEL_REVERSED,		false);

	out_cntrls[IN_CNTRL_THROTTLE] 	= frac_to_PWM(out_cntrls[IN_CNTRL_THROTTLE]	, 
											udb_pwTrim(THROTTLE_INPUT_CHANNEL), 
											THROTTLE_CHANNEL_REVERSED, true);

	ap_cntrls[AP_CNTRL_PITCH]		= frac_to_PWM(pitch_control		, udb_pwTrim(PITCH_INPUT_CHANNEL)		, PITCH_CHANNEL_REVERSED,	false);
	ap_cntrls[AP_CNTRL_ROLL]		= frac_to_PWM(roll_control		, udb_pwTrim(ROLL_INPUT_CHANNEL)		, ROLL_CHANNEL_REVERSED,	false);

	ap_cntrls[AP_CNTRL_THROTTLE]	= frac_to_PWM(throttle_control	, 
											udb_pwTrim(THROTTLE_INPUT_CHANNEL)	, 
											THROTTLE_CHANNEL_REVERSED, true);

	ap_cntrls[AP_CNTRL_YAW]			= frac_to_PWM(yaw_control		, udb_pwTrim(YAW_INPUT_CHANNEL)			, YAW_CHANNEL_REVERSED,	false);
	ap_cntrls[AP_CNTRL_WAGGLE]		= frac_to_PWM(waggle			, 0										, false,				false);
	// Controls zero until this control is implemented
	ap_cntrls[AP_CNTRL_CAMBER]		= 0;
	ap_cntrls[AP_CNTRL_BRAKE]		= 0;
	ap_cntrls[AP_CNTRL_FLAP]		= 0;
	
#endif	//(OUTPUT_CONTROL_IN_PWM_UNITS == 1)
}

// Do linear manual overide of autopilot controls.
// AP has full authority and manual no authority with no manual at control centre
// AP has zero authority and manual full authority with full manual input deflection
// Modifies values in both out_cntrls and ap_cntrls
void linear_mux_overide(IN_CNTRL in_control, AP_CNTRL ap_control)
{
	fractional mux_gain;
	union longww temp;

	if(out_cntrls[in_control] >= 0)
		mux_gain = out_cntrls[in_control];
	else
		mux_gain = -out_cntrls[in_control];

	if(mux_gain > RMAX)
		mux_gain = RMAX;

	temp.WW = __builtin_mulss( mux_gain, out_cntrls[in_control]);  //
	temp.WW <<= 2;
	out_cntrls[in_control] = temp._.W1;
	
	mux_gain = RMAX - mux_gain;

	temp.WW = __builtin_mulss( mux_gain, ap_cntrls[ap_control]);  //
	temp.WW <<= 2;
	ap_cntrls[ap_control] = temp._.W1;

	return;
}


