// This file is part of the MatrixPilot LedTest demo.
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


#include "../../libUDB/libUDB.h"


#define RATE_THRESHOLD_LED		120
#define ACCEL_THRESHOLD_LED		40


int calib_countdown = 10 ;
int x_rate, y_rate, z_rate ;
int x_accel, y_accel, z_accel ;
boolean eepromSuccess = 0 ;


extern void IOTest(void);


int main(void)
{
	// IOTest should only be run if the board is mounted in the SparkFun UDB4 Test Rig
	// IOTest() ;
	
	udb_init() ;
	
	udb_run() ;  // This never returns.
	return 0 ;
}



// Called every 1/2 second at low priority
void udb_background_callback_periodic(void)
{
	switch (calib_countdown) {
		case 9:
			udb_background_trigger() ;
			LED_RED = LED_ON ;
			break ;
		case 8:
			LED_RED = LED_OFF ;
			LED_GREEN = LED_ON ;
			break ;
		case 7:
			LED_GREEN = LED_OFF ;
			LED_BLUE = LED_ON ;
			break ;
		case 6:
			LED_BLUE = LED_OFF ;
			LED_ORANGE = LED_ON ;
			break ;
		case 5:
			LED_ORANGE = LED_OFF ;
			LED_RED = LED_ON ;
			break ;
		case 4:
			LED_RED = LED_OFF ;
			LED_GREEN = LED_ON ;
			break ;
		case 3:
			LED_GREEN = LED_OFF ;
			LED_BLUE = LED_ON ;
			break ;
		case 2:
			LED_BLUE = LED_OFF ;
			LED_ORANGE = LED_ON ;
			break ;
		case 1:
			LED_ORANGE = LED_OFF ;
			udb_a2d_record_offsets() ;
			break ;
		case 0:  // after 5 sec
			return ;
	}
	
	calib_countdown-- ;
	return ;
}


void udb_background_callback_triggered(void)
{
	// Write 1,2,3,4 into the first 4 bytes of the EEPROM
	unsigned char data[4] = {1,2,3,4} ;
	eeprom_PageWrite(0x0000, data, 4) ;
	
	// Read the first 4 bytes of the EEPROM
	data[0] = data[1] = data[2] = data[3] = 0 ;
	eeprom_SequentialRead(0x0000, data, 4) ;
	
	// Then test them
	if (data[0] == 1 && data[1] == 2 && data[2] == 3 && data[3] == 4) {
		eepromSuccess = 1 ;
	}
}


// Called at 40 Hz, before reseting the sensor sampling
void udb_callback_read_sensors(void)
{
	x_rate = XRATE_VALUE ;
	y_rate = YRATE_VALUE ;
	z_rate = ZRATE_VALUE ;
	
	x_accel = XACCEL_VALUE ;
	y_accel = YACCEL_VALUE ;
	z_accel = ZACCEL_VALUE ;
	
	return ;
}


// Called at 40 Hz, before sending servo pulses
void udb_servo_callback_prepare_outputs(void)
{
	if (calib_countdown)
	{
		udb_pwOut[ROLL_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[PITCH_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[YAW_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[X_ACCEL_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[Y_ACCEL_OUTPUT_CHANNEL] = 3000 ;
		udb_pwOut[Z_ACCEL_OUTPUT_CHANNEL] = 3000 ;
	}
	else if (eepromSuccess == 0) {
		// eeprom failure!
		if (udb_heartbeat_counter % 6 == 0) {
			udb_led_toggle(LED_RED) ;
			udb_led_toggle(LED_GREEN) ;
			udb_led_toggle(LED_BLUE) ;
			udb_led_toggle(LED_ORANGE) ;
		}
	}
	else
	{
		union longww accum ;
		
		accum.WW = __builtin_mulss( y_rate , 4000 ) ;
		udb_pwOut[ROLL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1) ;
		
		accum.WW = __builtin_mulss( x_rate , 4000 ) ;
		udb_pwOut[PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1) ;
		
		accum.WW = __builtin_mulss( z_rate , 4000 ) ;
		udb_pwOut[YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1) ;
		
		accum.WW = __builtin_mulss( x_accel , 4000 ) ;
		udb_pwOut[X_ACCEL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1) ;
		
		accum.WW = __builtin_mulss( y_accel , 4000 ) ;
		udb_pwOut[Y_ACCEL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1) ;
		
		accum.WW = __builtin_mulss( z_accel , 4000 ) ;
		udb_pwOut[Z_ACCEL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1) ;
		
		if ( (udb_heartbeat_counter / 600) % 2 == 0) {
			LED_ORANGE = LED_ON ;
			LED_RED = ((abs(udb_pwOut[ROLL_OUTPUT_CHANNEL]-3000) > RATE_THRESHOLD_LED) ? LED_ON : LED_OFF) ;
			LED_GREEN = ((abs(udb_pwOut[PITCH_OUTPUT_CHANNEL]-3000) > RATE_THRESHOLD_LED) ? LED_ON : LED_OFF) ;
			LED_BLUE = ((abs(udb_pwOut[YAW_OUTPUT_CHANNEL]-3000) > RATE_THRESHOLD_LED) ? LED_ON : LED_OFF) ;
		}
		else {
			LED_ORANGE = LED_OFF ;
			LED_RED = ((abs(udb_pwOut[X_ACCEL_OUTPUT_CHANNEL]-3000) > ACCEL_THRESHOLD_LED) ? LED_ON : LED_OFF) ;
			LED_GREEN = ((abs(udb_pwOut[Y_ACCEL_OUTPUT_CHANNEL]-3000) > ACCEL_THRESHOLD_LED) ? LED_ON : LED_OFF) ;
			LED_BLUE = ((abs(udb_pwOut[Z_ACCEL_OUTPUT_CHANNEL]-3000) > ACCEL_THRESHOLD_LED) ? LED_ON : LED_OFF) ;
		}
	}
	
	return ;
}


int udb_gps_callback_get_byte_to_send(void) { return -1 ; }
void udb_gps_callback_received_byte(char rxchar) {}

int udb_serial_callback_get_byte_to_send(void) { return -1 ; }
void udb_serial_callback_received_byte(char rxchar) {}
