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


////////////////////////////////////////////////////////////////////////////////
// options.h
// Bill Premerlani's UAV Dev Board
// 
// This file includes all of the user-configuration for this firmware,
// with the exception of waypoints, which live in the waypoints.h file.
// 


////////////////////////////////////////////////////////////////////////////////
// Set Up Board Type
// GREEN_BOARD - Board is green and includes 2 vertical gyro daugter-boards.
// RED_BOARD   - Board is red, and includes 2 vertical gyro daugter-boards.
// UDB3_BOARD  - Board is red, and includes a single, flat, multi-gyro daugter-board.
// AUAV2_BOARD - AUAV2 with MPU6000
// See the MatrixPilot wiki for more details on different UDB boards.
// If building for UDB4, use the RollPitchYaw-udb4.mcp project file.
#define BOARD_TYPE 	UDB4_BOARD
#define USE_MPU 1
#undef DUAL_IMU
#define ACCEL_RANGE 2


////////////////////////////////////////////////////////////////////////////////
// Select Clock Configuration (Set to CRYSTAL_CLOCK or FRC8X_CLOCK)
// CRYSTAL_CLOCK is the 16 MHz crystal.  This is the speed used in the past, and may be
// more compatible with other add-ons. The CRYSTAL_CLOCK supports a maximum baud rate of 19200 bps.
// FRC8X_CLOCK runs the fast RC clock (7.3728 MHz) with 8X PLL multiplier, and supports much
// faster baud rates.
#define CLOCK_CONFIG 	CRYSTAL_CLOCK

// 18.484 Volts max (in millivolts)
#define MAX_VOLTAGE 18484

// per-cell low voltage warning level
#define LVCELL 3300

////////////////////////////////////////////////////////////////////////////////
// Use board orientation to change the mounting direction of the board.
// The following 4 orientations have the board parallel with the ground.
// ORIENTATION_FORWARDS:  Component-side up,   GPS connector front
// ORIENTATION_BACKWARDS: Component-side up,   GPS connector back
// ORIENTATION_INVERTED:  Component-side down, GPS connector front
// ORIENTATION_FLIPPED:   Component-side down, GPS connector back
// The following 2 orientations are "knife edge" mountings
// ORIENTATION_ROLLCW: Rick's picture #9, board rolled 90 degrees clockwise,
//		from point of view of the pilot
// ORIENTATION_ROLLCW180: Rick's pitcure #11, board rolled 90 degrees clockwise,
//		from point of view of the pilot, then rotate the board 180 around the Z axis of the plane,
//		so that the GPS connector points toward the tail of the plane
// ********** NOTE: orientations are withrespect to the front motor for + configuration,  *******
// or with respect to left front motor, for X configuration
//	rmat is the matrix of direction cosines relating
//	the body and earth coordinate systems.
//	The columns of rmat are the axis vectors of the plane,
//	as measured in the earth reference frame.
//      The UDB4 is installed on the AeroFPV frame with the X axis pointing front-left
//      and the Y axis pointing front-right when in X configuration.
//      The Z axis points downward.
#define BOARD_ORIENTATION   ORIENTATION_FORWARDS

/////////////////////////////////////////////////////////////////////////////
// Select + or X flying configuration by defining exactly one of the following

#define CONFIG_PLUS
//#define CONFIG_X


////////////////////////////////////////////////////////////////////////////////
// Set this value to your GPS type.  (Set to GPS_STD, GPS_UBX_2HZ, or GPS_UBX_4HZ)
#define GPS_TYPE    GPS_STD

// Note: As of MatrixPilot 3.0, Dead Reckoning and Wind Estimation are automatically enabled.

// Define MAG_YAW_DRIFT to be 1 to read magnetometer for yaw drift correction.
#define MAG_YAW_DRIFT 	0
// disable MAG drift correction if this is not 1
#define ENABLE_MAG_DRIFT_CORRECTION 0

// if defined, enable magnetometer offset and alignment adjustments
// 17 May: mag offsets grow when sitting still on bench, stopped test at 277,251,458
//#define ENABLE_MAGALIGNMENT
#undef ENABLE_MAGALIGNMENT

// disable GPS yaw drift correction for quad, since GPS heading is independent of yaw
#define GPS_YAW_DRIFT   0

// set this non-zero to use Bill Premerlani's new GPS-based roll_pitch_drift correction
#define NEW_RP_DRIFT  0

// Set this to 1 if you want the UAV Dev Board to fly your plane without a radio transmitter or
// receiver. (Totally autonomous.)  This is just meant for debugging.  It is not recommended that
// you actually use this since there is no automatic landing code yet, and you'd have no manual
// control to fall back on if things go wrong.  It may not even be legal in your area.
#define NORADIO	0


////////////////////////////////////////////////////////////////////////////////
// Configure Input and Output Channels

