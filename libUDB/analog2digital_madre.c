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

#if (BOARD_TYPE == MADRE_BOARD)

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
struct ADchannel madre_vin ; // Source voltage of Madrescheda
struct ADchannel mbatt_v, mbatt_i ; // voltage and current of the motor's battery
struct ADchannel t_ext ;		// external temperature

#define K_VIN 		3300.* 4./4096.
#define K_VBAT 		3300.* 5.485/4096.
#define K_IBAT 		3300.* 10./4096.

// Number of locations for ADC buffer = 5 (AN1,2,3,8,10) x 1 = 4 words
// Align the buffer to 5 words or 10 bytes. This is needed for peripheral indirect mode
#define NUM_AD_CHAN 5
int  BufferA[NUM_AD_CHAN] __attribute__((space(dma),aligned(32))) ;
int  BufferB[NUM_AD_CHAN] __attribute__((space(dma),aligned(32))) ;


int vref_adj ;
int sample_count ;

#if (RECORD_FREE_STACK_SPACE == 1)
unsigned int maxstack = 0 ;
#endif


#define ALMOST_ENOUGH_SAMPLES 327 // there are 222 or 223 samples in a sum


void udb_init_gyros( void )
{
	// turn off auto zeroing 
//	_TRISC4 = _TRISB14 = 0 ;
//	_LATC4 = _LATB14 = 0 ;
	
	return ;
}

/*
void udb_init_accelerometer(void)
{
	rxAccelerometer();
	
	_TRISA6 = 0 ;  // GSELECT is an output
	_LATA6 = 1 ;   // 6 G setting
	
	//	set as inputs:
	_TRISB9 = 1 ;
	_TRISB10 = 1 ;
	_TRISB11 = 1 ;
	
	return ;
}
*/

void udb_init_ADC( void )
{
//	udb_init_gyros() ;
//	udb_init_accelerometer() ;
	sample_count = 0 ;
	
	AD1CON1bits.FORM   = 3 ;	// Data Output Format: Signed Fraction (Q15 format)
	AD1CON1bits.SSRC   = 7 ;	// Sample Clock Source: Auto-conversion
	AD1CON1bits.ASAM   = 1 ;	// ADC Sample Control: Sampling begins immediately after conversion
	AD1CON1bits.AD12B  = 1 ;	// 12-bit ADC operation
	
	AD1CON2bits.CSCNA = 1 ;		// Scan Input Selections for CH0+ during Sample A bit
	AD1CON2bits.CHPS  = 0 ;		// Converts CH0
	
	AD1CON3bits.ADRC = 0 ;		// ADC Clock is derived from Systems Clock
	AD1CON3bits.ADCS = 15 ;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*12 = 0.3us (3333.3Khz)
								// ADC Conversion Time for 12-bit Tc=14*Tad = 4.2us
	AD1CON3bits.SAMC = 1 ;		// No waiting between samples
	
	AD1CON2bits.VCFG = 0 ;		// use supply as reference voltage
	
	AD1CON1bits.ADDMABM = 1 ; 	// DMA buffers are built in sequential mode
	AD1CON2bits.SMPI    = (NUM_AD_CHAN-1) ;	// 4 ADC Channel are scanned
	AD1CON4bits.DMABL   = 0 ;	// Each buffer contains 1 word
	
	
	AD1CSSL = 0x0000 ;			// No AN selected for scan
	AD1PCFGL= 0xFFFF ;			// All AN pin set as digital
			
//  include the extra analog input pins
//	AD1CSSLbits.CSS1 = 1 ;		// Enable AN1	(Motor Battery current)	for channel scan
	AD1CSSLbits.CSS1 = 1 ;		// Enable AN1	(Vref)					for channel scan
	AD1CSSLbits.CSS2 = 1 ;		// Enable AN2	(MADRE battery voltage)	for channel scan
	AD1CSSLbits.CSS3 = 1 ;		// Enable AN3	(EXT Temperature)		for channel scan
	AD1CSSLbits.CSS8 = 1 ;		// Enable AN8	(IDG300 X axis)			for channel scan
//	AD1CSSLbits.CSS10 = 1 ;		// Enable AN10	(Motor Battery voltage)	for channel scan
	AD1CSSLbits.CSS10 = 1 ;		// Enable AN10	(IDG300 Y axis)			for channel scan
 	
	AD1PCFGLbits.PCFG1 = 0 ;	// AN1	as Analog Input
	AD1PCFGLbits.PCFG2 = 0 ;	// AN2	as Analog Input
	AD1PCFGLbits.PCFG3 = 0 ;	// AN3	as Analog Input
	AD1PCFGLbits.PCFG8 = 0 ;	// AN8	as Analog Input
	AD1PCFGLbits.PCFG10 = 0 ;	// AN10	as Analog Input

	
	_AD1IF = 0 ;				// Clear the A/D interrupt flag bit
	_AD1IP = 5 ;				// priority 5
	AD1CON1bits.ADON = 1 ;		// Turn on the A/D converter
	_AD1IE = 0 ;				// Do Not Enable A/D interrupt
	
	
//  DMA Setup
	DMA0CONbits.AMODE = 2 ;			// Configure DMA for Peripheral indirect mode
	DMA0CONbits.MODE  = 2 ;			// Configure DMA for Continuous Ping-Pong mode
	DMA0PAD=(int)&ADC1BUF0 ;
	DMA0CNT = NUM_AD_CHAN-1 ;					
	DMA0REQ = 13 ;					// Select ADC1 as DMA Request source
	
	DMA0STA = __builtin_dmaoffset(BufferA) ;
	DMA0STB = __builtin_dmaoffset(BufferB) ;
	
	IFS0bits.DMA0IF = 0 ;			//Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1 ;			//Set the DMA interrupt enable bit
	
	DMA0CONbits.CHEN = 1 ;			// Enable DMA
	
	return ;
}


