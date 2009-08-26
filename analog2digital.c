#include "p30f4011.h"
#include "defines.h"

//	Analog to digital processing.
//	Sampling and conversion is done automatically, so that all that needs to be done during 
//	interrupt processing is to read the data out of the buffer.
//	Raw samples are taken approximately 500 per second per channel.
//	A first order digital lowpass filter with a time constant of about 32 milliseconds 
//  is applied to improve signal to noise.

//	Variables.

struct ADchannel xaccel, yaccel , zaccel ; // x, y, and z accelerometer channels
struct ADchannel xrate , yrate, zrate ;  // x, y, and z gyro channels
#ifdef VREF
struct ADchannel vref ; // reference voltage
#endif

int firstsamp ; // used on startup to detect first A/D sample
int calibcount ; // number of PWM pulses before control is turned on

void init_ADC( void )
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

	IFS0bits.ADIF = 0 ; 	// clear the AD interrupt
	IPC2bits.ADIP = 5 ;     // priority 5
	IEC0bits.ADIE = 1 ;     // enable the interrupt
	ADCON1bits.ADON = 1 ;	// turn on the A to D
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _ADCInterrupt(void)
{
	indicate_loading_inter ;
	xrate.input =  xrateBUFF  ;
	yrate.input =  yrateBUFF  ;
	zrate.input =  zrateBUFF ;
#ifdef VREF
	vref.input  =   vrefBUFF ;
#endif
	xaccel.input =   xaccelBUFF ;
	yaccel.input =   yaccelBUFF ;
	zaccel.input =   zaccelBUFF ;
	if ( firstsamp )	// use the first sample to initialize the filters
	{
		firstsamp = 0 ;
		xaccel.value = xaccel.input ;
		yaccel.value = yaccel.input ;
		zaccel.value = zaccel.input ;
#ifdef VREF
		vref.value   = vref.input ;
#endif
		xrate.value = xrate.input ;
		yrate.value = yrate.input ;
		zrate.value = zrate.input ;
		IEC2bits.PWMIE = 1 ;     // enable the PWM interrupt
	}
	else
	{
		// perform just a little bit of filtering to improve signal to noise
 	    xaccel.value = xaccel.value + (( (xaccel.input>>1) - (xaccel.value>>1) )>> FILTERSHIFT ) ;
	    xrate.value = xrate.value + (( (xrate.input>>1) - (xrate.value>>1) )>> FILTERSHIFT ) ;
	    yaccel.value = yaccel.value + (( ( yaccel.input>>1) - (yaccel.value>>1) )>> FILTERSHIFT ) ;
	    yrate.value = yrate.value + (( (yrate.input>>1) - (yrate.value>>1) )>> FILTERSHIFT ) ;
	    zaccel.value = zaccel.value + (( (zaccel.input>>1) - (zaccel.value>>1) )>> FILTERSHIFT ) ;
	    zrate.value = zrate.value + ((( zrate.input>>1) - (zrate.value>>1) )>> FILTERSHIFT ) ;
#ifdef VREF
		vref.value = vref.value + (( (vref.input>>1) - (vref.value>>1) )>> FILTERSHIFT ) ;
#endif
	}
	IFS0bits.ADIF = 0 ; 	// clear the AD interrupt
	return ;
}
