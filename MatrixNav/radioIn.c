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
	T2CON = 0b1000000000000000  ; // turn on timer 2 with no prescaler
	TRISD = 0b1111111111111111 ; // make the d port input, to enable IC1 and IC2
	IC1CON = IC2CON = IC7CON = IC8CON = 0b0010000010000001 ;

	IPC0bits.IC1IP = IPC4bits.IC7IP = IPC4bits.IC8IP = 7 ; // priority 7
	IFS0bits.IC1IF = IFS1bits.IC7IF = IFS1bits.IC8IF = 0 ; // clear the interrupt
	IEC0bits.IC1IE = IEC1bits.IC7IE = IEC1bits.IC8IE = 1 ; // turn on the interrupt
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _IC1Interrupt(void)
{
	indicate_loading_inter ;
	if (PORTDbits.RD0)
	{
		 risec1 = IC1BUF ;
	}
	else
	{
		pwc1 = ((IC1BUF - risec1) >> 1 );
		if ( (pwc1> 1500) && (pwc1<4500 ) ) pulsesselin++ ;
	}
	IFS0bits.IC1IF =  0 ; // clear the interrupt
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _IC2Interrupt(void)
{
	indicate_loading_inter ;
	if (PORTDbits.RD1)
	{
		 risec2 = IC2BUF ;
	}
	else
	{
		pwc2 = ((IC2BUF - risec2) >> 1 ) ;
	}

	IFS0bits.IC2IF = 0 ; // clear the interrupt
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _IC7Interrupt(void)
{
	indicate_loading_inter ;
	if (PORTBbits.RB4)
	{
		 risec7 = IC7BUF ;
	}
	else
	{
		pwc7 = ((IC7BUF - risec7) >> 1 ) ;
	}

	IFS1bits.IC7IF = 0 ; // clear the interrupt
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _IC8Interrupt(void)
{
	indicate_loading_inter ;
	if (PORTBbits.RB5)
	{
		 risec8 = IC8BUF ;
	}
	else
	{
		pwc8 = ((IC8BUF - risec8) >> 1 ) ;

	}

	IFS1bits.IC8IF = 0 ; // clear the interrupt
	return ;
}
