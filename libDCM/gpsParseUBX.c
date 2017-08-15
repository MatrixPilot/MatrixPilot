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


#include "libDCM.h"
#include "gpsData.h"
#include "gpsParseCommon.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/magnetometer.h"
#include "mag_drift.h"
#include "rmat.h"
#include "hilsim.h"

#if (GPS_TYPE == GPS_UBX_2HZ || GPS_TYPE == GPS_UBX_4HZ || GPS_TYPE == GPS_ALL)

// Parse the GPS messages, using the binary interface.
// The parser uses a state machine implemented via a pointer to a function.
// Binary values received from the GPS are directed to program variables via a table
// of pointers to the variable locations.
// Unions of structures are used to be able to access the variables as long, ints, or bytes.

static union intbb payloadlength;
static union intbb checksum;
static uint16_t msg_class;
static uint16_t msg_id;
static uint16_t ack_class; // set but never used - RobD
static uint16_t ack_id; // set but never used - RobD
static uint16_t ack_type; // set but never used - RobD
static uint8_t CK_A;
static uint8_t CK_B;

static void msg_B3(uint8_t inchar);
static void msg_SYNC1(uint8_t inchar);
static void msg_SYNC2(uint8_t inchar);
static void msg_CLASS(uint8_t inchar);
static void msg_ID(uint8_t inchar);
static void msg_PL1(uint8_t inchar);
static void msg_POSLLH(uint8_t inchar);
static void msg_DOP(uint8_t inchar);
static void msg_SOL(uint8_t inchar);
static void msg_VELNED(uint8_t inchar);
static void msg_CS0(uint8_t inchar);
static void msg_CS1(uint8_t inchar);

#if (HILSIM == 1)
	static void msg_BODYRATES(uint8_t inchar);
	static void msg_KEYSTROKE(uint8_t gpschar);
#endif

static void msg_MSGU(uint8_t inchar);
static void msg_ACK_CLASS(uint8_t inchar);
static void msg_ACK_ID(uint8_t inchar);

//void bin_out(char outchar);

const char bin_mode_withnmea[] = "$PUBX,41,1,0003,0003,19200,0*21\r\n"; // turn on UBX + NMEA, 19200 baud
const char bin_mode_nonmea[] = "$PUBX,41,1,0003,0001,19200,0*23\r\n";   // turn on UBX only, 19200 baud
const char disable_GSV[] = "$PUBX,40,GSV,0,0,0,0,0,0*59\r\n"; //Disable the $GPGSV NMEA message
const char disable_VTG[] = "$PUBX,40,VTG,0,0,0,0,0,0*5E\r\n"; //Disable the $GPVTG NMEA message
const char disable_GLL[] = "$PUBX,40,GLL,0,0,0,0,0,0*5C\r\n"; //Disable the $GPGLL NMEA message
const char disable_GSA[] = "$PUBX,40,GSA,0,0,0,0,0,0*4E\r\n"; //Disable the $GPGSA NMEA message

#if (GPS_TYPE == GPS_UBX_4HZ)
const uint8_t set_rate[] = {
	0xB5, 0x62, // Header
	0x06, 0x08, // ID
	0x06, 0x00, // Payload Length
	0xFA, 0x00, // measRate
	0x01, 0x00, // navRate
	0x01, 0x00, // timeRef
	0x10, 0x96  // Checksum
};
#else
const uint8_t set_rate[] = {
	0xB5, 0x62, // Header
	0x06, 0x08, // ID
	0x06, 0x00, // Payload Length
	0xF4, 0x01, // measRate 2Hz
	0x01, 0x00, // navRate
	0x01, 0x00, // timeRef
	0x0B, 0x77  // Checksum
};
#endif

const uint8_t enable_UBX_only[] = {
	0xB5, 0x62, // Header
	0x06, 0x00, // ID
	0x14, 0x00, // Payload length
	0x01,       // Port ID
	0x00,       // res0
	0x00, 0x00, // res1
	0xD0, 0x08, 0x00, 0x00, // mode
	0x00, 0x4B, 0x00, 0x00, // baudrate
	0x03, 0x00, // inProtoMask
	0x01, 0x00, // outProtoMask
	0x00, 0x00, // Flags - reserved, set to 0
	0x00, 0x00, // Pad - reserved, set to 0
	0x42, 0x2B  // checksum
};

