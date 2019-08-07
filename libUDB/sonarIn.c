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
#include "interrupt.h"
#include "sonarIn.h"

#if (USE_SONAR_INPUT != 0)

int16_t udb_pwm_sonar;          // pulse width of sonar signal
static int16_t udb_pwm_sonar_rise;
static uint16_t sonar_pwm_count;

int16_t get_sonar_value(void)
{
	return udb_pwm_sonar;
}

uint16_t get_sonar_count(void)
{
	return sonar_pwm_count;
}

#if (BOARD_TYPE == AUAV3_BOARD)
#define REGTOK1 N1
#define REGLBL1 bits.ICTSEL
#else // UDB4 or 5
#define REGTOK1 N
#define REGLBL1 bits.ICTMR
#endif

//	_TRISD15 = 1; \  // TODO: check if we need to be setting the tris bit for our input capture

#define _SONAR_INIT(x, y, z) \
{ \
	IC##x##CO##y##z = 0; \
	IC##x##CO##y##bits.ICM = 1; \
	_IC##x##IP = 6; \
	_IC##x##IF = 0; \
	_IC##x##IE = 1; \
}
#define SONAR_INIT(x, y, z) _SONAR_INIT(x, y, z)

void udb_init_sonar(void)
{
	// Setup Channel 8 for Sonar
	// Sonar PWM Pulses are at 58 micro seconds per cm measured. Maximum for MB1230 is 765 cm. So Max Pulse is 44370 micro seconds.
	// ( MB1260 Maximum is 1053cm. So Max Pulse is 61074)
	// Clock of timer is running at 16,000,000 Hz. So Max Sonar Pulse is 16000000 * 0.044379 clock pulses which is 710064 pulses. 
	// If prescales of the timer is set to 64, then maxumum sonar measurement within matrixPIlot for MB1220 is 710064 / 64 = 11095.
	// ( For MB1260 maxumum will be 14.5 * 1053 which is 15268.5 )
	// If minimum reading is 0.2 meters, then minimum PWM is  (20 * 58) = 1160 micro seconds. So the
	// minimum integer in MatrixPilot should then be (16000000 * 0.001160) / 64 = 290
	// Each unit of UDB PWM sonar pulse is 64 / 16000000 seconds which is 0.000004 seconds in length.
	// Therefore each centimeter of measured distance will show 0.000058 / 0.000004 or 58 or 14.5 UDB PWM sonar units / centimeter.

	TMR3 = 0;               // initialize timer
	T3CONbits.TCKPS = 2;    // prescaler = 64,  see page 175 at http://ww1.microchip.com/downloads/en/DeviceDoc/70593C.pdf
	T3CONbits.TCS = 0;      // use the internal clock
	T3CONbits.TON = 1;      // turn on timer 3

	SONAR_INIT(USE_SONAR_INPUT, REGTOK1, REGLBL1);

}

#define _SONAR_HANDLER(x, y) \
void __attribute__((__interrupt__,__no_auto_psv__)) _IC##x##Interrupt(void) \
{ \
	indicate_loading_inter; \
	set_ipl_on_output_pin; \
	interrupt_save_set_corcon; \
	uint16_t time; \
	_IC##x##IF = 0; \
	while (IC##x##CO##y##bits.ICBNE) \
	{ \
		time = IC##x##BUF; \
	} \
	if (IC_PIN##x) \
	{ \
		udb_pwm_sonar_rise = time; \
		sonar_pwm_count++; \
	} \
	else \
	{ \
		udb_pwm_sonar = time - udb_pwm_sonar_rise; \
		udb_flags._.sonar_updated = 1; \
	} \
	interrupt_restore_corcon; \
	unset_ipl_on_output_pin;  \
}
#define SONAR_HANDLER(x, y) _SONAR_HANDLER(x, y)


SONAR_HANDLER(USE_SONAR_INPUT, REGTOK1);


#endif // USE_SONAR_INPUT