unsigned char DmaBuffer = 0 ;

void __attribute__((__interrupt__,__no_auto_psv__)) _DMA0Interrupt(void)
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
	
#if (HILSIM != 1)
	int *CurBuffer = (DmaBuffer == 0) ? BufferA : BufferB ;
	
	madre_vin.input = CurBuffer[VbrdBUFF-1] ;
	t_ext.input		= CurBuffer[TextBUFF-1] ;
	udb_xrate.input = CurBuffer[xrateBUFF-1] ;
	udb_yrate.input = CurBuffer[yrateBUFF-1] ;
//	mbatt_i.input	= CurBuffer[mbatiBUFF-1] ;
//	mbatt_v.input	= CurBuffer[mbatvBUFF-1] ;

#endif
	
	DmaBuffer ^= 1 ;			// Switch buffers
	IFS0bits.DMA0IF = 0 ;		// Clear the DMA0 Interrupt Flag
	
	
	if ( udb_flags._.a2d_read == 1 ) // prepare for the next reading
	{
		udb_flags._.a2d_read = 0 ;
		udb_xrate.sum = udb_yrate.sum = 0 ;
//		madre_vin.sum = t_ext.sum = mbatt_i.sum = mbatt_v.sum = 0 ;
		madre_vin.sum = t_ext.sum = 0 ;
		
		#ifdef VREF
		udb_vref.sum = 0 ;
		#endif

		sample_count = 0 ;
	}
	
	//	perform the integration:
	udb_xrate.sum += udb_xrate.input ;
	udb_yrate.sum += udb_yrate.input ;
#ifdef VREF
	udb_vref.sum  += udb_vref.input ;
#endif
	madre_vin.sum 	+= madre_vin.input ;
	t_ext.sum		+= t_ext.input ;
//	mbatt_i.sum		+= mbatt_i.input ;
//	mbatt_v.sum		+= mbatt_v.input ;

	sample_count ++ ;
	
	//	When there is a chance that read_gyros() and read_accel() will execute soon,
	//  have the new average values ready.
	if ( sample_count > ALMOST_ENOUGH_SAMPLES )
	{	
		udb_xrate.value = __builtin_divsd( udb_xrate.sum , sample_count ) ;
		udb_yrate.value = __builtin_divsd( udb_yrate.sum , sample_count ) ;
#ifdef VREF
		udb_vref.value = __builtin_divsd( udb_vref.sum , sample_count ) ;
#endif
		madre_vin.value	= (unsigned int)((float)(__builtin_divsd( madre_vin.sum , sample_count ))*K_VIN) ;
		t_ext.value		= __builtin_divsd( t_ext.sum , sample_count ) ;
//		mbatt_i.value	= (unsigned int)((float)(__builtin_divsd( mbatt_i.sum , sample_count ))*K_IBAT) ;
//		mbatt_v.value	= (unsigned int)((float)(__builtin_divsd( mbatt_v.sum , sample_count ))*K_VBAT) ;
	}
	
	interrupt_restore_corcon ;
	return ;
}

#endif
