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
#include "filters.h"

#if (BOARD_TYPE & AUAV2_BOARD)

//	Analog to digital processing.
//	Sampling and conversion is done automatically, so that all that needs to be done during
//	interrupt processing is to read the data out of the buffer.
//	Raw samples are taken approximately 500 per second per channel.
//	A first order digital lowpass filter with a time constant of about 32 milliseconds
//  is applied to improve signal to noise.

//	Variables.

struct ADchannel udb_vref ; // reference voltage
struct ADchannel primaryV ; // primary battery voltage

#if (NUM_ANALOG_INPUTS >= 1)
struct ADchannel udb_analogInputs[NUM_ANALOG_INPUTS] ; // 0-indexed, unlike servo pwIn/Out/Trim arrays
#endif


int vref_adj ;

#if (RECORD_FREE_STACK_SPACE == 1)
unsigned int maxstack = 0 ;
#endif

// dsPIC33FJXXXGPX06A/X08A/X10A
// minimum allowed 12-bit ADC clock period is 118ns or 8.47MHz
// minimum allowed 10-bit ADC clock period is 76ns, or 13.16MHz
// 12 bit mode conversion time is 14 TAD cycles
// total sample/conversion time is (14+SAMC) * TAD
// for 1.6usec TAD, total is 72usec for 13.9KHz conversion rate
// *** observed 72usec interval between interrupts on scope => interrupt every sample

// ADCS bits 6 and 7 are reserved; max divisor is 64
#define ADCLK_DIV_N_MINUS_1 63
// TAD is 1/ADC_CLK
#define ADC_CLK (FREQOSC / (CLK_PHASES * (ADCLK_DIV_N_MINUS_1 + 1)))
#if (ADC_CLK > 8470000)
#error ADC_CLK too fast
#endif
// for automatic sequential sampling and conversion, the sampling time is 
// SAMC=ADSAMP_TIME_N TAD cycles
#define ADSAMP_TIME_N 31
//FIXME ADC_RATE is dependent on clock frequency
// At FREQOSC=40MHz, ADC_CLK=625KHz, at 32MHz:250KHz
// At FREQOSC=40MHz, ADC_RATE=13.9KHz, at 32MHz:5.56KHz
// At 32MHz, per channel rate is about 800Hz and lp2 3dB point is at 30Hz
// At 40MHz: 13.9KHz ADC rate and 8 channels seq. sampled, the per channel rate is
// about 1.7 KHz and lp2 3dB point is at 45Hz.
// Going from 16 to 40MHz pushes the lowpass filter cutoffs up by 2.5x
// and this may require changing the lp2 filter coefficients to maintain stability.
// With PID loop at 400Hz, should be able to deal with 100Hz bandwidth...
#define ADC_RATE (1.0 * ADC_CLK / (ADSAMP_TIME_N + 14))
#define N_CHANNELS_SCANNED 1

char sampcount = 1 ;


void udb_init_ADC( void )
{
	AD1CSSL = 0 ; // start with no channels selected
	AD1CSSH = 0 ; // start with no channels selected
	AD1PCFGL = 0b1111111111111111 ; // start with all digital, set the A/D
	AD1PCFGH = 0b1111111111111111 ; // start with all digital, set the A/D

//      include AN16 (labeled ANA0) in the scan
        /** ADC ref says "Any subset of the analog inputs from AN0 to AN31
         * (AN0-AN12 for devices without DMA) can be selected for conversion.
         * The selected inputs are converted in ascending order."
         * This might mean that we can't scan AN15 without using DMA, but
         * section 16.10.2 gives an example scanning 16 channels without DMA.
         */
    _CSS16 = 1;
    _PCFG16 = 0;

	AD1CON1bits.AD12B = 1 ;		// 12 bit A to D
	AD1CON1bits.FORM = 3 ;		// signed fractional
	AD1CON1bits.SSRC = 7 ;		// auto convert
	AD1CON1bits.ASAM = 1 ;		// auto samp
	AD1CON1bits.SIMSAM = 0 ;	// multiple channels in sequence

	AD1CON2bits.VCFG = 0 ;		// use supply as reference voltage
	AD1CON2bits.CSCNA = 1 ;		// scanA ch0

	AD1CON2bits.ALTS = 0 ;		// always A
//	AD1CON2bits.BUFM = 1 ;		// ping-pong buffers

//	AD1CON3bits.ADCS = 127 ;        // TAD = 8 microseconds (127+1)/16MHz *** NO, bits 7:6 have no effect
	AD1CON3bits.ADCS = ADCLK_DIV_N_MINUS_1;	// TAD = (63+1)/(FREQOSC/2) = 1.6usec at 40mips
	AD1CON3bits.SAMC = ADSAMP_TIME_N ;	// auto sample time = 31 TAD, approximately 50 microseconds

//	AD1CON1bits.ADDMABM = 1 ;	// DMA buffer written in conversion order

        // N_CHANNELS_SCANNED samples ??? this affects both DMA and non-DMA operation
	AD1CON2bits.SMPI = N_CHANNELS_SCANNED-1 ;
//	AD1CON4bits.DMABL = 1 ;		// double buffering

	_AD1IF = 0 ;                    // clear the AD interrupt
	_AD1IP = 5 ;			// priority 5
	_AD1IE = 1 ;			// enable the interrupt
	AD1CON1bits.ADON = 1 ;		// turn on the A to D

	return ;
}

// 3dB frequency about 45Hz at 2KHz sample rate, 38Hz at fs = 1.7 KHz
#define LPCB (unsigned int)(65536 / 12)

#define ADC1SAMPLE ((int)(ADC1BUF0))
void __attribute__((__interrupt__,__no_auto_psv__)) _ADC1Interrupt(void)
{
        static union int32_w2 pv, rv;

	indicate_loading_inter ;
	interrupt_save_set_corcon ;

	_AD1IF = 0 ; 	// clear the AD interrupt

#if (RECORD_FREE_STACK_SPACE == 1)
	unsigned int stack = WREG15 ;
	if ( stack > maxstack )
	{
		maxstack = stack ;
	}
#endif

#if (HILSIM != 1)
	switch ( sampcount ) {
                case analogInput1BUFF :
                        primaryV.input = ADC1SAMPLE;
                        break;

		default :
			break;
	}
#else
	(void)ADC1SAMPLE ;	// HILSIM: Pull the sample, but ignore it
#endif

	sampcount++ ;
	if ( sampcount > N_CHANNELS_SCANNED )
	{
		sampcount = 1 ;
                primaryV.value = lp2(primaryV.input, &pv, LPCB);
                // shouldn't need to filter this reference voltage
                udb_vref.value = lp2(udb_vref.input, &rv, LPCB);
	}

	interrupt_restore_corcon ;
	return ;
}

#endif
