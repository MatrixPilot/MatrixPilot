 // This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


// The Coordinate Frame and Dimensional Units of Mavlink are
// explained in detail at this web URL:-
// http://pixhawk.ethz.ch/wiki/software/coordinate_frame
// An abreviated summary is:
// Mavlink Aviation  X Axis is the UDB Aviation Y axis which is the fuselage axis.
// Mavlink Avitation Y axis is out of the right wing, and so is the negative of the UDB X Axis
// Mavlink Aviation  Z axis is downward from the plane, ans so is the same as UDB Z axis.
// Mavlink Yaw is positive to the right (same as UDB)
// Pitch is positive when the front of the plane pitches up from horizontal (opposite of UDB)
// Roll is possitive to the right of the plane (same as UDB)
// So angles follow the "right hand rule"
 
// MatrixPilot uses the extra data streams as
// 	MAV_DATA_STREAM_EXTRA1 = SERIAL UDB EXTRA formated data
//	MAV_DATA_STREAM_EXTRA2 = Scaled position sensor messages (ALTITUDES / AIRSPEEDS)
//	MAV_DATA_STREAM_EXTRA3 not assigned yet

#include <string.h>
#include "defines.h"
#include "../libDCM/libDCM_internal.h" // Needed for access to internal DCM value

#if ( SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK  )

#include "mavlink_options.h"

//Note:  The trap flags need to be moved out of telemetry.c and mavlink.c
volatile int trap_flags __attribute__ ((persistent));
volatile long trap_source __attribute__ ((persistent));
volatile int osc_fail_count __attribute__ ((persistent));

// Setting MAVLINK_TEST_ENCODE_DECODE to 1, will replace the normal code that sends MAVLink messages with 
// as test suite.  The inserted code will self-test every message type to encode packets, de-code packets,
// and it will then check that the results match. The code reports a pass rate and fail rate
// out of the serial port (sent as normal ascii). There should never be any fails. The code
// runs purely within the UAV DevBoard so this purely tests software, not the communication links.
// Normal default is to set MAVLINK_TEST_ENCODE_DECODE to 0

// This testing section of code only compiles if you set the C-Compiler to use the "Large memory code model"
// In MPLAB IDE, select "Project / Build Options / Project", then select Tab MPLAB C30. Then select the
// drop down menu called "Categores" and select "Memory Model". Tick "Large Code Model" instead of 
// "Default Code Model". i.e. The test code will need more than 28K of ROM.
#define MAVLINK_TEST_ENCODE_DECODE	0

#if ( MAVLINK_TEST_ENCODE_DECODE == 0 )
// The following Macro enables MAVLink packets to be sent in one call to the serial driver
// rather than character by character.
#define MAVLINK_SEND_UART_BYTES mavlink_serial_send
#endif

#include "../MAVLink/include/matrixpilot_mavlink_bridge_header.h"

int mavlink_serial_send(mavlink_channel_t chan, uint8_t buf[], uint16_t len);

#if ( MAVLINK_TEST_ENCODE_DECODE == 1 )
mavlink_message_t last_msg ;
#define _ADDED_C_LIB 1 // Needed to get vsnprintf()
#include <stdio.h>
#include <stdarg.h>
#define MAVLINK_TEST_MESSAGE_SIZE 100
uint8_t mavlink_test_message_buffer[MAVLINK_TEST_MESSAGE_SIZE] ;
int mavlink_tests_pass = 0 ;
int mavlink_tests_fail = 0 ;
char mavlink_test_first_pass_flag = 1;
mavlink_status_t  r_mavlink_status ;

#define MAVLINK_ASSERT(exp)    if (!(exp))                                          \
                               {                                                    \
                                     serial_output("MAVLink Test Fail: "            \
                                     "at %s, line %d.\r\n", __FILE__, __LINE__) ;   \
                                      mavlink_tests_fail++ ;                        \
                               }                                                    \
                               else                                                 \
                               {                                                    \
                                     mavlink_tests_pass++ ;                         \
                               }

#endif 

#include "../MAVLink/include/matrixpilot/mavlink.h"

#if (USE_FLEXIFUNCTION_MIXING == 1)
	#ifdef MAVLINK_MSG_ID_FLEXIFUNCTION_SET
		#include "../libflexifunctions/flexifunctionservices.h"
	#else
		#error(" Flexifunctions must be defined in MAVlink to use them")
	#endif
#endif



#if(DECLINATIONANGLE_VARIABLE != 1)
union intbb dcm_declination_angle = {.BB=0};
#endif


/****************************************************************************/
// Variables to support compilation

#if(MAG_YAW_DRIFT != 1)
int udb_magOffset[3];  	// magnetic offset in the body frame of reference
int magGain[3]; 			// magnetometer calibration gains
int rawMagCalib[3];
#endif

#if(USE_NV_MEMORY == 1)
#include "data_services.h"
#endif

/****************************************************************************/

#if ( MAVLINK_TEST_ENCODE_DECODE == 1 )
#include "../MAVLink/include/matrixpilot/testsuite.h"
#endif


#define 	SERIAL_BUFFER_SIZE 			MAVLINK_MAX_PACKET_LEN
#define 	BYTE_CIR_16_TO_RAD  ((2.0 * 3.14159265) / 65536.0 ) // Conveert 16 bit byte circular to radians

void send_text(uint8_t text[]) ;
void handleMessage(mavlink_message_t* msg) ;
void init_mavlink( void ) ;

boolean is_this_the_moment_to_send( unsigned char counter, unsigned char max_counter ) ;
boolean mavlink_frequency_send( unsigned char transmit_frequency, unsigned char counter) ;
boolean mavlink_check_target( uint8_t target_system, uint8_t target_component ) ;

union intbb voltage_milis = {0} ;
unsigned char mavlink_counter_40hz = 0 ;
uint64_t usec = 0 ;			// A measure of time in microseconds (should be from Unix Epoch).
uint64_t msec = 0 ;			// A measure of time in microseconds (should be from Unix Epoch).

int sb_index = 0 ;
int end_index = 0 ;
char serial_interrupt_stopped = 1;
unsigned char serial_buffer[SERIAL_BUFFER_SIZE] ;

float previous_earth_pitch  = 0.0 ;
float previous_earth_roll   = 0.0 ;
float previous_earth_yaw    = 0.0 ;

unsigned char streamRates[MAV_DATA_STREAM_ENUM_END];

extern signed char calculated_heading ;

extern unsigned int number_of_waypoints ;
extern int  waypointIndex ;
uint16_t mavlink_waypoint_requested_sequence_number ;
unsigned char mavlink_waypoint_dest_sysid ;
unsigned char mavlink_waypoint_dest_compid ;
unsigned int  mavlink_waypoint_timeout  = 0 ;
unsigned char number_of_waypoint_retries = 2 ;
uint8_t mavlink_waypoint_frame = MAV_FRAME_GLOBAL ;
boolean mavlink_waypoint_current = true ;

struct mavlink_flag_bits {
			unsigned int unused							: 2 ;
			unsigned int mavlink_send_specific_variable : 1 ;
			unsigned int mavlink_send_variables 		: 1 ;
			unsigned int mavlink_send_waypoint_count    : 1 ;
			unsigned int mavlink_sending_waypoints		: 1 ;
			unsigned int mavlink_receiving_waypoints	: 1 ;
			unsigned int mavlink_send_specific_waypoint : 1 ;
			} mavlink_flags ;

void command_ack(unsigned int command, unsigned int result);
unsigned int 	mavlink_command_ack_command 	= 0;
boolean 		mavlink_send_command_ack		= false;
unsigned int 	mavlink_command_ack_result		= 0;

// Following are required for saving state of PWM variables for SERIAL_UDB_EXTRA compatibility
#define MAVLINK_SUE_CHANNEL_MAX_SIZE	10 //  MatrixPilot.xml MAVLink has fixed SUE protocol for 10 channels
int pwIn_save[MAVLINK_SUE_CHANNEL_MAX_SIZE + 1] ; 
int pwOut_save[MAVLINK_SUE_CHANNEL_MAX_SIZE + 1] ;

int mavlink_sue_telemetry_counter = 8 ;  // Countdown counter, for use with SERIAL_UDB_EXTRA compatibility
boolean mavlink_sue_telemetry_f2_a = true ;		 

#if(USE_NV_MEMORY == 1)
// callback for when nv memory storage is complete
inline void preflight_storage_complete_callback(boolean success);
#endif


void init_serial()
{
#if (( BOARD_TYPE != UDB4_BOARD ) && ( CLOCK_CONFIG !=  FRC8X_CLOCK )) // Cannot support high baud rates
    udb_serial_set_rate(19200) ;
#else
	udb_serial_set_rate(MAVLINK_BAUD) ; 
#endif
	init_mavlink() ;
return ;
}

void init_mavlink( void )
{
	mavlink_system.sysid  =  MAVLINK_SYSID ; // System ID, 1-255, ID of your Plane for GCS
	mavlink_system.compid = 1 ;  // Component/Subsystem ID,  (1-255) MatrixPilot on UDB is component 1.

	// Fill stream rates array with zeros to default all streams off;
	int index;
	for(index = 0; index < MAV_DATA_STREAM_ENUM_END; index++)
		streamRates[index] = 0;	

	// QGroundControl GCS lets user send message to increase stream rate
	streamRates[MAV_DATA_STREAM_RC_CHANNELS] 	= MAVLINK_RATE_RC_CHAN ;	
	streamRates[MAV_DATA_STREAM_RAW_SENSORS] 	= MAVLINK_RATE_IMU_RAW ;
	streamRates[MAV_DATA_STREAM_POSITION] 		= MAVLINK_RATE_POSITION ;

	streamRates[MAV_DATA_STREAM_EXTRA1] 		= MAVLINK_RATE_SUE ;
	streamRates[MAV_DATA_STREAM_EXTRA2] 		= MAVLINK_RATE_POSITION_SENSORS ;
}


int udb_serial_callback_get_byte_to_send(void)
{
	if ( sb_index < end_index && sb_index < SERIAL_BUFFER_SIZE ) // ensure never end up racing thru memory.
	{
		unsigned char txchar = serial_buffer[ sb_index++ ] ;
		return txchar ;
	}
	else
	{
		serial_interrupt_stopped = 1 ;
	}
	
	return -1;
}


