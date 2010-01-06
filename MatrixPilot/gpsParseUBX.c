
#include "p30f4011.h"
#include "defines.h"
#include "definesRmat.h"


#if ( GPS_TYPE == GPS_UBX )

//	Parse the GPS messages, using the binary interface.
//	The parser uses a state machine implemented via a pointer to a function.
//	Binary values received from the GPS are directed to program variables via a table
//	of pointers to the variable locations.
//	Unions of structures are used to be able to access the variables as long, ints, or bytes.


union intbb payloadlength ;
union intbb checksum;
unsigned int msg_class;
unsigned int msg_id;
unsigned int ack_class;
unsigned int ack_id;
unsigned int ack_type;
unsigned char CK_A;
unsigned char CK_B;

void msg_B3( unsigned char inchar ) ;
void msg_SYNC1( unsigned char inchar ) ;
void msg_SYNC2( unsigned char inchar ) ;
void msg_CLASS( unsigned char inchar ) ;
void msg_ID( unsigned char inchar ) ;
void msg_PL1( unsigned char inchar ) ;
void msg_POSLLH( unsigned char inchar ) ;
void msg_DOP( unsigned char inchar ) ;
void msg_SOL( unsigned char inchar ) ;
void msg_VELNED( unsigned char inchar ) ;
void msg_CS1( unsigned char inchar ) ;

void msg_MSGU( unsigned char inchar ) ;

void msg_ACK_CLASS( unsigned char inchar );
void msg_ACK_ID( unsigned char inchar );

void bin_out( char outchar );

#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI || SERIAL_OUTPUT_FORMAT == SERIAL_OSD_IF )
	const char bin_mode[]  = "$PUBX,41,1,0003,0003,9600,0*14\r\n" ; // turn on UBX + NMEA, 9600 baud
	// FOR TESTING ONLY
	// const char bin_mode[]  = "$PUBX,41,1,0003,0003,19200,0*21\r\n" ; // turn on UBX + NMEA, 19200 baud
#else
	const char bin_mode[]  = "$PUBX,41,1,0003,0001,9600,0*16\r\n" ; // turn on UBX only, 9600 baud
#endif


const char disable_GSV[] = "$PUBX,40,GSV,0,0,0,0,0,0*59\r\n" ; //Disable the $GPGSV NMEA message
const char disable_VTG[] = "$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n" ; //Disable the $GPVTG NMEA message
const char disable_GLL[] = "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n" ; //Disable the $GPGLL NMEA message
const char disable_GSA[] = "$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n" ; //Disable the $GPGSA NMEA message

const unsigned char set_4hz_rate[] =  { 0xB5, 0x62,  // Header
										0x06, 0x08, // ID
										0x06, 0x00, // Payload Length
										0xFA, 0x00, // measRate
										0x01, 0x00, // navRate
										0x01, 0x00, // timeRef
										0x10, 0x96}; // Checksum

const unsigned char enable_UBX_only[] ={0xB5, 0x62, 				// Header
										0x06, 0x00, 				// ID
										0x14, 0x00, 				// Payload length
										0x01, 						// Port ID
										0x00, 						// res0
										0x00, 0x00, 				// res1
										0xD0, 0x08, 0x00, 0x00, 	// mode
										0x80, 0x25, 0x00, 0x00, 	// baudrate
										0x03, 0x00, 				// inProtoMask
										0x01, 0x00, 				// outProtoMask
										0x00, 0x00, 				// Flags - reserved, set to 0
										0x00, 0x00,					// Pad - reserved, set to 0
										0x9C, 0x89					// checksum
										};

