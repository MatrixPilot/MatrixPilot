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
extern void compute_euler(void);
extern void compute_bill_angles(void);
extern void update_offset_table(void);
extern float roll_angle , pitch_angle , yaw_angle ;
extern float bill_angle_x , bill_angle_y , bill_angle_z ;
extern int16_t omegacorrI[];
extern uint16_t omega_magnitude ;
extern union longww omegagyro_filtered[];
extern struct ADchannel mpu_temp;
extern uint16_t samples ;
extern uint64_t samples_64t ;
extern uint32_t samples_32t ;
extern int16_t adjusted_temperature ;
extern int16_t gyro_offset_entry[] ;
extern uint64_t xx_sum ;
extern int64_t xy_sum[] ;
extern int32_t x_sum ;
extern int32_t y_sum[] ;
extern uint32_t xx_bar ;
extern int32_t xy_bar[] ;
extern int16_t x_bar ;
extern int16_t y_bar[] ;

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
extern float tilt_angle ;
extern boolean start_log , stop_log ;
boolean logging_on = 0 ;
extern int16_t omega_dot_rmat6 ;
extern int16_t omega_scaled[];
extern int16_t omega_yaw_drift[];
extern int16_t gravity_estimate[];
extern int16_t acceleration[];
boolean gyro_locking_on = 1;
extern int16_t accelOn ;
extern int16_t errorYawplane[];
extern int16_t errorRP[];
extern union longww gyroCorrectionIntegral[];
extern float aero_force_float[];
extern float gravity_float[];
extern float acceleration_float[];
extern float omegaAccum_float[];
extern float velocity ;
extern uint16_t index_msb , index_lsb ;
void send_imu_data(void)
{
#ifndef ALWAYS_LOG
	if (start_log == 1)
	{
		hasWrittenHeader = 0 ;
		header_line = 0 ;
		start_log = 0 ;
		logging_on = 1 ;
#ifdef		ALWAYS_SYNC_GYROS
		gyro_locking_on = 1 ;
#else
		gyro_locking_on = 0 ;
#endif // ALWAYS_SYNC_GYROS	
	}
	if ( stop_log == 1)
	{
		stop_log = 0 ;
		logging_on = 0 ;
		gyro_locking_on = 1 ;
	}
	if (logging_on == 0 ) return ;
#else
	logging_on = 1 ;
	if (start_log == 1)
	{
		start_log = 0 ;
#ifdef		ALWAYS_SYNC_GYROS
		gyro_locking_on = 1 ;
#else
		gyro_locking_on = 0 ;
#endif // ALWAYS_SYNC_GYROS	
	}
	if ( stop_log == 1)
	{
		stop_log = 0 ;
		gyro_locking_on = 1 ;
	}
#endif // ALWAYS_LOG
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
#ifndef NAME
				serial_output("WOLF-PAC SN%i%i%i IMU ", SERIAL_NUMBERD1 , SERIAL_NUMBERD2 , SERIAL_NUMBERD3 ) ;
#else
				serial_output("WOLF-PAC SN%i%i%i IMU, %s, ", SERIAL_NUMBERD1 , SERIAL_NUMBERD2 , SERIAL_NUMBERD3, NAME ) ;
#endif
			}
			break ;
		case 3:
			{
				serial_output(DATE);
			}
			break;
		case 4:
			{
				serial_output("in north-east-down body frame.\r\n") ;
			}
			break ;	
		case 5:
			{
#ifdef LOG_RATE
				serial_output("gyro rate version\r\n");
#endif // LOG_RATE
				
#ifdef LOG_EULER
				serial_output("euler angle version\r\n");
#endif // LOG_EULER
#ifdef LOG_RATE_AND_EULER
			serial_output("gyro rates and euler angles version\r\n");	
#endif // LOG_RATE_AND_EULER
#ifdef LOG_PITCH_AND_TWO_FORCES
			serial_output("x force, z force and pitch angle\r\n");
#endif // LOG_PITCH_AND_TWO_FORCES
			}
			break ;
		case 6:
			{
				serial_output("specific forces in ft/s^2.\r\n") ;
			}
			break ;	
		case 7:
			{
				serial_output("CCW rotation rates in d/s.\r\n");
			}
			break ;	
		case 8:
			{
				serial_output( BOARD );
			}
		break ;
		case 9:
			{
				serial_output( "Accelerometer range = %i times gravity\r\n" , ACCEL_RANGE ) ;
			}
			break ;
		case 10:
			{
				serial_output( "Gyro range = %i degrees per second\r\n" , GYRO_RANGE ) ;
			}
			break ;
		case 12:
			{
				serial_output("Gyro calibrations, x, y, z = %6.4f,%6.4f,%6.4f\r\n", 
						CALIBRATIONX ,CALIBRATIONY,CALIBRATIONZ );		
			}
			break ;
		case 14:
			{
				serial_output("Initial x, y, z binary gyro offsets = %i,%i,%i\r\n", 
						XRATE_OFFSET , YRATE_OFFSET, ZRATE_OFFSET );		
			}
			break ;
		case 16:
			{
				serial_output("tilt start, stop angles = %i,%i degrees\r\n", TILT_START , TILT_STOP);
			}
			break ;
		case 18:
			{
				serial_output( "accel binary offsets = %i,%i,%i\r\n",
					XACCEL_OFFSET ,
					YACCEL_OFFSET , 
					ZACCEL_OFFSET 
					 );	
			}
			break;
		case 19:
			{
				serial_output( "accel binary calibrations = %i,%i,%i\r\n",
					CAL_GRAV_X ,
					CAL_GRAV_Y , 
					CAL_GRAV_Z 
					 );
			}
			break;
		case 20:
			{
				serial_output("data rate = %i records/s\r\n", LOGGER_HZ );
			}
			break;
		case 21:
			{
				serial_output(FILTERING);
			}
			break;
		case 22:
			{
#ifdef LOG_IMU
#ifdef LOG_RATE
				serial_output( "forward_force , lateral , vertical , roll_rate , pitch , yaw\r\n" ) ;
#endif // LOG_RATE
#ifdef LOG_EULER
				serial_output( "x_force , y , z , yaw_angle , pitch , roll\r\n" ) ;			
#endif // LOG_EULER	
#ifdef LOG_RATE_AND_EULER
				serial_output( "x_rate , y_rate , z_rate , yaw_angle , pitch , roll\r\n" ) ;		
#endif // LOG_RATE_AND_EULER
#ifdef LOG_PITCH_AND_TWO_FORCES
				serial_output( "x_force,z_force,pitch_angle\r\n" ) ;	
#endif // LOG_PITCH_AND_TWO_FORCES
#endif // LOG_IMU
				
#ifdef RECORD_OFFSETS
				serial_output("tmptur,ax,ay,az,gx_val,gy_val,gz_val,gyr_x,gyr_y,gyr_z\r\n");
#endif // RECORD_OFFSETS
				
#ifdef TEST_LOGGER_HZ
				serial_output("logger bandwidth test\r\n");
#endif // TEST_LOGGER_HZ
#ifdef GYRO_CALIB
				serial_output("X, Y, Z calib angles\r\n") ;
#endif // GYRO_CALIB
#ifdef LOG_VELOCITY
				serial_output( "gyro_sync,cpu,wx,wy,wz,fx,fy,fz,gx,gy,gz,ax,ay,az,vel\r\n");
#endif // LOG_VELOCITY
#ifdef TEST_GYRO_LOCK
				serial_output("o_dot_r6,errYx,errYy,errYz,errRPx,errRPy,errRPz,wx,wy,wz\r\n" );
#endif // TEST_GYRO_LOCK
#ifdef GYRO_DRIFT
//				serial_output("gx,gy,gyz,gxfilt,gyfilt,gzfilt,ax,ay,az,axfilt,ayfilt,azfilt\r\n");
				serial_output("tmptur,synch,cpu_load,yaw,pitch,roll,gx,gy,gz,gxlp,gylp,gzlp,errx,erry,errz\r\n");
#endif // GYRO_DRIFT
#ifdef GYRO_OFFSETS
				serial_output("tmptr,ind_msb,ind_lsb,xrv,yrv,zrv,xro,yro,zro,wx,wy,wz\r\n");
#endif //				
#ifdef ROAD_TEST
		serial_output("synch,gx,gy,gyz,ax,ay,az,r6,r7,r8\r\n");
#endif // ROAD_TEST
#ifdef BUILD_OFFSET_TABLE
		serial_output("cpu,samples,X_bar,Y_bar_x,Y_bar_y,Y_bar_z,XX_bar,XY_bar_x,XY_bar_y,XY_bar_z,denom,lft_o_x,lft_o_y,lft_o_z,rght_o_x,rght_o_y,rght_o_z,\r\n");
#endif //BUILD_OFFSET_TABLE
			}
			break ;	
		case 23:
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
			serial_output( "%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n" ,
			mpu_temp.value,
			udb_xaccel.value , udb_yaccel.value , udb_zaccel.value ,
			udb_xrate.value , udb_yrate.value , udb_zrate.value	,
			omegagyro[0],omegagyro[1],omegagyro[2]
			 ) ;
		}
