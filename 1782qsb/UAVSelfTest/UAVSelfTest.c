
#include "p30f4011.h"

#ifdef ConfigRed
#include "ConfigRed.h"
#endif

#ifdef ConfigGreen
#include "ConfigGreen.h"
#endif

_FOSC( CSW_FSCM_OFF & HS ) ; 	// external high speed crystal
_FWDT( WDT_OFF ) ;				// no watchdog timer
_FBORPOR( 	PBOR_OFF & // brown out off
			MCLR_EN &  // enable MCLR
			RST_PWMPIN & // pwm pins as pwm
			PWMxH_ACT_HI & // PWMH is active high
			PWMxL_ACT_HI ) ; // PMWL is active high
_FGS( CODE_PROT_OFF ) ; // no protection
_FICD( 0xC003 ) ;		// normal use of debugging port

#define tmr1_period 0x8000 // time period for timer 1 interrupt

//#define ACCELSHIFT 3 // shift accelerometer output to the right
//#define GYROSHIFT 5  // shift gyro output to the right
#define FILTERSHIFT 5 // filter shift divide 

struct ADchannel {
 	int input ; // raw input
	int value ; // filtered a little bit
	int offset ;  // baseline at power up 
	 }  // variables for processing an AD channel
 ;

struct ADchannel xaccel, yaccel , zaccel ; // x, y, and z accelerometer channels
struct ADchannel xrate , yrate, zrate ;  // x, y, and z gyro channels
struct ADchannel vref ; // reference voltage

int risec1 , risec2 , risec7 , risec8 ; // rising edge clock capture for radio inputs
int pwc1 , pwc2 , pwc7 , pwc8 ; // pulse widths of radio inputs
int dutycycle ; // used to compute PWM duty cycle
int firstsamp ; // used on startup to detect first A/D sample
int calibcount ; // number of PWM pulses before control is turned on

const char gsv_off[] = "$PSRF103,03,00,00,01*27\r\n" ; // turn satellite messages off
const char vtg_on[]  = "$PSRF103,05,00,01,01*20\r\n" ; // turn velocity messages on

const unsigned int nmea_bin_on_length = 49 ;
const unsigned char nmea_bin_on[] = {0xA5,
							0x00,0x01,0x01,0x00,0x00,0x12,0xC0,0x08,0x01,0x00,0x00,0x00,
							0xFF,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
							0xFF,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,							0xFF,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} ;


int rxbufferindex ; // receiver buffer index

int pulsesat ( int pw ) // saturation logic to maintain pulse width within bounds
{
	if ( pw > 4000 ) pw = 4000 ;
	if ( pw < 2000 ) pw = 2000 ;
	return pw ;
}

void init_clock(void)	/* initialize timer 1 and LEDs */
	{
		
		TRISF = 0b1111111111101100 ;

		LATFbits.LATF0 = 0 ; 	// turn on LED1
		LATFbits.LATF1 = 0 ; 	// turn on LED2	

		TMR1 = 0 ; 				// initialize timer
		PR1 = tmr1_period ;		// set period register
		T1CONbits.TCKPS = 3 ;	// prescaler = 256 option
		T1CONbits.TCS = 0 ;		// use the crystal to drive the clock
		IPC0bits.T1IP = 1 ;		// low priority, the timer is not critical
		IFS0bits.T1IF = 0 ;		// clear the interrupt
		IEC0bits.T1IE = 1 ;		// enable the interrupt

		T1CONbits.TON = 1 ;		// turn on timer 1
	}

void set_gps2(void) ;

void __attribute__((interrupt,__no_auto_psv__)) _T1Interrupt(void) 
// process timer1 interrupt
	{

//	set_gps2() 	;	// try to set up the gps every once in a while

//	toggle three of the spare digital outputs

	__builtin_btg ( &LATE , 0 ) ;
	__builtin_btg ( &LATE , 2 ) ;
	__builtin_btg ( &LATE , 4 ) ;

	IFS0bits.T1IF = 0 ;			// clear the interrupt
	return ;
	}

//	The following two routines are simply to show how to employ unused timer interrupts to
//	schedule a lower priority process to start up from within another interrupt routine.
//	This technique might be usefull, for example, when the routine that processes data from
//	the GPS determines that enough data is available to perform a navigation computation.
//	In this simple example, the technique is used to schedule the toggling of an LED.
//  Even though timer 3 is not running, its interrupt can be used by setting it up without
//	turning on the timer. After that, whenever any higher priority routine wants to schedule a blink
//	of an LED, it simply sets the interrupt.


