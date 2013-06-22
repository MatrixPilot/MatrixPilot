//
//  SIL-udb.h
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef WIN

#define SIL_WINDOWS_INCS
#include <Windows.h>
#include <Time.h>

struct timezone
{
	int tz_minuteswest; /* of Greenwich */
	int tz_dsttime;     /* type of dst correction to apply */
};

int gettimeofday (struct timeval *tp, struct timezone *tzp);

#else

#include <sys/time.h>
#include <unistd.h>

#endif


#include "libUDB.h"
#include "magnetometer.h"
#include "magnetometerOptions.h"
#include "events.h"
#include "SIL-udb.h"
#include "UDBSocket.h"
#include "SIL-ui.h"
#include "SIL-events.h"
#include "SIL-eeprom.h"

uint16_t udb_heartbeat_counter;

int16_t udb_pwIn[MAX_INPUTS];		// pulse widths of radio inputs
int16_t udb_pwTrim[MAX_INPUTS];	// initial pulse widths for trimming
int16_t udb_pwOut[MAX_OUTPUTS];		// pulse widths for servo outputs

union udb_fbts_byte udb_flags;

struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel;	// x, y, and z accelerometer channels
struct ADchannel udb_xrate, udb_yrate, udb_zrate;	// x, y, and z gyro channels
struct ADchannel udb_vref;							// reference voltage
struct ADchannel udb_analogInputs[4];


int16_t udb_magFieldBody[3] ;  // magnetic field in the body frame of reference
int16_t udb_magOffset[3] = { 0 , 0 , 0 } ;  // magnetic offset in the body frame of reference
int16_t magGain[3] = { RMAX , RMAX , RMAX } ; // magnetometer calibration gains
int16_t rawMagCalib[3] = { 0 , 0 , 0 } ;
uint8_t magreg[6] ;  // magnetometer read-write buffer
int16_t magFieldRaw[3] ;


union longww battery_current;	// battery_current._.W1 is in tenths of Amps
union longww battery_mAh_used;	// battery_mAh_used._.W1 is in mAh
union longww battery_voltage;	// battery_voltage._.W1 is in tenths of Volts
uint8_t rc_signal_strength;	// rc_signal_strength is 0-100 as percent of full signal

int16_t magMessage ;
int16_t vref_adj ;

volatile int16_t trap_flags;
volatile int32_t trap_source;
volatile int16_t osc_fail_count ;
uint16_t mp_rcon = 3; // default RCON state at normal powerup

extern int mp_argc;
extern char **mp_argv;


uint8_t leds[4] = {0, 0, 0, 0};

UDBSocket serialSocket;

uint8_t sil_radio_on;

boolean handleUDBSockets(void);

uint16_t get_current_milliseconds();
void sleep_milliseconds(uint16_t ms);


#define UDB_HW_RESET_ARG "-r=EXTR"


void udb_init(void)
{
	// If we were reest:
	if (mp_argc >= 2 && strcmp(mp_argv[1], UDB_HW_RESET_ARG) == 0) {
		mp_rcon = 128; // enable just the external/MCLR reset bit
	}
	
	int16_t i;
	for (i=0; i<4; i++) {
		leds[i] = LED_OFF;
	}
	
	udb_heartbeat_counter = 0;
	
	udb_flags.B = 0;
	sil_radio_on = 1;
	
	sil_ui_init(mp_rcon);
	
	gpsSocket = UDBSocket_init((SILSIM_GPS_RUN_AS_SERVER) ? UDBSocketUDPServer : UDBSocketUDPClient, SILSIM_GPS_PORT, SILSIM_GPS_HOST, NULL, 0);
	telemetrySocket = UDBSocket_init((SILSIM_TELEMETRY_RUN_AS_SERVER) ? UDBSocketUDPServer : UDBSocketUDPClient, SILSIM_TELEMETRY_PORT, SILSIM_TELEMETRY_HOST, NULL, 0);
	
	if (strlen(SILSIM_SERIAL_RC_INPUT_DEVICE) > 0) {
		serialSocket = UDBSocket_init(UDBSocketSerial, 0, NULL, SILSIM_SERIAL_RC_INPUT_DEVICE, SILSIM_SERIAL_RC_INPUT_BAUD);
	}
}


#define UDB_STEP_TIME 25
#define UDB_WRAP_TIME 1000

