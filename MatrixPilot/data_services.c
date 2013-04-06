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
// DATA SERVICES
// 
// Data Services
// Responsible for serialisation of sets of variables
// Reponsible for recalling sets data storage on startup
// Reponsible for initialising data storage at startup
// 
//

#include "../libUDB/libUDB.h"

#if(USE_NV_MEMORY == 1)


#include "data_services.h"
#include "../libUDB/events.h"
#include "parameter_table.h"
#include <string.h>

// Data buffer used for services
uint8_t data_services_buffer[DATA_SERVICE_BUFFER_SIZE];

// callback type for data services user
DSRV_callbackFunc data_services_user_callback = NULL;

// All the states of the service
typedef enum 
{
	DATA_SERVICE_STATE_NOT_STARTED,
	DATA_SERVICE_STATE_INIT_ALL	,			// Initialise memory areas for all table entries
	DATA_SERVICE_STATE_INIT_WAIT ,			// Waiting for initialise of a table entry
	DATA_SERVICE_STATE_INIT ,				// Initialise the next area
	DATA_SERVICE_STATE_WAITING,				// Ready and waiting
	DATA_SERVICE_STATE_WRITE,				// A single write
	DATA_SERVICE_STATE_WRITE_WAITING,		// Waiting for write to complete
	DATA_SERVICE_STATE_READ,				// Start a read of an area
	DATA_SERVICE_STATE_READ_ALL,			// Start a read of all areas in the table
	DATA_SERVICE_STATE_READ_WAITING,		// Waiting for read to complete
	DATA_SERVICE_STATE_READ_DONE,			// Done reading, check and then commit it to ram.
//	DATA_SERVICE_STATE_WRITE_ALL,			// Write all
} DATA_SERVICE_STATE;

// Flag to show that the action is being done with all areas.
boolean data_services_do_all_areas = false;

// service state variable
uint16_t data_service_state = DATA_SERVICE_STATE_NOT_STARTED;

// Handle to event that will trigger the low priority service routine
uint16_t data_service_event_handle = INVALID_HANDLE;

// Low priority service routine
void data_services(void);

// Start init of all memory areas in the services table
void data_services_init_all(void);

// Service intialisation of memory areas starting at the tracking index
void data_services_init_table_index(void);

// Callback for completetion of memory area init
void data_services_init_all_callback(boolean success);

// Start of reading all areas in the table matching serialize flags
void data_services_read_all( void );

// Start of writing all areas in the table matching serialize flags
void data_services_write_all( void );

// Start of read a single area at index
void data_services_read_index( void );

// callback for reading areas in the table
void data_services_read_callback(boolean success);

// When finished reading an area, call to commit area to ram
void data_services_read_done( void );

// Serialise a list of data items/variables to the buffer
// returns size of items serialised
uint16_t serialise_items_to_buffer(uint16_t table_index);

// Serialise the buffer to a list of data items/variables
// returns size of items serialised
uint16_t serialise_buffer_to_items(uint16_t table_index);

// Start the write
void data_services_write( void );

// Write callback
void data_services_write_callback( boolean success );

// Get the index in the nv memory table for the storage handle.
uint16_t data_services_get_table_index(uint16_t data_storage_handle);

// Calculate the size of the complete list of variable items in bytes
uint16_t data_services_calc_item_size(uint16_t table_index);

// Tracking index into table
uint16_t data_services_table_index = 0;

// Flags that determine how and when to do serialisation
uint16_t data_services_serialize_flags = 0;


void data_services_init(void)
{
	if(data_service_state != DATA_SERVICE_STATE_NOT_STARTED) return;
	if( (data_service_event_handle = register_event(&data_services)) == INVALID_HANDLE)
		return;
	data_service_state = DATA_SERVICE_STATE_INIT;
};


// Low priority service routine trigger
inline void data_services_trigger(void)
{
	trigger_event(data_service_event_handle);
}

