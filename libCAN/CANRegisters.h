// SEE END OF FILE FOR LICENSE TERMS

#include "libCAN_defines.h"

#ifndef CAN_REGSITERS_H
#define CAN_REGSITERS_H

#if( (BOARD_TYPE == CAN_INTERFACE) || (BOARD_TYPE == CANPILOT_BOARD) )
	#define CAN_PORT	2
#else
	#define CAN_PORT	1
#endif


// Structure of TX extended identifier register
typedef struct tagCxRXEIDBITS
{
	unsigned int EID13_6	: 8;
	unsigned int EID17_14	: 4;
	unsigned int 			: 4;
} CxRXEIDBITS;

typedef union
{
	volatile unsigned int 	W;
	volatile CxRXEIDBITS	_;
} CxRXEIDbits;


#if(CAN_PORT == 1)
	#define CxTX1SIDbits 	C1TX1SIDbits
	#define CxTX1EIDbits 	C1TX1EIDbits
	#define CxTX1DLCbits 	C1TX1DLCbits
	#define CxTX1CONbits 	C1TX1CONbits
	#define CxTX1B1			C1TX1B1
	#define CxTX1B2			C1TX1B2
	#define CxTX1B3			C1TX1B3
	#define CxTX1B4			C1TX1B4

	#define CxRX1SIDbits	C1RX1SIDbits
	#define CxRX1EID		C1RX1EID
	#define CxRX1DLCbits	C1RX1DLCbits
	#define CxRX1CONbits	C1RX1CONbits
	#define CxRX1B1			C1RX1B1
	#define CxRX1B2			C1RX1B2
	#define CxRX1B3			C1RX1B3
	#define CxRX1B4			C1RX1B4


	#define CxTX0SIDbits 	C1TX0SIDbits
	#define CxTX0EIDbits 	C1TX0EIDbits
	#define CxTX0DLCbits 	C1TX0DLCbits
	#define CxTX0CONbits 	C1TX0CONbits
	#define CxTX0B1			C1TX0B1
	#define CxTX0B2			C1TX0B2
	#define CxTX0B3			C1TX0B3
	#define CxTX0B4			C1TX0B4

	#define CxRX0SIDbits	C1RX0SIDbits
	#define CxRX0EID		C1RX0EID
	#define CxRX0DLCbits	C1RX0DLCbits
	#define CxRX0CONbits	C1RX0CONbits
	#define CxRX0B1			C1RX0B1
	#define CxRX0B2			C1RX0B2
	#define CxRX0B3			C1RX0B3
	#define CxRX0B4			C1RX0B4

	#define CxIE			IEC1bits.C1IE
	#define	CxIF			IFS1bits.C1IF
	#define CxCTRLbits		C1CTRLbits
	#define CxINTFbits		C1INTFbits

#elif(CAN_PORT == 2)
	#define CxTX1SIDbits	C2TX1SIDbits
	#define CxTX1EIDbits	C2TX1EIDbits
	#define CxTX1DLCbits	C2TX1DLCbits
	#define CxTX1CONbits 	C2TX1CONbits
	#define CxTX1B1			C2TX1B1
	#define CxTX1B2			C2TX1B2
	#define CxTX1B3			C2TX1B3
	#define CxTX1B4			C2TX1B4

	#define CxRX1SIDbits	C2RX1SIDbits
	#define CxRX1EID		C2RX1EID
	#define CxRX1DLCbits	C2RX1DLCbits
	#define CxRX1CONbits	C2RX1CONbits
	#define CxRX1B1			C2RX1B1
	#define CxRX1B2			C2RX1B2
	#define CxRX1B3			C2RX1B3
	#define CxRX1B4			C2RX1B4

	#define CxTX0SIDbits	C2TX0SIDbits
	#define CxTX0EIDbits 	C2TX0EIDbits
	#define CxTX0DLCbits	C2TX0DLCbits
	#define CxTX0CONbits	C2TX0CONbits
	#define CxTX0B1			C2TX0B1
	#define CxTX0B2			C2TX0B2
	#define CxTX0B3			C2TX0B3
	#define CxTX0B4			C2TX0B4

	#define CxRX0SIDbits	C2RX0SIDbits
	#define CxRX0EID		C2RX0EID
	#define CxRX0DLCbits	C2RX0DLCbits
	#define CxRX0CONbits	C2RX0CONbits
	#define CxRX0B1			C2RX0B1
	#define CxRX0B2			C2RX0B2
	#define CxRX0B3			C2RX0B3
	#define CxRX0B4			C2RX0B4

	#define CxIE			IEC2bits.C2IE
	#define	CxIF			IFS2bits.C2IF

	#define CxCTRLbits		C2CTRLbits
	#define CxINTFbits		C2INTFbits

#endif  	// CAN PORT

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