const uint8_t enable_UBX_NMEA[] = {
	0xB5, 0x62, // Header
	0x06, 0x00, // ID
	0x14, 0x00, // Payload length
	0x01,       // Port ID
	0x00,       // res0
	0x00, 0x00, // res1
	0xD0, 0x08, 0x00, 0x00, // mode
	0x00, 0x4B, 0x00, 0x00, // baudrate
	0x03, 0x00, // inProtoMask
	0x03, 0x00, // outProtoMask
	0x00, 0x00, // Flags - reserved, set to 0
	0x00, 0x00, // Pad - reserved, set to 0
	0x44, 0x37  // checksum
};

const uint8_t enable_NAV_SOL[] = {
	0xB5, 0x62, // Header
	0x06, 0x01, // ID
	0x08, 0x00, // Payload length
	0x01,       // NAV message class
	0x06,       // SOL message ID
	0x00,       // Rate on I2C
	0x01,       // Rate on UART 1
	0x00,       // Rate on UART 2
	0x00,       // Rate on USB
	0x00,       // Rate on SPI
	0x00,       // Rate on ???
	0x17, 0xDA  // Checksum
};

const uint8_t enable_NAV_POSLLH[] = {
	0xB5, 0x62, // Header
	0x06, 0x01, // ID
	0x08, 0x00, // Payload length
	0x01,       // NAV message class
	0x02,       // POSLLH message ID
	0x00,       // Rate on I2C
	0x01,       // Rate on UART 1
	0x00,       // Rate on UART 2
	0x00,       // Rate on USB
	0x00,       // Rate on SPI
	0x00,       // Rate on ???
	0x13, 0xBE  // Checksum
};

const uint8_t enable_NAV_VELNED[] = {
	0xB5, 0x62, // Header
	0x06, 0x01, // ID
	0x08, 0x00, // Payload length
	0x01,       // NAV message class
	0x12,       // VELNED message ID
	0x00,       // Rate on I2C
	0x01,       // Rate on UART 1
	0x00,       // Rate on UART 2
	0x00,       // Rate on USB
	0x00,       // Rate on SPI
	0x00,       // Rate on ???
	0x23, 0x2E  // Checksum
};

#if (GPS_TYPE == GPS_UBX_4HZ)
const uint8_t enable_NAV_DOP[] = {
	0xB5, 0x62, // Header
	0x06, 0x01, // ID
	0x08, 0x00, // Payload length
	0x01,       // NAV message class
	0x04,       // DOP message ID
	0x00,       // Rate on I2C
	0x04,       // Rate on UART 1
	0x00,       // Rate on UART 2
	0x00,       // Rate on USB
	0x00,       // Rate on SPI
	0x00,       // Rate on ???
	0x18, 0xDB  // Checksum
};
#else
const uint8_t enable_NAV_DOP[] = {
	0xB5, 0x62, // Header
	0x06, 0x01, // ID
	0x08, 0x00, // Payload length
	0x01,       // NAV message class
	0x04,       // DOP message ID
	0x00,       // Rate on I2C
	0x02,       // Rate on UART 1
	0x00,       // Rate on UART 2
	0x00,       // Rate on USB
	0x00,       // Rate on SPI
	0x00,       // Rate on ???
	0x16, 0xD1  // Checksum
};
#endif

const uint8_t enable_SBAS[] = {
	0xB5, 0x62, // Header
	0x06, 0x16, // ID
	0x08, 0x00, // Payload length
	0x01,       // Enable SBAS
	0x03,       //
	0x01,       //
	0x00,       //
	0x00,       //
	0x00,       //
	0x00,       //
	0x00,       //
	0x29, 0xAD  // Checksum
};

