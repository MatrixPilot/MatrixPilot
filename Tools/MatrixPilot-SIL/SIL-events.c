//
//  SIL-events.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/4/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include "libUDB.h"
#include "events.h"
#include "SIL-events.h"

#define MAX_EVENTS 16
typedef void (*event_callback_t)(void);
event_callback_t events[MAX_EVENTS];
int16_t numEvents = 0;
int16_t eventFlags = 0;

uint16_t register_event(void (*event_callback)(void))
{
	if (numEvents >= MAX_EVENTS) return INVALID_HANDLE;
	events[numEvents++] = event_callback;
	return numEvents-1;
}

uint16_t register_event_p(void (*event_callback)(void), eventPriority priority)
{
	(void)priority; // unused parameter
	return register_event(event_callback);
}

void trigger_event(uint16_t hEvent)
{
	eventFlags |= (1 << hEvent);
}

void process_queued_events(void)
{
	if (eventFlags == 0) return;
	
	int16_t i;
	for (i = 0; i < MAX_EVENTS; i++) {
		if (eventFlags & (1 << i)) {
			events[i]();
		}
	}
	eventFlags = 0;
}

