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

#include "libUDB.h"

#if(USE_NV_MEMORY == 1)

#include "data_services.h"
#include "events.h"
#include <string.h>

// Data buffer used for services
unsigned char data_services_buffer[DATA_SERVICE_BUFFER_SIZE];

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
unsigned int data_service_state = DATA_SERVICE_STATE_NOT_STARTED;

// Handle to event that will trigger the low priority service routine
unsigned int data_service_event_handle = INVALID_HANDLE;

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
unsigned int serialise_items_to_buffer(unsigned int table_index);

// Serialise the buffer to a list of data items/variables
// returns size of items serialised
unsigned int serialise_buffer_to_items(unsigned int table_index);

// Start the write
void data_services_write( void );

// Write callback
void data_services_write_callback( boolean success );


// Calculate the size of the complete list of variable items in bytes
unsigned int data_services_calc_item_size(unsigned int table_index);

// Tracking index into table
unsigned int data_services_table_index = 0;

// Flags that determine how and when to do serialisation
unsigned int data_services_serialize_flags = 0;


void data_services_init(void)
{
	if(data_service_state != DATA_SERVICE_STATE_NOT_STARTED) return;
	if( (data_service_event_handle = data_service_event_handle = register_event(&data_services)) == INVALID_HANDLE)
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
	while(data_services_table_index < data_service_table_count)
	{
		if(storage_services_started())
		{
			if ( storage_check_area_exists(
						data_services_table[data_services_table_index].data_storage_handle,
						data_services_calc_item_size(data_services_table_index),
						data_services_table[data_services_table_index].data_type) == true)
			{
				data_services_table_index++;
			}
			else
			{
				// Storage area does not exist so request to create it
				if(storage_create_area(	data_services_table[data_services_table_index].data_storage_handle, 
										data_services_calc_item_size(data_services_table_index), 
										data_services_table[data_services_table_index].data_type, 
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
		data_services_serialize_flags = DS_LOAD_AT_STARTUP;
	else
		data_services_serialize_flags = DS_LOAD_AT_REBOOT;

	data_service_state = DATA_SERVICE_STATE_READ;
}


void data_services_init_all_callback(boolean success)
{
	if(success) data_services_table_index++;
	data_service_state =	DATA_SERVICE_STATE_INIT_ALL;
}


// Calculate the size of the complete list of variable items in bytes
unsigned int data_services_calc_item_size(unsigned int table_index)
{
	unsigned int item_index;
	unsigned int size = 0;

	for(item_index = 0; item_index < data_services_table[table_index].item_count; item_index++)
	{
		size += data_services_table[table_index].pItem[item_index].size;
	}
	return size;
}


// Read data area at index
void data_services_read_index( void )
{

	// If beyond end of table return to waiting.
	if(data_services_table_index >= data_service_table_count)
	{
		data_service_state =	DATA_SERVICE_STATE_WAITING;
		if(data_services_user_callback != NULL) data_services_user_callback(true);
		data_services_user_callback = NULL;
		return;
	}

	unsigned int service_flags = data_services_table[data_services_table_index].service_flags;

	// Check the serialise flags to see if this table entry should be loaded
	if( (service_flags & data_services_serialize_flags) | (service_flags & DS_LOAD_ALL) )
	{
		unsigned int handle = data_services_table[data_services_table_index].data_storage_handle;
		unsigned int size = data_services_calc_item_size(data_services_table_index);
		unsigned int type = data_services_table[data_services_table_index].data_type;
	
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

	data_services_table_index++;
}

// Request to save all memory areas from the table which match the serialize flags
// return true if services not busy and request can be serviced
boolean data_services_save_all( unsigned int serialize_flags, DSRV_callbackFunc pcallback)
{
	if(data_service_state !=	DATA_SERVICE_STATE_WAITING) return false;

	data_services_serialize_flags 	= serialize_flags;
	data_services_user_callback 	= pcallback;
	data_services_table_index 		= 0;
	data_services_do_all_areas 		= true;
	data_service_state 				= DATA_SERVICE_STATE_WRITE;

	return true;
}


// Request to load all memory areas from the table which match the serialize flags
void data_services_load_all(  unsigned int serialize_flags, DSRV_callbackFunc pcallback )
{
	if(data_service_state !=	DATA_SERVICE_STATE_WAITING) return;

	data_services_serialize_flags 	= serialize_flags;
	data_services_user_callback		= pcallback;
	data_services_table_index 		= 0;
	data_services_do_all_areas 		= true;
	data_service_state 				= DATA_SERVICE_STATE_READ;
}

//
//void data_services_write_all(void)
//{
//	data_service_state =	DATA_SERVICE_STATE_WAITING;
//}

// Data is correct so serialise it from the buffer to the live data
void data_services_read_done( void )
{
	serialise_buffer_to_items(data_services_table_index);

	if(data_services_serialize_flags & (DS_LOAD_AT_STARTUP | DS_LOAD_AT_REBOOT))
	{
		if(data_services_table[data_services_table_index].ploadCallback != NULL)
		{
			data_services_table[data_services_table_index].ploadCallback(true);
		}
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
			/// TODO: PUT DATA SERVICES CALLBACK HERE
			data_service_state =	DATA_SERVICE_STATE_WAITING;
		}
}


// Serialise a list of data items/variables to the buffer
// returns total size of the items
unsigned int serialise_items_to_buffer(unsigned int table_index)
{
	if(table_index >= data_service_table_count) return 0;

	const DATA_SERVICE_ITEM* 	pDataItem;
	const unsigned char*		pData;

	unsigned int 	item_index;
	unsigned int 	buffer_index = 0;
	unsigned int 	item_size;

	for(item_index = 0; item_index < data_services_table[table_index].item_count; item_index++)
	{
		pDataItem 	= (DATA_SERVICE_ITEM*) &(data_services_table[table_index].pItem[item_index]);
		pData 		= pDataItem->pData;
		item_size 	= pDataItem->size;
		if( (buffer_index + item_size) > DATA_SERVICE_BUFFER_SIZE )
			return 0;
		memcpy(&data_services_buffer[buffer_index], pData, item_size);
		buffer_index += item_size;
	}
	return buffer_index;
}


// Serialise the buffer to a list of data items/variables
// returns total size of the items
unsigned int  serialise_buffer_to_items(unsigned int table_index)
{
	if(table_index >= data_service_table_count) return 0;

	const DATA_SERVICE_ITEM*	pDataItem;
	const unsigned char*		pData;

	unsigned int 	item_index;
	unsigned int 	buffer_index = 0;
	unsigned int 	item_size;

	for(item_index = 0; item_index < data_services_table[table_index].item_count; item_index++)
	{
		pDataItem 	= (DATA_SERVICE_ITEM*) &(data_services_table[table_index].pItem[item_index]);
		pData	 	= pDataItem->pData;
		item_size 	= pDataItem->size;
		if( (buffer_index + item_size) > DATA_SERVICE_BUFFER_SIZE )
			return 0;
		memcpy(pData, &data_services_buffer[buffer_index], item_size);
		buffer_index += item_size;
	}
	return buffer_index;
}


inline unsigned int data_services_get_table_index(unsigned int data_storage_handle)
{
	int index;

	for(index = 0; index < data_service_table_count; index++)
	{
		if(data_services_table[index].data_storage_handle == data_storage_handle)
		{
			return index;
		}
	}

	return INVALID_HANDLE;
}


boolean data_services_save_specific(unsigned int data_storage_handle, DSRV_callbackFunc pcallback)
{
	if(data_service_state != DATA_SERVICE_STATE_WAITING) return false;

	data_services_table_index = data_services_get_table_index(data_storage_handle);
	if(data_services_table_index == INVALID_HANDLE) return false;
	
	data_services_user_callback = pcallback;

	data_services_serialize_flags = DS_SAVE_ALL;

	data_service_state = DATA_SERVICE_STATE_WRITE;
	
	return false;
}


// Start the write
void data_services_write( void )
{
	if(data_services_table_index >= data_service_table_count)
	{
		if(data_services_user_callback != NULL) data_services_user_callback(true);
		data_services_user_callback = NULL;
		data_service_state =	DATA_SERVICE_STATE_WAITING;
		return;
	}
	
	unsigned int size = serialise_items_to_buffer(data_services_table_index);

	if(size == 0)
		if(data_services_user_callback != NULL) data_services_user_callback(false);

	unsigned int handle = data_services_table[data_services_table_index].data_storage_handle;
	//data_services_calc_item_size(data_services_table_index);
	unsigned int type = data_services_table[data_services_table_index].data_type;

	// TODO: Check here if data handle is ok
	//storage_check_area_exists

	if( (type == DATA_STORAGE_CHECKSUM_STRUCT) && 
		(data_services_serialize_flags & data_services_table[data_services_table_index].service_flags) )
	{
		if(storage_write(handle, data_services_buffer, size, &data_services_write_callback) == true)
		{
			data_service_state =	DATA_SERVICE_STATE_WRITE_WAITING;
			return;
		}
	}
	else
	{
		data_services_table_index++;
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


