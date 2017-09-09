// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2016 MatrixPilot Team
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
// This file includes the main user-configuration for this firmware.
// Once an option is enabled, it may require further options, and these
// are often in a further more detail options file. For example if you enable
// mavlink, then you may also want to review the file options_mavlink.h. 
// Autonomous flight plans are either specified in flightplan-waypoints.h, which is 
// a simple list of waypoints to follow, or in flightplan-logo.h which provides  
// an interpreted language with more powerful features for dynamic flight planning.


////////////////////////////////////////////////////////////////////////////////
// Use board orientation to change the mounting direction of the board.
// Note:
//      For UDB4, X arrow points to the front, GPS connectors are on the front.
//      For AUAV3, airplane symbol points to the front, GPS connector is at rear.
//
// The following 6 orientations have the board parallel with the ground.
// ORIENTATION_FORWARDS:  Component-side up,   GPS connector front
// ORIENTATION_BACKWARDS: Component-side up,   GPS connector back
// ORIENTATION_INVERTED:  Component-side down, GPS connector front
// ORIENTATION_FLIPPED:   Component-side down, GPS connector back
// ORIENTATION_YAWCW:     Component-side up,   GPS connector to the right
// ORIENTATION_YAWCCW:    Component-side up,   GPS connector to the left
//
// The following 2 orientations are "knife edge" mountings
// ORIENTATION_ROLLCW: board rolled 90 degrees clockwise,
//        from point of view of the pilot
// ORIENTATION_ROLLCW180: board rolled 90 degrees clockwise,
//        from point of view of the pilot, then rotate the board 180 around the Z axis of the plane,
//        so that the GPS connector points toward the tail of the plane
#define BOARD_ORIENTATION                   ORIENTATION_BACKWARDS


////////////////////////////////////////////////////////////////////////////////
// Choose your airframe type:
//    AIRFRAME_STANDARD         Elevator, and Ailerons and/or Rudder control
//    AIRFRAME_VTAIL            Ailerons(optional), and Elevator and Rudder as V-tail controls
//    AIRFRAME_DELTA            Aileron and Elevator as Elevons, and Rudder(optional)
//    AIRFRAME_HELI             Not currently supported
//    AIRFRAME_QUAD             Under development
//    AIRFRAME_GLIDER           Under development. Elevator, Flaps, Ailerons and/or Rudder control, motor optional 
// (Note that although AIRFRAME_HELI is also recognized, the code for this airframe type is not ready.)
#ifndef AIRFRAME_TYPE
#define AIRFRAME_TYPE                       AIRFRAME_GLIDER
#endif


////////////////////////////////////////////////////////////////////////////////
// Set this value to your GPS type.  (Set to GPS_STD, GPS_UBX_2HZ, GPS_UBX_4HZ, GPS_MTEK, GPS_NMEA, or GPS_NONE)
#define GPS_TYPE                            GPS_UBX_4HZ
//#define DEFAULT_GPS_BAUD                    57600   // added for GPS_NMEA support

////////////////////////////////////////////////////////////////////////////////
// You can specify a level of good GNSS reception before MatrixPilot accepts "GPS ACQUIRED".
// You can generally leaves these lines at their default values. A value of zero switches off the check.
// The VDOP parameter is only available for Ublox GNSS devices. It is ignored for other GNSS units.
// The metrics are not used by HILSIM or SILSIM.

#define GNSS_HDOP_REQUIRED_FOR_STARTUP       20  //  Horizontal Dilution of Precision
#define GNSS_VDOP_REQUIRED_FOR_STARTUP	     60  //  Vertical Dilution of Precision
#define GNSS_SVS_REQUIRED_FOR_STARTUP	      6  //  Number of Sattelites in View

////////////////////////////////////////////////////////////////////////////////
// Enable/Disable core features of this firmware
//
// Roll, Pitch, and Yaw Stabilization
// Set any of these to 0 to disable the stabilization in that axis.
#define ROLL_STABILIZATION_AILERONS         1
#define ROLL_STABILIZATION_RUDDER           1
#define PITCH_STABILIZATION                 1
#define YAW_STABILIZATION_RUDDER            1
#define YAW_STABILIZATION_AILERON           1

// Aileron and Rudder Navigation
// Set either of these to 1 to enable helical turn control for navigation.
#define AILERON_NAVIGATION                  1
#define RUDDER_NAVIGATION                   1

// Cross track margin, in meters
// This is used when the cross track option is attached to a waypoint
// It defines the amount of cross track error at which the cross tracking
// bearing adjustment saturates at 45 degree. You can also think of it
// as the reciprocal of the cross tracking gain.
// A larger value of cross track margin is more stable, a smaller one
// holds the cross track error to smaller values.
// 64 meters is probably the largest value you might use on a fast model jet (more than 50 meters/sec)
// Use 32 meters for 20 to 50 meters/sec, and 16 meters for less than that.
#define CROSS_TRACK_MARGIN                  32

// Wind Gain Adjustment
// This is an option for modulating the navigation gains in flight
// to maintain a constant turn radius in heavy winds in waypoing mode.
// Define WIND_GAIN_ADJUSTMENT as 1 to turn this feature on.
#define WIND_GAIN_ADJUSTMENT                0

// Altitude Hold
// Use altitude hold in stabilized mode?  In waypoint mode?
// Each of these settings can be AH_NONE, AH_FULL, or AH_PITCH_ONLY
//  - In waypoint mode, the target altitude is defined by the waypoints or logo program.
//  - In stabilized mode, when ALTITUDEHOLD_STABILIZED is set to AH_PITCH_ONLY, the target
// altitude is whatever altitude the plane was at when switched into stabilized mode.
//  - In stabilized mode, when ALTITUDEHOLD_STABILIZED is set to AH_FULL, the target
// altitude is determined by the position of the throttle stick on the transmitter.
// NOTE: even when set to AH_NONE, MatrixPilot will still try to stabilize pitch as long
// as PITCH_STABILIZATION is set to 1 above, but will not aim for any specific altitude.
#define ALTITUDEHOLD_STABILIZED             AH_FULL
#define ALTITUDEHOLD_WAYPOINT               AH_FULL

