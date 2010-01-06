#include "p30f4011.h"
#include "defines.h"
#include "definesRmat.h"


union longbbbb lat_gps , long_gps , alt_sl_gps, tow ;  	// latitude, longitude, altitude
union intbb    sog_gps , cog_gps , climb_gps, week_no ;	// speed over ground, course over ground, climb
unsigned char  hdop ;									// horizontal dilution of precision

union longbbbb xpg , ypg , zpg ;						// gps x, y, z position
union intbb    xvg , yvg , zvg ;						// gps x, y, z velocity 
unsigned char  mode1 , mode2 , svs ;					// gps mode1, mode2, and number of satellites

unsigned char  	lat_cir ;
int				cos_lat ;

union longbbbb lat_origin , long_origin , alt_origin ;
union longbbbb x_origin , y_origin , z_origin ;

union fbts_int flags ;

signed char actual_dir , desired_dir ;

extern void (* msg_parse ) ( unsigned char inchar ) ;


void gpsoutchar2 ( unsigned char outchar ) // output one character to the GPS
{
	//bin_out(outchar);
	while ( ! U2STAbits.TRMT ) { }
	U2TXREG = outchar ;
	return ;
}

void gpsoutline2(char message[]) // output one NMEA line to the GPS
{
	int index ;
	char outchar ;
	index= 0 ;
	while  (  (outchar = message[index++])  ) 
	{
		gpsoutchar2( outchar ) ;
	}
}

void gpsoutbin2(int length , const unsigned char msg[] )  // output a binary message to the GPS
{
	int index = 0 ;
	while ( index < length )
	{
		gpsoutchar2( msg[index] ) ;
		index++;
	}
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	unsigned char rxchar ;
	
	if ( U2STAbits.FERR ) { init_GPS2(); }
	if ( U2STAbits.OERR ) { init_GPS2(); }
	IFS1bits.U2RXIF = 0 ; // clear the interrupt
	while( U2STAbits.URXDA )
	{
		rxchar = U2RXREG ;
		//bin_out ( rxchar ) ; // binary out to the debugging USART	
		(* msg_parse) ( rxchar ) ; // parse the input byte
	}
	
	interrupt_restore_extended_state ;
	return ;
}

void init_T3(void)	// set up the use of the T3 interrupt
{	
	//	The T3 interrupt is used to kick off the navigation processing after binary data
	//	is received from the GPS.
	
	IPC1bits.T3IP = 2 ;		// priority 2
	IFS0bits.T3IF = 0 ;		// clear the interrupt
	IEC0bits.T3IE = 1 ;		// enable the interrupt
}


void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt(void) 
//  process T3 interrupt
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	estYawDrift() ;
	
	if ( gps_nav_valid() )
	{
		gps_data_age = 0 ;
		
		commit_gps_data() ;
		
//		Perform the once per second navigation!!
		navigate() ;
		processwaypoints() ;
//		Ideally, navigate should take less than one second. For the gentleNAV, navigation takes only
//		a few milliseconds.
//		If you rewrite navigation to perform some rather ambitious calculations, perhaps using floating
//		point, matrix inversions, Kalman filters, etc., you will not cause a stack overflow if you
//		take more than 1 second, the interrupt handler will simply skip some of the navigation passes.

	}
	else
	{
		gps_data_age = GPS_DATA_MAX_AGE+1 ;
	}
	
	IFS0bits.T3IF = 0 ;			// clear the interrupt
	
	interrupt_restore_extended_state ;
	return ;
}

