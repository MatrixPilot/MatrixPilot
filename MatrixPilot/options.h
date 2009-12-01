////////////////////////////////////////////////////////////////////////////////
// options.h
// Bill Premerlani's UAV Dev Board
// 
// This file includes all of the user-configuration for this firmware,
// with the exception of waypoints, which live in the waypoints.h file.
// 


////////////////////////////////////////////////////////////////////////////////
// Set Up Board Type (Set to RED_BOARD or GREEN_BOARD)
#define BOARD_TYPE 							RED_BOARD


////////////////////////////////////////////////////////////////////////////////
// Choose your airframe type:
//    AIRFRAME_STANDARD		 	Elevator, and Ailerons and/or Rudder control
//    AIRFRAME_VTAIL			Ailerons(optional), and Elevator and Rudder as V-tail controls
//    AIRFRAME_DELTA			Aileron and Elevator as Elevons, and Rudder(optional)
// (Note that although AIRFRAME_HELI is also recognized, the code for this airframe type is not ready.)
#define AIRFRAME_TYPE						AIRFRAME_STANDARD


////////////////////////////////////////////////////////////////////////////////
// Set this value to your GPS type.  (Set to GPS_STD or GPS_UBX)
#define GPS_TYPE							GPS_STD


////////////////////////////////////////////////////////////////////////////////
// Enable/Disable core features of this firmware
//
// Roll, Pitch, and Yaw Stabilization
// Set any of these to 0 to disable the stabilization in that axis.
#define ROLL_STABILIZATION					1
#define PITCH_STABILIZATION					1
#define YAW_STABILIZATION_RUDDER			1
#define YAW_STABILIZATION_AILERON			1

// Aileron and Rudder Navigation
// Set either of these to 0 to disable use of that control surface for navigation.
#define AILERON_NAVIGATION					1
#define RUDDER_NAVIGATION					1

// Altitude Hold
// Set this to 0 to disable altitude hold.
#define USE_ALTITUDEHOLD					1

// Camera Stabilization
// To enable, set this value to 1, and assign one or more of the CAMERA_*_OUTPUT_CHANNELS below.
#define USE_CAMERA_STABILIZATION			0

// Racing Mode
// Setting this to 1 will keep the plane at full throttle while in waypoint mode
// Racing performance can be improved by disabling CROSSTRACKING in waypoints.h
#define RACING_MODE							0

// Set this to 1 if you want the UAV Dev Board to fly your plane without a radio transmitter or
// receiver. (Totally autonomous.)  This is just meant for debugging.  It is not recommended that
// you acually use this since there is no automatic landing code yet, and you'd have no manual
// control to fall back on if things go wrong.  It may not even be legal in your area.
#define NORADIO								0


////////////////////////////////////////////////////////////////////////////////
// Configure Input and Output Channels
//
// NUM_INPUTS: Set to 4 or 5 
//   4 enables only the standard 4 input channels
//   5 also enables E8 as the 5th input channel
#define NUM_INPUTS	5

// Channel numbers for each input.
// Use as is, or edit to match your setup.
//   - If you're set up to use Rudder Navigation (like MatrixNav), then you may want to swap
//     the aileron and rudder channels so that rudder is CHANNEL_1, and aileron is 5.
#define THROTTLE_INPUT_CHANNEL				CHANNEL_3
#define AILERON_INPUT_CHANNEL				CHANNEL_1
#define ELEVATOR_INPUT_CHANNEL				CHANNEL_2
#define RUDDER_INPUT_CHANNEL				CHANNEL_5
#define MODE_SWITCH_INPUT_CHANNEL			CHANNEL_4
#define CAMERA_ROLL_INPUT_CHANNEL			CHANNEL_UNUSED
#define CAMERA_PITCH_INPUT_CHANNEL			CHANNEL_UNUSED
#define CAMERA_YAW_INPUT_CHANNEL			CHANNEL_UNUSED

// NUM_OUTPUTS: Set to 3, 4, 5, or 6
//   3 enables only the standard 3 output channels
//   4 also enables E0 as the 4th output channel
//   5 also enables E2 as the 5th output channel
//   6 also enables E4 as the 6th output channel
#define NUM_OUTPUTS	5

// Channel numbers for each output
// Use as is, or edit to match your setup.
//   - Only assign each channel to one output purpose
//   - If you don't want to use an output channel, set it to CHANNEL_UNUSED
//   - If you're set up to use Rudder Navigation (like MatrixNav), then you may want to swap
//     the aileron and runner channels so that rudder is CHANNEL_1, and aileron is 5.
// 
// NOTE: If your board is powered from your ESC through the throttle cable, make sure to
// connect THROTTLE_OUTPUT_CHANNEL to one of the built-in Outputs (1, 2, or 3) to make
// sure your board gets power.
// 
#define THROTTLE_OUTPUT_CHANNEL				CHANNEL_3
#define AILERON_OUTPUT_CHANNEL				CHANNEL_1
#define ELEVATOR_OUTPUT_CHANNEL				CHANNEL_2
#define RUDDER_OUTPUT_CHANNEL				CHANNEL_4
#define AILERON_SECONDARY_OUTPUT_CHANNEL	CHANNEL_5
#define CAMERA_ROLL_OUTPUT_CHANNEL			CHANNEL_UNUSED
#define CAMERA_PITCH_OUTPUT_CHANNEL			CHANNEL_UNUSED
#define CAMERA_YAW_OUTPUT_CHANNEL			CHANNEL_UNUSED


