// SEE END OF FILE FOR LICENSE TERMS

/****************************************************************************
CANInterface_config_x
Specific register level configuration of CAN interface

Includes 
  setup of message receive acceptance filters and masks
  Setup of baud rate, sampling points and interrupt sources
 
Uses CANInterface_config.h for shared setting such as baud rate

 ****************************************************************************/

/****************************************************************************
THIS FILE SPECIFIC TO UDB2/3 (not ECAN module)
 ****************************************************************************/

#include "libCAN_defines.h"
#include "../libCAN/ECAN1Config.h"

#include "CANRegisters.h"
#include "CANProtocol_defines.h"

void init_CAN1(void);
void config_CAN1_RX_SERVO(void);
void config_CAN1_RX_COPRO(void);
void config_CAN1_TX_SERVO(void);
void config_CAN1_TX_DATA(void);
void config_CAN1_RX_ALL(void);


void ecan1WriteMessage(void);
void rxECAN1(mID *message);

void clearCANIntrflags(void);

// CAN Messages in RAM
mID rx_ecan1message;

// Define ECAN Message Buffers
//ECAN1MSGBUF ecan1msgBuf __attribute__((space(dma),aligned(ECAN1_MSG_BUF_LENGTH*16)));
ECAN1MSGBUF ecan1msgBuf __attribute__((eds, space(dma), aligned(sizeof(ECAN1MSGBUF))));

void CAN_send(void)
{
/* Write a Message in ECAN1 Transmit Buffer
   Request Message Transmission			*/
	ecan1WriteMessage();
        //DMA1REQBITS.FORCE = 1;
//        DMA1REQ=0x8046;
	C1TR01CONbits.TXREQ0=1;
}

void CAN_init(void)
{
    init_CAN1();
    _RF1 = !_RF1;
//    CAN_send();
};



//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
//init CAN1
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------

void init_CAN1(void)
{
    _C1IF = 0;


/* ECAN1 Initialisation
   Configure DMA Channel 0 for ECAN1 Transmit
   Configure DMA Channel 2 for ECAN1 Receive */
	ecan1Init();
	CAN1TxDMAinit();
	CAN1RxDMAinit();

/* Enable ECAN1 Interrupt */

	IEC2bits.C1IE = 1;
	C1INTEbits.TBIE = 1;
	C1INTEbits.RBIE = 1;


/* Write a Message in ECAN1 Transmit Buffer
   Request Message Transmission			*/
	ecan1WriteMessage();
	C1TR01CONbits.TXREQ0=1;


/* Loop infinitely */

	while (1);

};


//-----------------------------------------------------------------------------------------------------------------------
// Configure Receive buffer 0 and filter 1 for everything
//-----------------------------------------------------------------------------------------------------------------------

void config_CAN1_RX_ALL(void)
{
/*	Filter Configuration

	ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide,unsigned int bufPnt,unsigned int maskSel)

	n = 0 to 15 -> Filter number

	identifier -> SID <10:0> : EID <17:0>

	exide = 0 -> Match messages with standard identifier addresses
	exide = 1 -> Match messages with extended identifier addresses

	bufPnt = 0 to 14  -> RX Buffer 0 to 14
	bufPnt = 15 -> RX FIFO Buffer

	maskSel = 0	->	Acceptance Mask 0 register contains mask
	maskSel = 1	->	Acceptance Mask 1 register contains mask
	maskSel = 2	->	Acceptance Mask 2 register contains mask
	maskSel = 3	->	No Mask Selection

*/

	ecan1WriteRxAcptFilter(1,0x1FFEFFFF,1,1,0);


/*	Mask Configuration

	ecan1WriteRxAcptMask(int m, long identifierMask, unsigned int mide, unsigned int exide)

	m = 0 to 2 -> Mask Number

	identifier -> SID <10:0> : EID <17:0>

	mide = 0 -> Match either standard or extended address message if filters match
	mide = 1 -> Match only message types that correpond to 'exide' bit in filter

	exide = 0 -> Match messages with standard identifier addresses
	exide = 1 -> Match messages with extended identifier addresses

*/

	ecan1WriteRxAcptMask(1,0x1FFFFFFF,1,1);
}

//
//void clearCANIntrflags(void){
///* Clear Interrupt Flags */
//
//	IFS0=0;
//	IFS1=0;
//	IFS2=0;
//	IFS3=0;
//	IFS4=0;
//}

//-----------------------------------------------------------------------------------------------------------------------
// Configure Receive buffer 0 and filter 1 for servo control
//-----------------------------------------------------------------------------------------------------------------------

