#include "CANRegisters.h"
#include "libCAN_defines.h"
#include "CANInterface_config.h"
#include "CANProtocol_defines.h"
//---------------------------------------------------------------------

//
// Intilializing the receive registers to be 0

void init_CAN2(void);
void config_CAN2_RX_SERVO(void);
void config_CAN2_RX_DATA(void);
void config_CAN2_TX(void);
void config_CAN2_COPRO_TX(void);

void ResetAllCAN2Tx(void);
void ResetAllCAN2Rx(void);

void CAN_init(void)
{
	init_CAN2();

	config_CAN2_RX_SERVO();
	config_CAN2_TX();
//	config_CAN2_COPRO_TX();
//	config_CAN2_RX_DATA();

 //Change to Operation Mode from Configuration Mode
 C2CTRLbits.REQOP = 0;
 while(C2CTRLbits.OPMODE != 0);//Wait for CAN2 mode change from Configuration Mode to normal mode

};




//-------------------------------------------------------------------------------------------------------------------
				//init CAN2
//-------------------------------------------------------------------------------------------------------------------
void init_CAN2(void)
{
	TRISGbits.TRISG1 = 0;			// CAN 2 TX set to output	
	TRISGbits.TRISG0 = 1;			// CAN 2 RX set to input

	C2CTRLbits.CANCKS = 0;			// Select the CAN Master Clock . It is equal to 4Fcy here. 
									// (Fcy=4MHz)
	
	C2CFG1bits.SJW=00;			   	//Synchronized jump width time is 1 x TQ when SJW is equal to 00
	 
	C2CFG1bits.BRP = BRP_VAL;		//((FCY/(2*NTQ*BITRATE))-1)  
	
	 
	C2CFG2 = CAN_TIMING_CONFIG2;	// See canbus config for settings
	
	//CAN2 Interrupts
	C2INTF = 0;						//Reset all The CAN Interrupts 
	IFS2bits.C2IF = 0;  			//Reset the Interrupt Flag status register
	C2INTE = 0x000F;                //Enable interrupt sources
//	C2INTE = 0x00FF;                //Enable interrupt sources
	IEC2bits.C2IE = 1;				//Enable the CAN2 Interrupt 

};


//-----------------------------------------------------------------------------------------------------------------------
		// Configure recieve buffer 0 for servo 
//-----------------------------------------------------------------------------------------------------------------------
void config_CAN2_RX_SERVO(void)
{
// initializing the Receive Buffer 0 for high priority control transfers
 CAN_SID_RX rx_sid;

 rx_sid.bb = 0;
 rx_sid.format.protocol		 	= CANB_PROTOCOL_SERVO;
 rx_sid.format.priority 		= CANB_PRIORITY_CRITICAL;
 rx_sid.format.source 			= CANB_SOURCE_AUTOPILOT;
 rx_sid.format.EXIDE 			= 1;

 C2RX0CON =  0x0000; 	// Receive Status Reset

 // Acceptance Mask Registers
 C2RXM0SID  = 0x1FFD; 	//All of SID must be correct and extended address
 C2RXM0EIDH = 0x0000;	// Accept all data addresses
 C2RXM0EIDL = 0x0000;

//Filter 0 setup to receive controls from the autopilot only

 C2RXF0SID 	= rx_sid.bb;	//CAN2 Receive Acceptance Filter0 SID
 C2RXF0EIDH = 0x0000; 		//CAN2 Receive Acceptace  Filter0 Extended Identifier high byte
 C2RXF0EIDL = 0x0000;		//CAN2 Receive Acceptance Filter0 Extended identifier low byte


 C2RX1CON =  0x0000; 	// Receive Status Reset
};


//-----------------------------------------------------------------------------------------------------------------------
		// Configure recieve buffer 1 for data transfer
//-----------------------------------------------------------------------------------------------------------------------
void config_CAN2_RX_DATA()
{
 // initializing the Receive Buffer 1
 CAN_SID_RX 		rx_sid;
 CAN_RX_SIDFILTER	rx_sid_filter;

 rx_sid.bb = 0;
 rx_sid.format.priority 	= CANB_PRIORITY_LOW;
 rx_sid.format.source 		= CANB_SOURCE_AUTOPILOT;
 rx_sid.format.protocol		= CANB_PROTOCOL_DATA;
 rx_sid.format.EXIDE 		= 1;

 // Acceptance Mask Registers
 rx_sid_filter.bb				= 0;
 rx_sid_filter.format.priority 	= 0;	// Do not filter by priority
 rx_sid_filter.format.protocol 	= 3;	// Filter by protocol
 rx_sid_filter.format.EXIDE		= 1;	// Extended identifier messages only
 rx_sid_filter.format.source	= 0;	// Do not filter for source

 C2RXM1SID  = rx_sid_filter.bb;	//0x1FFD;
 C2RXM1EIDH = 0x0000;
 C2RXM1EIDL = 0x0000;

//Initializing of Acceptance Filters
 C2RXF2SID 	= rx_sid.bb;	//CAN1 Receive Acceptance Filter2 SID
 C2RXF2EIDH = 0x0000; 	//CAN1 Receive Acceptace  Filter2 Extended Identifier high byte
 C2RXF2EIDL = 0x0000;	//CAN1 Receive Acceptance Filter2 Extended identifier low byte

 C2RX1CON =  0x0000; 	// Receive Buffer Status Reset
};


//-----------------------------------------------------------------------------------------------------------------------
		// Configure Transmit buffer 0 for servo control
//-----------------------------------------------------------------------------------------------------------------------
 void config_CAN2_TX(void)
{
 CAN_SID_TX tx_sid;

 tx_sid.bb = 0;
 tx_sid.format.protocol		= CANB_PROTOCOL_SERVO;
 tx_sid.format.priority		= CANB_PRIORITY_CRITICAL;
 tx_sid.format.source 		= CANB_SOURCE_SERVO_IF;
 tx_sid.format.TXIDE 		= 1;

 C2TX0CON = 0x0003; 		// High priority
 C2TX0SID = tx_sid.bb;
 C2TX0EID = 0x0000;         // EID = 0000000000000000000 (0x00000)
 C2TX0DLC = 0x0040;			// 8 bytes to send
};


//-----------------------------------------------------------------------------------------------------------------------
		// Configure Transmit buffer 1 for data transfer from coprocessor
//-----------------------------------------------------------------------------------------------------------------------
void config_CAN2_COPRO_TX()
{
  //Configure Transmit registers
 CAN_SID_TX tx_sid;

 tx_sid.bb = 0;
 tx_sid.format.protocol		= CANB_PROTOCOL_DATA;
 tx_sid.format.priority		= CANB_PRIORITY_LOW;
 tx_sid.format.source 		= CANB_SOURCE_COPROCESSOR;
 tx_sid.format.TXIDE 		= 1;

 C2TX1CON = 0x0002;             // Low Intermediate priority
 C2TX1SID = tx_sid.bb;          // SID = 10101010101 (0x555) 
 C2TX1EID = 0x0000;             // EID = 1110000000100100011 (0x00123)                  
 C2TX1DLC = 0x0040;             // Data length set for each message
};


