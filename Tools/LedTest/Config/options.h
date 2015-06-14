// This file is part of the MatrixPilot LedTest demo.
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


// - First it tests all IO connections on the board.  This will only succeed
// if the board is sitting in SparkFun's green test rig.  If this fails (which
// it will for all normal users) the Red and Blue LEDs will flash 16 times.
// 
// - It then blinks the 4 LEDs in sequence 2 times, while calibrating the
// sensors and testing the EEPROM.  Keep the board flat and motionless
// until the this step is done.
// 
// - If the EEPROM test fails, it freaks out and flashes all 4 LEDs 16 times.
// 
// - If the EEPROM test succeeds, or after flashing if it failed, it switches
// over to alternately testing the accelerometers and testing the gyros for
// 15 seconds each.
// 
// - The Red LED turns on while testing the gyros, and off while
// testing the accelerometers.
// 
// - To test the gyros, rotate the board back and forth on one axis at
// a time and watch for the corresponding LED to flash.
// 
// - To test the accels, I like to let gravity do the work.  Point a
// short edge of the board up and see one LED go on, then point a long
// edge up, then sit it flat to test the Z accel.


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
//		from point of view of the pilot
// ORIENTATION_ROLLCW180: board rolled 90 degrees clockwise,
//		from point of view of the pilot, then rotate the board 180 around the Z axis of the plane,
//		so that the GPS connector points toward the tail of the plane
#define BOARD_ORIENTATION                   ORIENTATION_FORWARDS


////////////////////////////////////////////////////////////////////////////////
// Set this value to your GPS type.  (Set to GPS_STD, GPS_UBX_2HZ, GPS_UBX_4HZ, GPS_MTEK, GPS_NMEA, or GPS_NONE)
#define GPS_TYPE                            GPS_STD

// Note: As of MatrixPilot 3.0, Dead Reckoning and Wind Estimation are automatically enabled.

// Define MAG_YAW_DRIFT to be 1 to use magnetometer for yaw drift correction.
// Otherwise, if set to 0 the GPS will be used.
#define MAG_YAW_DRIFT                       0

// Set this to 1 if you want the UAV Dev Board to fly your plane without a radio transmitter or
// receiver. (Totally autonomous.)  This is just meant for simulation and debugging.  It is not
// recommended that you actually use this option, since you'd have no manual control to fall
// back on if things go wrong.  It may not even be legal in your area.
#define NORADIO                             1


////////////////////////////////////////////////////////////////////////////////
// Configure Input and Output Channels
//
// NUM_INPUTS:
// For classic boards: Set to 1-5 (or 1-8 when using PPM input)
//   1-4 enables only the first 1-4 of the 4 standard input channels
//   5 also enables E8 as the 5th input channel
// For UDB4 boards: Set to 1-8
#define NUM_INPUTS                          0

// NUM_OUTPUTS:
//   NOTE: If USE_PPM_INPUT is enabled above, up to 9 outputs are available.)
// For UDB4/5 boards: Set to 3-8 (or up to 10 using pins RA4 and RA1.)
// For AUAV3 boards:  Set to 3-8 (or up to 11 using pins RE1, RA6 and RA7.)
//                               (this needs developing, so contact the list)
#define NUM_OUTPUTS                         6

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

#define X_ACCEL_OUTPUT_CHANNEL              CHANNEL_4
#define Y_ACCEL_OUTPUT_CHANNEL              CHANNEL_5
#define Z_ACCEL_OUTPUT_CHANNEL              CHANNEL_6


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
#define FAILSAFE_INPUT_CHANNEL              CHANNEL_UNUSED
#define FAILSAFE_INPUT_MIN                  1500
#define FAILSAFE_INPUT_MAX                  4500
// NUM_ANALOG_INPUTS: 
// For classic boards: Set to 0, 1, or 2
//   1 enables Radio In 1 as an analog Input
//   2 also enables Radio In 2 as another analog Input
//   NOTE: Can only be set this higher than 0 if USE_PPM_INPUT is enabled above.
// For UDB4 boards: Set to 0-4.  Analog pins are AN15 - AN18.
//#define NUM_ANALOG_INPUTS                   3


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT                            1.0


////////////////////////////////////////////////////////////////////////////////
// Hardware In the Loop Simulation
// Only set this to 1 for testing in the simulator.  Do not try to fly with this set to 1!
// Requires setting GPS_TYPE to GPS_UBX_4HZ.
// See the MatrixPilot wiki for more info on using HILSIM.
#define HILSIM                              0


////////////////////////////////////////////////////////////////////////////////
// the following define is used to test the above gains and parameters.
// if you define TestGains, their functions will be enabled, even without GPS or Tx turned on.
// #define TestGains                        // uncomment this line if you want to test your gains without using GPS


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
