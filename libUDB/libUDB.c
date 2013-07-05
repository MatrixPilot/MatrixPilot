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

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#include "MyIpNetwork.h"
#endif

#include "mpu6000.h"
//#include "defines.h"
#include "options.h"
#include "debug.h"


union udb_fbts_byte udb_flags;

// Functions only included with nv memory.
#if (USE_NV_MEMORY == 1)
UDB_SKIP_FLAGS udb_skip_flags = {0, 0, 0};

void udb_skip_radio_trim()
{
	udb_skip_flags.skip_radio_trim = 1;
}

void udb_skip_imu_calibration()
{
	udb_skip_flags.skip_imu_cal = 1;
}

#endif


//#if(USE_NV_MEMORY == 1)
//if(udb_skip_flags.skip_radio_trim == 1)
//if(udb_skip_flags.skip_imu_cal == 1)
//#endif
//

void init_gps(void);
void init_analogs(void);

void udb_init(void)
{
	udb_flags.B = 0;

	init_gps(); // this sets function pointers so i'm calling it early for now
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
	udb_init_I2C();
#endif
#if (CONSOLE_UART != 1)
	udb_init_GPS();
#endif
#if (CONSOLE_UART != 2)
	udb_init_USART();
#endif
	udb_init_pwm();
	udb_init_osd();


#if (BOARD_TYPE != UDB4_BOARD)
	MPU6000_init16();
#endif

#ifdef __PIC32MX__
#else
	SRbits.IPL = 0; // turn on all interrupt priorities
#endif
}


void udb_run(void)
{
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
	//  nothing else to do... entirely interrupt driven
//	while (1)
	{
		// ISRs now start and stop the cpu timer
		//        // pause cpu counting timer while not in an ISR
		//        indicate_loading_main;

		// background task performs low priority tasks and idles when done
		run_background_task();
	}
	// Never returns
#else // AIRFRAME_TYPE
//	//  nothing else to do... entirely interrupt driven
//	while (1)
//	{
		// pause cpu counting timer while not in an ISR
		indicate_loading_main;
//	}
//	// Never returns
#endif // AIRFRAME_TYPE
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
	if (udb_skip_flags.skip_imu_cal == 1)
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
