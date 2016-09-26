// This file is part of MatrixPilot.
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
// mavlink_options.h
//

#ifndef USE_MAVLINK
//#define USE_MAVLINK                         1          //the intended new method for configuring Mavlink
//#define MAVLINK_BAUD                        57600      //the intended new method for configuring Mavlink
#endif

#ifndef USE_MAVLINK
#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)             //support the current method for configuring Mavlink
#define USE_MAVLINK                         1
#endif
#endif

#ifndef USE_MAVLINK
#define USE_MAVLINK                         0
#define MAVLINK_BAUD                        0
#endif

// MAVLink requires an aircraft Identifier (I.D) as it is designed to control multiple aircraft
// Each aircraft in the sky will need a unique I.D. in the range from 0-255
#define MAVLINK_SYSID                       1

// Common data rates
#define MAVLINK_RATE_RC_CHAN                0   // RC_CHANNELS_SCALED, RC_CHANNELS_RAW, SERVO_OUTPUT_RAW |
#define MAVLINK_RATE_IMU_RAW                0   // ATTITUDE_CONTROLLER_OUTPUT, POSITION_CONTROLLER_OUTPUT, NAV_CONTROLLER_OUTPUT. |
#define MAVLINK_RATE_POSITION               8   // LOCAL_POSITION, GLOBAL_POSITION/GLOBAL_POSITION_INT messages. |
#define MAVLINK_RATE_RAW_SENSORS            2   // IMU_RAW, GPS_RAW, GPS_STATUS packets.

// Fixed data rates
#define MAVLINK_RATE_HEARTBEAT              4
#define MAVLINK_RATE_SYSTEM_STATUS          4

// Matrixpilot specific data rates
#define MAVLINK_RATE_SUE                    8   // SERIAL_UDB_EXTRA data rate on channel EXTRA1
#define MAVLINK_RATE_FORCE                  4   // Send FORCE on plane (Aerodynamic force)
#define MAVLINK_RATE_POSITION_SENSORS       0   // Using channel EXTRA2

// Send VFR_HUD message at position rate, 1=yes, 0=no.  Needed for correct mavproxy state
#define MSG_VFR_HUD_WITH_POSITION           1

#define MAVLINK_FRAME_FREQUENCY             40
#define MAVLINK_WAYPOINT_TIMEOUT            120 // Dependent on frequency of calling mavlink_output_40hz. 120 is 3 second timeout.

// 19200,38400,57600,115200,230400,460800,921600
// Fixed 19200 for non free running clock
//#define MAVLINK_BAUD                        19200   // now using SERIAL_BAUDRATE in options.h
//#define MAVLINK_BAUD                        115200

// Include code to remove rounding errors on PID values when using QGroundControl
// 1 = Yes, 0 = No
#define QGROUNDCTONROL_PID_COMPATIBILITY    1