void data_services(void)
{
	switch(data_service_state)
	{
	case  DATA_SERVICE_STATE_INIT:
		data_services_init_all();
		break;
	case  DATA_SERVICE_STATE_INIT_ALL:
		data_services_init_table_index();
		break;
//	case DATA_SERVICE_STATE_READ_ALL:
//		data_services_read_all();
		break;
	case DATA_SERVICE_STATE_READ:
		data_services_read_index();
		break;
	case DATA_SERVICE_STATE_READ_DONE:
		data_services_read_done();
		break;
	case DATA_SERVICE_STATE_WRITE:
		data_services_write();
		break;
//	case DATA_SERVICE_STATE_WRITE_ALL:
//		data_services_write_all();
		break;
	}
}

void data_services_init_all(void)
{
	data_services_table_index = 0;
	data_services_do_all_areas = true;
	data_service_state =	DATA_SERVICE_STATE_INIT_ALL;
}


void data_services_init_table_index(void)
{
	while(data_services_table_index < mavlink_parameter_block_count)
	{
		if(storage_services_started())
		{
			if ( storage_check_area_exists(
				mavlink_parameter_blocks[data_services_table_index].data_storage_area,
				data_services_calc_item_size(data_services_table_index),
				DATA_STORAGE_CHECKSUM_STRUCT) == true)
			{
				data_services_table_index++;
			}
			else
			{
				// Storage area does not exist so request to create it
				if(storage_create_area(	mavlink_parameter_blocks[data_services_table_index].data_storage_area, 
										data_services_calc_item_size(data_services_table_index), 
										DATA_STORAGE_CHECKSUM_STRUCT, 
										&data_services_init_all_callback) == true)
				{
					// If the request succeeds, wait for it to complete
					data_service_state = DATA_SERVICE_STATE_INIT_WAIT;
					return;
				}
				else
				{
					// if the request failed, try again later.
					return;
				}
			}	
		}
		else return;
	}

	data_services_table_index = 0;
	data_services_do_all_areas = true;
	data_services_user_callback = NULL;

	if(_SWR == 0)
		data_services_serialize_flags = STORAGE_FLAG_LOAD_AT_STARTUP;
	else
		data_services_serialize_flags = STORAGE_FLAG_LOAD_AT_REBOOT;

	data_service_state = DATA_SERVICE_STATE_READ;
}


void data_services_init_all_callback(boolean success)
{
	if(success) data_services_table_index++;
	data_service_state =	DATA_SERVICE_STATE_INIT_ALL;
}


// Calculate the size of the complete list of variable items in bytes
uint16_t data_services_calc_item_size(uint16_t table_index)
{
	uint16_t item_index;
	uint16_t size = 0;
	uint16_t start_index = mavlink_parameter_blocks[table_index].block_start_index;
	uint16_t count = mavlink_parameter_blocks[table_index].block_size;

	for(item_index = 0; item_index < count; item_index++)
	{
		size += mavlink_parameters_list[start_index + item_index].param_size;
	}
	return size;
}


// Read data area at index
void data_services_read_index( void )
{

	// If beyond end of table return to waiting.
	if(data_services_table_index >= mavlink_parameter_block_count)
	{
		data_service_state =	DATA_SERVICE_STATE_WAITING;
		if(data_services_user_callback != NULL) data_services_user_callback(true);
		data_services_user_callback = NULL;
		return;
	}

	uint16_t service_flags = mavlink_parameter_blocks[data_services_table_index].data_storage_flags;

	// Check the serialise flags to see if this table entry should be loaded
	if( (service_flags & data_services_serialize_flags) | (data_services_serialize_flags & STORAGE_FLAG_ALL) )
	{
		uint16_t handle = mavlink_parameter_blocks[data_services_table_index].data_storage_area;
		uint16_t size = data_services_calc_item_size(data_services_table_index);
		uint16_t type = DATA_STORAGE_CHECKSUM_STRUCT; //mavlink_parameter_blocks[data_services_table_index].data_type;
	
		// TODO: Check here if data handle is ok 
	
		if(type == DATA_STORAGE_CHECKSUM_STRUCT)
		{
			if(storage_read(handle, data_services_buffer, size, &data_services_read_callback) == true)
			{
				data_service_state =	DATA_SERVICE_STATE_READ_WAITING;
			}
			return;
		}
	}
	else
	{
		if(data_services_do_all_areas == true)
			data_services_table_index++;
		else
		{
			if(data_services_user_callback != NULL) data_services_user_callback(false);
			data_services_user_callback = NULL;
			data_service_state = DATA_SERVICE_STATE_WAITING;
		}
	}
}