// Use a single PPM input connection from the RC receiver to the UDB on RC input channel 4.
// This frees up RC inputs 3, 2, and 1 to act as RC outputs 4, 5, and 6.
// If you're not sure, leave USE_PPM_INPUT set to 0.
// PPM_NUMBER_OF_CHANNELS is the number of channels sent on the PWM signal.  This is
// often different from the NUM_INPUTS value below, and should usually be left at 8.
// If PPM_ALT_OUTPUT_PINS is set to 0, the 9 available RC outputs will be sent to the
// following pins, in this order: Out1, Out2, Out3, In3, In2, In1, RE0, RE2, RE4.
// With it set to 1, the RC outputs will be in this alternate configuration:
// Out1, Out2, Out3, RE0, RE2, RE4, In3, In2, In1.
#define USE_PPM_INPUT		0
#define PPM_NUMBER_OF_CHANNELS	8
#define PPM_SIGNAL_INVERTED	0
#define PPM_ALT_OUTPUT_PINS	0

// NUM_INPUTS: Set to 0-8
//   1-4 enables only the first 1-4 of the 4 standard input channels
#define NUM_INPUTS	8

//OPTIONS: check input channel mappings in options.h
#error("input channel mappings not set")
#define ROLL_INPUT_CHANNEL	CHANNEL_2
#define PITCH_INPUT_CHANNEL	CHANNEL_3
#define THROTTLE_INPUT_CHANNEL  CHANNEL_1
#define YAW_INPUT_CHANNEL	CHANNEL_4

// NUM_OUTPUTS: Set to 3, 4, 5, or 6
//   3 enables only the standard 3 output channels
//   4 also enables E0 as the 4th output channel on UDB3
//   5 also enables E2 as the 5th output channel on UDB3
//   6 also enables E4 as the 6th output channel on UDB3
#define NUM_OUTPUTS	4

//OPTIONS: check HARD_TRIMS options.h setting
#error("HARD_TRIMS option not set")
// make this non-zero if you want the UDB to respect your TX trim settings
#define HARD_TRIMS      0
// set these to the zero-trim values for your RX/TX if you use HARD_TRIMS
#define NEUTRAL_TRIM    3040
#define THROTTLE_IDLE   2250

// Channel numbers for each output
// Use as is, or edit to match your setup.
//   - Only assign each channel to one output purpose
//   - If you don't want to use an output channel, set it to CHANNEL_UNUSED
// 
// NOTE: If your board is powered from your ESC through the throttle cable, make sure to
// connect THROTTLE_OUTPUT_CHANNEL to one of the built-in Outputs (1, 2, or 3) to make
// sure your board gets power.
// 
// Motor labels: A,B,C,D clockwise starting at front in + configuration
// Motor rotation directions: front and rear CCW, left and right CW (+ configuration)
// A: + front or X left front, CCW
// B: + right or X right front, CW
// C: + rear or X right rear, CCW
// D: + left or X left rear,	CW
// 
//OPTIONS: check output channel mappings in options.h
#error("output channel mappings not set")
#define MOTOR_A_OUTPUT_CHANNEL	CHANNEL_3
#define MOTOR_B_OUTPUT_CHANNEL	CHANNEL_4
#define MOTOR_C_OUTPUT_CHANNEL	CHANNEL_1
#define MOTOR_D_OUTPUT_CHANNEL	CHANNEL_2

// change this to -1 for reverse rotation of all motors
#define YAW_SIGN 1

// PWM rate for ESCs
#define ESC_HZ 400

// amount of throttle before fly-by-wire controls engage
#define THROTTLE_DEADBAND 100

// upper limit of throttle as fraction of maximum
#define THROTTLE_LIMIT  0.85

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT							1.0

////////////////////////////////////////////////////////////////////////////////
// The Failsafe Channel is the RX channel that is monitored for loss of signal
// Make sure this is set to a channel you actually have plugged into the UAV Dev Board!
// 
// For a receiver that remembers a failsafe value for when it loses the transmitter signal,
// like the Spektrum AR6100, you can program the receiver's failsafe value to a value below
// the normal low value for that channel.  Then set the FAILSAFE_INPUT_MIN value to a value
// between the receiver's programmed failsafe value and the transmitter's normal lowest
// value for that channel.  This way the firmware can detect the difference between a normal
// signal, and a lost transmitter.
//
// FAILSAFE_INPUT_MIN and _MAX define the range within which we consider the radio on.
// Normal signals should fall within about 2000 - 4000.
//OPTIONS: check failsafe parameters in options.h
#error("check failsafe parameters")
#define FAILSAFE_INPUT_CHANNEL  THROTTLE_INPUT_CHANNEL
#define FAILSAFE_INPUT_MIN	1980
#define FAILSAFE_INPUT_MAX	4500


//OPTIONS: check ENABLE_GAINADJ and ENABLE_FAILSAFE options.h setting
#error("check gain adjust/failsafe mux parameters")
// make this non-zero to activate FAILSAFE_MUX_CHANNEL
#define ENABLE__FAILSAFE 0

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

