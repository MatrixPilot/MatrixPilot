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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "defines.h"
#include "stptok.h"
#include "crc16.h"
#include "logo.h"
#include "logo_parser.h"

//#ifndef WIN32
//#define _strdup strdup
//#endif

#define MAX_LINE_LEN 80
#define MAX_TOKEN_LEN 32
#define MAX_CMD_LEN 21

typedef struct logoDef {
	uint16_t compound_cmd :  1;
	uint16_t cmd          :  5;
	uint16_t do_fly       :  1;
	uint16_t use_param    :  1;
	uint16_t subcmd       :  4;
	int16_t arg           :  4;
} logoDef_t;

typedef struct taglogoCmds {
	uint16_t cmd_hash;
	logoDef_t opcode;
} logo_cmd_t;

typedef struct tagDefs {
	char* label;
	char* value;
} defs_t;

static const char* logo_chk_define(const defs_t* defs, uint8_t size, const char* tok)
{
	uint8_t i;

	if (defs) {
		for (i = 0; i < size; i++) {
		if (strcmp(defs[i].label, tok) == 0) {
//			DPRINT("def: %s\r\n", defs[i].label);
			tok = defs[i].value;
			}
		}
	}
	return tok;
}

static boolean logo_parse_define(defs_t* def, const char* line)
{
	char token[MAX_TOKEN_LEN] = "";
	char* label = NULL;
	char* value = NULL;

	line = stptok(line, token, sizeof(token), " \t\r\n"); // eat the "#define" token
	if (line) {
		line = stptok(line, token, sizeof(token), " \t\r\n");
		label = _strdup(token);
		if (line) {
			line = stptok(line, token, sizeof(token), " \t\r\n");
//			 TODO: value should always be a number, so we could convert it now
			value = _strdup(token);
		}
	} else {
		DPRINT("WARNING: empty define '%s'\r\n", token);
	}
	if (!isdigit(value[0])) {
		DPRINT("WARNING: define is not a numeral\r\n");
	}
	if (label && value) {
		def->label = label;
		def->value = value;
		return true;
	} else {
		DPRINT("WARNING: define '%s' with no value\r\n", label);
	}
	return false;
}

// logocode.h is generated. if it is missing the following needs to be defined:
//#define P_A 8 // parameter A substitution value (first argument)
//#define P_B 9 // parameter B substitution value (second argument)
//static const logo_cmd_t logo_cmd_list[] = {
//	{ 44842, { 3, 1, 0, 0, 8 }, }, // FD
//};
#include "logocode.h"

static logoInstructionDef_t logo_build_instruction(logoDef_t logodef, int16_t param1, int16_t param2)
{
	logoInstructionDef_t opcode;

	opcode.cmd = logodef.cmd;
	opcode.do_fly = logodef.do_fly;
	opcode.use_param = logodef.use_param;
	opcode.subcmd = logodef.subcmd;
	opcode.arg = logodef.arg;
	if (opcode.subcmd == P_D) {
		opcode.subcmd = param1;
	}
	if (opcode.arg == P_A) {
		opcode.arg = param1;
	}
	if (opcode.arg == P_C) {
		opcode.arg = -param1;
	}
	if (opcode.arg == P_B) {
		opcode.arg = param2;
	}
	return opcode;
}

