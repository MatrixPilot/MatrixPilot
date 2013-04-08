// Flexifunction services.  Runs all required states for handling both
// transfers from nv memory and transfers from mavlink

// There are two state machines running but the service can only
// have one state at once.  If memory transfers are in progress, mavlink
// can't have access.

#include <string.h>
#include "defines.h"
#include "../libDCM/libDCM_internal.h" // Needed for access to internal DCM value

#include "../libUDB/events.h"
#include "flexifunction_options.h"
#include "flexiFunctionServices.h"
#include "data_storage.h"

// Include MAVlink library for checksums
#include "../MAVlink/include/mavlink_types.h"
//#include "../MAVlink/include/matrixpilot/matrixpilot.h"
#include "../MAVlink/include/checksum.h"

#include "../MatrixPilot/parameter_datatypes.h"


void flexifunction_commit_buffer_crc( void );
void flexifunction_commit_reloaded_buffer_crc( void );
void flexifunction_write_nvmemory(void);

void nv_write_callback(boolean success);
void nv_init_callback(boolean success);
void nv_reload_callback(boolean success);

inline void flexiFunction_NAK( void ); 		// Called to flag a negative acknowlege
inline void flexiFunction_ACK( void ); 		// Called to flag a positive acknowlege


// Buffer of all flexifunction data including used register and funciton count
NVMEM_FLEXIFUNCTION_DATA flexiFunctionBuffer;

// A constant preamble used to determine the start of flexifunction storage
const unsigned char flexifunction_storage_preamble[] = {0x5A, 0xAA, 0x55, 0xA5};

unsigned int flexiFunctionServiceHandle = INVALID_HANDLE;

unsigned int flexiFunctionState = FLEXIFUNCTION_NOT_STARTED;

unsigned int flexifunction_ref_index 	= 0;
unsigned int flexifunction_ref_checksum	= 0;
unsigned int flexifunction_ref_compID	= 0;
unsigned int flexifunction_ref_command	= 0;
unsigned int flexifunction_ref_result	= 0;
unsigned int flexifunction_ref_sysID	= 0;

// flexifunction service routine for low priority process
void flexiFunctionService(void)
{
	switch(flexiFunctionState)
	{
	case FLEXIFUNCTION_COMMIT_RELOADED_BUFFER:
		flexifunction_commit_reloaded_buffer_crc();	
		break;
	case FLEXIFUNCTION_COMMIT_BUFFER:
		flexifunction_commit_buffer_crc();
		break;
	case FLEXIFUNCTION_WRITE_NVMEMORY:
		flexifunction_write_nvmemory();
		break;
	case FLEXIFUNCTION_INIT:
		if(storage_services_started() == true)
		{
			if(storage_check_area_exists(STORAGE_HANDLE_MIXER, sizeof(flexiFunctionBuffer), DATA_STORAGE_SELF_MANAGED) == true)
				flexiFunctionState = FLEXIFUNCTION_LOAD_NVMEMORY;
			else
			{
				if(storage_create_area(STORAGE_HANDLE_MIXER, sizeof(flexiFunctionBuffer), DATA_STORAGE_SELF_MANAGED, &nv_init_callback) == true)
					flexiFunctionState = FLEXIFUNCTION_INIT_NVMEMORY;
				else
					flexiFunctionState = FLEXIFUNCTION_INIT;
			}
		}
		break;

	case FLEXIFUNCTION_LOAD_NVMEMORY:
		if(storage_read(STORAGE_HANDLE_MIXER, (unsigned char*) &flexiFunctionBuffer, sizeof(flexiFunctionBuffer), &nv_reload_callback) == true)
			flexiFunctionState = FLEXIFUNCTION_LOADING_NVMEMORY;
		break;
	}
}

// Initialise the flexifunction service
void flexiFunctionServiceInit(void)
{
	flexiFunctionServiceHandle = register_event(&flexiFunctionService);
	flexiFunctionState = FLEXIFUNCTION_INIT;
}

// Trigger the flexifunction service to run
void flexiFunctionServiceTrigger(void)
{
	trigger_event(flexiFunctionServiceHandle);
}



// Send a specific buffer to mavlink
boolean flexifunction_send_buffer_specific(unsigned int index)
{
	if(flexiFunctionState != FLEXIFUNCTION_WAITING) return false;

	if(index >=  FLEXIFUNCTION_MAX_FUNCS) return false;

	flexifunction_ref_index = index;
	flexiFunctionState = FLEXIFUNCTION_SEND_BUFFER_SPECIFIC;
	
	return true;
}