// Request to save all memory areas from the table which match the serialize flags
// return true if services not busy and request can be serviced
boolean data_services_save_all( uint16_t serialize_flags, DSRV_callbackFunc pcallback)
{
	if(data_service_state !=	DATA_SERVICE_STATE_WAITING) return false;

	data_services_serialize_flags 	= serialize_flags;
	data_services_user_callback 	= pcallback;
	data_services_table_index 		= 0;
	data_services_do_all_areas 		= true;
	data_service_state 				= DATA_SERVICE_STATE_WRITE;

	return true;
}


// Load a data area to nv memory with the given handle.
// Return true if services available to take request, otherwise return false
boolean data_services_load_specific(uint16_t data_storage_handle, DSRV_callbackFunc pcallback)
{
	if(data_service_state != DATA_SERVICE_STATE_WAITING) return false;

	data_services_table_index = data_services_get_table_index(data_storage_handle);
	if(data_services_table_index == INVALID_HANDLE) return false;
	
	data_services_user_callback = pcallback;
	data_services_do_all_areas = false;					// One area only
	data_services_serialize_flags = STORAGE_FLAG_ALL;	// Flag to read regardless of flags
	data_service_state = DATA_SERVICE_STATE_READ;
	
	return false;
}


// Request to load all memory areas from the table which match the serialize flags
void data_services_load_all(  uint16_t serialize_flags, DSRV_callbackFunc pcallback )
{
	if(data_service_state !=	DATA_SERVICE_STATE_WAITING) return;

	data_services_serialize_flags 	= serialize_flags;
	data_services_user_callback		= pcallback;
	data_services_table_index 		= 0;
	data_services_do_all_areas 		= true;
	data_service_state 				= DATA_SERVICE_STATE_READ;
}



// Data is correct so serialise it from the buffer to the live data
void data_services_read_done( void )
{
	serialise_buffer_to_items(data_services_table_index);

	if(mavlink_parameter_blocks[data_services_table_index].ploadCallback != NULL)
	{
		mavlink_parameter_blocks[data_services_table_index].ploadCallback(true);
	}

	if(data_services_do_all_areas == false)
	{
		if(data_services_user_callback != NULL) data_services_user_callback(true);
		data_services_user_callback = NULL;
		data_service_state = DATA_SERVICE_STATE_WAITING;
		return;
	}

	data_services_table_index++;
	data_service_state =	DATA_SERVICE_STATE_READ;
}


// Called when storage manager data read has finished
void data_services_read_callback(boolean success)
{
	if(success)
		data_service_state = DATA_SERVICE_STATE_READ_DONE;
	else
		if(data_services_do_all_areas == true)
		{
			data_services_table_index++;
			data_service_state =	DATA_SERVICE_STATE_READ;
		}
		else
		{
			if(data_services_user_callback != NULL) data_services_user_callback(false);
			data_services_user_callback = NULL;
			data_service_state =	DATA_SERVICE_STATE_WAITING;
		}
	return;
}


// Serialise a list of data items/variables to the buffer
// returns total size of the items
uint16_t serialise_items_to_buffer(uint16_t table_index)
{
	if(table_index >= mavlink_parameter_block_count) return 0;

	const mavlink_parameter* 	pParameter;
	const uint8_t*		pData;

	uint16_t 	item_index;
	uint16_t 	buffer_index = 0;
	uint16_t 	item_size;
	uint16_t 	block_start = mavlink_parameter_blocks[table_index].block_start_index;
	uint16_t 	item_count = mavlink_parameter_blocks[table_index].block_size;

	for(item_index = 0; item_index < item_count; item_index++)
	{
		pParameter 	= &mavlink_parameters_list[block_start + item_index];
		pData 		= pParameter->pparam;
		item_size 	= pParameter->param_size;

		if( (buffer_index + item_size) > DATA_SERVICE_BUFFER_SIZE )
			return 0;
		memcpy( &data_services_buffer[buffer_index], pData, item_size);
		buffer_index += item_size;
	}
	return buffer_index;
}


