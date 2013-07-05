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


// used for the AUAV2 with dsPIC33EP512MU810

#define analogInput1BUFF    1

#define XRATE_SIGN          +
#define YRATE_SIGN          -
#define ZRATE_SIGN          -
#define XACCEL_SIGN         -
#define YACCEL_SIGN         +
#define ZACCEL_SIGN         +

// with MPU6000 gyros set to 500 deg/sec range sensitivity is 65.535 LSB/deg/sec
// i.e. full scale is +/-500 deg/sec = +/-32768
// This sensitivity is divided by two in removing the offset: 32.768 LSB/(deg/sec)
// we need DEGPERSEC = 98.3/SCALEGYRO = 32.768, therefore
#define SCALEGYRO           (2.999)

#if ACCEL_RANGE == 8
// gravity is reported as 4096 counts by MPU6000
// this is divided by 2 in removing the offset: 2048 counts
// we need GRAVITY = 5280/SCALEACCEL = 2048 (8G range)
#define SCALEACCEL          (2.578)
#elif ACCEL_RANGE == 4
// we need GRAVITY = 5280/SCALEACCEL = 4096 (4G range)
#define SCALEACCEL          (2.578 / 2)
#elif ACCEL_RANGE == 2
// we need GRAVITY = 5280/SCALEACCEL = 8192 (2G range)
#define SCALEACCEL          (2.578 / 4)
#else
#error Unsupported ACCEL_RANGE
#endif

// Max inputs and outputs
#define MAX_INPUTS          8
#define MAX_OUTPUTS         8

// LED pins (inverted)
#define LED_GREEN           _LATB0
#define LED_BLUE            _LATB1
#define LED_RED             _LATB3
#define LED_YELLOW          _LATB4

// tail light output (inverted)
#define TAIL_LIGHT          _LATE0

// There are no hardware toggle switches on the AUAV2, so use values of 0
#define HW_SWITCH_1         0
#define HW_SWITCH_2         0
#define HW_SWITCH_3         0
