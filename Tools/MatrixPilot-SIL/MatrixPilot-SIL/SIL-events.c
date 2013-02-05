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


#define MAX_EVENTS 16
typedef void (*event_callback_t)();
event_callback_t events[MAX_EVENTS];
int numEvents = 0;
int eventFlags = 0;


unsigned int register_event( void (*event_callback) (void) )
{
	events[numEvents++] = event_callback;
	return numEvents-1;
}


unsigned int register_event_p( void (*event_callback) (void), eventPriority priority )
{
	return register_event(event_callback);
}


void trigger_event(unsigned int hEvent)
{
	eventFlags |= (1 << hEvent);
}


void process_queued_events(void)
{
	int i;
	for (i=0; i<16; i++) {
		if (eventFlags & (1 << i)) {
			events[i]();
		}
	}
	eventFlags = 0;
}

