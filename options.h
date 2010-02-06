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

// Inverted flight
// Set this to 1 to enable stabilization of inverted flight.
#define STABILIZE_INVERTED_FLIGHT			0

// Hovering
// Set this to 1 to enable stabilization of hovering.
#define STABILIZE_HOVERING					0

// Wind Estimation and Navigation
// Set this to 1 to use automatic wind estimation and navigation. 
// Wind estimation is done using a mathematical model developed by William Premerlani.
// Every time the plane performs a significant turn, the plane estimates the wind.
// This facility only requires a working GPS and the UAV DevBoard. 
#define WIND_ESTIMATION						0

// Camera Stabilization
// To enable, set this value to 1, and assign one or more of the CAMERA_*_OUTPUT_CHANNELS below.
#define USE_CAMERA_STABILIZATION			0

// Racing Mode
// Setting RACING_MODE to 1 will keep the plane at a set throttle value while in waypoint mode.
// RACING_MODE_WP_THROTTLE is the throttle value to use, and should be set between 0.0 and 1.0.
// Racing performance can be improved by disabling CROSSTRACKING in waypoints.h.
#define RACING_MODE							0
#define RACING_MODE_WP_THROTTLE				1.0

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

#define OPEN_LOG							0


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT							1.0

// Aileron/Roll Control Gains
// ROLLKP is the proportional gain, approximately 0.25
// ROLLKD is the deriviate (gyro) gain, approximately 0.125
// YAWKP_AILERON is the proportional feedback gain for ailerons in response to yaw error
// YAWKD_AILERON is the derivative feedback gain for ailerons in reponse to yaw rotation
// AILERON_BOOST is the additional gain multiplier for the manually commanded aileron deflection
#define ROLLKP								0.25
#define ROLLKD								0.125
#define YAWKP_AILERON						0.100
#define YAWKD_AILERON						0.2
#define AILERON_BOOST						1.0

// Elevator/Pitch Control Gains
// PITCHGAIN is the pitch stabilization gain, typically around 0.125
// PITCHKD feedback gain for pitch damping, around 0.0625
// RUDDER_ELEV_MIX is the degree of elevator adjustment for rudder and banking
// AILERON_ELEV_MIX is the degree of elevator adjustment for aileron
// ELEVATOR_BOOST is the additional gain multiplier for the manually commanded elevator deflection
#define PITCHGAIN							0.150
#define PITCHKD								0.0625
#define RUDDER_ELEV_MIX						0.5
#define ROLL_ELEV_MIX						0.1
#define ELEVATOR_BOOST						0.5

// Neutral pitch angle of the plane (in degrees) when flying inverted
// Use this to add extra "up" elevator while the plane is inverted, to avoid losing altitude.
#define INVERTED_NEUTRAL_PITCH	 			8.0

// Rudder/Yaw Control Gains
// YAWKP_RUDDER is the proportional feedback gain for rudder navigation
// YAWKD_RUDDER is the yaw gyro feedback gain for the rudder in reponse to yaw rotation
// RUDDER_BOOST is the additional gain multiplier for the manually commanded rudder deflection
#define YAWKP_RUDDER						0.0625
#define YAWKD_RUDDER						0.5
#define RUDDER_BOOST						1.0

// Gains for Hovering
// Gains are named based on plane's frame of reference (roll means ailerons)
// HOVER_ROLLKD is the roll gyro feedback gain applied to ailerons while stabilizing a hover
// HOVER_PITCHGAIN is the pitch-proportional feedback gain applied to the elevator while stabilizing a hover
// HOVER_PITCHKD is the pitch gyro feedback gain applied to elevator while stabilizing a hover
// HOVER_PITCH_OFFSET is the neutral pitch angle for the plane (in degrees) while stabilizing a hover
// HOVER_YAWKP is the yaw-proportional feedback gain applied to the rudder while stabilizing a hover
// HOVER_YAWKD is the yaw gyro feedback gain applied to rudder while stabilizing a hover
// HOVER_YAW_OFFSET is the neutral yaw angle for the plane (in degrees) while stabilizing a hover
#define HOVER_ROLLKD						0.3
#define HOVER_PITCHGAIN						0.5
#define HOVER_PITCHKD						0.3
#define HOVER_PITCH_OFFSET				    0.0		// + leans towards top, - leans towards bottom
#define HOVER_YAWKP							0.5
#define HOVER_YAWKD							0.3
#define HOVER_YAW_OFFSET					0.0

// The following section is for camera stabilization
// These values are untested and will need adjustment
#define CAMERA_ROLLKP						0.5
#define CAMERA_PITCHKP						0.5
#define CAMERA_YAWKP						0.5


////////////////////////////////////////////////////////////////////////////////
// Configure altitude hold
// These settings are only used when USE_ALTITUDEHOLD is enabled above.

// Min and Max target heights in meters.  These only apply to stabilized mode.
#define HEIGHT_TARGET_MIN					25.0
#define HEIGHT_TARGET_MAX					100.0

// The range of altitude within which to linearly vary the throttle
// and pitch to maintain altitude.  A bigger value makes altitude hold
// smoother, and is suggested for very fast planes.
#define HEIGHT_MARGIN						10

// Use ALT_HOLD_THROTTLE_MAX when below HEIGHT_MARGIN of the target height.
// Interpolate between ALT_HOLD_THROTTLE_MAX and ALT_HOLD_THROTTLE_MIN
// when within HEIGHT_MARGIN of the target height.
// Use ALT_HOLD_THROTTLE_MIN when above HEIGHT_MARGIN of the target height.
// Throttle values are from 0.0 - 1.0.
#define ALT_HOLD_THROTTLE_MIN				0.35
#define ALT_HOLD_THROTTLE_MAX				1.0

// Use ALT_HOLD_PITCH_MAX when below HEIGHT_MARGIN of the target height.
// Interpolate between ALT_HOLD_PITCH_MAX and ALT_HOLD_PITCH_MIN when
// within HEIGHT_MARGIN of the target height.
// Use ALT_HOLD_PITCH_HIGH when above HEIGHT_MARGIN of the target height.
// Pitch values are in degrees.  Negative values pitch the plane down.
#define ALT_HOLD_PITCH_MIN					 0.0
#define ALT_HOLD_PITCH_MAX					15.0
#define ALT_HOLD_PITCH_HIGH					 0.0


////////////////////////////////////////////////////////////////////////////////
// Return To Launch Pitch Down in degrees, a real number.
// this is the real angle in degrees that the nose of the plane will pitch downward during a return to launch.
// it is used to increase speed (and wind penetration) during a return to launch.
// set it to zero if you do not want to use this feature.
// This only takes effect when entering RTL mode, which only happens when the plane loses the transmitter signal.
#define RTL_PITCH_DOWN						0.0


////////////////////////////////////////////////////////////////////////////////
// Waypoint handling
//
// The Waypoint definitions and options are located in the waypoints.h file.


////////////////////////////////////////////////////////////////////////////////
// the following define is used to test the above gains and parameters.
// if you define TestGains, their functions will be enabled, even without GPS or Tx turned on.
// #define TestGains						// uncomment this line if you want to test your gains without using GPS
