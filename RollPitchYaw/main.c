// This file is part of the MatrixPilot RollPitchYaw demo.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2013 MatrixPilot Team
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
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/rmat.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/ADchannel.h"

// Used for serial debug output
#include <stdio.h>

char debug_buffer[128];
int db_index = 0;
void send_debug_line(void);

#define RECORD_OFFSETS ( 1 ) // set to 1 in order to record accelerometer and gyro offsets in telemetry

int main(void)
{
	mcu_init();
	gps_init();     // this sets function pointers 
	// Set up the libraries
	udb_init();
	dcm_init();
#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE 19200 // default
#warning SERIAL_BAUDRATE set to default value of 19200 bps
#endif
	udb_serial_set_rate(SERIAL_BAUDRATE);

#if (CONSOLE_UART != 2)
	udb_init_USART(&udb_serial_callback_get_byte_to_send, &udb_serial_callback_received_byte);
#endif

	led_off(LED_GREEN);

	// Start it up!
	while (1)
	{
		udb_run();
	}

	return 0;
}

// Called every 1/40 second at high priority
void udb_heartbeat_40hz_callback(void)
{
	static int count = 0;

	if (!dcm_flags._.calib_finished)
	{
		// If still calibrating, blink RED
		if (++count > 20)
		{
			count = 0;
			udb_led_toggle(LED_RED);
		}
	}
	else
	{
		// No longer calibrating: solid RED and send debug output
		led_on(LED_RED);
	}
}

// Called every time we get gps data (1, 2, or 4 Hz, depending on GPS config)
void dcm_callback_gps_location_updated(void)
{
	// Blink GREEN led to show that the GPS is communicating
	udb_led_toggle(LED_GREEN);
}

// Called at HEARTBEAT_HZ, before sending servo pulses
void dcm_heartbeat_callback(void) // was called dcm_servo_callback_prepare_outputs()
{
	if (!dcm_flags._.calib_finished)
	{
		udb_pwOut[ROLL_OUTPUT_CHANNEL] = 3000;
		udb_pwOut[PITCH_OUTPUT_CHANNEL] = 3000;
		udb_pwOut[YAW_OUTPUT_CHANNEL] = 3000;
	}
	else
	{
		union longww accum;

		accum.WW = __builtin_mulss(rmat[6], 4000);
		udb_pwOut[ROLL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

		accum.WW = __builtin_mulss(rmat[7], 4000);
		udb_pwOut[PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

		accum.WW = __builtin_mulss(rmat[4], 4000);
		udb_pwOut[YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);
	}

	// Serial output at 2Hz  (40Hz / 20)
	if (udb_heartbeat_counter % 20 == 0)
	{
		if (dcm_flags._.calib_finished)
		{
			send_debug_line();
		}
	}
}

// Prepare a line of serial output and start it sending
void send_debug_line(void)
{
	db_index = 0;
	if(RECORD_OFFSETS == 1)
	{
		int16_t gravity2x = (int16_t)2*GRAVITY;
		sprintf(debug_buffer, "%i, %i, %i, %i, %i, %i, %i\r\n", 
		    gravity2x, 
		    udb_xaccel.value, udb_yaccel.value, udb_zaccel.value, 
		    udb_xrate.value,  udb_yrate.value,  udb_zrate.value);
	}
	else
	{
		sprintf(debug_buffer, "lat: %li, long: %li, alt: %li\r\nrmat: %i, %i, %i, %i, %i, %i, %i, %i, %i\r\n", 
		    lat_gps.WW, lon_gps.WW, alt_sl_gps.WW, 
		    rmat[0], rmat[1], rmat[2], 
		    rmat[3], rmat[4], rmat[5], 
		    rmat[6], rmat[7], rmat[8]);
	}
	udb_serial_start_sending_data();
}

// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.
int16_t udb_serial_callback_get_byte_to_send(void)
{
	uint8_t c = debug_buffer[db_index++];

	if (c == 0) return -1;
	return c;
}

// Don't respond to serial input
void udb_serial_callback_received_byte(uint8_t rxchar)
{
	// Do nothing
}

void udb_callback_radio_did_turn_off(void)
{
}

