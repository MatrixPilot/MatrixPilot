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

// Responsible for 
// Mode control of data inputs to the mixer.
// Safety of outputs from the mixer.

#include "defines.h"
#include "fbw_options.h"
#include "fbwCntrl.h"

#if(USE_INPUT_CONTROL == 1)

// RMAX scaled inputs
fractional out_cntrls[IN_CNTRL_MAX];

// rmax scaled autopilot controls
fractional ap_cntrls[AP_CNTRL_MAX];

// Do linear manual overide of autopilot controls.
// AP has full authority and manual no authority with no manual at control centre
// AP has zero authority and manual full authority with full manual input deflection
// Modifies values in both out_cntrls and ap_cntrls
void linear_mux_overide(IN_CNTRL in_control, AP_CNTRL ap_control);

void output_controls(void)
{
	long temp ;

	// If radio is off, fix control values to zero
	if (udb_flags._.radio_on)
	{
		for (temp = 0; temp <= IN_CNTRL_MAX; temp++)
			out_cntrls[temp] = in_cntrls[temp];
	}
	else
	{
		for (temp = 0; temp <= IN_CNTRL_MAX; temp++)
			out_cntrls[temp] = 0;
	}

#if(AUTOPILOT_OUTPUT_PWM == 1)
	// AP controls translated to RMAX scaled values
	ap_cntrls[AP_CNTRL_PITCH]		= PWM_to_frac(pitch_control		,0	, false);
	ap_cntrls[AP_CNTRL_ROLL]		= PWM_to_frac(roll_control		,0	, false);
	ap_cntrls[AP_CNTRL_THROTTLE]	= PWM_to_frac(throttle_control	,0	, false);
	ap_cntrls[AP_CNTRL_YAW]			= PWM_to_frac(yaw_control		,0	, false);
	ap_cntrls[AP_CNTRL_WAGGLE]		= PWM_to_frac(waggle			,0	, false);
	// Controls zero until this control is implemented
	ap_cntrls[AP_CNTRL_CAMBER]		= 0;
	ap_cntrls[AP_CNTRL_BRAKE]		= 0;
	ap_cntrls[AP_CNTRL_FLAP]		= 0;
#else
	ap_cntrls[AP_CNTRL_PITCH]		= pitch_control;
	ap_cntrls[AP_CNTRL_ROLL]		= roll_control;
	ap_cntrls[AP_CNTRL_THROTTLE]	= throttle_control;
	ap_cntrls[AP_CNTRL_YAW]			= yaw_control;
	ap_cntrls[AP_CNTRL_WAGGLE]		= waggle;
	// Controls zero until this control is implemented
	ap_cntrls[AP_CNTRL_CAMBER]		= 0;
	ap_cntrls[AP_CNTRL_BRAKE]		= 0;
	ap_cntrls[AP_CNTRL_FLAP]		= 0;
#endif

#if(USE_FBW == 1)
	if(ap_state() == AP_STATE_STABILIZED)
		if(fbwManualControlLockout(IN_CNTRL_ROLL) == true)
			out_cntrls[IN_CNTRL_ROLL] = 0;
#endif //(USE_FBW == 1)	

#if(OUTPUT_CONTROL_GAIN_MUX == 1)
	linear_mux_overide(IN_CNTRL_ROLL, AP_CNTRL_ROLL);
	linear_mux_overide(IN_CNTRL_PITCH, AP_CNTRL_PITCH);
	linear_mux_overide(IN_CNTRL_YAW, AP_CNTRL_YAW);

 #if(OUT_CNTRL_AP_MAN_PREMIX == 1)
	out_cntrls[IN_CNTRL_ROLL] 		= 	out_cntrls[IN_CNTRL_ROLL] 	+ ap_cntrls[AP_CNTRL_ROLL];
	out_cntrls[IN_CNTRL_PITCH] 		= 	out_cntrls[IN_CNTRL_PITCH] 	+ ap_cntrls[AP_CNTRL_PITCH];
	out_cntrls[IN_CNTRL_YAW] 		= 	out_cntrls[IN_CNTRL_YAW] 	+ ap_cntrls[AP_CNTRL_YAW];
 #endif //(OUT_CNTRL_AP_MAN_PREMIX == 1)

#endif	//(OUTPUT_CONTROL_GAIN_MUX == 1)

#if(OUTPUT_CONTROL_IN_PWM_UNITS == 1)
	out_cntrls[IN_CNTRL_ROLL] 		= frac_to_PWM(out_cntrls[IN_CNTRL_ROLL]		, udb_pwTrim(ROLL_INPUT_CHANNEL)	, ROLL_CHANNEL_REVERSED);
	out_cntrls[IN_CNTRL_PITCH] 		= frac_to_PWM(out_cntrls[IN_CNTRL_PITCH]	, udb_pwTrim(PITCH_INPUT_CHANNEL)	, PITCH_CHANNEL_REVERSED);
	out_cntrls[IN_CNTRL_YAW] 		= frac_to_PWM(out_cntrls[IN_CNTRL_YAW]		, udb_pwTrim(YAW_INPUT_CHANNEL)		, YAW_CHANNEL_REVERSED);
	// TODO: Correct throttle scaling
	out_cntrls[IN_CNTRL_THROTTLE] 	= frac_to_PWM(out_cntrls[IN_CNTRL_THROTTLE]	, udb_pwTrim(THROTTLE_INPUT_CHANNEL), THROTTLE_CHANNEL_REVERSED);

	ap_cntrls[AP_CNTRL_PITCH]		= frac_to_PWM(pitch_control		, udb_pwTrim(PITCH_INPUT_CHANNEL)		, PITCH_CHANNEL_REVERSED);
	ap_cntrls[AP_CNTRL_ROLL]		= frac_to_PWM(roll_control		, udb_pwTrim(ROLL_INPUT_CHANNEL)		, ROLL_CHANNEL_REVERSED);
	// TODO, Correct throttle scaling
	ap_cntrls[AP_CNTRL_THROTTLE]	= frac_to_PWM(throttle_control	, udb_pwTrim(THROTTLE_INPUT_CHANNEL)	, THROTTLE_CHANNEL_REVERSED);
	ap_cntrls[AP_CNTRL_YAW]			= frac_to_PWM(yaw_control		, udb_pwTrim(YAW_INPUT_CHANNEL)			, YAW_CHANNEL_REVERSED);
	ap_cntrls[AP_CNTRL_WAGGLE]		= frac_to_PWM(waggle			, 0										, false);
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

	if(out_cntrls[IN_CNTRL_PITCH] >= 0)
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


#endif	//#if(USE_INPUT_CONTROL == 1)
