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


#include "../libUDB/libUDB.h"
/*
#include "defines.h"
#include "p33Exxxx.h"
#include "../libFlashFS/AT45D.h"
#include "../libFlashFS/FSIO.h"
#include "../libFlashFS/FSIO_DBG.h"
 */
#include "../libCommon/uart3.h"
#include <string.h>
#include <stdio.h>

#define LOWORD(a) ((WORD)(a))
#define HIWORD(a) ((WORD)(((DWORD)(a) >> 16) & 0xFFFF))

void AT45D_FormatFS(void);
void AT45D_WipeFS(void);
//		DisplayFS();
//		TestFS();

typedef struct tagCmds {
	int index;
	void (*fptr)(void);
	const char const * cmdstr;
} cmds_t;


int logging_enabled = 0;
int cmdlen = 0;
char cmdstr[32];

void cmd_ver(void)
{
	printf("AUAV3 v0.1, " __TIME__ " " __DATE__ "\r\n");
}

void cmd_format(void)
{
	printf("formatting dataflash\r\n");
	AT45D_FormatFS();
}

void cmd_wipe(void)
{
	printf("wiping dataflash\r\n");
	AT45D_WipeFS();
}

void cmd_start(void)
{
	printf("starting.\r\n");
	logging_enabled = 1;
}

void cmd_stop(void)
{
	printf("stopped.\r\n");
	logging_enabled = 0;
}


void cmd_on(void)
{
	printf("on.\r\n");
	SRbits.IPL = 0 ;	// turn on all interrupt priorities
}

void cmd_off(void)
{
	printf("off.\r\n");
	SRbits.IPL = 7 ;	// turn off all interrupt priorities
}

extern int heartbeat_count;

void cmd_cpuload(void)
{
	printf("CPU Load %u, %u.\r\n", udb_cpu_load(), heartbeat_count);
}

void cmd_crash(void)
{
	static int i;
	char buffer[32];

	sprintf(buffer, "overflowing stack %u.\r\n", i++);
	printf(buffer);
	cmd_crash();
}

void cmd_adc(void)
{
	printf("ADC vcc %u, 5v %u, rssi %u\r\n", udb_vcc.value, udb_5v.value, udb_rssi.value);
}


void printbin16(int a)
{
	unsigned int i;
	for (i = 0x8000; i > 0; i >>= 1) {
		if (a & i) printf("1");
		else printf("0");
	}
}

const char *byte_to_binary(int x)
{
    static char b[9];
    int z;

    b[0] = '\0';
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

const char *word_to_binary(int x)
{
    static char b[17];
    unsigned int z;

    b[0] = '\0';
    for (z = 0x8000; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

void cmd_reg(void)
{
	printf("USB Registers:\r\n");
	printf("\tU1OTGSTAT = %s\r\n", word_to_binary(U1OTGSTAT));
	printf("\tU1OTGCON  = %s\r\n", word_to_binary(U1OTGCON));
	printf("\tU1STAT    = %s\r\n", word_to_binary(U1STAT));
	printf("\tU1CON     = %s\r\n", word_to_binary(U1CON));
	printf("\tU1CNFG1   = %s\r\n", word_to_binary(U1CNFG1));
	printf("\tU1CNFG2   = %s\r\n", word_to_binary(U1CNFG2));
	printf("\tU1OTGIR   = %s\r\n", word_to_binary(U1OTGIR));
	printf("\tU1OTGIE   = %s\r\n", word_to_binary(U1OTGIE));
/*
UxOTGSTAT: USB OTG STATUS REGISTER
VBUSVD: A-VBUS Valid Indicator bit
1 = The VBUS voltage is above VA_VBUS_VLD (as defined in the USB OTG Specification) on the A device
0 = The VBUS voltage is below VA_VBUS_VLD on the A device


UxOTGCON: USB ON-THE-GO CONTROL REGISTER
bit 3 VBUSON: VBUS Power-on bit(1)
1 = VBUS line is powered
0 = VBUS line is not powered

UxCNFG2: USB CONFIGURATION REGISTER 2
 */
}

#if (RECORD_FREE_STACK_SPACE == 1)
extern uint16_t maxstack;#endif
void cmd_stack(void)
{
#if (RECORD_FREE_STACK_SPACE == 1)
	printf("maxstack %u\r\n", maxstack);
#else
	printf("stack reporting disabled.\r\n");
#endif
}

void cmd_help(void);

const cmds_t cmdslist[] = {
	{ 0, cmd_help,   "help" },
	{ 0, cmd_ver,    "ver" },
	{ 0, cmd_wipe,   "wipe" },
	{ 0, cmd_format, "format" },
	{ 0, cmd_start,  "start" },
	{ 0, cmd_stop,   "stop" },
	{ 0, cmd_on,     "on" },
	{ 0, cmd_off,    "off" },
	{ 0, cmd_stack,  "stack" },
	{ 0, cmd_reg,    "reg" },
	{ 0, cmd_adc,    "adc" },
	{ 0, cmd_cpuload,"cpu" },
	{ 0, cmd_crash,  "crash" },
};

void cmd_help(void)
{
	int i;

	printf("Commands:\r\n");
	for (i = 0; i < (sizeof(cmdslist)/sizeof(cmdslist[0])); i++) {
		printf("\t%s\r\n", cmdslist[i].cmdstr);
	}
}

// int strncmp(const char *string1, const char *string2, size_t count);
// int strcmp(const char *string1, const char *string2);

void command(char* cmdstr)
{
	int i;

	for (i = 0; i < (sizeof(cmdslist)/sizeof(cmdslist[0])); i++) {
//		printf("comparing %s with %s\r\n", cmdstr, cmdlist[i]);
		if (strcmp(cmdslist[i].cmdstr, cmdstr) == 0) {
//			printf("found command %u (%s)\r\n", i, cmdslist[i].cmdstr);
			cmdslist[i].fptr();
		}
	}
}

void console(void)
{
	if (UART3IsPressed()) {
		char ch = UART3GetChar();
//		UART3PutHex((int)ch);
		if (cmdlen < sizeof(cmdstr)) {
			cmdstr[cmdlen] = ch;
			if ((ch == '\r') || (ch == '\n')) {
				cmdstr[cmdlen] = '\0';			
//				printf("\r\n");
				if (strlen(cmdstr) > 0) {
//					printf("\r\nfound command: %s\r\n", cmdstr);
					UART3PutChar('\r');
					command(cmdstr);
					cmdlen = 0;
				}
			} else {
				UART3PutChar(ch);
				cmdlen++;
			}
		} else {
			cmdlen = 0;
		}
	}
}

