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


#include "libUDB_internal.h"

#if (BOARD_IS_CLASSIC_UDB == 1)

//	Measure the pulse widths of the servo channel inputs from the radio.
//	The dsPIC makes this rather easy to do using its capture feature.

//	One of the channels is also used to validate pulse widths to detect loss of radio.

//	The pulse width inputs can be directly converted to units of pulse width outputs to control
//	the servos by simply dividing by 2.

int udb_pwIn[MAX_INPUTS+1] ;	// pulse widths of radio inputs
int udb_pwTrim[MAX_INPUTS+1] ;	// initial pulse widths for trimming

int failSafePulses = 0 ;

unsigned int rise[MAX_INPUTS+1] ;	// rising edge clock capture for radio inputs


void udb_init_capture(void)
{
	T2CON = 0b1000000000000000  ;	// turn on timer 2 with no prescaler
	TRISD = 0b1111111111111111 ;	// make the d port input, to enable IC1 and IC2
	_TRISF6 = 1 ;			// make F6 an input to enable the 3rd switch
	IC1CON = IC2CON = IC7CON = IC8CON = 0b0010000010000001 ;
	
	int i;
	for (i=0; i <= NUM_INPUTS; i++)
		udb_pwIn[i] = udb_pwTrim[i] = 0 ;
	
	_IC1IP = _IC2IP = _IC7IP = _IC8IP = 6 ; // priority 6
	_IC1IF = _IC2IF = _IC7IF = _IC8IF = 0 ; // clear the interrupt
	
	if (NUM_INPUTS > 0) _IC7IE = 1 ; // turn on interrupt for input 1
	if (NUM_INPUTS > 1) _IC8IE = 1 ; // turn on interrupt for input 2
	if (NUM_INPUTS > 2) _IC2IE = 1 ; // turn on interrupt for input 3
	if (NUM_INPUTS > 3) _IC1IE = 1 ; // turn on interrupt for input 4
	
	if (NUM_INPUTS > 4)
	{
		_TRISE8 = 1 ;	 // set E8 to be an input pin
		_INT0EP = 0;  // Set up the 5th input channel to start out reading low-to-high edges
		_INT0IP = 7 ; // priority 7
		_INT0IF = 0 ; // clear the interrupt
		_INT0IE = 1 ; // turn on the interrupt
	}
	
	return ;
}


// Input Channel 1
void __attribute__((__interrupt__,__no_auto_psv__)) _IC7Interrupt(void)
{
	unsigned int time ;	
	_IC7IF = 0 ; // clear the interrupt
	while ( IC7CONbits.ICBNE )
	{
		time = IC7BUF ;
	}
	
#if ( NORADIO == 0 )
	if (PORTBbits.RB4)
	{
		 rise[1] = time ;
	}
	else
	{
		udb_pwIn[1] = ((time - rise[1]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 1 )
		if ( (udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			failSafePulses++ ;
		}
		else
		{
			failSafePulses = 0 ;
			udb_flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
#endif
	
	}
#endif

	return ;
}


// Input Channel 2
void __attribute__((__interrupt__,__no_auto_psv__)) _IC8Interrupt(void)
{
	unsigned int time ;
	_IC8IF = 0 ; // clear the interrupt
	while ( IC8CONbits.ICBNE )
	{
		time = IC8BUF ;
	}
	
#if ( NORADIO == 0 )
	if (PORTBbits.RB5)
	{
		 rise[2] = time ;
	}
	else
	{
		udb_pwIn[2] = ((time - rise[2]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 2 )
		if ( (udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			failSafePulses++ ;
		}
		else
		{
			failSafePulses = 0 ;
			udb_flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
#endif
	
	}	
#endif
	
	return ;
}


// Input Channel 3
void __attribute__((__interrupt__,__no_auto_psv__)) _IC2Interrupt(void)
{
	unsigned int time ;
	_IC2IF = 0 ; // clear the interrupt
	while ( IC2CONbits.ICBNE )
	{
		time = IC2BUF ;
	}
	
#if ( NORADIO == 0 )
	if (PORTDbits.RD1)
	{
		 rise[3] = time ;
	}
	else
	{
		udb_pwIn[3] = ((time - rise[3]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 3 )
		if ( (udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			failSafePulses++ ;
		}
		else
		{
			failSafePulses = 0 ;
			udb_flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
#endif
	
	}
#endif
	
	return ;
}


// Input Channel 4
void __attribute__((__interrupt__,__no_auto_psv__)) _IC1Interrupt(void)
{
	unsigned int time ;
	_IC1IF =  0 ; // clear the interrupt
	while ( IC1CONbits.ICBNE )
	{
		time = IC1BUF ;
	}
	
#if ( NORADIO == 0 )
	if (PORTDbits.RD0)
	{
		 rise[4] = time ;
	}
	else
	{
		udb_pwIn[4] = ((time - rise[4]) >> 1 );
		
#if ( FAILSAFE_INPUT_CHANNEL == 4 )
		if ( (udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			failSafePulses++ ;
		}
		else
		{
			failSafePulses = 0 ;
			udb_flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
#endif
	
	}
#endif
	
	return ;
}


// Input Channel 5 (Pin RE8)
void __attribute__((__interrupt__,__no_auto_psv__)) _INT0Interrupt(void)
{
	
#if ( NORADIO == 0 )
	int t = TMR2 ;
	
	if (PORTEbits.RE8)
	{
		rise[5] = t ;
		_INT0EP = 1 ;	// Set up the interrupt to read high-to-low edges
	}
	else
	{
		udb_pwIn[5] = ((t - rise[5]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 5 )
		if ( (udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			failSafePulses++ ;
		}
		else
		{
			failSafePulses = 0 ;
			udb_flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
#endif
		_INT0EP = 0 ;	// Set up the interrupt to read low-to-high edges
	}
#endif
	
	_INT0IF = 0 ; 		// clear the interrupt
	
	return;
}

#endif
