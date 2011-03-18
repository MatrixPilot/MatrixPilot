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

unsigned int sample_count = 0 ;

#if ( CLOCK_CONFIG == CRYSTAL_CLOCK ) // 2400 samples/sec
#define ALMOST_ENOUGH_SAMPLES 54 // there are 59 or 60 samples in a sum
#elif ( CLOCK_CONFIG == FRC8X_CLOCK ) // 8800 samples/sec
#define ALMOST_ENOUGH_SAMPLES 214 // there are 219 or 220 samples in a sum
#endif
#define ADCON3CONFIG 0b0000001100011111

void udb_init_ADC( void )
{
	TRISB =  0b0000000111111111 ; // all inputs
	ADCON1 = 0b0010001111100100 ; // signed fractional , auto convert , seq, auto samp
	ADCON2 = ADCON2CONFIG ;
	ADCON3 = ADCON3CONFIG ;
	ADCHS  = 0b0000000000000001 ; // channel AN1
	ADPCFG = 0b1111111000110000 ; // analog inputs on 8 7 6 3 2 1 0
	ADCSSL = 0b0000000111001111 ; 
	
	udb_flags._.a2d_read = 0 ;

	udb_xrate.sum = udb_yrate.sum = udb_zrate.sum = 0 ;
	udb_xaccel.sum = udb_yaccel.sum = udb_zaccel.sum = 0 ;
#ifdef VREF
	udb_vref.sum = 0 ;
#endif
	sample_count = 0 ;
	
	_ADIF = 0 ; 	// clear the AD interrupt
	_ADIP = 5 ;     // priority 5
	_ADIE = 1 ;     // enable the interrupt
	_ADON = 1 ;	// turn on the A to D
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _ADCInterrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
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

	_ADIF = 0 ; 	// clear the AD interrupt
	
	interrupt_restore_corcon ;
	return ;
}

#endif
