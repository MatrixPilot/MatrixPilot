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
void initBehavior( void ) ;

int cosine ( signed char angle ) ;
int sine ( signed char angle ) ;

void navigate(void) ;

void state_machine(void) ;
void filterInputs(void) ;

void yawCntrl(void) ;
void rollCntrl(void) ;
void pitchCntrl(void) ;
void altitudeCntrl(void) ;
void cameraCntrl(void) ;
void mixServos(void) ;
void updateBehavior(void) ;

void serial_output( char* format, ... ) ;
void serial_output_gps(void) ;

void processwaypoints(void) ;
void init_waypoints(void) ;

int pulsesat(long) ;


struct relative3D { int x ; int y ; int z ; } ;

struct absolute2D { long Lat ; long Long ; } ;

struct waypointDef { struct relative3D loc ; int flags ; } ;

extern struct relative3D GPSlocation ;
extern struct relative3D GPSvelocity ;

extern union longww IMUlocationx , IMUlocationy , IMUlocationz   ;
extern struct relative3D IMUvelocity ;
struct waypointparameters { int x ; int y ; int cosphi ; int sinphi ; signed char phi ; int height ; } ;


extern unsigned int rise[MAX_INPUTS+1] ;	// rising edge clock capture for radio inputs
extern int pwIn[MAX_INPUTS+1] ;	// pulse widths of radio inputs
extern int pwTrim[MAX_INPUTS+1] ;	// initial pulse widths for trimming
extern int pwOut[MAX_OUTPUTS+1] ;	// pulse widths for servo outputs

extern int pitch_control, roll_control, yaw_control, altitude_control ;

extern struct ADchannel xaccel, yaccel , zaccel ;	// x, y, and z accelerometer channels
extern struct ADchannel xrate , yrate, zrate ; 		// x, y, and z gyro channels
extern struct ADchannel vref ;						// reference voltage

extern int dutycycle ;	// used to compute PWM duty cycle
extern int firstsamp ;	// used on startup to detect first A/D sample
extern int calibcount ;	// number of PWM pulses before control is turned on

extern union longbbbb lat_gps , long_gps , alt_sl_gps , tow ;
extern union intbb    sog_gps , cog_gps , climb_gps, week_no ;
extern unsigned char  hdop ;
extern union longbbbb xpg , ypg , zpg ;
extern union intbb    xvg , yvg , zvg ;
extern unsigned char  mode1 , mode2 , svs ;

extern unsigned char  	lat_cir ;
extern int				cos_lat ;

extern union longbbbb lat_origin , long_origin , alt_origin ;
extern union longbbbb x_origin , y_origin , z_origin ;

extern signed char	desired_dir , actual_dir ;

extern int height ;

extern int waggle, calib_timer, standby_timer, pulsesselin ;
extern int gps_data_age;

extern union longww throttleFiltered ;
extern int pitchAltitudeAdjust ;

extern signed char GPS_pitch  ;

extern int velocity_magnitude ;
extern int forward_acceleration  ;
extern int velocity_previous  ;

extern boolean needSaveExtendedState ;
extern boolean timer_5_on ;

extern int defaultCorcon ;
extern unsigned int cpu_timer ;

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
#define interrupt_save_extended_state		if (needSaveExtendedState) { \
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
												CORCON = defaultCorcon; \
											}
#define interrupt_restore_extended_state	if (needSaveExtendedState) { \
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
#define RED_BOARD	1
#define GREEN_BOARD	2


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

// GPS Type
#define GPS_STD				1
#define GPS_UBX				2

#define GPS_RATE			((GPS_TYPE == GPS_UBX) ? 4 : 1)


// If GPS data has not been received for this many state machine cycles, consider the GPS lock to be lost.
#define GPS_DATA_MAX_AGE	9


// Negate VALUE if NEEDS_REVERSING is true
#define REVERSE_IF_NEEDED(NEEDS_REVERSING, VALUE)		((NEEDS_REVERSING) ? (-(VALUE)) : (VALUE))


#if (BOARD_TYPE == RED_BOARD)
#include "ConfigRed.h"
#elif (BOARD_TYPE == GREEN_BOARD)
#include "ConfigGreen.h"
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

#define SERVORANGE (int) SERVOSAT*1000
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
			unsigned int trigger		: 1 ;				// unimplemented
			unsigned int loiter			: 1 ;	// stay on the current waypoint
			unsigned int land			: 1 ;	// throttle off
			unsigned int unused			: 8 ;
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

union bfbts_word { struct behavior_flag_bits _ ; int W ; };

extern int current_orientation ;
extern union bfbts_word desired_behavior ;
