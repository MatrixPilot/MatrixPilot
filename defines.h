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


#include "p30f4011.h"
#include "options.h"


typedef char boolean;
#define true	1
#define false	0


// Max inputs and outputs
#define MAX_INPUTS	5
#define MAX_OUTPUTS	6


void gps_startup_sequence(int gpscount) ;
boolean gps_nav_valid(void) ;

void init_clock(void) ;
void set_gps2(void) ;
void init_T3(void) ;
void init_ADC(void) ;
void init_pwm(void) ;
void init_capture(void) ;
void init_GPS2(void) ;
void init_USART1(void) ;
void init_states(void) ;
void init_behavior( void ) ;
void init_I2C ( void ) ;

int cosine ( signed char angle ) ;
int sine ( signed char angle ) ;
signed char arcsine( int y ) ;

void setup_origin_2D_location(void) ;
void navigate(void) ;
int determine_navigation_deflection(char navType);

void estimateWind(void) ;
void rxMagnetometer(void) ;

void state_machine(void) ;

void yawCntrl(void) ;
void rollCntrl(void) ;
void pitchCntrl(void) ;
void altitudeCntrl(void) ;
void cameraCntrl(void) ;
void mixServos(void) ;
void setBehavior(int newBehavior) ;
void updateBehavior(void) ;
void updateTriggerAction(void) ;

void serial_output( char* format, ... ) ;
void serial_output_4hz(void) ;

void processwaypoints(void) ;
void init_waypoints( int waypointSetIndex ) ;

boolean canStabilizeInverted(void) ;
boolean canStabilizeHover(void) ;

int pulsesat(long) ;


struct waypoint2D { long x ; long y ; } ;
struct waypoint3D { long x ; long y ; int z ; } ;
struct waypointDef { struct waypoint3D loc ; int flags ; struct waypoint3D viewpoint ; } ;

extern struct waypoint3D GPSlocation ;
extern struct waypoint3D GPSvelocity ;
extern union longww IMUvelocityx , IMUvelocityy , IMUvelocityz   ;
extern union longww IMUlocationx , IMUlocationy , IMUlocationz   ;
#define IMUheight IMUlocationz._.W1 

extern struct relative2D velocity_thru_air ; // derived horizontal velocity relative to air in cm/sec
extern struct relative2D togoal ;

extern int    estimatedWind[3] ;			// wind velocity vectors in cm / sec

extern struct waypoint3D camera_view ;
struct waypointparameters { int x ; int y ; int cosphi ; int sinphi ; signed char phi ; int height ; int fromHeight; int legDist; } ;


extern unsigned int rise[MAX_INPUTS+1] ;	// rising edge clock capture for radio inputs
extern int pwIn[MAX_INPUTS+1] ;	// pulse widths of radio inputs
extern int pwTrim[MAX_INPUTS+1] ;	// initial pulse widths for trimming
extern int pwOut[MAX_OUTPUTS+1] ;	// pulse widths for servo outputs

extern int pitch_control, roll_control, yaw_control, altitude_control ;

extern struct ADchannel xaccel, yaccel , zaccel ;	// x, y, and z accelerometer channels
extern struct ADchannel xrate , yrate, zrate ; 		// x, y, and z gyro channels
extern struct ADchannel vref ;						// reference voltage

extern int firstsamp ;	// used on startup to detect first A/D sample
extern int calibcount ;	// number of PWM pulses before control is turned on

extern union longbbbb lat_gps , long_gps , alt_sl_gps , tow ;
extern union intbb    sog_gps , cog_gps , climb_gps, week_no ;
extern unsigned char  hdop ;
extern union longbbbb xpg , ypg , zpg ;
extern union intbb    xvg , yvg , zvg ;
extern unsigned char  mode1 , mode2 , svs , hdop;

extern unsigned char  	lat_cir ;
extern int				cos_lat ;

extern union longbbbb lat_origin , long_origin , alt_origin ;

extern signed char	desired_dir , actual_dir ;
extern signed char  calculated_heading ; // takes into account wind velocity

extern int tofinish_line ;
extern int progress_to_goal ; // Fraction of the way to the goal in the range 0-4096 (2^12)

extern int waggle, calib_timer, standby_timer, pulsesselin ;
extern int gps_data_age ;

extern union longww throttleFiltered ;
extern int pitchAltitudeAdjust ;

extern signed char GPS_pitch  ;

extern int velocity_magnitude ;
extern int forward_acceleration  ;
extern int velocity_previous  ;
extern int air_speed_magnitude;

extern boolean needSaveExtendedState ;
extern boolean timer_5_on ;

extern int defaultCorcon ;
extern unsigned int cpu_timer ;
extern int magMessage ;

//#define indicate_loading_main		//LATEbits.LATE4 = 0
//#define indicate_loading_inter	//LATEbits.LATE4 = 1

