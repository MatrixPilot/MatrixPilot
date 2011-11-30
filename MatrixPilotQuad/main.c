// This file is part of the MatrixPilotQuad firmware.
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


#include "../libDCM/libDCM.h"

boolean didCalibrate = 0 ;

void send_debug_line( void ) ;
void motorCntrl( void ) ;


int main (void)
{
	// Set up the libraries
	udb_init() ;
	dcm_init() ;
	
	udb_serial_set_rate(57600) ;
	
	LED_GREEN = LED_OFF ;
	
	// Start it up!
	udb_run() ;  // This never returns.
	
	return 0 ;
}


// Called every 1/2 second at high priority
void udb_background_callback_periodic(void)
{
	if (!didCalibrate)
	{
		// If still calibrating, blink RED
		udb_led_toggle(LED_RED) ;
		
		if (udb_flags._.radio_on && dcm_flags._.calib_finished)
		{
			udb_servo_record_trims() ;
			dcm_calibrate() ;
			didCalibrate = 1 ;
		}
	}
	else
	{
		// No longer calibrating: solid RED
		LED_RED = LED_ON ;
	}
	
	return ;
}


// Called every time we get gps data (1, 2, or 4 Hz, depending on GPS config)
void dcm_callback_gps_location_updated(void)
{
	return ;
}


// Called at 40 Hz, before sending servo pulses
void dcm_servo_callback_prepare_outputs(void)
{
	motorCntrl() ;
	
	// Update the Green LED to show RC radio status
	if (udb_flags._.radio_on)
	{
		LED_GREEN = LED_ON ;
	}
	else
	{
		LED_GREEN = LED_OFF ;
	}
	
	// Serial output at 8Hz  (40Hz / 5)
	if (udb_heartbeat_counter % 5 == 0)
	{
		send_debug_line() ;
	}
	
	return ;
}


void udb_callback_radio_did_turn_off( void ) {}
