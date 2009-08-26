#include "p30f4011.h"
#include "defines.h"

//	Parse the GPS messages, using the binary interface.
//	The parser uses a state machine implemented via a pointer to a function.
//	Binary values received from the GPS are directed to program variables via a table
//	of pointers to the variable locations.
//	Unions of structures are used to be able to access the variables as long, ints, or bytes.

union intbb payloadlength ;

void msg_A0( unsigned char inchar ) ;
void msg_A2( unsigned char inchar ) ;
void msg_PL1( unsigned char inchar ) ;
void msg_PL2( unsigned char inchar ) ;
void msg_MSG2( unsigned char inchar ) ;
void msg_MSG41( unsigned char inchar ) ;
void msg_MSGU( unsigned char inchar ) ;
void msg_B0( unsigned char inchar ) ;
void msg_B3( unsigned char inchar ) ;

const char bin_mode[]  = "$PSRF100,0,19200,8,1,0*39\r\n" ; // turn on binary mode, 19200 baud

const unsigned int mode_length = 9 ;
const unsigned char mode[] = {0x86,
							0x00,0x00,0x4B,0x00,
							0x08,
							0x01,
							0x00,
							0x00
							} ;

void (* msg_parse ) ( unsigned char inchar ) = &msg_B3 ;

unsigned char un ;

union longbbbb xpg_ , ypg_ , zpg_ ;
union intbb    xvg_ , yvg_ , zvg_ ;
unsigned char  mode1_ , mode2_ , svs_ ;
unsigned char svsmin = 24 ;
unsigned char svsmax = 0 ;

unsigned char * const msg2parse[] = {
			&xpg_.__.B3 , &xpg_.__.B2 ,
			&xpg_.__.B1 ,&xpg_.__.B0 ,
			&ypg_.__.B3 , &ypg_.__.B2 ,
			&ypg_.__.B1 ,&ypg_.__.B0 ,
			&zpg_.__.B3 , &zpg_.__.B2 ,
			&zpg_.__.B1 ,&zpg_.__.B0 ,
			&xvg_._.B1  , &xvg_._.B0 ,
			&yvg_._.B1  , &yvg_._.B0 ,
			&zvg_._.B1  , &zvg_._.B0 ,
			&mode1_ , 
			&un , 
			&mode2_ , 
			&un , &un , &un , &un , &un , &un ,
			&svs_ ,
			&un , &un , &un , &un , &un , &un , 
			&un , &un , &un , &un , &un , &un ,
			&un , &un } ;

union longbbbb lat_gps_ , long_gps_ , alt_sl_gps_ ;
union intbb    nav_valid_ , nav_type_ , sog_gps_ , cog_gps_ , climb_gps_ ;
unsigned char  hdop_ ;

unsigned char * const msg41parse[] = {
			&nav_valid_._.B1 , &nav_valid_._.B0 ,
			&nav_type_._.B1  , &nav_type_._.B0  ,
			&un , &un , &un , &un , &un , &un ,
			&un , &un , &un , &un , &un , &un ,
			&un , &un , &un , &un , &un , &un ,
			&lat_gps_.__.B3  , &lat_gps_.__.B2  , &lat_gps_.__.B1  , &lat_gps_.__.B0 ,
			&long_gps_.__.B3 , &long_gps_.__.B2 , &long_gps_.__.B1 , &long_gps_.__.B0 ,
			&un , &un , &un , &un ,
			&alt_sl_gps_.__.B3 , &alt_sl_gps_.__.B2 , &alt_sl_gps_.__.B1 , &alt_sl_gps_.__.B0 ,
			&un , 
			&sog_gps_._.B1  , &sog_gps_._.B0 ,
			&cog_gps_._.B1  , &cog_gps_._.B0 ,
			&un , &un ,
			&climb_gps_._.B1  , &climb_gps_._.B0 ,
			&un , &un , &un , &un , &un , &un , &un , &un , &un , &un ,
			&un , &un , &un , &un , &un , &un , &un , &un , &un , &un ,
			&un , &un , &un , &un , &un , &un , &un , &un , &un , &un ,
			&un , &un , &un , &un , &un , &un , &un , &un , &un , &un ,
			&un ,
			&hdop_ ,
			&un , &un , &un } ;

