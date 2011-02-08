// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#ifndef UDB_DEFINES_H
#define UDB_DEFINES_H


// Build for the specific board type
#define RED_BOARD		1
#define GREEN_BOARD		2
#define UDB3_BOARD		3	// Test board for Inversense Gyros
#define RUSTYS_BOARD	4	// Red board with Rusty's IXZ-500_RAD2a patch board
#define UDB4_BOARD		5
#define CAN_INTERFACE	6

// Clock configurations
#define CRYSTAL_CLOCK	1
#define FRC8X_CLOCK		2


// Include the necessary files for the current board type
#if (BOARD_TYPE == RED_BOARD)
#include "p30f4011.h"
#include "ConfigRed.h"

#elif (BOARD_TYPE == GREEN_BOARD)
#include "p30f4011.h"
#include "ConfigGreen.h"

#elif (BOARD_TYPE == UDB3_BOARD)
#include "p30f4011.h"
#include "ConfigIXZ500.h"

#elif (BOARD_TYPE == RUSTYS_BOARD)
#include "p30f4011.h"
#include "ConfigIXZ500RAD2a.h"

#elif (BOARD_TYPE == UDB4_BOARD)
#include "p33fj256gp710a.h"
#include "ConfigUDB4.h"

#elif (BOARD_TYPE == CAN_INTERFACE)
#include "p30f6010A.h"
#include "../CANInterface/ConfigCANInterface.h"
#endif


#if (HILSIM == 1)
#include "ConfigHILSIM.h"
#endif


#if (USE_PPM_INPUT == 1)
#undef MAX_INPUTS
#define MAX_INPUTS 8
#undef MAX_OUTPUTS
#define MAX_OUTPUTS 9
#endif


// define the board rotations here.
// This include must go jsut after the board type has been declared
// Do not move this
// Orientation of the board
#define ORIENTATION_FORWARDS		0
#define ORIENTATION_BACKWARDS		1
#define ORIENTATION_INVERTED		2
#define ORIENTATION_FLIPPED			3
#define ORIENTATION_ROLLCW			4
#define ORIENTATION_ROLLCW180		5

#include "boardRotation_defines.h"


#if (BOARD_TYPE == GREEN_BOARD || BOARD_TYPE == RED_BOARD || BOARD_TYPE == UDB3_BOARD || BOARD_TYPE == RUSTYS_BOARD)

#define BOARD_IS_CLASSIC_UDB		1
#define CLK_PHASES	4

#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#define FREQOSC		16000000
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#define FREQOSC		58982400
#endif

#else
#define BOARD_IS_CLASSIC_UDB		0
#define FREQOSC 	32000000
#define CLK_PHASES	2
#endif


// Types
typedef char boolean;
#define true	1
#define false	0

struct ADchannel {
	int input; // raw input
	int value; // average of the sum of inputs between report outs
	int offset;  // baseline at power up 
	long sum ; // used as an integrator
};  // variables for processing an AD channel


struct udb_flag_bits {
			unsigned int unused					: 6 ;
			unsigned int a2d_read				: 1 ;
			unsigned int radio_on				: 1 ;
			} ;


// Baud Rate Generator -- See section 19.3.1 of datasheet.
// Fcy = FREQOSC / CLK_PHASES
// UXBRG = (Fcy/(16*BaudRate))-1
// UXBRG = ((32000000/2)/(16*9600))-1
// UXBRG = 103
#define UDB_BAUD(x)		((int)((FREQOSC / CLK_PHASES) / ((long)16 * x) - 1))


// LED states
#define LED_ON		0
#define LED_OFF		1


// Channel numbers on the board, mapped to positions in the pulse width arrays.
#define CHANNEL_UNUSED	0	// udb_pwIn[0], udb_pwOut[0], etc. are not used, but allow lazy code everywhere else  :)
#define CHANNEL_1		1
#define CHANNEL_2		2
#define CHANNEL_3		3
#define CHANNEL_4		4
#define CHANNEL_5		5
#define CHANNEL_6		6
#define CHANNEL_7		7
#define CHANNEL_8		8
#define CHANNEL_9		9


// Constants
#define RMAX   0b0100000000000000	//	1.0 in 2.14 fractional format
#define GRAVITY ((long)(5280.0/SCALEACCEL))  // gravity in AtoD/2 units

#define SERVOCENTER 3000
#define SERVORANGE ((int)(SERVOSAT*1000))
#define SERVOMAX SERVOCENTER + SERVORANGE
#define SERVOMIN SERVOCENTER - SERVORANGE

extern int magMessage ;
#endif
