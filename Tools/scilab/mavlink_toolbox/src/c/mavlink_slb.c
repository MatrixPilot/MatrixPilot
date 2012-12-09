/*
 *  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 *  Copyright (C) 2011-2011 - DIGITEO - Bruno JOFRET
 *
 *  This file must be used under the terms of the CeCILL.
 *  This source file is licensed as described in the file COPYING, which
 *  you should have received as part of this distribution.  The terms
 *  are also available at
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */
 
#define RMAX 16384.0

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define inline __inline
#endif

#include <stdio.h>

#include "scicos_block4.h"
#include "udp_server.h"

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

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

#include "MAVLink/include/scilab_mavlink_bridge_header.h"

int mavlink_serial_send(mavlink_channel_t chan, uint8_t buf[], uint16_t len);
void mavlink_received_byte(scicos_block *block, char rxchar);
void handleMessage(scicos_block *block, mavlink_message_t* msg);

#include "../../../../../MAVLink/include/matrixpilot/mavlink.h"


static int 				iSocket = 0;
static int  			first_msg_received = 0;
static unsigned char 	msg_count = 0;
static long 			msg_fail_count = 0;
static int64_t	 		last_msg_time;


#define MAVLINK_SYSID 250

#define MAVLINK_RX_BUFF_SIZE  100
unsigned char mavlink_rx_buffer[MAVLINK_RX_BUFF_SIZE*2];

enum
{
	OUTPUT_PORT_MISSED_MSG_COUNT = 1,	// double
	OUTPUT_PORT_TIMESTEP,				// double
	OUTPUT_PORT_RMAT,					// 3*3 double
	OUTPUT_PORT_GROUNDSPEED,			// 1*3 double
	OUTPUT_PORT_WINDSPEED,				// 1*3 double
	OUTPUT_PORT_ACCELERATION,			// 1*3 double
	OUTPUT_PORT_ROTATION,				// 1*3 double
	OUTPUT_PORT_MAGNETOMETER,			// 1*3 double
};

FILE *file;


