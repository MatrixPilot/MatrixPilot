// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2014 MatrixPilot Team
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
// This file includes most of the user-configuration for this firmware,
// one of the exceptions being waypoints, which live in the waypoints.h file.
//


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
//      from point of view of the pilot
// ORIENTATION_ROLLCW180: board rolled 90 degrees clockwise,
//      from point of view of the pilot, then rotate the board 180 around the Z axis of the plane,
#define BOARD_ORIENTATION                   ORIENTATION_FORWARDS


////////////////////////////////////////////////////////////////////////////////
// Set this value to your GPS type.  (Set to GPS_STD, GPS_UBX_2HZ, GPS_UBX_4HZ, GPS_MTEK, GPS_NMEA, or GPS_NONE)
#define GPS_TYPE                            GPS_STD

///////////////////////////////////////////////////////////////////////////////
// You can specify a level of good GNSS reception before MatrixPilot accepts "GPS ACQUIRED".
// You can generally leaves these lines at their default values. A value of zero switches off the check.
// The VDOP parameter is only available for Ublox GNSS devices. It is ignored for other GNSS units.
// The metrics are not used by HILSIM or SILSIM.

#define GNSS_HDOP_REQUIRED_FOR_STARTUP      200  //  Horizontal Dilution of Precision
#define GNSS_VDOP_REQUIRED_FOR_STARTUP	    200  //  Vertical Dilution of Precision
#define GNSS_SVS_REQUIRED_FOR_STARTUP	      4  //  Number of Sattelites in View

// Note: As of MatrixPilot 3.0, Dead Reckoning and Wind Estimation are automatically enabled.

// Define MAG_YAW_DRIFT to be 1 to use magnetometer for yaw drift correction.
// Otherwise, if set to 0 the GPS will be used.
// If you select this option, you also need to set magnetometer options in
// the options_magnetometer.h file, including declination and magnetometer type.
#define MAG_YAW_DRIFT                       0

// Set this to 1 if you want the UAV Dev Board to fly your plane without a radio transmitter or
// receiver. (Totally autonomous.)  This is just meant for simulation and debugging.  It is not
// recommended that you actually use this option, since you'd have no manual control to fall
// back on if things go wrong.  It may not even be legal in your area.
#define NORADIO                             1


////////////////////////////////////////////////////////////////////////////////
// Configure Input and Output Channels
//
// NUM_INPUTS: Set to 0-5 
#define NUM_INPUTS                          0

// NUM_OUTPUTS: Set to 3, 4, 5, or 6
#define NUM_OUTPUTS                         3

// Channel numbers for each output
// Use as is, or edit to match your setup.
//   - Only assign each channel to one output purpose
//   - If you don't want to use an output channel, set it to CHANNEL_UNUSED
//
// NOTE: If your board is powered from your ESC through the throttle cable, make sure to
// connect THROTTLE_OUTPUT_CHANNEL to one of the built-in Outputs (1, 2, or 3) to make
// sure your board gets power.
//
#define ROLL_OUTPUT_CHANNEL                 CHANNEL_1
#define PITCH_OUTPUT_CHANNEL                CHANNEL_2
#define YAW_OUTPUT_CHANNEL                  CHANNEL_3

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
#define FAILSAFE_INPUT_CHANNEL              THROTTLE_INPUT_CHANNEL
#define FAILSAFE_INPUT_MIN                  1500
#define FAILSAFE_INPUT_MAX                  4500


////////////////////////////////////////////////////////////////////////////////
// Serial Output BAUD rate for status messages
//  19200, 38400, 57600, 115200, 230400, 460800, 921600 // yes, it really will work at this rate
#define SERIAL_BAUDRATE                     57600 // default


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT                            1.0


////////////////////////////////////////////////////////////////////////////////
// Hardware In the Loop Simulation
// Only set this to 1 for testing in the simulator.  Do not try to fly with this set to 1!
// See the MatrixPilot wiki for more info on using HILSIM.
#define HILSIM                              0


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
// Optionally enable the new power saving idle mode of the MCU during mainloop
//#define USE_MCU_IDLE                        1 // moved to interrupt.h


////////////////////////////////////////////////////////////////////////////////
// Debugging defines

// The following can be used to do a ground check of stabilization without a GPS.
// If you define TestGains, stabilization functions
// will be enabled, even without GPS or Tx turned on. (Tx is optional)
// #define TestGains                        // uncomment this line if you want to test your gains without using GPS

// Set this to 1 to calculate and print out free stack space
#define RECORD_FREE_STACK_SPACE             0


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
#define CONSOLE_UART                        0

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
#define DBG_PORT                            1

// Set this to 1 to enable filesystem support
#ifndef USE_FILESYS
#define USE_FILESYS                         0
#endif

// Set this to 1 to enable logging telemetry to filesystem
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
