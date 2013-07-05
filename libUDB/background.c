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
#include "debug.h"

#if(USE_I2C1_DRIVER == 1)
#include "I2C.h"
#endif
#if(BOARD_TYPE == UDB4_BOARD)
#include "events.h"
#endif

// Include the NV memory services if required
#if(USE_NV_MEMORY == 1)
#include "NV_memory.h"
#include "data_storage.h"
#include "data_services.h"
#endif

// Include flexifunction mixers if required
#if (USE_FLEXIFUNCTION_MIXING == 1)
#include "../libflexifunctions/flexifunctionservices.h"
#endif

//#define CPU_LOAD_PERCENT  839   // = ((65536 * 100) / ((64000000 / 2) / (16 * 256)))
//      65536 to move result into upper 16 bits of 32 bit word
//      100 to make a percentage
//      32000000 frequency of chrystal clock
//      2 is number of chrystal cycles to each cpu cycle
//      (16 * 256) Number of cycles for (see PR5 below) before timer interrupts
#define CPU_LOAD_PERCENT (6553600/((FCY)/4096))

uint16_t cpu_timer = 0;
uint16_t _cpu_timer = 0;

// Local elapsed time from boot (in heartbeats), used for timestamping.
// rolls over at 2^32 counts: interval is 497 days at 100Hz
unsigned long uptime = 0;

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
unsigned int idle_timer = 0;
unsigned int _idle_timer = 0;

extern union longww primary_voltage;
extern unsigned int lowVoltageWarning;

// decoded failsafe mux input: true means UDB outputs routed to motors, false means RX throttle to motors
extern boolean udb_throttle_enable;

// flag to control tail light
extern unsigned int tailFlash;
extern boolean didCalibrate;
#endif // AIRFRAME_TYPE

uint16_t udb_heartbeat_counter = 0;
#define HEARTBEAT_MAX 57600 // Evenly divisible by many common values: 2^8 * 3^2 * 5^2
#define MAX_NOISE_RATE 5    // up to 5 PWM "glitches" per second are allowed

void udb_run_init_step(void);


void udb_init_clock(void)   // initialize timers
{
#if (HEARTBEAT_HZ < 150)
#define TMR1_PRESCALE 64
#else
#define TMR1_PRESCALE 8
#endif

	// Initialize timer1, used as the HEARTBEAT_HZ heartbeat of libUDB.
	TMR1 = 0;
#if (TMR1_PRESCALE == 8)
	T1CONbits.TCKPS = 1;    // prescaler = 8
#elif (TMR1_PRESCALE == 64)
	T1CONbits.TCKPS = 2;    // prescaler = 64
#else
#error Invalid Timer1 configuration
#endif
//	PR1 = 50000;            // 25 millisecond period at 16 Mz clock, tmr prescale = 8
	PR1 = (FREQOSC / (TMR1_PRESCALE * CLK_PHASES)) / HEARTBEAT_HZ; // period 1/HEARTBEAT_HZ
	T1CONbits.TCS = 0;      // use the crystal to drive the clock
	_T1IP = INT_PRI_T1;     // set interrupt priority
	_T1IF = 0;              // clear the interrupt
	_T1IE = 1;              // enable the interrupt
	T1CONbits.TON = 1;      // turn on timer 1

#if ((BOARD_TYPE & AUAV2_BOARD) == 0)
	// AUAV2 uses MPU6000 interrupt for heartbeat instead of Timer1
	_T1IE = 1;              // enable the interrupt
#endif

	T1CONbits.TON = 1;      // turn on timer 1

	// Timer 5 is used to measure time spent per second in interrupt routines
	// which enables the calculation of the CPU loading.
	// Timer 5 will be turned on in interrupt routines and turned off in main()
	TMR5 = 0;               // initialize timer
	PR5 = 16*256;           // measure instructions in groups of 16*256 
	_cpu_timer = 0;         // initialize the load counter
	T5CONbits.TCKPS = 0;    // no prescaler
	T5CONbits.TCS = 0;      // use the crystal to drive the clock
	_T5IP = INT_PRI_T5;     // set interrupt priority
	_T5IF = 0;              // clear the interrupt
	_T5IE = 1;              // enable the interrupt
#if (USE_MCU_IDLE == 1)
	T5CONbits.TSIDL = 1;    // stop the timer during CPU IDLE
	T5CONbits.TON = 1;      // turn the timer 5 on until we idle
#else
	T5CONbits.TON = 0;      // turn off timer 5 until we enter an interrupt
#endif // USE_MCU_IDLE

	// The Timer7 interrupt is used to trigger background tasks such as 
	// navigation processing after binary data is received from the GPS.
	_T7IP = INT_PRI_T7;     // set interrupt priority
	_T7IF = 0;              // clear the interrupt
	_T7IE = 1;              // enable the interrupt

	// Enable the interrupt, but not the timer. This is used as a trigger from 
	// the high priority heartbeat ISR to start all the HEARTBEAT_HZ processing 
	// at a lower priority.
	_T6IP = INT_PRI_T6;     // set interrupt priority
	_T6IF = 0;              // clear the PWM interrupt
	_T6IE = 1;              // enable the PWM interrupt
}