const uint8_t config_NAV5[] = {
	0xB5, 0x62, // Header
	0x06, 0x24, // ID
	0x24, 0x00, // Payload length
	0xFF, 0xFF, // Bit Mask, 0XFF means apply all of the config below
	0x08,       // Dynamic Model Number, Airborne with <4g Acceleration
	0x02,       // Position Fixing Mode. 3D only.
	0x00, 0x00, // Fixed altitude (mean sea level) for 2D fix mode only.
	0x00, 0x00, // Part of fixed altitude above (4 bytes in total)
	0x10, 0x27, // Fixed Altitude variance for 2D Mode (4 bytes)
	0x00, 0x00, //
	0x05, 0x00, // Minimum Elevation of Sats in degrees (1 byte). Dead Reckoning Limit in Seconds.
	0xFA, 0x00, // Position DOP Mask (2 bytes)
	0xFA, 0x00, // Time DOP Mask (2 bytes))
	0x64, 0x00, // Position Accuracy Mask (2 bytes) Meters)
	0x2C, 0x01, // Time Accuracy Mask (2 bytes))
	0x00, 0x00, // Static Hold Threshold (1 bytes). DGPS timeout (1 bytes))
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x17, 0xFF  // Checksum
};

const uint16_t set_rate_length = 14;
const uint16_t enable_NAV_SOL_length = 16;
const uint16_t enable_NAV_POSLLH_length = 16;
const uint16_t enable_NAV_VELNED_length = 16;
const uint16_t enable_NAV_DOP_length = 16;
const uint16_t enable_UBX_only_length = 28;
const uint16_t enable_SBAS_length = 16;
const uint16_t config_NAV5_length = 44;

void (*msg_parse)(uint8_t gpschar) = &msg_B3;

static uint8_t un;
//static union longbbbb xpg_, ypg_, zpg_;
//static union longbbbb xvg_, yvg_, zvg_;
//static uint8_t mode1_, mode2_;
static uint8_t svs_, nav_valid_;
//static union longbbbb lat_gps_, lon_gps_, alt_sl_gps_;
static union longbbbb sog_gps_, cog_gps_, climb_gps_;
//static union longbbbb tow_;
static union longbbbb as_sim_;
//static union intbb hdop_;
static union intbb week_no_;

uint8_t svsmin = 24;
uint8_t svsmax = 0;
static int16_t store_index = 0;
static int16_t nmea_passthru_countdown = 0; // used by nmea_passthru to count how many more bytes are passed through
static uint8_t nmea_passthrough_char = 0;
//static int16_t frame_errors = 0;

#if (HILSIM == 1)
static union intbb g_a_x_sim_, g_a_y_sim_, g_a_z_sim_;
static union intbb g_a_x_sim,  g_a_y_sim,  g_a_z_sim;
static union intbb p_sim_,     q_sim_,     r_sim_;
static union intbb p_sim,      q_sim,      r_sim;
static uint8_t x_ckey_, x_vkey_;
static void commit_bodyrate_data(void);
static void commit_keystroke_data(void);
#endif

#if (HILSIM == 1 && MAG_YAW_DRIFT == 1)
extern uint8_t magreg[6];
#endif

uint8_t* const msg_SOL_parse[] = {
	&tow_.__.B0, &tow_.__.B1, &tow_.__.B2, &tow_.__.B3, // iTOW
	&un, &un, &un, &un,                                 // fTOW
	&week_no_._.B0, &week_no_._.B1,                     // week
	&nav_valid_,                                        // gpsFix
	&un,                                                // flags
	&un, &un, &un, &un,                                 // ecefX
	&un, &un, &un, &un,                                 // ecefY
	&un, &un, &un, &un,                                 // ecefZ
#if (HILSIM == 1 && MAG_YAW_DRIFT == 1)
	&magreg[1], &magreg[0], &magreg[3], &magreg[2],     // simulate the magnetometer with HILSIM, and use these slots
	                                                    // note: mag registers come out high:low from magnetometer
#else
	&un, &un, &un, &un,                                 // pAcc
#endif
	&un, &un, &un, &un,                                 // ecefVX
	&un, &un, &un, &un,                                 // ecefVY
	&un, &un, &un, &un,                                 // ecefVZ

#if (HILSIM == 1 && MAG_YAW_DRIFT == 1)
	&magreg[5], &magreg[4], &un, &un,                   // simulate the magnetometer with HILSIM, and use these slots
	                                                    // note: mag registers come out high:low from magnetometer
#else
	&un, &un, &un, &un,                                 // sAcc
#endif
	&un, &un,                                           // pDOP
	&un,                                                // res1
	&svs_,                                              // numSV
	&un, &un, &un, &un,                                 // res2
};

