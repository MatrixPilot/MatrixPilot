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


#if ( GPS_TYPE == GPS_NMEA )

//	Parse the GPS messages, using the NMEA interface.
//	The parser uses a state machine implemented via a pointer to a function.
//	Binary values received from the GPS are directed to program variables via a table
//	of pointers to the variable locations.
//	Unions of structures are used to be able to access the variables as long, ints, or bytes.


void dollar ( unsigned char inchar ) ;

void (* msg_parse ) ( unsigned char inchar ) = &dollar ;

int	skip = 0 ;

int digit ;
long degrees ;
long minutes ;

//unsigned char un ;
union longbbbb lat_gps_ , long_gps_ , alt_sl_gps_ , tow_, last_alt ;
union intbb    nav_valid_ , nav_type_ , sog_gps_ , cog_gps_ , climb_gps_ , week_no_ ;
unsigned char data_valid_ , NS_ , EW_, svs_, day_of_week, hdop_ ;

void gps_G(unsigned char inchar) ;
void gps_P(unsigned char inchar) ;
void gps_R(unsigned char inchar) ;
void gps_M(unsigned char inchar) ;
void gps_C(unsigned char inchar) ;
void gps_comma1(unsigned char inchar) ;
void gps_data_valid(unsigned char inchar) ;
void gps_comma2(unsigned char inchar) ;
void gps_skip1(unsigned char inchar) ;
void gps_status(unsigned char inchar) ;
void gps_lat(unsigned char inchar) ;
void gps_long(unsigned char inchar) ;
void gps_NS(unsigned char inchar) ;
void gps_EW(unsigned char inchar) ;
void gps_sog(unsigned char inchar) ;
void gps_cog(unsigned char inchar) ;
void gps_comma3(unsigned char inchar) ;
void gps_comma4(unsigned char inchar) ;
void gps_comma5(unsigned char inchar) ;
void gps_comma6(unsigned char inchar) ;
void gps_comma7(unsigned char inchar) ;
void gps_comma8(unsigned char inchar) ;
void gps_comma9(unsigned char inchar) ;
void gps_comma10(unsigned char inchar) ;

const char disable_GGA[] = "$PSRF103,00,00,00,01*24\r\n" ; 
const char disable_GLL[] = "$PSRF103,01,00,00,01*25\r\n" ; 
const char disable_GSA[] = "$PSRF103,02,00,00,01*26\r\n" ;
const char disable_GSV[] = "$PSRF103,03,00,00,01*27\r\n" ; 
const char disable_VTG[] = "$PSRF103,05,00,00,01*21\r\n" ; 
const char disable_ZDA[] = "$PSRF103,08,00,00,01*2C\r\n" ; 
const char enable_RMC[]  = "$PSRF103,04,00,01,01*21\r\n" ; 
const char set_FIX_1Hz[]  = "$PMTK220,1000*1F\r\n" ; 
const char set_FIX_2Hz[]  = "$PMTK220,500*2B\r\n" ; 
const char set_FIX_3Hz[]  = "$PMTK220,333*2D\r\n" ; 
const char set_FIX_4Hz[]  = "$PMTK220,250*29\r\n" ; 
const char set_FIX_5Hz[]  = "$PMTK220,200*2C\r\n" ; 
const char set_RMC[]  = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" ; 
const char set_RMC_GGA[]  = "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n" ; 

//	if nav_valid is zero, there is valid GPS data that can be used for navigation.
boolean gps_nav_valid(void)
{
	return (data_valid_ == 'A') ;
}

void gps_startup_sequence(int gpscount)
{
	if (gpscount == 980)
		udb_gps_set_rate(57600);	
//	else if( gpscount == 200 )
//		gpsoutline( (char*)disable_GGA );
//	else if( gpscount == 190 )
//		gpsoutline( (char*)disable_GLL );
//	else if( gpscount == 180 )
//		gpsoutline( (char*)disable_GSA );
//	else if( gpscount == 170 )
//		gpsoutline( (char*)disable_GSV );
//	else if( gpscount == 160 )
//		gpsoutline( (char*)disable_VTG );
	else if( gpscount == 150 )
		gpsoutline( (char*)set_FIX_1Hz );
	else if( gpscount == 140 )
		gpsoutline( (char*)set_RMC );

//	if (gpscount == 10)
		// Switch to 19200 baud
//		udb_gps_set_rate(19200);
	
	return ;
}


