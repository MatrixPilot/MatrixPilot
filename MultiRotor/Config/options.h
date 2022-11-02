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
//#define MAX_ALT_RATE_CONTROL 200
//#define MAX_ALT_PROP_CONTROL 50
//#define MIN_LIDAR_PULSE_THRESH 3
//#define IMU_CLIMB_RATE_DIVISOR 2
//#define IMU_ALT_DIVISOR 10
//#define TARGET_ALTITUDE 900
//#define DR_TAU 2.0


////////////////////////////////////////////////////////////////////////////////
// Set Up Board Type
// GREEN_BOARD - Board is green and includes 2 vertical gyro daugter-boards.
// RED_BOARD   - Board is red, and includes 2 vertical gyro daugter-boards.
// UDB3_BOARD  - Board is red, and includes a single, flat, multi-gyro daugter-board.
// See the MatrixPilot wiki for more details on different UDB boards.
// If building for UDB4, use the RollPitchYaw-udb4.mcp project file.
//#define BOARD_TYPE 							UDB5_BOARD


////////////////////////////////////////////////////////////////////////////////
// Select Clock Configuration (Set to CRYSTAL_CLOCK or FRC8X_CLOCK)
// CRYSTAL_CLOCK is the 16 MHz crystal.  This is the speed used in the past, and may be
// more compatible with other add-ons. The CRYSTAL_CLOCK supports a maximum baud rate of 19200 bps.
// FRC8X_CLOCK runs the fast RC clock (7.3728 MHz) with 8X PLL multiplier, and supports much
// faster baud rates.
//#define CLOCK_CONFIG 						CRYSTAL_CLOCK
//#define CLOCK_CONFIG 						FRC8X_CLOCK


#define GNSS_HDOP_REQUIRED_FOR_STARTUP      200  //  Horizontal Dilution of Precision
#define GNSS_VDOP_REQUIRED_FOR_STARTUP	    200  //  Vertical Dilution of Precision
#define GNSS_SVS_REQUIRED_FOR_STARTUP	      4  //  Number of Sattelites in View
#define CONSOLE_UART                        0
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

#define BOARD_ORIENTATION					ORIENTATION_FORWARDS


#define HIGH_RANGES
#ifdef HIGH_RANGES
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 2
#define ACCEL_RANGE         4
#define GYRO_RANGE	    1000
#endif // HIGH_RANGES
#ifdef LOW_RANGES
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 1
#define ACCEL_RANGE         2
#define GYRO_RANGE	    250
#endif // LOW_RANGES
#define TEST_LIDAR 0
#define MISSION_TIME 180
#define MAX_ALT_RATE_CONTROL 1300
#define MAX_ALT_PROP_CONTROL 1300
#define MIN_LIDAR_PULSE_THRESH 2
#define IMU_CLIMB_RATE_DIVISOR 2
#define IMU_ALT_DIVISOR 2
#define TARGET_MIN_ALTITUDE -1000
#define DR_TAU 2.5
#define MIN_THRUST 2400
#define MAX_THRUST 3100
#define WEIGHT 2800
#define MAX_CLIMB_RATE 200
#define MAX_ALT_ERROR 1000
#define COMMAND_STEP_RATE_MULTIPLIER 640
#define MIN_THROTTLE_COMMAND 3500
#define THROTTLE_CUTOUT 3100
#define FULL_OUTPUT 0
#define ROLL_TRIM -10
#define PITCH_TRIM -16
#define YAW_TRIM -35

#define THROTTLE_COMMAND_IN pwManual[THROTTLE_INPUT_CHANNEL]
#define THROTTLE_COMMAND throttle_accum._.W1 

