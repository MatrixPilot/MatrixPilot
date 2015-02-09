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


#include <stdint.h>
#include <stdio.h>
#include "FSconfig.h"
#include "EEPROM.h"
#include "../libUDB/I2C.h"
#include "../libUDB/eeprom_udb4.h"

#ifdef USE_EEPROM_FLASH

enum MCP24LC256_STATES
{
	MCP24LC256_STATE_STOPPED,
	MCP24LC256_STATE_READING,
	MCP24LC256_STATE_WRITING,
	MCP24LC256_STATE_WAITING_WRITE,
	MCP24LC256_STATE_FAILED_TRX,
};

#define MCP24LC256_COMMAND  0xA0
static uint8_t commandData[4] = {0x00, 0x00}; 
static uint16_t MCP24LC256_state = MCP24LC256_STATE_STOPPED;

void PageErase(uint16_t PageAdr)
{
}

static void MCP24LC256_callback(boolean I2CtrxOK)
{
	if (I2CtrxOK == false)
	{
/*
		MCP24LC256_Timer = 0;
		// If waiting for write ACK, continue to wait
		if (MCP24LC256_state != MCP24LC256_STATE_WAITING_WRITE)
		{
			MCP24LC256_state = MCP24LC256_STATE_FAILED_TRX;
			if (pcallerCallback != NULL) pcallerCallback(true);
			pcallerCallback = NULL;
		}
 */
		return;
	}
/*
	switch (MCP24LC256_state)
	{
		case MCP24LC256_STATE_READING:
			if (pcallerCallback != NULL) pcallerCallback(true);
			pcallerCallback = NULL;
			MCP24LC256_state = MCP24LC256_STATE_STOPPED;
			break;
		case MCP24LC256_STATE_WRITING:
			MCP24LC256_Timer = 0;
			MCP24LC256_state = MCP24LC256_STATE_WAITING_WRITE;
			break;
		case MCP24LC256_STATE_WAITING_WRITE:
			MCP24LC256_write_chunk();
			break;
		case MCP24LC256_STATE_FAILED_TRX:
			MCP24LC256_state = MCP24LC256_STATE_STOPPED;
			break;
		default:
			MCP24LC256_state = MCP24LC256_STATE_FAILED_TRX;
			break;
	}
 */
}

void ReadSector(uint16_t sector, uint8_t* buffer)
{
	commandData[1] = (uint8_t)(sector & 0xFF);
	commandData[0] = 0;
	if (I2C1_Read(MCP24LC256_COMMAND, commandData, 2, buffer, 64, &MCP24LC256_callback, 0) == false)
	{
//		MCP24LC256_state = MCP24LC256_STATE_STOPPED;
	}
/*
	commandData[0] = 1;
	if (I2C1_Read(MCP24LC256_COMMAND, commandData, 2, buffer + 64, 64, &MCP24LC256_callback, 0) == false)
	{
	}
	commandData[0] = 2;
	if (I2C1_Read(MCP24LC256_COMMAND, commandData, 2, buffer + 128, 64, &MCP24LC256_callback, 0) == false)
	{
	}
	commandData[0] = 3;
	if (I2C1_Read(MCP24LC256_COMMAND, commandData, 2, buffer + 192, 64, &MCP24LC256_callback, 0) == false)
	{
	}
 */
}

void WriteSector(uint16_t sector, uint8_t* buffer)
{
	// the 24LC256 device works with 64 byte pages
	// therefore writing a 256 byte sector will be done as four consecutive page writes

//	commandData[1] = (uint8_t)(MCP24LC256_write_address & 0xFF);
//	commandData[0] = (uint8_t)((MCP24LC256_write_address >> 8) & 0xFF);
	commandData[1] = (uint8_t)(sector & 0xFF);
	commandData[0] = 0;
	if (I2C1_Write(MCP24LC256_COMMAND, commandData, 2, buffer, 64, &MCP24LC256_callback) == false)
	{
//		MCP24LC256_Timer = 0;
//		MCP24LC256_state = MCP24LC256_STATE_FAILED_TRX;
	}
}

void eeprom_test_write()
{
}

void eeprom_test_read()
{
}

#endif // USE_EEPROM_FLASH