#endif // RECORD_OFFSETS
		
#ifdef GYRO_OFFSETS
		{
			serial_output( "%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n" ,
				mpu_temp.value,
				index_msb , index_lsb ,
				udb_xrate.value , udb_yrate.value , udb_zrate.value	,
				udb_xrate.offset , udb_yrate.offset ,udb_zrate.offset ,
				omegagyro[0] , omegagyro[1], omegagyro[2]
			 ) ;
		}
#endif // 

#ifdef TEST_LOGGER_HZ
#ifdef PARTIAL_RECORD		
		{
			serial_output("22 chars test string\r\n") ;
		}
#endif // PARTIAL_RECORD
#ifdef FULL_RECORD
		{
			serial_output("43 chars test string 43 chars test string\r\n") ;
		}
#endif	
#endif // TEST_LOGGER_HZ

#ifdef LOG_IMU
#ifdef LOG_RATE
		{
			serial_output( "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\r\n" ,
				((double)(aero_force[0]))/ACCEL_FACTOR ,
				((double)(aero_force[1]))/ACCEL_FACTOR ,
				((double)(aero_force[2]))/ACCEL_FACTOR ,
				((double)(omegaAccum[0]))/GYRO_FACTOR ,
				((double)(omegaAccum[1]))/GYRO_FACTOR , 
				((double)(omegaAccum[2]))/GYRO_FACTOR ) ;	
		}