//enable UBX + NMEA @ 19200
/*const unsigned char enable_UBX_NMEA[] ={0xB5, 0x62, 				// Header
										0x06, 0x00, 				// ID
										0x14, 0x00, 				// Payload length
										0x01, 						// Port ID
										0x00, 						// res0
										0x00, 0x00, 				// res1
										0xD0, 0x08, 0x00, 0x00, 	// mode
										0x00, 0x4B, 0x00, 0x00, 	// baudrate
										0x03, 0x00, 				// inProtoMask
										0x03, 0x00, 				// outProtoMask
										0x00, 0x00, 				// Flags - reserved, set to 0
										0x00, 0x00,					// Pad - reserved, set to 0
										0x44, 0x37					// checksum
										};*/



#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI || SERIAL_OUTPUT_FORMAT == SERIAL_OSD_IF )
//enable UBX + NMEA @ 9600 - only needed if were using an OSD
const unsigned char enable_UBX_NMEA[] ={0xB5, 0x62, 				// Header
										0x06, 0x00, 				// ID
										0x14, 0x00, 				// Payload length
										0x01, 						// Port ID
										0x00, 						// res0
										0x00, 0x00, 				// res1
										0xD0, 0x08, 0x00, 0x00, 	// mode
										0x80, 0x25, 0x00, 0x00, 	// baudrate
										0x03, 0x00, 				// inProtoMask
										0x03, 0x00, 				// outProtoMask
										0x00, 0x00, 				// Flags - reserved, set to 0
										0x00, 0x00,					// Pad - reserved, set to 0
										0x9E, 0x95					// checksum
										};
#endif


const unsigned char enable_NAV_SOL[] = {0xB5, 0x62, 				// Header
										0x06, 0x01, 				// ID
										0x08, 0x00, 				// Payload length
										0x01, 						// NAV message class
										0x06, 						// SOL message ID
										0x00, 						// Rate on I2C
										0x01, 						// Rate on UART 1
										0x00, 						// Rate on UART 2
										0x00, 						// Rate on USB
										0x00, 						// Rate on SPI
										0x00, 						// Rate on ???
										0x17, 0xDA 					// Checksum
										};

const unsigned char enable_NAV_POSLLH[] = {0xB5, 0x62, 				// Header
										0x06, 0x01, 				// ID
										0x08, 0x00, 				// Payload length
										0x01, 						// NAV message class
										0x02, 						// POSLLH message ID
										0x00, 						// Rate on I2C
										0x01, 						// Rate on UART 1
										0x00, 						// Rate on UART 2
										0x00, 						// Rate on USB
										0x00, 						// Rate on SPI
										0x00, 						// Rate on ???
										0x13, 0xBE 					// Checksum
										};

const unsigned char enable_NAV_VELNED[] = {0xB5, 0x62, 				// Header
										0x06, 0x01, 				// ID
										0x08, 0x00, 				// Payload length
										0x01, 						// NAV message class
										0x12, 						// VELNED message ID
										0x00, 						// Rate on I2C
										0x01, 						// Rate on UART 1
										0x00, 						// Rate on UART 2
										0x00, 						// Rate on USB
										0x00, 						// Rate on SPI
										0x00, 						// Rate on ???
										0x23, 0x2E 					// Checksum
										};

const unsigned char enable_NAV_DOP[] = {0xB5, 0x62, 				// Header
										0x06, 0x01, 				// ID
										0x08, 0x00, 				// Payload length
										0x01, 						// NAV message class
										0x04, 						// DOP message ID
										0x00, 						// Rate on I2C
										0x04, 						// Rate on UART 1
										0x00, 						// Rate on UART 2
										0x00, 						// Rate on USB
										0x00, 						// Rate on SPI
										0x00, 						// Rate on ???
										0x18, 0xDB 					// Checksum
										};

const unsigned char disable_SBAS[] =   {0xB5, 0x62, 				// Header
										0x06, 0x16, 				// ID
										0x08, 0x00, 				// Payload length
										0x00, 						// Disable SBAS
										0x01, 						// 
										0x01, 						// 
										0x00, 						// 
										0x00, 						// 
										0x00, 						// 
										0x00, 						// 
										0x00, 						// 
										0x26, 0x97 					// Checksum
										};

