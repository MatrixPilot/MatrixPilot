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

#ifndef LIB_UDB_INTERNAL_H
#define LIB_UDB_INTERNAL_H

#include "libUDB.h"

extern int failSafePulses;

void udb_init_leds(void);
void udb_init_ADC(void);
void udb_init_clock(void);
void udb_init_capture(void);
void udb_init_I2C(void);
void udb_init_GPS(void);
void udb_init_USART(void);
void udb_init_pwm(void);
void udb_init_osd(void);
void udb_eeprom_init(void);

//void start_pwm_outputs( void ) ;
void udb_set_dc(void);

void calculate_analog_sensor_values(void);

extern int defaultCorcon;
extern unsigned int cpu_timer;
extern unsigned int _cpu_timer;

//#define indicate_loading_main		//LATEbits.LATE4 = 0
//#define indicate_loading_inter	//LATEbits.LATE4 = 1

// digital outputs for monitoring cpu load; SCL2 is sync for heartbeat interrupt
//// SDA2 is high when in an ISR
//#define SCL2 _LATA2
//#define SDA2 _LATA3

#define indicate_loading_inter	{   \
        T5CONbits.TON = 1 ;         \
        T4CONbits.TON = 0 ;         \
        LED_YELLOW = 0;   \
                                }

// turn off the cpu timer as we drop back to IPL 0
// note that this shouldn't be done at IPL>0, i.e. not in an ISR
// since it may have interrupted another ISR
#define indicate_loading_main	{   \
                                }

// The dsp math calls change and restore CORCON internally, so if we fire an
// ISR in the middle of a dsp math call, CORCON can be set to an unexpected value.
// To prevent this, we restore CORCON to the application default when entering
// each ISR, which we saved in main().  Then, when exiting each ISR, we restore
// CORCON to the value it had when we entered.
#define interrupt_save_set_corcon		\
	{					\
		__asm__("push CORCON");		\
		CORCON = defaultCorcon;		\
	}

// check new IPL and stop cpu timer if it is zero
#define interrupt_restore_corcon                \
	{					\
		__asm__("pop CORCON");		\
        checkNewIPL();              \
	}
#endif
