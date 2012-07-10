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
//	This program parse parts of GGA and RMC ie rmc1(Time), rmc2(Data valid), rmc3,4,5,6(coordinates)
//	rmc7(Speed Over Ground), rmc8(Course Over Ground), rmc9(Date), gga7(number of satellites in view)
//	gga8(Hdop), gga9(Altitude above the sea)

#define MS_PER_DAY	86400000 // = (24 * 60 * 60 * 1000)

void dollar ( unsigned char inchar ) ;
void gps_G(unsigned char inchar) ;
void gps_P(unsigned char inchar) ;
void gps_id1(unsigned char inchar) ;
void gps_id2(unsigned char inchar) ;
void gps_id3(unsigned char inchar) ;
void gps_comma(unsigned char inchar) ;
void gps_rmc1(unsigned char inchar) ;
void gps_rmc2(unsigned char inchar) ;
void gps_rmc3(unsigned char inchar) ;
void gps_rmc4(unsigned char inchar) ;
void gps_rmc5(unsigned char inchar) ;
void gps_rmc6(unsigned char inchar) ;
void gps_rmc7(unsigned char inchar) ;
void gps_rmc8(unsigned char inchar) ;
void gps_rmc9(unsigned char inchar) ;
void gps_gga7(unsigned char inchar) ;
void gps_gga8(unsigned char inchar) ;
void gps_gga9(unsigned char inchar) ;
void gps_checksum(unsigned char inchar) ;

void (* msg_parse ) ( unsigned char inchar ) = &dollar ;


unsigned int rmc_counter, gga_counter;
unsigned char id1,id2, XOR;

int digit ;
long degrees, minutes ;

union longbbbb lat_gps_ , long_gps_ , alt_sl_gps_ , tow_, last_alt, date_gps_ , time_gps_ ;
union intbb    nav_valid_ , nav_type_ , sog_gps_ , cog_gps_ , climb_gps_ , week_no_ ;
unsigned char data_valid_ , NS_ , EW_, svs_, day_of_week, hdop_ ;


const unsigned char days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31} ;

//const char disable_GGA[] = "$PSRF103,00,00,00,01*24\r\n" ; 
//const char disable_GLL[] = "$PSRF103,01,00,00,01*25\r\n" ; 
//const char disable_GSA[] = "$PSRF103,02,00,00,01*26\r\n" ;
//const char disable_GSV[] = "$PSRF103,03,00,00,01*27\r\n" ; 
//const char disable_VTG[] = "$PSRF103,05,00,00,01*21\r\n" ; 
//const char disable_ZDA[] = "$PSRF103,08,00,00,01*2C\r\n" ; 
//const char enable_RMC[]  = "$PSRF103,04,00,01,01*21\r\n" ; 
//const char set_BAUD_4800[]  		= "$PMTK251,4800*14\r\n" ; 
//const char set_BAUD_9600[]  		= "$PMTK251,9600*17\r\n" ; 
//const char set_BAUD_14400[]  		= "$PMTK251,14400*29\r\n" ; 
//const char set_BAUD_19200[]  		= "$PMTK251,19200*22\r\n" ; 
//const char set_BAUD_38400[] 		= "$PMTK251,38400*27\r\n" ; 
//const char set_BAUD_57600[] 		= "$PMTK251,57600*2C\r\n" ; 
//const char set_BAUD_115200[]		= "$PMTK251,115200*1F\r\n" ; 
const char set_FIX_1Hz[]		= "$PMTK220,1000*1F\r\n" ; 
//const char set_FIX_2Hz[]		= "$PMTK220,500*2B\r\n" ; 
//const char set_FIX_3Hz[]		= "$PMTK220,333*2D\r\n" ; 
//const char set_FIX_4Hz[]		= "$PMTK220,250*29\r\n" ; 
//const char set_FIX_5Hz[]		= "$PMTK220,200*2C\r\n" ; 
//const char set_RMC[]		= "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" ; 
const char set_GGA_RMC[]	= "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n" ; 
//const char set_DEFAULT[]	= "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n" ; 

//	if data_valid is 'A', there is valid GPS data that can be used for navigation.
boolean gps_nav_valid(void)
{
	return (data_valid_ == 'A') ;
}

void gps_startup_sequence(int gpscount)
{
	if (gpscount == 980)
	{
		#ifdef DEFAULT_GPS_BAUD
		udb_gps_set_rate(DEFAULT_GPS_BAUD);
		#else
		udb_gps_set_rate(38400);
		#warning "Default GPS BAUD not specified, now set at 38400"
		#endif
	}		
	else if( gpscount == 800 )
		gpsoutline( (char*)set_FIX_1Hz );
	else if( gpscount == 600 )
		gpsoutline( (char*)set_GGA_RMC );
//	else if( gpscount == 850 )
//		gpsoutline( (char*)set_BAUD_9600 );
//	else if( gpscount == 800 )
//		udb_gps_set_rate(9600);
	
	return ;
}


void dollar(unsigned char inchar) 
{
	if ( inchar == '$' )
	{
		msg_parse = &gps_G ;			// Wait for the $
		rmc_counter = 0;
		gga_counter = 0;
		XOR = 0;
	}	
	return ;
}

