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

#define ACCEL_RANGE 4    // 4 g range

// note : it is possible to use other accelerometer ranges on the MPU6000

#define SCALEGYRO 3.0016 // 500 degree/second range
#define SCALEACCEL 1.29  // 4 g range

// A/D channels:

#define A_VCC_BUFF			1
#define A_5V_BUFF			2

#define analogInput1BUFF	3
#define analogInput2BUFF	4
#define analogInput3BUFF	5
#define analogInput4BUFF	6

// MPU6000 configuration
// device is rotated 90 degrees clockwise from breakout board/AUAV3
// y -> x
// x -> -y

#define	xrate_MPU_channel	4
#define	yrate_MPU_channel	5
#define	zrate_MPU_channel	6

#define temp_MPU_channel    3

#define	xaccel_MPU_channel			0
#define	yaccel_MPU_channel			1
#define	zaccel_MPU_channel			2

#define XRATE_SIGN +
#define YRATE_SIGN -
#define ZRATE_SIGN -

#define XACCEL_SIGN -
#define YACCEL_SIGN +
#define ZACCEL_SIGN +


// Max inputs and outputs
#define MAX_INPUTS	8
#define MAX_OUTPUTS	8

// LED pins
#define LED_BLUE			LATEbits.LATE4
#define LED_ORANGE			LATEbits.LATE3
#define LED_GREEN			LATEbits.LATE2
#define LED_RED				LATEbits.LATE1

// There are no hardware toggle switches on the UDB5, so use values of 0
#define HW_SWITCH_1			0
#define HW_SWITCH_2			0
#define HW_SWITCH_3			0

// SPI SS pin definitions

#define SPI1_SS   _LATB2
#define SPI2_SS	  _LATG9

#define SPI1_TRIS	_TRISB2
#define SPI2_TRIS	_TRISG9

