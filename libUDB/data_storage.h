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
// DATA STORAGE
// A lightweight file system that allows data versioning
// Does checksums on data structures or data arrays
// Has a basic directory/FAT table describing data areas
// Data areas do not have names, only the handles defined below
// 
//
//
//


#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include "libUDB_internal.h"

// CONFIGURATION OPTIONS

// Defines the chunks size for data areas.  Can be used to allign with
// NV memory page size.
#define FAT_CHUNK_BYTE_SIZE		64

// Size of data pre-amble used to mark the start of a memory area
#define DATA_PREAMBLE_SIZE 		4

// Maximum number of data areas
#define MAX_DATA_HANDLES		30

// callback type for data storage user
typedef void (*DS_callbackFunc)(boolean);

// A constant preamble used to determine the start of a data block
// This also allows the data to be found if the FAT is broken
extern const unsigned char data_storage_preamble[DATA_PREAMBLE_SIZE];

// A constant preamble used to determine the start of the data storage table
extern const unsigned char table_storage_preamble[DATA_PREAMBLE_SIZE];

// Data storage types defines the way in which the storage area works
//
// NULL - Nothing there
// CHECKSUM_STRUCT - A single structure with a checksum
// HEADER_NO_CHECKSUM - A header to start the area but no checksum use
// CHECKSUMMED_ARRAY - Chuncks of data with a checksum for each chunck
// SELF_MANAGED - No header or checksum. Only checks out of bounds writing
typedef enum
{
	DATA_STORAGE_NULL,
	DATA_STORAGE_CHECKSUM_STRUCT,
	DATA_STORAGE_HEADER_NO_CHECKSUM,
	DATA_STORAGE_CHECKSUMMED_ARRAY,
	DATA_STORAGE_SELF_MANAGED,
} DATA_STORAGE_TYPE;



// CAUTION - README FIRST
// NEVER INSERT A NEW HANDLE INTO THE LIST, ONLY EVER EXTEND THE DATA HANDLE LIST
// Handle list to data storage areas.
typedef enum
{
	DATA_HANDLE_NULL,
	DATA_HANDLE_FAILURE_DATALOG,	// A datalog of failures
	DATA_HANDLE_IMU_CALIB,			// IMU calibration data
	DATA_HANDLE_MAG_CALIB,			// Magnetomenter calibration data
	DATA_HANDLE_ANALOG_CALIB,		// Analog calibration data
	DATA_HANDLE_MIXER_SETTINGS,		// Mixer settings
	DATA_HANDLE_RTL_SETTINGS,		// Return to launch settings
	DATA_HANDLE_WAYPOINTS,			// Waypoints
	DATA_HANDLE_OSD_SETUP,
	DATA_HANDLE_TRIM_CALIB,
	DATA_HANDLE_MAX				// MUST BE AT END.  NEW HANDLES JUST ABOVE HERE.
} DATA_STORAGE_HANDLES;


#if(DATA_HANDLE_MAX >= MAX_DATA_HANDLES)
	#error("Number of defined data handles exceeds the maximum number of defined handles")
#endif


// Structure for entry to data directory array
typedef struct tagDATA_STORAGE_ENTRY
{
	unsigned int data_address;
	unsigned int data_type;
	unsigned int data_size;
} DATA_STORAGE_ENTRY;


// Structure of complete data directory including checksum.
typedef struct tagDATA_STORAGE_TABLE
{
	unsigned char 		table_preamble[DATA_PREAMBLE_SIZE];	
	DATA_STORAGE_ENTRY	table[MAX_DATA_HANDLES];
	unsigned int		table_checksum;
} DATA_STORAGE_TABLE;


typedef struct tagDATA_STORAGE_HEADER
{
	unsigned char 	data_preamble[DATA_PREAMBLE_SIZE];
	unsigned int 	data_handle;
	unsigned int 	data_version;
	unsigned int 	data_checksum;
} DATA_STORAGE_HEADER;

// Trigger storage service in low priority process.
extern void storage_service_trigger(void);

// Initialise the data storage
extern void data_storage_init(void);

// For access to a checksummed structure data area
extern boolean storage_write(unsigned int data_handle, unsigned char* pwrData, unsigned int size, DS_callbackFunc callback);
extern boolean storage_read(unsigned int data_handle, unsigned char* prdData, unsigned int size, DS_callbackFunc callback);

// Create a storage area
// Size = size in bytes
// type = data management type
// callback = user callback for when process finished
extern boolean storage_create_area(unsigned int data_handle, unsigned int size, unsigned int type, DS_callbackFunc callback);

// Lookup the data storage table to see if an area exists
// Does not require callback.  Always has immediate return
extern boolean storage_check_area_exists(unsigned int data_handle, unsigned int size, unsigned int type);

// Status of sotage services
boolean storage_services_started();

// For access to a checksummed array data area
extern boolean storage_write_array(unsigned int data_handle, unsigned char* pwrData, unsigned int size, DS_callbackFunc callback);
extern boolean storage_read_array(unsigned int data_handle, unsigned char* prdData, unsigned int size, DS_callbackFunc callback);


// For random access to a self managed area
extern boolean storage_write_part(unsigned int data_handle, unsigned char* pwrData, unsigned int offset, unsigned int size, DS_callbackFunc callback);
extern boolean storage_read_part(unsigned int data_handle, unsigned char* prdData, unsigned int offset, unsigned int size, DS_callbackFunc callback);

extern boolean storage_resize(unsigned int data_handle, unsigned int size, DS_callbackFunc callback);

// Clear all data storage areas by invalidating data
extern boolean storage_clear_all(DS_callbackFunc callback);

// Clear specific data storage area by invalidating data
extern boolean storage_clear_area(unsigned int data_handle, DS_callbackFunc callback);

#endif
