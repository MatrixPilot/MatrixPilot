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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define inline __inline
#endif

//#include <sys/types.h>
#include <winsock.h>

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

#include "MAVLink/include/matrixpilot_mavlink_bridge_header.h"

int mavlink_serial_send(mavlink_channel_t chan, uint8_t buf[], uint16_t len);


#include "MAVLink/include/matrixpilot/mavlink.h"



#define 	SERIAL_BUFFER_SIZE 			MAVLINK_MAX_PACKET_LEN
#define 	BYTE_CIR_16_TO_RAD  ((2.0 * 3.14159265) / 65536.0 ) // Conveert 16 bit byte circular to radians

void send_text(uint8_t text[]) ;
void handleMessage(mavlink_message_t* msg) ;
void init_mavlink( void ) ;


int sb_index = 0 ;
int end_index = 0 ;
char serial_interrupt_stopped = 1;
unsigned char serial_buffer[SERIAL_BUFFER_SIZE] ;




void init_serial()
{
//#if (( BOARD_TYPE != UDB4_BOARD ) && ( CLOCK_CONFIG !=  FRC8X_CLOCK )) // Cannot support high baud rates
//    udb_serial_set_rate(19200) ;
//#else
//	udb_serial_set_rate(MAVLINK_BAUD) ; 
//#endif
//	init_mavlink() ;
//return ;
}

void init_mavlink( void )
{
//	mavlink_system.sysid  =  MAVLINK_SYSID ; // System ID, 1-255, ID of your Plane for GCS
	//mavlink_system.compid = 1 ;  // Component/Subsystem ID,  (1-255) MatrixPilot on UDB is component 1.

	//// Fill stream rates array with zeros to default all streams off;
	//int index;
	//for(index = 0; index < MAV_DATA_STREAM_ENUM_END; index++)
	//	streamRates[index] = 0;	

	//// QGroundControl GCS lets user send message to increase stream rate
	//streamRates[MAV_DATA_STREAM_RC_CHANNELS] 	= MAVLINK_RATE_RC_CHAN ;	
	//streamRates[MAV_DATA_STREAM_RAW_SENSORS] 	= MAVLINK_RATE_IMU_RAW ;
	//streamRates[MAV_DATA_STREAM_POSITION] 		= MAVLINK_RATE_POSITION ;

	//streamRates[MAV_DATA_STREAM_EXTRA1] 		= MAVLINK_RATE_SUE ;
	//streamRates[MAV_DATA_STREAM_EXTRA2] 		= MAVLINK_RATE_POSITION_SENSORS ;
}


int udb_serial_callback_get_byte_to_send(void)
{
	//if ( sb_index < end_index && sb_index < SERIAL_BUFFER_SIZE ) // ensure never end up racing thru memory.
	//{
	//	unsigned char txchar = serial_buffer[ sb_index++ ] ;
	//	return txchar ;
	//}
	//else
	//{
	//	serial_interrupt_stopped = 1 ;
	//}
	//
	return -1;
}


int mavlink_serial_send(mavlink_channel_t chan, uint8_t buf[], uint16_t len)
// Note: Channel Number, chan, is currently ignored. 
{
	//// Note at the moment, all channels lead to the one serial port
	//if (serial_interrupt_stopped == 1) 
	//{
	//	sb_index = 0;
	//	end_index= 0;
	//}
	//int start_index = end_index ;
	//int remaining = SERIAL_BUFFER_SIZE - start_index ;
	//if ( len > remaining ) 
	//{
	//	// Chuck away the entire packet, as sending partial packet
	//	// will break MAVLink CRC checks, and so receiver will throw it away anyway.
	//	return(-1) ;
	//}
	//if (remaining > 1)
	//{
	//	memcpy(&serial_buffer[start_index], buf, len);
	//	end_index = start_index + len ;
	//}
	//if (serial_interrupt_stopped == 1)
	//{
	//	serial_interrupt_stopped  = 0;
	//	udb_serial_start_sending_data(); 
	//}
	return(1) ;
}



void mp_mavlink_transmit(uint8_t ch) 
// routine to send a single character used by MAVlink standard include routines.
// We forward to multi-byte sending routine so that firmware can interleave
// ascii debug messages with MAVLink binary messages without them overwriting the buffer.
{
	mavlink_serial_send(MAVLINK_COMM_0,&ch, 1);
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
		}
	}
}
		


// ROUTINES FOR CHANGING UAV ONBOARD PARAMETERS
// All paramaters are sent as type (mavlink_param_union_t) between Ground Control Station and MatrixPilot.
// So paramaters have to be converted between type (mavlink_param_union_t) and their normal representation.
// An explanation of the MAVLink protocol for changing paramaters can be found at:
// http://www.qgroundcontrol.org/parameter_interface





#ifdef __cplusplus
}
#endif // __cplusplus



