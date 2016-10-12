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


#include "libUDB.h"
#include "oscillator.h"
#include "interrupt.h"
#include "heartbeat.h"
#include "servoOut.h"
#include "analogs.h"
#include "radioIn.h"
#include "../libDCM/rmat.h"
#if (BOARD_TYPE == UDB4_BOARD)
#include "../libDCM/libDCM.h"
#endif
#if (USE_I2C1_DRIVER == 1)
#include "I2C.h"
#endif
#if (USE_NV_MEMORY == 1)
#include "NV_memory.h"
#endif

int one_hertz_flag = 0;
uint16_t udb_heartbeat_counter = 0;
uint16_t udb_pulse_counter = 0;
#define HEARTBEAT_MAX 57600 // Evenly divisible by many common values: 2^8 * 3^2 * 5^2

static void heartbeat_pulse(void);    // forward declaration

//#define HEARTBEAT_FREQ(x) (udb_heartbeat_counter % (HEARTBEAT_HZ/x) == 0)
//#define HEARTBEAT_CHK(x) (udb_heartbeat_counter % (HEARTBEAT_HZ/x) == 0)

#if (BOARD_TYPE == UDB4_BOARD)
boolean udb_gyros_autozero_latched_up = false ;
boolean udb_gyros_autozero_latched_down = false ;
void udb_gyros_auto_zero_latch_up(void);
void udb_gyros_auto_zero_latch_down(void);
#endif

inline boolean heartbeat_chk(uint16_t freq)
{
	return (udb_pulse_counter % (HEARTBEAT_HZ/freq) == 0);
//	return HEARTBEAT_CHK(freq);
}

inline uint16_t heartbeat_cnt(void)
{
	return udb_pulse_counter;
}

// NOTE: RobD - udb_heartbeat_counter is not being used at the libUDB layer
//              outside of this module, so it could be moved up.
inline void heartbeat(void) // called from ISR
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

	// TODO: determine why this is called from the high priority interrupt handler? is it req?
	// This calls the state machine implemented in MatrixPilot/states.c
	// it is called at high priority to ensure manual control takeover can
	// occur, even if the lower priority tasks hang
	// Call the periodic callback at 40 Hz
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
	{
		// by default runs the MatrixPilot state machine in states.c
		udb_heartbeat_40hz_callback(); // this was called udb_background_callback_periodic()
	}

	// Trigger the HEARTBEAT_HZ calculations, but at a lower priority
//	_T6IF = 1;
	udb_background_trigger_pulse(&heartbeat_pulse);
	// TODO: RobD - potential inversion issue here with incrementing the counter
	//              before versus after the pulse occurs, depending on the
	//              trigger implementation..
	udb_heartbeat_counter = (udb_heartbeat_counter+1) % HEARTBEAT_MAX;
}

// Executes whatever lower priority calculation needs to be done every heartbeat (default: 25 milliseconds)
// This is a good place to eventually compute pulse widths for servos.
static void heartbeat_pulse(void)
{
	led_off(LED_BLUE);  // indicates logfile activity
#if (BOARD_TYPE == UDB4_BOARD) 
	// IDG500 and ISZ500 Gyros settle at least 200 milliseconds after startup
	// Auto-zero the UDB4 gyros 1 second before calibration of offsets
	if (((udb_pulse_counter / (HEARTBEAT_HZ / 40)) > ( DCM_CALIB_COUNT - 40 )) 
		&& (udb_gyros_autozero_latched_up == false ))
	{
		udb_gyros_auto_zero_latch_up();
		udb_gyros_autozero_latched_up = true;
	}
	if (((udb_pulse_counter / (HEARTBEAT_HZ / 40)) > (DCM_CALIB_COUNT - 39))  
		&& (udb_gyros_autozero_latched_down == false ))
	{
		udb_gyros_auto_zero_latch_down();
		udb_gyros_autozero_latched_down = true;
	}
	// Gyros need 20 milliseconds to settle after auto-zero, before being used by DCM
#endif 
#if (NORADIO != 1)
	// 20Hz testing of radio link
	if ((udb_pulse_counter % (HEARTBEAT_HZ/20)) == 1)
	{
		radioIn_failsafe_check();
	}
	// Computation of noise rate
	// Noise pulses are counted when they are detected, and reset once a second
	if (udb_pulse_counter % (HEARTBEAT_HZ/1) == 1)
	{
		radioIn_failsafe_reset();
	}
#endif // NORADIO

#ifdef VREF
	vref_adj = (udb_vref.offset>>1) - (udb_vref.value>>1);
#else
	vref_adj = 0;
#endif // VREF

	udb_callback_read_sensors();
	if ((udb_pulse_counter % (HEARTBEAT_HZ/40)) == 0)
 	{
 		calculate_analog_sensor_values();
 		udb_flags._.a2d_read = 1; // signal the A/D to start the next summation
 	}

	// process sensor data, run flight controller, generate outputs. implemented in libDCM.c
	udb_heartbeat_callback(); // this was called udb_servo_callback_prepare_outputs()

	if (udb_pulse_counter % (HEARTBEAT_HZ/40) == 0)
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
	udb_pulse_counter = (udb_pulse_counter+1) % HEARTBEAT_MAX;
}