uint8_t* const msg_DOP_parse[] = {
	&un, &un, &un, &un,                                 // iTOW
	&un, &un,                                           // gDOP
	&un, &un,                                           // pDOP
	&un, &un,                                           // tDOP
	&un, &un,                                           // vDOP
	&hdop_._.B0, &hdop_._.B1,                           // hDOP
	&un, &un,                                           // nDOP
	&un, &un,                                           // eDOP
};

uint8_t* const msg_POSLLH_parse[] = {
	&un, &un, &un, &un,                                 // iTOW
	&lon_gps_.__.B0, &lon_gps_.__.B1,
	&lon_gps_.__.B2, &lon_gps_.__.B3,                   // lon
	&lat_gps_.__.B0, &lat_gps_.__.B1,
	&lat_gps_.__.B2, &lat_gps_.__.B3,                   // lat
	&un, &un, &un, &un,                                 // height
	&alt_sl_gps_.__.B0, &alt_sl_gps_.__.B1,
	&alt_sl_gps_.__.B2, &alt_sl_gps_.__.B3,             // hMSL
	&un, &un, &un, &un,                                 // hAcc
	&un, &un, &un, &un,                                 // vAcc
};

uint8_t* const msg_VELNED_parse[] = {
	&un, &un, &un, &un,                                 // iTOW
	&un, &un, &un, &un,                                 // velN
	&un, &un, &un, &un,                                 // velE
	&climb_gps_.__.B0, &climb_gps_.__.B1,
	&climb_gps_.__.B2, &climb_gps_.__.B3,               // velD
	&as_sim_.__.B0, &as_sim_.__.B1,
	&as_sim_.__.B2, &as_sim_.__.B3,                     // air speed
	&sog_gps_.__.B0, &sog_gps_.__.B1,
	&sog_gps_.__.B2, &sog_gps_.__.B3,                   // gSpeed
	&cog_gps_.__.B0, &cog_gps_.__.B1,
	&cog_gps_.__.B2, &cog_gps_.__.B3,                   // heading
	&un, &un, &un, &un,                                 // sAcc
	&un, &un, &un, &un,                                 // cAcc
};

#if (HILSIM == 1)
// These are the data being delivered from the hardware-in-the-loop simulator
uint8_t* const msg_BODYRATES_parse[] = {
	&p_sim_._.B0, &p_sim_._.B1,         // roll rate
	&q_sim_._.B0, &q_sim_._.B1,         // pitch rate
	&r_sim_._.B0, &r_sim_._.B1,         // yaw rate
	&g_a_x_sim_._.B0, &g_a_x_sim_._.B1, // x accel reading (grav - accel, body frame)
	&g_a_y_sim_._.B0, &g_a_y_sim_._.B1, // y accel reading (grav - accel, body frame)
	&g_a_z_sim_._.B0, &g_a_z_sim_._.B1, // z accel reading (grav - accel, body frame)
};
uint8_t* const msg_KEYSTROKE_parse[] = {
	&x_ckey_, &x_vkey_, // control code, virtual keystroke code
};
#endif // HILSIM

