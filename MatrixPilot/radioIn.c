#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	Measure the pulse widths of the servo channel inputs from the radio.
//	The dsPIC makes this rather easy to do using its capture feature.

//	One of the channels is also used to validate pulse widths to detect loss of radio.

//	The pulse width inputs can be directly converted to units of pulse width outputs to control
//	the servos by simply dividing by 2.

void init_capture(void)
{
	T2CON = 0b1000000000000000  ;	// turn on timer 2 with no prescaler
	TRISD = 0b1111111111111111 ;	// make the d port input, to enable IC1 and IC2
	TRISFbits.TRISF6 = 1 ;			// make F6 an input to enable the 3rd switch
	IC1CON = IC2CON = IC7CON = IC8CON = 0b0010000010000001 ;
	
	int i;
	for (i=0; i <= NUM_INPUTS; i++)
		pwIn[i] = pwTrim[i] = 3000;
	
	pwIn[THROTTLE_INPUT_CHANNEL] = pwTrim[THROTTLE_INPUT_CHANNEL] = 0 ;
	
#if (NORADIO == 1)
		pwIn[MODE_SWITCH_INPUT_CHANNEL] = pwTrim[MODE_SWITCH_INPUT_CHANNEL] = 4000 ;
#endif
	
	IPC0bits.IC1IP = IPC1bits.IC2IP = IPC4bits.IC7IP = IPC4bits.IC8IP = 6 ; // priority 6
	IFS0bits.IC1IF = IFS0bits.IC2IF = IFS1bits.IC7IF = IFS1bits.IC8IF = 0 ; // clear the interrupt
	
	if (NUM_INPUTS > 0) IEC1bits.IC7IE = 1 ; // turn on interrupt for input 1
	if (NUM_INPUTS > 1) IEC1bits.IC8IE = 1 ; // turn on interrupt for input 2
	if (NUM_INPUTS > 2) IEC0bits.IC2IE = 1 ; // turn on interrupt for input 3
	if (NUM_INPUTS > 3) IEC0bits.IC1IE = 1 ; // turn on interrupt for input 4
	
	if (NUM_INPUTS > 4)
	{
		TRISEbits.TRISE8 = 1 ;	 // set E8 to be an input pin
		INTCON2bits.INT0EP = 0;  // Set up the 5th input channel to start out reading low-to-high edges
		IPC0bits.INT0IP = 7 ; // priority 7
		IFS0bits.INT0IF = 0 ; // clear the interrupt
		IEC0bits.INT0IE = 1 ; // turn on the interrupt
	}
	
	return ;
}

extern int failSafePulses ;

// Input Channel 1
void __attribute__((__interrupt__,__no_auto_psv__)) _IC7Interrupt(void)
{
	unsigned int time ;	
	IFS1bits.IC7IF = 0 ; // clear the interrupt
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
		pwIn[1] = ((time - rise[1]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 1 )
		if ( (pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			pulsesselin++ ;
			failSafePulses++ ;
		}
		else
		{
			pulsesselin = failSafePulses = 0 ;
			flags._.radio_on = 0 ;
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
	IFS1bits.IC8IF = 0 ; // clear the interrupt
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
		pwIn[2] = ((time - rise[2]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 2 )
		if ( (pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			pulsesselin++ ;
			failSafePulses++ ;
		}
		else
		{
			pulsesselin = failSafePulses = 0 ;
			flags._.radio_on = 0 ;
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
	IFS0bits.IC2IF = 0 ; // clear the interrupt
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
		pwIn[3] = ((time - rise[3]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 3 )
		if ( (pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			pulsesselin++ ;
			failSafePulses++ ;
		}
		else
		{
			pulsesselin = failSafePulses = 0 ;
			flags._.radio_on = 0 ;
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
	IFS0bits.IC1IF =  0 ; // clear the interrupt
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
		pwIn[4] = ((time - rise[4]) >> 1 );
		
#if ( FAILSAFE_INPUT_CHANNEL == 4 )
		if ( (pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			pulsesselin++ ;
			failSafePulses++ ;
		}
		else
		{
			pulsesselin = failSafePulses = 0 ;
			flags._.radio_on = 0 ;
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
		INTCON2bits.INT0EP = 1 ;	// Set up the interrupt to read high-to-low edges
	}
	else
	{
		pwIn[5] = ((t - rise[5]) >> 1 ) ;
		
#if ( FAILSAFE_INPUT_CHANNEL == 5 )
		if ( (pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX ) )
		{
			pulsesselin++ ;
			failSafePulses++ ;
		}
		else
		{
			pulsesselin = failSafePulses = 0 ;
			flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
#endif
		INTCON2bits.INT0EP = 0 ;	// Set up the interrupt to read low-to-high edges
	}
#endif
	
	IFS0bits.INT0IF = 0 ; 		// clear the interrupt
	
	return;
}