static int8_t logo_parse_command(const defs_t* defs, uint8_t size, logoInstructionDef_t* opcode, const char* line, int16_t line_num, int8_t next)
{
	char token[MAX_TOKEN_LEN] = "";
	const char* str = line;
	const char* tok = NULL;
	uint8_t i = 0;
	int32_t param = 0;
	int16_t param1 = 0;
	int16_t param2 = 0;

	while (line) {
		if (*line == '/') {
			break; // stop parsing on start of comment
		}
		line = stptok(line, token, sizeof(token), " ,()\t\r\n");
		if (i++ == 0) {
		} else {
			tok = logo_chk_define(defs, size, token);
			if (isdigit(tok[0]) || ('-' == tok[0])) {
				if (i == 2) {
					param = atol(tok);
					if (param > 0x7FFF) { // 16 bit signed full scale
//						printf("long parameter detected: %li\r\n", param);
						param1 = param >> 16;
						param2 = param & 0xFFFF;
//						printf("long parameter detected: %li %04x:%04x\r\n", param, param1, param2);
					} else {
						param1 = atoi(tok);
					}
				} else {
					param2 = atoi(tok);
				}
			} else {
//				DPRINT("WARNING: parameter '%s' is not a numeral\r\n", tok);
			}
		}
	}
	if (i) {
		stptok(str, token, sizeof(token), " ,()\t\r\n"); // recover the first token again
		for (i = 0; i < (sizeof(logo_cmd_list)/sizeof(logo_cmd_list[0])); i++) {
			if (logo_cmd_list[i].cmd_hash == crc16(token, strlen(token))) {
				if (opcode) {
					if (next == 0) {
						*opcode = logo_build_instruction(logo_cmd_list[i].opcode, param1, param2);
						DPRINT("%04u %08X: %s %i %i\r\n", line_num, *opcode, token, param1, param2);
					} else {
//						*opcode = logo_build_instruction(logo_cmd_list[i+next].opcode, param2, 0);
						*opcode = logo_build_instruction(logo_cmd_list[i+1].opcode, param1, param2);
						DPRINT("%04u %08X: %s %i %i -- xxxxxxxxxx\r\n", line_num, *opcode, token, param1, param2);
					}
				}
				if (logo_cmd_list[i].opcode.compound_cmd) {
//					printf("compound command: %s\r\n", token);
					return 2; // this instruction requires two opcodes
				}
				return 1;
			}
		}
	}
	DPRINT("ERROR (line %u): bad command parse \"%s\"\r\n", line_num, line);
	return 0;
}

// call without an opcode array to only count the #defines in filename
static uint8_t logo_parse(defs_t* defslist, uint8_t* defscnt, logoInstructionDef_t* instlist, uint16_t size, const char* filename)
{
//	boolean result = false;
	uint8_t count = 0;
	FILE* fp;
	uint16_t line_num = 0;

	fp = fopen(filename, "r");
	if (fp) {
		char ch;
		uint8_t len = 0;
		char* line = malloc(MAX_LINE_LEN+1);
		if (line) {
			while (fread(&ch, 1, sizeof(ch), fp)) {
				if (ch == '\r' || ch == '\n' || ch == '\0') {
					line_num++;
					line[len] = '\0';
					if (line[0] == '/' || line[0] == '\0') {
						// eat comment and empty lines
					} else if (line[0] == '#') {
						if (defslist) {
							if (logo_parse_define(&defslist[*defscnt], line)) {
								(*defscnt)++;
							} else {
								DPRINT("ERROR (line %u): bad define parse \"%s\"\r\n", line_num, line);
								break;
							}
						} else {
							if (defscnt) {
								(*defscnt)++;
							}
						}
					} else {
						if (instlist) {
							int8_t opcodes = 0;
							opcodes = logo_parse_command(defslist, *defscnt, &instlist[count], line, line_num, opcodes);
							if (opcodes > 1) {
//								printf("parsing second opcode: %s\r\n", line);
								logo_parse_command(defslist, *defscnt, &instlist[count+1], line, line_num, opcodes);
							}
							count += opcodes;
						} else {
//							count += logo_parse_command(defslist, *defscnt, NULL, line, line_num, 0);
							count += logo_parse_command(NULL, 0, NULL, line, line_num, 0);
						}
					}
					len = 0;
				} else {
					if (!len && (ch == ' ' || ch == '\t')) {
						// eat whitespace at start of line
					} else {
						if (len < MAX_LINE_LEN) {
							line[len++] = ch;
						}
					}
				}
			}
			free(line);
		}
		fclose(fp);
	} else {
		DPRINT("ERROR: failed to open %s\r\n", filename);
	}
	return count;
}

