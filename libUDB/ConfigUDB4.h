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
#define LED_BLUE			LATEbits.LATE4
#define LED_ORANGE			LATEbits.LATE3
#define LED_GREEN			LATEbits.LATE2
#define LED_RED				LATEbits.LATE1

// There are no hardware toggle switches on the UDB4, so use values of 0
#define HW_SWITCH_1			0
#define HW_SWITCH_2			0
#define HW_SWITCH_3			0

// Radio IN
#define TRIS_IN1	_TRISD8
#define TRIS_IN2	_TRISD9
#define TRIS_IN3	_TRISD10
#define TRIS_IN4	_TRISD11
#define TRIS_IN5	_TRISD12
#define TRIS_IN6	_TRISD13
#define TRIS_IN7	_TRISD14
#define TRIS_IN8	_TRISD15

//
// Specify what the board is capable to do
//

// PWM and PPM inputs
#define BOARD_SUPPORT_RADIO_IN 		1

// PWM outputs
#define SERVO_OUT_PIN_1			_LATD0
#define SERVO_OUT_PIN_2			_LATD1
#define SERVO_OUT_PIN_3			_LATD2
#define SERVO_OUT_PIN_4			_LATD3
#define SERVO_OUT_PIN_5			_LATD4
#define SERVO_OUT_PIN_6			_LATD5
#define SERVO_OUT_PIN_7			_LATD6
#define SERVO_OUT_PIN_8			_LATD7
#define SERVO_OUT_PIN_9			_LATA4
#define SERVO_OUT_PIN_10		_LATA1

#define TRIS_OUT1				_TRISD0
#define TRIS_OUT2				_TRISD1
#define TRIS_OUT3				_TRISD2
#define TRIS_OUT4				_TRISD3
#define TRIS_OUT5				_TRISD4
#define TRIS_OUT6				_TRISD5
#define TRIS_OUT7				_TRISD6
#define TRIS_OUT8				_TRISD7
#define TRIS_OUT9				_TRISA4
#define TRIS_OUT10				_TRISA1


#define ACTION_OUT_PIN			SERVO_OUT_PIN_9

#define SCALE_FOR_PWM_OUT(x)	(x)

// Background
#define _TTRIGGERIP _T7IP
#define _TTRIGGERIF _T7IF
#define _TTRIGGERIE _T7IE
#define _TTRIGGERIR _T7Interrupt
#define _THEARTBEATIP _T6IP
#define _THEARTBEATIF _T6IF
#define _THEARTBEATIE _T6IE
#define _THEARTBEATIR _T6Interrupt

// Serials
#define GPS_ON_UART 1
#define SERIAL_ON_UART 2