// Send all buffers to mavlink
void flexifunction_send_buffer_all(unsigned int index)
{
//	flexifunction_ref_command	= MAVLINK_MSG_ID_FLEXIFUNCTION_SEND_ALL;
//	flexifunction_ref_compID	= compID;
//	flexifunction_ref_sysID		= sysID;

	if(flexiFunctionState != FLEXIFUNCTION_WAITING)
	{
		flexiFunction_NAK();
		return;
	}	

	if(index >=  FLEXIFUNCTION_MAX_FUNCS)
	{
		flexiFunction_NAK();
		return;
	}	

	flexifunction_ref_index = index;
	flexiFunctionState = FLEXIFUNCTION_SEND_BUFFER_SPECIFIC;
	
	return;
}


// Calling during the sending of all buffer items
void flexifunction_sending_buffer_all()
{
	if(flexiFunctionState != FLEXIFUNCTION_SENDING_BUFFER_ALL)
	{
		flexiFunction_NAK();
		return;
	}

	// If all of the funcitons have been sent, stop and send acknowledge command
	if(flexifunction_ref_index >=  flexiFunctionBuffer.flexiFunctionsUsed) 
	{
		flexiFunction_ACK();
		return;
	}

	

	flexifunction_ref_index++;
	
	return;
}

// Write a function to the buffer
void flexiFunction_write_buffer_function(unsigned char* pFuncData, unsigned int index, unsigned int address, unsigned int size, unsigned int count)
{
	if(index >=  FLEXIFUNCTION_MAX_FUNCS-1)
	{
		flexifunction_ref_result = 0;
		flexiFunctionState = FLEXIFUNCTION_BUFFER_FUNCTION_ACKNOWLEDGE;
		return;
	}

	functionSetting* pSetting = (functionSetting*) pFuncData;
	unsigned int tempAddr = address;

	// If this is a function update only, take the address from the directory.
	if(address == 0xFFFF)
	{
		if(count != flexiFunctionBuffer.flexiFunction_dataset.flexiFunctionsUsed)
		{
			flexifunction_ref_result = 0;
			flexiFunctionState = FLEXIFUNCTION_BUFFER_FUNCTION_ACKNOWLEDGE;
			return;
		}
		tempAddr = flexiFunctionBuffer.flexiFunction_dataset.flexiFunction_directory[index];
	}

	if( (tempAddr + size) >= FLEXIFUNCTION_MAX_DATA_SIZE )
	{
		flexifunction_ref_result = 0;
		flexiFunctionState = FLEXIFUNCTION_BUFFER_FUNCTION_ACKNOWLEDGE;
		return;
	}

	memcpy(&flexiFunctionBuffer.flexiFunction_dataset.flexiFunction_data[tempAddr], pFuncData, size);

	if(address == 0xFFFF)
	{
		flexiFunctionState = FLEXIFUNCTION_COMMITTING_BUFFER;
	}
	else
	{
		flexiFunctionBuffer.flexiFunction_dataset.flexiFunctionsUsed = count;
		flexiFunctionBuffer.flexiFunction_dataset.flexiFunction_directory[index] = tempAddr;
		flexiFunctionBuffer.flexiFunction_dataset.flexiFunction_directory[index+1] = tempAddr + size;
	
		flexifunction_ref_result = 1;
		flexiFunctionState = FLEXIFUNCTION_BUFFER_FUNCTION_ACKNOWLEDGE;
	}
	return;
}

//
//// Write a functions count to the buffer
//void flexiFunction_write_functions_count(unsigned int funcCount)
//{
//	if(funcCount >= FLEXIFUNCTION_MAX_FUNCS)
//	{
//		flexifunction_ref_result = 0;
//		flexiFunctionState = FLEXIFUNCTION_SIZES_ACKNOWLEDGE;
//		return;
//	}
//	
//	flexiFunctionBuffer.flexiFunctionsUsed = funcCount;
//	flexifunction_ref_result = 1;
//	flexiFunctionState = FLEXIFUNCTION_SIZES_ACKNOWLEDGE;
//	return;
//}

// Get functions count from buffer
unsigned int flexiFunction_get_functions_count( void )
{
	return flexiFunctionBuffer.flexiFunction_dataset.flexiFunctionsUsed;
}

// Write directory data
void flexiFunction_write_directory(unsigned char directory_type , unsigned char start_index, unsigned char count, unsigned char* pdirectory_data)
{
	if( (start_index + count) > FLEXIFUNCTION_MAX_DIRECTORY_SIZE)
	{
		flexifunction_ref_result = 0;
		switch(directory_type)
		{
		case 0:
			flexiFunctionState = FLEXIFUNCTION_OUTPUT_DIRECTORY_ACKNOWLEDGE;
			break;
		case 1:
			flexiFunctionState = FLEXIFUNCTION_INPUT_DIRECTORY_ACKNOWLEDGE;
			break;
		}
		return;
	}

	switch(directory_type)
	{
	case 0:
		memcpy(&flexiFunctionBuffer.flexiFunction_dataset.outputs_directory[start_index], 	pdirectory_data, count);
		flexifunction_ref_result = 1;
		flexiFunctionState = FLEXIFUNCTION_OUTPUT_DIRECTORY_ACKNOWLEDGE;
		break;
	case 1:
		memcpy(&flexiFunctionBuffer.flexiFunction_dataset.inputs_directory[start_index], 	pdirectory_data, count);
		flexifunction_ref_result = 1;
		flexiFunctionState = FLEXIFUNCTION_INPUT_DIRECTORY_ACKNOWLEDGE;
		break;
	default:
		flexifunction_ref_result = 0;
		flexiFunctionState = FLEXIFUNCTION_COMMAND_ACKNOWLEDGE;
	}


}