// Empirical results show that reading and writing to the
// "Timer On" function loses clock cycles in the timer. 
// So the software makes a test using a parallel variable
// called timer_5_on.
#define indicate_loading_inter		if ( timer_5_on == 0 )	\
									{						\
										T5CONbits.TON = 1 ;	\
										timer_5_on = 1 ;	\
									}

#define indicate_loading_main		if ( timer_5_on == 1 )	\
									{						\
										T5CONbits.TON = 0 ;	\
										timer_5_on = 0 ;	\
									}


// When ISRs fire during dsp math calls, state is not preserved properly, so we
// have to help preserve extra register state on entry and exit from ISRs.
// In addition, the dsp math calls change and restore CORCON internally, so
// if we fire an ISR in the middle of a dsp math call, CORCON can be set to
// an unexpected value, so we also restore CORCON to the application default,
// which we save in main().  We keep track of whether or not we're running dsp
// calls in needSaveExtendedState var, and only perform these actions if so.
#define interrupt_save_extended_state \
	{ \
		if (needSaveExtendedState) { \
			__asm__("push CORCON"); \
			__asm__("push SR"); \
			__asm__("push MODCON"); \
			__asm__("push XBREV"); \
			__asm__("push ACCAL"); \
			__asm__("push ACCAH"); \
			__asm__("push ACCAU"); \
			__asm__("push ACCBL"); \
			__asm__("push ACCBH"); \
			__asm__("push ACCBU"); \
			__asm__("push RCOUNT"); \
			__asm__("push DCOUNT"); \
			__asm__("push DOSTARTL"); \
			__asm__("push DOSTARTH"); \
			__asm__("push DOENDL"); \
			__asm__("push DOENDH"); \
			int asmDoRestoreExtendedState = 1; \
			__asm__("push %0" : "+r"(asmDoRestoreExtendedState)); \
			CORCON = defaultCorcon; \
			needSaveExtendedState = 0; \
		} \
		else \
		{ \
			int asmDoRestoreExtendedState = 0; \
			__asm__("push %0" : "+r"(asmDoRestoreExtendedState)); \
		} \
	}

#define interrupt_restore_extended_state \
	{ \
		int asmDoRestoreExtendedState; \
		__asm__("pop %0" : "+r"(asmDoRestoreExtendedState)); \
		if (asmDoRestoreExtendedState) { \
			__asm__("pop DOENDH"); \
			__asm__("pop DOENDL"); \
			__asm__("pop DOSTARTH"); \
			__asm__("pop DOSTARTL"); \
			__asm__("pop DCOUNT"); \
			__asm__("pop RCOUNT"); \
			__asm__("pop ACCBU"); \
			__asm__("pop ACCBH"); \
			__asm__("pop ACCBL"); \
			__asm__("pop ACCAU"); \
			__asm__("pop ACCAH"); \
			__asm__("pop ACCAL"); \
			__asm__("pop XBREV"); \
			__asm__("pop MODCON"); \
			__asm__("pop SR"); \
			__asm__("pop CORCON"); \
			needSaveExtendedState = 1; \
		} \
	}


// Channel numbers on the board, mapped to positions in the pulse width arrays.
#define CHANNEL_UNUSED	0	// pwIn[0], pwOut[0], etc. are not used, but allow lazy code everywhere else  :)
#define CHANNEL_1		1
#define CHANNEL_2		2
#define CHANNEL_3		3
#define CHANNEL_4		4
#define CHANNEL_5		5
#define CHANNEL_6		6


// Build for the red or green board
#define RED_BOARD		1
#define GREEN_BOARD		2
#define RED_GREEN_BOARD	3	// Test board for Inversense Gyros


// Choose the type of air frame by setting AIRFRAME_TYPE in options.h
// See options.h for a description of each type
#define AIRFRAME_STANDARD			0
#define AIRFRAME_VTAIL				1
#define AIRFRAME_DELTA				2
#define AIRFRAME_HELI				3


// Pin locations of the hardware toggle switches
#define HW_SWITCH_1			(PORTDbits.RD3 == 0)
#define HW_SWITCH_2			(PORTDbits.RD2 == 0)
#define HW_SWITCH_3			(PORTFbits.RF6 == 0)


// LED pins
#define LED_RED				LATFbits.LATF0
#define LED_GREEN			LATFbits.LATF1
#define LED_RED_DO_TOGGLE	__builtin_btg( (unsigned int*)&LATF , 0 )
#define LED_GREEN_DO_TOGGLE	__builtin_btg( (unsigned int*)&LATF , 1 )

// LED states
#define LED_ON		0
#define LED_OFF		1

