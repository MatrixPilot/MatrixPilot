//
//  libSTM.c
//  MatrixPilot
//
//  Created by Robert Dickenson on 17/6/2014.
//  Copyright (c) 2014 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../libUDB/libUDB.h"
#include "../libUDB/ADchannel.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/magnetometer.h"
#include "magnetometerOptions.h"
#include "../libUDB/events.h"

#if (BOARD_TYPE == PX4_BOARD)

//#include "SIL-config.h"
//#include "SIL-udb.h"
//#include "SIL-ui.h"
//#include "SIL-events.h"
//#include "SIL-eeprom.h"

uint16_t udb_heartbeat_counter;

int16_t udb_pwIn[MAX_INPUTS];   // pulse widths of radio inputs
int16_t udb_pwTrim[MAX_INPUTS]; // initial pulse widths for trimming
int16_t udb_pwOut[MAX_OUTPUTS]; // pulse widths for servo outputs

union udb_fbts_byte udb_flags;

struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel;    // x, y, and z accelerometer channels
struct ADchannel udb_xrate, udb_yrate, udb_zrate;       // x, y, and z gyro channels
struct ADchannel udb_vref;                              // reference voltage
struct ADchannel udb_analogInputs[4];

int16_t udb_magFieldBody[3];                    // magnetic field in the body frame of reference
int16_t udb_magOffset[3] = { 0 , 0 , 0 };       // magnetic offset in the body frame of reference
int16_t magGain[3] = { RMAX , RMAX , RMAX };    // magnetometer calibration gains
int16_t rawMagCalib[3] = { 0 , 0 , 0 };
uint8_t magreg[6];                              // magnetometer read-write buffer
int16_t magFieldRaw[3];

union longww battery_current;                   // battery_current._.W1 is in tenths of Amps
union longww battery_mAh_used;                  // battery_mAh_used._.W1 is in mAh
union longww battery_voltage;                   // battery_voltage._.W1 is in tenths of Volts
uint8_t rc_signal_strength;                     // rc_signal_strength is 0-100 as percent of full signal

int16_t magMessage;
int16_t vref_adj;

volatile uint16_t trap_flags;
volatile uint32_t trap_source;
volatile uint16_t osc_fail_count;
uint16_t mp_rcon = 3;                           // default RCON state at normal powerup

extern int mp_argc;
extern char **mp_argv;

uint8_t leds[5] = {0, 0, 0, 0, 0};
uint8_t sil_radio_on;

// Functions only included with nv memory.
#if (USE_NV_MEMORY == 1)
UDB_SKIP_FLAGS udb_skip_flags = {0, 0, 0};

void udb_skip_radio_trim(boolean b)
{
	udb_skip_flags.skip_radio_trim = 1;
}

void udb_skip_imu_calibration(boolean b)
{
	udb_skip_flags.skip_imu_cal = 1;
}
#endif

void udb_init(void)
{
	udb_heartbeat_counter = 0;
	udb_flags.B = 0;
}

void udb_run(void)
{
}

void udb_background_trigger(background_callback callback)
{
	if (callback) callback();
}

uint8_t udb_cpu_load(void)
{
	return 5; // sounds reasonable for a fake cpu%
}

int16_t udb_servo_pulsesat(int32_t pw)
{
	if (pw > SERVOMAX) pw = SERVOMAX;
	if (pw < SERVOMIN) pw = SERVOMIN;
	return (int16_t)pw;
}

void udb_servo_record_trims(void)
{
	int16_t i;

	for (i = 1; i <= NUM_INPUTS; i++)
	{
		udb_pwTrim[i] = udb_pwIn[i];
//		DPRINT("udb_pwTrim[%i] = %u\r\n", i, udb_pwTrim[i]);
	}
}

void udb_set_action_state(boolean newValue)
{
	// not simulated
	(void)newValue;   // unused parameter
}