void config_CAN1_RX_SERVO(void)
{
    // initializing the Receive Buffer 0
    CAN_SID_RX rx_sid;

    rx_sid.bb = 0;
    rx_sid.format.priority = CANB_PRIORITY_CRITICAL;
    rx_sid.format.source = CANB_SOURCE_SERVO_IF;
    rx_sid.format.protocol = CANB_PROTOCOL_SERVO;
    rx_sid.format.EXIDE = 1;

//    C1RX0CON = 0x0000; // Receive Buffer Status Reset
//
//    // Acceptance Mask Registers
//    C1RXM0SID = 0x1FFD; //All of SID must be correct and extended address
//    C1RXM0EIDH = 0x0000;
//    C1RXM0EIDL = 0x0000;
//
//    //Initializing of Acceptance Filters
//    C1RXF0SID = rx_sid.bb; //CAN1 Receive Acceptance Filter0 SID
//    C1RXF0EIDH = 0x0000; //CAN1 Receive Acceptace  Filter0 Extended Identifier high byte
//    C1RXF0EIDL = 0x0000; //CAN1 Receive Acceptance Filter0 Extended identifier low byte
};


//-----------------------------------------------------------------------------------------------------------------------
// Configure Receive buffer 1 for data and filter 1 for servo control
//-----------------------------------------------------------------------------------------------------------------------

void config_CAN1_RX_COPRO()
{
    // initializing the Receive Buffer 1
    CAN_SID_RX rx_sid;
    CAN_RX_SIDFILTER rx_sid_filter;

    rx_sid.bb = 0;
    rx_sid.format.priority = CANB_PRIORITY_LOW;
    rx_sid.format.source = CANB_SOURCE_COPROCESSOR;
    rx_sid.format.protocol = CANB_PROTOCOL_DATA;
    rx_sid.format.EXIDE = 1;

    // Acceptance Mask Registers
    rx_sid_filter.bb = 0;
    rx_sid_filter.format.priority = 0; // Do not filter by priority
    rx_sid_filter.format.protocol = 3; // Filter by protocol
    rx_sid_filter.format.EXIDE = 1; // Extended identifier messages only
    rx_sid_filter.format.source = 0; // Do not filter for source

//    C1RXM1SID = rx_sid_filter.bb; //0x1FFD;
//    C1RXM1EIDH = 0x0000;
//    C1RXM1EIDL = 0x0000;
//
//    //Initializing of Acceptance Filters
//    C1RXF2SID = rx_sid.bb; //CAN1 Receive Acceptance Filter2 SID
//    C1RXF2EIDH = 0x0000; //CAN1 Receive Acceptace  Filter2 Extended Identifier high byte
//    C1RXF2EIDL = 0x0000; //CAN1 Receive Acceptance Filter2 Extended identifier low byte
//
//    C1RX1CON = 0x0000; // Receive Buffer Status Reset
};


//-----------------------------------------------------------------------------------------------------------------------
// Configure Transmit Register 0 for servo control data transmission
//-----------------------------------------------------------------------------------------------------------------------

void config_CAN1_TX_SERVO(void)
{
//    CAN_SID_TX tx_sid;
//
//    tx_sid.bb = 0;
//    tx_sid.format.protocol = CANB_PROTOCOL_SERVO;
//    tx_sid.format.source = CANB_SOURCE_AUTOPILOT;
//    tx_sid.format.priority = CANB_PRIORITY_CRITICAL;
//    tx_sid.format.TXIDE = 1;
//
//    C1TX0CON = 0x0003; // High priority
//    C1TX0SID = tx_sid.bb; // SID
//    C1TX0EID = 0x0001; // EID
//    C1TX0DLC = 0x040; // 8 byte

//    CAN_tx_dma_init();
};


//-----------------------------------------------------------------------------------------------------------------------
// Configure Transmit Register 0 for flight data transmission
//-----------------------------------------------------------------------------------------------------------------------

void config_CAN1_TX_DATA(void)
{
//    CAN_SID_TX tx_sid;
//
//    tx_sid.bb = 0;
//    tx_sid.format.protocol = CANB_PROTOCOL_DATA;
//    tx_sid.format.source = CANB_SOURCE_AUTOPILOT;
//    tx_sid.format.priority = CANB_PRIORITY_LOW;
//    tx_sid.format.TXIDE = 1;
//
//    C1TX1CON = 0x0001; // Low Intermediate priority
//    C1TX1SID = tx_sid.bb; // SID
//    C1TX1EID = 0x0000; // EID
//    C1TX1DLC = 0x0040; // 8 bytes
};





/* ECAN1 buffer loaded with Identifiers and Data */
void ecan1WriteMessage(void){

/* Writing the message for Transmission
ecan1WriteTxMsgBufId(unsigned int buf, long txIdentifier, unsigned int ide, unsigned int remoteTransmit);
ecan1WriteTxMsgBufData(unsigned int buf, unsigned int dataLength, unsigned int data1, unsigned int data2, unsigned int data3, unsigned int data4);

buf -> Transmit Buffer number

txIdentifier -> SID<10:0> : EID<17:0>

ide = 0 -> Message will transmit standard identifier
ide = 1 -> Message will transmit extended identifier

remoteTransmit = 0 -> Normal message
remoteTransmit = 1 -> Message will request remote transmission

dataLength -> Data length can be from 0 to 8 bytes

data1, data2, data3, data4 -> Data words (2 bytes) each

*/

	ecan1WriteTxMsgBufId(0,0x1FFEFFFF,1,0);
	ecan1WriteTxMsgBufData(0,8,0x1111,0x2222,0x3333,0x4444);

}




