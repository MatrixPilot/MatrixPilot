// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
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


// used for the UDB5

#define	xrateBUFF			2
#define	yrateBUFF			1
#define	zrateBUFF			3

#define	xaccelBUFF			5
#define	yaccelBUFF			6
#define	zaccelBUFF			4

#define analogInput1BUFF	7
#define analogInput2BUFF	8
#define analogInput3BUFF	9
#define analogInput4BUFF	10

#define XRATE_SIGN -
#define YRATE_SIGN -
#define ZRATE_SIGN -

#define XACCEL_SIGN -
#define YACCEL_SIGN -
#define ZACCEL_SIGN +

//#define VREF

#define SCALEGYRO 4.95
#define SCALEACCEL 2.64

// Max inputs and outputs
#define MAX_INPUTS	8
#define MAX_OUTPUTS	8

// LED pins
#define LED_BLUE			LATBbits.LATB1
#define LED_ORANGE			LATBbits.LATB4
#define LED_GREEN			LATBbits.LATB0
#define LED_RED				LATBbits.LATB3

// There are no hardware toggle switches on the UDB4, so use values of 0
#define HW_SWITCH_1			0
#define HW_SWITCH_2			0
#define HW_SWITCH_3			0


// LED 1 -> Green (RB0)
// LED 2 -> Blue  (RB1)
// LED 3 -> Red   (RB3)
// LED 4 -> Yellow   (RB4)
