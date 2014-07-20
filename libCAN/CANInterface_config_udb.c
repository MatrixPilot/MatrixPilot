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

#include "CANRegisters.h"
#include "libCAN_defines.h"
#include "CANInterface_config.h"
#include "CANProtocol_defines.h"

void init_CAN1(void);
void config_CAN1_RX_SERVO(void);
void config_CAN1_RX_COPRO(void);
void config_CAN1_TX_SERVO(void);
void config_CAN1_TX_DATA(void);



void CAN_init(void)
{
	init_CAN1();
	config_CAN1_RX_SERVO();
//	config_CAN1_RX_COPRO();
	config_CAN1_TX_SERVO();
//	config_CAN1_TX_DATA();

	 //Change to Operation Mode from Configuration Mode
	 C1CTRLbits.REQOP = 0;
	 while(C1CTRLbits.OPMODE != 0);//Wait for CAN1 mode change from Configuration Mode to normal mode 
};




//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
				//init CAN1
//-------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------
void init_CAN1(void)
{


	TRISFbits.TRISF1 = 0;				// CAN 1 TX set to output	
	TRISFbits.TRISF0 = 1;				// CAN 1 RX set to input

//--------------------------------------------------------------------------------------------------------------------
				//Initialization of CAN1 Module and Enabling of CAN1 Interrupts
//--------------------------------------------------------------------------------------------------------------------

 
 C1CTRLbits.CANCKS = 0;			// Select the CAN Master Clock . It is equal to 4Fcy here. 
								// (Fcy=4MHz)

 C1CFG1bits.SJW=00;				//Synchronized jump width time is 1 x TQ when SJW is equal to 00
 
 C1CFG1bits.BRP = BRP_VAL;		//((FCY/(2*NTQ*BITRATE))-1) 	

 C1CFG2 = CAN_TIMING_CONFIG2;	// See canbus config for settings

///Interrupt Section of CAN Peripheral

 C1INTF = 0;					//Reset all The CAN Interrupts 
 IFS1bits.C1IF = 0;  			//Reset the Interrupt Flag status register
 C1INTE = 0x000F;               //Enable interrupt sources
// C1INTE = 0x00FF;               //Enable interrupt sources
 IEC1bits.C1IE = 1;				//Enable the CAN1 Interrupt
};





//-----------------------------------------------------------------------------------------------------------------------
		// Configure Receive buffer 0 and filter 1 for servo control
//-----------------------------------------------------------------------------------------------------------------------
void config_CAN1_RX_SERVO(void)
{
 // initializing the Receive Buffer 0
 CAN_SID_RX rx_sid;

 rx_sid.bb = 0;
 rx_sid.format.priority 	= CANB_PRIORITY_CRITICAL;
 rx_sid.format.source 		= CANB_SOURCE_SERVO_IF;
 rx_sid.format.protocol		= CANB_PROTOCOL_SERVO;
 rx_sid.format.EXIDE 		= 1;

 C1RX0CON =  0x0000; 	// Receive Buffer Status Reset
 
 // Acceptance Mask Registers
 C1RXM0SID  = 0x1FFD; //All of SID must be correct and extended address
 C1RXM0EIDH = 0x0000; 
 C1RXM0EIDL = 0x0000;

//Initializing of Acceptance Filters
 C1RXF0SID 	= rx_sid.bb;	//CAN1 Receive Acceptance Filter0 SID
 C1RXF0EIDH = 0x0000; 	//CAN1 Receive Acceptace  Filter0 Extended Identifier high byte
 C1RXF0EIDL = 0x0000;	//CAN1 Receive Acceptance Filter0 Extended identifier low byte
};


//-----------------------------------------------------------------------------------------------------------------------
		// Configure Receive buffer 1 for data and filter 1 for servo control
//-----------------------------------------------------------------------------------------------------------------------
void config_CAN1_RX_COPRO()
{
 // initializing the Receive Buffer 1
 CAN_SID_RX 		rx_sid;
 CAN_RX_SIDFILTER	rx_sid_filter;

 rx_sid.bb = 0;
 rx_sid.format.priority 	= CANB_PRIORITY_LOW;
 rx_sid.format.source 		= CANB_SOURCE_COPROCESSOR;
 rx_sid.format.protocol		= CANB_PROTOCOL_DATA;
 rx_sid.format.EXIDE 		= 1;

 // Acceptance Mask Registers
 rx_sid_filter.bb				= 0;
 rx_sid_filter.format.priority 	= 0;	// Do not filter by priority
 rx_sid_filter.format.protocol 	= 3;	// Filter by protocol
 rx_sid_filter.format.EXIDE		= 1;	// Extended identifier messages only
 rx_sid_filter.format.source	= 0;	// Do not filter for source

 C1RXM1SID  = rx_sid_filter.bb;	//0x1FFD;
 C1RXM1EIDH = 0x0000;
 C1RXM1EIDL = 0x0000;

//Initializing of Acceptance Filters
 C1RXF2SID 	= rx_sid.bb;	//CAN1 Receive Acceptance Filter2 SID
 C1RXF2EIDH = 0x0000; 	//CAN1 Receive Acceptace  Filter2 Extended Identifier high byte
 C1RXF2EIDL = 0x0000;	//CAN1 Receive Acceptance Filter2 Extended identifier low byte

 C1RX1CON =  0x0000; 	// Receive Buffer Status Reset
};


//-----------------------------------------------------------------------------------------------------------------------
		// Configure Transmit Register 0 for servo control data transmission
//-----------------------------------------------------------------------------------------------------------------------
void config_CAN1_TX_SERVO(void)
{
 CAN_SID_TX tx_sid;

 tx_sid.bb = 0;
 tx_sid.format.protocol 	= CANB_PROTOCOL_SERVO;
 tx_sid.format.source 		= CANB_SOURCE_AUTOPILOT;
 tx_sid.format.priority		= CANB_PRIORITY_CRITICAL;
 tx_sid.format.TXIDE 		= 1;
  
 C1TX0CON = 0x0003;     	// High priority
 C1TX0SID = tx_sid.bb;		// SID
 C1TX0EID = 0x0001;     	// EID
 C1TX0DLC = 0x040; 			// 8 byte
};


//-----------------------------------------------------------------------------------------------------------------------
		// Configure Transmit Register 0 for flight data transmission
//-----------------------------------------------------------------------------------------------------------------------
void config_CAN1_TX_DATA(void)
{
 CAN_SID_TX tx_sid;

 tx_sid.bb = 0;
 tx_sid.format.protocol 	= CANB_PROTOCOL_DATA;
 tx_sid.format.source 		= CANB_SOURCE_AUTOPILOT;
 tx_sid.format.priority		= CANB_PRIORITY_LOW;
 tx_sid.format.TXIDE 		= 1;

 C1TX1CON = 0x0001;             // Low Intermediate priority
 C1TX1SID = tx_sid.bb;          // SID
 C1TX1EID = 0x0000;             // EID                   
 C1TX1DLC = 0x0040;				// 8 bytes
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
