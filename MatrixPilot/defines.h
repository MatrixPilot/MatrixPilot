// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


#include "../libDCM/libDCM.h"


#define BYTECIR_TO_DEGREE 92160		// (360.0/256 * 2^16)


////////////////////////////////////////////////////////////////////////////////
// states.c
void init_states( void ) ;

extern int waggle ;

#define CALIB_PAUSE 21
#define STANDBY_PAUSE 48 // pause for 16 seconds of runs through the state machine
#define NUM_WAGGLES 4 // waggle 4 times during the end of the standby pause (this number must be less than STANDBY_PAUSE)
#define WAGGLE_SIZE 300

struct flag_bits {
			unsigned int unused					: 7 ;
			unsigned int save_origin   			: 1 ;
			unsigned int GPS_steering			: 1 ;
			unsigned int pitch_feedback			: 1 ;
			unsigned int altitude_hold_throttle	: 1 ;
			unsigned int altitude_hold_pitch	: 1 ;
			unsigned int man_req				: 1 ;
			unsigned int auto_req				: 1 ;
			unsigned int home_req				: 1 ;
			unsigned int f13_print_req			: 1 ;
			} ;
			
union fbts_int { struct flag_bits _ ; int WW ; } ;
extern union fbts_int flags ;



////////////////////////////////////////////////////////////////////////////////
// servoPrepare.c
void init_servoPrepare( void ) ;



////////////////////////////////////////////////////////////////////////////////
// Control code - rollCntrl.c, pitchCntrl.c, yawCntrl.c, altitudeCntrl.c
void rollCntrl( void ) ;
void pitchCntrl( void ) ;
void yawCntrl( void ) ;
void altitudeCntrl( void ) ;

extern int pitch_control, roll_control, yaw_control, altitude_control ;
extern union longww throttleFiltered ;
extern int pitchAltitudeAdjust ;

// AltitudeHold type
#define AH_NONE				0
#define AH_PITCH_ONLY		1
#define AH_FULL				3



////////////////////////////////////////////////////////////////////////////////
// servoMix.c
void servoMix( void ) ;

// Choose the type of air frame by setting AIRFRAME_TYPE in options.h
// See options.h for a description of each type
#define AIRFRAME_STANDARD			0
#define AIRFRAME_VTAIL				1
#define AIRFRAME_DELTA				2
#define AIRFRAME_HELI				3		// Untested

// Negate VALUE if NEEDS_REVERSING is true
#define REVERSE_IF_NEEDED(NEEDS_REVERSING, VALUE)		((NEEDS_REVERSING) ? (-(VALUE)) : (VALUE))



////////////////////////////////////////////////////////////////////////////////
// navigation.c
void set_goal( struct relative3D fromPoint , struct relative3D toPoint ) ;
void compute_bearing_to_goal ( void ) ;
void process_flightplan( void ) ;
int determine_navigation_deflection( char navType ) ;

struct waypointparameters { int x ; int y ; int cosphi ; int sinphi ; signed char phi ; int height ; int fromHeight; int legDist; } ;
extern struct waypointparameters goal ;

extern struct relative2D togoal ;
extern int tofinish_line ;
extern int progress_to_goal ; // Fraction of the way to the goal in the range 0-4096 (2^12)
extern signed char	desired_dir ;



////////////////////////////////////////////////////////////////////////////////
// Flight Planning modules - flightplan-waypoints.c and flightplan-logo.c
void init_flightplan( int flightplanNum ) ;
boolean use_fixed_origin( void ) ;
struct absolute2D get_fixed_origin( void ) ;
long get_fixed_altitude( void ) ;
void run_flightplan( void ) ;

// Failsafe Type
#define FAILSAFE_RTL					1
#define FAILSAFE_MAIN_FLIGHTPLAN		2

#define FP_WAYPOINTS					1
#define FP_LOGO							2



////////////////////////////////////////////////////////////////////////////////
// behavior.c
void init_behavior( void ) ;
void setBehavior( int newBehavior ) ;
void updateBehavior( void ) ;
void updateTriggerAction( void ) ;
boolean canStabilizeInverted( void ) ;
boolean canStabilizeHover( void ) ;

struct behavior_flag_bits {
			unsigned int takeoff		: 1 ;	// disable altitude interpolation for faster climbout
			unsigned int inverted		: 1 ;	// fly iverted
			unsigned int hover			: 1 ;	// hover the plane
			unsigned int rollLeft		: 1 ;				// unimplemented
			unsigned int rollRight		: 1 ;				// unimplemented
			unsigned int trigger		: 1 ;	// trigger action
			unsigned int loiter			: 1 ;	// stay on the current waypoint
			unsigned int land			: 1 ;	// throttle off
			unsigned int absolute		: 1 ;	// absolute waypoint
			unsigned int altitude		: 1 ;	// climb/descend to goal altitude
			unsigned int cross_track	: 1 ;	// use cross-tracking navigation
			unsigned int unused			: 5 ;
			} ;

#define F_NORMAL						   0
#define F_TAKEOFF						   1
#define F_INVERTED						   2
#define F_HOVER							   4
#define F_ROLL_LEFT						   8
#define F_ROLL_RIGHT					  16
#define F_TRIGGER						  32
#define F_LOITER						  64
#define F_LAND							 128
#define F_ABSOLUTE						 256
#define F_ALTITUDE_GOAL					 512
#define F_CROSS_TRACK					1024

union bfbts_word { struct behavior_flag_bits _ ; int W; } ;

extern int current_orientation ;
extern union bfbts_word desired_behavior ;

#define TRIGGER_TYPE_NONE				 0
#define TRIGGER_TYPE_SERVO				 1
#define TRIGGER_TYPE_DIGITAL			 2

#define TRIGGER_PULSE_HIGH				 4
#define TRIGGER_PULSE_LOW				 8
#define TRIGGER_TOGGLE					16
#define TRIGGER_REPEATING				32



////////////////////////////////////////////////////////////////////////////////
// serialIO.c
void init_serial( void ) ;
void serial_output( char* format, ... ) ;
void serial_output_8hz( void ) ;

// Serial Output Format
#define SERIAL_NONE			0	// No serial data is sent
#define SERIAL_DEBUG		1	// UAV Dev Board debug info
#define SERIAL_ARDUSTATION	2	// Compatible with ArduStation
#define SERIAL_UDB			3	// Pete's efficient UAV Dev Board format
#define SERIAL_OSD_REMZIBI	4	// Output data formatted to use as input to a Remzibi OSD (only works with GPS_UBX)
#define SERIAL_OSD_IF		5	// Output data formatted to use as input to a IF OSD (only works with GPS_UBX)
#define SERIAL_MAGNETOMETER	6	// Debugging the magnetometer
#define SERIAL_UDB_EXTRA	7	// Extra Telemetry beyond that provided by SERIAL_UDB for higher bandwidth connections



////////////////////////////////////////////////////////////////////////////////
// cameraCntrl.c
void set_camera_view( struct relative3D current_view ) ;
void compute_camera_view( void ) ;
void cameraCntrl( void ) ;

#define CAM_VIEW_LAUNCH					{ 0, 0, 0 }



////////////////////////////////////////////////////////////////////////////////
// mp_osd.c
void osd_countdown(int countdown);



////////////////////////////////////////////////////////////////////////////////
// GPS Parsers - gpsParseSTD.c, gpsParseUBX.c
// FIXME: This should move into libDCM
void gps_startup_sequence( int gpscount ) ;