void init_T3(void)	// set up the use of the T3 interrupt
	{		
		IPC1bits.T3IP = 2 ;		// priority 2
		IFS0bits.T3IF = 0 ;		// clear the interrupt
		IEC0bits.T3IE = 1 ;		// enable the interrupt
	}

void __attribute__((interrupt,__no_auto_psv__)) _T3Interrupt(void) 
//  process T3 interrupt
	{
	__builtin_btg(  &LATF , 1 ) ; // toggle green LED
	IFS0bits.T3IF = 0 ;			// clear the interrupt
	return ;
	}


void init_ADC( void )
	{
	TRISB =  0b0000000111111111 ; // all inputs
	ADCON1 = 0b0010001111100100 ; // signed fractional , auto convert , seq, auto samp
//	ADCON2 = 0b0000010000011000 ; // supply ref, scana ch0, int every 7, 16word, usa A only
	ADCON2 = ADCON2CONFIG ;
//	ADCON3 = 0b0001111100111111 ; // slowest possible
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
	yrate.input = ( (int) yrateBUFF ) ;
	zrate.input = ( (int) zrateBUFF ) ;
	xrate.input = ( (int) xrateBUFF ) ;
#ifdef VREF
	vref.input  = ( (int) vrefBUFF ) ;
#else
	vref.input = 0 ;
#endif
	xaccel.input = ( (int) xaccelBUFF ) ;
	yaccel.input = ( (int) yaccelBUFF ) ;
	zaccel.input = ( (int) zaccelBUFF ) ;
	
	if ( firstsamp )	// use the first sample to initialize one of the filters
	{
		firstsamp = 0 ;
		xaccel.value = xaccel.input ;
		yaccel.value = yaccel.input ;
		zaccel.value = zaccel.input ;
		vref.value  = vref.input ;
		xrate.value = xrate.input ;
		yrate.value = yrate.input ;
		zrate.value = zrate.input ;

		IEC2bits.PWMIE = 1 ;     // enable the PWM interrupt
	}
	else
	{
		// perform just a little bit of filtering to improve the signal to noise ratio

 	    xaccel.value = xaccel.value + (( xaccel.input - xaccel.value )>> FILTERSHIFT ) ;
	    xrate.value = xrate.value + (( xrate.input - xrate.value )>> FILTERSHIFT ) ;

	    yaccel.value = yaccel.value + (( yaccel.input - yaccel.value )>> FILTERSHIFT ) ;
	    yrate.value = yrate.value + (( yrate.input - yrate.value )>> FILTERSHIFT ) ;

	    zaccel.value = zaccel.value + (( zaccel.input - zaccel.value )>> FILTERSHIFT ) ;
	    zrate.value = zrate.value + (( zrate.input - zrate.value )>> FILTERSHIFT ) ;

		vref.value = vref.value + (( vref.input - vref.value )>> FILTERSHIFT ) ;

	}

	IFS0bits.ADIF = 0 ; 	// clear the AD interrupt
	return ;
	}

int gpscount; 

void init_pwm( void )	// initialize the PWM
{
	PDC1 = PDC2 = PDC3 = pwc1 = pwc2 = pwc7 = pwc8 = 3000 ; // initialize PWM duty cycles

	firstsamp = 1;	// flag for the first sample
	calibcount = 100 ; // wait 100 PWM pulses before turning on the control (2.5 seconds)
	gpscount = 200 ;
	U2BRG = 12 ;

	TRISE = 0b1111111011000000 ;

	LATEbits.LATE0 = 1 ; // turn on spare outputs
	LATEbits.LATE2 = 1 ;
	LATEbits.LATE4 = 1 ;
	LATEbits.LATE8 = 1 ;

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

	//  note: at this point the PWM is running, so there are pulses going out,
	//	but the PWM interrupt is still off, so no interrupts are coming in yet to compute pulses.
	//  the PWM interrupt is turned on within the A/D interrupt processing

}