int mavlink_serial_send(mavlink_channel_t chan, uint8_t buf[], uint16_t len)
// Note: Channel Number, chan, is currently ignored. 
{
	// Note at the moment, all channels lead to the one serial port
	if (serial_interrupt_stopped == 1) 
	{
		sb_index = 0;
		end_index= 0;
	}
	int start_index = end_index ;
	int remaining = SERIAL_BUFFER_SIZE - start_index ;
	if ( len > remaining ) 
	{
		// Chuck away the entire packet, as sending partial packet
		// will break MAVLink CRC checks, and so receiver will throw it away anyway.
		return(-1) ;
	}
	if (remaining > 1)
	{
		memcpy(&serial_buffer[start_index], buf, len);
		end_index = start_index + len ;
	}
	if (serial_interrupt_stopped == 1)
	{
		serial_interrupt_stopped  = 0;
		udb_serial_start_sending_data(); 
	}
	return(1) ;
}

#if ( MAVLINK_TEST_ENCODE_DECODE == 1 )
// add printf library when running tests to output ascii messages of test results
void serial_output( char* format, ... )
{
    int remaining = 0;
	int wrote = 0 ;
	va_list arglist ;
	va_start(arglist, format) ;	
	remaining = MAVLINK_TEST_MESSAGE_SIZE ;
	wrote = vsnprintf( (char*)(&mavlink_test_message_buffer[0]), (size_t)remaining, format, arglist) ;
	if ( wrote > 0 )
	{
		mavlink_serial_send(MAVLINK_COMM_0,&mavlink_test_message_buffer[0],(uint16_t) wrote) ;
	}
	return ;
}
#endif

#if ( MAVLINK_TEST_ENCODE_DECODE == 1 )
void mp_mavlink_transmit(uint8_t ch)
// This is a special version of the routine for testing MAVLink routines
// The incoming serial stream is parsed to reproduce a mavlink message.
// This will then be checked against the original message and results recorded
// using the MAVLINK_ASSERT macro.
{	
    mavlink_parse_char(0, ch, &last_msg, &r_mavlink_status ) ;
}
#else
void mp_mavlink_transmit(uint8_t ch) 
// routine to send a single character used by MAVlink standard include routines.
// We forward to multi-byte sending routine so that firmware can interleave
// ascii debug messages with MAVLink binary messages without them overwriting the buffer.
{
	mavlink_serial_send(MAVLINK_COMM_0,&ch, 1);
}
#endif

void send_text(uint8_t text[])
{
	uint16_t index = 0;
	while ( text[index++] != 0 && index < 80)
	{
		; // Do nothing, just measuring the length of the text 
	}
	mavlink_serial_send(MAVLINK_COMM_0,text, index - 1) ;
}


void send_uint8(uint8_t value)
// A simple routine for sending a uint8_t number as 2 bytes of text 
// Sent as hexadecimal notation
{
	uint8_t temp;
	temp = value >> 4 ; // Take upper half of hex int.
	if  (temp < 10 )
    {
			mp_mavlink_transmit(temp + 0x30 ) ; //1,2,3,4,5,6,7,8,9
	}
	else
	{
		    mp_mavlink_transmit(temp - 10 + 0x41 ) ; // A,B,C,D,E,F
	}
	temp = value & 0x0f  ; // Take lower half of hex int
	if  (temp < 10 )
    {
			mp_mavlink_transmit(temp + 0x30 ) ; //1,2,3,4,5,6,7,8,9
	}
	else
	{
		    mp_mavlink_transmit(temp - 10 + 0x41 ) ; // A,B,C,D,E,F
	}
}


////////////////////////////////////////////////////////////////////////////////////////
// 
// MAIN MATRIXPILOT MAVLINK CODE FOR RECEIVING COMMANDS FROM THE GROUND CONTROL STATION
//

mavlink_message_t msg ;
mavlink_status_t  r_mavlink_status ;

void udb_serial_callback_received_byte(char rxchar)
{
	if (mavlink_parse_char(0, rxchar, &msg, &r_mavlink_status ))
    {
		handleMessage(&msg) ;
	}
	return ;
}

extern unsigned int maxstack ;
int16_t send_variables_counter = 0;
int16_t send_by_index = 0 ;

// ROUTINES FOR CHANGING UAV ONBOARD PARAMETERS
// All paramaters are sent as type (mavlink_param_union_t) between Ground Control Station and MatrixPilot.
// So paramaters have to be converted between type (mavlink_param_union_t) and their normal representation.
// An explanation of the MAVLink protocol for changing paramaters can be found at:
// http://www.qgroundcontrol.org/parameter_interface

#include "parameter_table.h"

#if ( RECORD_FREE_STACK_SPACE ==  1)
void mavlink_send_param_maxstack( int16_t ) ;
void mavlink_set_maxstack(float setting, int16_t i ) ;
#endif

boolean mavlink_parameter_out_of_bounds( mavlink_param_union_t parm, int16_t i ) ;


boolean mavlink_parameter_out_of_bounds( mavlink_param_union_t parm, int16_t i )
{
	switch(  mavlink_parameter_parsers[mavlink_parameters_list[i].udb_param_type].mavlink_type  )
	{
	case MAVLINK_TYPE_FLOAT:
		if(parm.param_float < mavlink_parameters_list[i].min.param_float)
			return true;
		if(parm.param_float > mavlink_parameters_list[i].max.param_float)
			return true;
		break;

	case MAVLINK_TYPE_UINT32_T:
		if(parm.param_int32 < mavlink_parameters_list[i].min.param_int32)
			return true;
		if(parm.param_int32 > mavlink_parameters_list[i].max.param_int32)
			return true;
		break;

	case MAVLINK_TYPE_INT32_T:
		if(parm.param_int32 < mavlink_parameters_list[i].min.param_int32)
			return true;
		if(parm.param_int32 > mavlink_parameters_list[i].max.param_int32)
			return true;
		break;
	default:
		return true;
		break;
	}

	return false ;
}

#if ( RECORD_FREE_STACK_SPACE ==  1)
void mavlink_send_param_maxstack( int16_t i )
{
	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name , 
		( 4096 - maxstack ) , MAVLINK_TYPE_FLOAT,  count_of_parameters_list , i ) ;
    //mavlink_msg_param_value_send(mavlink_channel_t chan, const char *param_id, float param_value, uint8_t param_type, uint16_t param_count, uint16_t param_index)	
	return ;
} 

void mavlink_set_maxstack( float setting , int16_t i ) 
{
	//send_text((unsigned char*)"Setting maxstack \r\n");
	if (( mavlink_parameters_list[i].readonly == true ) ||
			 ( mavlink_parameter_out_of_bounds( setting , i ) == true )) return ;
	maxstack = (int)( 4096 - setting ) ;
	return ;
}

#endif


void mavlink_send_param_gyroscale_Q14( int16_t i)
{
	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		(float) ( *((int*) mavlink_parameters_list[i].pparam) / ( SCALEGYRO * 16384.0 )) , MAVLINK_TYPE_FLOAT, count_of_parameters_list, i ) ; // 16384.0 is RMAX defined as a float.	
	return ;
}

void mavlink_set_param_gyroscale_Q14(mavlink_param_union_t setting, int16_t i )
{
	if(setting.type != MAVLINK_TYPE_FLOAT) return;

	*((int*) mavlink_parameters_list[i].pparam) = (int) ( setting.param_float * ( SCALEGYRO * 16384.0 ) ) ;
	return ;
}


void mavlink_send_param_Q14( int16_t i )
{
	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		(float) ( *((int*) mavlink_parameters_list[i].pparam) / 16384.0 ) , MAVLINK_TYPE_FLOAT, count_of_parameters_list, i ) ; // 16384.0 is RMAX defined as a float.	
	return ;
}


void mavlink_set_param_Q14(mavlink_param_union_t setting, int16_t i )
{
	if(setting.type != MAVLINK_TYPE_FLOAT) return;

	*((int*) mavlink_parameters_list[i].pparam) = (int) ( setting.param_float * 16384.0 ) ;
	return ;
}


void mavlink_send_param_pwtrim( int16_t i )
{
	// Check that the size of the udb_pwtrim array is not exceeded
	if(mavlink_parameters_list[i].pparam >=  (unsigned char*) (&udb_pwTrim[0] + (sizeof(udb_pwTrim[0]) * NUM_INPUTS)) )
		return;

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		(float) ( *((int*) mavlink_parameters_list[i].pparam) / 2.0 ) , MAVLINK_TYPE_FLOAT, count_of_parameters_list, i ) ; // 16384.0 is RMAX defined as a float.	
	return ;
}


void mavlink_set_param_pwtrim(mavlink_param_union_t setting, int16_t i )
{
	if(setting.type != MAVLINK_TYPE_FLOAT) return;

	// Check that the size of the ubb_pwtrim array is not exceeded
	if(mavlink_parameters_list[i].pparam >=  (unsigned char*) (&udb_pwTrim[0] + (sizeof(udb_pwTrim[0]) * NUM_INPUTS)) )
		return;
						
	*((int*) mavlink_parameters_list[i].pparam) = (int) ( setting.param_float * 2.0 ) ;
	return ;
}


void mavlink_send_param_int16( int16_t i )
{
	param_union_t param ;
	param.param_int32 = *((int*) mavlink_parameters_list[i].pparam);

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		param.param_float , MAVLINK_TYPE_INT32_T, count_of_parameters_list, i ) ; // 16384.0 is RMAX defined as a float.	
	return ;
}


void mavlink_set_param_int16(mavlink_param_union_t setting, int16_t i )
{
	if(setting.type != MAVLINK_TYPE_INT32_T) return;

	*((int*) mavlink_parameters_list[i].pparam) = (int) setting.param_int32 ;
	return ;
}



void mavlink_send_param_null( int16_t i )
{
	return ;
}

void mavlink_set_param_null(float setting, int16_t i )
{
	return ;
}


void mavlink_send_int_circular( int16_t i )
{
	param_union_t param ;
	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int*) mavlink_parameters_list[i].pparam);

	deg_angle.WW = __builtin_mulss(deg_angle._.W0 , (int) (RMAX * 180.0 / 256.0) );

	deg_angle.WW >>= 5;
	if(deg_angle._.W0 > 0x8000)	deg_angle._.W1 ++;		// Take care of the rounding error

	param.param_int32 = deg_angle._.W1;	// >> 6;

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		param.param_float , MAVLINK_TYPE_INT32_T, count_of_parameters_list, i ) ;
	return;
}

