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


#include "libUDB_internal.h"

#if(USE_NV_MEMORY == 1)

#include "NV_memory.h"
#include "I2C.h"
#include "events.h"

#define MCP24XX_COMMAND	0xA0

#if (BOARD_TYPE == UDB4_BOARD)
// 24LC256
#define PAGE_SIZE 0X40
#define MAX_ADDRESS 0x7FFF
#elif (BOARD_TYPE & AUAV2_BOARD)
// 24AA64
#define PAGE_SIZE 0X20
#define MAX_ADDRESS 0x3FFF
#else
#error "unsupported BOARD_TYPE for USE_NV_MEMORY"
#endif

enum MCP24XX_STATES
{
	MCP24XX_STATE_STOPPED,
	MCP24XX_STATE_READING,
	MCP24XX_STATE_WRITING,
	MCP24XX_STATE_WAITING_WRITE,
	MCP24XX_STATE_FAILED_TRX,
};


unsigned char commandData[4] = {0x00, 0x00}; 

unsigned int MCP24XX_state = MCP24XX_STATE_STOPPED;

unsigned int 	MCP24XX_write_address;
unsigned int 	MCP24XX_write_size;
unsigned char* 	MCP24XX_pwrBuffer = NULL;

unsigned int 	MCP24XX_Timer = 0;

boolean MCP24XX_write_chunk();


/** V A R I A B L E S **********************************************/

//void (*callbackWriteDone) (void);

void NVMemory_callback(void);

void MCP24XX_callback(boolean I2CtrxOK);

NVMemory_callbackFunc pcallerCallback = NULL;

unsigned int nv_memory_service_handle = INVALID_HANDLE;


void nv_memory_service( void )
{
	switch(MCP24XX_state)
	{
	case  MCP24XX_STATE_WAITING_WRITE:
		I2C1_checkACK(MCP24XX_COMMAND, &MCP24XX_callback);
		break;	
	case  MCP24XX_STATE_FAILED_TRX:
		I2C1_checkACK(MCP24XX_COMMAND, &MCP24XX_callback);
		break;
	}
}


void nv_memory_init( void )
{
	nv_memory_service_handle = register_event(&nv_memory_service);
}

void nv_memory_service_trigger( void )
{
	trigger_event(nv_memory_service_handle);
}


boolean udb_nv_memory_read( unsigned char* rdBuffer, unsigned int address, unsigned int rdSize, NVMemory_callbackFunc pCallback)
{
	if(MCP24XX_state != MCP24XX_STATE_STOPPED) return false;
	MCP24XX_state = MCP24XX_STATE_READING;

	commandData[1] = (unsigned char) (address & 0xFF);
	commandData[0] = (unsigned char) ((address >> 8) & 0xFF);

	pcallerCallback = pCallback;

	if(I2C1_Read( MCP24XX_COMMAND, commandData , 2,  rdBuffer, rdSize, &MCP24XX_callback) == false)
	{
		MCP24XX_state = MCP24XX_STATE_STOPPED;
		return false;
	}
	return true;
}

boolean udb_nv_memory_write( unsigned char* wrBuffer, unsigned int address, unsigned int wrSize, NVMemory_callbackFunc pCallback)
{
	if(MCP24XX_state != MCP24XX_STATE_STOPPED) return false;

	// Check address range.
	if(address > MAX_ADDRESS) return false;
	if(address > MAX_ADDRESS) return false;

	MCP24XX_pwrBuffer = wrBuffer;

	MCP24XX_write_address = address;
	MCP24XX_write_size = wrSize;

	pcallerCallback = pCallback;

	return MCP24XX_write_chunk();
}


boolean MCP24XX_write_chunk()
{
	unsigned int writeSize = MCP24XX_write_size;
	// Truncate write at page boundary
	if(writeSize > PAGE_SIZE)
		writeSize = PAGE_SIZE;
	
	// Check if writes are finished
	if(writeSize == 0)
	{
		MCP24XX_state = MCP24XX_STATE_STOPPED;
		if(pcallerCallback != NULL)	pcallerCallback(true);
		pcallerCallback = NULL;
		return true;
	}

	// Find remaining bytes in the page
	unsigned int pageRemaining = PAGE_SIZE - (MCP24XX_write_address & (PAGE_SIZE-1));

	if( writeSize > pageRemaining) writeSize = pageRemaining;

	commandData[1] = (unsigned char) (MCP24XX_write_address & 0xFF);
	commandData[0] = (unsigned char) ((MCP24XX_write_address >> 8) & 0xFF);

	MCP24XX_state = MCP24XX_STATE_WRITING;

	if(I2C1_Write( MCP24XX_COMMAND, commandData , 2,  MCP24XX_pwrBuffer, writeSize, &MCP24XX_callback) == false)
	{
		MCP24XX_Timer = 0;
		MCP24XX_state = MCP24XX_STATE_FAILED_TRX;
		return false;
	}

	MCP24XX_write_size -= writeSize;
	MCP24XX_write_address += writeSize;
	MCP24XX_pwrBuffer = &MCP24XX_pwrBuffer[writeSize];
	return true;
}


void NVMemory_callback(void)
{
}


void MCP24XX_callback(boolean I2CtrxOK)
{
	if(I2CtrxOK == false)
	{
		MCP24XX_Timer = 0;
		// If waiting for write ACK, continue to wait
		if(MCP24XX_state != MCP24XX_STATE_WAITING_WRITE)
		{
			MCP24XX_state = MCP24XX_STATE_FAILED_TRX;
			if(pcallerCallback != NULL)	pcallerCallback(true);
			pcallerCallback = NULL;
		}
		return;
	}

	switch(MCP24XX_state)
	{
		case MCP24XX_STATE_READING:
			if(pcallerCallback != NULL)	pcallerCallback(true);
			pcallerCallback = NULL;
			MCP24XX_state = MCP24XX_STATE_STOPPED;
			break;
		case MCP24XX_STATE_WRITING:
			MCP24XX_Timer = 0;
			MCP24XX_state = MCP24XX_STATE_WAITING_WRITE;
			break;
		case MCP24XX_STATE_WAITING_WRITE:
			MCP24XX_write_chunk();
			break;
 		case MCP24XX_STATE_FAILED_TRX:
			MCP24XX_state = MCP24XX_STATE_STOPPED;
			break;
		default:
			MCP24XX_state = MCP24XX_STATE_FAILED_TRX;
			break;
	};

}

#endif // #if(USE_NV_MEMORY == 1)



