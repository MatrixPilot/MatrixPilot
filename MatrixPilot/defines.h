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

#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "../libDCM/libDCM.h"


#define BYTECIR_TO_DEGREE 92160 // (360.0/256 * 2^16)


////////////////////////////////////////////////////////////////////////////////
// Control code - helicalTurnCntrl.c,rollCntrl.c, pitchCntrl.c, yawCntrl.c, altitudeCntrl.c
void helicalTurnCntrl(void);
void rollCntrl(void);
void pitchCntrl(void);
void yawCntrl(void);
void InnerrollCntrl(void);
void InnerpitchCntrl(void);
void InneryawCntrl(void);
void InneraltitudeCntrl(void);
void altitudeCntrl(void);
void setTargetAltitude(int16_t targetAlt);

void init_yawCntrl(void);
void init_rollCntrl(void);
void init_pitchCntrl(void);
void init_altitudeCntrl(void);
void init_altitudeCntrlVariable(void);

void save_yawCntrl(void);
void save_rollCntrl(void);
void save_pitchCntrl(void);
void save_altitudeCntrl(void);
void save_altitudeCntrlVariable(void);
extern int16_t outerpitch_control, outerroll_control, outeryaw_control,outeraltitude_control;


// Negate VALUE if NEEDS_REVERSING is true
#define REVERSE_IF_NEEDED(NEEDS_REVERSING, VALUE) ((NEEDS_REVERSING) ? (-(VALUE)) : (VALUE))


// ALTITUDEHOLD_STABILIZED and ALTITUDEHOLD_WAYPOINT options
#define AH_NONE             0
#define AH_PITCH_ONLY       1
#define AH_FULL             3


// Choose the type of air frame by setting AIRFRAME_TYPE in options.h
// See options.h for a description of each type
//#define AIRFRAME_STANDARD   0
//#define AIRFRAME_VTAIL      1
//#define AIRFRAME_DELTA      2
//#define AIRFRAME_HELI       3    // Untested
//#define AIRFRAME_QUAD       4    // Under development

// AIRFRAME_TYPE options
#define AIRFRAME_NONE       0
#define AIRFRAME_STANDARD   1
#define AIRFRAME_VTAIL      2
#define AIRFRAME_DELTA      3
#define AIRFRAME_HELI       4    // Untested
#define AIRFRAME_QUAD       5    // Under development
#define AIRFRAME_GLIDER     6    // Under development

// FAILSAFE_TYPE options
#define FAILSAFE_RTL                1
#define FAILSAFE_MAIN_FLIGHTPLAN    2

// FLIGHT_PLAN_TYPE options
#define FP_NONE                     0
#define FP_WAYPOINTS                1
#define FP_LOGO                     2

// TELEMETRY_OUTPUT_FORMAT options
#define SERIAL_NONE           0     // No serial data is sent
#define SERIAL_DEBUG          1     // UAV Dev Board debug info
#define SERIAL_ARDUSTATION    2     // Compatible with ArduStation
#define SERIAL_UDB            3     // Legacy protocol. Superceded by SERIAL_UDB_EXTRA
#define SERIAL_OSD_REMZIBI    4     // Output data formatted to use as input to a Remzibi OSD (only works with GPS_UBX)
#define SERIAL_OSD_IF         5     // Output data formatted to use as input to a IF OSD (only works with GPS_UBX)
#define SERIAL_MAGNETOMETER   6     // Debugging the magnetometer
#define SERIAL_UDB_EXTRA      7     // Extra Telemetry beyond that provided by SERIAL_UDB for higher bandwidth connections
#define SERIAL_CAM_TRACK      8     // Output Location in a format usable by a 2nd UDB to target its camera at this plane
#define SERIAL_MAVLINK        9     // The Micro Air Vehicle Link protocol from the PixHawk Project
#define SERIAL_MAG_CALIBRATE 10     // Used to calibrate and report static magnetometer offsets


#include "gain_variables.h"

// GNU compiler specific macros for specifically marking variables as unused
// If not using GNU, then these macros make no alteration to the code
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

#endif // _DEFINES_H_
