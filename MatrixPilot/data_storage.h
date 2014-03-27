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


#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

//#include "../libUDB/libUDB_internal.h"

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

// callback type for data storage user
typedef void (*DS_callbackFunc)(boolean);

// Trigger storage service in low priority process.
void storage_service_trigger(void);

// Initialise the data storage
void data_storage_init(void);

// For access to a checksummed structure data area
boolean storage_write(uint16_t data_handle, uint8_t* pwrData, uint16_t size, DS_callbackFunc callback);
boolean storage_read(uint16_t data_handle, uint8_t* prdData, uint16_t size, DS_callbackFunc callback);

// Create a storage area
// Size = size in bytes
// type = data management type
// callback = user callback for when process finished
boolean storage_create_area(uint16_t data_handle, uint16_t size, uint16_t type, DS_callbackFunc callback);

// Lookup the data storage table to see if an area exists
// Does not require callback.  Always has immediate return
boolean storage_check_area_exists(uint16_t data_handle, uint16_t size, uint16_t type);

// Status of sotage services
boolean storage_services_started(void);

// For access to a checksummed array data area
//extern boolean storage_write_array(uint16_t data_handle, uint8_t* pwrData, uint16_t size, DS_callbackFunc callback);
//extern boolean storage_read_array(uint16_t data_handle, uint8_t* prdData, uint16_t size, DS_callbackFunc callback);

// For random access to a self managed area
//extern boolean storage_write_part(uint16_t data_handle, uint8_t* pwrData, uint16_t offset, uint16_t size, DS_callbackFunc callback);
//extern boolean storage_read_part(uint16_t data_handle, uint8_t* prdData, uint16_t offset, uint16_t size, DS_callbackFunc callback);

//extern boolean storage_resize(uint16_t data_handle, uint16_t size, DS_callbackFunc callback);

// Clear all data storage areas by invalidating data
//extern boolean storage_clear_all(DS_callbackFunc callback);

// Clear specific data storage area by invalidating data
boolean storage_clear_area(uint16_t data_handle, DS_callbackFunc callback);

#endif // DATA_STORAGE_H