// Select the gains to be adjusted for mode switch positions 0,1,2
#define ADJ_GAIN_0 TILT_KP_INDEX
#define ADJ_GAIN_1 RATE_KP_INDEX
#define ADJ_GAIN_2 RATE_KD_INDEX

// make this non-zero to activate FLIGHT_MODE_CHANNEL and GAIN_CHANNEL for gain adjustment
// Flight mode will be DEFAULT_FLIGHT_MODE, regardless of mode switch position
//FIXME: ??? must cycle UDB4 power when changing ENABLE_GAINADJ from zero to one ???
// otherwise gains stored in eeprom are all zero
#define ENABLE_GAINADJ 0

// make this non-zero to activate FLIGHT_MODE_CHANNEL for flight mode
// If 0, Flight mode will be FLIGHT_MODE_TILT, regardless of mode switch position
#define ENABLE_FLIGHTMODE 0

// flight mode to use if ENABLE_FLIGHTMODE is zero
#define DEFAULT_FLIGHT_MODE TILT_MODE

#if ((ENABLE_GAINADJ != 0) && (ENABLE_FLIGHTMODE != 0))
#error("only one of ENABLE_GAINADJ  and ENABLE_FLIGHTMODE can be selected")
#endif

// use RX channel 5 (gear) for failsafe mux on/off
#define FAILSAFE_MUX_CHANNEL    5
#define FAILSAFE_MUX_THRESH     3000

// use RX channel 6 (flight mode) to select gain for modification
#define FLIGHT_MODE_CHANNEL 6
#define FLIGHT_MODE_THRESH1 2500
#define FLIGHT_MODE_THRESH2 3500

// map of flight modes to mode switch positions
// In tilt mode, right stick controls quadcopter tilt angle
// In compass mode, forward stick tilts to North, right stick tilts to East
// In pos mode, forward stick flies North, right stick flies East
// In rate mode right stick controls tilt rate, forward stick tilts forward
#define TILT_MODE   0
#define COMPASS_MODE    1
#define POS_MODE        2
#define RATE_MODE       3
#define FLIGHT_MODE_0   TILT_MODE
#define FLIGHT_MODE_1   RATE_MODE
#define FLIGHT_MODE_2   COMPASS_MODE

#define POS_HOLD_KP 10
#define POS_HOLD_KD 50

// use RX channel 7 as gain inc/dec (connected to UDB input 8)
// there are +/-26 steps on the DX7 hover throttle rocker and 2*790 counts of range for 30 counts/step
// PWM range of channel is 1845 to 4236
#define GAIN_CHANNEL 5
#define GAIN_INC 0.05;
#define GAIN_DELTA 6

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
#define TELEMETRY_TYPE  0
#define TELEMETRY_HZ    40
#define TELEMETRY_BAUD  115200

// if non-zero, start telemetry immediately instead of after calibration
#define TEL_ALWAYS_ON   0


//OPTIONS: check ENABLE_RPM_SENSOR options.h setting
#error("check rpm sensor parameters")
// make this non-zero to read brushless motor rpm sensor on channel input channel 7
#define ENABLE_RPM_SENSOR   0
#define MOTOR_POLES         14
#define COMFREQ_TO_RPM      (60 * 2.0 / MOTOR_POLES)

////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.
//
// store PID gains in 32KB EEPROM as N 16 bit words starting at address 0x400
#define PID_GAINS_BASE_ADDR     (0X400)
#define PID_GAINS_N             10
//
// Tilt PID(DD) control gains: valid range [0,3.99]
#define TILT_KI 0.0
#define TILT_KP 0.3
#define RATE_KP 1.25
#define ACRO_KP 2.6
#define RATE_KD 1.8
#define RATE_KI 0.2
//
// Yaw PID control gains
#define YAW_KI 0.0
#define YAW_KP 1.8
#define YAW_KD 2.0
//
// Vertical damping
// ****Note*** if your ESCs work "backwards", meaning that faster speed requires shorter pulses, then flip the sign to minus
// landings seem to be much softer with this turned on
#define ACCEL_K 0.0
//
// limiting values for roll/pitch and yaw control outputs
#define ROLLPITCH_CLAMP 300
#define YAW_CLAMP 300
//
////////////////////////////////////////////////////////////////////////////////

// currently unused
//#define MAX_YAW_RATE 51  // maximum yaw rate, degrees per second, must be between 50 and 500 degrees/second
//#define MAX_TILT 45       // maximum roll or pitch, degrees, not to exceed 45 degrees

// Specify maximum tilt angle setpoint as 45 degrees
// PWM command input range is +/-1000 counts
// tilt angle is represented as sine(angle)
#define CMD_TILT_GAIN (unsigned int) (2 * (0.707 * RMAX) / 1000)


////////////////////////////////////////////////////////////////////////////////
// Hardware In the Loop Simulation
// Only set this to 1 for testing in the simulator.  Do not try to fly with this set to 1!
// Requires setting GPS_TYPE to GPS_UBX_4HZ.
// See the MatrixPilot wiki for more info on using HILSIM.
#define HILSIM 	0