void __attribute__((__interrupt__,__no_auto_psv__)) _PWMInterrupt(void)
{
	switch (calibcount ) {
	// case 0 is when the control is up and running
	case 0: {
		// set servo outputs to radio pulses on channels 1, 2, and 3
		// plus accelerometer x, y and z, or gyro, depending on switches
		if ( PORTDbits.RD3 )
		{
			PDC1 = pulsesat ( pwc7 	+ ( xaccel.value >> ACCELSHIFT ) 
									- ( xaccel.offset >> ACCELSHIFT ) ) ;
		}
		else
		{
			PDC1 = pulsesat ( pwc7 	+ ( xrate.value >> GYROSHIFT ) 
									- ( xrate.offset >> GYROSHIFT )
									- ( vref.value >> GYROSHIFT ) 
									+ (vref.offset >> GYROSHIFT)  ) ;
		}

		if ( PORTDbits.RD2 )
		{
			
			PDC2 = pulsesat ( pwc8 	+ ( yaccel.value >> ACCELSHIFT ) 
									- ( yaccel.offset >> ACCELSHIFT ) ) ;
		}
		else
		{		
			PDC2 = pulsesat ( pwc8 	+ ( yrate.value >> GYROSHIFT ) 
									- ( yrate.offset >> GYROSHIFT )
									- ( vref.value >> GYROSHIFT ) 
									+ (vref.offset >> GYROSHIFT)  ) ;
		}

		if ( PORTFbits.RF6 )
		{
			PDC3 = pulsesat((( pwc2 + pwc1 ) >> 1 ) + ( zaccel.value >> ACCELSHIFT ) 
													- ( zaccel.offset >> ACCELSHIFT ) ) ;
		}
		else
		{
			PDC3 = pulsesat((( pwc2 + pwc1 ) >> 1 ) + ( zrate.value >> GYROSHIFT ) 
													- ( zrate.offset >> GYROSHIFT ) 
													- ( vref.value >> GYROSHIFT ) 
													+ (vref.offset >> GYROSHIFT) ) ;
		}
		break ;

	}
	case 1: {
		// almost ready to turn the control on, save the input offsets and initialize the filters
		xaccel.offset = xaccel.value ;
		xrate.offset = xrate.value ;

		yaccel.offset = yaccel.value ;
		yrate.offset = yrate.value ;

		zaccel.offset = zaccel.value ;
		zrate.offset = zrate.value ;

		vref.offset = vref.value ;

		LATFbits.LATF0 = 1 ; // turn off LED1
		LATFbits.LATF1 = 1 ; // turn off LED2

		LATEbits.LATE0 = 0 ; // turn off spare outputs
		LATEbits.LATE2 = 0 ;
		LATEbits.LATE4 = 0 ;
		LATEbits.LATE8 = 0 ;	
	
		break ;
	}
	default: {
	}
		// otherwise, there is not anything to do
		break ;
	}
	// count down the startup counter to 0
	if ( calibcount > 0 ) calibcount-- ;

	switch ( gpscount ) {
		case 15 : {
			set_gps2() ;
			break ;
		}
		case 10 : {
			U2BRG = 51 ;
			break ;
		}
		case 5 : {
			set_gps2() ;
			break ;
		}
		default : {
			break ;
		}
		break ;
	}
	if ( gpscount > 0 ) gpscount-- ;

	IFS2bits.PWMIF = 0 ; /* clear the interrupt */

	return ;
}