#endif // LOG_RATE
#ifdef LOG_EULER
		{
			compute_euler();
			serial_output( "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\r\n" ,
				((double)(aero_force[0]))/ACCEL_FACTOR ,
				((double)(aero_force[1]))/ACCEL_FACTOR ,
				((double)(aero_force[2]))/ACCEL_FACTOR ,
				yaw_angle ,  pitch_angle , roll_angle  ) ;	
		}
#endif // LOG_EULER
#ifdef LOG_RATE_AND_EULER
		{
			compute_euler();
			serial_output( "%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\r\n" ,
				((double)(omegaAccum[0]))/GYRO_FACTOR ,
				((double)(omegaAccum[1]))/GYRO_FACTOR , 
				((double)(omegaAccum[2]))/GYRO_FACTOR ,
				yaw_angle ,  pitch_angle , roll_angle ) ;	
		}
#endif // LOG_RATE_AND_EULER
#ifdef LOG_PITCH_AND_TWO_FORCES
		{
			compute_euler();
			serial_output( "%.1f,%.1f,%.1f\r\n" ,
				((double)(aero_force[0]))/ACCEL_FACTOR ,
				((double)(aero_force[2]))/ACCEL_FACTOR ,
				pitch_angle   ) ;	
		}	
#endif // LOG_PITCH_AND_TWO_FORCES
#endif // LOG_IMU
#ifdef GYRO_CALIB

		{	compute_bill_angles();
			serial_output("%6.1f,%6.1f,%6.1f\r\n", bill_angle_x , bill_angle_y , bill_angle_z);

		}	
#endif // GYRO_CALIB
#ifdef GYRO_DRIFT
/*		serial_output("%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
				omegagyro[0],
				omegagyro[1],
				omegagyro[2] ,
				omegagyro_filtered[0]._.W1 ,
				omegagyro_filtered[1]._.W1 ,
				omegagyro_filtered[2]._.W1 ,
				aero_force[0] ,
				aero_force[1] ,
				aero_force[2] ,
				aero_force_filtered[0]._.W1 ,
				aero_force_filtered[1]._.W1 ,
				aero_force_filtered[2]._.W1 
					);*/
		compute_euler();
		serial_output("%i,%i,%i,%.1f,%.1f,%.1f,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
				mpu_temp.value,
				accelOn ,
				udb_cpu_load(),
				yaw_angle , pitch_angle , roll_angle ,
				omegagyro[0],
				omegagyro[1],
				omegagyro[2],
				(int16_t)((omegagyro_filtered[0].WW)>>12) ,
				(int16_t)((omegagyro_filtered[1].WW)>>12) ,
				(int16_t)((omegagyro_filtered[2].WW)>>12) ,
				errorRP[0] ,
				errorRP[1] ,
				errorRP[2]
					);
		
#endif // GYRO_DRIFT
		
#ifdef BUILD_OFFSET_TABLE
		update_offset_table();
	
		
#endif //

#ifdef ROAD_TEST
		serial_output("%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
				gyro_locking_on ,
				omegaAccum[0],
				omegaAccum[1],
				omegaAccum[2] ,
				aero_force[0] ,
				aero_force[1] ,
				aero_force[2] ,
				rmat[6] ,
				rmat[7] ,
				rmat[8] 
					);
	
#endif // GYRO_DRIFT


#ifdef LOG_VELOCITY
		{
			serial_output("%i,%i,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n",
					accelOn , udb_cpu_load() ,
					omegaAccum_float[0] ,
					omegaAccum_float[1] , 
					omegaAccum_float[2] ,
					aero_force_float[0] , aero_force_float[1] , aero_force_float[2] ,
					gravity_float[0] , gravity_float[1] , gravity_float[2] ,
					acceleration_float[0] , acceleration_float[1] , acceleration_float[2] , velocity 
					);
		}
#endif // LOG_VELOCITY
		
#ifdef TEST_GYRO_LOCK
		{
			serial_output( "%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
					omega_dot_rmat6 ,
					errorYawplane[0] , errorYawplane[1] , errorYawplane[2] , 
					errorRP[0] , errorRP[1] , errorRP[2] , 
					omegaAccum[0] , omegaAccum[1] , omegaAccum[2] 
					) ;
		}
#endif // 
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