const unsigned char config_NAV5[] =    {0xB5, 0x62, 				// Header
										0x06, 0x24, 				// ID
										0x24, 0x00, 				// Payload length
										0xFF, 0xFF,					//
										0x08, 						// Dynamic Model Number (or 0x07 for <2g)
										0x03, 						//
										0x00, 0x00,					//
										0x00, 0x00,					//
										0x10, 0x27,					//
										0x00, 0x00,					//
										0x05, 0x00,					//
										0xFA, 0x00,					//
										0xFA, 0x00,					//
										0x64, 0x00, 				//
										0x2C, 0x01, 				//
										0x00, 0x00,					//
										0x00, 0x00,					//
										0x00, 0x00,					//
										0x00, 0x00,					//
										0x00, 0x00,					//
										0x00, 0x00,					//
										0x00, 0x00,					//
										0x18, 0x20					// Checksum (or 0x17, 0xFE for <2g)
										};

const unsigned int  set_4hz_rate_length = 14 ;
const unsigned int  enable_NAV_SOL_length = 16 ;
const unsigned int  enable_NAV_POSLLH_length = 16 ;
const unsigned int  enable_NAV_VELNED_length = 16 ;
const unsigned int  enable_NAV_DOP_length = 16 ;
const unsigned int  enable_UBX_only_length = 28;
const unsigned int  disable_SBAS_length = 16;
const unsigned int  config_NAV5_length = 44;

void (* msg_parse ) ( unsigned char inchar ) = &msg_B3 ;

unsigned char un ;

union longbbbb 	xpg_ , ypg_ , zpg_ ;
union longbbbb  xvg_ , yvg_ , zvg_ ;
unsigned char  	mode1_ , mode2_ , svs_, nav_valid_ ;
union longbbbb 	lat_gps_ , long_gps_ , alt_sl_gps_ ;
union longbbbb  sog_gps_ , cog_gps_ , climb_gps_ , tow_ ;
union intbb   	hdop_ , week_no_ ;

unsigned char svsmin = 24 ;
unsigned char svsmax = 0 ;

unsigned char * const msg_SOL_parse[] = {
            &tow_.__.B0 , &tow_.__.B1 , &tow_.__.B2 , &tow_.__.B3,	//iTOW
			&un, &un, &un, &un, 									//fTOW
			&week_no_._.B0, &week_no_._.B1,							//week
			&nav_valid_ , 											//gpsFix
			&un, 													//flags
			&xpg_.__.B0 , &xpg_.__.B1 , &xpg_.__.B2 , &xpg_.__.B3 , //ecefX
			&ypg_.__.B0 , &ypg_.__.B1 , &ypg_.__.B2 , &ypg_.__.B3 , //ecefY
			&zpg_.__.B0 , &zpg_.__.B1 , &zpg_.__.B2 , &zpg_.__.B3 , //ecefZ
			&un, &un, &un, &un,										//pAcc
			&xvg_.__.B0 , &xvg_.__.B1 , &xvg_.__.B2 , &xvg_.__.B3 , //ecefVX
			&yvg_.__.B0 , &yvg_.__.B1 , &yvg_.__.B2 , &yvg_.__.B3 , //ecefVY
			&zvg_.__.B0 , &zvg_.__.B1 , &zvg_.__.B2 , &zvg_.__.B3 , //ecefVZ
			&un, &un, &un, &un, 									//sACC
			&un, &un, 												//pDOP
			&un, 													//res1
			&svs_ ,													//numSV
			&un, &un, &un, &un, 									//res2
};

unsigned char * const msg_DOP_parse[] = {
			&un, &un, &un, &un, 		//iTOW
			&un, &un, 					//gDOP
			&un, &un, 					//pDOP
			&un, &un, 					//tDOP
			&un, &un, 					//vDOP
			&hdop_._.B0, &hdop_._.B1,	//hDOP
			&un, &un, 					//nDOP
			&un, &un, 					//eDOP
};

