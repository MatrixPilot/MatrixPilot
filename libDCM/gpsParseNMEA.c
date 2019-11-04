// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2016 MatrixPilot Team
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


#include "libDCM.h"
#include "gpsData.h"
#include "gpsParseCommon.h"
#include "../libUDB/serialIO.h"


#if (GPS_TYPE == GPS_NMEA || GPS_TYPE == GPS_ALL)

//#define DEBUG_NMEA

#ifdef DEBUG_NMEA
static uint16_t RMCpos = 0;
static uint16_t GGApos = 0;
int8_t debug_RMC[80];
int8_t debug_GGA[80];
#include <string.h>
void debug_rmc(uint8_t ch)
{
	if (RMCpos < (sizeof(debug_RMC)-1))
	{
		debug_RMC[RMCpos++] = ch;
		debug_RMC[RMCpos] = '\0';
	}
}
void debug_rmc_send(int8_t ch)
{
	debug_rmc(ch);
	printf("%s\r\n", debug_RMC);
}
void debug_gga(uint8_t ch)
{
	if (GGApos < (sizeof(debug_GGA)-1))
	{
		debug_GGA[GGApos++] = ch;
		debug_GGA[GGApos] = '\0';
	}
}
#else
//#define debug_rmc(a)
//#define debug_rmc_send(a)
//#define debug_gga(a)
#endif


// Parse the GPS messages, using the NMEA interface.
// The parser uses a state machine implemented via a pointer to a function.
// This program parses parts of GGA and RMC ie rmc1(Time), rmc2(Data valid), rmc3,4,5,6(coordinates)
// rmc7(Speed Over Ground), rmc8(Course Over Ground), rmc9(Date), gga7(number of satellites in view)
// gga8(hdop), gga9(Altitude above the sea)

static void msg_start(uint8_t gpschar);
static void gps_G(uint8_t gpschar);
static void gps_P(uint8_t gpschar);
static void gps_id1(uint8_t gpschar);
static void gps_id2(uint8_t gpschar);
static void gps_id3(uint8_t gpschar);
static void gps_comma(uint8_t gpschar);
static void gps_rmc1(uint8_t gpschar);
static void gps_rmc2(uint8_t gpschar);
static void gps_rmc3(uint8_t gpschar);
static void gps_rmc4(uint8_t gpschar);
static void gps_rmc5(uint8_t gpschar);
static void gps_rmc6(uint8_t gpschar);
static void gps_rmc7(uint8_t gpschar);
static void gps_rmc8(uint8_t gpschar);
static void gps_rmc9(uint8_t gpschar);
static void gps_gga7(uint8_t gpschar);
static void gps_gga8(uint8_t gpschar);
static void gps_gga9(uint8_t gpschar);
static void gps_checksum(uint8_t gpschar);

void (*msg_parse)(uint8_t gpschar) = &msg_start;

//const char disable_GGA[]        = "$PSRF103,00,00,00,01*24\r\n";
//const char disable_GLL[]        = "$PSRF103,01,00,00,01*25\r\n";
//const char disable_GSA[]        = "$PSRF103,02,00,00,01*26\r\n";
//const char disable_GSV[]        = "$PSRF103,03,00,00,01*27\r\n";
//const char disable_VTG[]        = "$PSRF103,05,00,00,01*21\r\n";
//const char disable_ZDA[]        = "$PSRF103,08,00,00,01*2C\r\n";
//const char enable_RMC[]         = "$PSRF103,04,00,01,01*21\r\n";
//const char set_BAUD_4800[]      = "$PMTK251,4800*14\r\n";
//const char set_BAUD_9600[]      = "$PMTK251,9600*17\r\n";
//const char set_BAUD_14400[]     = "$PMTK251,14400*29\r\n";
//const char set_BAUD_19200[]     = "$PMTK251,19200*22\r\n";
//const char set_BAUD_38400[]     = "$PMTK251,38400*27\r\n";
//const char set_BAUD_57600[]     = "$PMTK251,57600*2C\r\n";
//const char set_BAUD_115200[]    = "$PMTK251,115200*1F\r\n";
static const char set_FIX_1Hz[] = "$PMTK220,1000*1F\r\n";
//const char set_FIX_2Hz[]        = "$PMTK220,500*2B\r\n";
//const char set_FIX_3Hz[]        = "$PMTK220,333*2D\r\n";
//const char set_FIX_4Hz[]        = "$PMTK220,250*29\r\n";
//const char set_FIX_5Hz[]        = "$PMTK220,200*2C\r\n";
//const char set_RMC[]            = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
//const char set_GGA_RMC[]        = "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n";
static const char set_DEFAULT[] = "$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n";

static uint16_t rmc_counter, gga_counter;
static uint8_t id1, id2, XOR;