// Serial Output Format
#define SERIAL_NONE			0	// No serial data is sent
#define SERIAL_DEBUG		1	// UAV Dev Board debug info
#define SERIAL_ARDUSTATION	2	// Compatible with ArduStation
#define SERIAL_UDB			3	// Pete's efficient UAV Dev Board format
#define SERIAL_OSD_REMZIBI	4	// Output data formatted to use as input to a Remzibi OSD (only works with GPS_UBX)
#define SERIAL_OSD_IF		5	// Output data formatted to use as input to a IF OSD (only works with GPS_UBX)
#define SERIAL_MAGNETOMETER	6	// Debugging the magnetometer
#define SERIAL_UDB_EXTRA	7	// Extra Telemetry beyond that provided by SERIAL_UDB for higher bandwidth connections

// GPS Type
#define GPS_STD				1
#define GPS_UBX_2HZ			2
#define GPS_UBX_4HZ			4

#define GPS_RATE			GPS_TYPE //Hah hah, this works for now

// Failsafe Type
#define FAILSAFE_RTL		1
#define FAILSAFE_WAYPOINTS	2

// AltitudeHold type
#define AH_NONE				0
#define AH_PITCH_ONLY		1
#define AH_FULL				3

// If GPS data has not been received for this many state machine cycles, consider the GPS lock to be lost.
#define GPS_DATA_MAX_AGE	9


// Negate VALUE if NEEDS_REVERSING is true
#define REVERSE_IF_NEEDED(NEEDS_REVERSING, VALUE)		((NEEDS_REVERSING) ? (-(VALUE)) : (VALUE))


#if (BOARD_TYPE == RED_BOARD)
#include "ConfigRed.h"
#elif (BOARD_TYPE == GREEN_BOARD)
#include "ConfigGreen.h"
#elif (BOARD_TYPE == RED_GREEN_BOARD)
#include "ConfigIXZ500.h"
#endif


#define tmr1_period 0x2000 // sets time period for timer 1 interrupt to 0.5 seconds
//#define FILTERSHIFT 6 // filter shift divide
#define FILTERSHIFT 3 
#define LONGDEG_2_BYTECIR 305 // = (256/360)*((256)**4)/(10**7)
#define COURSEDEG_2_BYTECIR 466 // = (256/360)*((256)**2)/(10**2)
#define CPU_LOAD_PERCENT	400   // = (100 / (8192 * 2)) * (256**2)
#define CALIB_PAUSE 12
#define STANDBY_PAUSE 48 // pause for 16 seconds of runs through the state machine
#define NUM_WAGGLES 4 // waggle 4 times during the end of the standby pause (this number must be less than STANDBY_PAUSE)
#define WAGGLE_SIZE 300

#define SERVORANGE (int) (SERVOSAT*1000)
#define SERVOMAX 3000 + SERVORANGE
#define SERVOMIN 3000 - SERVORANGE

#define ACCTAU 200 // in milliseconds
#define RATETAU 100

#define ACCTBYTAU 5120/ACCTAU	// 256*(time_step/time_constant)
#define RATETBYTAU 5120/RATETAU

struct behavior_flag_bits {
			unsigned int takeoff		: 1 ;				// unimplemented
			unsigned int inverted		: 1 ;	// fly iverted
			unsigned int hover			: 1 ;	// hover the plane
			unsigned int rollLeft		: 1 ;				// unimplemented
			unsigned int rollRight		: 1 ;				// unimplemented
			unsigned int trigger		: 1 ;	// trigger action
			unsigned int loiter			: 1 ;	// stay on the current waypoint
			unsigned int land			: 1 ;	// throttle off
			unsigned int absolute		: 1 ;	// absolute waypoint
			unsigned int altitude		: 1 ;	// climb/descend to goal altitude
			unsigned int climbout		: 1 ;	// no linear interpolation on altitude control
			unsigned int unused			: 5 ;
			} ;

#define F_NORMAL						0
#define F_TAKEOFF						1
#define F_INVERTED						2
#define F_HOVER							4
#define F_ROLL_LEFT						8
#define F_ROLL_RIGHT					16
#define F_TRIGGER						32
#define F_LOITER						64
#define F_LAND							128
#define F_ABSOLUTE						256
#define F_ALTITUDE_GOAL					512
#define F_CLIMBOUT						1024

union bfbts_word { struct behavior_flag_bits _ ; int W ; };

extern int current_orientation ;
extern union bfbts_word desired_behavior ;


#define TRIGGER_TYPE_NONE				0
#define TRIGGER_TYPE_SERVO				1
#define TRIGGER_TYPE_DIGITAL			2

#define TRIGGER_PULSE_HIGH				4
#define TRIGGER_PULSE_LOW				8
#define TRIGGER_TOGGLE					16
#define TRIGGER_REPEATING				32


#define WIND_NAV_AIR_SPEED_MIN			200		// Minimum airspeed in cm/sec for wind navigation to apply
#define GPS_SPEED_MIN					150		// Minimum ground speed in cm/sec to use GPS for yaw drift compensation

#define CAM_VIEW_LAUNCH					{ 0, 0, 0 }