void udb_run(void)
{
	uint16_t currentTime;
	uint16_t nextHeartbeatTime;
	
	
	if (strlen(SILSIM_SERIAL_RC_INPUT_DEVICE) == 0) {
		udb_pwIn[THROTTLE_INPUT_CHANNEL] = 2000;
		udb_pwTrim[THROTTLE_INPUT_CHANNEL] = 2000;
	}
	
	nextHeartbeatTime = get_current_milliseconds();
	
	while (1) {
		if (!handleUDBSockets()) {
			sleep_milliseconds(1);
		}
		
		currentTime = get_current_milliseconds();
		
		if (currentTime >= nextHeartbeatTime && !(nextHeartbeatTime <= UDB_STEP_TIME && currentTime >= UDB_WRAP_TIME-UDB_STEP_TIME)) {
			udb_callback_read_sensors();
			
			udb_flags._.radio_on = (sil_radio_on && udb_pwIn[FAILSAFE_INPUT_CHANNEL] >= FAILSAFE_INPUT_MIN && udb_pwIn[FAILSAFE_INPUT_CHANNEL] <= FAILSAFE_INPUT_MAX);
			LED_GREEN = (udb_flags._.radio_on) ? LED_ON : LED_OFF ;

			udb_background_callback_periodic(); // Run at 40Hz
			udb_servo_callback_prepare_outputs();
			
			sil_ui_update();
			
			if (udb_heartbeat_counter % 80 == 0) writeEEPROMFileIfNeeded(); // Run at 0.5Hz
			
			udb_heartbeat_counter++;
			nextHeartbeatTime = nextHeartbeatTime + UDB_STEP_TIME;
			if (nextHeartbeatTime > UDB_WRAP_TIME) nextHeartbeatTime -= UDB_WRAP_TIME;
		}
		process_queued_events();
	}
}


void udb_background_trigger(void)
{
	udb_background_callback_triggered();
}


uint8_t udb_cpu_load(void)
{
	return 5; // sounds reasonable for a fake cpu%
}


int16_t  udb_servo_pulsesat(int32_t pw)
{
	if ( pw > SERVOMAX ) pw = SERVOMAX ;
	if ( pw < SERVOMIN ) pw = SERVOMIN ;
	return (int16_t)pw ;
}


void udb_servo_record_trims(void)
{
	int16_t i;
	for (i=1; i <= NUM_INPUTS; i++)
		udb_pwTrim[i] = udb_pwIn[i] ;
	
	return ;
}


void udb_set_action_state(boolean newValue)
{
	// not simulated
	(void)newValue;   // unused parameter
}


void udb_a2d_record_offsets(void)
{
	UDB_XACCEL.offset = UDB_XACCEL.value ;
	udb_xrate.offset = udb_xrate.value ;
	UDB_YACCEL.offset = UDB_YACCEL.value - ( Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY)) ); // opposite direction
	udb_yrate.offset = udb_yrate.value ;
	UDB_ZACCEL.offset = UDB_ZACCEL.value ;
	udb_zrate.offset = udb_zrate.value ;
	udb_vref.offset = udb_vref.value ;
}


uint16_t get_reset_flags(void)
{
	return mp_rcon;
}


void sil_reset(void)
{
	sil_ui_will_reset();
	
	if (gpsSocket) UDBSocket_close(gpsSocket);
	if (telemetrySocket) UDBSocket_close(telemetrySocket);
	if (serialSocket) UDBSocket_close(serialSocket);
	
	char *args[3] = {mp_argv[0], UDB_HW_RESET_ARG, 0};
	execv(mp_argv[0], args);
	fprintf(stderr, "Failed to reset UDB %s\n", mp_argv[0]);
	exit(1);
}


// time functions

uint16_t get_current_milliseconds()
{
	// *nix / mac implementation
	struct timeval tv;
	struct timezone tz;
	
	gettimeofday(&tv,&tz);
	return tv.tv_usec / 1000;
}


void sleep_milliseconds(uint16_t ms)
{
#ifdef WIN
	// windows implementation
	Sleep(ms);
	
#else
	// *nix / mac implementation
	usleep(1000*ms);
#endif
}