void gps_startup_sequence(int16_t gpscount)
{
	if (gpscount == 980)
	{
#if (HILSIM == 1)
		udb_gps_set_rate(HILSIM_BAUD);
#else
		udb_gps_set_rate(9600);
#endif
	}
	else if (dcm_flags._.nmea_passthrough && gpscount == 200)
		gpsoutline(disable_GSV);
	else if (dcm_flags._.nmea_passthrough && gpscount == 190)
		gpsoutline(disable_GSA);
	else if (dcm_flags._.nmea_passthrough && gpscount == 180)
		gpsoutline(disable_GLL);
	else if (dcm_flags._.nmea_passthrough && gpscount == 170)
		gpsoutline(disable_VTG);
	else if (dcm_flags._.nmea_passthrough && gpscount == 160)
		// set the UBX to use binary and nmea
		gpsoutline(bin_mode_withnmea);
	else if (!dcm_flags._.nmea_passthrough && gpscount == 160)
		// set the UBX to use binary mode
		gpsoutline(bin_mode_nonmea);
#if (HILSIM != 1)
	else if (gpscount == 150)
		udb_gps_set_rate(19200);
#endif
	else if (gpscount == 140)
		gpsoutbin(set_rate_length, set_rate);
	else if (gpscount == 130)
		// command GPS to select which messages are sent, using UBX interface
		gpsoutbin(enable_NAV_SOL_length, enable_NAV_SOL);
	else if (gpscount == 120)
		gpsoutbin(enable_NAV_POSLLH_length, enable_NAV_POSLLH);
	else if (gpscount == 110)
		gpsoutbin(enable_NAV_VELNED_length, enable_NAV_VELNED);
	else if (gpscount == 100)
		gpsoutbin(enable_NAV_DOP_length, enable_NAV_DOP);
	else if (dcm_flags._.nmea_passthrough && gpscount == 90)
		gpsoutbin(enable_UBX_only_length, enable_UBX_NMEA);
	else if (!dcm_flags._.nmea_passthrough && gpscount == 90)
		gpsoutbin(enable_UBX_only_length, enable_UBX_only);
	else if (gpscount == 80)
		gpsoutbin(enable_SBAS_length, enable_SBAS);
	else if (gpscount == 70)
		gpsoutbin(config_NAV5_length, config_NAV5);
}

boolean gps_nav_valid(void)
{
	return (nav_valid_ == 3);
}

/*
int16_t hex_count = 0;
const char convert[] = "0123456789ABCDEF";
const char endchar = 0xB5;

// Used for debugging purposes, converts to HEX and outputs to the debugging USART
// Only the first 5 bytes following a B3 are displayed.
void hex_out(char outchar)
{
	if (hex_count > 0) 
	{
		U1TXREG = convert[((outchar>>4) & 0x0F)];
		U1TXREG = convert[(outchar & 0x0F)];
		U1TXREG = ' ';
		hex_count--;
	}
	if (outchar == endchar)
	{
		hex_count = 5;
		U1TXREG = '\r';
		U1TXREG = '\n';
	}
}
 */

// The parsing routines follow. Each routine is named for the state in which the routine is applied.
// States correspond to the portions of the binary messages.
// For example, msg_B3 is the routine that is applied to the byte received after a B3 is received.
// If an A0 is received, the state machine transitions to the A0 state.

void nmea_passthru(uint8_t gpschar)
{
	nmea_passthrough_char = gpschar;
	gpsoutbin(1, &nmea_passthrough_char);

	nmea_passthru_countdown--;
/* removed in favor of the line ending mechanism, see below. While this is compliant with
   (published) standards, the issue appears to center around the end of line.

	if (gpschar == '*')
	{ // * indicates the start of the checksum, 2 characters remain in the message
		nmea_passthru_countdown = 2;
	}
 */
	if (gpschar == 0x0A)
	{ // end of line appears to always be 0x0D, 0x0A (\r\n)
		msg_parse = &msg_B3; // back to the inital state
	}
	else if (nmea_passthru_countdown == 0)
	{
		msg_parse = &msg_B3; // back to the inital state
	}
}

static void msg_B3(uint8_t gpschar)
{
	if (gpschar == 0xB5)
	{
		//bin_out(0x01);
		msg_parse = &msg_SYNC1;
	}
	else if (dcm_flags._.nmea_passthrough && gpschar == '$' && udb_gps_check_rate(19200))
	{
		nmea_passthru_countdown = 128; // this limits the number of characters we will passthrough. (Most lines are 60-80 chars long.)
		msg_parse = &nmea_passthru;
		nmea_passthru (gpschar);
	}
	else
	{
		// error condition
	}
}

