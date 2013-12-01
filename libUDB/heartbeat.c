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
#include "heartbeat.h"
#if (USE_I2C1_DRIVER == 1)
#include "I2C.h"
#endif
#if (USE_NV_MEMORY == 1)
#include "NV_memory.h"
#endif

int one_hertz_flag = 0;
uint16_t udb_heartbeat_counter = 0;
#define HEARTBEAT_MAX 57600 // Evenly divisible by many common values: 2^8 * 3^2 * 5^2

//#define HEARTBEAT_FREQ(x) (udb_heartbeat_counter % (HEARTBEAT_HZ/x) == 0)

inline void heartbeat(void)
{
	// Start the sequential servo pulses at frequency SERVO_HZ
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/SERVO_HZ) == 0)
	{
		start_pwm_outputs();
	}

	// Capture cpu_timer once per second.
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/1) == 0)
	{
		cpu_load_calc();
		one_hertz_flag = 1;
	}

	// Call the periodic callback at 40 Hz
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
	{
		udb_heartbeat_40hz_callback(); // this was called udb_background_callback_periodic()
	}

	// Trigger the HEARTBEAT_HZ calculations, but at a lower priority
//	_T6IF = 1;
	udb_background_trigger_pulse(&pulse);

	udb_heartbeat_counter = (udb_heartbeat_counter+1) % HEARTBEAT_MAX;
}

//	Executes whatever lower priority calculation needs to be done every heartbeat (default: 25 milliseconds)
//	This is a good place to eventually compute pulse widths for servos.
inline void pulse(void)
{
	LED_BLUE = LED_OFF;     // indicates logfile activity

#if (NORADIO != 1)
	// 20Hz testing of radio link
	if ((udb_heartbeat_counter % (HEARTBEAT_HZ/20)) == 1)
	{
		radioIn_failsafe_check();
	}
	// Computation of noise rate
	// Noise pulses are counted when they are detected, and reset once a second
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/1) == 1)
	{
		radioIn_failsafe_reset();
	}
#endif // NORADIO

#ifdef VREF
	vref_adj = (udb_vref.offset>>1) - (udb_vref.value>>1);
#else
	vref_adj = 0;
#endif // VREF

	calculate_analog_sensor_values();
	udb_callback_read_sensors();
	udb_flags._.a2d_read = 1; // signal the A/D to start the next summation

	udb_heartbeat_callback(); // this was called udb_servo_callback_prepare_outputs()

	if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
	{
#if (USE_I2C1_DRIVER == 1)
		I2C1_trigger_service();
#endif

#if (USE_NV_MEMORY == 1)
		nv_memory_service_trigger();
		storage_service_trigger();
		data_services_trigger();
#endif

#if (USE_FLEXIFUNCTION_MIXING == 1)
		flexiFunctionServiceTrigger();
#endif
	}
}
