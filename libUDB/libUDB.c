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


#include "libUDB_internal.h"
#include "oscillator.h"
#include "interrupt.h"
#include "events.h"

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (CONSOLE_UART != 0)
#include "console.h"
#endif

#if (USE_I2C1_DRIVER == 1)
#include "I2C.h"
#endif

// Include the NV memory services if required
#if (USE_NV_MEMORY == 1)
#include "NV_memory.h"
#include "data_storage.h"
#include "data_services.h"
#endif

// Include flexifunction mixers if required
#if (USE_FLEXIFUNCTION_MIXING == 1)
#include "../libflexifunctions/flexifunctionservices.h"
#endif


union udb_fbts_byte udb_flags;

#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_current;
union longww battery_mAh_used;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_voltage;	// battery_voltage._.W1 is in tenths of Volts
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
uint8_t rc_signal_strength;
#define MIN_RSSI	((int32_t)((RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 65536))
#define RSSI_RANGE	((int32_t)((RSSI_MAX_SIGNAL_VOLTAGE-RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 100))
#endif


// Functions only included with nv memory.
#if (USE_NV_MEMORY == 1)
UDB_SKIP_FLAGS udb_skip_flags = {0,0,0};

void udb_skip_radio_trim(boolean b)
{
	udb_skip_flags.skip_radio_trim = 1;
}

void udb_skip_imu_calibration(boolean b)
{
	udb_skip_flags.skip_imu_cal = 1;
}

#endif


//#if(USE_NV_MEMORY == 1)
//if(udb_skip_flags.skip_radio_trim == 1)
//if(udb_skip_flags.skip_imu_cal == 1)
//#endif
//

void udb_init(void)
{
	udb_flags.B = 0;
	
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	battery_current.WW = 0;
	battery_mAh_used.WW = 0;
#endif
#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	battery_voltage.WW = 0;
#endif
#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	rc_signal_strength = 0;
#endif
	udb_init_ADC();
	init_events();
#if (USE_I2C1_DRIVER == 1)
	I2C1_Init();
#endif
#if (USE_NV_MEMORY == 1)
	nv_memory_init();
	data_storage_init();
	data_services_init();
#endif
#if (USE_FLEXIFUNCTION_MIXING == 1)
	flexiFunctionServiceInit();
#endif
	udb_init_clock();
	udb_init_capture();
#if (MAG_YAW_DRIFT == 1 && HILSIM != 1)
//	udb_init_I2C();
#endif
#if (CONSOLE_UART != 1)
	udb_init_GPS();
#endif
#if (CONSOLE_UART != 2)
	udb_init_USART();
#endif
	udb_init_pwm();
#if (USE_OSD == 1)
	udb_init_osd();
#endif

//FIXME: add AUAV3 support
#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
	udb_eeprom_init();
#endif

#if (BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD)
	MPU6000_init16();
#endif

	SRbits.IPL = 0;	// turn on all interrupt priorities
}

void udb_run(void)
{
	//  nothing else to do... entirely interrupt driven
	while (1)
	{
#if (USE_TELELOG == 1)
		telemetry_log();
#endif

#if (USE_USB == 1)
		USBPollingService();
#endif

#if (CONSOLE_UART != 0)
		console();
#endif

#if (USE_MCU_IDLE == 1)
		Idle();
#else
		// pause cpu counting timer while not in an ISR
		indicate_loading_main;
#endif
		// TODO: is the LPRC disabled?
	}
	// Never returns
}

#ifdef INITIALIZE_VERTICAL // for VTOL, vertical initialization
void udb_a2d_record_offsets(void)
{
#if (USE_NV_MEMORY == 1)
	if (udb_skip_flags.skip_imu_cal == 1)
		return;
#endif

	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value;
	udb_xrate.offset = udb_xrate.value;
	UDB_YACCEL.offset = UDB_YACCEL.value - (Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // opposite direction
	udb_yrate.offset = udb_yrate.value;
	UDB_ZACCEL.offset = UDB_ZACCEL.value;
	udb_zrate.offset = udb_zrate.value;
#ifdef VREF
	udb_vref.offset = udb_vref.value;
#endif
}
#else  // horizontal initialization
void udb_a2d_record_offsets(void)
{
#if (USE_NV_MEMORY == 1)
	if(udb_skip_flags.skip_imu_cal == 1)
		return;
#endif

	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value;
	udb_xrate.offset = udb_xrate.value;
	UDB_YACCEL.offset = UDB_YACCEL.value;
	udb_yrate.offset = udb_yrate.value;
	UDB_ZACCEL.offset = UDB_ZACCEL.value + (Z_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // same direction
	udb_zrate.offset = udb_zrate.value;
#ifdef VREF
	udb_vref.offset = udb_vref.value;
#endif
}
#endif // INITIALIZE_VERTICAL

void udb_servo_record_trims(void)
{
	int16_t i;
	for (i = 0; i <= NUM_INPUTS; i++)
	{
		udb_pwTrim[i] = udb_pwIn[i];
	}
}

// saturation logic to maintain pulse width within bounds
int16_t udb_servo_pulsesat(int32_t pw)
{
	if (pw > SERVOMAX) pw = SERVOMAX;
	if (pw < SERVOMIN) pw = SERVOMIN;
	return (int16_t)pw;
}

void calculate_analog_sensor_values(void)
{
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to current in tenths of Amps
	battery_current.WW = (udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_CURRENT) + (((int32_t)(CURRENT_SENSOR_OFFSET)) << 16);
	
	// mAh = mA / 144000 (increment per 40Hz tick is /40*60*60)
	// 90000/144000 == 900/1440
	battery_mAh_used.WW += (battery_current.WW / 1440);
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to voltage in tenths of Volts
	battery_voltage.WW = (udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_VOLTAGE) + (((int32_t)(VOLTAGE_SENSOR_OFFSET)) << 16);
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	union longww rssi_accum;
	rssi_accum.WW = (((udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value + 32768) - (MIN_RSSI)) * (10000 / (RSSI_RANGE)));
	if (rssi_accum._.W1 < 0)
		rc_signal_strength = 0;
	else if (rssi_accum._.W1 > 100)
		rc_signal_strength = 100;
	else
		rc_signal_strength = (uint8_t)rssi_accum._.W1;
#endif
}