unsigned char * const msg_POSLLH_parse[] = {
			&un, &un, &un, &un, 																//iTOW
			&long_gps_.__.B0 , &long_gps_.__.B1 , &long_gps_.__.B2 , &long_gps_.__.B3 , 		//lon
			&lat_gps_.__.B0  , &lat_gps_.__.B1  , &lat_gps_.__.B2  , &lat_gps_.__.B3 , 			//lat
			&un, &un, &un, &un, 																//height
			&alt_sl_gps_.__.B0 , &alt_sl_gps_.__.B1 , &alt_sl_gps_.__.B2 , &alt_sl_gps_.__.B3 ,	//hMSL
			&un, &un, &un, &un, 																//hAcc
			&un, &un, &un, &un, 																//vAcc
};

unsigned char * const msg_VELNED_parse[] = {
			&un, &un, &un, &un, 															//iTOW
			&un, &un, &un, &un, 															//velN
			&un, &un, &un, &un, 															//velE
			&climb_gps_.__.B0, &climb_gps_.__.B1, &climb_gps_.__.B2, &climb_gps_.__.B3, 	//velD
			&un, &un, &un, &un, 															//speed
			&sog_gps_.__.B0, &sog_gps_.__.B1, &sog_gps_.__.B2, &sog_gps_.__.B3, 			//gSpeed
			&cog_gps_.__.B0, &cog_gps_.__.B1, &cog_gps_.__.B2, &cog_gps_.__.B3, 			//heading
			&un, &un, &un, &un, 															//sAcc
			&un, &un, &un, &un,																//cAcc
};


void gps_startup_sequence(int gpscount)
{
#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI || SERIAL_OUTPUT_FORMAT == SERIAL_OSD_IF )
	if (gpscount == 190)
		gpsoutline2( (char*)disable_GSV );
	else if (gpscount == 180)
		gpsoutline2( (char*)disable_GSA );
	else if (gpscount == 170)
		gpsoutline2( (char*)disable_GLL );
#else
	if (gpscount == 190);
		// do nothing
#endif

#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI )
	else if (gpscount == 160)
		gpsoutline2( (char*)disable_VTG );
#endif
	
	else if (gpscount == 150)
		//set the UBX to use binary mode
		gpsoutline2( (char*)bin_mode );
	else if (gpscount == 140)
		gpsoutbin2( set_4hz_rate_length, set_4hz_rate );
	else if (gpscount == 130)
		// command GPS to select which messages are sent, using UBX interface
		gpsoutbin2( enable_NAV_SOL_length, enable_NAV_SOL );
	else if (gpscount == 120)
		gpsoutbin2( enable_NAV_POSLLH_length, enable_NAV_POSLLH );
	else if (gpscount == 110)
		gpsoutbin2( enable_NAV_VELNED_length, enable_NAV_VELNED );
	else if (gpscount == 100)
		gpsoutbin2( enable_NAV_DOP_length, enable_NAV_DOP );
	
#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI || SERIAL_OUTPUT_FORMAT == SERIAL_OSD_IF )
	else if (gpscount == 90)
		gpsoutbin2( enable_UBX_only_length, enable_UBX_NMEA );
#else
	else if (gpscount == 90)
		gpsoutbin2( enable_UBX_only_length, enable_UBX_only );
#endif
	
	else if (gpscount == 80)
		gpsoutbin2( disable_SBAS_length, disable_SBAS );
	else if (gpscount == 70)
		gpsoutbin2( config_NAV5_length, config_NAV5 );
	
	return ;
}


void init_GPS2(void)
{
//	Initialize the USART that communicates with the GPS
	U2MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U2STA  = 0b0000010100010000 ;

	U2BRG =  25 ;

	U2MODE = 0b1010000000000000 ;
	U2STA  = 0b0000010100010000 ;

	IFS1bits.U2RXIF = 0 ; // clear the interrupt
	IPC6bits.U2RXIP = 4 ; // priority 4
	IEC1bits.U2RXIE = 1 ; // turn on the interrupt
	
	return ;
}

