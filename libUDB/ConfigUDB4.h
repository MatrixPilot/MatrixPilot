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


// used for the UDB4

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
#define LED_BLUE   _LATE4
#define LED_ORANGE _LATE3
#define LED_GREEN  _LATE2
#define LED_RED    _LATE1

#define IC_PIN1 _RD8
#define IC_PIN2 _RD9
#define IC_PIN3 _RD10
#define IC_PIN4 _RD11
#define IC_PIN5 _RD12
#define IC_PIN6 _RD13
#define IC_PIN7 _RD14
#define IC_PIN8 _RD15