// Speed Control
// If you define SPEED_CONTROL to be 1, MatrixPilot will take air speed into account
// in the altitude controls, and will trim the throttle and pitch to maintain air speed.
// Define DESIRED_SPEED to be the air speed that you want, in meters/second.
#define SPEED_CONTROL                       1
#define DESIRED_SPEED                       11.3    // meters/second

// Variable altitude and airspeed
#define GAINS_VARIABLE                      1
#define ALTITUDE_GAINS_VARIABLE             1

// Inverted flight
// Set these to 1 to enable stabilization of inverted flight in stabilized and/or waypoint modes.
#define INVERTED_FLIGHT_STABILIZED_MODE     0
#define INVERTED_FLIGHT_WAYPOINT_MODE       0

// Hovering
// Set these to 1 to enable stabilization of hovering in stabilized and/or waypoint modes.
#define HOVERING_STABILIZED_MODE            0
#define HOVERING_WAYPOINT_MODE              0

// Note: As of MatrixPilot 3.0, Dead Reckoning and Wind Estimation are automatically enabled.

// Camera Stabilization
// Set this value to 1, for camera to be stabilized using camera options further below.
#ifndef USE_CAMERA_STABILIZATION
#define USE_CAMERA_STABILIZATION            0
#endif

// Define MAG_YAW_DRIFT to be 1 to use magnetometer for yaw drift correction.
// Otherwise, if set to 0 the GPS will be used.
// If you select this option, you also need to set magnetometer options in
// the options_magnetometer.h file, including declination and magnetometer type.
#ifndef MAG_YAW_DRIFT
#define MAG_YAW_DRIFT                       0
#endif

// Define USE_BAROMETER_ALTITUDE to be 1 to use barometer for altitude correction.
// Otherwise, if set to 0 only the GPS will be used.
#ifndef USE_BAROMETER_ALTITUDE
#define USE_BAROMETER_ALTITUDE              0
#endif

// Racing Mode
// Setting RACING_MODE to 1 will keep the plane at a set throttle value while in waypoint mode.
// RACING_MODE_WP_THROTTLE is the throttle value to use, and should be set between 0.0 and 1.0.
// Racing performance can be improved by disabling cross tracking for your waypoints.
#define RACING_MODE                         0
#define RACING_MODE_WP_THROTTLE             1.0

// Set this to 1 if you want the UAV Dev Board to fly your plane without a radio transmitter or
// receiver. (Totally autonomous.)  This is just meant for simulation and debugging.  It is not
// recommended that you actually use this option, since you'd have no manual control to fall
// back on if things go wrong.  It may not even be legal in your area.
#ifndef NORADIO
#define NORADIO                             0
#endif


////////////////////////////////////////////////////////////////////////////////
// Configure Input and Output Channels
//
// For setups with an external Rx device:
//   Use a single PPM input connection from the RC receiver to the UDB on RC input channel PPM_IC.
//   The 8 standard output channels remain unaffected.
//   For UDB4 and UDB5 boards:
//     2 additional output channels are available on pins RA4 and RA1.
//   For AUAV3 boards:
//     support for additional output channels may be developed upon request
//
// For all boards:
// If you're not sure, leave USE_PPM_INPUT set to 0.
// PPM_NUMBER_OF_CHANNELS is the number of channels sent on the PWM signal.  This is
// often different from the NUM_INPUTS value below, and should usually be left at 8.
//
#define USE_PPM_INPUT                       0
#define PPM_NUMBER_OF_CHANNELS              8
#define PPM_SIGNAL_INVERTED                 0
#define PPM_ALT_OUTPUT_PINS                 0

// Select which Input Capture pin the PPM device is connected to
// changing this can be useful when using PPM and fitting a UDB into
// very tight airframes, as it allows alternative input pins to be
// assigned for connection to the receiver.
// If not using PPM, then this must be left set to '1'
#define PPM_IC                              1

// NUM_INPUTS:
// If using PWM inputs (parallel Rx connections), set to the number of cables connected, 1-8
// If using PPM inputs (serial Rx connection), set to the number of Rx channels, up to PPM_NUMBER_OF_CHANNELS
// If using LRS library (integrated SPI tranceiver), set to the number of Rx channels, up to 16
#define NUM_INPUTS                          8

// Channel numbers for each input.
// Use as is, or edit to match your setup.
//   - If you're set up to use Rudder Navigation (like MatrixNav), then you may want to swap
//     the aileron and rudder channels so that rudder is CHANNEL_1, and aileron is 5.
#define AILERON_INPUT_CHANNEL               CHANNEL_1
#define ELEVATOR_INPUT_CHANNEL              CHANNEL_3
#define THROTTLE_INPUT_CHANNEL              CHANNEL_4
#define RUDDER_INPUT_CHANNEL                CHANNEL_5
#define BRAKE_THR_SEL_INPUT_CHANNEL         CHANNEL_6
#define BRAKE_INPUT_CHANNEL                 CHANNEL_UNUSED
#define MODE_SWITCH_INPUT_CHANNEL           CHANNEL_7
#define FLAPS_INPUT_CHANNEL                 CHANNEL_8
#define SPOILER_INPUT_CHANNEL               CHANNEL_UNUSED
#define CAMERA_PITCH_INPUT_CHANNEL          CHANNEL_UNUSED
#define CAMERA_YAW_INPUT_CHANNEL            CHANNEL_UNUSED
#define CAMERA_MODE_INPUT_CHANNEL           CHANNEL_UNUSED
#define OSD_MODE_SWITCH_INPUT_CHANNEL       CHANNEL_UNUSED
#define MODE_INVERTED_CHANNEL               CHANNEL_UNUSED
#define PASSTHROUGH_A_INPUT_CHANNEL         CHANNEL_UNUSED
#define PASSTHROUGH_B_INPUT_CHANNEL         CHANNEL_UNUSED
#define PASSTHROUGH_C_INPUT_CHANNEL         CHANNEL_UNUSED
#define PASSTHROUGH_D_INPUT_CHANNEL         CHANNEL_UNUSED