void udb_a2d_record_offsets(void)
{
	UDB_XACCEL.offset = UDB_XACCEL.value;
	udb_xrate.offset  = udb_xrate.value;
	UDB_YACCEL.offset = UDB_YACCEL.value - (Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // opposite direction
	udb_yrate.offset  = udb_yrate.value;
	UDB_ZACCEL.offset = UDB_ZACCEL.value;
	udb_zrate.offset  = udb_zrate.value;
	udb_vref.offset   = udb_vref.value;
}

uint16_t get_reset_flags(void)
{
	return mp_rcon;
}

#if (MAG_YAW_DRIFT == 1)

static magnetometer_callback_funcptr magnetometer_callback = NULL;

void rxMagnetometer(magnetometer_callback_funcptr callback)
{
	magnetometer_callback = callback;
}

void I2C_doneReadMagData(void)
{
	magFieldRaw[0] = (magreg[0]<<8) + magreg[1];
	magFieldRaw[1] = (magreg[2]<<8) + magreg[3];
	magFieldRaw[2] = (magreg[4]<<8) + magreg[5];

	if (magMessage == 7)
	{
		udb_magFieldBody[0] = MAG_X_SIGN((__builtin_mulsu((magFieldRaw[MAG_X_AXIS]), magGain[MAG_X_AXIS]))>>14)-(udb_magOffset[0]>>1);
		udb_magFieldBody[1] = MAG_Y_SIGN((__builtin_mulsu((magFieldRaw[MAG_Y_AXIS]), magGain[MAG_Y_AXIS]))>>14)-(udb_magOffset[1]>>1);
		udb_magFieldBody[2] = MAG_Z_SIGN((__builtin_mulsu((magFieldRaw[MAG_Z_AXIS]), magGain[MAG_Z_AXIS]))>>14)-(udb_magOffset[2]>>1);

		if ((abs(udb_magFieldBody[0]) < MAGNETICMAXIMUM) &&
			(abs(udb_magFieldBody[1]) < MAGNETICMAXIMUM) &&
			(abs(udb_magFieldBody[2]) < MAGNETICMAXIMUM))
		{
//			udb_magnetometer_callback();
			if (magnetometer_callback != NULL)
			{
				magnetometer_callback();
			}
			else
			{
				printf("ERROR: magnetometer_callback function pointer not set\r\n");
			}
		}
		else
		{
			magMessage = 0;         // invalid reading, reset the magnetometer
		}
	}
}

void HILSIM_MagData(magnetometer_callback_funcptr callback)
{
	(void)callback;
//	magnetometer_callback = callback;
	magMessage = 7;                 // indicate valid magnetometer data
	I2C_doneReadMagData();          // run the magnetometer computations
}

#endif // MAG_YAW_DRIFT

int setjmp(void)
{
	return 0;
}

int16_t FindFirstBitFromLeft(int16_t val)
{
	int16_t i = 0;

	if (val != 0) 
	{
		for (i = 1; i <= 16; i++)
		{
			if (val & 0x8000) break;
			val <<= 1;
		}
	}
	return i;
}


int led_dummy;

void udb_led_toggle(uint8_t x) { led_dummy = x; }
void led_on(uint8_t x) { led_dummy = x; }
void led_off(uint8_t x) { led_dummy = x; }

void udb_gps_start_sending_data(void) {}
void udb_gps_set_rate(int32_t rate) { led_dummy = rate; }

void udb_serial_start_sending_data(void) {}
void udb_serial_set_rate(int32_t rate) { led_dummy = rate; }

char IsPressed(void) {}
char GetChar(void) { return 0; }

//void console(void) {}
void mcu_init(void) {}
void gentrap(void) {}

//void telemetry_restart(void) {}
void FSInit(void) {}
void filesys_init(void) {}
boolean udb_gps_check_rate(int32_t rate) { return 0; }
uint16_t register_event_p(void (*event_callback)(void), eventPriority priority) { return 0; }
void trigger_event(uint16_t hEvent) {}

void Reset_Handler(void) {}

//int32_t  __builtin_mulss(int16_t p0,   int16_t p1)   { return (p0 * p1); }
//int32_t  __builtin_mulus(uint16_t p0,  int16_t p1)   { return (p0 * p1); }
//int32_t  __builtin_mulsu(int16_t p0,   uint16_t p1)  { return (p0 * p1); }
//uint32_t __builtin_muluu(uint16_t p0,  uint16_t p1)  { return (p0 * p1); }
//uint16_t __builtin_divud(uint32_t num, uint16_t den) { return (num / den); }
//int16_t  __builtin_divsd(int32_t num,  int16_t den)  { return (num / den); }

#endif // (BOARD_TYPE == PX4_BOARD)