void dollar(unsigned char inchar) 
{
	if ( inchar == '$' ) msg_parse = &gps_G ;			// Wait for the $
	return ;
}

void gps_G( unsigned char inchar)
{
	if ( inchar == 'G' )		// "$G" String started
	{
		msg_parse = &gps_P ;	// Get ready to parse the next char
	}
	else
	{
		msg_parse = &dollar ;	// No NMEA string or string error, abort the current string and wait for $
	}
	return ;
}

void gps_P( unsigned char inchar)
{
	if ( inchar == 'P' )		// "$GP", go on
	{
		msg_parse = &gps_R ;	
	}
	else
	{
		msg_parse = &dollar ;	// Abort current string
	}
	return ;
}

void gps_R( unsigned char inchar)
{
	if ( inchar == 'R' )		// "$GPR" RMC string detected
	{
		msg_parse = &gps_M ;	// Go on with RMC
	LED_RED = LED_ON;
	}
	else
	{
		msg_parse = &dollar ;	// Abort current string
	}
	return ;
}


void gps_M( unsigned char inchar)
{
	if ( inchar == 'M' )		// "$GPRM"
	{
		msg_parse = &gps_C ;
	}
	else
	{
		msg_parse = &dollar ;
	}
	return ;
}

void gps_C( unsigned char inchar)
{
	if ( inchar == 'C' )		// "$GPRMC"
	{
		msg_parse = &gps_comma1 ;	// A comma ',' is expected now
	}
	else
	{
		msg_parse = &dollar ;
	}
	return ;
}

void gps_comma1( unsigned char inchar )
{
	if ( inchar == ',' )		// "$GPRMC,"
	{
		skip = 10 ;				// Skip the time HHMMSS.SSS = 10 char max
		msg_parse = &gps_skip1 ;
	}
	else
	{
		msg_parse = &dollar ;
	}
	return ;
}

void gps_skip1( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = &gps_status ;
	}
	else if ( (skip--) == 0 ) msg_parse = &gps_status ;	// Force exit after 10 character
	return ;
}

void gps_status( unsigned char inchar )
{
	if ( inchar == ',' )		// GPS status information not available
	{
		msg_parse = &gps_lat ;
		digit = 0 ;
		minutes = 0 ;
		degrees = 0 ;
	}
	else
	{
		data_valid_ = inchar ;
		msg_parse = &gps_comma2 ;
	}
	return ;
}

void gps_comma2 ( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = &gps_lat ;
		digit = 0 ;
		minutes = 0 ;
		degrees = 0 ;
	}
	else
	{
		msg_parse = &dollar ;
	}
	return ;
}

void gps_lat ( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = &dollar ;
	}
	else
	{
		digit++ ;
		switch ( digit )
		{
			case 1 : case 2 :
				degrees = 10*degrees + ( inchar - '0' ) ;
			break ;
			case 3 : case 4 : case 6 : case 7 : case 8 :
				minutes = 10*minutes + ( inchar - '0' ) ;
			break ;
			case 5 :	// DDMM.MMMM Skip the point in the middle
			break ;
			case 9 :
				minutes = 10*minutes + ( inchar - '0' ) ;
				msg_parse = &gps_comma3 ;
				lat_gps_.WW = (((long)10000000)*degrees+(50*minutes)/3) ;
			break ;
			default :
			break ;
		}	
	}
	return ;
}

void gps_comma3( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = & gps_NS ;
	}
	else
	{
		msg_parse = & dollar ;
	}
}

void gps_NS( unsigned char inchar ) 
{
	if ( inchar == ',' )			// NS information not available
	{
		NS_ = ' ' ;
		msg_parse = & gps_long ;
		digit = 0 ;
		minutes = 0 ;
		degrees = 0 ;
	}
	else
	{
		NS_ = inchar ;
		msg_parse = & gps_comma4 ;
	}
	if ( inchar == 'S' )
	{
		lat_gps_.WW = - lat_gps_.WW ;
	}
	return ;
}

void gps_comma4( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = & gps_long ;
		digit = 0 ;
		minutes = 0 ;
		degrees = 0 ;
	}
	else
	{
		msg_parse = & dollar ;
	}
}

