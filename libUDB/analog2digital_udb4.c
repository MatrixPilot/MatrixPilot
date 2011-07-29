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

#if (BOARD_TYPE == UDB4_BOARD)

//	Analog to digital processing.
//	Sampling and conversion is done automatically, so that all that needs to be done during 
//	interrupt processing is to read the data out of the buffer.
//	Raw samples are taken approximately 500 per second per channel.
//	A first order digital lowpass filter with a time constant of about 32 milliseconds 
//  is applied to improve signal to noise.

//	Variables.

struct ADchannel udb_xaccel, udb_yaccel , udb_zaccel ; // x, y, and z accelerometer channels
struct ADchannel udb_xrate , udb_yrate, udb_zrate ;  // x, y, and z gyro channels
struct ADchannel udb_vref ; // reference voltage
int vref_adj ;

#if (RECORD_FREE_STACK_SPACE == 1)
unsigned int maxstack = 0 ;
#endif

unsigned int sample_count = 0 ;

#define ALMOST_ENOUGH_SAMPLES 18 // there are 22 or 23 samples in a sum


char sampcount = 1 ;


void udb_init_gyros( void )
{
	// turn off auto zeroing 
	_TRISC4 = _TRISB14 = 0 ;
	_LATC4 = _LATB14 = 0 ;
	
	return ;
}


void udb_init_accelerometer(void)
{
	_TRISA6 = 0 ;  // GSELECT is an output
	_LATA6 = 1 ;   // 6 G setting
	
	//	set as inputs:
	_TRISB9 = 1 ;
	_TRISB10 = 1 ;
	_TRISB11 = 1 ;
	
	return ;
}


void udb_init_ADC( void )
{
	udb_init_gyros() ;
	udb_init_accelerometer() ;
	
	AD2CSSL = 0 ; // start with no channels selected
	AD2PCFGL = 0b1111111111111111 ; // start with all digital, set the A/D
	
//	include the 110 degree/second scale, gyro
	/*
	AD2CSSLbits.CSS0 = 1 ;
	AD2CSSLbits.CSS3 = 1 ;
	AD2CSSLbits.CSS5 = 1 ;

	AD2PCFGLbits.PCFG0 = 0 ;
	AD2PCFGLbits.PCFG3 = 0 ;
	AD2PCFGLbits.PCFG5 = 0 ;
	*/
	
//	include the 500 degree/second scale, gyro
	AD2CSSLbits.CSS1 = 1 ;
	AD2CSSLbits.CSS4 = 1 ;
	AD2CSSLbits.CSS6 = 1 ;
	
	AD2PCFGLbits.PCFG1 = 0 ;
	AD2PCFGLbits.PCFG4 = 0 ;
	AD2PCFGLbits.PCFG6 = 0 ;
	
//	include the accelerometer in the scan:
	AD2CSSLbits.CSS9 = 1 ;
	AD2CSSLbits.CSS10 = 1 ;
	AD2CSSLbits.CSS11 = 1 ;
	
	AD2PCFGLbits.PCFG9 = 0 ;
	AD2PCFGLbits.PCFG10 = 0 ;
	AD2PCFGLbits.PCFG11 = 0 ;
	
	AD2CON1bits.AD12B = 1 ;		// 12 bit A to D
	AD2CON1bits.FORM = 3 ;		// signed fractional
	AD2CON1bits.SSRC = 7 ;		// auto convert
	AD2CON1bits.ASAM = 1 ;		// auto samp
	AD2CON1bits.SIMSAM = 0 ;	// multiple channels in sequence
	
	AD2CON2bits.VCFG = 0 ;		// use supply as reference voltage
	AD2CON2bits.CSCNA = 1 ;		// scanA ch0
	
	AD2CON2bits.ALTS = 0 ;		// always A
//	AD2CON2bits.BUFM = 1 ;		// ping-pong buffers
	
	AD2CON3bits.SAMC = 31 ;		// auto sample time = 31 TAD, approximately 248 microseconds
	AD2CON3bits.ADCS = 127 ;	// TAD = 8 microseconds
	
//	AD2CON1bits.ADDMABM = 1 ;	// DMA buffer written in conversion order
	AD2CON2bits.SMPI = 5 ;		// 6 samples
//	AD2CON4bits.DMABL = 1 ;		// double buffering
	
	_AD2IF = 0 ;				// clear the AD interrupt
	_AD2IP = 5 ;				// priority 5
	_AD2IE = 1 ;				// enable the interrupt
	AD2CON1bits.ADON = 1 ;		// turn on the A to D
	
	return ;
}


