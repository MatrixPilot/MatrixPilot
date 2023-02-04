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
#include "../libDCM/libDCM_defines.h"
#include "../libDCM/mathlibNAV.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/rmat.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/ADchannel.h"
#include <math.h>

// Used for serial debug output
#include <stdio.h>

extern boolean differential_gps(void) ;
extern boolean hasWrittenHeader ;
extern boolean start_log;
extern boolean logging_on;

boolean didCalibrate = 0 ;

void send_debug_line( int8_t differential_flag , uint16_t sats , int32_t lat , int32_t lon , int32_t alt , int16_t sog , int16_t  cog , int16_t climb ) ;
void send_imu_data(void);
void motorCntrl( void ) ;
void offsets_init(void) ;

const int max_tilt = 0 ;  // maximum tilt in byte cicular
int commanded_tilt_gain ;

int main (void)
{
//	offsets_init();
	// Set up the libraries
	if ( GPS_TYPE != GPS_NONE)
	{
		gps_init() ;
	}
	offsets_init() ;
	mcu_init() ;
	udb_init() ;
	dcm_init() ;
	
#ifdef USE_PACKETIZED_TELEMERTY
	udb_serial_set_rate(460800) ;
    start_log = 0;
    hasWrittenHeader = 1;
    logging_on = 0 ;
#else
	udb_serial_set_rate(57600) ;
#endif
    
	LED_GREEN = LED_OFF ;
	LED_RED = LED_OFF ;

#if (CONSOLE_UART != 2)
	udb_init_USART(&udb_serial_callback_get_byte_to_send, &udb_serial_callback_received_byte);
#endif
	commanded_tilt_gain = sine ( max_tilt ) / 1000 ;
	
#ifdef ENABLE_ESP32
    // Pull ESP32 Enable pin high
    _TRISD15 = 0;
    _LATD15 = 1;
#endif
    
	// Start it up!
	while(1)
	{
		udb_run() ; 
	}
	
	return 0 ;
}

void align_rmat_to_mag(void);
// Called every 1/2 second at high priority
void udb_background_callback_periodic(void)
{
	int gplane[3];
	if (!didCalibrate)
	{
		if (udb_flags._.radio_on && dcm_flags._.calib_finished)
		{
			dcm_calibrate() ;	
			// record vertical
			gplane[0] = XACCEL_VALUE;
			gplane[1] = YACCEL_VALUE;
			gplane[2] = ZACCEL_VALUE;
			vector3_normalize(&rmat[6],gplane);
			didCalibrate = 1 ;	
		}
	}
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

extern boolean origin_recorded ;
extern uint16_t number_pulses ;
extern uint16_t altitude ;

#define DEG_PER_RAD 57.296
float tilt_angle ;
boolean start_log = 1 , stop_log = 0 , slide_in_progress = 0 ;
uint16_t stop_count = 0 ;
void update_slide_detection(void)
{
	int16_t tilt_angle_int ;
	tilt_angle = DEG_PER_RAD*atan2f(sqrtf(((float)aero_force_filtered[0]._.W1)*((float)aero_force_filtered[0]._.W1)+((float)aero_force_filtered[1]._.W1)*((float)aero_force_filtered[1]._.W1)),-(float)aero_force_filtered[2]._.W1);
	tilt_angle_int = (int16_t)tilt_angle ;
	if ( slide_in_progress == 1)
		{
		if ( tilt_angle_int > TILT_STOP )
			if ( stop_count == SLIDE_DET_HZ*TILT_STOP_DELAY)
			{
				stop_log = 1 ;
				slide_in_progress = 0 ;
				LED_RED = LED_OFF ;
				udb_led_toggle(LED_GREEN);
			}
			else
			{
				stop_count ++ ;
				LED_RED = LED_ON ;
				LED_GREEN = LED_OFF ;
			}
		else
			{
				stop_count = 0 ;
				udb_led_toggle(LED_RED);
				LED_GREEN = LED_OFF ;
			}
		}
	else
		{
		if ( tilt_angle_int < TILT_START )
			{
				stop_count = 0 ;
				start_log = 1 ;
				slide_in_progress = 1 ;
				udb_led_toggle(LED_RED);
				LED_GREEN = LED_OFF ;
			}
		else
			{
				LED_RED = LED_OFF ;
				udb_led_toggle(LED_GREEN);
			}
		}
}

// Called at heartbeat Hz, before sending servo pulses
extern boolean log_residuals ;
extern void send_residual_data(void) ;
boolean stop_residuals = 1 ;
boolean start_residuals = 0 ;
extern float yaw_previous , yaw_angle , heading_previous ;
extern void compute_euler(void);
uint16_t residual_log_counter = 0 ;
void dcm_heartbeat_callback(void)
{
	if ( didCalibrate )
	{
#ifndef BUILD_OFFSET_TABLE
		update_slide_detection();
#endif // BUILD_OFFSET_TABLE
		if (!hasWrittenHeader)
		{
			if ((udb_heartbeat_counter % (HEARTBEAT_HZ/HEADER_HZ)) == 0)
			{
				send_imu_data();
			}
		}
		else
		{
			if ((udb_heartbeat_counter % (HEARTBEAT_HZ/SLIDE_DET_HZ)) == 0)
			{
#ifndef BUILD_OFFSET_TABLE
				if (start_log ==1) 
				{
					compute_euler();
					yaw_previous = yaw_angle ;
					heading_previous = 0.0 ;
					log_residuals = 0 ;
				}
				if (stop_log ==1 ) 
				{ 
					start_residuals = 1 ;
					log_residuals = 1 ;
				}
#endif
			}
			
			{
				if ((udb_heartbeat_counter % (HEARTBEAT_HZ/LOGGER_HZ)) == 0)
				{
					send_imu_data();
				}
			}
		}
	//
#ifdef LOG_RESIDUALS
		if (log_residuals == 1)
		{
			if ((udb_heartbeat_counter % HEARTBEAT_HZ )== 0) residual_log_counter ++ ;
			if ( residual_log_counter == 10 ) // 6 times per minute
			{
				residual_log_counter = 0 ;
				send_residual_data();
			}
		}
#endif // LOG_RESIDUALS
	}
	return ;
}


void udb_callback_radio_did_turn_off( void ) {}