void mavlink_set_int_circular(mavlink_param_union_t setting, int16_t i ){
	if(setting.type != MAVLINK_TYPE_INT32_T) return;

	union longww dec_angle;
	dec_angle.WW = __builtin_mulss( (int) setting.param_int32, (int) ( RMAX * (256.0 / 180.0) ) );
	dec_angle.WW <<= 9;
	if(dec_angle._.W0 > 0x8000)	dec_angle.WW += 0x8000;		// Take care of the rounding error
	*((int*) mavlink_parameters_list[i].pparam) = dec_angle._.W1;
	
	return ;
}


void mavlink_send_dm_airspeed_in_cm( int16_t i )
{
	param_union_t param ;
	union longww airspeed;

	airspeed.WW = 0;
	airspeed._.W0 = *((int*) mavlink_parameters_list[i].pparam);

	airspeed.WW = __builtin_mulss(airspeed._.W0 , 10.0 );

	param.param_int32 = airspeed._.W0;

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		param.param_float , MAVLINK_TYPE_INT32_T, count_of_parameters_list, i ) ;
	return;
}

void mavlink_set_dm_airspeed_from_cm(mavlink_param_union_t setting, int16_t i ){
	if(setting.type != MAVLINK_TYPE_INT32_T) return;
	
	union longww airspeed;
	
	airspeed.WW = __builtin_mulss((int) setting.param_int32 , (RMAX / 10.0) );
	airspeed.WW <<= 2;

	*((int*) mavlink_parameters_list[i].pparam) = airspeed._.W1;

	return ;
}



void mavlink_send_cm_airspeed_in_m( int16_t i )
{
	param_union_t param ;

	param.param_float = (float) *((int*) mavlink_parameters_list[i].pparam);
	param.param_float *= 0.01;

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		param.param_float , MAVLINK_TYPE_FLOAT, count_of_parameters_list, i ) ;
	return;
}

void mavlink_set_cm_airspeed_from_m(mavlink_param_union_t setting, int16_t i )
{
	if(setting.type != MAVLINK_TYPE_FLOAT) return;
	
	union longww airspeed;
	
	airspeed.WW = __builtin_mulss((int) setting.param_int32 , (RMAX / 10.0) );
	airspeed.WW <<= 2;

	*((int*) mavlink_parameters_list[i].pparam) = (int) (setting.param_float * 100.0);

	return ;
}



void mavlink_send_dm_airspeed_in_m( int16_t i )
{
	param_union_t param ;

	param.param_float = (float) *((int*) mavlink_parameters_list[i].pparam);
	param.param_float *= 0.1;

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		param.param_float , MAVLINK_TYPE_FLOAT, count_of_parameters_list, i ) ;
	return;
}

void mavlink_set_dm_airspeed_from_m(mavlink_param_union_t setting, int16_t i )
{
	if(setting.type != MAVLINK_TYPE_FLOAT) return;
	
	union longww airspeed;
	
	airspeed.WW = __builtin_mulss((int) setting.param_int32 , (RMAX / 10.0) );
	airspeed.WW <<= 2;

	*((int*) mavlink_parameters_list[i].pparam) = (int) (setting.param_float * 10.0);

	return ;
}


// send angle in dcm units
void mavlink_send_dcm_angle( int16_t i )
{
	param_union_t param ;
	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int*) mavlink_parameters_list[i].pparam);

//	deg_angle.WW = __builtin_mulss(deg_angle._.W0 , 40);
	deg_angle.WW = __builtin_mulss(deg_angle._.W0 , (int) (57.3 * 16.0) ); 	//(RMAX * 180.0 / 256.0 ) );
	deg_angle.WW >>= 2;
	if(deg_angle._.W0 > 0x8000)	deg_angle._.W1 ++;		// Take care of the rounding error

	param.param_int32 = deg_angle._.W1;	// >> 6;

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		param.param_float , MAVLINK_TYPE_INT32_T, count_of_parameters_list, i ) ;
	return;
}

// set angle in dcm units
void mavlink_set_dcm_angle(mavlink_param_union_t setting, int16_t i)
{
	if(setting.type != MAVLINK_TYPE_INT32_T) return;

	union longww dec_angle;
	dec_angle.WW = __builtin_mulss( (int) setting.param_int32, (RMAX* (16.0 / 57.3) ) );	//(int) ( RMAX * 64 / 57.3 )
	dec_angle.WW <<= 12;
	if(dec_angle._.W0 > 0x8000)	dec_angle.WW += 0x8000;		// Take care of the rounding error
	*((int*) mavlink_parameters_list[i].pparam) = dec_angle._.W1;
	
	return ;
}

// send angle rate in units of angle per frame
void mavlink_send_frame_anglerate( int16_t i )
{
	param_union_t param ;
	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int*) mavlink_parameters_list[i].pparam);

//	deg_angle.WW = __builtin_mulss(deg_angle._.W0 , 40);
	deg_angle.WW = __builtin_mulss(deg_angle._.W0 , (int) (57.3 * 40.0) ); 	//(RMAX * 180.0 / 256.0 ) );
	deg_angle.WW <<= 2;
	if(deg_angle._.W0 > 0x8000)	deg_angle._.W1 ++;		// Take care of the rounding error

	param.param_int32 = deg_angle._.W1;	// >> 6;

	mavlink_msg_param_value_send( MAVLINK_COMM_0, mavlink_parameters_list[i].name ,
		param.param_float , MAVLINK_TYPE_INT32_T, count_of_parameters_list, i ) ;
	return;
}

// set angle rate in units of angle per frame
void mavlink_set_frame_anglerate(mavlink_param_union_t setting, int16_t i)
{
	if(setting.type != MAVLINK_TYPE_INT32_T) return;

	union longww dec_angle;
	dec_angle.WW = __builtin_mulss( (int) setting.param_int32, (128.0*7.15) );	//(int) ( RMAX * 128 / (57.3 * 40.0) )
	dec_angle.WW <<= 9;
	if(dec_angle._.W0 > 0x8000)	dec_angle.WW += 0x8000;		// Take care of the rounding error
	*((int*) mavlink_parameters_list[i].pparam) = dec_angle._.W1;
	
	return ;
}

// END OF GENERAL ROUTINES FOR CHANGING UAV ONBOARD PARAMETERS

boolean mavlink_check_target( uint8_t target_system, uint8_t target_component )
{
	if (( target_system == mavlink_system.sysid )
			 // QgroundControl sends parameter refresh list to component 25 (regardless)
			 // But  "Transmit" of parameter updates are sent using a specific component ID of 1 by QGroundControl.
			 // Only use mavlink_check_target if you expect all of the sysid, and compid to be correct.
			 &&( target_component == mavlink_system.compid ))
	{
		return false ;
	}
	else
	{
		return true ;
	}
}


// Portions of the following code in handlesmessage() are templated off source code written by James Goppert for the
// ArdupilotMega, and are used by his kind permission and also in accordance with the GPS V3 licensing
// of that code.

