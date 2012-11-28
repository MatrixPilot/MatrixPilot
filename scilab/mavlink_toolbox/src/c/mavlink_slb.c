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
void mavlink_received_byte(scicos_block *block, char rxchar);
void handleMessage(scicos_block *block, mavlink_message_t* msg);

#include "MAVLink/include/matrixpilot/mavlink.h"


static int iSocket = 0;

#define MAVLINK_SYSID 250

#define MAVLINK_RX_BUFF_SIZE  50
unsigned char mavlink_rx_buffer[MAVLINK_RX_BUFF_SIZE*2];

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
    double *y = GetRealOutPortPtrs(block,1);
    int *piPort = GetIparPtrs(block);

	switch(flag) {
    case DerivativeState:
    {
        printf("[DEBUG] mavlink_receive :: DerivativeState\n");
    }
    break;
    case OutputUpdate:
    {
        //printf("[DEBUG] udp_receive :: OutputUpdate\n");
        // receive data from UDP (can block)
    	unsigned int datasize = 0;
    	datasize = getData(iSocket, mavlink_rx_buffer, MAVLINK_RX_BUFF_SIZE);
        unsigned int index;
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
        //file = fopen("mavlink_scb_log.txt","w"); /* open for writing */
        y[0] = 0;
    }
    break;
    case Ending:
    {
        printf("[DEBUG] mavlink_receive :: Ending\n");
        // close the connection
        //fclose(file); /* close the file before ending program */
        closeServer(iSocket);
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
	    printf("[DEBUG] mavlink_receive :: mavlink found message\n");
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
	    	long total = ((long) packet.vx * (long) packet.vx);
	    	total += ((long) packet.vy * (long) packet.vy);
	        double *y = GetRealOutPortPtrs(block,1);
	    	y[0] = sqrt(total);
	    } break;
	}
}


#ifdef __cplusplus
}
#endif // __cplusplus


