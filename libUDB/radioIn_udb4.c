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

#if (BOARD_SUPPORT_RADIO_IN == 1)
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
#define MIN_SYNC_PULSE_WIDTH 17500	// 3.5ms * 5
unsigned int rise_ppm ;				// rising edge clock capture for PPM radio input
#endif


void udb_init_capture(void)
{
	int i;

#if(USE_NV_MEMORY == 1)
	if(udb_skip_flags.skip_radio_trim == 0)
	{	
#endif
		for (i=0; i <= NUM_INPUTS; i++)
	#if (FIXED_TRIMPOINT == 1)
			if(i == THROTTLE_OUTPUT_CHANNEL)
				udb_pwTrim[i] = udb_pwIn[i] = THROTTLE_TRIMPOINT;
			else
				udb_pwTrim[i] = udb_pwIn[i] = CHANNEL_TRIMPOINT;			
	#else
			udb_pwTrim[i] = udb_pwIn[i] = 0 ;
	#endif
#if(USE_NV_MEMORY == 1)
	}
#endif
	
	TMR2 = 0 ; 				// initialize timer
	T2CONbits.TCKPS = 1 ;	// prescaler = 8 option
	T2CONbits.TCS = 0 ;		// use the internal clock
	T2CONbits.TON = 1 ;		// turn on timer 2
	

	//	configure the capture pins
#if (BOARD_TYPE == MADRE_BOARD)	
	_IC1R 		= 7;			// Connect IC1 to Pin 43, RP7
#endif

	IC1CONbits.ICTMR = 1 ;  // use timer 2
#if (USE_PPM_INPUT != 1)
	IC1CONbits.ICM = 1 ; // capture every edge
#elif (PPM_SIGNAL_INVERTED == 0)
	IC1CONbits.ICM = 3 ; // capture the rising edge
#elif (PPM_SIGNAL_INVERTED == 1)
	IC1CONbits.ICM = 2 ; // capture the falling edge
#endif
	TRIS_IN1 = 1 ;
	_IC1IP = 6 ;
	_IC1IF = 0 ;
	if (NUM_INPUTS > 0) _IC1IE = 1 ;
	
#if (USE_PPM_INPUT != 1)
	#if (NUM_INPUTS > 1)
		IC2CON 		= IC1CON ;
		TRIS_IN2 	= TRIS_IN1 ;
		_IC2IP 		= _IC1IP ;
		_IC2IF 		= _IC1IF ;
		_IC2IE 		= 1 ;
	#endif
	
	#if (NUM_INPUTS > 2)
		IC3CON 		= IC1CON ;
		TRIS_IN3 	= TRIS_IN1 ;
		_IC3IP 		= _IC1IP ;
		_IC3IF 		= _IC1IF ;
		_IC3IE 		= 1 ;
	#endif
	
	#if (NUM_INPUTS > 3)
		IC4CON 		= IC1CON ;
		TRIS_IN4 	= TRIS_IN1 ;
		_IC4IP 		= _IC1IP ;
		_IC4IF 		= _IC1IF ;
		_IC4IE 		= 1 ;
	#endif
	
	#if (NUM_INPUTS > 4)
		IC5CON 		= IC1CON ;
		TRIS_IN5 	= TRIS_IN1 ;
		_IC5IP 		= _IC1IP ;
		_IC5IF 		= _IC1IF ;
		_IC5IE 		= 1 ;
	#endif
	
	#if (NUM_INPUTS > 5)
		IC6CON 		= IC1CON ;
		TRIS_IN6 	= TRIS_IN1 ;
		_IC6IP 		= _IC1IP ;
		_IC6IF 		= _IC1IF ;
		_IC6IE 		= 1 ;
	#endif
	
	#if (NUM_INPUTS > 6)
		IC7CON 		= IC1CON ;
		TRIS_IN7 	= TRIS_IN1 ;
		_IC7IP 		= _IC1IP ;
		_IC7IF 		= _IC1IF ;
		_IC7IE 		= 1 ;
	#endif
	
	#if (NUM_INPUTS > 7)
		IC8CON 		= IC1CON ;
		TRIS_IN8 	= TRIS_IN1 ;
		_IC8IP 		= _IC1IP ;
		_IC8IF 		= _IC1IF ;
		_IC8IE 		= 1 ;
	#endif
	
#endif
	
	return ;
}


#if (USE_PPM_INPUT != 1)

// Input Channel 1
#if (NUM_INPUTS > 0)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC1Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;	
	_IC1IF = 0 ; // clear the interrupt
	while ( IC1CONbits.ICBNE )
	{
		time = IC1BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD8)
	{
		 rise[1] = time ;
	}
	else
	{
		udb_pwIn[1] = time - rise[1] ;
		
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
	
	interrupt_restore_corcon ;
	return ;
}
#endif

// Input Channel 2
#if (NUM_INPUTS > 1)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC2Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;
	_IC2IF = 0 ; // clear the interrupt
	while ( IC2CONbits.ICBNE )
	{
		time = IC2BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD9)
	{
		 rise[2] = time ;
	}
	else
	{
		udb_pwIn[2] = time - rise[2] ;
		
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
	
	interrupt_restore_corcon ;
	return ;
}
#endif