static int16_t digit;
static int32_t degrees, minutes;

//static union longbbbb lat_gps_, lon_gps_, alt_sl_gps_;
static union intbb sog_gps_;
static union uintbb cog_gps_;
static uint8_t svs_;
static uint8_t data_valid_, NS_, EW_;
//static uint8_t hdop_;

//union longbbbb tow_;
//union longbbbb date_gps_, time_gps_;
union intbb nav_valid_, nav_type_;
//union longbbbb climb_gps_;
union intbb week_no_;


// if data_valid is 'A', there is valid GPS data that can be used for navigation.
boolean gps_nav_valid(void)
{
	return (data_valid_ == 'A');
}

void gps_startup_sequence(int16_t gpscount)
{
	if (gpscount == 60)
	{
		#ifdef DEFAULT_GPS_BAUD
		udb_gps_set_rate(DEFAULT_GPS_BAUD);
		#else
		udb_gps_set_rate(38400);
		#warning "Default GPS BAUD not specified, now set at 38400"
		#endif
	}
	else if (gpscount == 50)
	{
		gpsoutline(set_FIX_1Hz);
	}
	else if (gpscount == 20)
	{
		gpsoutline(set_DEFAULT);
	}
//	else if (gpscount == 850)
//		gpsoutline(set_BAUD_9600);
//	else if (gpscount == 800)
//		udb_gps_set_rate(9600);
}

static void msg_start(uint8_t gpschar)
{
	if (gpschar == '$')
	{
#ifdef DEBUG_NMEA
//		udb_led_toggle(LED_BLUE);
#endif
		msg_parse = &gps_G;                 // Wait for the $
		rmc_counter = 0;
		gga_counter = 0;
		XOR = 0;
	}
	else
	{
		// error condition - stay in start state
	}
}

// The parsing routines follow. Each routine is named for the state in which the routine is applied.
// States correspond to the portions of the NMEA messages.
// If an 'G' is received, the state machine transitions to the 'P' state.

static void gps_G(uint8_t gpschar)
{
	if (gpschar == 'G')                     // "$G" String started
	{
		XOR ^= gpschar;
		msg_parse = &gps_P;                 // Get ready to parse the next int8_t
	}
	else
	{
		msg_parse = &msg_start;             // No NMEA string or string error, abort the current string and wait for $
	}
}

static void gps_P(uint8_t gpschar)
{
	if (gpschar == 'P')                     // "$GP", go on
	{
		XOR ^= gpschar;
		msg_parse = &gps_id1;
	}
	else
	{
		msg_parse = &msg_start;             // Abort current string
	}
}

static void gps_id1(uint8_t gpschar)
{
	XOR ^= gpschar;
	id1 = gpschar;
	msg_parse = &gps_id2;
}

static void gps_id2(uint8_t gpschar)
{
	XOR ^= gpschar;
	id2 = gpschar;
	msg_parse = &gps_id3;
}

static void gps_id3(uint8_t gpschar)
{
	XOR ^= gpschar;

	if (id1 == 'R' && id2 == 'M' && gpschar == 'C')      // "$GPRMC"
	{
		rmc_counter = 1;                    // Next rmc message after the comma
		msg_parse = &gps_comma;             // A comma ',' is expected now
#ifdef DEBUG_NMEA
//		msg_parse = &msg_start;
		strcpy(debug_RMC, "$GPRMC");
		udb_led_toggle(LED_BLUE);
		RMCpos = 6;
#endif
	}
	else if (id1 == 'G' && id2 == 'G' && gpschar == 'A') // "$GPGGA"
	{
		gga_counter = 1;                    // Next gga message after the comma
		msg_parse = &gps_comma;             // A comma ',' is expected now
#ifdef DEBUG_NMEA
		GGApos = 6;
//		msg_parse = &msg_start;
		strcpy(debug_GGA, "$GPGGA");
#endif
	}
	else    // ID not detected, abort
	{
		msg_parse = &msg_start;
	}
}

