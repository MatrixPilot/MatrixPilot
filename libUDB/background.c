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

#if (BOARD_IS_CLASSIC_UDB == 1)
#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#define CPU_LOAD_PERCENT	16*400   // = (100 / (8192 * 2)) * (256**2)
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#define CPU_LOAD_PERCENT	16*109   // = ((100 / (8192 * 2)) * (256**2))/3.6864
#endif

#elif (BOARD_TYPE == UDB4_BOARD)
#define CPU_LOAD_PERCENT	16*100
#endif


unsigned int cpu_timer = 0 ;
unsigned int _cpu_timer = 0 ;

unsigned int udb_heartbeat_counter = 0 ;
#define HEARTBEAT_MAX	57600		// Evenly divisible by many common values: 2^8 * 3^2 * 5^2

void udb_run_init_step( void ) ;


#if ( BOARD_TYPE == UDB4_BOARD )
#define _TTRIGGERIP _T7IP
#define _TTRIGGERIF _T7IF
#define _TTRIGGERIE _T7IE
#else
#define _TTRIGGERIP _T3IP
#define _TTRIGGERIF _T3IF
#define _TTRIGGERIE _T3IE
#endif

#if ( BOARD_TYPE == UDB4_BOARD )
#define _THEARTBEATIP _T6IP
#define _THEARTBEATIF _T6IF
#define _THEARTBEATIE _T6IE
#else
#define _THEARTBEATIP _PWMIP
#define _THEARTBEATIF _PWMIF
#define _THEARTBEATIE _PWMIE
#endif


void udb_init_clock(void)	/* initialize timers */
{
	TRISF = 0b1111111111101100 ;
	
	
	// Initialize timer1, used as the 40Hz heartbeat of libUDB.
	TMR1 = 0 ;
#if (BOARD_TYPE == UDB4_BOARD)
	PR1 = 50000 ;			// 25 millisecond period at 16 Mz clock, tmr prescale = 8
	T1CONbits.TCKPS = 1;	// prescaler = 8
#elif ( CLOCK_CONFIG == CRYSTAL_CLOCK )
	PR1 = 12500 ;			// 25 millisecond period at 16 Mz clock, inst. prescale 4, tmr prescale 8	
	T1CONbits.TCKPS = 1;	// prescaler = 8
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
	PR1 = 46080 ;			// 25 millisecond period at 58.982 Mz clock,inst. prescale 4, tmr prescale 8	
	T1CONbits.TCKPS = 1;	// prescaler = 8
#endif
	T1CONbits.TCS = 0 ;		// use the crystal to drive the clock
	_T1IP = 6 ;				// High priority
	_T1IF = 0 ;				// clear the interrupt
	_T1IE = 1 ;				// enable the interrupt
	T1CONbits.TON = 1 ;		// turn on timer 1
	
	
	// Timer 5 is used to measure time spent per second in interrupt routines
	// which enables the calculation of the CPU loading.
	// Timer 5 will be turned on in interrupt routines and turned off in main()
	TMR5 = 0 ; 				// initialize timer
	PR5 = 16*256 ;			// measure instructions in groups of 16*256 
	_cpu_timer = 0 ;		// initialize the load counter
	T5CONbits.TCKPS = 0 ;	// no prescaler
	T5CONbits.TCS = 0 ;	    // use the crystal to drive the clock
	_T5IP = 6 ;				// high priority, but ISR is very short
	_T5IF = 0 ;				// clear the interrupt
	_T5IE = 1 ;				// enable the interrupt
	T5CONbits.TON = 0 ;		// turn off timer 5 until we enter an interrupt
	
	
	// The TTRIGGER interrupt (T3 or T7 depending on the board) is used to
	// trigger background tasks such as navigation processing after binary data
	// is received from the GPS.
	_TTRIGGERIP = 2 ;		// priority 2
	_TTRIGGERIF = 0 ;		// clear the interrupt
	_TTRIGGERIE = 1 ;		// enable the interrupt
	
	
	// Start the PWM Interrupt, but not the PWM timer.
	// This is used as a trigger from the high priority heartbeat ISR to
	// start all the 40Hz processing at a lower priority.
	_THEARTBEATIF = 0 ;					// clear the PWM interrupt
	_THEARTBEATIP = 3 ;					// priority 3
#if (BOARD_TYPE != UDB4_BOARD)
	_PEN1L = _PEN2L = _PEN3L = 0 ;		// low pins used as digital I/O
	_PEN1H = _PEN2H = _PEN3H = 0 ;		// high pins used as digital I/O
#endif
	_THEARTBEATIE = 1 ;					// enable the PWM interrupt
	
	return ;
}


