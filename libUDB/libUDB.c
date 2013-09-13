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
#include "analogs.h"
#include "events.h"

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


//#if(USE_NV_MEMORY == 1)
//if(udb_skip_flags.skip_radio_trim == 1)
//if(udb_skip_flags.skip_imu_cal == 1)
//#endif
//

void udb_init(void)
{
	udb_flags.B = 0;

	init_analogs();
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
	udb_init_osd();

//FIXME: add AUAV3 support
#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
	udb_eeprom_init();
#endif

#if (BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD)
	MPU6000_init16();
#endif

#ifdef __PIC32MX__
#else
	SRbits.IPL = 0; // turn on all interrupt priorities
#endif
}

int show_cpu_load = 0;
extern int one_hertz_flag;

void udb_run(void)
{
	if (one_hertz_flag)
	{
		one_hertz_flag = 0;
		if (show_cpu_load)
		{
			printf("cpu_load: %u%%\r\n", udb_cpu_load());
		}
	}
#if (USE_MCU_IDLE == 1)
//	DIG2 = 0;
	Idle();
//	DIG2 = 1;
#else
	// pause cpu counting timer while not in an ISR
	indicate_loading_main;
#endif
}

// TODO: move the below to the UDB4 analog module
void udb_a2d_record_offsets(void)
{
#if (USE_NV_MEMORY == 1)
	if (udb_skip_flags.skip_imu_cal == 1)
		return;
#endif

	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value;
#ifdef INITIALIZE_VERTICAL // for VTOL, vertical initialization
	UDB_YACCEL.offset = UDB_YACCEL.value - (Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // opposite direction
	UDB_ZACCEL.offset = UDB_ZACCEL.value;
#else  // horizontal initialization
	UDB_YACCEL.offset = UDB_YACCEL.value;
	UDB_ZACCEL.offset = UDB_ZACCEL.value + (Z_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // same direction
#endif // INITIALIZE_VERTICAL

	udb_xrate.offset = udb_xrate.value;
	udb_yrate.offset = udb_yrate.value;
	udb_zrate.offset = udb_zrate.value;
#ifdef VREF
	udb_vref.offset = udb_vref.value;
#endif
}

// saturation logic to maintain pulse width within bounds
int16_t udb_servo_pulsesat(int32_t pw)
{
	if (pw > SERVOMAX) pw = SERVOMAX;
	if (pw < SERVOMIN) pw = SERVOMIN;
	return (int16_t)pw;
}
