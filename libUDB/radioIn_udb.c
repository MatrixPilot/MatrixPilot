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

int udb_pwIn[NUM_INPUTS+1] ;	// pulse widths of radio inputs
int udb_pwTrim[NUM_INPUTS+1] ;	// initial pulse widths for trimming

int failSafePulses = 0 ;

#if (USE_PPM_INPUT != 1)
unsigned int rise[NUM_INPUTS+1] ;	// rising edge clock capture for radio inputs
#else
unsigned int rise_ppm ;				// rising edge clock capture for PPM radio input
#endif

void udb_init_capture(void)
{
	T2CON = 0b1000000000000000  ;	// turn on timer 2 with no prescaler
	_TRISF6 = 1 ;			// make F6 an input to enable the 3rd switch
	
	TRISD = 0b1111111111111111 ;	// make the d port input, to enable IC1 and IC2
	
#if (USE_PPM_INPUT != 1)
	IC1CON = IC2CON = IC7CON = IC8CON = 0b0010000010000001 ;
	_IC1IP = _IC2IP = _IC7IP = _IC8IP = 6 ; // priority 6
	_IC1IF = _IC2IF = _IC7IF = _IC8IF = 0 ; // clear the interrupt
#else
	IC7CON = 0b0010000010000001 ;
	_IC7IP = 6 ; // priority 6
	_IC7IF = 0 ; // clear the interrupt
#endif
	
	int i;
	for (i=0; i <= NUM_INPUTS; i++)
		udb_pwIn[i] = udb_pwTrim[i] = 0 ;
	
	if (NUM_INPUTS > 0) _IC7IE = 1 ; // turn on interrupt for input 1
	
#if (USE_PPM_INPUT != 1)
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
#endif
	
	return ;
}


#if (USE_PPM_INPUT != 1)

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


#else // #if (USE_PPM_INPUT == 1)

unsigned char ppm_ch = 1 ;
boolean frameOK = true ;

// PPM Input on Channel 1
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
		unsigned int pulse = (time - rise_ppm) >> 1 ;
		rise_ppm = time ;
		
		if (pulse > 10000)			//sync pulse
		{
			ppm_ch = 1 ;
			frameOK = true ;
		}
		else
		{
			if (ppm_ch <= NUM_INPUTS)
			{
				udb_pwIn[ppm_ch] = pulse ;
			}
			
			if (ppm_ch <= PPM_NUMBER_OF_CHANNELS )
			{
				if ( ppm_ch == FAILSAFE_INPUT_CHANNEL && udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN && udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX )
				{
					failSafePulses++ ;
				}
				if ( (pulse < PPM_FAILSAFE_INPUT_MIN ) )
				{
					frameOK = false ;
				}
				ppm_ch++ ;		//scan next channel
			}
			else 
			{
				if (frameOK == false)
				{
					failSafePulses = 0 ;
					udb_flags._.radio_on = 0 ;
					LED_GREEN = LED_OFF ;
				}
				else
				{
					ppm_ch = 1 ;								//reset
					frameOK = true ;
				}
			}
		}
	}
#endif

	return ;
}

#endif

#endif