#if (BOARD_TYPE & AUAV2_BOARD) || (DUAL_IMU == 1)
// The Heartbeat of libUDB is the MPU6000 interrupt

void doT1Interrupt(void)
{
	indicate_loading_inter;             // for cpu loading measurement

	static boolean secToggle = true;
	static int twoHzCounter = 0;

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
	udb_set_dc();
	// set the motor PWM values; these are sent to all ESCs continuously at ESC_HZ
	udb_set_dc();
#else
	// Start the sequential servo pulses
//	start_pwm_outputs() ;               // TODO: check if this should be called
#endif // AIRFRAME_TYPE

	// Call the periodic callback at 2Hz
	if (++twoHzCounter >= (HEARTBEAT_HZ / 2))
	{
		twoHzCounter = 0;

		two_hertz = 1;
		two_hertz_2 = 1;

		udb_background_callback_periodic();

		// Capture cpu_timer once per second.
		if ((secToggle = !secToggle))   // the assignment is intentional
		{
			T5CONbits.TON = 0;          // turn off timer 5
			cpu_timer = _cpu_timer;     // snapshot the load counter
			_cpu_timer = 0;             // reset the load counter
			T5CONbits.TON = 1;          // turn on timer 5

			one_hertz = 1;
			one_hertz_2 = 1;

//			printf("count %u freq %u mc %u adc %u rmat %u 2hz %u 8hz %u task %u\r", count++, freq, freq_mc, freq_adc, freq_rmat, freq_2hz, freq_8hz, freq_task);
//			freq = freq_mc = freq_adc = freq_rmat = freq_2hz = freq_8hz = freq_task = 0;
		}
	}

	// Trigger the HEARTBEAT_HZ calculations, but at a lower priority
	_T6IF = 1;

	uptime++;
	udb_heartbeat_counter++;

	if (udb_heartbeat_counter % (HEARTBEAT_HZ / 40))
	{
		forty_hertz = 1;
	}
}

#else

// This interrupt is the Heartbeat of libUDB.
void __attribute__((__interrupt__,__no_auto_psv__)) _T1Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;

	_T1IF = 0;              // clear the interrupt

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
	// set the motor PWM values; these are sent to all ESCs continuously at ESC_HZ
	udb_set_dc();
#else
	// Start the sequential servo pulses
	start_pwm_outputs();