static defs_t* logo_defines_create(uint8_t* defscnt, const char* define_filename, const char* source_filename)
{
	defs_t* defslist = NULL;

	logo_parse(NULL, defscnt, NULL, 0, define_filename);
	logo_parse(NULL, defscnt, NULL, 0, source_filename);
	DPRINT("%u defines found\r\n", *defscnt);
	defslist = calloc(*defscnt, sizeof(defs_t));
	*defscnt = 0;
	if (defslist) {
		logo_parse(defslist, defscnt, NULL, 0, define_filename);
	}
	return defslist;
}

static void logo_defines_destroy(defs_t* defslist, uint8_t defs_cnt)
{
	uint8_t i;

	for (i = 0; i < defs_cnt; i++) {
		free(defslist[i].label);
		free(defslist[i].value);
	}
	free(defslist);
}

static uint8_t logo_compose(logoInstructionDef_t opcode[], uint16_t size, const char* filename)
{
	uint8_t result = 0;
	uint8_t defs_cnt = 0;

	defs_t* defslist = NULL;
	defslist = logo_defines_create(&defs_cnt, "logodefs.h", filename);

	result = logo_parse(defslist, &defs_cnt, opcode, size, filename);

	logo_defines_destroy(defslist, defs_cnt);

	return result;
}

logoInstructionDef_t* logo_compile(uint16_t* count, const char* source_filename)
{
//	uint8_t count = 0;
	logoInstructionDef_t* logo = NULL;

	*count = logo_parse(NULL, NULL, NULL, 0, source_filename);
	if (*count != 0) {
		DPRINT("%u instructions found in %s\r\n", *count, source_filename);
		logo = calloc(*count, sizeof(logoInstructionDef_t));
		if (logo) {
			memset(logo, 0, *count * sizeof(logoInstructionDef_t));
			if (!(*count = logo_compose(logo, *count, source_filename))) {
				DPRINT("ERROR: parsing logo script %s\r\n", source_filename);
			} else {
			}
		}
	}
	return logo;
}

uint16_t logo_save(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename)
{
	FILE* fp;
	
	DPRINT("opcodes = %u\r\n", count);
	fp = fopen(logo_filename, "w+");
	if (fp) {
		fwrite(logo, sizeof(logoInstructionDef_t), count, fp);
		fclose(fp);
	} else {
		DPRINT("ERROR: opening %s\r\n", logo_filename);
		count = 0;
	}
	return count;
}

uint16_t logo_save_hex(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename)
{
	FILE* fp;
	uint16_t i;
	
	fp = fopen(logo_filename, "w+");
	if (fp) {
		for (i = 0; i < count; i++) {
			fprintf(fp, "%08X\n", logo[i]);
		}
		fclose(fp);
	} else {
		DPRINT("ERROR: opening %s\r\n", logo_filename);
		count = 0;
	}
	return count;
}

/*
uint8_t logo_compile(char* source_filename, char* logo_filename)
{
	logoInstructionDef_t* logo = NULL;

	*count = logo_parse(NULL, NULL, NULL, 0, source_filename);
	DPRINT("%u instructions found in %s\r\n", *count, source_filename);
	logo = calloc(*count, sizeof(logoInstructionDef_t));
	if (logo) {
		memset(logo, 0, *count * sizeof(logoInstructionDef_t));
		if (!(*count = logo_compose(logo, *count, source_filename))) {
			DPRINT("ERROR: parsing logo script %s\r\n", source_filename);
		} else {
		}
	}
	return logo;
}

uint16_t logo_save(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename)
{
	FILE* fp;
	
	{
			DPRINT("opcodes = %u\r\n", count);
			fp = fopen(logo_filename, "w+");
			if (fp) {
				fwrite(logo, sizeof(logoInstructionDef_t), count, fp);
				fclose(fp);
			} else {
				DPRINT("ERROR: opening %s\r\n", logo_filename);
				count = 0;
			}
	}
	return count;
}

uint16_t logo_save_hex(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename)
{
	FILE* fp;
	uint16_t i;
	
	fp = fopen(logo_filename, "w+");
	if (fp) {
		for (i = 0; i < count; i++) {
			fprintf(fp, "%08X\n", logo[i]);
		}
		fclose(fp);
	} else {
		DPRINT("ERROR: opening %s\r\n", logo_filename);
		count = 0;
	}
	return count;
}
 */