boolean gps_nav_valid(void)
{
	return (nav_valid_ == 3);
}


int bin_count = 0 ;
const char convert[] = "0123456789ABCDEF" ;
const char endchar = 0xB5 ;

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

#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI || SERIAL_OUTPUT_FORMAT == SERIAL_OSD_IF )

int countdown = 0 ; //used by nmea_passthru to count how many more bytes are passed through

void nmea_passthru ( unsigned char gpschar)
{
	U1TXREG = gpschar;
	countdown --;
/* removed in favor of the line ending mechanism, see below. While this is compliant with
   (published) standards, the issue appears to center around the end of line.

	if ( gpschar == '*' )
	{ // * indicates the start of the checksum, 2 characters remain in the message
		countdown = 2;
	}
*/
	if ( gpschar == 0x0A )
    { // end of line appears to always be 0x0D, 0x0A (\r\n)
		msg_parse = &msg_B3; // back to the inital state
	}
	else if (countdown == 0)
	{
		msg_parse = &msg_B3; // back to the inital state
	}
	return;
}
#endif

void msg_B3 ( unsigned char gpschar )
{
	
	if ( gpschar == 0xB5 )
	{
		//bin_out(0x01);
		msg_parse = &msg_SYNC1 ;
	}

#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI || SERIAL_OUTPUT_FORMAT == SERIAL_OSD_IF )
	else if ( gpschar == '$')
	{
		countdown = 1024; // this limits the number of characters we will passthrough. //TODO: smaller number?
		msg_parse = &nmea_passthru;
		nmea_passthru ( gpschar );
	}
#endif

	else
	{
				// error condition
	}
	return ;
}

void msg_SYNC1 ( unsigned char gpschar )
{
	
	if ( gpschar == 0x62 )
	{
		//bin_out(0x02);
		store_index = 0 ;
		msg_parse = &msg_SYNC2 ;
	}
	else
	{
		msg_parse = &msg_B3 ;	// error condition
	}
	return ;
}

void msg_SYNC2 ( unsigned char gpschar )
{
	//bin_out(0x03);
	msg_class = gpschar ;
	CK_A = 0;
	CK_B = 0;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_CLASS ;
	return ;
}

void msg_CLASS ( unsigned char gpschar )
{
	//bin_out(0x04);
	msg_id = gpschar ;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_ID ;
	return ;
}

void msg_ID ( unsigned char gpschar )
{
	//bin_out(0x05);
	payloadlength._.B0 = gpschar ;	//UBX stored payload length in little endian order
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_PL1 ;
	return ;
}

void msg_PL1 ( unsigned char gpschar )
{
	//bin_out(0x06);
	payloadlength._.B1 = gpschar ;	//UBX stored payload length in little endian order
	CK_A += gpschar;
	CK_B += CK_A;
	switch ( msg_class ) {
	case 0x01 : {
		switch ( msg_id ) {
	case 0x02 : { // NAV-POSLLH message
		msg_parse = &msg_POSLLH ;
				}
				break ;
	case 0x04 : { // NAV-DOP message
		msg_parse = &msg_DOP ;
				}
				break ;
	case 0x06 : { // NAV-SOL message
		msg_parse = &msg_SOL ;
				}
				break ;
	case 0x12 : {	// NAV-VELNED message
		msg_parse = &msg_VELNED ;
				}
				break ;
	default : { 	// some other NAV class message
		msg_parse = &msg_MSGU ;
			  }
			  break ;
		}
		break ;
	case 0x05 : {
		switch ( msg_id ) {
	case 0x00 : { // NACK message
		ack_type = 0;
		msg_parse = &msg_ACK_CLASS;
				}
				break;
	case 0x01 : { // ACK message
		ack_type = 1;
		msg_parse = &msg_ACK_CLASS;
				}
				break;	
	default : { // There are no other messages in this class, so this is an error
		msg_parse = &msg_B3	;
			  }
			  break;
		}
		break;
				}
	default : { 	// a non NAV class message
		msg_parse = &msg_MSGU ;
			  }
			  break ;
				}
	}
	return ;
}