// NUM_OUTPUTS:
//   NOTE: If USE_PPM_INPUT is enabled above, up to 9 outputs are available.)
// For UDB4/5 boards: Set to 3-8 (or up to 10 using pins RA4 and RA1.)
// For AUAV3 boards:  Set to 3-8 (or up to 11 using pins RE1, RA6 and RA7.)
//                               (this needs developing, so contact the list)
#define NUM_OUTPUTS                         8

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
//#define THROTTLE_OUTPUT_CHANNEL             CHANNEL_3
#define AILERON_LEFT_OUTPUT_CHANNEL         CHANNEL_1
#define FLAP_RIGHT_OUTPUT_CHANNEL           CHANNEL_2
#define AILERON_RIGHT_OUTPUT_CHANNEL        CHANNEL_3
#define FLAP_LEFT_OUTPUT_CHANNEL            CHANNEL_4
#define ELEVATOR_OUTPUT_CHANNEL             CHANNEL_5  // fuselage
#define RUDDER_OUTPUT_CHANNEL               CHANNEL_6  // fuselage
#define THROTTLE_OUTPUT_CHANNEL             CHANNEL_7  // fuselage

#define AILERON_OUTPUT_CHANNEL              CHANNEL_UNUSED
#define AILERON_SECONDARY_OUTPUT_CHANNEL    CHANNEL_UNUSED
#define CAMERA_PITCH_OUTPUT_CHANNEL         CHANNEL_UNUSED
#define CAMERA_YAW_OUTPUT_CHANNEL           CHANNEL_UNUSED
#define TRIGGER_OUTPUT_CHANNEL              CHANNEL_UNUSED
#define PASSTHROUGH_A_OUTPUT_CHANNEL        CHANNEL_UNUSED
#define PASSTHROUGH_B_OUTPUT_CHANNEL        CHANNEL_UNUSED
#define PASSTHROUGH_C_OUTPUT_CHANNEL        CHANNEL_UNUSED
#define PASSTHROUGH_D_OUTPUT_CHANNEL        CHANNEL_UNUSED
#define BRAKE_OUTPUT_CHANNEL                CHANNEL_8


////////////////////////////////////////////////////////////////////////////////
// Servo Reversing Configuration
// For any of these that are set to 1, that servo will be sent reversed controls.
// Note that your servo reversing settings here should match what you set on your transmitter.
#define AILERON_CHANNEL_REVERSED            0
#define ELEVATOR_CHANNEL_REVERSED           0
#define RUDDER_CHANNEL_REVERSED             0
#define AILERON_SECONDARY_CHANNEL_REVERSED  0
#define THROTTLE_CHANNEL_REVERSED           0
#define CAMERA_PITCH_CHANNEL_REVERSED       0
#define CAMERA_YAW_CHANNEL_REVERSED         0

// Set this to 1 if you need to switch the left and right elevon or vtail surfaces
#define ELEVON_VTAIL_SURFACES_REVERSED      0


////////////////////////////////////////////////////////////////////////////////
// Mode Switch is ideally controlled by a 3-position switch on your transmitter.
// Often the Flap channel will be controlled by a 3-position switch.
// These are the thresholds for the cutoffs between low and middle, and between middle and high.
// Normal signals should fall within about 2000 - 4000.
#define MODE_SWITCH_THRESHOLD_LOW           2813  //knob full left  :p6i2697   Sw right: 2264, 3109, 4147
#define MODE_SWITCH_THRESHOLD_HIGH          3490  //knob full right :p6i4204

// Setting MODE_SWITCH_TWO_POSITION to 1,  allows a two state mode switch on the transmitter to be used
// to create three flight modes. When switch is "Down" the plane always reverts to Manual. When "Up",
// the plane moves to Stabilized". If the user is in stabilized ("Up"), and then the user toggles
// the switch to Down, Up, Down, Up, then the plane moves to autonomous.
// Each toggle must be achieved with a limited time period ( 1/2 a second ) and not faster than 1/40th of a second.
// When in Autonomous, a move to "Down" puts the switch state  back to Manual. And a futher move to "Up", will put the
// switch state back in stabilized. The important design concept is that Manual position is always Manual state immediately.
// Stabilized position is Stabilized mode unless you try  hard to reach Autonomous mode.
// Set MODE_SWITCH_TWO_POSITION to 0 for a normal three position mode switch.
#define MODE_SWITCH_TWO_POSITION            0

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
#define FAILSAFE_INPUT_CHANNEL             MODE_SWITCH_INPUT_CHANNEL
#define FAILSAFE_INPUT_MIN                 1950 // knob full left :p6i1890  - left switch must be left, knob left (switch shorts 1.1K in series with potmeter. Sw off: 2264, 3109, 4147
#define FAILSAFE_INPUT_MAX                 4300 // knob full right :p6i4204 => set full right, power on aircraft, set auto for FS auto, takeoff manual

// FAILSAFE_TYPE controls the UDB's behavior when in failsafe mode due to loss of transmitter
// signal.  (Set to FAILSAFE_RTL or FAILSAFE_MAIN_FLIGHTPLAN.)
//
// When using FAILSAFE_RTL (Return To Launch), the UDB will begin following the RTL flight plan
// as defined near the bottom of the flightplan-waypoints.h or flightplan-logo.h files.  By default, this
// is set to return to a point above the location where the UDB was powered up, and to loiter there.
// See the flightplan-waypoints.h or flightplan-logo.h files for info on modifying this behavior.
//
// When set to FAILSAFE_MAIN_FLIGHTPLAN, the UDB will instead follow the main flight plan as
// defined in either flightplan-waypoints.h or flightplan-logo.h.  If the UDB was already in waypoint mode
// when it lost signal, the plane will just continue following the main flight plan without
// starting them over.  And if the transmitter is still in waypoint mode when the UDB sees it
// again, the UDB will still continue following the main flight plan without restarting.  If
// the UDB loses signal while not in waypoint mode, it will start the main flight plan from the
// beginning.
#define FAILSAFE_TYPE                       FAILSAFE_RTL

// When FAILSAFE_HOLD is set to 1, then once Failsafe has engaged, and you have subsequently
// regained your RC TX-RX connection, you will need to manually change the Mode Switch in order
// to exit Failsafe mode.  This avoids the situation where your plane flies in and out of range,
// and keeps switching into and out of Failsafe mode, which depending on your configuration,
// could be confusing and/or dangerous.
#define FAILSAFE_HOLD                       0


