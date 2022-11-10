

// the following defines select what gets sent to the logger. define one of them
//#define LOG_IMU
//#define RECORD_OFFSETS
//#define TEST_LOGGER_HZ
#define GYRO_CALIB

// the following selects a full record (x,y and z acceleration and gyro)
// or a partial record (x and z acceleration and y gyro, which is pitch rate)
// select one
#define FULL_RECORD
//define PARTIAL_RECORD

// set the logger hertz, allowable values are 1,2,4,5,10,20,25,40,50,100 or 200
#define LOGGER_HZ	10
#define SLIDE_DET_HZ	10
#define TILT_START	15
#define TILT_STOP	900

//#define UDB5_TEST_BOARD
#define SN003
#ifdef UDB5_TEST_BOARD
#define SERIAL_NUMBERD1	3
#define SERIAL_NUMBERD2	4
#define SERIAL_NUMBERD3 3
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // UDB5
#ifdef SN003
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 3
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // HIGH_RANGES
#ifdef SN002
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 2
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // HIGH_RANGES
#ifdef SN001
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 1
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // LOW_RANGES
#define TEST_LIDAR 0

#define DR_TAU 2.5

#define GPS_TYPE						GPS_NONE
#define NO_RADIO						1
#define MAG_YAW_DRIFT 						0
#define DEBUG_MAG						0

#define NUM_INPUTS	7
#define NUM_OUTPUTS	4

#ifdef SN001
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 88 )
#define YACCEL_OFFSET	( -8 )
#define ZACCEL_OFFSET	( -257 )
#define XRATE_OFFSET	( 0 )
#define YRATE_OFFSET	( 0 )
#define ZRATE_OFFSET	( 0 )
#define CALIBRATIONX	1.0140
#define CALIBRATIONY	1.0180
#define CALIBRATIONZ	1.0156
#endif //SN1

#ifdef SN002
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 161 )
#define YACCEL_OFFSET	( -103 )
#define ZACCEL_OFFSET	( 614 )
#define XRATE_OFFSET	( -48 )
#define YRATE_OFFSET	( 25 )
#define ZRATE_OFFSET	( -58 )
#define CALIBRATIONX	1.0000
#define CALIBRATIONY	1.0000
#define CALIBRATIONZ	1.0000
#endif //SN2

#ifdef SN003
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 158 )
#define YACCEL_OFFSET	( -29 )
#define ZACCEL_OFFSET	( -287 )
#define XRATE_OFFSET	( 0 )
#define YRATE_OFFSET	( 0 )
#define ZRATE_OFFSET	( 0 )
#define CALIBRATIONX	1.0000
#define CALIBRATIONY	1.0000
#define CALIBRATIONZ	1.0000
#endif //SN3

#ifdef UDB5_TEST_BOARD
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 549 )
#define YACCEL_OFFSET	( -73 )
#define ZACCEL_OFFSET	( 123 )
#define CALIBRATIONX	0.9999
#define CALIBRATIONY	0.9999
#define CALIBRATIONZ	0.9999
#define XRATE_OFFSET	( 0 )
#define YRATE_OFFSET	( 0 )
#define ZRATE_OFFSET	( 0 )
#endif // UDB5

#ifdef GYRO_CALIB
#undef LOGGER_HZ
#define LOGGER_HZ 10
#endif // GYRO_CALIB

#ifdef RECORD_OFFSETS
#undef LOGGER_HZ
#define LOGGER_HZ 10
#ifndef ALWAYS_LOG
#define ALWAYS_LOG
#endif // ALWAYS_LOG
#endif // RECORD_OFFSETS

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

