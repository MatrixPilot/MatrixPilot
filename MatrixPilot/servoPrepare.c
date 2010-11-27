// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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

//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int gpscount ; // counter to initialize GPS
int calibcount ; // number of PWM pulses before control is turned on
int pitch_control, roll_control, yaw_control, altitude_control ;

char eightHertzCounter = 0 ;
boolean startTelemetry = 0 ;


void manualPassthrough( void ) ;


void init_servoPrepare( void )	// initialize the PWM
{
	calibcount = 400 ;	// wait 400 PWM pulses before turning on the control (10 seconds)
	gpscount = 1000 ;	// wait 25 seconds for GPS to initialize

	int i;
	for (i=0; i <= NUM_INPUTS; i++)
		udb_pwTrim[i] = udb_pwIn[i] = ((i == THROTTLE_INPUT_CHANNEL) ? 0 : 3000) ;
	
	for (i=0; i <= NUM_OUTPUTS; i++)
		udb_pwTrim[i] = udb_pwIn[i] = udb_pwOut[i] = ((i == THROTTLE_INPUT_CHANNEL) ? 0 : 3000) ;
	
#if (NORADIO == 1)
	udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = udb_pwTrim[MODE_SWITCH_INPUT_CHANNEL] = 4000 ;
#endif
	
	return ;
}


void dcm_servo_callback_prepare_outputs(void)
{
	switch ( calibcount ) {
		// case 0 is when the control is up and running
			
		case 0: {
			
#if ( DEADRECKONING == 1 )
			process_flightplan() ;
#endif
			
			updateBehavior() ;
			rollCntrl() ;
			yawCntrl() ;
			altitudeCntrl();
			pitchCntrl() ;
			servoMix() ;
#if ( USE_CAMERA_STABILIZATION == 1 )
			cameraCntrl();
#endif
			updateTriggerAction() ;
			break ;
		}
			
		case 1: {
			// almost ready to turn the control on, save the sensor offsets to allow testing
			// the dcm before the radio is on.  we record offsets again along with trims
			// immediately before the first wag.
			dcm_calibrate() ;
			manualPassthrough() ;	// Allow manual control while starting up
			startTelemetry = 1 ;
			break ;
		}
		default: {
			// otherwise, there is not anything to do
			manualPassthrough() ;	// Allow manual control while starting up
			break ;
		}
	}
	
	// count down the startup counter to 0
	if ( calibcount > 0 ) calibcount-- ;
	
	// FIXME: this code should move into libDCM
	// count down the startup counter to 0
	if ( gpscount > 0 )
	{
		gps_startup_sequence(gpscount) ;
		gpscount-- ;
	}
	
	// This is a simple counter to do stuff at 8hz
	eightHertzCounter++ ;
	if ( eightHertzCounter >= 5 )
	{
		if ( startTelemetry )
		{
			serial_output_8hz() ;
		}
		eightHertzCounter = 0 ;
	}
	
#if (USE_OSD == 1)
	osd_countdown(gpscount) ;
#endif
	
	return ;
}

void manualPassthrough( void )
{
	roll_control = pitch_control = yaw_control = altitude_control = 0 ;
	servoMix() ;
	
	return ;
}

