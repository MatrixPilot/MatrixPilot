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
#include "sysTick.h"
#include "timer.h"

#define TMR3_PRESCALE   8
#define SYS_TICK_HZ     1000

uint16_t ticktime = 0;

void sys_tick_init(void)
{
        // Initialize timer3, used as the system tick timer
	TMR3 = 0;
	T3CONbits.TCKPS = 1;    // prescaler = 8

        //	PR1 = 50000;            // 25 millisecond period at 16 Mz clock, tmr prescale = 8
	PR3 = (FREQOSC / (TMR3_PRESCALE * CLK_PHASES)) / SYS_TICK_HZ; // period 1/HEARTBEAT_HZ
	T3CONbits.TCS = 0;      // use the crystal to drive the clock
	_T3IP = INT_PRI_T3;     // set interrupt priority
	_T3IF = 0;              // clear the interrupt
	_T3IE = 1;              // enable the interrupt
	T3CONbits.TON = 1;      // turn on timer 1

}


// This interrupt is the system clock tick
void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;

	_T3IF = 0;              // clear the interrupt

        ticktime++;

        // Poll software timers
        timer_tick();

	interrupt_restore_corcon;
}