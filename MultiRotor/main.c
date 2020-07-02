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
#include "../libDCM/mathlibNAV.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/rmat.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/ADchannel.h"

// Used for serial debug output
#include <stdio.h>

extern boolean differential_gps(void) ;

boolean didCalibrate = 0 ;

void send_debug_line( int8_t differential_flag , uint16_t sats , int32_t lat , int32_t lon , int32_t alt , int16_t sog , int16_t  cog , int16_t climb ) ;
void motorCntrl( void ) ;

const int max_tilt = (int) (MAX_TILT*.7111) ;  // maximum tilt in byte cicular
int commanded_tilt_gain ;

int main (void)
{
	// Set up the libraries
	if ( GPS_TYPE != GPS_NONE)
	{
		gps_init() ;
	}
	mcu_init() ;
	udb_init() ;
	dcm_init() ;
	
	udb_serial_set_rate(19200) ;
	
	LED_GREEN = LED_OFF ;
#if (CONSOLE_UART != 2)
	udb_init_USART(&udb_serial_callback_get_byte_to_send, &udb_serial_callback_received_byte);
#endif

	commanded_tilt_gain = sine ( max_tilt ) / 1000 ;
	
	// Start it up!
	while(1)
	{
		udb_run() ; 
	}
	
	return 0 ;
}

// Called every 1/2 second at high priority
void udb_background_callback_periodic(void)
{
	int gplane[3];
	if (!didCalibrate)
	{
		// If still calibrating, blink RED
		udb_led_toggle(LED_RED) ;	
		if (udb_flags._.radio_on && dcm_flags._.calib_finished)
		{
			udb_servo_record_trims() ;
			dcm_calibrate() ;	
			// record vertical
			gplane[0] = XACCEL_VALUE;
			gplane[1] = YACCEL_VALUE;
			gplane[2] = ZACCEL_VALUE;
			vector3_normalize(&rmat[6],gplane);
			didCalibrate = 1 ;	
		}
	}
	else
	{
		// No longer calibrating, indicate command status
		if ( dcm_flags._.fpv_tilt_req == 1) 
		{
			LED_RED = LED_OFF ;
		}
		else
		{
			if (dcm_flags._.earth_frame_tilt_req == 1)
			{
				LED_RED = LED_ON ;
			}
			else
			{
				if ( dcm_flags._.position_hold_req == 1)
				{
					udb_led_toggle(LED_RED) ; 
				}
				else // just in case PWM6 gets disconnected
				{
					LED_RED = LED_OFF ;
				}			
			}
		}
	}
	
	return ;
}

void udb_heartbeat_40hz_callback(void)
{
	static int count = 0;
	if (++count > 20)
	{
		count = 0;
		udb_background_callback_periodic();	
	}
}

// Called every time we get gps data (1, 2, or 4 Hz, depending on GPS config)
void dcm_callback_gps_location_updated(void)
{
	return ;
}



// Called at heartbeat Hz, before sending servo pulses
void dcm_heartbeat_callback(void)
{
	if((udb_heartbeat_counter%(HEARTBEAT_HZ/PID_HZ))==0)
	{
#ifdef DEBUG_MAGNETOMETER
		int motor_A, motor_B, motor_C, motor_D ;
		union longww long_accum ;
		long_accum.WW = __builtin_mulss(rmat[1], 4000);
		motor_A=(3000 + long_accum._.W1);	
		long_accum.WW = __builtin_mulss(rmat[4], 4000);
		motor_B=(3000 + long_accum._.W1);	
		long_accum.WW = __builtin_mulss(rmat[6], 4000);
		motor_C=(3000 + long_accum._.W1);
		long_accum.WW = __builtin_mulss(rmat[7], 4000);
		motor_D = (3000 + long_accum._.W1) ;
		udb_pwOut[1] = udb_servo_pulsesat( motor_A ) ;		
		udb_pwOut[2] = udb_servo_pulsesat( motor_B ) ;
		udb_pwOut[3] = udb_servo_pulsesat( motor_C ) ;
		udb_pwOut[4] = udb_servo_pulsesat( motor_D ) ;
#else
		motorCntrl() ;
#endif // debug magnetometer
	}
	
	// Update the Green LED to show RC radio status
	if (udb_flags._.radio_on)
	{
		//LED_GREEN = LED_ON ;
	}
	else
	{
		//LED_GREEN = LED_OFF ;
	}
	
	// Serial output one fifth of SERVO_HZ  
	// if ((udb_heartbeat_counter % (5*HEARTBEAT_HZ/SERVO_HZ)) == 0)
	// Serial output one tenth of SERVO_HZ  (5 Hz)
	if ((udb_heartbeat_counter % (10*HEARTBEAT_HZ/SERVO_HZ)) == 0)

	{
		if ( didCalibrate )
		{
			send_debug_line(differential_gps() , svs , lat_gps.WW , lon_gps.WW , alt_sl_gps.WW , sog_gps.BB , cog_gps.BB , climb_gps.BB ) ;
		}
	}
	
	return ;
}


void udb_callback_radio_did_turn_off( void ) {}