static void msg_SYNC1(uint8_t gpschar)
{
	if (gpschar == 0x62)
	{
		//bin_out(0x02);
		store_index = 0;
		msg_parse = &msg_SYNC2;
	}
	else
	{
		msg_parse = &msg_B3;        // error condition
	}
}

static void msg_SYNC2(uint8_t gpschar)
{
	//bin_out(0x03);
	msg_class = gpschar;
	CK_A = 0;
	CK_B = 0;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_CLASS;
}

static void msg_CLASS(uint8_t gpschar)
{
	//bin_out(0x04);
	msg_id = gpschar;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_ID;
}

static void msg_ID(uint8_t gpschar)
{
	//bin_out(0x05);
	payloadlength._.B0 = gpschar;   // UBX stored payload length in little endian order
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_PL1;
}

static void msg_PL1(uint8_t gpschar)
{
	//bin_out(0x06);
	payloadlength._.B1 = gpschar;   // UBX stored payload length in little endian order
	CK_A += gpschar;
	CK_B += CK_A;

	switch (msg_class) {
		case 0x01 : {
			switch (msg_id) {
				case 0x02 : { // NAV-POSLLH message
					if (payloadlength.BB  == NUM_POINTERS_IN(msg_POSLLH_parse))
					{
						msg_parse = &msg_POSLLH;
					}
					else
					{
						gps_parse_errors++;
						msg_parse = &msg_B3;    // error condition
					}
					break;
				}
				case 0x04 : { // NAV-DOP message
					if (payloadlength.BB  == NUM_POINTERS_IN(msg_DOP_parse))
					{
						msg_parse = &msg_DOP;
					}
					else
					{
						gps_parse_errors++;
						msg_parse = &msg_B3;    // error condition
					}
					break;
				}
				case 0x06 : { // NAV-SOL message
					if (payloadlength.BB  == NUM_POINTERS_IN(msg_SOL_parse))
					{
						msg_parse = &msg_SOL;
					}
					else
					{
						gps_parse_errors++;
						msg_parse = &msg_B3;    // error condition
					}
					break;
				}
				case 0x12 : { // NAV-VELNED message
					if (payloadlength.BB  == NUM_POINTERS_IN(msg_VELNED_parse))
					{
						msg_parse = &msg_VELNED;
					}
					else
					{
						gps_parse_errors++;
						msg_parse = &msg_B3;    // error condition
					}
					break;
				}
#if (HILSIM == 1)
				case 0xAB : { // NAV-BODYRATES message - THIS IS NOT AN OFFICIAL UBX MESSAGE
					// WE ARE FAKING THIS FOR HIL SIMULATION
					if (payloadlength.BB  == NUM_POINTERS_IN(msg_BODYRATES_parse))
					{
						msg_parse = &msg_BODYRATES;
					}
					else
					{
						msg_parse = &msg_B3;    // error condition
					}
					break;
				}
				case 0xAC : { // NAV-KEYSTROKE message - THIS IS NOT AN OFFICIAL UBX MESSAGE
					// WE ARE FAKING THIS FOR HIL SIMULATION
					if (payloadlength.BB ==  NUM_POINTERS_IN(msg_KEYSTROKE_parse))
					{
						msg_parse = &msg_KEYSTROKE;
					}
					else
					{
						printf("NAV-KEYSTROKE, bad payloadlength %i != %i\r\n", payloadlength.BB, (int)NUM_POINTERS_IN(msg_KEYSTROKE_parse));
						msg_parse = &msg_B3;    // error condition
					}
					break;
				}
#endif
				default : {     // some other NAV class message
					msg_parse = &msg_MSGU;
					break;
				}
			}
			break;
		}
		case 0x05 : {
			switch (msg_id) {
				case 0x00 : {   // NACK message
					ack_type = 0;
					msg_parse = &msg_ACK_CLASS;
					break;
				}
				case 0x01 : {   // ACK message
					ack_type = 1;
					msg_parse = &msg_ACK_CLASS;
					break;
				}
				default : {     // There are no other messages in this class, so this is an error
					msg_parse = &msg_B3;
					break;
				}
			}
			break;
		}
		default : {             // a non NAV class message
			msg_parse = &msg_MSGU;
			break;
		}
	}
}

