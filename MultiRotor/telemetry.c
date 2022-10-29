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
				serial_output("3 axis IMU in north-east-down body frame.\r\n") ;
			}
			break ;
		case 3:
			{
				serial_output("specific forces in feet/sec^2.\r\n") ;
			}
			break ;	
		case 4:
			{
				serial_output("CCW rotation rates in deg/sec.\r\n");
			}
			break ;	
		case 5:
			{
				serial_output( "Accelerometer range = %i times gravity\r\n" , ACCEL_RANGE ) ;
			}
			break ;
		case 6:
			{
				serial_output( "Gyro range = %i degrees per second\r\n" , GYRO_RANGE ) ;
			}
			break ;
		case 7:
			{
				serial_output( "mems binary offsets = %i,%i,%i,%i,%i,%i\r\n",
					XACCEL_OFFSET ,
					YACCEL_OFFSET , 
					ZACCEL_OFFSET ,
					XRATE_OFFSET ,
					YRATE_OFFSET ,
					ZRATE_OFFSET );	
			}
			break ;			
		case 8:
			break ;
		case 9:
			break ;
		case 10:
			break ;			
		case 11:
			break ;				
		case 12:
			{
				serial_output( "x_force , y_force , z_force , x_gyro , y_gyro , z_gyro\r\n" ) ;
			}
			break ;	
		case 13:
			break ;
		case 14:
			hasWrittenHeader = 1 ;
			break ;
		default:
			hasWrittenHeader = 1 ;
			break ;
		}
	}
	else
	{
#if ( RECORD_OFFSETS == 1)
		{	
			serial_output( "%i,%i,%i,%i,%i,%i\r\n" ,
			udb_xaccel.value , udb_yaccel.value , udb_zaccel.value ,
			udb_xrate.value , udb_yrate.value , udb_zrate.value ) ;
		}
 
		
#else //  not RECORD_OFFSETS	
		
		{
			serial_output( "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n" ,
//			udb_heartbeat_counter , (int) udb_cpu_load() ,
				((double)(aero_force[1]))/ACCEL_FACTOR ,
				((double)(- aero_force[0]))/ACCEL_FACTOR ,
				((double)(aero_force[2]))/ACCEL_FACTOR ,
				((double)(omegagyro[1]))/GYRO_FACTOR ,
				((double)(- omegagyro[0]))/GYRO_FACTOR , 
				((double)(omegagyro[2]))/GYRO_FACTOR 
			) ;	
		}
#endif // RECORD_OFFSETS
	}
	return ;
}
void send_debug_line( int8_t differential_flag , uint16_t sats , int32_t lat , int32_t lon , int32_t alt , int16_t sog , int16_t  cog , int16_t climb )
{
	db_index = 0 ;
	
	if (!hasWrittenHeader)
	{
		header_line ++ ;
		switch ( header_line ) {
			case 1:
			initialize_yaw_rmat();
			serial_output("weight=%i , max_clmb_rate=%i, max_alt_err=%i\r\n",
					WEIGHT,
					MAX_CLIMB_RATE,
					MAX_ALT_ERROR) ;
			break ;
		case 2:
			serial_output( "RCON=0x%X , TRAP_FLAGS=0x%X , TRAP_SOURCE=0x%lX , ALARMS=%i\r\n",
			    get_reset_flags(), trap_flags, trap_source, osc_fail_count);
			break ;
		case 3:
			serial_output( "MAX_TILT = %i\r\n" , MAX_TILT ) ;
			break ;	
		case 4:
			serial_output( "TILT_KP = %5f, YAW_KP = %5f\r\n" ,
				TILT_KP ,
				YAW_KP  ) ;
			break ;	
		case 5:
			serial_output( "TILT_KI = %5f, YAW_KI = %5f\r\n" ,
				TILT_KI ,
				YAW_KI  ) ;
			break ;
		case 6:
			serial_output( "TILT_KD = %5f, YAW_KD = %5f\r\nLAT_RATE_GAIN = %i, MAX_V = %i, MAX_D = %i\r\n" ,
				TILT_KD ,
				YAW_KD ,
				LATERAL_RATE_GAIN , MAX_SPEED , MAX_DISTANCE ) ;
			break ;
		case 7:
	/*		serial_output( "TILT_FF = %5f, TILT_KDD = %5f, ACCEL_K = %5f\r\n" ,
					TILT_FF ,
					TILT_KDD ,
				ACCEL_K ) ; */
			serial_output( "min_thrust=%i, max_thrust=%i\r\ncmd_step_rate=%i, min_throt_cmd=%i, throt_cutout=%i\r\n", 
					MIN_THRUST, MAX_THRUST , COMMAND_STEP_RATE_MULTIPLIER ,
					MIN_THROTTLE_COMMAND , THROTTLE_CUTOUT ) ;
			break ;			
		case 8:
			serial_output( ROTOR_CRAFT );
			break ;
		case 9:
			serial_output( "ESC_HZ = %i , PID_HZ = %i , SERVO_HZ = %i , DR_TAU = %5f\r\n" , ESC_HZ , PID_HZ , SERVO_HZ , DR_TAU );
			break ;
		case 10:
			serial_output( "USE_LIDAR = %i , MAX_ALT_RATE_CONTROL = %i , MAX_ALT_PROP_CONTROL = %i\r\n" ,
				USE_LIDAR ,
				MAX_ALT_RATE_CONTROL ,
				MAX_ALT_PROP_CONTROL ) ;
			break ;			
		case 11:
			serial_output( "MIN_LIDAR_PULSE_THRESH = %i, IMU_CLIMB_RATE_DIVISOR = %i\r\n" ,
				MIN_LIDAR_PULSE_THRESH ,
				IMU_CLIMB_RATE_DIVISOR ) ;
			break ;				
		case 12:
			serial_output( "IMU_ALT_DIVISOR = %i, TARGET_MIN_ALTITUDE = %i\r\n" ,
				IMU_ALT_DIVISOR ,
				TARGET_MIN_ALTITUDE ) ;
			break ;	
		case 13:
			serial_output( "RPY trims = %i , %i , %i\r\nAGoffsets = %i , %i , %i , %i , %i , %i\r\no_lat, o_lon, o_alt , %li , %li , %li\r\n " ,
					ROLL_TRIM , PITCH_TRIM , YAW_TRIM,
					udb_xaccel.offset , udb_yaccel.offset , udb_zaccel.offset ,
					udb_xrate.offset , udb_yrate.offset , udb_zrate.offset ,
					lat , lon , alt ) ;
			break ;
		case 14:
#if (DEBUG_MAG == 1)
			{
				serial_output( "rmat1 , rmat4 , yw_err_z , magBdyx , magBdyy , magBdyz , mErthx , mErthy , mErthz , offx , offy , offz , algn0 , algn1 , algn2 , algn3 , gain0 , gain1 , gain2\r\n") ;
			}
#else
			{
				serial_output( "gps_alt , thrust_cmd , roll_cmd , pitch_cmd , x_vel_target , y_vel_target , mode, dgps, sog , cog , svert, mx, my, mz, X , Y , VX , VY , targ_alt , target_climb , pulses, IMU_alt , alt , IMU_climb , clmb_r , alt_cntrl , hrtbt , cpu , mtra , mtrb , mtrc ,mtrd , r6 , r7 , w0 , w1 , w2 , rfb , pfb , yfb , rerr, perr, yerr, tkoff_flg, thrtle_cmd, m_time\r\n" ) ;
			
			}
#endif // DEBUG_MAG
			hasWrittenHeader = 1 ;
			break ;
		default:
			hasWrittenHeader = 1 ;
			break ;
		}
	}
	else
	{
#if (DEBUG_MAG == 1)
		{
			serial_output( "%i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i\r\n" ,
			rmat[1] , rmat[4] , errorYawplane[2] , 
			udb_magFieldBody[0] , udb_magFieldBody[1] , udb_magFieldBody[2] ,
			magFieldEarth[0] , magFieldEarth[1] , magFieldEarth[2] , 
			udb_magOffset[0] , udb_magOffset[1] , udb_magOffset[2] ,
			magAlignment[0] , magAlignment[1] , magAlignment[2] , magAlignment[3] ,
			magGain[0] , magGain[1] , magGain[2]
			);	
		}
#else
		{
			serial_output( "%li,%i,%i,%i,%i,%i,%i,%i,%u,%u,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%u,%i,%u,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n" ,
			alt ,
			pwManual[THROTTLE_INPUT_CHANNEL] , commanded_roll , commanded_pitch ,
			x_velocity_target , y_velocity_target ,
			udb_pwIn[6] ,
			differential_flag ,
			//sats , lat , lon , alt , 
			sog , cog , climb ,
			udb_magFieldBody[0] , udb_magFieldBody[1] , udb_magFieldBody[2] ,
			IMUlocationx._.W1 , IMUlocationy._.W1 , IMUvelocityx._.W1 , IMUvelocityy._.W1 ,
//			target_rate[0] , target_rate[1] , target_rate[2] , 
			target_altitude , target_climb_rate ,
			number_pulses , IMU_altitude , altitude , IMU_climb  , climb_rate , altitude_control , 
			udb_heartbeat_counter , (int) udb_cpu_load() ,
			udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] ,		
			udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] ,
			udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] ,
			udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] ,
			rmat[6] , rmat[7] , 
			omegagyro[0] , omegagyro[1] , omegagyro[2] , 
			roll_control , pitch_control, yaw_control ,
			roll_error , pitch_error , yaw_error ,
			udb_pwIn[7]	, THROTTLE_COMMAND , mission_time	) ;	
		}
#endif // DEBUG_MAG
	
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