void handleMessage(mavlink_message_t* msg)
// This is the main routine for taking action against a parsed message from the GCS
{
//	send_text( ( unsigned char*) "Handling message ID 0x");
//    send_uint8(msg->msgid);
//    send_text( (unsigned char*) "\r\n");

	switch (msg->msgid)
	{
	    case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:  
	    {
	        // decode
	        mavlink_request_data_stream_t packet;
	        mavlink_msg_request_data_stream_decode(msg, &packet);
			//send_text((const unsigned char*) "Action: Request data stream\r\n");
			// QgroundControl sends data stream request to component ID 1, which is not our component for UDB.
	        if (packet.target_system != mavlink_system.sysid) break; 
																     
	        int freq = 0; // packet frequency
	
	        if (packet.start_stop == 0) freq = 0; // stop sending
	        else if (packet.start_stop == 1) freq = packet.req_message_rate; // start sending
	        else break;
			if(packet.req_stream_id == MAV_DATA_STREAM_ALL)
			{
				// Warning: mavproxy automatically sets all.  Do not include all here, it will overide defaults.
	            streamRates[MAV_DATA_STREAM_RAW_SENSORS] = freq ;  
	            streamRates[MAV_DATA_STREAM_RC_CHANNELS] = freq; 
			}
			else
			{
				if(packet.req_stream_id < MAV_DATA_STREAM_ENUM_END)
					streamRates[packet.req_stream_id] = freq ; 
			}
			break;
	    } 

	    case MAVLINK_MSG_ID_COMMAND_LONG:
		{
	        mavlink_command_long_t packet;
	        mavlink_msg_command_long_decode(msg, &packet);
	        //if (mavlink_check_target(packet.target,packet.target_component) == false ) break;
//		    send_text( ( unsigned char*) "Command ID 0x");
//    		send_uint8(packet.command);
//    		send_text( (unsigned char*) "\r\n");
			switch(packet.command)
			{
			case MAV_CMD_PREFLIGHT_CALIBRATION:
				if(packet.param1 == 1)
				{
#if(USE_NV_MEMORY ==1)
					udb_skip_flags.skip_imu_cal = 0;
#endif
					udb_a2d_record_offsets();
				} 
				else if(packet.param4 == 1)	//param4 = radio calibration
				{
					if(udb_flags._.radio_on == 1)
					{
						udb_servo_record_trims();
						command_ack(packet.command, MAV_CMD_ACK_OK);
					}
					else
						command_ack(packet.command, MAV_CMD_ACK_ERR_FAIL);
				}
				else
					command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
				break;
#if(USE_NV_MEMORY == 1)
			case MAV_CMD_PREFLIGHT_STORAGE:
				if(packet.param1 == MAV_PFS_CMD_WRITE_ALL)
				{
					if(packet.param2 == MAV_PFS_CMD_WRITE_ALL)
						data_services_save_all(STORAGE_FLAG_STORE_CALIB | STORAGE_FLAG_STORE_WAYPOINTS, &preflight_storage_complete_callback);
					else
						data_services_save_all(STORAGE_FLAG_STORE_CALIB, &preflight_storage_complete_callback);
				}
				else if(packet.param1 == MAV_PFS_CMD_READ_ALL)
				{
					if(packet.param2 == MAV_PFS_CMD_READ_ALL)
						data_services_load_all(STORAGE_FLAG_STORE_CALIB | STORAGE_FLAG_STORE_WAYPOINTS, &preflight_storage_complete_callback);
					else
						data_services_load_all(STORAGE_FLAG_STORE_CALIB, &preflight_storage_complete_callback);
				}
				else
					command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
				break;

			case MAV_CMD_PREFLIGHT_STORAGE_ADVANCED:
				{
				switch( (unsigned int) packet.param1  ) 
					{
					case MAV_PFS_CMD_CLEAR_SPECIFIC:
						storage_clear_area(packet.param2, &preflight_storage_complete_callback);
						break;
					case MAV_PFS_CMD_WRITE_SPECIFIC:
						data_services_save_specific(packet.param2, &preflight_storage_complete_callback);
						break;
					case MAV_PFS_CMD_READ_SPECIFIC:
						data_services_load_specific(packet.param2, &preflight_storage_complete_callback);
						break;
					default:
						command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
						break;
					};
				} ;
				break;
#endif
			default:
				command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
				break;
			}
			break;
		} 

//	    case MAVLINK_MSG_ID_COMMAND:
//			break;
/*
	    case MAVLINK_MSG_ID_ACTION:
	    {
			// send_text((unsigned char*) "Action: Specific Action Required\r\n");
	        // decode
	        mavlink_action_t packet;
	        mavlink_msg_action_decode(msg, &packet);
	        if (mavlink_check_target(packet.target,packet.target_component) == false ) break;
			
	        switch(packet.action)
	        {
	
	            case MAV_ACTION_LAUNCH:
					// send_text((unsigned char*) "Action: Launch !\r\n");
	                //set_mode(TAKEOFF);
						
	                break;
	
	            case MAV_ACTION_RETURN:
					// send_text((unsigned char*) "Action: Return !\r\n");
	                //set_mode(RTL);
	                break;
	
	            case MAV_ACTION_EMCY_LAND:
					// send_text((unsigned char*) "Action: Emergency Land !\r\n");
	                //set_mode(LAND);
	                break;
	
	            case MAV_ACTION_HALT: 
					// send_text((unsigned char*) "Action: Halt !\r\n");
	                //loiter_at_location();
	                break;
	
	            case MAV_ACTION_MOTORS_START:
	            case MAV_ACTION_CONFIRM_KILL:
	            case MAV_ACTION_EMCY_KILL:
	            case MAV_ACTION_MOTORS_STOP:
	            case MAV_ACTION_SHUTDOWN: 
	                //set_mode(MANUAL);
	                break;
	
	            case MAV_ACTION_CONTINUE:
	                //process_next_command();
	                break;
	
	            case MAV_ACTION_SET_MANUAL: 
	                //set_mode(MANUAL);
	                break;
	
	            case MAV_ACTION_SET_AUTO:
	                //set_mode(AUTO);
	                break; 
	
	            case MAV_ACTION_STORAGE_READ:
					// send_text((unsigned char*) "Action: Storage Read\r\n");
	                break; 
	
	            case MAV_ACTION_STORAGE_WRITE:
					//send_text((unsigned char*) "Action: Storage Write\r\n");
	                break;
	
	            case MAV_ACTION_CALIBRATE_RC:
					//send_text((unsigned char*) "Action: Calibrate RC\r\n"); 
	                break;
	            
	            case MAV_ACTION_CALIBRATE_GYRO:
	            case MAV_ACTION_CALIBRATE_MAG: 
	            case MAV_ACTION_CALIBRATE_ACC: 
	            case MAV_ACTION_CALIBRATE_PRESSURE:
	            case MAV_ACTION_REBOOT: 
	                //startup_IMU_ground();     
	                break; 
	
	            case MAV_ACTION_REC_START: break; 
	            case MAV_ACTION_REC_PAUSE: break; 
	            case MAV_ACTION_REC_STOP: break; 
	
	            case MAV_ACTION_TAKEOFF:
					//send_text((unsigned char*) "Action: Take Off !\r\n");
	                //set_mode(TAKEOFF);
	                break; 
	
	            case MAV_ACTION_NAVIGATE:
					// send_text((unsigned char*) "Action: Navigate !\r\n");
	                //set_mode(AUTO);
	                break; 
	
	            case MAV_ACTION_LAND:
	                //set_mode(LAND);
	                break; 
	
	            case MAV_ACTION_LOITER:
	                //set_mode(LOITER);
	                break; 
	
	            default: break;
	        }
	    }
	    break;
*/
#if (FLIGHT_PLAN_TYPE == FP_WAYPOINTS )
		/************** Not converted to MAVLink wire protocol 1.0 yet *******************
	    case MAVLINK_MSG_ID_WAYPOINT_REQUEST_LIST:
	    {
			// BULDING
			//send_text((unsigned char*) "waypoint request list\r\n");
	
	        // decode
	        mavlink_waypoint_request_list_t packet;
	        mavlink_msg_waypoint_request_list_decode(msg, &packet);
	        if (mavlink_check_target(packet.target_system,packet.target_component)) break;
			mavlink_waypoint_timeout  = MAVLINK_WAYPOINT_TIMEOUT ; 
	        mavlink_flags.mavlink_sending_waypoints = true;
	        mavlink_flags.mavlink_receiving_waypoints = false;
	        mavlink_waypoint_dest_sysid = msg->sysid;
	        mavlink_waypoint_dest_compid = msg->compid;
			// Start sending waypoints
			mavlink_flags.mavlink_send_waypoint_count = 1 ;
	    }
	    break;
	
	    case MAVLINK_MSG_ID_WAYPOINT_REQUEST:
	    {
			//send_text((unsigned char*)"waypoint request\r\n");
	
	        // Check if in sending waypoint mode ...
	        if (!mavlink_flags.mavlink_sending_waypoints) 
			{
				send_text((unsigned char*)"ID WAYPOINT REQUEST not valid, no longer sending\r\n") ;
				break;
			}
	        // decode
	        mavlink_waypoint_request_t packet;
	        mavlink_msg_waypoint_request_decode(msg, &packet);
	        if (mavlink_check_target(packet.target_system,packet.target_component)) break;
			mavlink_waypoint_timeout  = MAVLINK_WAYPOINT_TIMEOUT ;  
	        mavlink_waypoint_requested_sequence_number =  packet.seq ;
	        mavlink_waypoint_frame = MAV_FRAME_GLOBAL; // reference frame 
			if (mavlink_waypoint_requested_sequence_number == waypointIndex )
			{
				mavlink_waypoint_current = true ;
			}
			else
			{
				mavlink_waypoint_current = false ;
			}
			// send waypoint
			mavlink_flags.mavlink_send_specific_waypoint = 1 ;
			
	        //uint8_t action = MAV_ACTION_NAVIGATE; // action
	        //uint8_t orbit_direction = 0; // clockwise(0), counter-clockwise(1)
	        //float orbit = 0; // loiter radius
	        //float param1 = 0, param2 = 0;
	
	        //switch(tell_command.id)
	        //{
	
	        //case CMD_WAYPOINT: // navigate
	            //action = MAV_ACTION_NAVIGATE; // action
	            //break;
	
	       // case CMD_LOITER_TIME: // loiter
	            //orbit = get(PARAM_WP_RADIUS); // XXX setting loiter radius as waypoint acceptance radius
	            //action = MAV_ACTION_LOITER; // action
	            //param1 = get(PARAM_WP_RADIUS);
	            //param2 = tell_command.p1*100; // loiter time
	            //break;
	
	       // case CMD_TAKEOFF: // takeoff
	            //action = MAV_ACTION_TAKEOFF;
	            //break;
	
	        //case CMD_LAND: // land
	            //action = MAV_ACTION_LAND;
	            //break; 
	
	        //defaut:
				//gcs.send_text("command not handled");
	            //break;
	        //}
	
	        // time that the mav should loiter in milliseconds
	        //uint8_t current = 0; // 1 (true), 0 (false)
	        //if (packet.seq == get(PARAM_WP_INDEX)) current = 1;
	        //float yaw_dir = 0; // yaw orientation in radians, 0 = north XXX: what does this do?
	        //uint8_t autocontinue = 1; // 1 (true), 0 (false)
	        //float x = tell_command.lng/1.0e7; // local (x), global (longitude)
	        //float y = tell_command.lat/1.0e7; // local (y), global (latitude)
	        //float z = tell_command.alt/1.0e2; // local (z), global (altitude)
	        // note XXX: documented x,y,z order does not match with gps raw
	        //mavlink_msg_waypoint_send(chan,msg->sysid,
	                                  //msg->compid,packet.seq,frame,action,
	                                  //orbit,orbit_direction,param1,param2,current,x,y,z,yaw_dir,autocontinue);
	
	        // update last waypoint comm stamp
	        //global_data.waypoint_timelast_send = millis();
	    }
	    break;
	
	    case MAVLINK_MSG_ID_WAYPOINT_ACK:
	    {
			//send_text((unsigned char*)"waypoint ack\r\n");
	
	        // decode
	        mavlink_waypoint_ack_t packet;
	        mavlink_msg_waypoint_ack_decode(msg, &packet);
	        if (mavlink_check_target(packet.target_system,packet.target_component)) break;
	
	        // parse for error - although we do nothing about an error.
	        uint8_t type = packet.type; // ok (0), error(1)
	
	        // turn off waypoint send
	        mavlink_flags.mavlink_sending_waypoints = false;
			mavlink_waypoint_timeout  = 0 ;
	    }
	    break;
	
	    case MAVLINK_MSG_ID_WAYPOINT_CLEAR_ALL:
	    {
			//send_text((unsigned char*)"waypoint clear all\r\n");
	
	        // decode
	        //mavlink_waypoint_clear_all_t packet;
	        //mavlink_msg_waypoint_clear_all_decode(msg, &packet);
	        //if (mavlink_check_target(packet.target_system,packet.target_component)) break;
	
	        // clear all waypoints
	        //uint8_t type = 0; // ok (0), error(1)
	        //set(PARAM_WP_TOTAL,0);
	
	        // send acknowledgement 3 times to makes sure it is received
	        //for (int i=0;i<3;i++) mavlink_msg_waypoint_ack_send(chan,msg->sysid,msg->compid,type);
	
	        break;
	    }
	
	    case MAVLINK_MSG_ID_WAYPOINT_SET_CURRENT:
	    {
			//send_text((unsigned char*)"waypoint set current\r\n");
	
	        // decode
	        //mavlink_waypoint_set_current_t packet;
	        //mavlink_msg_waypoint_set_current_decode(msg, &packet);
	        //if (mavlink_check_target(packet.target_system,packet.target_component)) break;
	
	        // set current waypoint
	        //set(PARAM_WP_INDEX,packet.seq);
			//{
				//Location temp;	// XXX this is gross
				//temp = get_wp_with_index(packet.seq);
				//set_next_WP(&temp);
			//}
	        //mavlink_msg_waypoint_current_send(chan,get(PARAM_WP_INDEX));
	        break;
	    }
	
	    case MAVLINK_MSG_ID_WAYPOINT_COUNT:
	    {
			send_text((unsigned char*)"waypoint count\r\n");
	
	        // decode
	        //mavlink_waypoint_count_t packet;
	        //mavlink_msg_waypoint_count_decode(msg, &packet);
	        //if (mavlink_check_target(packet.target_system,packet.target_component)) break;
	
	        // start waypoint receiving
	        //set(PARAM_WP_TOTAL,packet.count);
	        //if (get(PARAM_WP_TOTAL) > MAX_WAYPOINTS)
	            //set(PARAM_WP_TOTAL,MAX_WAYPOINTS);
	        //global_data.waypoint_timelast_receive = millis();
	        //global_data.waypoint_receiving = true;
	        //global_data.waypoint_sending = false;
	        //global_data.waypoint_request_i = 0;
	        break;
	    }
	
	    case MAVLINK_MSG_ID_WAYPOINT:
	    {
			send_text((unsigned char*)"waypoint\r\n");
	        // Check if receiving waypiont
	        //if (!global_data.waypoint_receiving) break;
	
	        // decode
	        //mavlink_waypoint_t packet;
	        //mavlink_msg_waypoint_decode(msg, &packet);
	        //if (mavlink_check_target(packet.target_system,packet.target_component)) break;
	
	        // check if this is the requested waypoint
	        //if (packet.seq != global_data.waypoint_request_i) break;
	
	        // store waypoint
	       // uint8_t loadAction = 0; // 0 insert in list, 1 exec now
	
	        //switch (packet.frame)
	        //{
	            //case MAV_FRAME_GLOBAL:
	            //{
	                //tell_command.lng = 1.0e7*packet.x;
	                //tell_command.lat = 1.0e7*packet.y;
	                //tell_command.alt = packet.z*1.0e2;
	               // break;
	            //}
	
	            //case MAV_FRAME_LOCAL: // local (relative to home position)
	            //{
	                //tell_command.lng = 1.0e7*ToDeg(packet.x/
	                        //(radius_of_earth*cos(ToRad(home.lat/1.0e7)))) + home.lng;
	                //tell_command.lat = 1.0e7*ToDeg(packet.y/radius_of_earth) + home.lat;
	                //tell_command.alt = -packet.z*1.0e2 + home.alt;
	                //break;
	            //}
	        //}
	
	        // defaults
	        //tell_command.id = CMD_BLANK;
	
	       // switch (packet.action)
	        //{
	        
	            //case MAV_ACTION_TAKEOFF:
	            //{
	                //tell_command.id = CMD_TAKEOFF;
	                //break;
	           // } 
	            //case MAV_ACTION_LAND:
	            //{
	                //tell_command.id = CMD_LAND;
	                //break;
	            //}
	
	            //case MAV_ACTION_NAVIGATE:
	            //{
	                //tell_command.id = CMD_WAYPOINT;
	               // break;
	            //}
	
	            //case MAV_ACTION_LOITER:
	            //{
	                //tell_command.id = CMD_LOITER_TIME;
	               // tell_command.p1 = packet.param2/1.0e2;
	               // break;
	            //}
	        //}
	
	        // save waypoint
	        //set_wp_with_index(tell_command, packet.seq);
	
	        // update waypoint receiving state machine
	        //global_data.waypoint_timelast_receive = millis();
	        //global_data.waypoint_request_i++;
	
	        //if (global_data.waypoint_request_i == get(PARAM_WP_TOTAL))
	        //{
				//gcs.send_text("flight plane received");
	            //uint8_t type = 0; // ok (0), error(1)
	            //mavlink_msg_waypoint_ack_send(chan,msg->sysid,msg->compid,type);
	            //global_data.waypoint_receiving = false;
	           	// XXX ignores waypoint radius for individual waypoints, can
				// only set WP_RADIUS parameter
	        //}
	        break;
	    }
	    ********************* END of WAYPOINT SECTION not converted to wire protocol 1.0 *****/
#endif //(FLIGHT_PLAN_TYPE == FP_WAYPOINTS )

		case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
	    {
			//send_text((unsigned char*)"param request list\r\n");
	        mavlink_param_request_list_t packet;
	        mavlink_msg_param_request_list_decode(msg, &packet);
	        if ( packet.target_system != mavlink_system.sysid) break ;
		  	// Start sending parameters
			send_variables_counter = 0 ;
	        mavlink_flags.mavlink_send_variables = 1 ;
			
	    }
	    break;

		case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
		{
			//send_text((unsigned char*)"Requested specific parameter\r\n");
			mavlink_param_request_read_t packet;
			mavlink_msg_param_request_read_decode(msg, &packet) ;
			if ( packet.target_system != mavlink_system.sysid) break ;
			if (( packet.param_index >= 0 )&& ( packet.param_index <= count_of_parameters_list ))
			{
				send_by_index = packet.param_index ;
				mavlink_flags.mavlink_send_specific_variable = 1 ;
			}
		} break;

	
	    case MAVLINK_MSG_ID_PARAM_SET:
	    {
	        // decode
			//send_text((unsigned char*)"Param Set\r\n");
	        mavlink_param_set_t packet;
	        mavlink_msg_param_set_decode(msg, &packet);
	        if (mavlink_check_target(packet.target_system,packet.target_component) == true)
			{
				send_text((unsigned char*) "failed target system check on parameter set \r\n");
				break;
			}
			else
			{
		        // set parameter
		        const char * key = (const char*) packet.param_id;
		
		        // iterate known parameters
				int16_t i = 0 ;
		        for ( i = 0; i < count_of_parameters_list ; i++) 
				{
		            // compare key with parameter name
		            if (!strcmp(key,(const char *) mavlink_parameters_list[i].name))
				    {
						mavlink_param_union_t param;
						param.type = packet.param_type;
						param.param_float = packet.param_value;

						if (( mavlink_parameters_list[i].readonly == false ) && 
								( mavlink_parameter_out_of_bounds( param, i ) == false ))
						{

							mavlink_parameter_parsers[mavlink_parameters_list[i].udb_param_type].set_param(param, i) ;
							// After setting parameter, re-send it to GCS as acknowledgement of success.
							if( mavlink_flags.mavlink_send_specific_variable == 0 )
							{
								send_by_index = i ;
								mavlink_flags.mavlink_send_specific_variable = 1 ;
							}
						}
					}
				}
	        }
	        break;
	    } // end case


#if(USE_FLEXIFUNCTION_MIXING == 1)
	    case MAVLINK_MSG_ID_FLEXIFUNCTION_SET:
	    {
			// Do nothing with this funciton since it is obsolete
			// Must keep function defined to activate flexifunction mavlink libraries
		}
		break;
	    case MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION:
	    {
	        mavlink_flexifunction_buffer_function_t packet;
	        mavlink_msg_flexifunction_buffer_function_decode(msg, &packet);

	        if (mavlink_check_target(packet.target_system,packet.target_component)) break ;

			// can't respond if busy doing something
			if(flexiFunctionState != FLEXIFUNCTION_WAITING)	return;
		
			flexiFunction_write_buffer_function(&packet.data[0], 
												packet.func_index, 
												packet.data_address, 
												packet.data_size, 
												packet.func_count);
		}
		break;
		case MAVLINK_MSG_ID_FLEXIFUNCTION_DIRECTORY:
	    {
	        mavlink_flexifunction_directory_t packet;
	        mavlink_msg_flexifunction_directory_decode(msg, &packet);
		
			// can't respond if busy doing something
			if(flexiFunctionState != FLEXIFUNCTION_WAITING)	return;

			flexiFunction_write_directory(packet.directory_type , packet.start_index, packet.count, packet.directory_data);
		}
		break;
		case MAVLINK_MSG_ID_FLEXIFUNCTION_COMMAND:
	    {

	        mavlink_flexifunction_command_t packet;
	        mavlink_msg_flexifunction_command_decode(msg, &packet);

			// can't respond if busy doing something
			if(flexiFunctionState != FLEXIFUNCTION_WAITING)	return;
		
			switch(packet.command_type)
			{
			case FLEXIFUNCTION_COMMAND_COMMIT_BUFFER:
				flexiFunctionState = FLEXIFUNCTION_COMMIT_BUFFER;
				break;
			case FLEXIFUNCTION_COMMAND_WRITE_NVMEMORY:
				flexiFunctionState = FLEXIFUNCTION_WRITE_NVMEMORY;
				break;
			}
		}
		break;
#endif	// #if(USE_FLEXIFUNCTION_MIXING == 1)



   }   // end switch
} // end handle mavlink