// Input Channel 3
#if (NUM_INPUTS > 2)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC3Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;
	_IC3IF = 0 ; // clear the interrupt
	while ( IC3CONbits.ICBNE )
	{
		time = IC3BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD10)
	{
		 rise[3] = time ;
	}
	else
	{
		udb_pwIn[3] = time - rise[3] ;
		
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
	
	interrupt_restore_corcon ;
	return ;
}
#endif

// Input Channel 4
#if (NUM_INPUTS > 3)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC4Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;
	_IC4IF =  0 ; // clear the interrupt
	while ( IC4CONbits.ICBNE )
	{
		time = IC4BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD11)
	{
		 rise[4] = time ;
	}
	else
	{
		udb_pwIn[4] = time - rise[4] ;
		
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
	
	interrupt_restore_corcon ;
	return ;
}
#endif


// Input Channel 5
#if (NUM_INPUTS > 4)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC5Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;
	_IC5IF =  0 ; // clear the interrupt
	while ( IC5CONbits.ICBNE )
	{
		time = IC5BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD12)
	{
		 rise[5] = time ;
	}
	else
	{
		udb_pwIn[5] = time - rise[5] ;
		
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
	
	}
#endif
	
	interrupt_restore_corcon ;
	return ;
}
#endif


// Input Channel 6
#if (NUM_INPUTS > 5)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC6Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;
	_IC6IF =  0 ; // clear the interrupt
	while ( IC6CONbits.ICBNE )
	{
		time = IC6BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD13)
	{
		 rise[6] = time ;
	}
	else
	{
		udb_pwIn[6] = time - rise[6] ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 6 )
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
	
	interrupt_restore_corcon ;
	return ;
}
#endif


// Input Channel 7
#if (NUM_INPUTS > 6)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC7Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;
	_IC7IF =  0 ; // clear the interrupt
	while ( IC7CONbits.ICBNE )
	{
		time = IC7BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD14)
	{
		 rise[7] = time ;
	}
	else
	{
		udb_pwIn[7] = time - rise[7] ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 7 )
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
	
	interrupt_restore_corcon ;
	return ;
}
#endif


// Input Channel 8
#if (NUM_INPUTS > 7)
void __attribute__((__interrupt__,__no_auto_psv__)) _IC8Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;
	_IC8IF =  0 ; // clear the interrupt
	while ( IC8CONbits.ICBNE )
	{
		time = IC8BUF ;
	}
	
#if ( NORADIO != 1 )
	if (PORTDbits.RD15)
	{
		 rise[8] = time ;
	}
	else
	{
		udb_pwIn[8] = time - rise[8] ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 8 )
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
	
	interrupt_restore_corcon ;
	return ;
}
#endif

#else // #if (USE_PPM_INPUT == 1)

#if (PPM_SIGNAL_INVERTED == 1)
#define PPM_PULSE_VALUE 0
#else
#define PPM_PULSE_VALUE 1
#endif

unsigned char ppm_ch = 0 ;

// PPM Input on Channel 1
void __attribute__((__interrupt__,__no_auto_psv__)) _IC1Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	unsigned int time ;	
	_IC1IF = 0 ; // clear the interrupt
	while ( IC1CONbits.ICBNE )
	{
		time = IC1BUF ;
	}
	
#if ( NORADIO != 1 )

	unsigned int pulse = time - rise_ppm ;
	rise_ppm = time ;
	
	if (pulse > MIN_SYNC_PULSE_WIDTH )			//sync pulse
	{
		ppm_ch = 1 ;
	}
	else
	{
		if (ppm_ch > 0 && ppm_ch <= PPM_NUMBER_OF_CHANNELS)
		{
			if (ppm_ch <= NUM_INPUTS)
			{
				udb_pwIn[ppm_ch] = pulse ;
				
				if ( ppm_ch == FAILSAFE_INPUT_CHANNEL && udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN && udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX )
				{
					failSafePulses++ ;
				}
			}
			ppm_ch++ ;		//scan next channel
			if( ppm_ch == PPM_NUMBER_OF_CHANNELS )		// Capture the last non value edge
			{
				IC1CONbits.ICM = 0 ; // The user application must disable the Input Capture module (i.e., ICM<2:0> = 000 ) before changing a capture mode.
			#if (PPM_SIGNAL_INVERTED == 0)
				IC1CONbits.ICM = 2 ; // capture the falling edge
			#elif (PPM_SIGNAL_INVERTED == 1)
				IC1CONbits.ICM = 3 ; // capture the rising edge
			#endif
			}
			else if( ppm_ch >= PPM_NUMBER_OF_CHANNELS )	// Restore the default capture config
			{
				IC1CONbits.ICM = 0 ; // The user application must disable the Input Capture module (i.e., ICM<2:0> = 000 ) before changing a capture mode.
			#if (PPM_SIGNAL_INVERTED == 0)
				IC1CONbits.ICM = 3 ; // capture the rising edge
			#elif (PPM_SIGNAL_INVERTED == 1)
				IC1CONbits.ICM = 2 ; // capture the falling edge
			#endif				
			}	
		}
	}
#endif

	interrupt_restore_corcon ;
	return ;
}

#endif

#endif