////////////////////////////////////////////////////////////////////////////////
// Serial Output Format (Can be SERIAL_NONE, SERIAL_DEBUG, SERIAL_ARDUSTATION,
// SERIAL_UDB_EXTRA, SERIAL_MAVLINK, SERIAL_CAM_TRACK, SERIAL_OSD_REMZIBI, SERIAL_MAGNETOMETER)
// This determines the format of the output sent out the spare serial port.
// Note that SERIAL_OSD_REMZIBI only works with a ublox GPS.
// SERIAL_UDB_EXTRA will add additional telemetry fields to those of SERIAL_UDB.
// SERIAL_UDB_EXTRA can be used with the OpenLog without characters being dropped.
// SERIAL_UDB_EXTRA may result in dropped characters if used with the XBEE wireless transmitter.
// SERIAL_CAM_TRACK is used to output location data to a 2nd UDB, which will target its camera at this plane.
// SERIAL_MAVLINK is a bi-directional binary format for use with QgroundControl, HKGCS or MAVProxy (Ground Control Stations.)
// SERIAL_MAGNETOMETER outputs the automatically calculated offsets and raw magnetometer data.
// Note that SERIAL_MAVLINK defaults to using a baud rate of 57600 baud (other formats default to 19200)

#ifndef SERIAL_OUTPUT_FORMAT
#define SERIAL_OUTPUT_FORMAT                SERIAL_NONE
#endif


////////////////////////////////////////////////////////////////////////////////
// Serial Output BAUD rate for either standard telemetry streams or MAVLink
//  19200, 38400, 57600, 115200, 230400, 460800, 921600 // yes, it really will work at this rate
//#define SERIAL_BAUDRATE                     19200


// NUM_ANALOG_INPUTS:
// For UDB4 boards: Set to 0-4.  Analog pins are AN15 - AN18.
//#define NUM_ANALOG_INPUTS                   0 // moved to board specific config files

// Channel numbers for each analog input
//   - Only assign each channel number to one analog sensor
//   - If you don't want to use an output channel, set it to CHANNEL_UNUSED
//   - Only 2 analog inputs are available, so you can't use all the defined analog
//     sensors at once
//
// ANALOG_CURRENT_INPUT_CHANNEL and ANALOG_VOLTAGE_INPUT_CHANNEL let you plug in and
// use this Voltage/Current sensor board from SparkFun:
//    http://www.sparkfun.com/products/9028
// Just plug the ground and signal lines of the chosen current input channel into the
// ground and current outputs of the current sensor, and the signal line of the chosen
// voltage input channel to the voltage output from the current sensor.  Values for
// instantaneous current, voltage, and mAh used will become available for use with the
// OSD layout.
//
// ANALOG_RSSI_INPUT_CHANNEL lets you connect your RC Receiver's RSSI output to your
// UDB, in order to see the RC signal strength on your OSD.  Just plug RSSI and ground
// from your Receiver to Input2's signal and ground on your UDB.  If you use this feature,
// you'll also need to set up the RSSI_MIN_SIGNAL_VOLTAGE and RSSI_MAX_SIGNAL_VOLTAGE
// to match your Receiver's RSSI format.  Note that some receivers use a higher voltage to
// represent a lower signal strength, so you may need to set MIN higher than MAX.

#define ANALOG_CURRENT_INPUT_CHANNEL        CHANNEL_UNUSED
#define ANALOG_VOLTAGE_INPUT_CHANNEL        1     //A15 on Udb5   //A2 on Auav3
#define ANALOG_RSSI_INPUT_CHANNEL           CHANNEL_UNUSED

#define MAX_CURRENT                         900 // 90.0 Amps max for the sensor from SparkFun (in tenths of Amps)
#define CURRENT_SENSOR_OFFSET               10  // Add 1.0 Amp to whatever value we sense

#define MAX_VOLTAGE                         1120 // 56.0 Volts max for the sensor from SparkFun (in tenths of Volts)
#define VOLTAGE_SENSOR_OFFSET               0   // Add 0.0 Volts to whatever value we sense

// RSSI - RC Receiver signal strength
#define RSSI_MIN_SIGNAL_VOLTAGE             0.5     // Voltage when RSSI should show 0%
#define RSSI_MAX_SIGNAL_VOLTAGE             3.3     // Voltage when RSSI should show 100%


////////////////////////////////////////////////////////////////////////////////
// MAXBOTIX SONAR LANDING FLARE
// Designed for use with the following device:-
// http://www.maxbotix.com/Ultrasonic_Sensors/MB1230.htm
// Can be used on INPUT 8 of the UDB4/5 if that is not used for a channel input.
// Will return distance to ground in meters and compensate for roll subject to
// receiving a returned sonar signal.
// This option is designed to be used with Logo Flight Planning.
// Logo allows the user to Interrupt a Landing and flare, or Go Around,
// based on sonar distance to ground.

// Set USE_SONAR_INPUT to the input capture channel which the sensor
// is connected to. Must be greater than the last used servo channel.
#define USE_SONAR_INPUT                     0


////////////////////////////////////////////////////////////////////////////////
// Trigger Action
// Use the trigger to do things like drop an item at a certain waypoint, or take a photo every
// N seconds during certain waypoint legs.

// TRIGGER_TYPE can be set to TRIGGER_TYPE_NONE, TRIGGER_TYPE_SERVO, or TRIGGER_TYPE_DIGITAL.
// If using TRIGGER_TYPE_SERVO, set the TRIGGER_OUTPUT_CHANNEL above to choose which output channel
// receives trigger events, and set the TRIGGER_SERVO_LOW and TRIGGER_SERVO_HIGH values below.
// If using TRIGGER_TYPE_DIGITAL, the trigger will be on pin RE4.  In this case make sure to set
// NUM_OUTPUTS to be less than 6 to avoid a conflict between digital output and servo output on
// that pin.