void gps_G( unsigned char inchar)
{
	if ( inchar == 'G' )		// "$G" String started
	{
		XOR ^= inchar;
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
		XOR ^= inchar;
		msg_parse = &gps_id1 ;	
	}
	else
	{
		msg_parse = &dollar ;	// Abort current string
	}
	return ;
}

void gps_id1( unsigned char inchar)
{
	XOR ^= inchar;
	id1 = inchar;
	msg_parse = &gps_id2 ;
	return;
}


void gps_id2( unsigned char inchar)
{
	XOR ^= inchar;
	id2 = inchar;
	msg_parse = &gps_id3 ;
	return ;
}

void gps_id3( unsigned char inchar)
{
	XOR ^= inchar;

	if ( id1 == 'R' && id2 == 'M' && inchar == 'C' )		// "$GPRMC"
	{
		rmc_counter = 1;			// Next rmc message after the comma
		msg_parse = &gps_comma ;	// A comma ',' is expected now
	}
	else if ( id1 == 'G' && id2 == 'G' && inchar == 'A' )		// "$GPGGA"
	{		
		gga_counter = 1;			// Next gga message after the comma
		msg_parse = &gps_comma ;	// A comma ',' is expected now		
	}	
	else	// ID not detected, abort
	{
		msg_parse = &dollar ;
	}
	return ;
}

void gps_comma( unsigned char inchar )
{
	if ( inchar != '*' )	XOR ^= inchar;
	if ( inchar == ',' )
	{
		switch (rmc_counter)
		{
			case 1:
				time_gps_.WW = 0;	
				msg_parse = &gps_rmc1 ;
				break;
			case 3:
				digit = 0 ;
				minutes = 0 ;
				degrees = 0 ;
				msg_parse = &gps_rmc3 ;
				break;
			case 4:
				msg_parse = &gps_rmc4 ;
				break;
			case 5:
				digit = 0 ;
				minutes = 0 ;
				degrees = 0 ;
				msg_parse = &gps_rmc5 ;
				break;
			case 6:
				msg_parse = &gps_rmc6 ;
				break;
			case 7:
				sog_gps_.BB = 0 ;
				msg_parse = &gps_rmc7 ;
				break;
				
			case 0:
				gga_counter++;				// I want to parse just gga7,8,9
				if (gga_counter == 7)
				{
					svs_ = 0;
					msg_parse = &gps_gga7 ;
				}	
				break;
							
			default:
				rmc_counter++;
//				if(rmc_counter > 11)
//				{
//					rmc_counter = 0;
//					msg_parse = &dollar;
//				}				
				break;
		}	
	}
	else
	{
		if( gga_counter == 14 && inchar == '*' )
		{
			msg_parse = &gps_checksum;
		}	
		if( rmc_counter == 11 && inchar == '*' )
		{
			msg_parse = &gps_checksum;
		}	
	}	
	return ;
}

void gps_rmc1( unsigned char inchar )	// rmc1 -> Time HHMMSS.SSS
{
	XOR ^= inchar;
	if ( inchar == ',' )				// rmc1 not present or reading finished
	{
		rmc_counter = 2;
		msg_parse = &gps_rmc2 ;
	}
	else if ( inchar != '.' )
	{
		time_gps_.WW = time_gps_.WW*10 + ( inchar - '0' ) ;
	}
	return ;
}

void gps_rmc2( unsigned char inchar )	// GPS status
{
	XOR ^= inchar;
	if ( inchar == ',' )		// GPS status information not available
	{
		rmc_counter = 3;
		digit = 0 ;
		minutes = 0 ;
		degrees = 0 ;
		msg_parse = &gps_rmc3 ;
	}
	else
	{
		data_valid_ = inchar ;
		rmc_counter = 3;
		msg_parse = &gps_comma ;
	}
	return ;
}

void gps_rmc3 ( unsigned char inchar )		// latitude
{
	XOR ^= inchar;
	if ( inchar == ',' )	// latitude not providev, error! start over again
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
				rmc_counter = 4;
				lat_gps_.WW = (((long)10000000)*degrees+(50*minutes)/3) ;
				msg_parse = &gps_comma ;
			break ;
			default :
			break ;
		}	
	}
	return ;
}

void gps_rmc4( unsigned char inchar )	// N or S char
{
	XOR ^= inchar;
	if ( inchar == ',' )			// NS information not available
	{
		NS_ = ' ' ;
		rmc_counter = 5;
		digit = 0 ;
		minutes = 0 ;
		degrees = 0 ;
		msg_parse = & gps_rmc5 ;
	}
	else
	{
		NS_ = inchar ;
		if ( NS_ == 'S' ) lat_gps_.WW = - lat_gps_.WW ;
		rmc_counter = 5;
		msg_parse = & gps_comma ;
	}
	return ;
}