void init_capture(void)
{
	T2CON = 0b1000000000000000  ; // turn on timer 2 with no prescaler
	TRISD = 0b1111111111111111 ; // make the d port input, to enable IC1 and IC2
	IC1CON = IC2CON = IC7CON = IC8CON = 0b0010000010000001 ;

	IPC0bits.IC1IP = IPC1bits.IC2IP = IPC4bits.IC7IP = IPC4bits.IC8IP = 7 ; // priority 7
	IFS0bits.IC1IF = IFS0bits.IC2IF = IFS1bits.IC7IF = IFS1bits.IC8IF = 0 ; // clear the interrupt
	IEC0bits.IC1IE = IEC0bits.IC2IE = IEC1bits.IC7IE = IEC1bits.IC8IE = 1 ; // turn on the interrupt

	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _IC1Interrupt(void)
{
	if (PORTDbits.RD0)
	{
		 risec1 = IC1BUF ;
	}
	else
	{
		pwc1 = ((IC1BUF - risec1) >> 1 );
	}
	IFS0bits.IC1IF =  0 ; // clear the interrupt
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _IC2Interrupt(void)
{
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

void gpsoutchar2 ( char outchar ) // output one character to the GPS
{
	while ( ! U2STAbits.TRMT ) { }
	U2TXREG = outchar ;
	return ;
}

void gpsoutline2(char message[]) // output one line to the GPS
{
	int index ;
	char outchar ;
	index= 0 ;
	while  (  (outchar = message[index++])  ) 
	{
		gpsoutchar2( outchar ) ;
	}
}


void gpsoutbin2(int length , const unsigned char msg[] )
{
	int checksum = 0 ;
	int index = 0 ;
	gpsoutchar2( 0xA0 ) ;
	gpsoutchar2( 0xA2 ) ;
	gpsoutchar2 ( ( ( unsigned char)(( length >> 8 ) & 0x00FF )) ) ;
	gpsoutchar2 ( ( ( unsigned char)((  length     ) & 0x00FF )) ) ;
	while ( index < length )
	{
		gpsoutchar2( msg[index] ) ;
		checksum = checksum + msg[index++] ;
		checksum = checksum & 0x7FFF ;
	}
	gpsoutchar2 ( ( ( unsigned char)(( checksum >> 8 ) & 0x00FF )) ) ;
	gpsoutchar2 ( ( ( unsigned char)(( checksum      ) & 0x00FF )) ) ;
	gpsoutchar2 ( 0xB0 ) ;
	gpsoutchar2 ( 0xB3 ) ;
	return ;
}


void set_gps2(void)  // command GPS to select which messages are sent
{
	gpsoutbin2( nmea_bin_on_length , nmea_bin_on ) ;
	gpsoutline2(gsv_off) ;
	gpsoutline2(vtg_on) ;
	return ;
}

void init_GPS2(void)
{
	rxbufferindex = 0 ;

	U2MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U2STA  = 0b0000010100010000 ;

//	U2BRG =  51 ;

//	U2BRG = 12 ;

	U2MODE = 0b1010000000000000 ;
	U2STA  = 0b0000010100010000 ;

	IFS1bits.U2RXIF = 0 ; // clear the interrupt
	IPC6bits.U2RXIP = 4 ; // priority 4
	IEC1bits.U2RXIE = 1 ; // turn on the interrupt

	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt(void)
{
	char rxchar ;
	if ( U2STAbits.FERR ) {  
		if ( gpscount == 0 ) {
		__builtin_btg ( &LATF , 0 ) ; 
		}
		init_GPS2(); }
	if ( U2STAbits.OERR ) {  init_GPS2(); }
	rxchar = U2RXREG ;
	U1TXREG = rxchar ;
	if ( rxchar == '$' ) { rxbufferindex = 0 ; }
	if ( rxbufferindex == 4 )
		{
			if ( rxchar == 'T' ) { 	IFS0bits.T3IF = 1 ; }  // toggle the green LED via an interrupt.
			// This could have been done by simply toggling the LED, but is done this way here simply
			// to demonstrate how to use an unused timer interrupt to schedule a lower
			// priority process for execution after a higher priority process completes.
			// In this case, the LED will toggle AFTER this routine returns.
			// This approach could be used, for example, to schedule a navigation computation
			// once all of the needed information is available from the GPS.
		}
	rxbufferindex++  ;
	IFS1bits.U2RXIF = 0 ; // clear the interrupt
	return ;
}


void init_USART1(void)
{	

	U1MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U1STA  = 0b0000010100010000 ;

	U1BRG =  51 ;

	U1MODEbits.UARTEN = 1 ; // turn on uart
	U1MODEbits.ALTIO = 1 ; // alternate pins
	
	U1STAbits.UTXEN = 1 ; // turn on transmitter

	IFS0bits.U1RXIF = 0 ; // clear the interrupt
	IPC2bits.U1RXIP = 4 ; // priority 4
	IEC0bits.U1RXIE = 1 ; // turn on the interrupt

	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt(void)
{
	char rxchar ;
	rxchar = U1RXREG ;
	if ( U2STAbits.FERR ) {  init_USART1(); }
	else if ( U2STAbits.OERR ) {  init_USART1(); }
	else { __builtin_btg ( &LATE , 8 ) ; }

	IFS0bits.U1RXIF = 0 ; // clear the interrupt
	return ;
}

int main (void)
{
	init_capture() ; 	// initialize capture module
	init_pwm() ;		//  set up the PWM
	init_ADC() ;		//  initialze the AD
	init_GPS2() ;    	//  set up communications to the GPS
	init_USART1() ;  	//  debuging output
	init_clock() ; 		//  turn on the clock
	init_T3()  ;		//  enable the T3 internally triggered interrupt

	SRbits.IPL = 0 ;	// turn on all interrupt priorities

	while (1)			//  nothing else to do...entirely interrupt driven
	{
	
	}
	return 0 ;
}
	