// TRIGGER_ACTION can be: TRIGGER_PULSE_HIGH, TRIGGER_PULSE_LOW, TRIGGER_TOGGLE, or TRIGGER_REPEATING
// The trigger action output is always either low or high.  In servo mode, low and high are servo
// values set below.  In digital mode, low and high are 0V and 5V on pin RE4.
// The action is triggered when starting on a waypoint leg that includes the F_TRIGGER flag (see the
// flightplan-waypoints.h file).
// If set to TRIGGER_PULSE_HIGH or TRIGGER_PULSE_LOW, then the output will pulse high or low for the
// number of milliseconds set by TRIGGER_PULSE_DURATION.
// If set to TRIGGER_TOGGLE, the output will just switch from high to low, or low to high each time
// the action is triggered.
// If set to TRIGGER_REPEATING, then during any waypoint leg with F_TRIGGER set, high pulses will be
// sent every TRIGGER_REPEAT_PERIOD milliseconds.

// Note, durations in milliseconds are rounded down to the nearest 25ms.

#define TRIGGER_TYPE                        TRIGGER_TYPE_NONE
#define TRIGGER_ACTION                      TRIGGER_PULSE_HIGH
#define TRIGGER_SERVO_LOW                   2000
#define TRIGGER_SERVO_HIGH                  4000
#define TRIGGER_PULSE_DURATION              250
#define TRIGGER_REPEAT_PERIOD               4000


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.
// Proportional gains should be less than 4.0.
// Rate gains should be less than 0.8.
// With the new helical turn control, rate gains are not even needed, try setting them all to zero.
// Proportional gains include ROLLKP, YAWKP_AILERON, PITCHGAIN,
// ELEVATOR_BOOST, YAWKP_RUDDER, ROLLKP_RUDDER,
// MANUAL_AILERON_RUDDER_MIX, RUDDER_BOOST, HOVER_ROLLKP, HOVER_PITCHGAIN, HOVER_YAWKP
// Rate gains include ROLLKD, YAWKD_AILERON, PITCHKD, YAWKD_RUDDER, ROLLKD_RUDDER,
// HOVER_ROLLKD, HOVER_PITCHKD, HOVER_YAWKD

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT                            1.0

// FEED_FORWARD is a feed forward gain for deflecting control surfaces for turn rate.
// The KP gains for each axis are multiplied by FEED_FORWARD to determine
// the feed forward gain for that axis.
// For each axis, a deflection term is added equal to the feed forward gain for that axis
// times projection of the desired earth vertical rotation rate onto that axis
#define FEED_FORWARD                        0.8

// TURN_RATE_NAV and TURN_RATE_FBW set the gains of the helical turn control for
// waypoint navigation mode and fly by wire mode respectively.
// They are specified in terms of the maximum desired turning rate in degrees per second in each mode.
// The largest possible value is 240 degrees per second, anything larger will be clipped to 240.
#define TURN_RATE_NAV                       30.0
#define TURN_RATE_FBW                       60.0

// Aileron/Roll Control Gains
// ROLLKP is the proportional gain, approximately 0.25
// ROLLKD is the derivative (gyro) gain, approximately 0.125
// YAWKP_AILERON is the proportional feedback gain for ailerons in response to yaw error.
// use it only if there is no rudder.
// YAWKD_AILERON is the derivative feedback gain for ailerons in response to yaw rotation.
// use it only if there is no rudder.
#define ROLLKP                              0.20
#define ROLLKD                              0.00
#define YAWKP_AILERON                       0.10
#define YAWKD_AILERON                       0.00

// Elevator/Pitch Control Gains
// PITCHGAIN is the pitch stabilization gain, typically around 0.125
// PITCHKD feedback gain for pitch damping, around 0.0625
// ELEVATOR_BOOST is the additional gain multiplier for the manually commanded elevator deflection
#define PITCHGAIN                           0.10 // Bill cessna - 0.10 Matt cularius
#define PITCHKD                             0.00
#define ELEVATOR_BOOST                      0.50

// Parameters below are used in the computation of angle of attack and pitch trim.
// ( INVERTED_NEUTRAL_PITCH is no longer used and should not be used.) -- Note (RobD) yes it is?
// If these parameters are not defined, angle of attack and pitch trim will be set to zero.
// REFERENCE_SPEED                      The nominal speed in meters per second at which the parameters are defined.
// ANGLE_OF_ATTACK_NORMAL               Angle of attack in degrees in the body frame for normal straight and level flight at cruise speed.
// ANGLE_OF_ATTACK_INVERTED             Angle of attack in degrees in the body frame for inverted straight and level flight at cruise speed.
// Note: ANGLE_OF_ATTACK_INVERTED is usually negative, with typical values in the -5 to -10 degree range.
// ELEVATOR_TRIM_NORMAL                 Elevator trim in fractional servo units (-1.0 to 1.0 ) for normal straight and level flight at cruise speed.
// ELEVATOR_TRIM_INVERTED               Elevator trim in fractional servo units (-1.0 to 1.0 ) for inverted straight and level flight at cruise speed.
// Note: ELEVATOR_TRIM_INVERTED is usually negative, with typical values in the -0.5 to -1.0 range.

#define REFERENCE_SPEED                 (  11.3 )
#define ANGLE_OF_ATTACK_NORMAL          (   0.0 )
#define ANGLE_OF_ATTACK_INVERTED        (   0.0 )
#define ELEVATOR_TRIM_NORMAL            (   0.0 )
#define ELEVATOR_TRIM_INVERTED          (   0.0 )

// CUSTOM OFFSETS are recommended when using Angle of Attack and Trim Parameters
// They ensure that the measured orientation of the plane, particularly in pitch,
// are the same for each flight (they are not re-calibrated on bootup but are
// pre-measured and fixed for all flights by the offsets below).
// Note each offset is specific to one specific autopilot board.
// Be careful not to use the offsets below with the wrong board.
// Uncomment the line below to activate the CUSTOM_OFFSETS feature in MatrixPilot.

//#define CUSTOM_OFFSETS
#define XACCEL_OFFSET (  000 ) 
#define YACCEL_OFFSET (  000 )
#define ZACCEL_OFFSET (  000 )
#define XRATE_OFFSET  (  000 ) // not used by the UDB4
#define YRATE_OFFSET  (  000 ) // not used by the UDB4
#define ZRATE_OFFSET  (  000 ) // not used by the UDB4

