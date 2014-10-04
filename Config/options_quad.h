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


#ifndef OPTIONS_QUAD_H
#define OPTIONS_QUAD_H


#define TAIL_LIGHT  _LATD4


//OPTIONS: check ENABLE_GAINADJ and ENABLE_FAILSAFE options.h setting
//#error("check gain adjust/failsafe mux parameters")
// make this non-zero to activate FAILSAFE_MUX_CHANNEL
#define ENABLE__FAILSAFE 0

// Select the gains to be adjusted for mode switch positions 0,1,2
#define ADJ_GAIN_0 ACRO_KP_INDEX
#define ADJ_GAIN_1 RATE_KP_INDEX
#define ADJ_GAIN_2 RATE_KD_INDEX

// make this non-zero to activate FLIGHT_MODE_CHANNEL and GAIN_CHANNEL for gain adjustment
// Flight mode will be FLIGHT_MODE_TILT, regardless of mode switch position
//FIXME: ??? must cycle UDB4 power when changing ENABLE_GAINADJ from zero to one ???
// otherwise gains stored in eeprom are all zero
#define ENABLE_GAINADJ 0

// make this non-zero to activate FLIGHT_MODE_CHANNEL for flight mode
// If 0, Flight mode will be FLIGHT_MODE_TILT, regardless of mode switch position
#define ENABLE_FLIGHTMODE 1

// flight mode to use if ENABLE_FLIGHTMODE is zero
//#define DEFAULT_FLIGHT_MODE RATE_MODE
#define DEFAULT_FLIGHT_MODE TILT_MODE

#if ((ENABLE_GAINADJ != 0) && (ENABLE_FLIGHTMODE != 0))
#error("only one of ENABLE_GAINADJ  and ENABLE_FLIGHTMODE can be selected")
#endif

// use RX channel 5 (gear) for failsafe mux on/off
#define FAILSAFE_MUX_CHANNEL    6
#define FAILSAFE_MUX_THRESH     3000

// use RX channel 6 (flight mode) to select gain for modification
#define FLIGHT_MODE_CHANNEL 5
#define FLIGHT_MODE_THRESH1 2500
//#define FLIGHT_MODE_THRESH2 3500
#define FLIGHT_MODE_THRESH2 4500

// map of flight modes to mode switch positions
#define TILT_MODE       0
#define COMPASS_MODE    1
#define POS_MODE        2
#define RATE_MODE       3
#define FLIGHT_MODE_0   TILT_MODE
#define FLIGHT_MODE_1   RATE_MODE
#define FLIGHT_MODE_2   TILT_MODE

//OPTIONS check telemetry settings
// debug telemetry is the largest set of data, output at TELEMETRY_HZ
// 80Hz is the highest rate allowable at 115.2Kbaud (OpenLog flow control mod may be necessary)
// HEARTRATE_HZ / TELEMETRY_HZ must be an integer

// standard telemetry is type 0 (was DEBUG_TELEMETRY in MatrixPilotQuad rev1174)
// standard parser is parseLogXHz.py, standard analyzer is procLogXaHz.m
// IMU telemetry is type 1, parser parseLogIMU.py, analyzer procLog_drift.m
// IMU/magnetometer is type 2, parser parseLogMag.py, analyzer procLog???
// dead reckoning is type 3, parser parseLogIMU.py, analyzer procGPS_loc.m
// PID is type 4: parser parseLogpid.py, analyzer procLogpid.m
// PID2 with gplane is type 5: parser parseLogpid2.py, analyzer procLogpid2.m
#define TELEMETRY_TYPE  5
#define TELEMETRY_HZ    100
//#define TELEMETRY_BAUD  227270
#define TELEMETRY_BAUD  115200

// if non-zero, start telemetry immediately instead of after calibration
#define TEL_ALWAYS_ON   1

// per-cell low voltage warning level
#define LVCELL 3300


//OPTIONS: check HARD_TRIMS options.h setting
//#error("HARD_TRIMS option not set")
// make this non-zero if you want the UDB to respect your TX trim settings
//#define HARD_TRIMS      1
#define HARD_TRIMS      0
// set these to the zero-trim values for your RX/TX if you use HARD_TRIMS
#define NEUTRAL_TRIM    3010
#define THROTTLE_IDLE   2250

#define ROLL_INPUT_CHANNEL  CHANNEL_1
#define PITCH_INPUT_CHANNEL CHANNEL_2
#define YAW_INPUT_CHANNEL   CHANNEL_4

// Channel numbers for each output
// Use as is, or edit to match your setup.
//   - Only assign each channel to one output purpose
//   - If you don't want to use an output channel, set it to CHANNEL_UNUSED
// 
// NOTE: If your board is powered from your ESC through the throttle cable, make sure to
// connect THROTTLE_OUTPUT_CHANNEL to one of the built-in Outputs (1, 2, or 3) to make
// sure your board gets power.
// 
//OPTIONS: check output channel mappings in options.h
//#error("output channel mappings not set")

#define MOTOR_A_OUTPUT_CHANNEL CHANNEL_3    // + front or X left front, CCW
#define MOTOR_B_OUTPUT_CHANNEL CHANNEL_4    // + right or X right front, CW
#define MOTOR_C_OUTPUT_CHANNEL CHANNEL_1    // + rear or X right rear, CCW
#define MOTOR_D_OUTPUT_CHANNEL CHANNEL_2    // + left or X left rear,	CW

////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.
//
// store PID gains in 32KB EEPROM as N 16 bit words starting at address 0x400
#define PID_GAINS_BASE_ADDR     (0X400)
#define PID_GAINS_N             10
//
// Tilt PID(DD) control gains: valid range [0,3.99]
#define TILT_KI 0.1
#define TILT_KP 0.5
#define RATE_KP 0.7
#define ACRO_KP 3.0
#define RATE_KD 0.95
#define RATE_KI 0.125
//
// Yaw PID control gains
#define YAW_KI 0.0
#define YAW_KP 1.0
#define YAW_KD 1.0
//
// Vertical damping
// ****Note*** if your ESCs work "backwards", meaning that faster speed requires shorter pulses, then flip the sign to minus
// landings seem to be much softer with this turned on
#define ACCEL_K 0.0
//


#endif // OPTIONS_QUAD_H
