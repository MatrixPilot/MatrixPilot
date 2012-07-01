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
#define ZRATE_SIGN -

#define XACCEL_SIGN -
#define YACCEL_SIGN -
#define ZACCEL_SIGN -


//#define SCALEGYRO 4.95
//#define SCALEACCEL 2.64
#define SCALEGYRO 6.8383/4
#define SCALEACCEL 20.625

// Max inputs and outputs
#define MAX_INPUTS	6
#define MAX_OUTPUTS	5

// LED pins
#define LED_GREEN			LATAbits.LATA10
#define LED_RED				LATAbits.LATA7

// There are no hardware toggle switches on the MADRESCHEDA, so use values of 0
#define HW_SWITCH_1			0
#define HW_SWITCH_2			0
#define HW_SWITCH_3			0

// Radio IN
#define TRIS_IN1	_TRISB7

//
// Specify what the board is capable to do
//

// PWM and PPM inputs
#define BOARD_SUPPORT_RADIO_IN 		1

// PWM outputs
#define SERVO_OUT_PIN_1			_LATA8
#define SERVO_OUT_PIN_2			_LATB4
#define SERVO_OUT_PIN_3			_LATA4
#define SERVO_OUT_PIN_4			_LATA9
#define SERVO_OUT_PIN_5			_LATC3

#define TRIS_OUT1				_TRISA8
#define TRIS_OUT2				_TRISB4
#define TRIS_OUT3				_TRISA4
#define TRIS_OUT4				_TRISA9
#define TRIS_OUT5				_TRISC3

#define ACTION_OUT_PIN			SERVO_OUT_PIN_3

#define SCALE_FOR_PWM_OUT(x)	(x)

// Background
#define _TTRIGGERIP _DCIIP
#define _TTRIGGERIF _DCIIF
#define _TTRIGGERIE _DCIIE
#define _TTRIGGERIR _DCIInterrupt
#define _THEARTBEATIP _SPI2IP
#define _THEARTBEATIF _SPI2IF
#define _THEARTBEATIE _SPI2IE
#define _THEARTBEATIR _SPI2Interrupt

// Serials
#define GPS_ON_UART 2
#define SERIAL_ON_UART 1