static void msg_POSLLH(uint8_t gpschar)
{
	if (payloadlength.BB > 0)
	{
		*msg_POSLLH_parse[store_index++] = gpschar;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB--;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x08);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1;
	}
}

static void msg_DOP(uint8_t gpschar)
{
	if (payloadlength.BB > 0)
	{
		*msg_DOP_parse[store_index++] = gpschar;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB--;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x09);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1;
	}
}

static void msg_SOL(uint8_t gpschar)
{
	if (payloadlength.BB > 0)
	{
		*msg_SOL_parse[store_index++] = gpschar;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB--;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x0A);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1;
	}
}

static void msg_VELNED(uint8_t gpschar)
{
	if (payloadlength.BB > 0)
	{
		*msg_VELNED_parse[store_index++] = gpschar;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB--;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x0B);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1;
	}
}

#if (HILSIM == 1)
static void msg_BODYRATES(uint8_t gpschar)
{
	if (payloadlength.BB > 0)
	{
		*msg_BODYRATES_parse[store_index++] = gpschar;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB--;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1;
	}
}

static void msg_KEYSTROKE(uint8_t gpschar)
{
	if (payloadlength.BB > 0)
	{
		*msg_KEYSTROKE_parse[store_index++] = gpschar;
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB--;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1;
	}
}

#endif // HILSIM

static void msg_ACK_CLASS(uint8_t gpschar)
{
	//bin_out(0xAA);
	ack_class = gpschar;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_ACK_ID;
}

static void msg_ACK_ID(uint8_t gpschar)
{
	//bin_out(0xBB);
	ack_id = gpschar;
	CK_A += gpschar;
	CK_B += CK_A;
	msg_parse = &msg_CS0;
}

static void msg_MSGU(uint8_t gpschar)
{
	if (payloadlength.BB > 0)
	{
		CK_A += gpschar;
		CK_B += CK_A;
		payloadlength.BB--;
	}
	else
	{
		// If the payload length is zero, we have received the entire payload, or the payload length
		// was zero to start with. either way, the byte we just received is the first checksum byte.
		//gpsoutchar2(0x08);
		checksum._.B1 = gpschar;
		msg_parse = &msg_CS1;
	}
}

static void msg_CS0(uint8_t gpschar)
{
	checksum._.B1 = gpschar;
	msg_parse = &msg_CS1;
}

static void msg_CS1(uint8_t gpschar)
{
	checksum._.B0 = gpschar;
	if ((checksum._.B1 == CK_A) && (checksum._.B0 == CK_B))
	{
		if (msg_id == 0x12)
		{
			// correct checksum for VELNED message
			gps_parse_common(); // parsing is complete, schedule navigation
		}
#if (HILSIM == 1)
		else if (msg_id == 0xAB)
		{
			// If we got the correct checksum for bodyrates, commit that data immediately
			commit_bodyrate_data();
		}
		else if (msg_id == 0xAC)
		{
			// If we got the correct checksum for keystroke, commit that data immediately
			commit_keystroke_data();
		}
#endif
	}
	else
	{
		gps_parse_errors++;
		gps_data_age = GPS_DATA_MAX_AGE+1;  // if the checksum is wrong then the data from this packet is invalid. 
		                                    // setting this ensures the nav routine does not try to use this data.
	}
	msg_parse = &msg_B3;
}

void gps_update_basic_data(void)
{
	week_no         = week_no_;
	svs             = svs_;
}