static void gps_comma(uint8_t gpschar)
{
#ifdef DEBUG_NMEA
	if (gga_counter > 0) debug_GGA[GGApos++] = gpschar;
	if (rmc_counter > 0) debug_rmc(gpschar);
#endif
	if (gpschar != '*') XOR ^= gpschar;
	if (gpschar == ',')
	{
		switch (rmc_counter)
		{
			case 1:
				time_gps_.WW = 0;
				msg_parse = &gps_rmc1;
				break;
			case 3:
				digit = 0;
				minutes = 0;
				degrees = 0;
				msg_parse = &gps_rmc3;
				break;
			case 4:
				msg_parse = &gps_rmc4;
				break;
			case 5:
				digit = 0;
				minutes = 0;
				degrees = 0;
				msg_parse = &gps_rmc5;
				break;
			case 6:
				msg_parse = &gps_rmc6;
				break;
			case 7:
				sog_gps_.BB = 0;
				msg_parse = &gps_rmc7;
				break;
			case 0:
				if (gga_counter++ == 7)     // I want to parse just gga7,8,9
				{
					svs_ = 0;
					msg_parse = &gps_gga7;
				}
				break;
			default:
				rmc_counter++;
				break;
		}
	}
	else
	{
		if (gpschar == '*')
		{
			msg_parse = &gps_checksum;
		}
	}
//	if (rmc_counter > 11)
//	{
//#ifdef DEBUG_NMEA
//		led_on(LED_RED);
//#endif
//		rmc_counter = 0;
//		msg_parse = &msg_start;
//	}
//	if (gga_counter > 14)
//	{
//#ifdef DEBUG_NMEA
//		led_on(LED_RED);
//#endif
//		gga_counter = 0;
//		msg_parse = &msg_start;
//	}
}

static void gps_rmc1(uint8_t gpschar)       // rmc1 -> Time HHMMSS.SSS
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // rmc1 not present or reading finished
	{
		rmc_counter = 2;
		msg_parse = &gps_rmc2;
	}
	else if (gpschar != '.')
	{
		time_gps_.WW = time_gps_.WW * 10 + (gpschar - '0');
	}
}

static void gps_rmc2(uint8_t gpschar)       // GPS status
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // GPS status information not available
	{
		rmc_counter = 3;
		digit = 0;
		minutes = 0;
		degrees = 0;
		msg_parse = &gps_rmc3;
	}
	else
	{
		data_valid_ = gpschar;
		rmc_counter = 3;
		msg_parse = &gps_comma;
	}
}

static void gps_rmc3(uint8_t gpschar)       // latitude
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // latitude not providev, error! start over again
	{
		msg_parse = &msg_start;
	}
	else
	{
		digit++;
		switch (digit)
		{
			case 1: case 2:
				degrees = 10 * degrees + (gpschar - '0');
				break;
			case 3: case 4: case 6: case 7: case 8:
				minutes = 10 * minutes + (gpschar - '0');
				break;
			case 5:                         // DDMM.MMMM Skip the point in the middle
				break;
			case 9:
				minutes = 10*minutes + (gpschar - '0');
				rmc_counter = 4;
				lat_gps_.WW = (((long)10000000)*degrees+(50*minutes)/3);
				msg_parse = &gps_comma;
				break;
			default:
				break;
		}
	}
}

static void gps_rmc4(uint8_t gpschar)       // N or S int8_t
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // NS information not available
	{
		NS_ = ' ';
		rmc_counter = 5;
		digit = 0;
		minutes = 0;
		degrees = 0;
		msg_parse = & gps_rmc5;
	}
	else
	{
		NS_ = gpschar;
		if (NS_ == 'S') lat_gps_.WW = - lat_gps_.WW;
		rmc_counter = 5;
		msg_parse = & gps_comma;
	}
}

static void gps_rmc5(uint8_t gpschar)       // Longitude
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // Longitude not provided, error! start over again
	{
		msg_parse = &msg_start;
	}
	else
	{
		digit++;
		switch (digit)                      // DDDMM.MMMM
		{
			case 1: case 2: case 3:
				degrees = 10 * degrees + (gpschar - '0');
				break;
			case 4: case 5: case 7: case 8: case 9:
				minutes = 10 * minutes + (gpschar - '0');
				break;
			case 6:
				break;
			case 10:
				minutes = 10 * minutes + (gpschar - '0');
				rmc_counter = 6;
				lon_gps_.WW = (((long)10000000)*degrees+(50*minutes)/3); // Sure that minutes should be multiplied by 10?
				msg_parse = &gps_comma;
				break;
			default:
				break;
		}
	}
}

static void gps_rmc6(uint8_t gpschar)       // E or W int8_t
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // EW not provided
	{
		EW_ = ' ';
		rmc_counter = 7;
		msg_parse = &gps_rmc7;              // Parse the next message
	}
	else
	{
		EW_ = gpschar;
		if (EW_ == 'W') lon_gps_.WW = - lon_gps_.WW;
		rmc_counter = 7;
		msg_parse = &gps_comma;
	}
}

static void gps_rmc7(uint8_t gpschar)       // Speed over ground
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')
	{
		sog_gps_.BB = sog_gps_.BB >> 1;     // knots*100/2? almost cm/s. 1 knot  50cm/s
		rmc_counter = 8;
		cog_gps_.BB = 0;
		msg_parse = &gps_rmc8;
	}
	else if (gpschar != '.')
	{
		sog_gps_.BB = sog_gps_.BB * 10 + (gpschar - '0');
	}
}