#endif // AIRFRAME_TYPE

	// Capture cpu_timer once per second.
	if (udb_heartbeat_counter % HEARTBEAT_HZ == 0)
	{
//			printf("count %u freq %u mc %u adc %u rmat %u 2hz %u task %u\r", count++, freq, freq_mc, freq_adc, freq_rmat, freq_2hz, freq_task);
//			printf("count %u freq %u mc %u adc %u rmat %u 2hz %u 8hz %u task %u\r", count++, freq, freq_mc, freq_adc, freq_rmat, freq_2hz, freq_8hz, freq_task);
//			freq = freq_mc = freq_adc = freq_rmat = freq_2hz = freq_8hz = freq_task = 0;
//
// UDB5 build:   count 67 freq 0 mc 400 adc 22213 rmat 400 2hz 2 task 30970
//
// MW build:     count 14 freq 0 mc 400 adc 13884 rmat 400 2hz 2 task 13599
//
// UDB4+quadadc: count 14 freq 0 mc 400 adc 13883 rmat 400 2hz 2 task 22748
//
			one_hertz = 1;
			one_hertz_2 = 1;
		T5CONbits.TON = 0;      // turn off timer 5
		cpu_timer = _cpu_timer; // snapshot the load counter
		_cpu_timer = 0;         // reset the load counter
		T5CONbits.TON = 1;      // turn on timer 5
	}

	// Call the periodic callback at 40Hz
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
	udb_background_callback_periodic_quad();
#else
	udb_background_callback_periodic();
#endif

	// Trigger the HEARTBEAT_HZ calculations, but at a lower priority
	_T6IF = 1;

	udb_heartbeat_counter = (udb_heartbeat_counter+1) % HEARTBEAT_MAX;

	interrupt_restore_corcon;
}
#endif // BOARD_TYPE


// Trigger the low priority background processing interrupt.
void udb_background_trigger(void)
{
	_T7IF = 1;              // trigger the interrupt
}

// Process the TRIGGER interrupt.
// This is used by libDCM to kick off gps-based calculations at a lower
// priority after receiving each new set of GPS data.
void __attribute__((__interrupt__,__no_auto_psv__)) _T7Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;

	_T7IF = 0;              // clear the interrupt

	if ((BOARD_TYPE & AUAV2_BOARD) == 0)
		udb_background_callback_triggered();

	interrupt_restore_corcon;
}

uint8_t udb_cpu_load(void)
{
	// scale cpu_timer to seconds*100 for percent loading
	return (uint8_t)(__builtin_muluu(cpu_timer, CPU_LOAD_PERCENT) >> 16);
}

void __attribute__((__interrupt__,__no_auto_psv__)) _T5Interrupt(void)
{
	interrupt_save_set_corcon;

	TMR5 = 0;               // reset the timer
	_cpu_timer ++;          // increment the load counter
	_T5IF = 0;              // clear the interrupt

	interrupt_restore_corcon;
}

//	Executes whatever lower priority calculation needs to be done every heartbeat (default: 25 milliseconds)
//	This is a good place to eventually compute pulse widths for servos.
void __attribute__((__interrupt__,__no_auto_psv__)) _T6Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;

	_T6IF = 0; // clear the interrupt

#if (NORADIO != 1)
	// 20Hz testing of radio link
	if ((udb_heartbeat_counter % (HEARTBEAT_HZ/20)) == 1)
	{
		// check to see if at least one valid pulse has been received,
		// and also that the noise rate has not been exceeded
		if ((failSafePulses == 0) || (noisePulses > MAX_NOISE_RATE))
		{
			if (udb_flags._.radio_on == 1) {
				udb_flags._.radio_on = 0;
				udb_callback_radio_did_turn_off();
				printf("Radio OFF\r\n");
			}
			LED_GREEN = LED_OFF;
			noisePulses = 0; // reset count of noise pulses
		}
		else
		{
			if (udb_flags._.radio_on == 0) {
				printf("Radio ON\r\n");
			}
			udb_flags._.radio_on = 1;
			LED_GREEN = LED_ON;
		}
		failSafePulses = 0;
	}
	// Computation of noise rate
	// Noise pulses are counted when they are detected, and reset once a second
	if ((udb_heartbeat_counter % HEARTBEAT_HZ) == 1)
	{
		noisePulses = 0;
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

	udb_servo_callback_prepare_outputs();

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

	interrupt_restore_corcon;
}