void gps_commit_data(void)
{
	//bin_out(0xFF);
	week_no         = week_no_;
	tow             = tow_;
	lat_gps         = lat_gps_;
	lon_gps         = lon_gps_;
	alt_sl_gps.WW   = alt_sl_gps_.WW / 10;          // SIRF provides altMSL in cm, UBX provides it in mm
	sog_gps.BB      = sog_gps_._.W0;                // SIRF uses 2 byte SOG, UBX provides 4 bytes
#if (HILSIM == 1)
	hilsim_airspeed.BB = as_sim_._.W0;              // provided by HILSIM, simulated airspeed
#endif
	cog_gps.BB      = (uint16_t)(cog_gps_.WW / 1000);// SIRF uses 2 byte COG, 10^-2 deg, UBX provides 4 bytes, 10^-5 deg

	climb_gps.BB    = - climb_gps_._.W0;            // SIRF uses 2 byte climb rate, UBX provides 4 bytes
	hdop            = (uint8_t)(hdop_.BB / 20);     // SIRF scales HDOP by 5, UBX by 10^-2
	// SIRF provides position in m, UBX provides cm
//	xpg.WW          = xpg_.WW / 100;
//	ypg.WW          = ypg_.WW / 100;
//	zpg.WW          = zpg_.WW / 100;
	// SIRF provides 2 byte velocity in m scaled by 8,
	// UBX provides 4 bytes in cm
//	xvg.BB          = (int16_t)(xvg_.WW / 100 * 8);
//	yvg.BB          = (int16_t)(yvg_.WW / 100 * 8);
//	zvg.BB          = (int16_t)(zvg_.WW / 100 * 8);
//	mode1           = mode1_;
//	mode2           = mode2_;
	svs             = svs_;

#if (HILSIM == 1 && MAG_YAW_DRIFT == 1)
	HILSIM_MagData(mag_drift_callback); // run the magnetometer computations
#endif // HILSIM
}

#if (HILSIM == 1)
static void commit_bodyrate_data(void)
{
	g_a_x_sim = g_a_x_sim_;
	g_a_y_sim = g_a_y_sim_;
	g_a_z_sim = g_a_z_sim_;
	p_sim = p_sim_;
	q_sim = q_sim_;
	r_sim = r_sim_;
}

void HILSIM_saturate(int16_t size, int16_t vector[3])
{
	// hardware 16 bit signed integer gyro and accelerometer data and offsets
	// are divided by 2 prior to subtracting offsets from values.
	// This is done to prevent overflow.
	// However, it limits range to approximately +- RMAX.
	// Data coming in from Xplane is in 16 bit signed integer format with range +-2*RMAX,
	// so it needs to be passed through a saturation computation that limits to +-RMAX.
	uint16_t index;
	for (index = 0; index < size; index ++)
	{
		if (vector[index ] > RMAX)
		{
			vector[index] = RMAX;
		}
		if (vector[index] < -RMAX)
		{
			vector[index] = -RMAX;
		}
	}
}

void commit_keystroke_data(void)
{
//	if ((x_vkey_ != 0) && ((x_ckey_ & 0x08) || (x_ckey_ & 0x00))) // key down or key repeat
	if ((x_vkey_ != 0) && ((x_ckey_ & 0x08) || (x_ckey_ == 0x00))) // key down or key repeat
	{
/*
xplm_ShiftFlag      1   The shift key is down
xplm_OptionAltFlag  2   The option or alt key is down
xplm_ControlFlag    4   The control key is down*
xplm_DownFlag       8   The key is being pressed down
xplm_UpFlag         16  The key is being released
 */
//		printf("HILSIM keystroke %u %02x\r\n", x_vkey_, x_ckey_);
		hilsim_handle_key_input(x_vkey_);
	}
}

void HILSIM_set_gplane(fractional gplane[])
{
	gplane[0] = g_a_x_sim.BB;
	gplane[1] = g_a_y_sim.BB;
	gplane[2] = g_a_z_sim.BB;
	HILSIM_saturate(3, gplane);
}

void HILSIM_set_omegagyro(void)
{
	omegagyro[0] = q_sim.BB;
	omegagyro[1] = p_sim.BB;
	omegagyro[2] = r_sim.BB;
	HILSIM_saturate(3, omegagyro);
}
#endif // HILSIM

void init_gps_ubx(void)
{
}

#endif // (GPS_TYPE == GPS_UBX_2HZ || GPS_TYPE == GPS_UBX_4HZ || GPS_TYPE == GPS_ALL)
