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


#include "libUDB.h"
#include "ADchannel.h"


struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
struct ADchannel udb_xrate,  udb_yrate,  udb_zrate;  // x, y, and z gyro channels
struct ADchannel udb_vref; // reference voltage (deprecated, here for MAVLink compatibility)

#ifdef UDB4
void udb_init_gyros(void);
#endif


void udb_a2d_record_offsets(void)
{
#ifdef UDB4
	udb_init_gyros();
#endif
#if (USE_NV_MEMORY == 1)
	if (udb_skip_flags.skip_imu_cal == 1)
		return;
#endif
#ifdef CUSTOM_OFFSETS
	// offsets have been measured manually and entered into the options.h file	
	udb_xaccel.offset = XACCEL_OFFSET;
	udb_yaccel.offset = YACCEL_OFFSET;
	udb_zaccel.offset = ZACCEL_OFFSET;
	
#else   // Measure offsets from sensor values now
#ifdef INITIALIZE_VERTICAL // for VTOL, vertical initialization
	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value;
	UDB_YACCEL.offset = UDB_YACCEL.value - (Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // opposite direction
	UDB_ZACCEL.offset = UDB_ZACCEL.value;
#else	// horizontal initialization
	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value;
	UDB_YACCEL.offset = UDB_YACCEL.value;
	UDB_ZACCEL.offset = UDB_ZACCEL.value + (Z_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // same direction
#endif // INITIALIZE_VERTICAL	
	
#endif // CUSTOM_OFFSETS
	
	udb_xrate.offset  = udb_xrate.value;
	udb_yrate.offset  = udb_yrate.value;
	udb_zrate.offset  = udb_zrate.value;
	
#ifdef VREF
	udb_vref.offset   = udb_vref.value;
#endif
}

