// SEE END OF FILE FOR LICENSE TERMS

/****************************************************************************
CANInterfaceIO

CAN Interface drivers for microchip CAN module (not dsPIC33 ECAN)
This drivers is common to all dsPIC30 nodes

Use non port specific CAN registers definitions from CANRegisters.h
CANRegsiters.h changes all CAN registers depending on the CAN port to be used

Basic implementation of CAN port ISR and supporting functions.
****************************************************************************/

#include "libCAN.h"
#include "libCAN_defines.h"
#include "CANRegisters.h"
#include "CANInterface_config.h"
#include "CANDataProtocol.h"

unsigned int RxTimeouts = 0;

//---------------------------------------------------------------------
void ResetAllCANTx(void);
void ResetAllCANRx(void);

//-----------------------------------------------------------------------------------------------------------------------
		// CAN ISR
//-----------------------------------------------------------------------------------------------------------------------
// Choose ISR dependant on port register settings
#if(CAN_PORT == 1)
void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
#elif(CAN_PORT == 2)
void __attribute__((interrupt, no_auto_psv)) _C2Interrupt(void)
#endif
{
	interrupt_save_extended_state ;

	switch (CxCTRLbits.ICODE) 
	{
		case 7: 
			CxINTFbits.WAKIF = 0; 
			break;
		case 6:
			// read the message 
		    if(CxRX0CONbits.RXFUL==1)
		    {
				if(CxRX0CONbits.FILHIT0 == 0)
				{
					CAN_parse_received_data();
				};
		    }
			CxINTFbits.RX0IF = 0;
			CxRX0CONbits.RXFUL = 0;
			break;
		case 5:
		    if(CxRX1CONbits.RXFUL==1)
		    {
//				parse_received_telemetry();
			}
			CxINTFbits.RX1IF = 0;
			CxRX1CONbits.RXFUL = 0;
			break;
		case 4:
			CAN_request_send_next_data();
			CxINTFbits.TX0IF = 0;
			break;
		case 3: 
			CxINTFbits.TX1IF = 0; 
			//transmit_telemetry();
			break;
		case 2: 
			CxINTFbits.TX2IF = 0;
			break;
		case 1:
			if(CxINTFbits.EWARN)
			{
				if(CxINTFbits.TXWAR)
				{
					ResetAllCANTx();
					CxINTFbits.TXWAR = 0;
				}

				if(CxINTFbits.RXWAR)
				{
					ResetAllCANRx();
					CxINTFbits.RXWAR = 0;
				}

				if(CxINTFbits.RXEP)
				{
					ResetAllCANRx();
					CxINTFbits.RXEP = 0;
				}

				if(CxINTFbits.TXEP)
				{
					ResetAllCANTx();
					CxINTFbits.TXEP = 0;
				}

				CxINTFbits.EWARN = 0;
			}
			else if(CxINTFbits.TXBO)
			{
				ResetAllCANTx();
				CxINTFbits.TXBO = 0;
			}
			CxINTFbits.ERRIF = 0; // Add error handler code
		break;
		case 0:
			if(CxINTFbits.IVRIF)
			{
				ResetAllCANRx();
				CxINTFbits.IVRIF = 0;
			};
			break;
	}

	ResetAllCANRx();
	interrupt_restore_extended_state ;

	CxIF = 0; // Clear CAN1 interrupt flag before returning
}



void ResetAllCANTx()
{
	CxINTFbits.TX0IF = 0;
	CxINTFbits.TX1IF = 0;
	CxINTFbits.TX2IF = 0;
};

void ResetAllCANRx()
{
	CxINTFbits.RX0IF = 0;
	CxINTFbits.RX1IF = 0;
	CxRX0CONbits.RXFUL = 0;
	CxRX1CONbits.RXFUL = 0;
	CxINTFbits.RX0OVR = 0;
	CxINTFbits.RX1OVR = 0;
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
