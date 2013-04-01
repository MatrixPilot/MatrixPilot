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
//
// Based on code from Microchip AppNote1100, by Martin Bowman.
//    http://ww1.microchip.com/downloads/en/AppNotes/AN1100.zip


#ifndef DATA_SERVICES_H
#define DATA_SERVICES_H

#include "data_storage.h"
#include "parameter_datatypes.h"

#define DATA_SERVICE_BUFFER_SIZE	256

// callback type for data services user
typedef void (*DSRV_callbackFunc)(boolean);


/*
// reference to a RAM variable/item and its size.
typedef struct tagDATA_SERVICE_ITEM
{
	unsigned char*	pData;
	unsigned int	size;
} DATA_SERVICE_ITEM;

//A list of associated variables
typedef DATA_SERVICE_ITEM DATA_SERVICE_ITEMS[];

// 
typedef struct tagDATA_SERVICE_TABLE_ENTRY
{
	unsigned int 					data_storage_handle;
	const DATA_SERVICE_ITEM* const	pItem;
	unsigned int 					item_count;
	unsigned int 					data_type;
	unsigned int 					service_flags;
	DSRV_callbackFunc				ploadCallback;
} DATA_SERVICE_TABLE_ENTRY;
*/

// Initialisation of data services.
void data_services_init(void);

// Trigger the event which calls low priority service routine
void data_services_trigger(void);

// Request to load all memory areas from the table which match the serialize flags
void data_services_load_all(  unsigned int serialize_flags, DSRV_callbackFunc pcallback );

// Load a data area to nv memory with the given handle.
// Return true if services available to take request, otherwise return false
boolean data_services_load_specific(unsigned int data_storage_handle, DSRV_callbackFunc pcallback);

// Save a data area to nv memory with the given handle.
// Return true if services available to take request, otherwise return false
boolean data_services_save_specific(unsigned int data_storage_handle, DSRV_callbackFunc pcallback);

// Request to save all memory areas from the table which match the serialize flags
// return true if services not busy and request can be serviced
boolean data_services_save_all( unsigned int serialize_flags, DSRV_callbackFunc pcallback);

#endif	// DATA_SERVICES_H