static void gps_rmc8(uint8_t gpschar)       // Course Over Ground
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')
	{
		rmc_counter = 9;
		date_gps_.WW = 0;
		msg_parse = &gps_rmc9;
	}
	else if (gpschar != '.')
	{
		cog_gps_.BB = cog_gps_.BB * 10 + (gpschar - '0');    // Course*100
	}
}

static void gps_rmc9(uint8_t gpschar)       // rmc9 -> Date DDMMYY
{
#ifdef DEBUG_NMEA
	debug_rmc(gpschar);
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // rmc9 not present or reading finished
	{
//		rmc_counter = 10;
		msg_parse = &gps_comma;             // rmc_counter will be incremented in gps_comma()
	}
	else if (gpschar != '.')
	{
		date_gps_.WW = date_gps_.WW * 10 + (gpschar - '0');
	}
}

static void gps_gga7(uint8_t gpschar)       // gga7 -> svs XX
{
#ifdef DEBUG_NMEA
	debug_GGA[GGApos++] = gpschar;
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // gga7 not present or reading finished
	{
		gga_counter = 8;
		msg_parse = &gps_gga8;
	}
	else
	{
		svs_ = svs_ * 10 + (gpschar - '0');
	}
}

static void gps_gga8(uint8_t gpschar)       // Hdop XX.XX -> Meters*5
{
	static uint16_t temp = 0;

#ifdef DEBUG_NMEA
	debug_GGA[GGApos++] = gpschar;
#endif
	XOR ^= gpschar;
	if (gpschar == ',')
	{
		hdop_._.B0 = temp >> 1;             // From meters*100 to meters*5
		temp = 0;
		alt_sl_gps_.WW = 0;
		gga_counter = 9;
		msg_parse = &gps_gga9;
	}
	else if (gpschar != '.')
	{
		temp = temp * 10 + (gpschar - '0');
	}
}

static void gps_gga9(uint8_t gpschar)       // Altitude above sea .m
{
#ifdef DEBUG_NMEA
	debug_GGA[GGApos++] = gpschar;
#endif
	XOR ^= gpschar;
	if (gpschar == ',')                     // gga9 not present or reading finished
	{
		alt_sl_gps_.WW *= 10;               // From dm to cm
		gga_counter = 10;
		msg_parse = &gps_comma;             // gga_counter will be incremented in gps_comma()
	}
	else if (gpschar != '.')
	{
		alt_sl_gps_.WW = alt_sl_gps_.WW * 10 + (gpschar - '0');
	}
}

static void gps_checksum(uint8_t gpschar)   // checksum calculation
{
	static uint8_t checksum = 0;
	static boolean gga_chksm_ok;

#ifdef DEBUG_NMEA
	if (gga_counter > 0) debug_GGA[GGApos++] = gpschar;
	if (rmc_counter > 0) debug_rmc(gpschar);
#endif
	if (gpschar == 0x0D)                    // checksum reading finished
	{
//		debug_rmc_send('a');
		msg_parse = &msg_start;

		if (XOR == checksum)
		{
			if (gga_counter > 0)            // We are checking gga
			{
				gga_chksm_ok = 1;           // And the checksum is good
			}
			if (rmc_counter > 0 && gga_chksm_ok == 1)
			{
#ifdef DEBUG_NMEA
				debug_rmc_send(gpschar);
#endif
				gps_parse_common();         // parsing is complete, schedule navigation
			}
		}
		else
		{
			if (gga_counter > 0)            // We are checking gga
			{
				gga_chksm_ok = 0;           // And the checksum is bad
			}
		}
		checksum = 0;
	}
	else
	{
		if (gpschar <= '9')
		{
			checksum = (checksum << 4) + (gpschar - '0');
		}
		else
		{
			checksum = (checksum << 4) + (gpschar - 'A' + 0x0A);
		}
	}
#ifdef DEBUG_NMEA
	led_off(LED_RED);
#endif
}

void gps_commit_data(void)
{
	static union longbbbb last_alt = { 0 };

	if (week_no.BB == 0)
	{
		week_no.BB = calculate_week_num(date_gps_.WW);
	}
	tow.WW = calculate_time_of_week(time_gps_.WW);
	lat_gps      = lat_gps_;
	lon_gps      = lon_gps_;
	alt_sl_gps   = alt_sl_gps_;             // Altitude
	sog_gps      = sog_gps_;                // Speed over ground
	cog_gps      = cog_gps_;                // Course over ground
	climb_gps.BB = (alt_sl_gps_.WW - last_alt.WW) * GPS_RATE;
	hdop         = hdop_._.B0;
	svs          = svs_;
	last_alt     = alt_sl_gps_;
}

void gps_update_basic_data(void)
{
	svs          = svs_;
}

void init_gps_nmea(void)
{
}

#endif // (GPS_TYPE == GPS_NMEA || GPS_TYPE == GPS_ALL)
