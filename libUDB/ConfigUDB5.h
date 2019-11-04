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

// ACCEL_RANGE must be either 4 or 8
// ACCEL_RANGE 2 will cause all sorts of trouble, do not use it

#define ACCEL_RANGE         4       // 4 g range

// note : it is possible to use other accelerometer ranges on the MPU6000
// gfm : SCALEGYRO is defined by the relation
// SCALEGYRO=GYRO_RANGE/1024/6*HEARTBEAT thus 3.255 instead of 3.0016?
#define SCALEGYRO           3.0016  // 500 degree/second range
// gfm : SCALEACCEL=5280/RMAX/ACCEL_RANGE ; 5280 is defined for GRAVITY in libUDB_defines.h
#define SCALEACCEL          1.29    // 4 g range

#define NUM_ANALOG_INPUTS   1

// External A/D channels:
#define analogInput1BUFF    3
#define analogInput2BUFF    4
#define analogInput3BUFF    5
#define analogInput4BUFF    6

#define A_VOLT_BUFF         7       // V, pin label Bat Volt
#define A_AMPS_BUFF         8       // I, pin label CS Curr
#define A_RSSI_BUFF         9       // RS, pin label RSSI

#define A_VCC_BUFF          1
#define A_5V_BUFF           2

// MPU6000 configuration
// device is rotated 90 degrees clockwise from breakout board/AUAV3
// y -> x
// x -> -y

#define xrate_MPU_channel   4
#define yrate_MPU_channel   5
#define zrate_MPU_channel   6
#define temp_MPU_channel    3
#define xaccel_MPU_channel  0
#define yaccel_MPU_channel  1
#define zaccel_MPU_channel  2

#define XRATE_SIGN          +
#define YRATE_SIGN          -
#define ZRATE_SIGN          -
#define XACCEL_SIGN         -
#define YACCEL_SIGN         +
#define ZACCEL_SIGN         +

// Max inputs and outputs
#define MAX_INPUTS          8
#define MAX_OUTPUTS         8

// LED pins
#define LED_BLUE            _LATE4
#define LED_ORANGE          _LATE3
#define LED_GREEN           _LATE2
#define LED_RED             _LATE1

// SPI SS pin definitions
#define SPI1_SS             _LATB2
#define SPI2_SS             _LATG9
#define SPI1_TRIS           _TRISB2
#define SPI2_TRIS           _TRISG9

// Input Capture pin definitions
#define IC_PIN1             _RD8
#define IC_PIN2             _RD9
#define IC_PIN3             _RD10
#define IC_PIN4             _RD11
#define IC_PIN5             _RD12
#define IC_PIN6             _RD13
#define IC_PIN7             _RD14
#define IC_PIN8             _RD15

#if (USE_LIDAR_ALTITUDE	== 1)
#define LIDAR_Trigger LATAbits.LATA4//Sortie trigger Lidar sur RA4
#endif
