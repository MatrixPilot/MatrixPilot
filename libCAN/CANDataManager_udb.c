// SEE END OF FILE FOR LICENSE TERMS

/****************************************************************************
CANDataManager

Responsible for implementing CAN control data transfer library functions
 Transmit requests
 receive data callbacks
Control data is:
  Radio inputs
  Autopilot controls (yaw,pitch,roll etc...)
  Servo outputs

Control information can go in any direction between UDB and interface devices

Translates between CAN data and UDB data with relevant flag and format changes
  Using data type to call the correct callback
  Using data length to call callbacks at the right time
     Only call callbacks once the last packet is received
  Transfer transmit information into the CAN buffers and request transmission

CAUTION: With the existing implementation, care must be taken to only transfer
a set of control data in one direction.  
  ie. both ends should not transfer servo outputs. The result will be unpredicatable

****************************************************************************/

#include "CANDataManager.h"
#include "CANDataProtocol.h"
#include "libCAN.h"
#include "../MatrixPilot/defines.h"
#include "../CANInterface/CANInterface_defines.h"

#include <string.h>

unsigned int 	CANServoDataBuff[MAX_OUTPUTS];
RADIO_DATA 		CANRadioDataBuff[MAX_INPUTS];

// Transmission request flags
DATA_TX_REQUESTS	txRequests;

unsigned int 	radioChannelsFound = 0;

//-----------------------------------------------------------------------------------------------------------------------
		// Broadcast data on CANbus
//-----------------------------------------------------------------------------------------------------------------------

// Copy the received radio inputs to the CAN buffer then request that they are sent.
void CAN_send_radio_inputs()
{
};



// request that servo outputs are sent
inline void CAN_send_servo_outputs()
{
	txRequests._.tx_req_servo_data = 1;
	CAN_request_send_next_data();
}



// Get the next data type block to transmit
// Fill out the variables in the protocol header for the block to transmit
// These include: Data type, data length, pointer to the data source buffer.
void CAN_get_next_tx_data_block(void)
{
	can_tx_bufferIndex = 0;

	if(txRequests._.tx_req_servo_data == 1)
	{
		ptxBuff = (unsigned int*) udb_pwOut;
		txRequests._.tx_req_servo_data = 0;
		can_tx_type = CONTROL_DATA_SERVO;
		can_tx_maxSize = MAX_OUTPUTS;
	}
	else
	{
		can_tx_maxSize = 0;
		ptxBuff = NULL;
		can_tx_type = CONTROL_DATA_NONE;
	}
}


//-----------------------------------------------------------------------------------------------------------------------
		// Receive data from CANbus
//-----------------------------------------------------------------------------------------------------------------------

// Parses the raw data from the data protocol service
void CAN_received_data(unsigned char type, unsigned char index, unsigned char length, unsigned char* pData)
{
	unsigned int* pRxDataBuff;

	switch(	type )
	{
	case CONTROL_DATA_RADIO:
		pRxDataBuff = (unsigned int*) &CANRadioDataBuff;
		break;
	}

	memcpy( (unsigned char*) &pRxDataBuff[index], pData , length);
	length >>= 1;	// divide datalength to get number of int channels transmitted

	switch ( type )
	{
	case CONTROL_DATA_RADIO:
		if( (index+length) >= NUM_INPUTS)
			CAN_received_radio_inputs_callback();
		break;
	}
}


// Autopilot recieves, reformats and acts on servo data from CAN interface
void CAN_received_radio_inputs_callback(void)
{
    int index;
	int offset_index;

	offset_index = index+1;		// correct for array offsets

	for(index = 0; index < MAX_INPUTS; index++)
	{
		if(CANRadioDataBuff[index]._.channelFound)
		{
			udb_pwIn[index+1] = CANRadioDataBuff[index]._.PWM;

			if(index == FAILSAFE_INPUT_CHANNEL -1)
			{
				if ( (udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
				{
					failSafePulses++ ;
				}
				else
				{
					failSafePulses = 0 ;
					udb_flags._.radio_on = 0;
				}
			}
		}
	}

};



/****************************************************************************/
// This is part of the servo and radio interface software
//
// ServoInterface source code
//	http://code.google.com/p/rc-servo-interface
//
// Copyright 2010 ServoInterface Team
// See the AUTHORS.TXT file for a list of authors of ServoInterface.
//
// ServoInterface is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ServoInterface is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with ServoInterface.  If not, see <http://www.gnu.org/licenses/>.
//
// Many parts of ServoInterface use either modified or unmodified code
// from the MatrixPilot pilot project.
// The project also contains code for modifying MatrixPilot to operate
// with ServoInterface.
// For details, credits and licenses of MatrixPilot see the AUTHORS.TXT file.
// or see this website: http://code.google.com/p/gentlenav
/****************************************************************************/
