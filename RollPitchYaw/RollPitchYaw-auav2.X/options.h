// This file is part of the MatrixPilot RollPitchYaw demo.
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
// See the MatrixPilot wiki for more details on different UDB boards.
// If building for UDB4, use the RollPitchYaw-udb4.mcp project file.
#define BOARD_TYPE AUAV2_BOARD


////////////////////////////////////////////////////////////////////////////////
// Select Clock Configuration (Set to CRYSTAL_CLOCK or FRC8X_CLOCK)
// CRYSTAL_CLOCK is the 16 MHz crystal.  This is the speed used in the past, and may be
// more compatible with other add-ons. The CRYSTAL_CLOCK supports a maximum baud rate of 19200 bps.
// FRC8X_CLOCK runs the fast RC clock (7.3728 MHz) with 8X PLL multiplier, and supports much
// faster baud rates.
#define CLOCK_CONFIG 						CRYSTAL_CLOCK

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
#define BOARD_ORIENTATION					ORIENTATION_FORWARDS

/////////////////////////////////////////////////////////////////////////////
// Select + or X flying configuration by defining exactly one of the following

#define CONFIG_PLUS
//#define CONFIG_X


////////////////////////////////////////////////////////////////////////////////
// Set this value to your GPS type.  (Set to GPS_STD, GPS_UBX_2HZ, or GPS_UBX_4HZ)
#define GPS_TYPE							GPS_STD

// Note: As of MatrixPilot 3.0, Dead Reckoning and Wind Estimation are automatically enabled.

// Define MAG_YAW_DRIFT to be 1 to read magnetometer for yaw drift correction.
#define MAG_YAW_DRIFT 						0
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
#define NORADIO	1


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
//   5 also enables E8 as the 5th input channel
#define NUM_INPUTS	0
#define THROTTLE_INPUT_CHANNEL 0

// NUM_OUTPUTS: Set to 3, 4, 5, or 6
//   3 enables only the standard 3 output channels
//   4 also enables E0 as the 4th output channel on UDB3
//   5 also enables E2 as the 5th output channel on UDB3
//   6 also enables E4 as the 6th output channel on UDB3
#define NUM_OUTPUTS	4

//OPTIONS: check HARD_TRIMS options.h setting
//#error("HARD_TRIMS option not set")
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
#define ROLL_OUTPUT_CHANNEL					CHANNEL_1
#define PITCH_OUTPUT_CHANNEL				CHANNEL_2
#define YAW_OUTPUT_CHANNEL					CHANNEL_3

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
//#error("check failsafe parameters")
#define FAILSAFE_INPUT_CHANNEL				THROTTLE_INPUT_CHANNEL
#define FAILSAFE_INPUT_MIN					1500
#define FAILSAFE_INPUT_MAX					4500


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT							1.0


////////////////////////////////////////////////////////////////////////////////
// Hardware In the Loop Simulation
// Only set this to 1 for testing in the simulator.  Do not try to fly with this set to 1!
// Requires setting GPS_TYPE to GPS_UBX_4HZ.
// See the MatrixPilot wiki for more info on using HILSIM.
#define HILSIM 								0


////////////////////////////////////////////////////////////////////////////////
// the following define is used to test the above gains and parameters.
// if you define TestGains, their functions will be enabled, even without GPS or Tx turned on.
// #define TestGains						// uncomment this line if you want to test your gains without using GPS
