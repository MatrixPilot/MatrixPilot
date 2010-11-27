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

#if (RECORD_FREE_STACK_SPACE == 1)
unsigned int maxstack = 0 ;
#endif


void udb_init_ADC( void )
{
	TRISB =  0b0000000111111111 ; // all inputs
	ADCON1 = 0b0010001111100100 ; // signed fractional , auto convert , seq, auto samp
//	ADCON2 = 0b0000010000011000 ; // supply ref, scana ch0, int every 7, 16word, usa A only
	ADCON2 = ADCON2CONFIG ;
//	ADCON3 = 0b0001111100111111 ; // slowest possible, approximately 500 samples per second for each channel
	ADCON3 = 0b0000001100111111 ;
	ADCHS  = 0b0000000000000001 ; // channel AN1
	ADPCFG = 0b1111111000110000 ; // analog inputs on 8 7 6 3 2 1 0
	ADCSSL = 0b0000000111001111 ; 
	
	udb_flags._.firstsamp = 1 ;
	
	_ADIF = 0 ; 	// clear the AD interrupt
	_ADIP = 5 ;     // priority 5
	_ADIE = 1 ;     // enable the interrupt
	_ADON = 1 ;	// turn on the A to D
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _ADCInterrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
#if (RECORD_FREE_STACK_SPACE == 1)
	unsigned int stack = WREG15 ;
	if ( stack > maxstack )
	{
		maxstack = stack ;
	}
#endif
	
	udb_xrate.input =  xrateBUFF  ;
	udb_yrate.input =  yrateBUFF  ;
	udb_zrate.input =  zrateBUFF ;
#ifdef VREF
	udb_vref.input  =   vrefBUFF ;
#endif
	udb_xaccel.input =   xaccelBUFF ;
	udb_yaccel.input =   yaccelBUFF ;
	udb_zaccel.input =   zaccelBUFF ;
	if ( udb_flags._.firstsamp )	// use the first sample to initialize the filters
	{
		udb_flags._.firstsamp = 0 ;
		udb_xaccel.value = udb_xaccel.input ;
		udb_yaccel.value = udb_yaccel.input ;
		udb_zaccel.value = udb_zaccel.input ;
#ifdef VREF
		udb_vref.value   = udb_vref.input ;
#endif
		udb_xrate.value = udb_xrate.input ;
		udb_yrate.value = udb_yrate.input ;
		udb_zrate.value = udb_zrate.input ;
		IEC2bits.PWMIE = 1 ;     // enable the PWM interrupt
	}
	else
	{
		// perform just a little bit of filtering to improve signal to noise
 	    udb_xaccel.value = udb_xaccel.value + (( (udb_xaccel.input>>1) - (udb_xaccel.value>>1) )>> FILTERSHIFT ) ;
	    udb_xrate.value = udb_xrate.value + (( (udb_xrate.input>>1) - (udb_xrate.value>>1) )>> FILTERSHIFT ) ;
	    udb_yaccel.value = udb_yaccel.value + (( ( udb_yaccel.input>>1) - (udb_yaccel.value>>1) )>> FILTERSHIFT ) ;
	    udb_yrate.value = udb_yrate.value + (( (udb_yrate.input>>1) - (udb_yrate.value>>1) )>> FILTERSHIFT ) ;
	    udb_zaccel.value = udb_zaccel.value + (( (udb_zaccel.input>>1) - (udb_zaccel.value>>1) )>> FILTERSHIFT ) ;
	    udb_zrate.value = udb_zrate.value + ((( udb_zrate.input>>1) - (udb_zrate.value>>1) )>> FILTERSHIFT ) ;
#ifdef VREF
		udb_vref.value = udb_vref.value + (( (udb_vref.input>>1) - (udb_vref.value>>1) )>> FILTERSHIFT ) ;
#endif
	}
	
	_ADIF = 0 ; 	// clear the AD interrupt
	
	interrupt_restore_extended_state ;
	return ;
}

#endif
