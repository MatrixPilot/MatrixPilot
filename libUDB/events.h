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


// Manages software triggered events to run registered callbacks

#ifndef EVENTS_H
#define EVENTS_H

#include "libUDB_internal.h"

#define INVALID_HANDLE 0xFFFF

void init_events(void);
void trigger_event(uint16_t hEvent);

typedef enum eventP
{
	EVENT_PRIORITY_LOW = 0,
	EVENT_PRIORITY_MEDIUM,
	EVENT_PRIORITY_HIGH,
} eventPriority;

typedef struct tagEVENT
{
	boolean eventPending;
	void (*event_callback)(void);
	int16_t priority;
} EVENT;

uint16_t register_event(void (*event_callback)(void));

uint16_t register_event_p(void (*event_callback)(void), eventPriority priority);


#endif // EVENTS_H
