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


#include "../MatrixPilot/defines.h"
#include "fbw_options.h"
#include "inputCntrl.h"

//	Perform control based on the airframe type.
//	Use the radio to determine the baseline pulse widths if the radio is on.
//	Otherwise, use the trim pulse width measured during power up.
//
//	Mix computed roll and pitch controls into the output channels for the compiled airframe type

#if(USE_INPUT_CONTROL == 1)

// This uses pre-mixed controls from outputCntrl with RMAX scaling.  Use options
// OUTPUT_CONTROL_GAIN_MUX=1
// OUT_CNTRL_AP_MAN_PREMIX=1
// MIXER_OUTPUTS_TO_UDB=1
// DO_SAFE_THROTTLE_MIXING=1
// OUTPUT_CONTROL_IN_PWM_UNITS=0

#if(OUTPUT_CONTROL_GAIN_MUX != 1)
	#error("Use option OUTPUT_CONTROL_GAIN_MUX=1")
#endif

#if(OUT_CNTRL_AP_MAN_PREMIX != 1)
	#error("Use option OUT_CNTRL_AP_MAN_PREMIX=1")
#endif

#if(MIXER_OUTPUTS_TO_UDB != 1)
	#error("Use option MIXER_OUTPUTS_TO_UDB=1")
#endif

#if(DO_SAFE_THROTTLE_MIXING != 1)
	#error("Use option DO_SAFE_THROTTLE_MIXING=1")
#endif

#if(OUTPUT_CONTROL_IN_PWM_UNITS != 0)
	#error("Use option OUTPUT_CONTROL_IN_PWM_UNITS=0")
#endif

void servoMix( void )
{
	int32_t temp ;
		
	// Standard airplane airframe
	// Mix roll_control into ailerons
	// Mix pitch_control into elevators
	// Mix yaw control and waggle into rudder
#if ( AIRFRAME_TYPE == AIRFRAME_STANDARD )
		mixer_outputs[AILERON_OUTPUT_CHANNEL] = 
			frac_to_PWM(out_cntrls[IN_CNTRL_ROLL], udb_pwTrim[ROLL_INPUT_CHANNEL], AILERON_CHANNEL_REVERSED, false);

		mixer_outputs[AILERON_SECONDARY_OUTPUT_CHANNEL] = 
			frac_to_PWM(out_cntrls[IN_CNTRL_ROLL], CHANNEL_TRIMPOINT, AILERON_SECONDARY_CHANNEL_REVERSED, false);

		mixer_outputs[AILERON_OUTPUT_CHANNEL] = 
			frac_to_PWM(out_cntrls[IN_CNTRL_PITCH], udb_pwTrim[PITCH_INPUT_CHANNEL], ELEVATOR_CHANNEL_REVERSED, false);

		mixer_outputs[RUDDER_OUTPUT_CHANNEL] = 
			frac_to_PWM(out_cntrls[IN_CNTRL_YAW] + ap_cntrls[AP_CNTRL_WAGGLE], udb_pwTrim[YAW_INPUT_CHANNEL], RUDDER_CHANNEL_REVERSED, false);
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
		
		mixer_outputs[PASSTHROUGH_A_OUTPUT_CHANNEL] = udb_servo_pulsesat( udb_pwIn[PASSTHROUGH_A_INPUT_CHANNEL] ) ;
		mixer_outputs[PASSTHROUGH_B_OUTPUT_CHANNEL] = udb_servo_pulsesat( udb_pwIn[PASSTHROUGH_B_INPUT_CHANNEL] ) ;
		mixer_outputs[PASSTHROUGH_C_OUTPUT_CHANNEL] = udb_servo_pulsesat( udb_pwIn[PASSTHROUGH_C_INPUT_CHANNEL] ) ;
		mixer_outputs[PASSTHROUGH_D_OUTPUT_CHANNEL] = udb_servo_pulsesat( udb_pwIn[PASSTHROUGH_D_INPUT_CHANNEL] ) ;
}



#endif //(USE_INPUT_CONTROL == 1)

