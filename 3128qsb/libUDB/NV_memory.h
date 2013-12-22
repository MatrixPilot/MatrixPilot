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


// non volatile memory driver defintion


#ifndef NV_MEMORY_H
#define NV_MEMORY_H

#include "libUDB_internal.h"

typedef void (*NVMemory_callbackFunc)(boolean);

// Trigger service of nv memory in low priority process
void nv_memory_service_trigger(void);

// Initialise non volatile memory service
void nv_memory_init(void);

boolean udb_nv_memory_read(uint8_t* rdBuffer, uint16_t address, uint16_t rdSize, NVMemory_callbackFunc pCallback);

boolean udb_nv_memory_write(uint8_t* wrBuffer, uint16_t address, uint16_t wrSize, NVMemory_callbackFunc pCallback);

#endif