/******************************************************************************
*
*    Function:			rxECAN1
*    Description:       moves the message from the DMA memory to RAM
*
*    Arguments:			*message: a pointer to the message structure in RAM
*						that will store the message.
*	 Author:            Jatinder Gharoo
*
*
******************************************************************************/
void rxECAN1(mID *message)
{
	unsigned int ide=0;
	unsigned int srr=0;
	unsigned long id=0,d;

	/*
	Standard Message Format:
	Word0 : 0bUUUx xxxx xxxx xxxx
			     |____________|||
 					SID10:0   SRR IDE(bit 0)
	Word1 : 0bUUUU xxxx xxxx xxxx
			   	   |____________|
						EID17:6
	Word2 : 0bxxxx xxx0 UUU0 xxxx
			  |_____||	     |__|
			  EID5:0 RTR   	  DLC
	word3-word6: data bytes
	word7: filter hit code bits

	Substitute Remote Request Bit
	SRR->	"0"	 Normal Message
			"1"  Message will request remote transmission

	Extended  Identifier Bit
	IDE-> 	"0"  Message will transmit standard identifier
	   		"1"  Message will transmit extended identifier

	Remote Transmission Request Bit
	RTR-> 	"0"  Message transmitted is a normal message
			"1"  Message transmitted is a remote message
	*/
	/* read word 0 to see the message type */
	ide=ecan1msgBuf[message->buffer][0] & 0x0001;
	srr=ecan1msgBuf[message->buffer][0] & 0x0002;

	/* check to see what type of message it is */
	/* message is standard identifier */
	if(ide==0)
	{
		message->id=(ecan1msgBuf[message->buffer][0] & 0x1FFC) >> 2;
		message->frame_type=CAN_FRAME_STD;
	}
	/* mesage is extended identifier */
	else
	{
		id=ecan1msgBuf[message->buffer][0] & 0x1FFC;
		message->id=id << 16;
		id=ecan1msgBuf[message->buffer][1] & 0x0FFF;
		message->id=message->id+(id << 6);
		id=(ecan1msgBuf[message->buffer][2] & 0xFC00) >> 10;
		message->id=message->id+id;
		message->frame_type=CAN_FRAME_EXT;
	}
	/* check to see what type of message it is */
	/* RTR message */
	if(srr==1)
	{
		message->message_type=CAN_MSG_RTR;
	}
	/* normal message */
	else
	{
		message->message_type=CAN_MSG_DATA;
		message->data[0]=(unsigned char)ecan1msgBuf[message->buffer][3];
		message->data[1]=(unsigned char)((ecan1msgBuf[message->buffer][3] & 0xFF00) >> 8);
		message->data[2]=(unsigned char)ecan1msgBuf[message->buffer][4];
		message->data[3]=(unsigned char)((ecan1msgBuf[message->buffer][4] & 0xFF00) >> 8);
		message->data[4]=(unsigned char)ecan1msgBuf[message->buffer][5];
		message->data[5]=(unsigned char)((ecan1msgBuf[message->buffer][5] & 0xFF00) >> 8);
		message->data[6]=(unsigned char)ecan1msgBuf[message->buffer][6];
		message->data[7]=(unsigned char)((ecan1msgBuf[message->buffer][6] & 0xFF00) >> 8);
		message->data_length=(unsigned char)(ecan1msgBuf[message->buffer][2] & 0x000F);
	}
}




void __attribute__((interrupt, no_auto_psv))_C1Interrupt(void)
{
	IFS2bits.C1IF = 0;        // clear interrupt flag
	if(C1INTFbits.TBIF)
    {
    	C1INTFbits.TBIF = 0;
    }

    if(C1INTFbits.RBIF)
    {
		// read the message
	    if(C1RXFUL1bits.RXFUL1==1)
	    {
	    	rx_ecan1message.buffer=1;
	    	C1RXFUL1bits.RXFUL1=0;
	    }
	    rxECAN1(&rx_ecan1message);
		C1INTFbits.RBIF = 0;
	}
}



//------------------------------------------------------------------------------
//    DMA interrupt handlers
//------------------------------------------------------------------------------


void __attribute__((interrupt, no_auto_psv)) _DMA3Interrupt(void)
{
   _DMA3IF = 0;          // Clear the DMA3 Interrupt Flag;
}

void __attribute__((interrupt, no_auto_psv)) _DMA4Interrupt(void)
{
   _DMA4IF = 0;          // Clear the DMA4 Interrupt Flag;
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