void command_ack(unsigned int command, unsigned int result)
{
	if(mavlink_send_command_ack == false)
	{
		mavlink_command_ack_result = result;
		mavlink_command_ack_command = command;
		mavlink_send_command_ack = true;
	}
}


////////////////////////////////////////////////////////////////////////////////
// 
// Callbacks for triggering command complete messaging
//

inline void preflight_storage_complete_callback(boolean success)
{
	if(mavlink_send_command_ack == false)
	{
		if(success == true)
			mavlink_command_ack_result = MAV_CMD_ACK_OK;
		else
			mavlink_command_ack_result = MAV_CMD_ACK_ERR_FAIL;

		mavlink_command_ack_command = MAV_CMD_PREFLIGHT_STORAGE;
		mavlink_send_command_ack = true;
	}	
}


////////////////////////////////////////////////////////////////////////////////
// 
// MAIN MAVLINK CODE FOR SENDING COMMANDS TO THE GROUND CONTROL STATION
//

const unsigned char mavlink_freq_table[] = { 0,40,20,13,10,8,7,6,5,4,4 } ;

boolean is_this_the_moment_to_send( unsigned char counter, unsigned char max_counter )
{
	if ( counter % max_counter == 0 )
	{
		return true ;
	}
	else
	{
		return false;
	}	
}

boolean mavlink_frequency_send( unsigned char frequency, unsigned char counter)
// Decide whether it the correct moment to send a given telemetry update, depending on requested frequency
{
	unsigned char max_counter ;
	if ( frequency == 0 )
	{
		return false ;
	}
	else if ( frequency > 0 && frequency < 11 )
	{
		max_counter = mavlink_freq_table[frequency] ;
		return is_this_the_moment_to_send( counter, max_counter ) ;
	}	 
	else if ( frequency > 10  && frequency < 14 )
	{
		max_counter = 4 ;
		return is_this_the_moment_to_send( counter,max_counter ) ;
	}
	else if (frequency > 13 && frequency < 17)
	{
		max_counter = 3 ;
		return is_this_the_moment_to_send( counter,max_counter ) ;
	}
	else if (frequency > 16 && frequency < 24 )
	{
		max_counter = 2 ;
		return is_this_the_moment_to_send( counter,max_counter ) ;
	}
	else if ( frequency > 23 )
	{
		return true ; // send data on every call
	}
	else
	{
		return false ; // should never reach this line
	}
}