void msg_POSLLH( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		*msg_POSLLH_parse[store_index++] = gpschar ;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB-- ;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x08);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1 ;
	}
	return ;
}

void msg_DOP( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		*msg_DOP_parse[store_index++] = gpschar ;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB-- ;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x09);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1 ;
	}
	return ;
}

void msg_SOL( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		*msg_SOL_parse[store_index++] = gpschar ;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB-- ;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x0A);
		checksum._.B1 = gpschar;
		IFS0bits.T3IF = 1 ;  // parsing is complete, schedule navigation
		msg_parse = &msg_CS1 ;
	}
	return ;
}

void msg_VELNED( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		*msg_VELNED_parse[store_index++] = gpschar ;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB-- ;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x0B);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1 ;
	}
	return ;
}

void msg_ACK_CLASS ( unsigned char gpschar )
{
	//bin_out(0xAA);
	ack_class = gpschar ;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_ACK_ID ;
	return ;
}

void msg_ACK_ID ( unsigned char gpschar )
{
	//bin_out(0xBB);
	ack_id = gpschar ;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_CS1 ;
	return ;
}

void msg_MSGU ( unsigned char gpschar )
{
	if ( payloadlength.BB > 0 )
	{
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB-- ;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x08);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1 ;
	}
	return ;
}

void msg_CS1 ( unsigned char gpschar )
{
	checksum._.B0 = gpschar;
	if((checksum._.B1 == CK_A) && (checksum._.B0 == CK_B))
	{
		if(msg_id == 0x02)
		{
			//correct checksum, do nothing
		}
	}	
	else
	{
		gps_data_age = GPS_DATA_MAX_AGE+1 ;	// if the checksum is wrong then the data from this packet is invalid. 
											// setting this ensures the nav routine does not try to use this data.
	}
	msg_parse = &msg_B3 ;
	return ;
}

int frame_errors = 0 ;

void commit_gps_data(void) 
{
	//bin_out(0xFF);
	week_no			= week_no_ ;
	tow				= tow_ ;
	lat_gps			= lat_gps_ ;
	long_gps		= long_gps_ ;
	alt_sl_gps.WW	= alt_sl_gps_.WW / 10 ;				// SIRF provides altMSL in cm, UBX provides it in mm
	sog_gps.BB 		= sog_gps_._.W0 ; 					// SIRF uses 2 byte SOG, UBX provides 4 bytes
	cog_gps.BB 		= (int)(cog_gps_.WW / 1000) ;		// SIRF uses 2 byte COG, 10^-2 deg, UBX provides 4 bytes, 10^-5 deg
	climb_gps.BB 	= - climb_gps_._.W0 ;				// SIRF uses 2 byte climb rate, UBX provides 4 bytes
	hdop			= (unsigned char)(hdop_.BB * 20) ; 	// SIRF scales HDOP by 5, UBX by 10^-2
	// SIRF provides position in m, UBX provides cm
	xpg.WW			= xpg_.WW / 100 ;
	ypg.WW			= ypg_.WW / 100 ;
	zpg.WW			= zpg_.WW / 100 ;
	// SIRF provides 2 byte velocity in m scaled by 8,
	// UBX provides 4 bytes in cm
	xvg.BB			= (int)(xvg_.WW / 100 * 8) ;
	yvg.BB			= (int)(yvg_.WW / 100 * 8) ;
	zvg.BB			= (int)(zvg_.WW / 100 * 8) ;
	mode1			= mode1_ ;
	mode2 			= mode2_ ;
	svs				= svs_ ;
	
	return ;
}

#endif
