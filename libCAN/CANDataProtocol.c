// SEE END OF FILE FOR LICENSE TERMS

/****************************************************************************
CANDataProtocol

Resposible for tranlating data packed in CAN messages into data useable
by CANDataManager.c

****************************************************************************/

#include "CANRegisters.h"
#include "libCAN.h"
#include "CANDataProtocol.h"

#include <string.h>

// A pointer to the data to send
unsigned int* 			ptxBuff					= NULL;

// An index into the next part of the data block to be sent
// index is in sizeof(uint)
unsigned char  			can_tx_bufferIndex		= 0;

// Maximum size of the transmitting data block
unsigned char  			can_tx_maxSize			= 0;

// The type of the data being sent
unsigned char  			can_tx_type 			= CONTROL_DATA_NONE;


// Copies the next required data chunk from the block to the CAN registers.
// Completes data length and data type in the CAN registers.
// Reqeusts the next data transmission
void send_next_data(void);


// Parse received CAN data so that the data manager can understand it.
void CAN_parse_received_data(void)
{
	CxRXEIDbits rxBits;
	rxBits.W = CxRX0EID;

	unsigned char type  			= rxBits._.EID17_14;  		// Find the control data type
	unsigned char rxBufferIndex  	= rxBits._.EID13_6;			// Find the index into the data array
	unsigned char dataLength 		= CxRX0DLCbits.DLC;			// Find the size of data sent (in bytes)

	CAN_received_data(type, rxBufferIndex, dataLength, (unsigned char*) &CxRX0B1);
}


// Manages request for a data send.  If data send is in progress it returns immediately
// The data send will then be managed by the CAN tx end interrupt calling send_next_data
void CAN_request_send_next_data(void)
{
	// If CAN is not enabled yet, return immediately
	if(CxIE == 0) return;

	// Disable the CAN interrupt so that lower priority threads can run in safety
	CxIE = 0;

	// Check hardware flag for running transmission
	if(CxTX0CONbits.TXREQ == 1)
	{
		CxIE = 1;	//re-enable the CAN interrupt and return
		return;
	};

	if(ptxBuff == NULL)
	{
		CAN_get_next_tx_data_block();
	}

	if(ptxBuff != NULL)
	{
		send_next_data();
	}
	
	CxIE = 1;		// re-enable the CAN interrupt
}



// Copies the next required data chunk from the block to the CAN registers.
// Completes data length and data type in the CAN registers.
// Reqeusts the next data transmission
void send_next_data(void)
{
	unsigned char dataLength = can_tx_maxSize - can_tx_bufferIndex;

	if(dataLength > 4)
	{
		dataLength = 4;
	}

	CxTX0EIDbits.EID17_14 	= can_tx_type;
	CxTX0EIDbits.EID13_6	= can_tx_bufferIndex;
	CxTX0DLCbits.DLC		= dataLength << 1;

	CxTX0B1 = ptxBuff[can_tx_bufferIndex];
	can_tx_bufferIndex++;
	CxTX0B2 = ptxBuff[can_tx_bufferIndex];
	can_tx_bufferIndex++;
	CxTX0B3 = ptxBuff[can_tx_bufferIndex];
	can_tx_bufferIndex++;
	CxTX0B4 = ptxBuff[can_tx_bufferIndex];
	can_tx_bufferIndex++;

	// If the end of the buffer is reached, reset it for the next transmission.
	if(can_tx_bufferIndex >= can_tx_maxSize)
	{
		ptxBuff = NULL;
		can_tx_bufferIndex = 0;
	}

	CxTX0CONbits.TXREQ = 1;
}



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
