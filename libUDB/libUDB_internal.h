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

extern int failSafePulses ;

void udb_init_leds(void) ;
void udb_init_ADC(void) ;
void udb_init_clock(void) ;
void udb_init_capture(void) ;
void udb_init_I2C(void) ;
void udb_init_GPS(void) ;
void udb_init_USART(void) ;
void udb_init_pwm(void) ;
void udb_init_osd( void ) ;
void udb_eeprom_init( void ) ;

void start_pwm_outputs( void ) ;

void calculate_analog_sensor_values( void ) ;

extern int defaultCorcon ;
extern unsigned int cpu_timer ;
extern unsigned int _cpu_timer ;

//#define indicate_loading_main		//LATEbits.LATE4 = 0
//#define indicate_loading_inter	//LATEbits.LATE4 = 1

#define indicate_loading_inter	{							\
									T5CONbits.TON = 1 ;		\
								}

#define indicate_loading_main	{							\
									T5CONbits.TON = 0 ;		\
								}


// The dsp math calls change and restore CORCON internally, so if we fire an
// ISR in the middle of a dsp math call, CORCON can be set to an unexpected value.
// To prevent this, we restore CORCON to the application default when entering
// each ISR, which we saved in main().  Then, when exiting each ISR, we restore
// CORCON to the value it had when we entered.
#define interrupt_save_set_corcon		\
	{										\
		__asm__("push CORCON");				\
		CORCON = defaultCorcon;				\
	}

#define interrupt_restore_corcon	\
	{										\
		__asm__("pop CORCON");				\
	}
