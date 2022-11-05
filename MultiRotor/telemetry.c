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
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/rmat.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/ADchannel.h"
#include "../libUDB/mcu.h"

// Used for serial debug output
#include <stdio.h>
#if ( GYRO_RANGE == 250 )
#define GYRO_FACTOR ((double)(65.5))
#elif ( GYRO_RANGE == 500 )
#define GYRO_FACTOR ((double)(32.75))
#elif ( GYRO_RANGE == 1000 )
#define GYRO_FACTOR ((double)(16.375))
#elif ( GYRO_RANGE == 2000 )
#define GYRO_FACTOR ((double)(8.1875))
#else
#error "invalid GYRO_RANGE"
#endif // GYRO_RANGE 	
		
#if ( ACCEL_RANGE == 2 )
#define ACCEL_FACTOR ((double)(8192.0/32.17))
#elif ( ACCEL_RANGE == 4 )
#define ACCEL_FACTOR ((double)(4096.0/32.17))
#elif ( ACCEL_RANGE == 8 )
#define ACCEL_FACTOR ((double)(2048.0/32.17))
#elif ( ACCEL_RANGE == 16 )
#define ACCEL_FACTOR ((double)(1024.0/32.17))
#else
#error "invalid ACCEL_RANGE"
#endif // ACCEL_RANGE 	


char debug_buffer[1024] ;
int db_index = 0 ;
boolean hasWrittenHeader = 0 ;
int header_line = 0 ;

extern int16_t theta[3] , roll_control , pitch_control , yaw_control , accelEarth[3] , accel_feedback ;
extern int16_t commanded_roll, commanded_pitch, commanded_yaw, pwManual[] ;
extern int16_t roll_error , pitch_error , yaw_error ;
extern union longww roll_error_integral, pitch_error_integral , yaw_error_integral ;
extern int16_t target_rmat[9] ;
extern int16_t altitude , altitude_control , climb_rate ;
extern int16_t number_pulses ;
extern int16_t IMU_climb , IMU_altitude ;
extern int16_t target_rate[3] ;
extern union longww IMUlocationx , IMUlocationy , IMUvelocityx , IMUvelocityy ;
extern int16_t x_velocity_feedback , y_velocity_feedback ;
extern int16_t x_velocity_target , y_velocity_target ;
extern int16_t udb_magFieldBody[3] ;
extern fractional magFieldEarth[3];
extern fractional magAlignment[4];
extern int16_t udb_magOffset[3] , errorYawplane[3] , magGain[3] ;
extern uint16_t mission_time ;

/*void send_debug_line( void )
{
	
}*/
extern int16_t gplane[];
extern int16_t aero_force[];
extern void serial_output(const char* format, ...);
// Prepare a line of serial output and start it sending
// GPS data needs to be passed in
extern int16_t yaw_rmat[];
extern int16_t target_altitude , target_climb_rate ;
extern union longww throttle_accum ;
extern void initialize_yaw_rmat(void) ;
uint16_t record_number = 0 ;
#define DIGITS_5 -12805
void send_imu_data(void)
{
	db_index = 0 ;
	
	if (!hasWrittenHeader)
	{
		header_line ++ ;
		switch ( header_line ) {
			case 1:
			initialize_yaw_rmat();
			break ;
		case 2:
			{
				serial_output("WOLF-PAC SN%i%i%i IMU ", SERIAL_NUMBERD1 , SERIAL_NUMBERD2 , SERIAL_NUMBERD3 ) ;
			}
			break ;
		
		case 4:
			{
				serial_output("in north-east-down body frame.\r\n") ;
			}
			break ;	
		case 6:
			{
				serial_output("specific forces in feet/sec^2.\r\n") ;
			}
			break ;	
		case 8:
			{
				serial_output("CCW rotation rates in deg/sec.\r\n");
			}
			break ;	
		case 10:
			{
				serial_output( "Accelerometer range = %i times gravity\r\n" , ACCEL_RANGE ) ;
			}
			break ;
		case 12:
			{
				serial_output( "Gyro range = %i degrees per second\r\n" , GYRO_RANGE ) ;
			}
			break ;
		case 14:
			{
				serial_output( "mems binary offsets = %i,%i,%i,%i,%i,%i\r\n",
					XACCEL_OFFSET ,
					YACCEL_OFFSET , 
					ZACCEL_OFFSET ,
					XRATE_OFFSET ,
					YRATE_OFFSET ,
					ZRATE_OFFSET );	
			}
			break;
		case 16:
			{
#ifdef LOG_IMU
#ifdef FULL_RECORD
				serial_output( "x_force , y_force , z_force , x_gyro , y_gyro , z_gyro\r\n" ) ;
#endif // FULL_RECORD
#endif // LOG_IMU
			}
			break ;	
		case 18:
			hasWrittenHeader = 1 ;
			break ;
		default:
			break ;
		}
	}
	else
	{
#ifdef RECORD_OFFSETS 
		{	
			serial_output( "%i,%i,%i,%i,%i\r\n", record_number++ , udb_cpu_load(),
					DIGITS_5 , DIGITS_5 , DIGITS_5  ) ;
//			serial_output( "%i,%i,%i,%i,%i,%i\r\n" ,
//			udb_xaccel.value , udb_yaccel.value , udb_zaccel.value ,
//			udb_xrate.value , udb_yrate.value , udb_zrate.value ) ;
		}
 
		
#else //  not RECORD_OFFSETS	
		
		{
#ifdef LOG_IMU
#ifdef FULL_RECORD

			serial_output( "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\r\n" ,
//			udb_heartbeat_counter , (int) udb_cpu_load() ,
				((double)(aero_force[0]))/ACCEL_FACTOR ,
				((double)(aero_force[1]))/ACCEL_FACTOR ,
				((double)(aero_force[2]))/ACCEL_FACTOR ,
				((double)(omegagyro[0]))/GYRO_FACTOR ,
				((double)(omegagyro[1]))/GYRO_FACTOR , 
				((double)(omegagyro[2]))/GYRO_FACTOR 
			) ;	
						
#endif // FULL_RECORD
#endif // LOG_IMU
		}
#endif // RECORD_OFFSETS
	}
	return ;
}
/*
extern int gplane[] ;
// Prepare a line of serial output and start it sending
void send_debug_line( void )
{
	db_index = 0 ;
	
	if (!hasWrittenHeader)
	{
		sprintf(debug_buffer, "w0 , w1 , w2 , a0 , a1 , a2\r\n") ;
		hasWrittenHeader = 1 ;
	}
	else
	{
		sprintf(debug_buffer, "%i , %i , %i , %i , %i , %i\r\n" ,
		omegagyro[0] , 	omegagyro[1] , omegagyro[2] ,
		gplane[0] , gplane[1] , gplane[2]
		 ) ;
	}
	
	udb_serial_start_sending_data() ;
	
	return ;
}
*/
int16_t sb_index = 0 ;
extern int16_t end_index ;
extern char serial_buffer[] ;
int16_t udb_serial_callback_get_byte_to_send(void)
{
	uint8_t txchar = serial_buffer[ sb_index++ ];

	if (txchar)
	{
		return txchar;
	}
	else
	{
		sb_index = 0;
		end_index = 0;
	}
	return -1;
}

/*
// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.
int udb_serial_callback_get_byte_to_send(void)
{
	unsigned char c = debug_buffer[ db_index++ ] ;
	
	if (c == 0) return -1 ;
	
	return c ;
}
*/

// Don't respond to serial input
void udb_serial_callback_received_byte(uint8_t rxchar)
{
	// Do nothing
	return ;
}


