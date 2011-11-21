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


// main program for testing the IMU.


#include "../libDCM/libDCM.h"


void send_debug_line( void ) ;
void motorCntrl( void ) ;


int main (void)
{
	// Set up the libraries
	udb_init() ;
	dcm_init() ;
	
	udb_serial_set_rate(19200) ;
	
	LED_GREEN = LED_OFF ;
	
	// Start it up!
	udb_run() ;  // This never returns.
	
	return 0 ;
}


// Called every 1/2 second at high priority
void udb_background_callback_periodic(void)
{
	if (!dcm_flags._.calib_finished)
	{
		// If still calibrating, blink RED
		udb_led_toggle(LED_RED) ;
	}
	else
	{
		// No longer calibrating: solid RED and send debug output
		LED_RED = LED_ON ;
	}
	
	return ;
}


// Called every time we get gps data (1, 2, or 4 Hz, depending on GPS config)
void dcm_callback_gps_location_updated(void)
{
	// Blink GREEN led to show that the GPS is communicating
	udb_led_toggle(LED_GREEN) ;
	return ;
}


// Called at 40 Hz, before sending servo pulses
void dcm_servo_callback_prepare_outputs(void)
{
	motorCntrl() ;
	
	// Serial output at 2Hz  (40Hz / 20)
	if (udb_heartbeat_counter % 20 == 0)
	{
		send_debug_line() ;
	}
	
	return ;
}


void udb_callback_radio_did_turn_off( void ) {}