void mavlink_output_40hz( void )
#if ( MAVLINK_TEST_ENCODE_DECODE == 1 )
{
	if (mavlink_test_first_pass_flag == 1 )
    {
		serial_output("\r\nRunning MAVLink encode / decode Tests.\r\n") ;
		// reset serial buffer in preparation for testing against buffer
		mavlink_tests_pass = 0 ;
		mavlink_tests_fail = 0 ;
	 	mavlink_test_all(mavlink_system.sysid, mavlink_system.compid, &last_msg) ; 
		serial_output("\r\nMAVLink Tests Pass: %d\r\nMAVLink Tests Fail: %d\r\n", mavlink_tests_pass, mavlink_tests_fail) ;
		mavlink_test_first_pass_flag = 0 ;
    }
	return ;
}

#else
{
	struct relative2D matrix_accum ;
	float earth_pitch ;			   // pitch in radians with respect to earth 
	float earth_roll ;		 	   // roll in radians of the plane with respect to earth frame
	float earth_yaw ;			   // yaw in radians with respect to earth frame
	float earth_pitch_velocity ;   // radians / sec with respect to earth
	float earth_roll_velocity ;    // radians / sec with respect to earth
	float earth_yaw_velocity ;     // radians / sec with respect to earth
	int accum ;					   // general purpose temporary storage
	union longbbbb accum_A_long ;  // general purpose temporary storage
	union longbbbb accum_B_long ;  // general purpose temporary storage
	uint8_t mavlink_mode; 		   // System mode, see MAV_MODE ENUM in mavlink/include/mavlink_types.h
	int32_t lat, lon, alt, relative_alt  = 0 ;
	uint16_t mavlink_heading  = 0 ;
	int angle = 0 ;

	unsigned char spread_transmission_load = 0; // Used to spread sending of different message types over a period of 1 second.

    if ( ++mavlink_counter_40hz >= 40) mavlink_counter_40hz = 0 ;
	
	usec += 25000 ; // Frequency sensitive code
	msec += 25 ; 	// Frequency sensitive code

	// Note that message types are arranged in order of importance so that if the serial buffer fills up,
	// critical message types are more likely to still be transmitted.

	// HEARTBEAT
	spread_transmission_load = 1;

	if ( mavlink_frequency_send( MAVLINK_RATE_HEARTBEAT, mavlink_counter_40hz + spread_transmission_load)) 
	{	
		if (flags._.GPS_steering == 0 && flags._.pitch_feedback == 0)
		{
				 mavlink_mode = MAV_MODE_MANUAL_ARMED ;
		}
		else if (flags._.GPS_steering == 0 && flags._.pitch_feedback == 1)
		{ 
				 mavlink_mode = MAV_MODE_GUIDED_ARMED ;
		}
		else if (flags._.GPS_steering == 1 && flags._.pitch_feedback == 1 && udb_flags._.radio_on == 1)
		{
				 mavlink_mode = MAV_MODE_AUTO_ARMED ;
		}
		else if (flags._.GPS_steering == 1 && flags._.pitch_feedback == 1 && udb_flags._.radio_on == 0)
		{
				 mavlink_mode = MAV_MODE_AUTO_ARMED ; // Return to Landing (lost contact with transmitter)
				// MAVLink wire protocol 1.0 seems not to distinguish a mode for "Return to Landing".
		}
		else
		{
				 mavlink_mode = MAV_MODE_TEST_ARMED ; // Unknown state 
		}
		mavlink_msg_heartbeat_send(MAVLINK_COMM_0,MAV_TYPE_FIXED_WING, MAV_AUTOPILOT_ARDUPILOTMEGA, mavlink_mode, 0, MAV_STATE_ACTIVE ) ;
		//mavlink_msg_heartbeat_send(mavlink_channel_t chan, uint8_t type, uint8_t autopilot, uint8_t base_mode, uint32_t custom_mode, uint8_t system_status)
	}

	spread_transmission_load = 4 ;
	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_RAW_SENSORS] , mavlink_counter_40hz + spread_transmission_load))
	{
		int gps_fix_type;
		if(gps_nav_valid())
			gps_fix_type = 3;
		else
			gps_fix_type = 0;
		mavlink_msg_gps_raw_int_send(MAVLINK_COMM_0, usec, gps_fix_type, lat_gps.WW, long_gps.WW,  alt_sl_gps.WW, hdop, 65535, sog_gps.BB, cog_gps.BB, svs) ;

	}
	
	// GLOBAL POSITION - derived from fused sensors
	// Note: This code assumes that Dead Reckoning is running.
	spread_transmission_load = 6 ;
	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_POSITION] , mavlink_counter_40hz + spread_transmission_load))
	{ 
		accum_A_long.WW = IMUlocationy._.W1 + (long int) ( lat_origin.WW / 90.0 ) ; //  meters North from Equator
		lat  =  (long int) accum_A_long.WW * 90  ;		                          // degrees North from Equator
		if  (cos_lat == 0 )
		{
			// We are at the north or south poles, where there is no longitude
			lon = 0 ;
		}
		else 
		{
			accum_A_long.WW = IMUlocationx._.W1 ;
			accum_A_long.WW = accum_A_long.WW * 16384  ;               // Compiler uses (shift left 14) for this multiplication	
			accum_B_long.WW = ( accum_A_long.WW + 8192 ) / cos_lat  ;  // 8192 improves rounding accuracy
			lon = long_origin.WW + (accum_B_long.WW * 90 ) ;           // degrees 
		}
		accum_A_long.WW = IMUlocationz._.W1 ;
		relative_alt = accum_A_long.WW * 1000  ;
		alt  =  relative_alt + (alt_origin.WW * 10 ) ;      //In millimeters; more accurate if used IMUlocationz._.W0

		// Could calculate heading from DCM, but going to use 2D "calculated_heading" for now until Maths peer reviewed.
		angle = (calculated_heading * 180 + 64) >> 7 ;	// 0-359 (ccw, 0=East)
		angle = -angle + 90 ;
		if (angle > 360 ) angle = angle - 360 ;
		if (angle < 0   ) angle = angle + 360 ;
		mavlink_heading = angle * 100 ;
		mavlink_msg_global_position_int_send(MAVLINK_COMM_0, msec, lat, lon,  alt, relative_alt, 					 
		   	-IMUvelocityy._.W1, IMUvelocityx._.W1, -IMUvelocityz._.W1, //  IMUVelocity  normal units are in cm / second
			mavlink_heading ) ; // heading should be from 0 to 35999 meaning 0 to 359.99 degrees.
		// mavlink_msg_global_position_int_send(mavlink_channel_t chan, uint32_t time_boot_ms, int32_t lat, int32_t lon, int32_t alt,
		//   int32_t relative_alt, int16_t vx, int16_t vy, int16_t vz, uint16_t hdg)
	}
	
	// ATTITUDE
	//  Roll: Earth Frame of Reference
	spread_transmission_load = 12 ;

	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_POSITION] , mavlink_counter_40hz + spread_transmission_load))
	{ 
		matrix_accum.x = rmat[8] ;
		matrix_accum.y = rmat[6] ;
		accum = rect_to_polar16(&matrix_accum) ;			// binary angle (0 to 65536 = 360 degrees)
		earth_roll = ( - accum ) * BYTE_CIR_16_TO_RAD ;		// Convert to Radians
		
		//  Pitch: Earth Frame of Reference
		//  Note that we are using the matrix_accum.x
		//  left over from previous rect_to_polar in this calculation.
		//  so this Pitch calculation must follow the Roll calculation
		matrix_accum.y = rmat[7] ;
		accum = - rect_to_polar16(&matrix_accum) ;			// binary angle (0 to 65536 = 360 degrees)
		earth_pitch = ( accum) * BYTE_CIR_16_TO_RAD ;		// Convert to Radians
		
		// Yaw: Earth Frame of Reference
		
		matrix_accum.x = rmat[4] ;
		matrix_accum.y = rmat[1] ;
		accum = rect_to_polar16(&matrix_accum) ;			// binary angle (0 to 65536 = 360 degrees)
		earth_yaw = ( - accum * BYTE_CIR_16_TO_RAD) ;		// Convert to Radians

		// Beginning of frequency sensitive code
		earth_pitch_velocity = ( earth_pitch - previous_earth_pitch ) * streamRates[MAV_DATA_STREAM_POSITION] ; 
		earth_roll_velocity  = ( earth_roll  - previous_earth_roll  ) * streamRates[MAV_DATA_STREAM_POSITION] ;
		earth_yaw_velocity   = ( earth_yaw   - previous_earth_yaw   ) * streamRates[MAV_DATA_STREAM_POSITION] ;
		// End of frequency sensitive code

		previous_earth_pitch = earth_pitch ;
		previous_earth_roll  = earth_roll  ;
		previous_earth_yaw   = earth_yaw   ;

		mavlink_msg_attitude_send(MAVLINK_COMM_0,msec, earth_roll, earth_pitch, earth_yaw, 
				                  earth_roll_velocity, earth_pitch_velocity, earth_yaw_velocity ) ;
		// mavlink_msg_attitude_send(mavlink_channel_t chan, uint32_t time_boot_ms, float roll, float pitch, float yaw, 
		//	float rollspeed, float pitchspeed, float yawspeed)
	}

#if(MSG_VFR_HUD_WITH_POSITION == 1)
	// ATTITUDE
	//  Roll: Earth Frame of Reference
	spread_transmission_load = 14 ;

	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_POSITION] , mavlink_counter_40hz + spread_transmission_load))
	{
		mavlink_msg_vfr_hud_send(MAVLINK_COMM_0, 0.0, 0.0, mavlink_heading, 0, ((float) IMUlocationz._.W1), (float) -IMUvelocityz._.W1);
	}
