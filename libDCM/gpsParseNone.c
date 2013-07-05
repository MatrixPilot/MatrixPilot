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
#include "gpsParseCommon.h"
#include "../libUDB/heartbeat.h"

#if (GPS_TYPE == GPS_NONE)

static void msg_none(uint8_t gpschar)
{
}

static boolean gps_nav_valid_(void)
{
	return false;
}

static void gps_commit_data_(void)
{
// TODO: perhaps we need to set some of these variables?

//	week_no     = week_no_;
//	tow         = tow_;
//	lat_gps     = lat_gps_;
//	lon_gps     = lon_gps_;
//	alt_sl_gps  = alt_sl_gps_;
//	sog_gps     = sog_gps_;
//	cog_gps     = cog_gps_;
//	climb_gps   = climb_gps_;
//	hdop        = hdop_._.B0;
//	svs         = svs_;
}

static void gps_startup_sequence_(int16_t gpscount)
{
}

void init_gps_none(void)
{
	msg_parse = &msg_none;
	gps_nav_valid = &gps_nav_valid_;
	gps_commit_data = &gps_commit_data_;
	gps_startup_sequence = &gps_startup_sequence_;
}

#endif // (GPS_TYPE == GPS_NONE)