#define ADC2SAMPLE ((int)(ADC2BUF0))

void __attribute__((__interrupt__,__no_auto_psv__)) _ADC2Interrupt(void)
{	
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_AD2IF = 0 ; 	// clear the AD interrupt
	
#if (RECORD_FREE_STACK_SPACE == 1)
	unsigned int stack = WREG15 ;
	if ( stack > maxstack )
	{
		maxstack = stack ;
	}
#endif
	
#if (HILSIM != 1)
	switch ( sampcount ) {
		case yrateBUFF :
			udb_yrate.input = ADC2SAMPLE ;
			break;
			
		case zrateBUFF :
			udb_zrate.input = ADC2SAMPLE ;
			break;
			
		case xrateBUFF :
			udb_xrate.input = ADC2SAMPLE ;
			break;
			
		case zaccelBUFF :
			udb_zaccel.input = ADC2SAMPLE ;
			break;
			
		case xaccelBUFF :
			udb_xaccel.input = -ADC2SAMPLE ;
			break;
			
		case yaccelBUFF :
			udb_yaccel.input = -ADC2SAMPLE ;
			break;
			
		default :
			break;
	}
#else
	(void)ADC2SAMPLE ;	// HILSIM: Pull the sample, but ignore it
#endif
	
	sampcount++ ;
	if ( sampcount > 6 )
	{
		sampcount = 1 ;
		
		if ( udb_flags._.a2d_read == 1 ) // prepare for the next reading
		{
			udb_flags._.a2d_read = 0 ;
			udb_xrate.sum = udb_yrate.sum = udb_zrate.sum = 0 ;
			udb_xaccel.sum = udb_yaccel.sum = udb_zaccel.sum = 0 ;
#ifdef VREF
			udb_vref.sum = 0 ;
#endif
			sample_count = 0 ;
		}
		
		//	perform the integration:
		udb_xrate.sum += udb_xrate.input ;
		udb_yrate.sum += udb_yrate.input ;
		udb_zrate.sum += udb_zrate.input ;
#ifdef VREF
		udb_vref.sum  +=   udb_vref.input ;
#endif
		udb_xaccel.sum += udb_xaccel.input ;
		udb_yaccel.sum += udb_yaccel.input ;
		udb_zaccel.sum += udb_zaccel.input ;
		sample_count ++ ;
		
		//	When there is a chance that read_gyros() and read_accel() will execute soon,
		//  have the new average values ready.
		if ( sample_count > ALMOST_ENOUGH_SAMPLES )
		{	
			udb_xrate.value = __builtin_divsd( udb_xrate.sum , sample_count ) ;
			udb_yrate.value = __builtin_divsd( udb_yrate.sum , sample_count ) ;
			udb_zrate.value = __builtin_divsd( udb_zrate.sum , sample_count ) ;
#ifdef VREF
			udb_vref.value = __builtin_divsd( udb_vref.sum , sample_count ) ;
#endif
			udb_xaccel.value =  __builtin_divsd( udb_xaccel.sum , sample_count ) ;
			udb_yaccel.value =  __builtin_divsd( udb_yaccel.sum , sample_count ) ;
			udb_zaccel.value =  __builtin_divsd( udb_zaccel.sum , sample_count ) ;
		}
	}
	
	interrupt_restore_corcon ;
	return ;
}

#endif