void gpsoutchar2 ( unsigned char outchar ) // output one character to the GPS
{
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

void set_bin(void)  // command GPS to select which messages are sent, using NMEA interface
{
	gpsoutline2(bin_mode)  ;
	return ;
}

void init_GPS2(void)
{
//	Initialize the USART that communicates with the GPS
	U2MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U2STA  = 0b0000010100010000 ;

	U2BRG =  51 ;  // start out at 4800 with 16 Mhz clock to communicate with GPS at factory setting

	U2MODE = 0b1010000000000000 ;
	U2STA  = 0b0000010100010000 ;

	IFS1bits.U2RXIF = 0 ; // clear the interrupt
	IPC6bits.U2RXIP = 4 ; // priority 4
	IEC1bits.U2RXIE = 1 ; // turn on the interrupt

	return ;
}

int bin_count = 0 ;
const char convert[] = "0123456789ABCDEF" ;
const char endchar = 0xB3 ;

void bin_out( char outchar )
//	Used for debugging purposes, converts to HEX and outputs to the debugging USART
//	Only the first 5 bytes following a B3 are displayed.
{
	if ( bin_count > 0 ) 
	{
		U1TXREG = convert[ ( (outchar>>4) & 0x0F ) ] ;
		U1TXREG = convert[ ( outchar & 0x0F ) ] ;
		U1TXREG = ' ' ;
		bin_count -- ;
	}
	if ( outchar == endchar )
	{
		bin_count = 5 ;
		U1TXREG = '\r' ;
		U1TXREG = '\n' ;
	}
	return ;
}

int store_index = 0 ;

//	The parsing routines follow. Each routine is named for the state in which the routine is applied.
//	States correspond to the portions of the binary messages.
//	For example, msg_B3 is the routine that is applied to the byte received after a B3 is received.
//	If an A0 is received, the state machine transitions to the A0 state.

void msg_B3 ( unsigned char gpschar )
{
	if ( gpschar == 0xA0 )
	{
		msg_parse = &msg_A0 ;
	}
	else
	{
				// error condition
	}
	return ;
}

void msg_A0 ( unsigned char gpschar )
{
	if ( gpschar == 0xA2 )
	{
		store_index = 0 ;
		msg_parse = &msg_A2 ;
	}
	else
	{
		msg_parse = &msg_B3 ;	// error condition
	}
	return ;
}

void msg_A2 ( unsigned char gpschar )
{
	payloadlength._.B1 = gpschar ;
	msg_parse = &msg_PL1 ;
	return ;
}

void msg_PL1 ( unsigned char gpschar )
{
	payloadlength._.B0 = gpschar ;
	payloadlength.BB++ ; // take care of checksum
	msg_parse = &msg_PL2 ;
	return ;
}

void msg_PL2 ( unsigned char gpschar )
{
	//	the only two messages that are being used by the gentleNAV are 2 and 41.
	switch ( gpschar ) {
		case 0x02 : {
			msg_parse = &msg_MSG2 ;
			}
			break ;
		case 0x29 :
				{
			msg_parse = &msg_MSG41 ;
			}
			break ;
		default : {
			msg_parse = &msg_MSGU ;
			}
			break ;
		}
		return ;
}

void msg_MSG2 ( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		*msg2parse[store_index++] = gpschar ;
		payloadlength.BB-- ;
	}
	else
	{
		if ( gpschar == 0xB0 )
		{
			msg_parse = &msg_B0 ;
		}
		else
		{
			msg_parse = &msg_B3 ;  // error condition
		}
	}
	return ;
}