// Rudder/Yaw Control Gains
// YAWKP_RUDDER is the proportional feedback gain for rudder control of yaw orientation.
// YAWKD_RUDDER is the yaw gyro feedback gain for the rudder in reponse to yaw rotation.
// ROLLKP_RUDDER is the feedback gain for the rudder in response to the current roll angle,
// use it only if there are no ailerons.
// ROLLKD_RUDDER is the feedback gain for the rudder in response to the rate of change roll angle,
// use it only if there are no ailerons.
// MANUAL_AILERON_RUDDER_MIX is the fraction of manual aileron control to mix into the rudder when
// in stabilized or waypoint mode.  This mainly helps aileron-initiated turning while in stabilized.
// MANUAL_AILERON_RUDDER_MIX is no longer needed with the new controls, it should be set to zero.
// RUDDER_BOOST is the additional gain multiplier for the manually commanded rudder deflection
#define YAWKP_RUDDER                        0.10
#define YAWKD_RUDDER                        0.00
#define ROLLKP_RUDDER                       0.06
#define ROLLKD_RUDDER                       0.00
#define MANUAL_AILERON_RUDDER_MIX           0.00
#define RUDDER_BOOST                        1.00

// Gains for Hovering
// These are still here from the previous version of the controls, because the new controls have not yet been set up for hovering.
// Gains are named based on plane's frame of reference (roll means ailerons)
// HOVER_ROLLKP is the roll-proportional feedback gain applied to the ailerons while navigating a hover
// HOVER_ROLLKD is the roll gyro feedback gain applied to ailerons while stabilizing a hover
// HOVER_PITCHGAIN is the pitch-proportional feedback gain applied to the elevator while stabilizing a hover
// HOVER_PITCHKD is the pitch gyro feedback gain applied to elevator while stabilizing a hover
// HOVER_PITCH_OFFSET is the neutral pitch angle for the plane (in degrees) while stabilizing a hover
// HOVER_YAWKP is the yaw-proportional feedback gain applied to the rudder while stabilizing a hover
// HOVER_YAWKD is the yaw gyro feedback gain applied to rudder while stabilizing a hover
// HOVER_YAW_OFFSET is the neutral yaw angle for the plane (in degrees) while stabilizing a hover
// HOVER_PITCH_TOWARDS_WP is the max angle in degrees to pitch the nose down towards the WP while navigating
// HOVER_NAV_MAX_PITCH_RADIUS is the radius around a waypoint in meters, within which the HOVER_PITCH_TOWARDS_WP
//                            value is proportionally scaled down.
#define HOVER_ROLLKP                        0.05
#define HOVER_ROLLKD                        0.05
#define HOVER_PITCHGAIN                     0.2
#define HOVER_PITCHKD                       0.25
#define HOVER_PITCH_OFFSET                  0.0        // + leans towards top, - leans towards bottom
#define HOVER_YAWKP                         0.2
#define HOVER_YAWKD                         0.25
#define HOVER_YAW_OFFSET                    0.0
#define HOVER_PITCH_TOWARDS_WP             30.0
#define HOVER_NAV_MAX_PITCH_RADIUS         20


////////////////////////////////////////////////////////////////////////////////
// Camera Stabilization and Targeting
//
// There are three camera modes within MatrixPilot
/// Canera Mode 1: No stabilisation for camera pitch or yaw
//  Camera Mode 2: Stabilisation of camera pitch but not yaw.
//  Camera Mode 3: Camera targetting. The camera is aimed at a GPS location.

// Control of camera modes
// If CAMERA_MODE_INPUT_CHANNEL is assigned to a channel in the channels section of
// options.h then a three position switch can be used to select between the three camera
// stabilization modes. The following min and max values should work for most transmitters.

#define CAMERA_MODE_THRESHOLD_LOW          2600
#define CAMERA_MODE_THRESHOLD_HIGH         3400

// If you do not have a spare channel for CAMERA_MODE_INPUT_CHANNEL then,
// If CAMERA_MODE_INPUT_CHANNEL is defined as CHANNEL_UNUSED :-
//  In UDB Manual Mode the camera is fixed straight ahead. (Camera mode 1)
//  In UDB Stabilized Mode, the camera stabilizes in the pitch axis but stabilizes a constant yaw
//     relative to the plane's frame of reference. (Camera mode 2).
//  In Waypoint Mode, the direction of the camera is driven from a flight camera plan in flightplan-waypoints.h
// In all three flight modes, if you set CAMERA_INPUT_CHANNEL then the transmitter camera controls
// will be mixed into the camera stabilisation. This allows a pilot to override the camera stabilization dynamically
// during flight and point the camera at a specific target of interest.

// Setup and configuration of camera targetting at installation of camera servos:-
// To save cpu cycles, you will need to pre-compute the tangent of the desired pitch of the camera
// when in stabilized mode. This should be expressed in 2:14 format.
// Example: You require the camera to be pitched down by 15 degrees from the horizon in stabilized mode.
// Paste the following line into a google search box (without the //)
// tan((( 15 /180 )* 3.1416 ))* 16384
// The result, as an integer, will be 4390. Change the angle, 15, for whatever angle you would like.
// Note that CAM_TAN_PITCH_IN_STABILIZED_MODE should not exceed 32767 (integer overflows to negative).

#define CAM_TAN_PITCH_IN_STABILIZED_MODE    1433    // 1443 is 5 degrees of pitch. Example: 15 degrees is 4389
#define CAM_YAW_IN_STABILIZED_MODE          0       // in degrees relative to the plane's yaw axis.    Example: 0

// All number should be integers
#define CAM_PITCH_SERVO_THROW               95      // Camera lens rotation at maximum PWM change (2000 to 4000), in degrees.
#define CAM_PITCH_SERVO_MAX                 85      // Max pitch up that plane can tilt and keep camera level, in degrees.
#define CAM_PITCH_SERVO_MIN                -22      // Max pitch down that plane can tilt and keep camera level, in degrees.
#define CAM_PITCH_OFFSET_CENTRED            38      // Offset in degrees of servo that results in a level camera.
                                                    // Example: 30 would mean that a centered pitch servo points the camera
                                                    // 30 degrees down from horizontal when looking to the front of the plane.

