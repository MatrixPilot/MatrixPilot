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


#include "libDCM_internal.h"


#if ( GPS_TYPE == GPS_MTEK )

//	Parse the DIYDrones MediaTek GPS messages, using the binary interface.
//	The parser uses a state machine implemented via a pointer to a function.
//	Binary values received from the GPS are directed to program variables via a table
//	of pointers to the variable locations.
//	Unions of structures are used to be able to access the variables as long, ints, or bytes.

void msg_start( unsigned char inchar ) ;
void msg_D0( unsigned char inchar ) ;
void msg_DD( unsigned char inchar ) ;
void msg_MSG_DATA( unsigned char inchar ) ;
void msg_CS1( unsigned char inchar ) ;

void (* msg_parse ) ( unsigned char inchar ) = &msg_start ;


const char gps_refresh_rate[]	= "$PMTK220,250*29\r\n" ;		// Set to 4Hz
const char gps_baud_rate[]		= "$PMTK251,19200*22\r\n" ;		// Set to 19200
const char gps_bin_mode[]		= "$PGCMD,16,0,0,0,0,0*6A\r\n" ;// turn on binary


unsigned char payloadlength ;
unsigned char un ; // dummy char
union longbbbb lat_gps_ , long_gps_ , alt_sl_gps_ , sog_gps_ , cog_gps_ , date_gps_ , time_gps_ ;
unsigned char svs_ ;
unsigned char fix_type_ ;
union intbb hdop_ ;
union intbb checksum ;

union longbbbb last_alt ;
unsigned char CK_A ;
unsigned char CK_B ;
int store_index = 0 ;


unsigned char * const msgDataParse[] = {
			&lat_gps_.__.B0  , &lat_gps_.__.B1  , &lat_gps_.__.B2  , &lat_gps_.__.B3 ,
			&long_gps_.__.B0 , &long_gps_.__.B1 , &long_gps_.__.B2 , &long_gps_.__.B3 ,
			&alt_sl_gps_.__.B0 , &alt_sl_gps_.__.B1 , &alt_sl_gps_.__.B2 , &alt_sl_gps_.__.B3 ,
			&sog_gps_.__.B0  , &sog_gps_.__.B1 , &sog_gps_.__.B2  , &sog_gps_.__.B3 ,
			&cog_gps_.__.B0  , &cog_gps_.__.B1 , &cog_gps_.__.B2  , &cog_gps_.__.B3 ,
			&svs_ ,
			&fix_type_ ,
			&date_gps_.__.B0  , &date_gps_.__.B1 , &date_gps_.__.B2  , &date_gps_.__.B3 ,
			&time_gps_.__.B0  , &time_gps_.__.B1 , &time_gps_.__.B2  , &time_gps_.__.B3 ,
			&hdop_._.B0 , &hdop_._.B0
} ;


boolean gps_nav_valid(void)
{
	return (fix_type_ == 0x03) ; // Fix type is 3D fix
}


void gps_startup_sequence(int gpscount)
{
	if (gpscount == 60)
		// Start at 9600 baud
		udb_gps_set_rate(9600) ;
	else if (gpscount == 50)
		// Set to 4Hz refresh rate
		gpsoutline((char*)gps_refresh_rate) ;
	else if (gpscount == 40)
		// Set up GPS for 19200 baud
		gpsoutline((char*)gps_baud_rate)  ;
	else if (gpscount == 30)
		// Switch UDB to 19200 baud
		udb_gps_set_rate(19200) ;
	else if (gpscount == 20)
		// Switch the GPS to use the custom DIY Drones binary protocol
		gpsoutline((char*)gps_bin_mode) ;
	
	return ;
}


//	The parsing routines follow. Each routine is named for the state in which the routine is applied.
//	States correspond to the portions of the binary messages.
//	For example, msg_B3 is the routine that is applied to the byte received after a B3 is received.
//	If an A0 is received, the state machine transitions to the A0 state.

void msg_start ( unsigned char gpschar )
{
	if ( gpschar == 0xD0 )
	{
		msg_parse = &msg_D0 ;
	}
	else
	{
		// error condition - stay in start state
	}
	
	return ;
}


void msg_D0 ( unsigned char gpschar )
{
	if ( gpschar == 0xDD )
	{
		store_index = 0 ;
		msg_parse = &msg_DD ;
	}
	else
	{
		msg_parse = &msg_start ;	// error condition
	}
	
	return ;
}


void msg_DD ( unsigned char gpschar )
{
	payloadlength = gpschar + 2 ; // take care of checksum
	CK_A = 0 ;
	CK_B = 0 ;
	msg_parse = &msg_MSG_DATA ;
	
	return ;
}


void msg_MSG_DATA ( unsigned char gpschar )
{
	if ( payloadlength > 0 )
	{
		// read data into msgDataParse array
		*msgDataParse[store_index++] = gpschar ;
		payloadlength-- ;
		CK_A += gpschar ;
		CK_B += CK_A ;
	}
	else
	{
		// done reading data.  read checksum
		checksum._.B1 = gpschar ;
		msg_parse = &msg_CS1  ;
	}
	
	return ;
}


void msg_CS1 ( unsigned char gpschar )
{
	checksum._.B0 = gpschar ;
	
	if ((checksum._.B1 == CK_A) && (checksum._.B0 == CK_B))
	{
		// correct checksum for DATA message
		udb_background_trigger() ;  // parsing is complete, schedule navigation
	}
	else
	{
		gps_data_age = GPS_DATA_MAX_AGE+1 ;	// if the checksum is wrong then the data from this packet is invalid.
											// setting this ensures the nav routine does not try to use this data.
	}
	msg_parse = &msg_start ;
	
	return ;
}


void commit_gps_data(void) 
{
	week_no.BB	= (2^16) - 1 ; // magic number to signify weird date/time format
	tow			= time_gps_ ;
	lat_gps		= lat_gps_ ;
	long_gps	= long_gps_ ;
	alt_sl_gps	= alt_sl_gps_ ;
	sog_gps.BB	= sog_gps_._.W0 ; 
	cog_gps.BB	= cog_gps_._.W0 ;
	climb_gps.BB= (alt_sl_gps_.WW - last_alt.WW) * 4 ;
	hdop		= (unsigned char)(hdop_.BB / 20) ;
	svs			= svs_ ;
	
	last_alt = alt_sl_gps_ ;
	
	return ;
}


#endif
