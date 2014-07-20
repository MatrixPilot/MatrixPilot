// SEE END OF FILE FOR LICENSE TERMS

// This is only for the classic boards.
// The CANbus code will not work for ECAN on dsPIC33

#include "../libUDB/libUDB_internal.h"
#include "libCAN.h"
#include "CANInterface_internal.h"
#include "CANTelemetryProtocol.h"
#include "CANInterface_Config.h"

#include <string.h>  	// for memcpy

#if(BOARD_GENERATION == GENERATION_CLASSIC)


//extern int failSafePulses = 0 ;
//extern int twentyHertzCounter 	= 0 ;

int outputNum ;						// Which servo data to send next.

inline void ResetAllCAN1Tx(void);
inline void ResetAllCAN1Rx(void);


//---------------------------------------------------------------------

unsigned int RxBytes = 0;




//-----------------------------------------------------------------------------------------------------------------------
		// CAN ISR
//-----------------------------------------------------------------------------------------------------------------------
void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
{
	interrupt_save_extended_state ;

	switch (C1CTRLbits.ICODE) 
	{
		case 7: 
			C1INTFbits.WAKIF = 0; 
			break;
		case 6:
			if(C1RX0CONbits.FILHIT0 == 0)
			{
				switch(C1RX0EID)
				{
				case 1:
			        ServoInBuff[0] = C1RX0B1; 
			        ServoInBuff[1] = C1RX0B2;  	//Move the recieve data from Buffers to InData
			        ServoInBuff[2] = C1RX0B3;	
			        ServoInBuff[3] = C1RX0B4;
					break;
				case 2:
			        ServoInBuff[4] = C1RX0B1; 
			        ServoInBuff[5] = C1RX0B2;  	//Move the recieve data from Buffers to InData
			        ServoInBuff[6] = C1RX0B3;	
			        ServoInBuff[7] = C1RX0B4;
					CAN_received_radio_inputs_callback();	
					break;
				};
			};
			C1INTFbits.RX0IF = 0;
			C1RX0CONbits.RXFUL = 0;
			break;
		case 5:
		    if(C1RX1CONbits.RXFUL==1)
		    {
				parse_received_data(&CANxRX2ProtoStruct);
			}
			C1INTFbits.RX1IF = 0; // Add code to read buffer 1
			C1RX1CONbits.RXFUL = 0;
			break;
		case 4:
			C1INTFbits.TX0IF = 0;

			if(TxMsgCount == 1)
			{
				C1TX0EID = 0x0002;	// Second servo address
				C1TX0B1 = ServoOutBuff[4];
				C1TX0B2 = ServoOutBuff[5];
				C1TX0B3 = ServoOutBuff[6];
				C1TX0B4 = ServoOutBuff[7];
				TxMsgCount++;

				C1TX0CONbits.TXREQ = 1;
			}
			else
			{
				TxMsgCount = 0;
			};
			break;
		case 3: 
			C1INTFbits.TX1IF = 0; 
			transmit_data(&CANxTX1ProtoStruct);
			break;
		case 2: 
			C1INTFbits.TX2IF = 0;
			break;
		case 1: 
			if(C1INTFbits.EWARN)
			{
				if(C1INTFbits.TXWAR)
				{
					ResetAllCAN1Tx();
					C1INTFbits.TXWAR = 0;
				}

				if(C1INTFbits.RXWAR)
				{
					ResetAllCAN1Rx();
					C1INTFbits.RXWAR = 0;
				}

				if(C1INTFbits.RXEP)
				{
					ResetAllCAN1Rx();
					C1INTFbits.RXEP = 0;
				}

				if(C1INTFbits.TXEP)
				{
					ResetAllCAN1Tx();
					C1INTFbits.TXEP = 0;
				}

				C1INTFbits.EWARN = 0;
			}
			else if(C1INTFbits.TXBO)
			{
				ResetAllCAN1Tx();
				C1INTFbits.TXBO = 0;
			}

			C1INTFbits.ERRIF = 0;

			break;
		case 0:
			if(C1INTFbits.IVRIF)
			{
				ResetAllCAN1Rx();
				C1INTFbits.IVRIF = 0;
			};


			break;
	}

	ResetAllCAN1Rx();
	interrupt_restore_extended_state ;

	IFS1bits.C1IF = 0; // Clear CAN1 interrupt flag before returning
};

inline void ResetAllCAN1Tx()
{
	C1INTFbits.TX0IF = 0;
	C1INTFbits.TX1IF = 0;
	C1INTFbits.TX2IF = 0;
};


inline void ResetAllCAN1Rx()
{
	C1INTFbits.RX0IF = 0;
	C1INTFbits.RX1IF = 0;
	C1RX0CONbits.RXFUL = 0;
	C1RX1CONbits.RXFUL = 0;
	C1INTFbits.RX0OVR = 0;
	C1INTFbits.RX1OVR = 0;
};




#endif


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
