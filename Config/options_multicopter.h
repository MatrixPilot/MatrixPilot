// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2017 MatrixPilot Team
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


////////////////////////////////////////////////////////////////////////////////
// options_multicopter.h
// Bill Premerlani's UAV Dev Board
// gfm
// This file includes configuration for multicopter adaptations of the user-configuration for this firmware.
////modif gfm quadcopter
/////////////////////////////////////////////////////////////////////////////
// Specify tricopter, quadcopter, hexacopter, octocopter
#define NUM_ROTORS 4

// Select X flying configuration by uncommenting the following
#define CONFIG_X 1

//OPTIONS: check HARD_TRIMS options.h setting
//#error("HARD_TRIMS option not set")
// make this non-zero if you want the UDB to respect your TX trim settings
#define HARD_TRIMS      0
// set these to the zero-trim values for your RX/TX if you use HARD_TRIMS
#define NEUTRAL_TRIM    3000
#define THROTTLE_IDLE   2200

// Channel numbers for each output
// Use as is, or edit to match your setup.
//   - Only assign each channel to one output purpose
//   - If you don't want to use an output channel, set it to CHANNEL_UNUSED
//
//OPTIONS: check output channel mappings in options.h
//#error("output channel mappings not set")
#define MOTOR_A_OUTPUT_CHANNEL	CHANNEL_1		// + front or X left front, CCW
#define MOTOR_B_OUTPUT_CHANNEL	CHANNEL_2		// + right or X right front, CW
#define MOTOR_C_OUTPUT_CHANNEL	CHANNEL_3		// + rear or X right rear, CCW
#define MOTOR_D_OUTPUT_CHANNEL	CHANNEL_4		// + left or X left rear,   CW

// change this to -1 for reverse rotation of all motors
#define YAW_SIGN 1

// If set to 1, pwm rate is reduced, useful when output x is a servo while putput y is a fast esc (tricopter case)
#define CHANNEL_1_REDUCED_RATE 0
#define CHANNEL_2_REDUCED_RATE 0
#define CHANNEL_3_REDUCED_RATE 0
#define CHANNEL_4_REDUCED_RATE 0
#define CHANNEL_5_REDUCED_RATE 0
#define CHANNEL_6_REDUCED_RATE 0
// Define how many pwm pulses we have to skip
#define REDUCED_RATE_SKIPS 7

// amount of throttle before fly-by-wire controls engage
#define THROTTLE_DEADBAND 100

// upper limit of throttle
#define THROTTLE_LIMIT  0.90

// make this non-zero to activate CONTROL_MODE_CHANNEL for control mode
// If 0, control mode will be MODE_TILT, regardless of mode switch position
#define ENABLE_CTRL_MODE 1

// control mode to use if ENABLE_CTRLMODE is zero
#define DEFAULT_CTRL_MODE TILT_MODE

// map of control modes to mode switch positions
#define TILT_MODE              0
#define RATE_MODE            1
#define COMPASS_MODE    2
//#define POS_MODE            3
#define CTRL_MODE_0   TILT_MODE
#define CTRL_MODE_1   RATE_MODE
#define CTRL_MODE_2   COMPASS_MODE

// make this non-zero to activate AUTOPILOT_MODE_CHANNEL for autopilot mode
// If 0, autopilot mode will be MANUAL_MODE, regardless of mode switch position
#define ENABLE_AUTO_MODE 1

// map of flight modes to mode switch positions
#define MANUAL_MODE       0
#define AUTO_MODE           1
#define HOME_MODE           2
//#define POS_MODE        3
#define AUTOPILOT_MODE_0   MANUAL_MODE
#define AUTOPILOT_MODE_1   AUTO_MODE
#define AUTOPILOT_MODE_2   HOME_MODE
// autopilot mode to use if ENABLE_AUTOPILOTMODE is zero
#define DEFAULT_AUTOPILOT_MODE MANUAL_MODE



//OPTIONS: check ENABLE_RPM_SENSOR options.h setting
//#error("check rpm sensor parameters")
// make this non-zero to read brushless motor rpm sensor on channel input channel 7
#define ENABLE_RPM_SENSOR   0
#define MOTOR_POLES                14
#define COMFREQ_TO_RPM      (60 * 2.0 / MOTOR_POLES)

// limiting values for roll/pitch and yaw control outputs
#define RPY_CLAMP    300
// limiting values for roll/pitch and yaw control outputs
#define Thrust_CLAMP 150 // gfm thrust clamp
#define RP_CLAMP      300 
#define YAW_CLAMP   700

// gfm #define CMD_TILT_GAIN (unsigned int) (16384 / 1250)
#define CMD_TILT_GAIN (unsigned int) 1


// fin modif gfm quadcopter
