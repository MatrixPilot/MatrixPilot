// SEE END OF FILE FOR LICENSE TERMS

#include "libCAN_defines.h"

#ifndef CAN_INTERFACE_INTERNAL_H
#define CAN_INTERFACE_INTERNAL_H


//-----------------------------------------------------------------------------------------------------------------------
			// ADDRESS AND FILTER SETUPS
//-----------------------------------------------------------------------------------------------------------------------
// Both standard (SID) and extended(EID) identifier will be used.
// The SID is used to show data source, protocol and prioroty
// The EID is used for address and/or data identifier.

// NOTE: CANbus transmits little endian identifiers.  MSB wins priority first
//  The result is that the smallest number wins since zero is dominant.
//  It is wire AND, not wire OR

// The structure to follow for the TX SID
typedef struct tagSID_TX_format
{
	unsigned int TXIDE		 		: 1 ;	// Transmit extended frame
	unsigned int SRR		 		: 1 ;	// Substitute remote request control bit
	unsigned int spare2		 		: 2 ;	// Spare bits for something, set to 0
	unsigned int source	 			: 4 ;	// Data source, used for arbitration and some filtering

	unsigned int unused		 		: 3 ;	// Unused part of register
	unsigned int spare1				: 1 ;	// Spare bit for something, set to 0
	unsigned int protocol			: 2 ;	// The protocol that the message was sent with
	unsigned int priority 			: 2 ;	// Priority of data message. 0=highest 3=lowest
} SID_TX_format;


// A union of plain old uint and TX SID format
typedef union tagCAN_SID_TX
{
	SID_TX_format 	format;
	unsigned int 	bb;
} CAN_SID_TX;


typedef struct tagEID_TX_FORMAT
{
	unsigned int EID_13_6		: 8;		// Extended identifier bits 13 downto 6
	unsigned int EID_unused		: 4;		// Unused part of register
	unsigned int EID_17_14		: 4;		// Extended identifier bits 17 downto 14
} EID_TX_FORMAT;


// A union of plain old uint and TX EID format
typedef union tagCAN_EID_TX
{
	EID_TX_FORMAT 	format;
	unsigned int 	bb;
} CAN_EID_TX;


typedef struct tagDLC_TX_FORMAT
{
	unsigned int DLC_unused1	: 3;
	unsigned int DLC_length		: 4;		// Data length
	unsigned int DLC_Reserved	: 2;		// Must be set to zero
	unsigned int DLC_RTR		: 1;		// Request remote transmit
	unsigned int EID_5_0		: 4;		// Lowest bits of identifier
} DLC_TX_FORMAT;


// The structure to follow for the RX SID
// Note EXIDE should normally be set to 1
typedef struct tagSID_RX_format
{
	unsigned int EXIDE		 		: 1 ;
	unsigned int SRR		 		: 1 ;
	unsigned int spare2		 		: 2 ;	// Spare bits for something, set to 0
	unsigned int source	 			: 4 ;	// Data source, used for arbitration and some filtering

	unsigned int spare1				: 1 ;	// Spare bit for something, set to 0
	unsigned int protocol			: 2 ;	// The protocol that the message was sent with
	unsigned int priority 			: 2 ;	// Priority of data message. 0=highest 3=lowest
	unsigned int unused		 		: 3 ;	// Unused part of register
} SID_RX_format;

// A union of plain old uint and RX SID format
typedef union tagCAN_SID_RX
{
	SID_RX_format format;
	unsigned int bb;
} CAN_SID_RX;

// Data structure for building SID acceptance filters
typedef struct tagCAN_RX_SIDFILTER_FORMAT
{
	unsigned int EXIDE		: 1;
	unsigned int			: 1;
	unsigned int spare2		: 2;
	unsigned int source		: 4;
	unsigned int spare1		: 1;
	unsigned int protocol	: 2;
	unsigned int priority	: 2;
	unsigned int 			: 3;
} CAN_RX_SIDFILTER_FORMAT;

// A union of plain old uint and RX SID acceptance filter format
typedef union tagCAN_SID_RX_FILTER
{
	CAN_RX_SIDFILTER_FORMAT format;
	unsigned int bb;
} CAN_RX_SIDFILTER;

typedef enum
{
	CANB_PROTOCOL_BROADCAST	= 0,
	CANB_PROTOCOL_SERVO,
	CANB_PROTOCOL_DATA,
	CANB_PROTOCOL_UNUSED
} CANBUS_PROTOCOLS;


// Enumeration of priority levels
// WARNING. ALWAYS USE CRITICAL FOR SERVOS. NEVER USE CRITICAL FOR TELEMETRY.
typedef enum
{
	CANB_PRIORITY_CRITICAL = 0,		// Critical control eg. servo connection
	CANB_PRIORITY_HIGH,				// An action
	CANB_PRIORITY_LOW,
	CANB_PRIORITY_LOWEST,
} CANBUS_PRIORITY;


typedef enum
{
	CANB_SOURCE_BROADCAST = 0,
	CANB_SOURCE_AUTOPILOT,
	CANB_SOURCE_SERVO_IF,
	CANB_SOURCE_SENSOR,
	CANB_SOURCE_COPROCESSOR,
	CANB_SOURCE_FLIGHT_DIRECTOR,
} CANBUS_SOURCES;

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
