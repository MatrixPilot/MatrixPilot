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


//#include "../MatrixPilot/defines.h" // TODO: remove, temporarily here for options to work correctly
#include "libDCM.h"
#include "gpsData.h"

// GPS MatrixPilot global variables (these are the GPS system outputs)
#ifdef USE_EXTENDED_NAV
struct relative3D_32 GPSlocation = { 0, 0, 0 };
#else
struct relative3D GPSlocation = { 0, 0, 0 };
#endif // USE_EXTENDED_NAV
struct relative3D GPSvelocity = { 0, 0, 0 };
int16_t cos_lat = 0;
int16_t gps_data_age;
int8_t actual_dir;
uint16_t ground_velocity_magnitudeXY = 0;
int16_t forward_acceleration = 0;
uint16_t air_speed_magnitudeXY = 0;
uint16_t air_speed_3DGPS = 0;
int8_t calculated_heading;           // takes into account wind velocity

//union longbbbb xpg, ypg, zpg;                     // gps x, y, z position
//union intbb    xvg, yvg, zvg;                     // gps x, y, z velocity
//uint8_t mode1, mode2;                             // gps mode1, mode2
volatile union intbb hilsim_airspeed;  // referenced in estWind and deadReckoning modules

union longbbbb lat_origin, lon_origin, alt_origin;  // (COULD THIS BETTER BE A VECTOR??)

///////////////////////////////////////////////////////////////////////////////
// WRAP ALL THIS UP INTO A STRUCTURE
volatile union longbbbb lat_gps, lon_gps, alt_sl_gps;        // latitude, longitude, altitude   (COULD THIS BETTER BE A VECTOR??)
volatile uint8_t hdop;                                       // horizontal dilution of precision
volatile uint8_t vdop;                                       // vertical dilution of precision
volatile uint8_t svs;    // referenced by telemetry and OSD modules  // number of satellites
// these are only exported for telemetry output
volatile union intbb week_no;
volatile union intbb sog_gps;                                // speed over ground
volatile union uintbb cog_gps;                               // course over ground
volatile union intbb climb_gps;                              // climb
volatile union longbbbb tow;
///////////////////////////////////////////////////////////////////////////////
// PASS THE STRUCTURE* TO THE gps_commit_data() functions

/*
void gps_commit_data(void) // MTEK
{
	week_no.BB = calculate_week_num(date_gps_.WW);
	tow.WW = calculate_time_of_week(time_gps_.WW);
	lat_gps.WW   = lat_gps_.WW * 10;
	lon_gps.WW   = lon_gps_.WW * 10;
	alt_sl_gps   = alt_sl_gps_;
	sog_gps.BB   = sog_gps_._.W0;
	cog_gps.BB   = cog_gps_._.W0;
	climb_gps.BB = (alt_sl_gps_.WW - last_alt.WW) * GPS_RATE;
	hdop         = (uint8_t)(hdop_.BB / 20);
	svs          = svs_;
}

void gps_commit_data(void) // NMEA
{
	week_no.BB = calculate_week_num(date_gps_.WW);
	tow.WW = calculate_time_of_week(time_gps_.WW);
	lat_gps      = lat_gps_;
	lon_gps      = lon_gps_;
	alt_sl_gps   = alt_sl_gps_;             // Altitude
	sog_gps      = sog_gps_;                // Speed over ground
	cog_gps      = cog_gps_;                // Course over ground
	climb_gps.BB = (alt_sl_gps_.WW - last_alt.WW) * GPS_RATE;
	hdop         = hdop_._.B0;
	svs          = svs_;
}

void gps_commit_data(void) // STD
{
	week_no     = week_no_;
	tow         = tow_;
	lat_gps     = lat_gps_;
	lon_gps     = lon_gps_;
	alt_sl_gps  = alt_sl_gps_;
	sog_gps     = sog_gps_;
	cog_gps     = cog_gps_;
	climb_gps   = climb_gps_;
	hdop        = hdop_._.B0;
	svs         = svs_;
}

void gps_commit_data(void) // UBX
{
	week_no         = week_no_;
	tow             = tow_;
	lat_gps         = lat_gps_;
	lon_gps         = lon_gps_;
	alt_sl_gps.WW   = alt_sl_gps_.WW / 10;          // SIRF provides altMSL in cm, UBX provides it in mm
	sog_gps.BB      = sog_gps_._.W0;                // SIRF uses 2 byte SOG, UBX provides 4 bytes
	cog_gps.BB      = (uint16_t)(cog_gps_.WW / 1000);// SIRF uses 2 byte COG, 10^-2 deg, UBX provides 4 bytes, 10^-5 deg
	climb_gps.BB    = - climb_gps_._.W0;            // SIRF uses 2 byte climb rate, UBX provides 4 bytes
	hdop            = (uint8_t)(hdop_.BB / 20);     // SIRF scales HDOP by 5, UBX by 10^-2
	// SIRF provides position in m, UBX provides cm
	svs             = svs_;
}
*/
