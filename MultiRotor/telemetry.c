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

char debug_buffer[512] ;
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
extern int16_t udb_magFieldBody[3] ;
extern fractional magFieldEarth[3];
extern fractional magAlignment[4];
extern int16_t udb_magOffset[3] ;

// Prepare a line of serial output and start it sending
void send_debug_line( void )
{
	db_index = 0 ;
	
	if (!hasWrittenHeader)
	{
		header_line ++ ;
		switch ( header_line ) {
		case 1:
			sprintf(debug_buffer, "\r\n") ;
			break ;
		case 2:
			sprintf(debug_buffer, "RCON=0x%X , TRAP_FLAGS=0x%X , TRAP_SOURCE=0x%lX , ALARMS=%i\r\n",
			    get_reset_flags(), trap_flags, trap_source, osc_fail_count);
			break ;
		case 3:
			sprintf(debug_buffer, "MAX_TILT = %i\r\n" , MAX_TILT ) ;
			break ;	
		case 4:
			sprintf(debug_buffer, "TILT_KP = %5f, YAW_KP = %5f\r\n" ,
				TILT_KP ,
				YAW_KP  ) ;
			break ;	
		case 5:
			sprintf(debug_buffer, "TILT_KI = %5f, YAW_KI = %5f\r\n" ,
				TILT_KI ,
				YAW_KI  ) ;
			break ;
		case 6:
			sprintf(debug_buffer, "TILT_KD = %5f, YAW_KD = %5f\r\n" ,
				TILT_KD ,
				YAW_KD ) ;
			break ;
		case 7:
			sprintf(debug_buffer, "TILT_FF = %5f, TILT_KDD = %5f, ACCEL_K = %5f\r\n" ,
					TILT_FF ,
					TILT_KDD ,
				ACCEL_K ) ;
			break ;			
		case 8:
			sprintf(debug_buffer, ROTOR_CRAFT );
			break ;
		case 9:
			sprintf(debug_buffer, "ESC_HZ = %i , PID_HZ = %i , SERVO_HZ = %i , DR_TAU = %5f\r\n" , ESC_HZ , PID_HZ , SERVO_HZ , DR_TAU );
			break ;
		case 10:
			sprintf(debug_buffer, "USE_LIDAR = %i , MAX_ALT_RATE_CONTROL = %i , MAX_ALT_PROP_CONTROL = %i\r\n" ,
				USE_LIDAR ,
				MAX_ALT_RATE_CONTROL ,
				MAX_ALT_PROP_CONTROL ) ;
			break ;			
		case 11:
			sprintf(debug_buffer, "MIN_LIDAR_PULSE_THRESH = %i, IMU_CLIMB_RATE_DIVISOR = %i\r\n" ,
				MIN_LIDAR_PULSE_THRESH ,
				IMU_CLIMB_RATE_DIVISOR ) ;
			break ;				
		case 12:
			sprintf(debug_buffer, "IMU_ALT_DIVISOR = %i, TARGET_ALTITUDE = %i\r\n" ,
				IMU_ALT_DIVISOR ,
				TARGET_ALTITUDE ) ;
			break ;			
		case 13:
			/*sprintf(debug_buffer, "mx, my, mz, X , Y , VX , VY , ffx , ffy , ffz , pulses, IMU_alt , alt , IMU_climb , clmb_r , alt_cntrl , hrtbt , cpu , mtra , mtrb , mtrc ,mtrd , r6 , r7 , w0 , w1 , w2 , rfb , pfb , yfb , rerr, perr, yerr\r\n" ) ;*/
			sprintf(debug_buffer, "magBdyx , magBdyy , magBdyz , mErthx , mErthy , mErthz , offx , offy , offz , algn0 , algn1 , algn2 , algn3\r\n") ;
			hasWrittenHeader = 1 ;
			break ;
		default:
			hasWrittenHeader = 1 ;
			break ;
		}
	}
	else
	{
		/* sprintf(debug_buffer, "%i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i\r\n" ,
			udb_magFieldBody[0] , udb_magFieldBody[1] , udb_magFieldBody[2] ,
			IMUlocationx._.W1 , IMUlocationy._.W1 , IMUvelocityx._.W1 , IMUvelocityy._.W1 ,
			target_rate[0] , target_rate[1] , target_rate[2] , 
				//IMU_altitude , IMU_climb  ,
				//number_pulses , altitude , climb_rate , altitude_control , 
			number_pulses , IMU_altitude , altitude , IMU_climb  , climb_rate , altitude_control , 
			udb_heartbeat_counter , (int) udb_cpu_load() ,
			udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] ,		
			udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] ,
			udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] ,
			udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] ,
			rmat[6] , rmat[7] , 
			// target_rmat[6] , target_rmat[7] ,
			omegagyro[0] , omegagyro[1] , omegagyro[2] , 
			roll_control , pitch_control, yaw_control ,
			roll_error , pitch_error , yaw_error ) ;
			//commanded_roll , commanded_pitch , commanded_yaw , pwManual[THROTTLE_INPUT_CHANNEL] ,
			//accel_feedback ) ; */
			sprintf(debug_buffer, "%i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i , %i\r\n" ,
			udb_magFieldBody[0] , udb_magFieldBody[1] , udb_magFieldBody[2] ,
			magFieldEarth[0] , magFieldEarth[1] , magFieldEarth[2] , 
			udb_magOffset[0] , udb_magOffset[1] , udb_magOffset[2] ,
			magAlignment[0] , magAlignment[1] , magAlignment[2] , magAlignment[3] 
			);
	}
	
	udb_serial_start_sending_data() ;
	
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

// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.
int udb_serial_callback_get_byte_to_send(void)
{
	unsigned char c = debug_buffer[ db_index++ ] ;
	
	if (c == 0) return -1 ;
	
	return c ;
}


// Don't respond to serial input
void udb_serial_callback_received_byte(uint8_t rxchar)
{
	// Do nothing
	return ;
}


