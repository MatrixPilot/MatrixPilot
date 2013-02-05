//
//  SIL-eeprom.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/4/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include "libUDB.h"
#include "strings.h"


#define EEPROMFilePath "EEPROM.bin"

unsigned char EEPROMbuffer[32*1024];
boolean EEPROMloaded = 0;


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
	if (n < 32*1024) {
		memset(EEPROMbuffer, 0, 32*1024);
	}
}


void writeEEPROMFileIfNeeded(void)
{
	if (!EEPROMloaded) return;
	
	FILE *fp;
	fp = fopen(EEPROMFilePath, "w");
	if (!fp) {
		return;
	}
	fwrite(EEPROMbuffer, 32, 1024, fp);
	fclose(fp);
}


void eeprom_ByteWrite(unsigned int address, unsigned char data)
{
	loadEEPROMFileIfNeeded();
	EEPROMbuffer[address] = data;
}


void eeprom_ByteRead(unsigned int address, unsigned char *data)
{
	loadEEPROMFileIfNeeded();
	*data = EEPROMbuffer[address];
}


void eeprom_PageWrite(unsigned int address, unsigned char *data, unsigned char numbytes)
{
	loadEEPROMFileIfNeeded();
	memcpy(EEPROMbuffer, data, numbytes);
}


void eeprom_SequentialRead(unsigned int address, unsigned char *data, unsigned int numbytes)
{
	loadEEPROMFileIfNeeded();
	memcpy(data, EEPROMbuffer, numbytes);
}