void msg_MSG41 ( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		*msg41parse[store_index++] = gpschar ;
		payloadlength.BB-- ;
	}
	else
	{
		IFS0bits.T3IF = 1 ;  // parsing is complete, schedule navigation
		if ( gpschar == 0xB0 )
		{
			msg_parse = &msg_B0 ;
		}
		else
		{
			msg_parse = &msg_B3 ;  // error condition
		}
	}
	return ;
}


void msg_MSGU ( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		payloadlength.BB-- ;
	}
	else
	{
		if ( gpschar == 0xB0 )
		{
			msg_parse = &msg_B0 ;
		}
		else
		{
			msg_parse = &msg_B3 ;  // error condition
		}
	}
	return ;
}

void msg_B0 ( unsigned char gpschar )
{
	if ( gpschar == 0xB3 )
	{
		msg_parse = &msg_B3 ;
	}
	else
	{
		msg_parse = &msg_B3 ; // error condition
	}
	return ;
}

int frame_errors = 0 ;

void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt(void)
{
	unsigned char rxchar ;
	indicate_loading_inter ;
	if ( U2STAbits.FERR ) { init_GPS2(); frame_errors++ ;}
	if ( U2STAbits.OERR ) { init_GPS2(); }
	IFS1bits.U2RXIF = 0 ; // clear the interrupt
	while( U2STAbits.URXDA )
	{
		rxchar = U2RXREG ;
		(* msg_parse) ( rxchar ) ; // parse the input byte
	}
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

void __attribute__((interrupt,__no_auto_psv__)) _T3Interrupt(void) 
//  process T3 interrupt
{
	indicate_loading_inter ;
	nav_valid = nav_valid_ ;
	nav_type  = nav_type_ ;
	estYawDrift() ;
	//	if nav_valid is zero, there is valid GPS data that can be used for navigation.
	if ( nav_valid_.BB == 0 )
	{
		flags._.nav_capable = 1 ;
		lat_gps		= lat_gps_ ;
		long_gps	= long_gps_ ;
		alt_sl_gps	= alt_sl_gps_ ;
		nav_type	= nav_type_ ; 
		sog_gps		= sog_gps_ ; 
		cog_gps		= cog_gps_ ;
		climb_gps	= climb_gps_ ;
		hdop		= hdop_ ;
		xpg			= xpg_ ;
		ypg			= ypg_ ; 
		zpg			= zpg_ ;
		xvg			= xvg_ ; 
		yvg			= yvg_ ; 
		zvg			= zvg_ ;
		mode1		= mode1_ ; 
		mode2 		= mode2_ ; 
		svs			= svs_ ;
//
//
//		Perform the once per second navigation!!
//		navigate() ;
//		Ideally, navigate should take less than one second. For the gentleNAV, navigation takes only
//		a few milliseconds.
//		If you rewrite navigation to perform some rather ambitious calculations, perhaps using floating
//		point, matrix inversions, Kalman filters, etc., you will not cause a stack overflow if you
//		take more than 1 second, the interrupt handler will simply skip some of the navigation passes.

	}
	else
	{
		flags._.nav_capable = 0 ;
	}
	debug_output() ;
	IFS0bits.T3IF = 0 ;			// clear the interrupt
	return ;
}


void set_baud(void)
{
	gpsoutbin2( mode_length , mode ) ;
	return ;
}

union longbbbb lat_gps , long_gps , alt_sl_gps ;  	// latitude, longitude, altitude
union intbb    nav_valid , nav_type ;				// navigation valid, navigation type
union intbb    sog_gps , cog_gps , climb_gps ;		// speed over ground, course over ground, climb
unsigned char  hdop ;								// horizontal dilution of precision

union longbbbb xpg , ypg , zpg ;					// gps x, y, z position
union intbb    xvg , yvg , zvg ;					// gps x, y, z velocity 
unsigned char  mode1 , mode2 , svs ;				// gps mode1, mode2, and number of satellites

unsigned char  	lat_cir ;
int				cos_lat ;

union longbbbb lat_origin , long_origin , alt_origin ;
union longbbbb x_origin , y_origin , z_origin ;

union fbts_int flags ;

signed char actual_dir , desired_dir ;