#define CAM_YAW_SERVO_THROW                 350     // Camera yaw movement for maximum yaw PWM change (2000 to 4000) in Degrees.
#define CAM_YAW_SERVO_MAX                   130     // Max positive yaw of camera relative to front of plane in Degrees.
#define CAM_YAW_SERVO_MIN                  -130     // Min reverse  yaw of camera relative to front of plane in Degrees.
#define CAM_YAW_OFFSET_CENTRED              11      // Yaw offset in degrees that results in camera pointing forward.

// Camera test mode will move the yaw from + 90 degrees to + 90 degrees every 5 seconds. (180 degree turn around)
// That will show whether the CAM_PITCH_SERVO_THROW value is set correctly for your servo.
// Once the camera rotates correctly through 180 degrees, then you can adjust CAM_PITCH_OFFSET_CENTRED to center the camera.
// In Camera test mode, pitch angle changes permanently to 90 degrees down in stabilized mode, and  0 (level) in Manual Mode.

#define CAM_TESTING_OVERIDE                 0       // Set to 1 for camera to move to test angles in stabilized mode.
#define CAM_TESTING_YAW_ANGLE               90      // e.g. 90 degrees. Will try to swing 90 degrees left, then 90 degrees right
#define CAM_TESTING_PITCH_ANGLE             90      // In degrees.

// Set this to 1 to ignore camera target data from the flightplan, and instead use camera target data coming in on the serial port.
// This data can be generated by another UDB running MatrixPilot, using SERIAL_CAM_TRACK.
// NOTE: When using camera tracking, both UDBs must be set to use the same fixed origin location.
#define CAM_USE_EXTERNAL_TARGET_DATA        0


////////////////////////////////////////////////////////////////////////////////
// Configure altitude hold
// These settings are only used when Altitude Hold is enabled above.

// Min and Max target heights in meters.  These only apply to stabilized mode.
#define HEIGHT_TARGET_MIN                   25.0
#define HEIGHT_TARGET_MAX                   100.0

// The range of altitude within which to linearly vary the throttle
// and pitch to maintain altitude.  A bigger value makes altitude hold
// smoother, and is suggested for very fast planes.
#define HEIGHT_MARGIN                       10

// Use ALT_HOLD_THROTTLE_MAX when below HEIGHT_MARGIN of the target height.
// Interpolate between ALT_HOLD_THROTTLE_MAX and ALT_HOLD_THROTTLE_MIN
// when within HEIGHT_MARGIN of the target height.
// Use ALT_HOLD_THROTTLE_MIN when above HEIGHT_MARGIN of the target height.
// Throttle values are from 0.0 - 1.0.
#define ALT_HOLD_THROTTLE_MIN               0.0
#define ALT_HOLD_THROTTLE_MAX               1.0

// Use ALT_HOLD_PITCH_MAX when below HEIGHT_MARGIN of the target height.
// Interpolate between ALT_HOLD_PITCH_MAX and ALT_HOLD_PITCH_MIN when
// within HEIGHT_MARGIN of the target height.
// Use ALT_HOLD_PITCH_HIGH when above HEIGHT_MARGIN of the target height.
// Pitch values are in degrees.  Negative values pitch the plane down.
#define ALT_HOLD_PITCH_MIN                 -15.0
#define ALT_HOLD_PITCH_MAX                  -4.0
#define ALT_HOLD_PITCH_HIGH                -15.0


////////////////////////////////////////////////////////////////////////////////
// Return To Launch Pitch Down in degrees, a real number.
// this is the real angle in degrees that the nose of the plane will pitch downward during a return to launch.
// it is used to increase speed (and wind penetration) during a return to launch.
// set it to zero if you do not want to use this feature.
// This only takes effect when entering RTL mode, which only happens when the plane loses the transmitter signal.
#define RTL_PITCH_DOWN                      0.0


////////////////////////////////////////////////////////////////////////////////
// Hardware In the Loop Simulation
// Only set this to 1 for testing in the simulator.  Do not try to fly with this set to 1!
// See the MatrixPilot wiki for more info on using HILSIM.
// HILSIM_BAUD is the serial speed for communications with the X-Plane plugin.  Default is
// now 38400.  Make sure the X-Plane plugin's Setup file has its speed set to match.
#ifndef HILSIM
#define HILSIM                              1
#endif
#define HILSIM_USB                          0           // AUAV3 only (under development)
#define HILSIM_BAUD                         38400


////////////////////////////////////////////////////////////////////////////////
// Flight Plan handling
//
// You can define your flightplan either using the UDB Waypoints format, or using UDB Logo
// Set this to either FP_WAYPOINTS or FP_LOGO
// The Waypoint definitions and options are located in the flightplan-waypoints.h file.
// The Logo flight plan definitions and options are located in the flightplan-logo.h file.
#ifndef FLIGHT_PLAN_TYPE
#define FLIGHT_PLAN_TYPE                    FP_LOGO
#endif

////////////////////////////////////////////////////////////////////////////////
// Waypoint handling

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
#define WAYPOINT_PROXIMITY_RADIUS	40

// Origin Location
// When using relative waypoints, the default is to interpret those waypoints as relative to the
// plane's power-up location.  Here you can choose to use any specific, fixed 3D location as the
// origin point for your relative waypoints.
//
// USE_FIXED_ORIGIN should be 0 to use the power-up location as the origin for relative waypoints.
// Set it to 1 to use a fixed location as the origin, no matter where you power up.
// FIXED_ORIGIN_LOCATION is the location to use as the origin for relative waypoints.  It uses the
// format { X, Y, Z } where:
// X is Longitude in degrees * 10^7
// Y is Latitude in degrees * 10^7
// Z is altitude above sea level, in meters, as a floating point value.
// 
// If you are using waypoints for an autonomous landing, it is a good idea to set the altitude value
// to be the altitude of the landing point, and then express the heights of all of the waypoints with
// respect to the landing point.
// If you are using OpenLog, an easy way to determine the altitude of your landing point is to
// examine the telemetry after a flight, take a look in the .csv file, it will be easy to spot the
// altitude, expressed in meters.

