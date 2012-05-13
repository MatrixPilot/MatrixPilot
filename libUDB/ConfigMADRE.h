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


// used for the MADRE

#define VbrdBUFF			1
#define TextBUFF			2

#define mbatiBUFF			3
#define mbatvBUFF			4

#define XRATE_SIGN -
#define YRATE_SIGN -
#define ZRATE_SIGN +

#define XACCEL_SIGN -
#define YACCEL_SIGN -
#define ZACCEL_SIGN -


//#define SCALEGYRO 4.95
//#define SCALEACCEL 2.64
#define SCALEGYRO 6.8383
#define SCALEACCEL 20.625

// Max inputs and outputs
#define MAX_INPUTS	0
#define MAX_OUTPUTS	5

// LED pins
#define LED_GREEN			LATAbits.LATA10
#define LED_RED				LATAbits.LATA7

// There are no hardware toggle switches on the MADRESCHEDA, so use values of 0
#define HW_SWITCH_1			0
#define HW_SWITCH_2			0
#define HW_SWITCH_3			0
