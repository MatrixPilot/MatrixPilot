// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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

extern int twentyHertzCounter ;
extern int failSafePulses ;

void udb_init_leds(void) ;
void udb_init_ADC(void) ;
void udb_init_clock(void) ;
void udb_init_capture(void) ;
void udb_init_I2C(void) ;
void udb_init_GPS(void) ;
void udb_init_USART(void) ;
void udb_init_pwm(void) ;


extern boolean needSaveExtendedState ;
extern boolean timer_5_on ;
extern int defaultCorcon ;
extern unsigned int cpu_timer ;

//#define indicate_loading_main		//LATEbits.LATE4 = 0
//#define indicate_loading_inter	//LATEbits.LATE4 = 1

// Empirical results show that reading and writing to the
// "Timer On" function loses clock cycles in the timer. 
// So the software makes a test using a parallel variable
// called timer_5_on.
#define indicate_loading_inter		if ( timer_5_on == 0 )	\
									{						\
										T5CONbits.TON = 1 ;	\
										timer_5_on = 1 ;	\
									}

#define indicate_loading_main		if ( timer_5_on == 1 )	\
									{						\
										T5CONbits.TON = 0 ;	\
										timer_5_on = 0 ;	\
									}


// When ISRs fire during dsp math calls, state is not preserved properly, so we
// have to help preserve extra register state on entry and exit from ISRs.
// In addition, the dsp math calls change and restore CORCON internally, so
// if we fire an ISR in the middle of a dsp math call, CORCON can be set to
// an unexpected value, so we also restore CORCON to the application default,
// which we save in main().  We keep track of whether or not we're running dsp
// calls in needSaveExtendedState var, and only perform these actions if so.
#define interrupt_save_extended_state \
	{ \
		if (needSaveExtendedState) { \
			__asm__("push CORCON"); \
			__asm__("push SR"); \
			__asm__("push MODCON"); \
			__asm__("push XBREV"); \
			__asm__("push ACCAL"); \
			__asm__("push ACCAH"); \
			__asm__("push ACCAU"); \
			__asm__("push ACCBL"); \
			__asm__("push ACCBH"); \
			__asm__("push ACCBU"); \
			__asm__("push RCOUNT"); \
			__asm__("push DCOUNT"); \
			__asm__("push DOSTARTL"); \
			__asm__("push DOSTARTH"); \
			__asm__("push DOENDL"); \
			__asm__("push DOENDH"); \
			int asmDoRestoreExtendedState = 1; \
			__asm__("push %0" : "+r"(asmDoRestoreExtendedState)); \
			CORCON = defaultCorcon; \
			needSaveExtendedState = 0; \
		} \
		else \
		{ \
			int asmDoRestoreExtendedState = 0; \
			__asm__("push %0" : "+r"(asmDoRestoreExtendedState)); \
		} \
	}

#define interrupt_restore_extended_state \
	{ \
		int asmDoRestoreExtendedState; \
		__asm__("pop %0" : "+r"(asmDoRestoreExtendedState)); \
		if (asmDoRestoreExtendedState) { \
			__asm__("pop DOENDH"); \
			__asm__("pop DOENDL"); \
			__asm__("pop DOSTARTH"); \
			__asm__("pop DOSTARTL"); \
			__asm__("pop DCOUNT"); \
			__asm__("pop RCOUNT"); \
			__asm__("pop ACCBU"); \
			__asm__("pop ACCBH"); \
			__asm__("pop ACCBL"); \
			__asm__("pop ACCAU"); \
			__asm__("pop ACCAH"); \
			__asm__("pop ACCAL"); \
			__asm__("pop XBREV"); \
			__asm__("pop MODCON"); \
			__asm__("pop SR"); \
			__asm__("pop CORCON"); \
			needSaveExtendedState = 1; \
		} \
	}