#endif

	// SYSTEM STATUS
	spread_transmission_load = 18 ;
	if (mavlink_frequency_send( MAVLINK_RATE_SYSTEM_STATUS, mavlink_counter_40hz + spread_transmission_load)) 
	{
		mavlink_msg_sys_status_send(MAVLINK_COMM_0,
			0 , // Sensors fitted
			0,  // Sensors enabled
			0, 	// Sensor health
		    udb_cpu_load() * 10, 
			0,                   // Battery voltage in mV
			0 ,                  // Current
		    0 ,    				 // Percentage battery remaining 100 percent is 1000 
		    r_mavlink_status.packet_rx_drop_count,
			0,					 // errors_comm
			0,					 // errors_count1
			0, 					 // errors_count2
			0, 					 // errors_count3
            0 ); 				 // errors_count4

        //mavlink_msg_sys_status_send(mavlink_channel_t chan, uint32_t onboard_control_sensors_present, uint32_t onboard_control_sensors_enabled, 
			//uint32_t onboard_control_sensors_health, uint16_t load, uint16_t voltage_battery, int16_t current_battery, int8_t battery_remaining,
			// uint16_t drop_rate_comm, uint16_t errors_comm, uint16_t errors_count1, uint16_t errors_count2, uint16_t errors_count3, uint16_t errors_count4)    

		// Sensor Indices: 0: 3D gyro, 1: 3D acc, 2: 3D mag, 3: absolute pressure, 4: differential pressure, 5: GPS, 6: optical flow, 7: computer vision position, 8: laser based position, 9: external ground-truth (Vicon or Leica). Controllers: 10: 3D angular rate control 11: attitude stabilization, 12: yaw position, 13: z/altitude control, 14: x/y position control, 15: motor outputs / control
	}


	// RC CHANNELS
	// Channel values shifted left by 1, to divide by two, so values reflect PWM pulses in microseconds.
	// mavlink_msg_rc_channels_raw_send(mavlink_channel_t chan, uint16_t chan1_raw, uint16_t chan2_raw,
	//    uint16_t chan3_raw, uint16_t chan4_raw, uint16_t chan5_raw, uint16_t chan6_raw, uint16_t chan7_raw,
	//    uint16_t chan8_raw, uint8_t rssi)
	spread_transmission_load = 24 ;
	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_RAW_SENSORS], mavlink_counter_40hz + spread_transmission_load)) 
	{			
	 mavlink_msg_rc_channels_raw_send(MAVLINK_COMM_0, msec,
			 	(uint16_t)(udb_pwOut[1]>>1),  
				(uint16_t) (udb_pwOut[2]>>1), 
				(uint16_t) (udb_pwOut[3]>>1), 
				(uint16_t) (udb_pwOut[4]>>1),
			 	(uint16_t) (udb_pwOut[5]>>1), 
				(uint16_t) (udb_pwOut[6]>>1), 
				(uint16_t) (udb_pwOut[7]>>1), 
				(uint16_t) (udb_pwOut[8]>>1),
			 	(uint8_t) 0,	// port number for more than 8 servos
			 	(uint8_t) 0); // last item, RSSI currently not measured on UDB.
	}

	// RAW SENSORS - ACCELOREMETERS and GYROS
	// It is expected that these values are graphed to allow users to check basic sensor operation,
	// and to graph noise on the signals. As this code if for testing and graphing basic hardware, it uses 
	// UDB conventions coordinate conventions for X,Y and Z axis rather than MAVLink conventions.
	// See:- http://code.google.com/p/gentlenav/wiki/UDBCoordinateSystems and the "Aviation Convention" diagram. 
   
	spread_transmission_load = 30 ;
	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_RAW_SENSORS] , mavlink_counter_40hz + spread_transmission_load))
	{ 				
#if ( MAG_YAW_DRIFT == 1 ) // Magnetometer is connected
		extern int magFieldRaw[] ;
		mavlink_msg_raw_imu_send(MAVLINK_COMM_0, usec,
					 (int16_t)   udb_xaccel.value,    (int16_t)   udb_yaccel.value,    (int16_t) - udb_zaccel.value, 
					 (int16_t) - udb_xrate.value ,    (int16_t) - udb_yrate.value,     (int16_t) - udb_zrate.value, 
					 (int16_t)   magFieldRaw[0],     ( int16_t)   magFieldRaw[1],      (int16_t)   magFieldRaw[2]) ; 
#else // magnetometer is not connected
		mavlink_msg_raw_imu_send(MAVLINK_COMM_0, usec,
					 (int16_t)   udb_xaccel.value,     (int16_t)   udb_yaccel.value,    (int16_t) - udb_zaccel.value,
					 (int16_t) - udb_xrate.value,      (int16_t) - udb_yrate.value,     (int16_t) - udb_zrate.value,
					 (int16_t)   0,                    ( int16_t)  0,                    (int16_t)   0 ) ; // zero as mag not connected.
#endif
		// mavlink_msg_raw_imu_send(mavlink_channel_t chan, uint64_t time_usec, int16_t xacc, int16_t yacc, int16_t zacc, 
		//		int16_t xgyro, int16_t ygyro, int16_t zgyro, int16_t xmag, int16_t ymag, int16_t zmag)
	}


	// POSITION SENSOR DATA - Using STREAM_EXTRA2 
  
	spread_transmission_load = 36 ;
	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_EXTRA2] , mavlink_counter_40hz + spread_transmission_load))
	{ 				
		mavlink_msg_altitudes_send(MAVLINK_COMM_0, msec, alt_sl_gps.WW, relative_alt, 0, 0 ,0 ,0);
		//static inline void mavlink_msg_altitudes_send(mavlink_channel_t chan, uint32_t time_boot_ms, int32_t alt_gps, int32_t alt_imu, int32_t alt_barometric, int32_t alt_optical_flow, int32_t alt_range_finder, int32_t alt_extra)
	}

	spread_transmission_load = 40 ;
	if (mavlink_frequency_send( streamRates[MAV_DATA_STREAM_EXTRA2] , mavlink_counter_40hz + spread_transmission_load))
	{
		mavlink_msg_airspeeds_send(MAVLINK_COMM_0, msec, 0, 0, 0, 0, 0, 0);
		//mavlink_msg_airspeeds_send(mavlink_channel_t chan, uint32_t time_boot_ms, int16_t airspeed_imu, int16_t airspeed_pitot, int16_t airspeed_hot_wire, int16_t airspeed_ultrasonic, int16_t aoa, int16_t aoy)
	}

	// SEND SERIAL_UDB_EXTRA (SUE) VIA MAVLINK FOR BACKWARDS COMPATIBILITY with FLAN.PYW (FLIGHT ANALYZER)
    // The MAVLink messages for this section of code are unique to MatrixPilot and are defined in matrixpilot.xml 
    spread_transmission_load = 10 ;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_EXTRA1] , mavlink_counter_40hz + spread_transmission_load)) // SUE code historically ran at 8HZ
    {
		switch (mavlink_sue_telemetry_counter)
		{	
			case 8:
				if ( _SWR == 0 )
				{
					// if there was not a software reset (trap error) clear the trap data
					trap_flags = trap_source = osc_fail_count = 0 ;
				}
				mavlink_msg_serial_udb_extra_f14_send(MAVLINK_COMM_0, WIND_ESTIMATION, GPS_TYPE, DEADRECKONING, BOARD_TYPE, AIRFRAME_TYPE, RCON, 
					trap_flags, trap_source, osc_fail_count,CLOCK_CONFIG,FLIGHT_PLAN_TYPE) ;
				RCON = 0 ;
				trap_flags = 0 ;
				trap_source = 0 ;
				osc_fail_count = 0 ;
				mavlink_sue_telemetry_counter-- ;
				break ;
			case 7:
				mavlink_msg_serial_udb_extra_f15_send(MAVLINK_COMM_0, (uint8_t *) ID_VEHICLE_MODEL_NAME, (uint8_t *) ID_VEHICLE_REGISTRATION) ;
                mavlink_sue_telemetry_counter-- ;
				break ;
			case 6:
				mavlink_msg_serial_udb_extra_f16_send(MAVLINK_COMM_0, (uint8_t *) ID_LEAD_PILOT, (uint8_t *) ID_DIY_DRONES_URL ) ;
                mavlink_sue_telemetry_counter-- ;
				break ;
			case 5:
				mavlink_msg_serial_udb_extra_f4_send(MAVLINK_COMM_0, ROLL_STABILIZATION_AILERONS, ROLL_STABILIZATION_RUDDER, PITCH_STABILIZATION, 
                     YAW_STABILIZATION_RUDDER, YAW_STABILIZATION_AILERON, AILERON_NAVIGATION, RUDDER_NAVIGATION, ALTITUDEHOLD_STABILIZED,
					 ALTITUDEHOLD_WAYPOINT, RACING_MODE) ;
				mavlink_sue_telemetry_counter-- ;
				break ;
			case 4:
				mavlink_msg_serial_udb_extra_f5_send(MAVLINK_COMM_0, YAWKP_AILERON, YAWKD_AILERON, ROLLKP, ROLLKD,
					YAW_STABILIZATION_AILERON, AILERON_BOOST) ;
				mavlink_sue_telemetry_counter-- ;
				break ;
			case 3:
				mavlink_msg_serial_udb_extra_f6_send(MAVLINK_COMM_0, PITCHGAIN, PITCHKD, RUDDER_ELEV_MIX, ROLL_ELEV_MIX, ELEVATOR_BOOST) ;
				mavlink_sue_telemetry_counter-- ;
				break ;
			case 2:
				mavlink_msg_serial_udb_extra_f7_send(MAVLINK_COMM_0, YAWKP_RUDDER, YAWKD_RUDDER, ROLLKP_RUDDER, ROLLKD_RUDDER, 
					RUDDER_BOOST, RTL_PITCH_DOWN) ;
				mavlink_sue_telemetry_counter-- ;
				break ;
			case 1:
				mavlink_msg_serial_udb_extra_f8_send(MAVLINK_COMM_0, HEIGHT_TARGET_MAX, HEIGHT_TARGET_MIN, ALT_HOLD_THROTTLE_MIN,
					ALT_HOLD_THROTTLE_MAX, ALT_HOLD_PITCH_MIN, ALT_HOLD_PITCH_MAX, ALT_HOLD_PITCH_HIGH) ;
				mavlink_sue_telemetry_counter-- ;
				break ;
			default:
				{
					if (mavlink_sue_telemetry_f2_a == true)
					{
						mavlink_sue_telemetry_f2_a = false ;
						// Approximate time passing between each telemetry line, even though
						// we may not have new GPS time data each time through.
						// This line is important when GPS lock is lost during flight
		                // It allows telemetry to have a time reference when the GPS time reference is lost
						// Note this does increment the official Time of Week (TOW) for the entire system.
						// It is not changed for now, to preserve close compatibility with origin SERIAL_UDB_EXTRA code.
						if (tow.WW > 0) tow.WW += 250 ; 
		
						if (flags._.f13_print_req == 1)
							{
								// The F13 line of telemetry is printed just once  when origin has been captured after GPS lock
				                mavlink_msg_serial_udb_extra_f13_send(MAVLINK_COMM_0, week_no.BB, lat_origin.WW, long_origin.WW, alt_origin.WW) ;
								flags._.f13_print_req = 0 ;
							}       
				
#if (MAG_YAW_DRIFT == 1)
				    	mavlink_msg_serial_udb_extra_f2_a_send(MAVLINK_COMM_0, tow.WW, ((udb_flags._.radio_on << 2) + (dcm_flags._.nav_capable << 1) + flags._.GPS_steering),
				    	lat_gps.WW , long_gps.WW , alt_sl_gps.WW, waypointIndex,
				        rmat[0] , rmat[1] , rmat[2] , rmat[3] , rmat[4] , rmat[5] , rmat[6] , rmat[7] , rmat[8] ,
				    	( uint16_t ) cog_gps.BB, sog_gps.BB, (uint16_t) udb_cpu_load(), voltage_milis.BB, 
				    	air_speed_3DIMU, estimatedWind[0], estimatedWind[1], estimatedWind[2],
				        magFieldEarth[0],magFieldEarth[1],magFieldEarth[2],
				        svs, hdop) ;
#else
				        mavlink_msg_serial_udb_extra_f2_a_send(MAVLINK_COMM_0, tow.WW, ((udb_flags._.radio_on << 2) + (dcm_flags._.nav_capable << 1) + flags._.GPS_steering),
				    	lat_gps.WW , long_gps.WW , alt_sl_gps.WW, waypointIndex,
				        rmat[0] , rmat[1] , rmat[2] , rmat[3] , rmat[4] , rmat[5] , rmat[6] , rmat[7] , rmat[8] ,
				    	( uint16_t ) cog_gps.BB, sog_gps.BB, (uint16_t) udb_cpu_load(), voltage_milis.BB, 
				    	air_speed_3DIMU, estimatedWind[0], estimatedWind[1], estimatedWind[2],
				        0, 0, 0,
				        svs, hdop) ;
#endif	
				
				        // Save  pwIn and PwOut buffers for sending next time around in f2_b format message
						int i ;
						for (i=0; i <= (NUM_INPUTS  > MAVLINK_SUE_CHANNEL_MAX_SIZE ? MAVLINK_SUE_CHANNEL_MAX_SIZE : NUM_INPUTS) ; i++) 
							pwIn_save[i] = udb_pwIn[i] ;
						for (i=0; i <= (NUM_OUTPUTS > MAVLINK_SUE_CHANNEL_MAX_SIZE ? MAVLINK_SUE_CHANNEL_MAX_SIZE : NUM_OUTPUTS); i++)
							pwOut_save[i] = udb_pwOut[i] ;
					}
					else
					{
						mavlink_sue_telemetry_f2_a = true ;
#if (RECORD_FREE_STACK_SPACE == 1)
						int stack_free = (int)(4096-maxstack); // This is actually wrong for the UDB4, but currently left the same as for telemetry.c
#else
						int stack_free = 0 ;
#endif
	
				        mavlink_msg_serial_udb_extra_f2_b_send( MAVLINK_COMM_0, tow.WW,
					       pwIn_save[1],pwIn_save[2], pwIn_save[2], pwIn_save[4], pwIn_save[5], pwIn_save[6], pwIn_save[7], pwIn_save[8], pwIn_save[9], pwIn_save[10], 
				           pwOut_save[1],pwOut_save[2], pwOut_save[2], pwOut_save[4], pwOut_save[5], pwOut_save[6], pwOut_save[7], pwOut_save[8], pwOut_save[9], pwOut_save[10],
				           IMUlocationx._.W1 ,IMUlocationy._.W1 ,IMUlocationz._.W1, flags.WW, osc_fail_count,
				           IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1,
				           goal.x, goal.y, goal.height, stack_free ) ;
					}		
				}
		}
	}
   // END OF SECTION FOR SERIAL_UDB_EXTRA VIA MAVLINK COMPATIBILITY

	// SEND VALUES OF PARAMETERS IF THE LIST HAS BEEN REQUESTED
	if 	( mavlink_flags.mavlink_send_variables == 1 )
	{
		if ( send_variables_counter < count_of_parameters_list)
		{
			mavlink_parameter_parsers[mavlink_parameters_list[send_variables_counter].udb_param_type].send_param( send_variables_counter) ;
			send_variables_counter++ ;
		}
		else 
		{
			send_variables_counter = 0 ;
			mavlink_flags.mavlink_send_variables = 0 ;
		}	
	}

	// SEND SPECIFICALLY REQUESTED PARAMETER
	if ( mavlink_flags.mavlink_send_specific_variable == 1 )
	{
		mavlink_parameter_parsers[mavlink_parameters_list[send_by_index].udb_param_type].send_param( send_by_index ) ;
		mavlink_flags.mavlink_send_specific_variable = 0 ;
	}