// This high priority interrupt is the Heartbeat of libUDB.
void __attribute__((__interrupt__,__no_auto_psv__)) _T1Interrupt(void) 
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_T1IF = 0 ;			// clear the interrupt
	
	// Start the sequential servo pulses
	start_pwm_outputs() ;
	
	// Capture cpu_timer once per second.
	if (udb_heartbeat_counter % 40 == 0)
	{
		T5CONbits.TON = 0 ;		// turn off timer 5
		cpu_timer = _cpu_timer ;// snapshot the load counter
		_cpu_timer = 0 ; 		// reset the load counter
		T5CONbits.TON = 1 ;		// turn on timer 5
	}
	
	// Call the periodic callback at 2Hz
	if (udb_heartbeat_counter % 20 == 0)
	{
		udb_background_callback_periodic() ;
	}
	
	
	// Trigger the 40Hz calculations, but at a lower priority
	_THEARTBEATIF = 1 ;
	
	
	udb_heartbeat_counter = (udb_heartbeat_counter+1) % HEARTBEAT_MAX;
	
	interrupt_restore_corcon ;
	return ;
}


// Trigger the TRIGGER interrupt.
void udb_background_trigger(void)
{
	_TTRIGGERIF = 1 ;  // trigger the interrupt
	return ;
}


// Process the TRIGGER interrupt.
// This is used by libDCM to kick off gps-based calculations at a lower
// priority after receiving each new set of GPS data.
#if ( BOARD_TYPE == UDB4_BOARD )
void __attribute__((__interrupt__,__no_auto_psv__)) _T7Interrupt(void) 
#else
void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt(void) 
#endif
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_TTRIGGERIF = 0 ;			// clear the interrupt
	
	udb_background_callback_triggered() ;
	
	interrupt_restore_corcon ;
	return ;
}


unsigned char udb_cpu_load(void)
{
	return (unsigned char)(__builtin_muluu(cpu_timer, CPU_LOAD_PERCENT) >> 16) ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _T5Interrupt(void) 
{
	interrupt_save_set_corcon ;
	
	TMR5 = 0 ;		// reset the timer
	_cpu_timer ++ ;	// increment the load counter
	_T5IF = 0 ;		// clear the interrupt
	
	interrupt_restore_corcon ;
	return ;
}


//	Executes whatever lower priority calculation needs to be done every 25 milliseconds.
//	This is a good place to eventually compute pulse widths for servos.
#if ( BOARD_TYPE == UDB4_BOARD )
void __attribute__((__interrupt__,__no_auto_psv__)) _T6Interrupt(void)
#else
void __attribute__((__interrupt__,__no_auto_psv__)) _PWMInterrupt(void)
#endif
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_THEARTBEATIF = 0 ; /* clear the interrupt */
	
#if ( NORADIO != 1 )
	// 20Hz testing for radio link
	if ( udb_heartbeat_counter % 2 == 1)
	{
		if ( failSafePulses == 0 )
		{
			if (udb_flags._.radio_on == 1) {
				udb_flags._.radio_on = 0 ;
				udb_callback_radio_did_turn_off() ;
			}
			LED_GREEN = LED_OFF ;
		}
		else
		{
			udb_flags._.radio_on = 1 ;
			LED_GREEN = LED_ON ;
		}
		failSafePulses = 0 ;
	}
#endif
	
#ifdef VREF
	vref_adj = (udb_vref.offset>>1) - (udb_vref.value>>1) ;
#else
	vref_adj = 0 ;
#endif
	
	calculate_analog_sensor_values() ;
	udb_callback_read_sensors() ;
	udb_flags._.a2d_read = 1 ; // signal the A/D to start the next summation
	
	udb_servo_callback_prepare_outputs() ;
	
	interrupt_restore_corcon ;
	return ;
}