void sil_handle_seial_rc_input(uint8_t *buffer, int bytesRead)
{
	int i;
	
	uint8_t CK_A = 0 ;
	uint8_t CK_B = 0 ;
	
	uint8_t headerBytes = 0;
	uint8_t numServos = 0;
	
	if (bytesRead >= 2 && buffer[0]==0xFF && buffer[1]==0xEE) {
		headerBytes = 2;
		numServos = 8;
	}
	else if (bytesRead >= 3 && buffer[0]==0xFE && buffer[1]==0xEF) {
		headerBytes = 3;
		numServos = buffer[2];
	}
	
	if (numServos && bytesRead >= headerBytes + numServos*2 + 2) {
		for (i=headerBytes; i < headerBytes + numServos*2; i++)
		{
			CK_A += buffer[i] ;
			CK_B += CK_A ;
		}
		if (CK_A == buffer[headerBytes + numServos*2] && CK_B == buffer[headerBytes + numServos*2 + 1]) {
			for (i=1; i <= numServos; i++) {
				udb_pwIn[i] = (uint16_t)(buffer[headerBytes + (i-1)*2])*256 + buffer[headerBytes + (i-1)*2 + 1];
			}
		}
	}
}


#define BUFLEN 512

boolean handleUDBSockets(void)
{
	uint8_t buffer[BUFLEN];
	int32_t bytesRead;
	int16_t i;
	boolean didRead = false;
	
	// Handle GPS Socket
	if (gpsSocket) {
		bytesRead = UDBSocket_read(gpsSocket, buffer, BUFLEN);
		if (bytesRead < 0) {
			UDBSocket_close(gpsSocket);
			gpsSocket = NULL;
		}
		else {
			for (i=0; i<bytesRead; i++) {
				udb_gps_callback_received_byte(buffer[i]);
			}
			if (bytesRead>0) didRead = true;
		}
	}
	
	// Handle Telemetry Socket
	if (telemetrySocket) {
		bytesRead = UDBSocket_read(telemetrySocket, buffer, BUFLEN);
		if (bytesRead < 0) {
			UDBSocket_close(telemetrySocket);
			telemetrySocket = NULL;
		}
		else {
			for (i=0; i<bytesRead; i++) {
				udb_serial_callback_received_byte(buffer[i]);
			}
			if (bytesRead>0) didRead = true;
		}
	}
	
	// Handle optional Serial RC input Socket
	if (serialSocket) {
		bytesRead = UDBSocket_read(serialSocket, buffer, BUFLEN);
		if (bytesRead < 0) {
			UDBSocket_close(serialSocket);
			serialSocket = NULL;
		}
		else {
			if (bytesRead>0) {
				sil_handle_seial_rc_input(buffer, bytesRead);
				didRead = true;
			}
		}
	}
	
	return didRead;
}


#if  (MAG_YAW_DRIFT == 1)
void I2C_doneReadMagData(void)
{
	magFieldRaw[0] = (magreg[0]<<8)+magreg[1] ;
	magFieldRaw[1] = (magreg[2]<<8)+magreg[3] ;
	magFieldRaw[2] = (magreg[4]<<8)+magreg[5] ;
	
	if ( magMessage == 7 )
	{
		udb_magFieldBody[0] = MAG_X_SIGN((__builtin_mulsu((magFieldRaw[MAG_X_AXIS]), magGain[MAG_X_AXIS] ))>>14)-(udb_magOffset[0]>>1) ;
		udb_magFieldBody[1] = MAG_Y_SIGN((__builtin_mulsu((magFieldRaw[MAG_Y_AXIS]), magGain[MAG_Y_AXIS] ))>>14)-(udb_magOffset[1]>>1) ;
		udb_magFieldBody[2] = MAG_Z_SIGN((__builtin_mulsu((magFieldRaw[MAG_Z_AXIS]), magGain[MAG_Z_AXIS] ))>>14)-(udb_magOffset[2]>>1) ;
		
		if ( ( abs(udb_magFieldBody[0]) < MAGNETICMAXIMUM ) &&
			( abs(udb_magFieldBody[1]) < MAGNETICMAXIMUM ) &&
			( abs(udb_magFieldBody[2]) < MAGNETICMAXIMUM ) )
		{
			udb_magnetometer_callback();
		}
		else
		{
			magMessage = 0 ; // invalid reading, reset the magnetometer
		}
	}
}

void HILSIM_MagData(void)
{
	magMessage = 7 ; // indicate valid magnetometer data
	I2C_doneReadMagData() ; // run the magnetometer computations
}

#endif