#if (FLIGHT_PLAN_TYPE == FP_WAYPOINTS ) // LOGO_WAYPOINTS cannot be uploaded / downloaded

    /****************** Note yet converted to wire protocol 1.0 *****************

	// CHECK WHETHER WAYPOINT PROTOCOL HAS TIMED OUT WAITING ON A RESPONSE
	if ( mavlink_waypoint_timeout  <= 0 )
	{
		if ( mavlink_flags.mavlink_sending_waypoints ||  mavlink_flags.mavlink_receiving_waypoints )
		{
			send_text((unsigned char *)"Timeout on waypoint protocol.\r\n");
		}
		mavlink_flags.mavlink_sending_waypoints   = false ;
	    mavlink_flags.mavlink_receiving_waypoints = false ;
	}

	// SEND NUMBER OF WAYPOINTS IN WAYPOINTS LIST
	if ( mavlink_flags.mavlink_send_waypoint_count == 1 )
	{
		//send_text((unsigned char *)"Sending waypoint count\r\n") ;
		mavlink_msg_waypoint_count_send(MAVLINK_COMM_0,	mavlink_waypoint_dest_sysid, mavlink_waypoint_dest_compid, number_of_waypoints) ;
		mavlink_flags.mavlink_send_waypoint_count = 0 ;
	}

	// SEND DETAILS OF A SPECIFIC WAYPOINT
	if ( mavlink_flags.mavlink_send_specific_waypoint == 1 )
	{
			//send_text((unsigned char *)"Time to send a specific waypoint\r\n") ;
			
            mavlink_msg_waypoint_send(mavlink_channel_t chan, uint8_t target_system, uint8_t target_component, \
					uint16_t seq, uint8_t frame, uint8_t command, uint8_t current, uint8_t autocontinue,       \
					float param1, float param2, float param3, float param4,                                    \
                    float x, float y, float z) ;
            
			//BUILDING 

			float lat_float, lon_float, alt_float = 0.0 ;
			//accum_long = IMUlocationy._.W1 + ( lat_origin.WW / 90 ) ; //  meters North from Equator
			//lat_float  = (float) (( accum_long * 90 ) / 10000000.0) ; // degrees North from Equator
			//lon_float = (float) ((float) long_origin.WW  + ((float)(IMUlocationx._.W1) * 90.0 ) / ( float )( cos_lat / 16384.0 )) / 10000000.0 ;
			//	extern struct relWaypointDef wp_to_relative(struct waypointDef wp) ;
			//struct relWaypointDef current_waypoint = wp_to_relative( waypoints[waypointIndex] ) ;
			alt_float =  ((float)(IMUlocationz._.W1)) + (float)(alt_origin.WW / 100.0) ;
			 mavlink_msg_waypoint_send(MAVLINK_COMM_0, mavlink_waypoint_dest_sysid, mavlink_waypoint_dest_compid, \
					mavlink_waypoint_requested_sequence_number, mavlink_waypoint_frame, 0, mavlink_waypoint_current, true,       \
					0.0, 0.0, 0.0, 0.0,                                    \
                    2.0, 54.0, 500.0) ;
            
			mavlink_flags.mavlink_send_specific_waypoint = 0 ;
	}

	if ( mavlink_waypoint_timeout  > 0 ) mavlink_waypoint_timeout-- ;

   ************End of section not yet converted to 1.0 wire protocol ***********************/

#endif  // (FLIGHT_PLAN_TYPE == FP_WAYPOINTS )
    

#if(USE_FLEXIFUNCTION_MIXING == 1)
	switch(flexiFunctionState)
	{
	case FLEXIFUNCTION_BUFFER_FUNCTION_ACKNOWLEDGE:
		mavlink_msg_flexifunction_buffer_function_ack_send(MAVLINK_COMM_0, 0,0, flexifunction_ref_index, flexifunction_ref_result);
		flexiFunctionState = FLEXIFUNCTION_WAITING;
		break;
	case FLEXIFUNCTION_INPUT_DIRECTORY_ACKNOWLEDGE:
		mavlink_msg_flexifunction_directory_ack_send(MAVLINK_COMM_0, 0,0, 1, 0, 32, flexifunction_ref_result);
		flexiFunctionState = FLEXIFUNCTION_WAITING;
		break;
	case FLEXIFUNCTION_OUTPUT_DIRECTORY_ACKNOWLEDGE:
		mavlink_msg_flexifunction_directory_ack_send(MAVLINK_COMM_0, 0,0, 0, 0, 32, flexifunction_ref_result);
		flexiFunctionState = FLEXIFUNCTION_WAITING;
		break;
	case FLEXIFUNCTION_COMMAND_ACKNOWLEDGE:
		mavlink_msg_flexifunction_command_ack_send(MAVLINK_COMM_0, flexifunction_ref_command, flexifunction_ref_result);
		flexiFunctionState = FLEXIFUNCTION_WAITING;
		break;

	}
#endif	//#if(USE_FLEXIFUNCTION_MIXING == 1)

	// Acknowledge a command if flaged to do so.
	if(mavlink_send_command_ack == true)
	{
		mavlink_msg_command_ack_send(MAVLINK_COMM_0, mavlink_command_ack_command, mavlink_command_ack_result);
		mavlink_send_command_ack = false;
	}


	return ;
}
#endif // ( MAVLINK_TEST_ENCODE_DECODE == 1 )
#endif  // ( SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK )

