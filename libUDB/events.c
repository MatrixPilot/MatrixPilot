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


#include "events.h"
#include "libUDB_internal.h"

#if (BOARD_TYPE != MADRE_BOARD)
#define _EVENT_TRIGGERIP _C2IP
#define _EVENT_TRIGGERIF _C2IF
#define _EVENT_TRIGGERIE _C2IE
#else
#define _EVENT_TRIGGERIP _C1IP
#define _EVENT_TRIGGERIF _C1IF
#define _EVENT_TRIGGERIE _C1IE
#endif

#define MAX_EVENTS	32

EVENT	events[MAX_EVENTS];

boolean event_init_done = false;

unsigned int register_event( void (*event_callback) (void) )
{
	int eventIndex;

	for(eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++)
	{
		if(events[eventIndex].event_callback == NULL)
		{
			events[eventIndex].event_callback = event_callback;
			return eventIndex;
		}
	}

	while(1);		// STOP HERE ON FAILURE.
	return INVALID_HANDLE;
};


void trigger_event(unsigned int hEvent)
{
	if(hEvent > MAX_EVENTS) return;

	// If the event has NULL handle do not trigger it.
	if(events[hEvent].event_callback == NULL) return;

	events[hEvent].eventPending = true;
	_EVENT_TRIGGERIF = 1 ;  // trigger the interrupt
};


void init_events(void)	/* initialize events handler */
{
	// The TTRIGGER interrupt is used a software interrupt event trigger
	_EVENT_TRIGGERIP = 1 ;		// priority 1
	_EVENT_TRIGGERIF = 0 ;		// clear the interrupt
	_EVENT_TRIGGERIE = 1 ;		// enable the interrupt

	int eventIndex;

	for(eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++)
	{
		events[eventIndex].event_callback 	= NULL;
		events[eventIndex].eventPending 	= false;
	}

	event_init_done = true;
	
	return ;
}


//  process EVENT TRIGGER interrupt = software interrupt
#if (BOARD_TYPE != MADRE_BOARD)
void __attribute__((__interrupt__,__no_auto_psv__)) _C2Interrupt(void) 
#else
void __attribute__((__interrupt__,__no_auto_psv__)) _C1Interrupt(void) 
#endif
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;

	int eventIndex;
	EVENT* pEvent;

	_EVENT_TRIGGERIF = 0 ;			// clear the interrupt

	if(event_init_done)
	{
		for(eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++)
		{
			pEvent = &events[eventIndex];
			if(pEvent->eventPending == true)
			{
				pEvent->eventPending = false;
				if(pEvent->event_callback != NULL)
				{
					pEvent->event_callback();
				}
			}
		}
	}

	interrupt_restore_corcon ;
	return ;
}

