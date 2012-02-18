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

//******************************************************************/
// DATA SERVICES TABLE
// Describes serialisation data collections, when to serialise and
//  callbacks to perform after serialisation
//

#include "libUDB_internal.h"

#if(USE_NV_MEMORY == 1)

#include "data_services.h"

// List of imu calibration variable items
const DATA_SERVICE_ITEM data_service_imu_calib_items[] =
{
	{ (unsigned char*) &udb_xaccel.offset,  sizeof(udb_xaccel.offset)},
	{ (unsigned char*) &udb_yaccel.offset,  sizeof(udb_yaccel.offset)},
	{ (unsigned char*) &udb_zaccel.offset,  sizeof(udb_zaccel.offset)},

	{ (unsigned char*) &udb_xrate.offset,	sizeof(udb_xrate.offset)},
	{ (unsigned char*) &udb_yrate.offset,	sizeof(udb_yrate.offset)},
	{ (unsigned char*) &udb_zrate.offset,	sizeof(udb_zrate.offset)},

	{ (unsigned char*) &udb_vref.offset,	sizeof(udb_vref.offset)},
};

#define DATA_SERVICE_IMU_CALIB_COUNT sizeof(data_service_imu_calib_items) / sizeof(DATA_SERVICE_ITEM)

// List of trim calibration variable items
const DATA_SERVICE_ITEM data_service_trim_calib_items[] =
{
//	{ (unsigned char*) udb_pwTrim,  sizeof(udb_pwTrim)},
};


const DATA_SERVICE_TABLE_ENTRY data_services_table[] = 
{
	{DATA_HANDLE_IMU_CALIB, 	&data_service_imu_calib_items[0], 	DATA_SERVICE_IMU_CALIB_COUNT , 	DATA_STORAGE_CHECKSUM_STRUCT,	DS_LOAD_AT_REBOOT,						NULL},
//	{DATA_HANDLE_TRIM_CALIB, 	&data_service_trim_calib_items[0], 	1, 								DATA_STORAGE_CHECKSUM_STRUCT,	DS_LOAD_AT_STARTUP | DS_LOAD_AT_REBOOT,	NULL},
};

const unsigned int data_service_table_count = sizeof(data_services_table) / sizeof(DATA_SERVICE_TABLE_ENTRY);

#endif		//#if(USE_NV_MEMORY == 1)
