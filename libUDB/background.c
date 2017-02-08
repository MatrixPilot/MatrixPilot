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

//#define CPU_LOAD_PERCENT  1678  // = ((65536 * 100) / ((32000000 / 2) / (16 * 256)))
//#define CPU_LOAD_PERCENT  839   // = ((65536 * 100) / ((64000000 / 2) / (16 * 256)))
//      65536 to move result into upper 16 bits of 32 bit word
//      100 to make a percentage
//      32000000 frequency of chrystal clock
//      2 is number of chrystal cycles to each cpu cycle
//      (16 * 256) Number of cycles for (see PR5 below) before timer interrupts
#define CPU_LOAD_PERCENT (6553600/((FCY)/4096))
static uint16_t cpu_timer = 0;
static uint16_t _cpu_timer = 0;

inline uint8_t udb_cpu_load(void)
{
	// scale cpu_timer to seconds*100 for percent loading
	return (uint8_t)(__builtin_muluu(cpu_timer, CPU_LOAD_PERCENT) >> 16);
}

static inline void init_heartbeat(void)
{
//#ifdef USE_MPU_HEARTBEAT
//#if (HEARTBEAT_HZ != 200)
//#error HEARTBEAT_HZ must be set to 200 when using the MPU6000 as a heartbeat
//#endif
//#endif

#if (BOARD_TYPE != UDB4_BOARD && HEARTBEAT_HZ == 200)

	// MPU6000 interrupt is used as the HEARTBEAT_HZ heartbeat of libUDB.
	// Timer1 is not used for heartbeat, but its interrupt flag is set in the
	// MPU6000 ISR.
	T1CONbits.TON = 0;      // turn off timer 1
	TMR1 = 0;
	_T1IP = INT_PRI_T1;     // set interrupt priority
	_T1IF = 0;              // clear the interrupt
	_T1IE = 1;              // enable the interrupt

	// TODO: can we use timer1 to determine the error between the mcu and the mpu 200Hz?

#else // BOARD_TYPE && HEARTBEAT_HZ

	// use Timer1 as the HEARTBEAT source

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

#endif // (BOARD_TYPE != UDB4_BOARD && HEARTBEAT_HZ == 200)
}

static inline void init_cpu_timer(void)
{
	// Timer 5 is used to measure CPU usage
	// Two techniques are supported, depending on whether USE_MCU_IDLE is selected
	//   Timer 5 free runs until stopped during CPU idle
	// else
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
}

inline void cpu_load_calc(void)
{
	T5CONbits.TON = 0;      // turn off timer 5
	cpu_timer = _cpu_timer; // snapshot the load counter
	_cpu_timer = 0;         // reset the load counter
	T5CONbits.TON = 1;      // turn on timer 5
}

static inline void init_callback_1(void)
{
	// The Timer7 interrupt is used to trigger background tasks such as
	// navigation processing after binary data is received from the GPS.
	_T7IP = INT_PRI_T7;     // set interrupt priority
	_T7IF = 0;              // clear the interrupt
	_T7IE = 1;              // enable the interrupt
}

static inline void init_callback_2(void)
{
	// Enable the interrupt, but not the timer. This is used as a trigger from
	// the high priority heartbeat ISR to start all the HEARTBEAT_HZ processing
	// at a lower priority.
	_T6IP = INT_PRI_T6;     // set interrupt priority
	_T6IF = 0;              // clear the PWM interrupt
	_T6IE = 1;              // enable the PWM interrupt
}

void udb_init_clock(void)   // initialize timers
{
	init_heartbeat();
	init_cpu_timer();
	init_callback_1();
	init_callback_2();
}

void udb_init_irq(void)
{
	SRbits.IPL = 0; // turn on all interrupt priorities
}

// This interrupt is the Heartbeat of libUDB.
void __attribute__((__interrupt__,__no_auto_psv__)) _T1Interrupt(void)
{
	indicate_loading_inter;
	set_ipl_on_output_pin;
	interrupt_save_set_corcon;
	_T1IF = 0;              // clear the interrupt
	heartbeat();
	interrupt_restore_corcon;
	unset_ipl_on_output_pin;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _T5Interrupt(void)
{
	interrupt_save_set_corcon;
	set_ipl_on_output_pin;
	TMR5 = 0;               // reset the timer
	_cpu_timer++;           // increment the load counter
	_T5IF = 0;              // clear the interrupt
	interrupt_restore_corcon;
	unset_ipl_on_output_pin;
}

static background_callback callback_fptr_1 = NULL;

// Executes whatever lower priority calculation needs to be done every heartbeat (default: 25 milliseconds)
// This is a good place to eventually compute pulse widths for servos.
void __attribute__((__interrupt__,__no_auto_psv__)) _T6Interrupt(void)
{
	indicate_loading_inter;
	set_ipl_on_output_pin;
	interrupt_save_set_corcon;
	_T6IF = 0;              // clear the interrupt
	if (callback_fptr_1) callback_fptr_1(); // was called pulse() or heartbeat_pulse()
	interrupt_restore_corcon;
	unset_ipl_on_output_pin;
}

// Trigger the low priority background processing interrupt.
void udb_background_trigger_pulse(background_callback callback)
{
	// Trigger the HEARTBEAT_HZ calculations, but at a lower priority
	callback_fptr_1 = callback;
	_T6IF = 1;
}

static background_callback callback_fptr_2 = NULL;

// Process the TRIGGER interrupt.
// This is used by libDCM to kick off gps-based calculations at a lower
// priority after receiving each new set of GPS data.
void __attribute__((__interrupt__,__no_auto_psv__)) _T7Interrupt(void)
{
	indicate_loading_inter;
	set_ipl_on_output_pin;
	interrupt_save_set_corcon;
	_T7IF = 0;              // clear the interrupt
	//udb_background_callback_triggered(); // replaced by function pointer callback below
	if (callback_fptr_2) callback_fptr_2();
	interrupt_restore_corcon;
	unset_ipl_on_output_pin;
}

// Trigger the low priority background processing interrupt.
void udb_background_trigger(background_callback callback)
{
	callback_fptr_2 = callback;
	_T7IF = 1;              // trigger the interrupt
}