////////////////////////////////////////////////////////////////////////////////
// Servo Reversing Configuration
// Here you can choose which reversing switches use hardware switches, and hard code the rest.
// Note that your servo reversing settings here should match what you set on your transmitter.
// For any of these that evaluate to 1 (either hardcoded or by flipping a switch on the board,
// as you define below), that servo will be sent reversed controls.
#define AILERON_CHANNEL_REVERSED			HW_SWITCH_1
#define ELEVATOR_CHANNEL_REVERSED			HW_SWITCH_2
#define RUDDER_CHANNEL_REVERSED				HW_SWITCH_3
#define AILERON_SECONDARY_CHANNEL_REVERSED	0 // Hardcoded to be unreversed, since we have only 3 switches.
#define THROTTLE_CHANNEL_REVERSED			0 // Set to 1 to hardcode a channel to be reversed
#define CAMERA_ROLL_CHANNEL_REVERSED		0
#define CAMERA_PITCH_CHANNEL_REVERSED		0
#define CAMERA_YAW_CHANNEL_REVERSED			0

// Set this to 1 if you need to switch the left and right elevon or vtail surfaces
#define ELEVON_VTAIL_SURFACES_REVERSED		0


////////////////////////////////////////////////////////////////////////////////
// Mode Switch is ideally controlled by a 3-position switch on your transmitter.
// Often the Flap channel will be controlled by a 3-position switch.
// These are the thresholds for the cutoffs between low and middle, and between middle and high.
// Normal signals should fall within about 2000 - 4000.
#define MODE_SWITCH_THRESHOLD_LOW			2600
#define MODE_SWITCH_THRESHOLD_HIGH			3400


////////////////////////////////////////////////////////////////////////////////
// The Failsafe Channel is the RX channel that is monitored for loss of signal
// Make sure this is set to a channel you actually have plugged into the UAV Dev Board!
// 
// No matter which input you use as your failsafe, make sure you are also sending
// valid servo pulses to Input channel 4 every ~20ms, or the failsafe will not work properly.
// Just making sure you have Input 4 connected to your receiver will accomplish this.
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


////////////////////////////////////////////////////////////////////////////////
// Serial Output Format (Can be SERIAL_NONE, SERIAL_DEBUG, SERIAL_ARDUSTATION, SERIAL_UDB, or SERIAL_OSD_REMZIBI)
// This determines the format of the output sent out the spare serial port.
// Note that SERIAL_OSD_REMZIBI only works with GPS_UBX.
#define SERIAL_OUTPUT_FORMAT				SERIAL_NONE


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT				1.0

// Aileron/Roll Control Gains
// ROLLKP is the proportional gain, approximately 0.25
// ROLLKD is the deriviate (gyro) gain, approximately 0.125
// YAWKP_AILERON is the proportional feedback gain for ailerons in response to yaw error
// YAWKD_AILERON is the derivative feedback gain for ailerons in reponse to yaw rotation
// AILERON_BOOST is the additional gain multiplier for the manually commanded aileron deflection
#define ROLLKP					0.25
#define ROLLKD					0.125
#define YAWKP_AILERON			0.100
#define YAWKD_AILERON			0.2
#define AILERON_BOOST			1.0

// Elevator/Pitch Control Gains
// PITCHGAIN is the pitch stabilization gain, typically around 0.125
// PITCHKD feedback gain for pitch damping, around 0.0625
// RUDDER_ELEV_MIX is the degree of elevator adjustment for rudder and banking
// AILERON_ELEV_MIX is the degree of elevator adjustment for aileron
// ELEVATOR_BOOST is the additional gain multiplier for the manually commanded elevator deflection
#define PITCHGAIN				0.150
#define PITCHKD					0.0625
#define RUDDER_ELEV_MIX			0.5
#define ROLL_ELEV_MIX			0.1
#define ELEVATOR_BOOST			0.5

// Rudder/Yaw Control Gains
// YAWKP_RUDDER is the proportional feedback gain for rudder navigation
// YAWKD_RUDDER is the yaw gyro feedback gain for the rudder in reponse to yaw rotation
// RUDDER_BOOST is the additional gain multiplier for the manually commanded rudder deflection
#define YAWKP_RUDDER			0.0625
#define YAWKD_RUDDER			0.5
#define RUDDER_BOOST			1.0

// The following section is for camera stabilization
// These values are untested and will need adjustment
#define CAMERA_ROLLKP			0.5
#define CAMERA_PITCHKP			0.5
#define CAMERA_YAWKP			0.5


////////////////////////////////////////////////////////////////////////////////
// Configure altitude hold
#define HEIGHTMIN				 25.0	// minimum target height in meters, only applies to stabilized mode
#define HEIGHTMAX				100.0	// maximum target height in meters, only applies to stabilized mode
#define HEIGHTMARGIN			 10		// full throttle until height is margin below target, min when height is margin above target
#define MINIMUMTHROTTLE			  0.35	// minimum throttle
#define MAXIMUMTHROTTLE			  1.0	// maximum throttle from 0.0 - 1.0
#define PITCHATMINTHROTTLE		  0.0	// target pitch angle in degrees at minimum throttle
#define PITCHATMAXTHROTTLE		 15.0	// target pitch angle in degrees at maximum throttle
#define PITCHATZEROTHROTTLE		  0.0	// target pitch angle in degrees while gliding


////////////////////////////////////////////////////////////////////////////////
// Return To Launch Pitch Down in degrees, a real number.
// this is the real angle in degrees that the nose of the plane will pitch downward during a return to launch.
// it is used to increase speed (and wind penetration) during a return to launch.
// set it to zero if you do not want to use this feature.
// This only takes effect when entering RTL mode, which only happens when the plane loses the transmitter signal.
#define RTL_PITCH_DOWN			0.0


////////////////////////////////////////////////////////////////////////////////
// the following define is used to test the above gains and parameters.
// if you define TestGains, their functions will be enabled, even without GPS or Tx turned on.
// #define TestGains			// uncomment this line if you want to test your gains without using GPS
