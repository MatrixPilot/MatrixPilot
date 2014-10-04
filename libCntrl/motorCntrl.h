// This file is part of the MatrixPilotQuad firmware.
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


#ifndef MOTOR_CNTRL_H
#define MOTOR_CNTRL_H


///////////////////////////
// DON'T change these
#define TILT_KP_INDEX   0
#define RATE_KP_INDEX   1
#define RATE_KD_INDEX   2
#define TILT_KI_INDEX   3
#define YAW_KI_INDEX    4
#define YAW_KP_INDEX    5
#define YAW_KD_INDEX    6
#define ACCEL_K_INDEX   7
#define ACRO_KP_INDEX   8
#define RATE_KI_INDEX   9
///////////////////////////

// use RX channel 5 as gain inc/dec (connected to UDB input 5)
// there are +/-26 steps on the hover throttle rocker and 2*790 counts of range for 30 counts/step
// PWM range of channel is 1845 to 4236
#define GAIN_CHANNEL 5
#define GAIN_INC 0.05;

// change this to -1 for reverse rotation of all motors
#define YAW_SIGN 1

// PWM rate for ESCs
//#define ESC_HZ 400

// amount of throttle before fly-by-wire controls engage
#define THROTTLE_DEADBAND 100

// upper limit of throttle
#define THROTTLE_LIMIT  0.90

// limiting values for roll/pitch and yaw control outputs
#define ROLLPITCH_CLAMP 300
#define YAW_CLAMP 300

extern uint16_t pid_gains[];
extern boolean didCalibrate;

extern union longww primary_voltage;
extern uint16_t lowVoltageWarning;

void udb_set_dc(void);
void motorCntrl(int16_t flight_mode);


#endif // MOTOR_CNTRL_H
