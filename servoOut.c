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


#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int gpscount ; // counter to initialize GPS
int outputNum ;

void setupOutputs( void ) ;
void manualPassthrough( void ) ;


void init_pwm( void )	// initialize the PWM
{
	PDC1 = PDC2 = PDC3 = 3000 ;
	
	firstsamp = 1;	// flag for the first sample
	calibcount = 400 ; // wait 400 PWM pulses before turning on the control (10 seconds)
	gpscount = 1000 ; // wait 25 seconds for GPS to initialize

	int i;
	for (i=0; i <= NUM_OUTPUTS; i++)
		pwOut[i] = 3000;
	
	pwOut[THROTTLE_OUTPUT_CHANNEL] = 0 ;
	
	TRISE = 0b1111111111000000 ;
	PTPER = 25000 ;			// 25 millisecond period at 16 Mz clock, prescale = 4	
	PTCONbits.PTCKPS = 1;	// prescaler = 4

	PWMCON1bits.PMOD1 = 1 ; // independent PWM mode
	PWMCON1bits.PMOD2 = 1 ;
	PWMCON1bits.PMOD3 = 1 ;
	PWMCON1bits.PEN1L = 0 ; // low pins used as digital I/O
	PWMCON1bits.PEN2L = 0 ;
	PWMCON1bits.PEN3L = 0 ;
	
	PTCONbits.PTEN = 1; 	// turn on the PWM 
	IFS2bits.PWMIF = 0 ; 	// clear the PWM interrupt
	IPC9bits.PWMIP = 3 ;    // priority 3
	
	if (NUM_OUTPUTS > 3)
	{
		T4CON = 0b1000000000000000  ;	// turn on timer 4 with no prescaler
		IPC5bits.T4IP = 7 ;				// priority 7
		IEC1bits.T4IE = 0 ;				// disable timer 4 interrupt for now (enable for each set of pulses)
	}
	
	//  note: at this point the PWM is running, so there are pulses going out,
	//	but the PWM interrupt is still off, so no interrupts are coming in yet to compute pulses.
	//  the PWM interrupt is turned on within the A/D interrupt processing
	
	return ;
}

int pulsesat ( long pw ) // saturation logic to maintain pulse width within bounds
{
	if ( pw > SERVOMAX ) pw = SERVOMAX ;
	if ( pw < SERVOMIN ) pw = SERVOMIN ;
	return (int)pw ;
}


int fourHertzCounter = 0 ;
int startTelemetry = 0 ;

int twentyHertzCounter = 0 ;
int failSafePulses = 0 ;