#define USE_FIXED_ORIGIN	    0
//#define FIXED_ORIGIN_LOCATION	    { -1219950467, 374124664, 30.0 }	// A point in Baylands Park in Sunnyvale, CA
#define FIXED_ORIGIN_LOCATION	    { 113480854, 472580108, 578 }	// Innsbruck, useful for X-Plane flight simulator

///////////////////////////////////////////////////////////////////////////////////
// Vehicle and Pilot Identification

// Once you are flying your plane and swapping flights and telemetry with other's across
// the world, you may like to fill in some of the fields below. This will be embedded in your
// telemetry, and used to make more interesting flights in Google Earth.
// ID_VEHICLE_MODEL_NAME provides indication of what model of plane, quad, car etc you are using
// ID_VEHICLE_REGISTRATION should be short (less than 12 continuous characters with no space
// it will be used in Google Earth as the folder name containing your flights.
// ID_LEAD_PILOT is your lead pilot flight name or alias e.g. "UAV Flight Director"
// ID_DIY_DRONES_URL should be the URL of your member page on DIY Drones.
// That will allow Google Earth viewers of your flights to click straight through to your latest discussions.
// EXAMPLE:-
//#define ID_VEHICLE_MODEL_NAME "Multiplex Twinstar 2"
//#define ID_VEHICLE_REGISTRATION "TW2-PDH-UK"
//#define ID_LEAD_PILOT "Pete Hollands"
//#define ID_DIY_DRONES_URL "http://www.diydrones.com/profile/PeterHollands"
#define ID_VEHICLE_MODEL_NAME               "Not Defined"
#define ID_VEHICLE_REGISTRATION             "Not Defined"
#define ID_LEAD_PILOT                       "Not Defined"
#define ID_DIY_DRONES_URL                   "http://www.diydrones.com"


////////////////////////////////////////////////////////////////////////////////
// The following define is used to enable vertical initialization for VTOL
// To enable vertical initialization, uncomment the line
//#define INITIALIZE_VERTICAL


////////////////////////////////////////////////////////////////////////////////
// Fly-By-Datalink Configure
// This allows flight of an aircraft using data instead of an RC transmitter using the app found
// in /Tools/FlyByDatalink/UDB_FlyByDatalink.exe. This app takes input from a typical off-the-shelf
// gaming joystick and transmits it to the UDB over serial or IP. The joystick used for development
// was the Logitech Attack3. This data overrides the PWM inputs allowing for direct control of the flight
// surfaces. While this is enabled, instead of the usual manual/stabilized/WP flight modes, it's
// FBDL/stabilized/WP. For saftey reasons, an RC transmitter is still required for flight to set the modes.
#define FLY_BY_DATALINK_ENABLED             0


////////////////////////////////////////////////////////////////////////////////
// Optionally enable experimental extended range navigation support (merged from ballon launch branch)
#define USE_EXTENDED_NAV


////////////////////////////////////////////////////////////////////////////////
// Debugging defines

// The following can be used to do a ground check of stabilization without a GPS.
// If you define TestGains, stabilization functions
// will be enabled, even without GPS or Tx turned on. (Tx is optional)
// #define TestGains                        // uncomment this line if you want to test your gains without using GPS

// Set this to 1 to calculate and print out free stack space
#ifndef RECORD_FREE_STACK_SPACE
#define RECORD_FREE_STACK_SPACE             0
#endif


////////////////////////////////////////////////////////////////////////////////
// The UDB4/5 has two UART's, while the AUAV3 has four UART's.
// Three MatrixPilot features are currently defined for using a UART.
// These being the GPS, Telemetry and a 'debug' console.
// Therefore UDB4/5 is one UART short, the AUAV3 has one UART extra.
//
// CONSOLE_UART specfies which UART is used for stdio support, aka the console.
// Set CONSOLE_UART to 1, 2, 3 or 4 to enable the console on UART of that number.
// Setting CONSOLE_UART to 0 disables console support.
// On the UDB4/5, optionally specifying console support on UART 1 or 2 overrides
// the default usage of that UART, being the GPS and Telemetry respectively.
// CONSOLE_UART 3 and 4 options are only available with the AUAV3 board.
// Thus UDB4/5 options are 0, 1, or 2  AUAV3 options are 0, 3, or 4
// Set to 9 in order to use the USB for the console connection (under development)
#ifndef CONSOLE_UART
#define CONSOLE_UART                        0
//#define CONSOLE_UART                        6
#endif

// Define USE_DEBUG_IO to enable DPRINT macro to call printf(..)
//#define USE_DEBUG_IO


////////////////////////////////////////////////////////////////////////////////
// AUAV3 only options

////////////////////////////////////////////////////////////////////////////////
// At present, the AUAV3 schematic and 'installation & basic connections' document
// are drafts and hence there is some inconsistency in labelling conventions.
//
// The following standard labelling convention is proposed.
//
// AUAV3 schematic:
//        TLM      -    PORT1
//        OSD      -    PORT2
//        UART3    -    PORT3
//        GPS      -    PORT4
//
// 'AUAV3 Installation and Basic Connections' document:
//        OUART1   -    PORT1
//        OUART2   -    PORT2
//        UART3    -    PORT3
//        GPS      -    PORT4
//
////////////////////////////////////////////////////////////////////////////////
// On the AUAV3, the external UART connections are known as ports 1 through 4.
// The definitions below specifies which feature maps to an external port.
//
// NOTE: on the AUAV3, do not confuse the CONSOLE_UART definition with the
// external port assignment.
// Assign the console to an internal UART with CONSOLE_UART, map this console to
// external port connection with DBG_PORT.
#define GPS_PORT                            4
#define TLM_PORT                            3
#define DBG_PORT                            2

// Set this to 1 to enable filesystem support
#ifndef USE_FILESYS
#define USE_FILESYS                         0
#endif

// Set this to 1 to enable logging telemetry to builtin filesystem
#ifndef USE_TELELOG
#define USE_TELELOG                         0
#endif

// Set this to 1 to enable the USB stack on AUAV3
#ifndef USE_USB
#define USE_USB                             0
#endif

// Set this to 1 to enable the Mass Storage Driver support over USB on AUAV3
#ifndef USE_MSD
#define USE_MSD                             0
#endif