void flexiFunction_commit_buffer()
{
	if(flexiFunctionState != FLEXIFUNCTION_WAITING)	
	{
		flexifunction_ref_result = 0;
		flexiFunctionState = FLEXIFUNCTION_COMMAND_ACKNOWLEDGE;
		return;
	}

	if(flexiFunctionBuffer.flexiFunctionsUsed > FLEXIFUNCTION_MAX_REGS)
	{
		flexiFunction_NAK();
		return;
	}


	// Next step is to calculate checksum
	flexiFunctionState = FLEXIFUNCTION_COMMIT_BUFFER;
}

// A place to do checksums if it is ever implemented
void flexifunction_commit_buffer_crc( void )
{
	flexiFunctionState = FLEXIFUNCTION_COMMITTING_BUFFER;
}


void flexifunction_commit_reloaded_buffer_crc( void )
{
	// If preamble is not valid, do not commit values and return to wait.
	if(memcmp(flexifunction_storage_preamble, flexiFunctionBuffer.preamble, 4) != 0)
	{
		flexiFunctionState = FLEXIFUNCTION_WAITING;
		return;
	}

	// If checksum is not correct, do not commit values and return to wait.
	if(flexiFunctionBuffer.checksum != crc_calculate( (uint8_t*) &flexiFunctionBuffer.flexiFunction_dataset, sizeof(flexiFunctionBuffer.flexiFunction_dataset) ) )
	{
		flexiFunctionState = FLEXIFUNCTION_WAITING;
		return;
	}

	flexiFunctionState = FLEXIFUNCTION_COMMITTING_BUFFER;
}

void flexifunction_write_nvmemory(void)
{
	flexifunction_ref_command = FLEXIFUNCTION_COMMAND_WRITE_NVMEMORY;

	// Make sure the preamble is set correctly
	memcpy(flexiFunctionBuffer.preamble, flexifunction_storage_preamble, 4);

	// Calculate data checksum
	flexiFunctionBuffer.checksum = crc_calculate( (uint8_t*) &flexiFunctionBuffer.flexiFunction_dataset, sizeof(flexiFunctionBuffer.flexiFunction_dataset) );

	if( storage_write(STORAGE_HANDLE_MIXER, (unsigned char*) &flexiFunctionBuffer, sizeof(flexiFunctionBuffer), &nv_write_callback) == true)
	{
		flexiFunctionState = FLEXIFUNCTION_WRITING_NVMEMORY;
	}
	else
	{
		flexiFunction_NAK();
	}

	return;
}


void nv_write_callback(boolean success)
{
	if(success)
		flexiFunction_ACK();
	else
		flexiFunction_NAK();
}


void nv_init_callback(boolean success)
{
	if(success == true)
		flexiFunctionState = FLEXIFUNCTION_WAITING;
	else
		flexiFunctionState = FLEXIFUNCTION_INIT;
}

void nv_reload_callback(boolean success)
{
	if(success == true)
		flexiFunctionState = FLEXIFUNCTION_COMMIT_RELOADED_BUFFER;
	else
		flexiFunctionState = FLEXIFUNCTION_WAITING;
}


// Called by flexifunction loop to test for a new set of functions available
void flexiFunction_commit_buffer_check()
{
	if(flexiFunctionState != FLEXIFUNCTION_COMMITTING_BUFFER) return;

	memcpy(&flexiFunction_dataset, &flexiFunctionBuffer.flexiFunction_dataset, sizeof(flexiFunction_dataset));

	flexifunction_ref_command = FLEXIFUNCTION_COMMAND_COMMIT_BUFFER;

	flexiFunction_ACK();
}

// Called to flag a negative acknowlege
inline void flexiFunction_NAK( void )
{
	flexifunction_ref_result = 0;
	flexiFunctionState = FLEXIFUNCTION_COMMAND_ACKNOWLEDGE;
}

// Called to flag a positive acknowlege
inline void flexiFunction_ACK( void )
{
	flexifunction_ref_result = 1;
	flexiFunctionState = FLEXIFUNCTION_COMMAND_ACKNOWLEDGE;
};


