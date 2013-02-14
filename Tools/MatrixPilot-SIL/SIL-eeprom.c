//
//  SIL-eeprom.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/4/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include "libUDB.h"
#include "string.h"


#define EEPROMFilePath "EEPROM.bin"

uint8_t EEPROMbuffer[32*1024];
boolean EEPROMloaded = 0;
boolean EEPROMdirty = 0;


void loadEEPROMFileIfNeeded(void)
{
	if (EEPROMloaded) return;
	
	EEPROMloaded = 1;
	
	FILE *fp;
	fp = fopen(EEPROMFilePath, "r");
	if (!fp) {
		memset(EEPROMbuffer, 0, 32*1024);
		return;
	}
	long n = fread(EEPROMbuffer, 32, 1024, fp);
	fclose(fp);
	if (n < 1024) {
		memset(EEPROMbuffer, 0, 32*1024);
	}
}


void writeEEPROMFileIfNeeded(void)
{
	if (!EEPROMdirty) return;
	
	FILE *fp;
	fp = fopen(EEPROMFilePath, "w");
	if (!fp) {
		return;
	}
	fwrite(EEPROMbuffer, 32, 1024, fp);
	fclose(fp);
	EEPROMdirty = 0;
}


void eeprom_ByteWrite(uint16_t address, uint8_t data)
{
	loadEEPROMFileIfNeeded();
	EEPROMbuffer[address] = data;
	EEPROMdirty = 1;
}


void eeprom_ByteRead(uint16_t address, uint8_t *data)
{
	loadEEPROMFileIfNeeded();
	*data = EEPROMbuffer[address];
	EEPROMdirty = 1;
}


void eeprom_PageWrite(uint16_t address, uint8_t *data, uint8_t numbytes)
{
	loadEEPROMFileIfNeeded();
	memcpy(EEPROMbuffer+address, data, numbytes);
	EEPROMdirty = 1;
}


void eeprom_SequentialRead(uint16_t address, uint8_t *data, uint16_t numbytes)
{
	loadEEPROMFileIfNeeded();
	memcpy(data, EEPROMbuffer+address, numbytes);
	EEPROMdirty = 1;
}

