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
#include "../MatrixPilot/mode_switch.h"
#include "airspeedCntrlFBW.h"
#include "fbw_options.h"
#include "inputCntrl.h"
#include "fbwCntrl.h"
#include "motionCntrl.h"
#include "autopilotCntrl.h"

//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int pitch_control, roll_control, yaw_control, throttle_control ;

void manualPassthrough( void ) ;


void init_servoPrepare( void )	// initialize the PWM
{
	int16_t i;

#if(USE_NV_MEMORY == 1)
	if(udb_skip_flags.skip_radio_trim == 1)
		return;
#endif

	for (i=0; i <= NUM_INPUTS; i++)
#if (FIXED_TRIMPOINT == 1)
		udb_pwTrim[i] = udb_pwIn[i] = ((i == THROTTLE_INPUT_CHANNEL) ? THROTTLE_TRIMPOINT : CHANNEL_TRIMPOINT ) ;
#else
		udb_pwIn[i] = udb_pwTrim[i] = ((i == THROTTLE_INPUT_CHANNEL) ? 0 : 3000) ;	
#endif

#if (FIXED_TRIMPOINT == 1)
	for (i=0; i <= NUM_OUTPUTS; i++)
		udb_pwOut[i] = ((i == THROTTLE_OUTPUT_CHANNEL) ? THROTTLE_TRIMPOINT : CHANNEL_TRIMPOINT) ;
#else
	for (i=0; i <= NUM_OUTPUTS; i++)
		udb_pwOut[i] = ((i == THROTTLE_OUTPUT_CHANNEL) ? 0 : 3000) ;
#endif

	
#if (NORADIO == 1)
	udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = udb_pwTrim[MODE_SWITCH_INPUT_CHANNEL] = 4000 ;
#endif
	
	return ;
}

unsigned int16_t wind_gain ;

void dcm_servo_callback_prepare_outputs(void)
{
	input_controls();

//	positionCntrl();		// Calculate heading and 

	if (dcm_flags._.calib_finished)
	{
		flight_mode_switch_2pos_poll();

		updateBehavior() ;

		fbwDemandCntrl();

#if ( DEADRECKONING == 1 )
		process_flightplan() ;
#endif	

//		wind_gain = wind_gain_adjustment () ;

		airspeedCntrl();	// Calculate a safe demand airspeed wrt ground

		airframeStateUpdate();	// Update airframe conditions
	
		altitudeCntrl();	// Calculate energy-altitude corrections

        autopilotCntrl();       // Choose correct demand and turn into aircraft target vector.

		motionCntrl();		// Calculate rotation rate and pitch corrections

		throttleCntrl();        // Throttle filtering
		pitchCntrl() ;
		yawCntrl() ;
		rollCntrl() ;		// roll done last since pitch must run first

		pre_mix();
		servoMix() ;

#if ( USE_CAMERA_STABILIZATION == 1 )
		cameraCntrl() ;
#endif
		cameraServoMix() ;

		post_mix();

		updateTriggerAction() ;
	}
	else
	{
		// otherwise, there is not anything to do
		manualPassthrough() ;	// Allow manual control while starting up
	}
	
	if ( dcm_flags._.calib_finished ) // start telemetry after calibration
	{
#if ( SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK )
		mavlink_output_40hz() ;
#else
		// This is a simple check to send telemetry at 8hz
		if (udb_heartbeat_counter % 5 == 0)
		{
			serial_output_8hz() ;
		}
#endif
	}
	
#if (USE_OSD == 1)
	osd_run_step() ;
#endif
	
	return ;
}

void manualPassthrough( void )
{
	roll_control = pitch_control = yaw_control = throttle_control = 0 ;

#if(USE_INPUT_CONTROL == 1)
	pre_mix();
#endif  // (USE_INPUT_CONTROL == 1)

	servoMix() ;
	
#if(USE_INPUT_CONTROL == 1)
	post_mix();
#endif  // (USE_INPUT_CONTROL == 1)

	return ;
}

