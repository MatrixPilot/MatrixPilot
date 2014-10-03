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
#include "../libUDB/libUDB.h"
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

#define MCP24LC256_COMMAND 0xA0
static uint8_t commandData[4] = {0x00, 0x00}; 
static uint16_t MCP24LC256_state = MCP24LC256_STATE_STOPPED;

void PageErase(uint16_t PageAdr) {}

//typedef void (*I2C_callbackFunc)(boolean);
static void MCP24LC256_callback(boolean I2CtrxOK)
{
//	printf("MCP24LC256_callback(%s)\r\n", I2CtrxOK ? "true" : "false");

	if (I2CtrxOK == false)
	{
		printf("MCP24LC256_STATE_FAILED_TRX\r\n");
/*
//		MCP24LC256_Timer = 0;
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

	switch (MCP24LC256_state)
	{
		case MCP24LC256_STATE_READING:
//			if (pcallerCallback != NULL) pcallerCallback(true);
//			pcallerCallback = NULL;
			MCP24LC256_state = MCP24LC256_STATE_STOPPED;
			break;
		case MCP24LC256_STATE_WRITING:
//			MCP24LC256_Timer = 0;
			MCP24LC256_state = MCP24LC256_STATE_WAITING_WRITE;
			break;
		case MCP24LC256_STATE_WAITING_WRITE:
//			MCP24LC256_write_chunk();

			MCP24LC256_state = MCP24LC256_STATE_STOPPED;

			break;
		case MCP24LC256_STATE_FAILED_TRX:
			MCP24LC256_state = MCP24LC256_STATE_STOPPED;
			break;
		default:
			MCP24LC256_state = MCP24LC256_STATE_FAILED_TRX;
			break;
	}
}

void eeprom_service(void)
{
	switch (MCP24LC256_state)
	{
		case MCP24LC256_STATE_WAITING_WRITE:
//		printf("eeprom_service() MCP24LC256_STATE_WAITING_WRITE\r\n");
			I2C1_CheckAck(MCP24LC256_COMMAND, &MCP24LC256_callback);
			break;
		case MCP24LC256_STATE_FAILED_TRX:
//		printf("eeprom_service() MCP24LC256_STATE_FAILED_TRX\r\n");
			I2C1_CheckAck(MCP24LC256_COMMAND, &MCP24LC256_callback);
			break;
	}
}

#if 1

int block(void)
{
	long timeout = 500000;
//	unsigned long timeout = 0xffffffff;
	while (MCP24LC256_state != MCP24LC256_STATE_STOPPED)
	{
//		eeprom_service();
		if (!timeout--)
		{
			printf("timeout\r\n");
			return 1;
		} 
	}
	return 0;
}

void ReadPage(uint16_t page, uint8_t* buffer)
{
//	int timeout = 50000;
//	printf("ReadPage(%u, ...)\r\n", page);

	commandData[1] = page * 64;
	if (page > 3) commandData[0] |= 1;

	MCP24LC256_state = MCP24LC256_STATE_READING;
//boolean I2C1_Read(uint8_t address, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* prxData, uint16_t rxSize, I2C_callbackFunc pCallback, uint16_t I2C_mode);
	if (I2C1_Read(MCP24LC256_COMMAND, commandData, 2, buffer, 64, &MCP24LC256_callback, 0) == false)
	{
		MCP24LC256_state = MCP24LC256_STATE_STOPPED;
		printf("ReadPage(...) - ERROR\r\n");
	}
//	printf("ReadPage(...) - 1\r\n");
	block();
//	while (MCP24LC256_state != MCP24LC256_STATE_STOPPED) { if (!timeout--) { printf("timeout\r\n"); break; } }
//	printf("ReadPage(...) - 2\r\n");
}

static uint16_t last_sector = 0xFFFF;

void ReadSector(uint16_t sector, uint8_t* buffer)
{
	int page = 0;

	if (last_sector == sector) return;

last_sector = sector;

//	printf("ReadSector(%u, ...)\r\n", sector);
//	while (MCP24LC256_state != MCP24LC256_STATE_STOPPED) {}
	if (block())
	{
		printf("ReadSector(...) failed\r\n");
		return;
	}
//	commandData[0] = (uint8_t)(sector & 0xFF);
	commandData[0] = (uint8_t)(sector << 1);
	do {
		ReadPage(page, buffer);		
		buffer += 64;
	} while (page++ < 7);
}

void WritePage(uint16_t page, uint8_t* buffer)
{
//	int timeout = 50000;
//	printf("WritePage(%u, ...)\r\n", page);
	commandData[1] = page * 64;
	if (page > 3) commandData[0] |= 1;
	MCP24LC256_state = MCP24LC256_STATE_WRITING;
	if (I2C1_Write(MCP24LC256_COMMAND, commandData, 2, buffer, 64, &MCP24LC256_callback) == false)
//boolean I2C1_Write(uint8_t address, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* ptxData, uint16_t txSize, I2C_callbackFunc pCallback);
	{
		MCP24LC256_state = MCP24LC256_STATE_STOPPED;
		printf("WritePage(...) - ERROR\r\n");
	}
//	printf("WritePage(...) - 1\r\n");
	block();
//	printf("WritePage(...) - 2\r\n");
}

void WriteSector(uint16_t sector, uint8_t* buffer)
{
	// the 24LC256 device works with 64 byte pages
	// therefore writing a 512 byte sector will be done as eight consecutive page writes
	int page = 0;

last_sector = 0xFFFF;

//	printf("WriteSector(%u, ...)\r\n", sector);
	if (block()) return;
//	printf("WriteSector(...) - 2\r\n");
//	commandData[0] = (uint8_t)(sector & 0xFF) << 1;
	commandData[0] = (uint8_t)(sector << 1);
	do {
		WritePage(page, buffer);		
		buffer += 64;
	} while (page++ < 7);
}


#else

//void udb_eeprom_init(void);
//void eeprom_PageWrite(uint16_t address, uint8_t* data, uint8_t numbytes);
//void eeprom_SequentialRead(uint16_t address, uint8_t* data, uint16_t numbytes);

static uint16_t last_sector = 0xFFFF;
static int last_op = -1;

void ReadSector(uint16_t sector, uint8_t* buffer)
{
	int page = 0;
	uint16_t address = sector * 512;

	if (last_sector == sector && last_op == 0) return;
	last_sector = sector;
	last_op = 0;
	printf("ReadSector(%u, ...)\r\n", sector);
	eeprom_SequentialRead(address, buffer, 512);
}

void WriteSector(uint16_t sector, uint8_t* buffer)
{
	// the 24LC256 device works with 64 byte pages
	// therefore writing a 512 byte sector will be done as eight consecutive page writes
	int page = 0;
	uint16_t address = sector * 256;

	last_sector = sector;
	last_op = 1;

	printf("WriteSector(%u, ...)\r\n", sector);

	do {
		eeprom_PageWrite(address, buffer, 64);
		buffer += 64;
		address += 64;
	} while (page++ < 7);
}

#endif

extern BYTE __attribute__ ((aligned(4)))   gDataBuffer[MEDIA_SECTOR_SIZE];     // The global data sector buffer

void eeprom_test_read(char* arg)
{
	int sector;
	int i;
	int j = 0;

	sector = atol(arg);
	printf("eeprom_test_read(%u)\r\n", sector);
	ReadSector(sector, gDataBuffer);
	for (i = 0; i < 512; i++)
	{
		printf("%02X ", gDataBuffer[i]);
		if (j++ >= 15)
		{
			j = 0;
			printf("\r\n");
		}
	}
}

void eeprom_test_write(char* arg)
{
	int sector;
	int i;
	int j = 'a';

	sector = atol(arg);
	printf("eeprom_test_write(%u)\r\n", sector);
	for (i = 0; i < 512; i++)
	{
		gDataBuffer[i] = j;
		if (j++ >= 'z')
		{
			j = 'a';
		}
	}
	WriteSector(sector, gDataBuffer);
}

#endif // USE_EEPROM_FLASH
