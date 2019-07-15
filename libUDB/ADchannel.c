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
#include "../libDCM/libDCM.h"


struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
struct ADchannel udb_xrate,  udb_yrate,  udb_zrate;  // x, y, and z gyro channels
struct ADchannel udb_vref; // reference voltage (deprecated, here for MAVLink compatibility)

#if (BOARD_TYPE == UDB4_BOARD) 
// CUSTOM_OFFSETS only used for the Accelerometers with the UDB4 Board
void udb_a2d_record_offsets(void)
{
#if (USE_NV_MEMORY == 1)
	if (udb_skip_flags.skip_imu_cal == 1)
		return;
#endif

#ifdef CUSTOM_OFFSETS
	// offsets have been measured manually and entered into the options.h file
	udb_xaccel.offset = XACCEL_OFFSET;
	udb_yaccel.offset = YACCEL_OFFSET;
	udb_zaccel.offset = ZACCEL_OFFSET;
#else
#ifdef INITIALIZE_VERTICAL // for VTOL, vertical initialization
	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value;
	UDB_YACCEL.offset = UDB_YACCEL.value - (Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY))); // opposite direction
	UDB_ZACCEL.offset = UDB_ZACCEL.value;	
#else
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

#else

// For Non UDB4 Boards Use CUSTOM_OFFSETS for both Accelerometers and Gyros
void udb_a2d_record_offsets(void)
{
#if (USE_NV_MEMORY == 1)
	if (udb_skip_flags.skip_imu_cal == 1)
		return;
#endif

#ifdef CUSTOM_OFFSETS
	// offsets have been measured manually and entered into the options.h file
	udb_xaccel.offset = XACCEL_OFFSET;
	udb_yaccel.offset = YACCEL_OFFSET;
	udb_zaccel.offset = ZACCEL_OFFSET;
	udb_xrate.offset   = XRATE_OFFSET;
	udb_yrate.offset  = YRATE_OFFSET;
	udb_zrate.offset  = ZRATE_OFFSET;
	
#else
	
#ifdef INITIALIZE_VERTICAL // for VTOL, vertical initialization
	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value;
	UDB_YACCEL.offset = UDB_YACCEL.value - (Y_GRAVITY_SIGN ((int16_t)(ACCEL_RANGE/2*GRAVITY))); // opposite direction
	UDB_ZACCEL.offset = UDB_ZACCEL.value;	
#else
	// almost ready to turn the control on, save the input offsets
		UDB_XACCEL.offset = __builtin_divsd(udb_xaccel.sum, DCM_CALIB_COUNT);
		UDB_YACCEL.offset = __builtin_divsd(udb_yaccel.sum, DCM_CALIB_COUNT);
		UDB_ZACCEL.offset = __builtin_divsd(udb_zaccel.sum, DCM_CALIB_COUNT)+ (Z_GRAVITY_SIGN ((int16_t)(ACCEL_RANGE/2*GRAVITY))); // same direction;
#endif // INITIALIZE_VERTICAL

		udb_xrate.offset  = __builtin_divsd(udb_xrate.sum,  DCM_CALIB_COUNT);
		udb_yrate.offset  = __builtin_divsd(udb_yrate.sum,  DCM_CALIB_COUNT);
		udb_zrate.offset  = __builtin_divsd(udb_zrate.sum,  DCM_CALIB_COUNT);
	
#endif // CUSTOM_OFFSETS
	
#ifdef VREF
	udb_vref.offset   = udb_vref.value;
#endif 
}

#endif //(BOARD_TYPE == (UDB4_BOARD)