void gps_rmc5 ( unsigned char inchar )	// Longitude
{
	XOR ^= inchar;
	if ( inchar == ',' )		// Longitude not provided, error! start over again
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
				rmc_counter = 6;
				long_gps_.WW = (((long)10000000)*degrees+(50*minutes)/3) ; //Sure that minutes should be multiplied by 10?
				msg_parse = &gps_comma ;
			break ;
			default :
			break ;
		}	
	}
	return ;
}

void gps_rmc6( unsigned char inchar )	// E or W char
{
	XOR ^= inchar;
	if ( inchar == ',' )	// EW not provided
	{
		EW_ = ' ' ;
		rmc_counter = 7;
		msg_parse = & gps_rmc7 ;	// Parse the next message
	}
	else
	{
		EW_ = inchar ;
		if( EW_ == 'W' ) long_gps_.WW = - long_gps_.WW ;
		rmc_counter = 7;
		msg_parse = & gps_comma ;
	}

	return ;
}

void gps_rmc7( unsigned char inchar )	// Speed over ground
{
	XOR ^= inchar;
	if ( inchar == ',' )
	{
		sog_gps_.BB = sog_gps_.BB >> 1 ;	// knots*100/2? almost cm/s. 1 knot ˜ 50cm/s
		rmc_counter = 8;
		cog_gps_.BB = 0 ;
		msg_parse = & gps_rmc8 ;
	}
	else if ( inchar != '.' )
	{
		sog_gps_.BB = sog_gps_.BB*10 + ( inchar - '0' ) ;
	}
	return ;
}

void gps_rmc8( unsigned char inchar )	// Course Over Ground
{
	XOR ^= inchar;
	if ( inchar == ',' )
	{
		rmc_counter = 9;
		date_gps_.WW = 0;
		msg_parse = &gps_rmc9 ;
	}
	else if ( inchar != '.' )
	{
		cog_gps_.BB = cog_gps_.BB*10 + ( inchar - '0' ) ;	// Course*100
	}
	return ;
}

void gps_rmc9( unsigned char inchar )	// rmc9 -> Date DDMMYY
{
	XOR ^= inchar;
	if ( inchar == ',' )				// rmc9 not present or reading finished
	{
//		rmc_counter = 10;
		msg_parse = &gps_comma ;		// rmc_counter will be incremented in gps_comma()
	}
	else if ( inchar != '.' )
	{
		date_gps_.WW = date_gps_.WW*10 + ( inchar - '0' ) ;
	}
	return ;
}

void gps_gga7( unsigned char inchar )	// gga7 -> svs XX
{
	XOR ^= inchar;
	if ( inchar == ',' )				// gga7 not present or reading finished
	{
		gga_counter = 8;
		msg_parse = &gps_gga8 ;
	}
	else
	{
		svs_ = svs_*10 + ( inchar - '0' ) ;
	}
	return ;
}

void gps_gga8( unsigned char inchar )	// Hdop XX.XX -> Meters*5
{
	static unsigned int temp = 0;
	
	XOR ^= inchar;
	if ( inchar == ',' )
	{
		hdop_ = temp>>1;	// From meters*10 to meters*5
		temp = 0;
		alt_sl_gps_.WW = 0;
		gga_counter = 9;
		msg_parse = &gps_gga9 ;
	}
	else if ( inchar != '.' )
	{
		temp = temp*10 + ( inchar - '0' ) ;
	}
	return ;
}

void gps_gga9( unsigned char inchar )	// Altitude above sea .m
{
	XOR ^= inchar;
	if ( inchar == ',' )				// gga9 not present or reading finished
	{
		alt_sl_gps_.WW *= 10;	// From dm to cm
		msg_parse = &gps_comma ;	// gga_counter will be incremented in gps_comma()
	}
	else if ( inchar != '.' )
	{
		alt_sl_gps_.WW = alt_sl_gps_.WW*10 + ( inchar - '0' ) ;
	}
	return ;
}

void gps_checksum( unsigned char inchar )	// checksum calculation
{
	static unsigned char checksum = 0;
	static boolean gga_chksm_ok;

	if ( inchar == 0x0D )				// checksum reading finished
	{
		msg_parse = &dollar ;
	
		if( XOR == checksum )
		{
			if(gga_counter >0)		// We are checking gga
				gga_chksm_ok = 1;	// And the checksum is good
	
			if(rmc_counter >0 && gga_chksm_ok == 1)
			{
				udb_background_trigger() ;  // parsing is complete, schedule navigation
			}	
		}
		else
		{
			if(gga_counter >0)		// We are checking gga
				gga_chksm_ok = 0;	// And the checksum is bad
		}	
		checksum = 0;	
	}	
	else
	{
		if(inchar <= '9')
			checksum = (checksum<<4) + ( inchar - '0' ) ;
		else
			checksum = (checksum<<4) + ( inchar - 'A' + 0x0A ) ;
	}

	return;
}
void calculate_week_num(void)
{
	// Convert date from DDMMYY to week_num and day_of_week
	long date = date_gps_.WW ;
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
	unsigned long time = time_gps_.WW ;
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

	climb_gps.BB= (alt_sl_gps_.WW - last_alt.WW);

	hdop		= hdop_ ;				
	svs			= svs_ ;
	
	last_alt = alt_sl_gps_ ;

	return ;
}


#endif