// Serialise the buffer to a list of data items/variables
// returns total size of the items
uint16_t  serialise_buffer_to_items(uint16_t table_index)
{
	if(table_index >= mavlink_parameter_block_count) return 0;

	const mavlink_parameter* 	pParameter;
	const uint8_t*		pData;

	uint16_t 	item_index;
	uint16_t 	buffer_index = 0;
	uint16_t 	item_size;
	uint16_t 	block_start = mavlink_parameter_blocks[table_index].block_start_index;
	uint16_t 	item_count = mavlink_parameter_blocks[table_index].block_size;

	for(item_index = 0; item_index < item_count; item_index++)
	{
		pParameter 	= &mavlink_parameters_list[block_start + item_index];
		pData 		= pParameter->pparam;
		item_size 	= pParameter->param_size;

		if( (buffer_index + item_size) > DATA_SERVICE_BUFFER_SIZE )
			return 0;
		memcpy( (uint8_t*) pData, &data_services_buffer[buffer_index], item_size);
		buffer_index += item_size;
	}
	return buffer_index;
}


uint16_t data_services_get_table_index(uint16_t data_storage_handle)
{
	int16_t index;

	for(index = 0; index < mavlink_parameter_block_count; index++)
	{
		if(mavlink_parameter_blocks[index].data_storage_area == data_storage_handle)
		{
			return index;
		}
	}

	return INVALID_HANDLE;
}


boolean data_services_save_specific(uint16_t data_storage_handle, DSRV_callbackFunc pcallback)
{
	if(data_service_state != DATA_SERVICE_STATE_WAITING) return false;

	data_services_table_index = data_services_get_table_index(data_storage_handle);
	if(data_services_table_index == INVALID_HANDLE) return false;
	
	data_services_user_callback = pcallback;
	data_services_do_all_areas = false;				// One area only
	data_services_serialize_flags = STORAGE_FLAG_ALL;	// Flag to write regardless of flags

	data_service_state = DATA_SERVICE_STATE_WRITE;
	
	return false;
}


// Start the write of a checksummed structure
void data_services_write( void )
{
	if(data_services_table_index >= mavlink_parameter_block_count)
	{
		if(data_services_user_callback != NULL) data_services_user_callback(true);
		data_services_user_callback = NULL;
		data_service_state = DATA_SERVICE_STATE_WAITING;
		return;
	}
	
	uint16_t size = serialise_items_to_buffer(data_services_table_index);

	if(size == 0)
	{
		if(data_services_user_callback != NULL) data_services_user_callback(false);
		data_services_user_callback = NULL;
		data_service_state = DATA_SERVICE_STATE_WAITING;
		return;
	}

	uint16_t handle = mavlink_parameter_blocks[data_services_table_index].data_storage_area;
	//data_services_calc_item_size(data_services_table_index);
	uint16_t type = DATA_STORAGE_CHECKSUM_STRUCT;

	// TODO: Check here if data handle is ok
	//storage_check_area_exists

	if( (type == DATA_STORAGE_CHECKSUM_STRUCT) && 
		( (data_services_serialize_flags & mavlink_parameter_blocks[data_services_table_index].data_storage_flags) ||
		  (data_services_serialize_flags & STORAGE_FLAG_ALL) ) )
	{
		if(storage_write(handle, data_services_buffer, size, &data_services_write_callback) == true)
		{
			data_service_state =	DATA_SERVICE_STATE_WRITE_WAITING;
			return;
		}
	}
	else
	{
		if(data_services_do_all_areas == true)
			data_services_table_index++;
		else
		{
			if(data_services_user_callback != NULL) data_services_user_callback(false);
			data_services_user_callback = NULL;
			data_service_state = DATA_SERVICE_STATE_WAITING;
		}
	}
}


// Write callback
void data_services_write_callback( boolean success )
{
	if(success)
	{
		if(data_services_do_all_areas == true)
		{
			data_services_table_index++;
			data_service_state =	DATA_SERVICE_STATE_WRITE;
		}
		else
		{
			if(data_services_user_callback != NULL) data_services_user_callback(true);
			data_service_state =	DATA_SERVICE_STATE_WAITING;
		}
	}
	else
	{
		if(data_services_user_callback != NULL) data_services_user_callback(false);
		data_service_state = DATA_SERVICE_STATE_WAITING;
	}
}

#endif	//#if(USE_NV_MEMORY == 1)


