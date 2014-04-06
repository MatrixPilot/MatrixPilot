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


/*
#if (WIN == 1 || NIX == 1)
#define DPRINT printf
#else
#define DPRINT(args, ...)
#endif // WIN

typedef int boolean;
#define false 0
#define true !false

typedef struct logoInstructionDef {
	uint16_t cmd        :  6;
	uint16_t do_fly     :  1;
	uint16_t use_param  :  1;
	uint16_t subcmd     :  8;
	int16_t arg         : 16;
} logoInstructionDef_t;
 */
logoInstructionDef_t* logo_compile(uint16_t* count, const char* source_filename);
uint16_t logo_save(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename);
uint16_t logo_save_hex(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename);