void gps_long ( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = &dollar ;
	}
	else
	{
		digit++ ;
		switch ( digit )	// DDDMM.MMMM
		{
			case 1 : case 2 : case 3 :
				degrees = 10*degrees + ( inchar - '0' ) ;
			break ;
			case 4 : case 5 : case 7 : case 8 : case 9 : 
				minutes = 10*minutes + ( inchar - '0' ) ;
			break ;
			case 6 :
			break ;
			case 10 :
				minutes = 10*minutes + ( inchar - '0' ) ;
				msg_parse = &gps_comma5 ;
				long_gps_.WW = (((long)10000000)*degrees+(50*minutes)/3) ; //Sure that minutes should be multiplied by 10?
			break ;
			default :
			break ;
		}	
	}
	return ;
}

void gps_comma5( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = & gps_EW ;
	}
	else
	{
		msg_parse = & dollar ;
	}
	return ;
}

void gps_EW( unsigned char inchar ) 
{
	if ( inchar == ',' )
	{
		EW_ = ' ' ;
		msg_parse = & gps_comma6 ;
	}
	else
	{
		EW_ = inchar ;
		msg_parse = & gps_comma6 ;
	}
	if ( inchar == 'W' )
	{
		long_gps_.WW = - long_gps_.WW ;
	}
	return ;
}

void gps_comma6( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = & gps_sog ;
		sog_gps_.BB = 0 ;
	}
	else
	{
		msg_parse = & dollar ;
	}
	return ;
}

void gps_sog( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		sog_gps_.BB = sog_gps_.BB >> 1 ;	// knots*100/2?
		msg_parse = & gps_cog ;
		cog_gps_.BB = 0 ;
	}
	else if ( inchar != '.' )
	{
		sog_gps_.BB = sog_gps_.BB*10 + ( inchar - '0' ) ;
	}
	return ;
}

void gps_cog( unsigned char inchar )
{
	if ( inchar == ',' )
	{
		msg_parse = & dollar ;
	LED_RED = LED_OFF;
		udb_background_trigger() ;  // parsing is complete, schedule navigation
	}
	else if ( inchar != '.' )
	{
		cog_gps_.BB = cog_gps_.BB*10 + ( inchar - '0' ) ;	// Course*100
	}
	return ;
}

const unsigned char days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;
#define MS_PER_DAY	86400000 // = (24 * 60 * 60 * 1000)

void calculate_week_num(void)
{
	// Convert date from DDMMYY to week_num and day_of_week
	long date = 0 ;
	unsigned char year = date % 100 ;
	date /= 100 ;
	unsigned char month = date % 100 ;
	date /= 100 ;
	int day = date % 100 ;
	
	// Wait until we have real date data
	if (day == 0 || month == 0) return ;
	
	// Begin counting at May 1, 2011 since this 1st was a Sunday
	unsigned char m = 5 ;	// May
	unsigned char y = 11 ;	// 2011
	int c = 0 ;				// loop counter
	
	while (m < month || y < year) {
		day += days_in_month[m-1] ;			// (m == 1) means Jan, so use days_in_month[0]
		if ((m == 2) && (y % 4 == 0) && (y % 100 != 0)) day += 1 ;	// Add leap day
		m++ ;
		if (m == 13)
		{
			m = 1 ;
			y++ ;
		}
		
		if (++c > 1200) break ; // Emergency escape from this loop.  Works correctly until May 2111.
	}
	
	// We started at week number 1634
	week_no.BB	= 1634 + (day / 7) ;
	day_of_week = (day % 7) - 1 ;
	
	return ;
}


void calculate_time_of_week(void)
{
	// Convert time from HHMMSSmil to time_of_week in ms
	unsigned long time = 0 ;
	int ms = time % 1000 ;
	time /= 1000 ;
	unsigned char s = time % 100 ;
	time /= 100 ;
	unsigned char m = time % 100 ;
	time /= 100 ;
	unsigned char h = time % 100 ;
	time = ((( ((long)(h)) * 60) + m) * 60 + s) * 1000 + ms ;
	tow.WW = time + (((long)day_of_week) * MS_PER_DAY) ;
	
	return ;
}


void commit_gps_data(void) 
{
	if (week_no.BB == 0) calculate_week_num() ;
	calculate_time_of_week() ;

	lat_gps		= lat_gps_ ;
	long_gps	= long_gps_ ;
	alt_sl_gps	= alt_sl_gps_ ;			// Altitude
	sog_gps		= sog_gps_ ; 			// Speed over ground
	cog_gps		= cog_gps_ ;			// Course over ground

	climb_gps.BB= (alt_sl_gps_.WW - last_alt.WW) * 5 ;		// 5Hz

	hdop		= hdop_ ;				
	svs			= svs_ ;
	
	last_alt = alt_sl_gps_ ;

	return ;
}


#endif