#define GPS_TYPE						GPS_NONE
#define NO_RADIO						1
#define MAG_YAW_DRIFT 						0
#define DEBUG_MAG						0
#define TEST_ONOFF 1.0
#define TILT_KI ( 0.01 * TEST_ONOFF )
#define TILT_KP ( 0.02 * TEST_ONOFF )
#define TILT_KD ( 1.0 * TEST_ONOFF )
#define TILT_FF 0.2
#define TILT_KDD 0.0
#define YAW_KI 0.5*0.0
#define YAW_KP ( 0.3* TEST_ONOFF )
#define YAW_KD ( 3.0* TEST_ONOFF )
#define ACCEL_K 0.2*0.0
#define MAX_YAW_RATE 51  
// maximum yaw rate, degrees per second, must be between 50 and 500 degrees/second
#define MAX_TILT 45
#define USE_LIDAR 1
#define MAX_SPEED 500 // cm/sec
#define MAX_DISTANCE 1000 // cm
#define LATERAL_RATE_GAIN 32
#define ROTOR_CRAFT "draganflier, Sept. 24, 2022\r\n"
#define NUM_INPUTS	7
#if (TEST_LIDAR == 0 )
#define NUM_OUTPUTS	4
#else
#define NUM_OUTPUTS 5
#endif

#ifdef LOW_RANGES
#define RECORD_OFFSETS	    (0)
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 362 )
#define YACCEL_OFFSET	( -17 )
#define ZACCEL_OFFSET	( -792 )
#define XRATE_OFFSET	( -301 )
#define YRATE_OFFSET	( 163 )
#define ZRATE_OFFSET	( -64 )
#endif //LOW_RANGES

#ifdef HIGH_RANGES
#define RECORD_OFFSETS	    (0)
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 161 )
#define YACCEL_OFFSET	( -103 )
#define ZACCEL_OFFSET	( 614 )
#define XRATE_OFFSET	( -48 )
#define YRATE_OFFSET	( 25 )
#define ZRATE_OFFSET	( -58 )
#endif //LOW_RANGES

#define USE_PPM_INPUT						0
#define PPM_NUMBER_OF_CHANNELS				8
#define PPM_SIGNAL_INVERTED					0
#define PPM_ALT_OUTPUT_PINS					0

// NUM_INPUTS: Set to 0-5 
//   1-4 enables only the first 1-4 of the 4 standard input channels
//   5 also enables E8 as the 5th input channel

#define ROLL_INPUT_CHANNEL					CHANNEL_1
#define PITCH_INPUT_CHANNEL					CHANNEL_2
#define THROTTLE_INPUT_CHANNEL				CHANNEL_3
#define YAW_INPUT_CHANNEL					CHANNEL_4

// NUM_OUTPUTS: Set to 3, 4, 5, or 6
//   3 enables only the standard 3 output channels
//   4 also enables E0 as the 4th output channel on UDB3
//   5 also enables E2 as the 5th output channel on UDB3
//   6 also enables E4 as the 6th output channel on UDB3

// Channel numbers for each output
// Use as is, or edit to match your setup.
//   - Only assign each channel to one output purpose
//   - If you don't want to use an output channel, set it to CHANNEL_UNUSED
// 
// NOTE: If your board is powered from your ESC through the throttle cable, make sure to
// connect THROTTLE_OUTPUT_CHANNEL to one of the built-in Outputs (1, 2, or 3) to make
// sure your board gets power.
// 
#define MOTOR_A_OUTPUT_CHANNEL				CHANNEL_1		// + front or X left front, CCW
#define MOTOR_B_OUTPUT_CHANNEL				CHANNEL_2		// + right or X right front, CW
#define MOTOR_C_OUTPUT_CHANNEL				CHANNEL_3		// + rear or X right rear, CCW
#define MOTOR_D_OUTPUT_CHANNEL				CHANNEL_4		// + left or Z left rear,	CW	

// after you have read the above, delete the following line:
//#error("The motor channels have been changed to the standard assignments. Go to the options.h file and read the section on output channels.")

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
#define FAILSAFE_INPUT_CHANNEL				THROTTLE_INPUT_CHANNEL
#define FAILSAFE_INPUT_MIN					1500
#define FAILSAFE_INPUT_MAX					4500


// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT							1.0


////////////////////////////////////////////////////////////////////////////////
// Hardware In the Loop Simulation
// Only set this to 1 for testing in the simulator.  Do not try to fly with this set to 1!
// Requires setting GPS_TYPE to GPS_UBX_4HZ.
// See the MatrixPilot wiki for more info on using HILSIM.
#define HILSIM 								0