void __attribute__((__interrupt__,__no_auto_psv__)) _PWMInterrupt(void)
{
	// interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	//	Executes whatever needs to be done every 25 milliseconds, using the PWM clock.
	//	This is a good place to run the A/D digital filters and compute pulse widths for servos.
	//	Also, this is used to wait a few pulses before recording input DC offsets.

#if ( NORADIO == 0 )	
	twentyHertzCounter++ ;
	if ( twentyHertzCounter >= 2 )
	{
		if ( failSafePulses == 0 )
		{
			flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
		twentyHertzCounter = 0 ;
		failSafePulses = 0 ;
	}
#endif
	
	// This is a simple counter to do stuff at 4hz
	fourHertzCounter++ ;
	if ( fourHertzCounter >= 10 )
	{
		if ( startTelemetry )
		{
			serial_output_4hz() ;
			rxMagnetometer() ;
		}
		fourHertzCounter = 0 ;
	}
	
	
	switch ( calibcount ) {
	// case 0 is when the control is up and running

	case 0: {
		imu() ;
		updateBehavior() ;
		rollCntrl() ;
		yawCntrl() ;
		altitudeCntrl();
		pitchCntrl() ;
		servoMix() ;
#if ( USE_CAMERA_STABILIZATION == 1 )
		cameraCntrl();
#endif
		updateTriggerAction() ;
		break ;
	}

	case 1: {
		// almost ready to turn the control on, save the input offsets
		xaccel.offset = xaccel.value ;
		xrate.offset = xrate.value ;
		yaccel.offset = yaccel.value ;
		yrate.offset = yrate.value ;
		zaccel.offset = zaccel.value - ((int)(2*GRAVITY)) ; // GRAVITY is measured in A-D/2 units
		zrate.offset = zrate.value ;
#ifdef VREF
		vref.offset = vref.value ;
#endif
		manualPassthrough() ;	// Allow manual control while starting up
		startTelemetry = 1 ;
		break ;
	}
	default: {
		// otherwise, there is not anything to do
		manualPassthrough() ;	// Allow manual control while starting up
		break ;
	}
	}
	
	setupOutputs() ;
	
	// count down the startup counter to 0
	if ( calibcount > 0 ) calibcount-- ;

	
	// count down the startup counter to 0
	gps_startup_sequence(gpscount) ;

	if ( gpscount > 0 ) gpscount-- ;
	
	IFS2bits.PWMIF = 0 ; /* clear the interrupt */
	
	// interrupt_restore_extended_state ;
	return ;
}

void manualPassthrough( void )
{
	roll_control = pitch_control = yaw_control = altitude_control = 0 ;
	servoMix() ;
	
	return ;
}

void setupOutputs( void )
{
	PDC1 = pwOut[1] ;
	PDC2 = pwOut[2] ;
	PDC3 = pwOut[3] ;
	
	if (NUM_OUTPUTS > 3)
	{
		outputNum = 4 ;
		if ( pwOut[4] > 0 )
		{
			PR4 = (pwOut[4] << 1) ;	// set timer to the pulse width
			LATEbits.LATE0 = 1 ;	// start the pulse by setting the E0 pin high (output 4)
		}
		else
		{
			PR4 = 100 ;				// set timer to a short wait
			LATEbits.LATE0 = 0 ;	// skip the pulse by setting the E0 pin low (output 4)
		}	
		TMR4 = 0 ;				// start timer at 0
		IFS1bits.T4IF = 0 ;		// clear the interrupt
		IEC1bits.T4IE = 1 ;		// enable timer 4 interrupt
	}
	
	return;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _T4Interrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	switch ( outputNum ) {
		case 4:
			LATEbits.LATE0 = 0 ;		// end the pulse by setting the E0 pin low (output 4)
			if (NUM_OUTPUTS > 4)
			{
				outputNum = 5 ;
				if ( pwOut[5] > 0 )
				{
					PR4 = (pwOut[5] << 1) ;	// set timer to the pulse width
					LATEbits.LATE2 = 1 ;	// start the pulse by setting the E2 pin high (output 5)
				}
				else
				{
					PR4 = 100 ;				// set timer to the pulse width
					LATEbits.LATE2 = 0 ;	// skip the pulse by setting the E2 pin low (output 5)
				}	
				TMR4 = 0 ;				// start timer at 0
			}
			else
			{
				IEC1bits.T4IE = 0 ;		// disable timer 4 interrupt
			}
			break ;
		
		case 5:
			LATEbits.LATE2 = 0 ;		// end the pulse by setting the E2 pin low (output 5)
			if (NUM_OUTPUTS > 5)
			{
				outputNum = 6 ;
				if ( pwOut[6] > 0 )
				{
					PR4 = (pwOut[6] << 1) ;	// set timer to the pulse width
					LATEbits.LATE4 = 1 ;	// start the pulse by setting the E4 pin high (output 6)
				}
				else
				{
					PR4 = 100 ;				// set timer to the pulse width
					LATEbits.LATE4 = 0 ;	// start the pulse by setting the E4 pin high (output 6)
				}
				TMR4 = 0 ;				// start timer at 0
			}
			else
			{
				IEC1bits.T4IE = 0 ;		// disable timer 4 interrupt
			}
			break ;
		case 6:
			LATEbits.LATE4 = 0 ;		// end the pulse by setting the E4 pin low (output 6)
			IEC1bits.T4IE = 0 ;			// disable timer 4 interrupt
			break ;
	}
	
	IFS1bits.T4IF = 0 ;					// clear the interrupt
	
	interrupt_restore_extended_state ;
	return;
}
