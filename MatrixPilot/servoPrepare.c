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
#include "mode_switch.h"
#include "airspeedCntrl.h"
#include "motorCntrl.h"

uint16_t wind_gain ;

#if (AIRFRAME_TYPE == AIRFRAME_HELI)
#error "airframe type heli not supported"
#elif (AIRFRAME_TYPE == AIRFRAME_MULTI)
// Called at HEARTBEAT_HZ, before sending servo pulses
boolean telem_on = false;

void dcm_servo_callback_prepare_outputs(void) {
    static int pidCounter = 0;

#if (BOARD_TYPE == AUAV2_BOARD_ALPHA1)
    if (sbusDAV) parseSbusData();
#endif

    // PID loop at x Hz
    if (++pidCounter >= HEARTBEAT_HZ / PID_HZ) {
        pidCounter = 0;
        motorCntrl();
    }
    // don't send telemetry till calibrated
    if (TEL_ALWAYS_ON || didCalibrate) {
        if (telem_on && !throttleUp) {
            // telemetry was just stopped, output gains
            sendGains = true;
            telem_on = false;
        }
#if (TELEMETRY_TYPE != 9)
        static int telCounter = 0;
        // send telemetry if not in failsafe mode, or if gains need recording
        // stops telemetry when failsafe is activated;
        // after .5 second OpenLog will sync its logfile and card may be removed
        if (TEL_ALWAYS_ON || throttleUp || sendGains) {
            if (throttleUp) telem_on = true;
            // Serial output at TELEMETRY_HZ
            if (++telCounter >= HEARTBEAT_HZ / TELEMETRY_HZ) {
                telCounter = 0;
                callSendTelemetry = true;
                // call at IPL3
                //                send_telemetry();
            }
        }
#endif
    }
    return;
}
#else
//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int16_t pitch_control, roll_control, yaw_control, throttle_control ;

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

void dcm_servo_callback_prepare_outputs(void)
{
	if (dcm_flags._.calib_finished)
	{
		flight_mode_switch_2pos_poll();
#if ( DEADRECKONING == 1 )
		process_flightplan() ;
#endif
#if(ALTITUDE_GAINS_VARIABLE == 1)
		airspeedCntrl();
#endif // ALTITUDE_GAINS_VARIABLE == 1
		updateBehavior() ;
		wind_gain = wind_gain_adjustment () ;
		rollCntrl() ;
		yawCntrl() ;
		altitudeCntrl();
		pitchCntrl() ;
		servoMix() ;
#if ( USE_CAMERA_STABILIZATION == 1 )
		cameraCntrl() ;
#endif
		cameraServoMix() ;
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
	servoMix() ;
	
	return ;
}
#endif