void mavlink_init(scicos_block *block, int flag)
{
	mavlink_system.sysid  =  MAVLINK_SYSID ; // System ID, 1-255, ID of your Plane for GCS
	mavlink_system.compid = 1 ;  // Component/Subsystem ID,  (1-255) MatrixPilot on UDB is component 1.
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


void mavlink_receive(scicos_block *block, int flag)
{
    double *y = NULL;
    int *piPort = GetIparPtrs(block);
  	unsigned int datasize = 0;
    unsigned int index = 0;

	switch(flag) {
    case DerivativeState:
    {
        printf("[DEBUG] mavlink_receive :: DerivativeState\n");
    }
    break;
    case OutputUpdate:
    {
        // check socket is valid
        if( (iSocket == ERROR_CAN_T_OPEN_SOCKET) | (iSocket == ERROR_CAN_T_BIND) )
        {
        	set_block_error(-3);
            return;
        }
        
    	datasize = getData(iSocket, mavlink_rx_buffer, MAVLINK_RX_BUFF_SIZE);
        if(datasize > 0)
            printf("[DEBUG] udp_receive :: mavlink rx buffer data ready\n");

        //write(file, mavlink_rx_buffer, datasize);

        for(index = 0; index < datasize; index++)
        {
            mavlink_received_byte(block, mavlink_rx_buffer[index]);
        }

    }
    break;
    case StateUpdate:
    {
        printf("[DEBUG] udp_receive :: StateUpdate\n");
    }
    break;
    case OutputEventTiming:
    {
        printf("[DEBUG] mavlink_receive :: OutputEventTiming\n");
        // can emit output event (not supported yet)
    }
    break;
    case Initialization:
    {
        printf("[DEBUG] mavlink_receive :: Initialization\n");
        // initialise the connection
        // use block->work to store any internal state
        iSocket = startServer(*piPort);
 
        // check socket is valid
        if( (iSocket == ERROR_CAN_T_OPEN_SOCKET) | (iSocket == ERROR_CAN_T_BIND) | (iSocket == 0) )
        {
            printf("[DEBUG] mavlink_receive :: socket start failure\n");
        	set_block_error(-3);
            return;
        }
        
        printf("[DEBUG] mavlink_receive :: socket start ok\n");

        y = GetRealOutPortPtrs(block,OUTPUT_PORT_MISSED_MSG_COUNT);
        y[0] = 0;
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_TIMESTEP);
        y[0] = 0;
        /*
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_RMAT);
        y[0,0] = 0;
        y[0,1] = 0;
        y[0,2] = 0;
        y[1,0] = 0;
        y[1,1] = 0;
        y[1,2] = 0;
        y[2,0] = 0;
        y[2,1] = 0;
        y[2,2] = 0;
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_GROUNDSPEED);
        y[0,0] = 0;
        y[0,1] = 0;
        y[0,2] = 0;
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_WINDSPEED);
        y[0,0] = 0;
        y[0,1] = 0;
        y[0,2] = 0;
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_ACCELERATION);
        y[0,0] = 0;
        y[0,1] = 0;
        y[0,2] = 0;
        y = GetRealOutPortPtrs(block,OUTPUT_PORT_ROTATION);
        y[0,0] = 0;
        y[0,1] = 0;
        y[0,2] = 0;
        */
    }
    break;
    case Ending:
    {
        printf("[DEBUG] mavlink_receive :: Ending\n");
        // close the connection
        //fclose(file); /* close the file before ending program */
        if( (iSocket == ERROR_CAN_T_OPEN_SOCKET) | (iSocket == ERROR_CAN_T_BIND) | (iSocket == 0) )
            return;
        closeServer(iSocket);
        iSocket = 0;
    }
    break;
    case ReInitialization:
    {
        printf("[DEBUG] mavlink_receive :: ReInitialization\n");
    }
    break;
    default:
        printf("[DEBUG] mavlink_receive :: default ?????\n");
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
//
// MAIN CODE FOR RECEIVING MAVLINK
//

mavlink_message_t msg ;
mavlink_status_t  r_mavlink_status ;

void mavlink_received_byte(scicos_block *block, char rxchar)
{
	if (mavlink_parse_char(0, rxchar, &msg, &r_mavlink_status ))
    {
		handleMessage(block, &msg) ;
	}
	return ;
}


void handleMessage(scicos_block *block, mavlink_message_t* msg)
// This is the main routine for taking action against a parsed message from the GCS
{
//	send_text( ( unsigned char*) "Handling message ID 0x");
//    send_uint8(msg->msgid);
//    send_text( (unsigned char*) "\r\n");

    long total = 0;
    double *y = NULL;
    int64_t	msg_time;	// message time in ms
	mavlink_sitl_imu_output_t packet;


	#ifndef _MSC_VER
		struct timeval tv;
	#else
	#endif

    if(first_msg_received == 0)
    {
        msg_count = msg->seq;

		#ifndef _MSC_VER
        	gettimeofday(&tv, NULL);
			last_msg_time =
				(int64_t)(tv.tv_sec) * 1000 +
				(int64_t)(tv.tv_usec) * 0.001;

		#endif
		first_msg_received = 1;
    }
    else
    {
    	msg_count++;
    	if(msg_count != msg->seq)
    		msg_fail_count += (msg->seq - msg_count);
        msg_count = msg->seq;

		#ifndef _MSC_VER
			gettimeofday(&tv, NULL);

			msg_time =
				(int64_t)(tv.tv_sec) * 1000 +
				(int64_t)(tv.tv_usec) * 0.001;
		#else
		#endif

        y = GetRealOutPortPtrs(block,OUTPUT_PORT_MISSED_MSG_COUNT);
        y[0] = ((double) msg_fail_count);	//

        y = GetRealOutPortPtrs(block,OUTPUT_PORT_TIMESTEP);
        y[0] = (double) (msg_time - last_msg_time);
        last_msg_time = msg_time;
    }


	switch (msg->msgid)
	{
	    case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:
	    {
		} break;
	    case MAVLINK_MSG_ID_HEARTBEAT:
	    {
            printf("[DEBUG] mavlink heartbeat\n");

	    } break;
	    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT :
	    {
	    	mavlink_global_position_int_t packet;
	    	mavlink_msg_global_position_int_decode(msg, &packet);
//	    	total = ((long) packet.vx * (long) packet.vx);
//	    	total += ((long) packet.vy * (long) packet.vy);
//	        y = GetRealOutPortPtrs(block,1);
//	    	y[0] = sqrt(total);
	    } break;
	    case MAVLINK_MSG_ID_SITL_IMU_OUTPUT:
	    {
            printf("[DEBUG] sitl imu output\n");
	    	mavlink_msg_sitl_imu_output_decode(msg, &packet);

	        y = GetRealOutPortPtrs(block,OUTPUT_PORT_RMAT);
	        y[0] = ((double) packet.rmat0)/RMAX;
	        y[1] = ((double) packet.rmat1)/RMAX;
	        y[2] = ((double) packet.rmat2)/RMAX;
	        y[3] = ((double) packet.rmat3)/RMAX;
	        y[4] = ((double) packet.rmat4)/RMAX;
	        y[5] = ((double) packet.rmat5)/RMAX;
	        y[6] = ((double) packet.rmat6)/RMAX;
	        y[7] = ((double) packet.rmat7)/RMAX;
	        y[8] = ((double) packet.rmat8)/RMAX;


	        y = GetRealOutPortPtrs(block,OUTPUT_PORT_GROUNDSPEED);
	        y[1] = ((double) packet.vx) / 100.0;
	        y[2] = ((double) packet.vy) / 100.0;
	        y[3] = ((double) packet.vz) / 100.0;

	        y = GetRealOutPortPtrs(block,OUTPUT_PORT_WINDSPEED);
	        y[1] = ((double) packet.estimated_wind_x) / 100.0;
	        y[2] = ((double) packet.estimated_wind_y) / 100.0;
	        y[3] = ((double) packet.estimated_wind_z) / 100.0;

	        y = GetRealOutPortPtrs(block,OUTPUT_PORT_ACCELERATION);
	        y[0] = ((double) packet.xacc);
	        y[1] = ((double) packet.yacc);
	        y[2] = ((double) packet.zacc);

	        y = GetRealOutPortPtrs(block,OUTPUT_PORT_ROTATION);
	        y[0] = ((double) packet.xgyro);
	        y[1] = ((double) packet.ygyro);
	        y[2] = ((double) packet.zgyro);      

	    } break;

	}
}


#ifdef __cplusplus
}
#endif // __cplusplus


