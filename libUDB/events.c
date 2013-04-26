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

#define _EVENTL_TRIGGERIP _C2IP
#define _EVENTL_TRIGGERIF _C2IF
#define _EVENTL_TRIGGERIE _C2IE
#define _EVENTL_INTERUPT _C2Interrupt

#define _EVENTM_TRIGGERIP _C1IP
#define _EVENTM_TRIGGERIF _C1IF
#define _EVENTM_TRIGGERIE _C1IE
#define _EVENTM_INTERUPT _C1Interrupt

#define MAX_EVENTS	16

EVENT	events[MAX_EVENTS];

boolean event_init_done = false;

uint16_t register_event( void (*event_callback) (void) )
{
	return register_event_p( event_callback, EVENT_PRIORITY_MEDIUM);
};

uint16_t register_event_p( void (*event_callback) (void), eventPriority priority )
{
	int16_t eventIndex;

	for(eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++)
	{
		if(events[eventIndex].event_callback == NULL)
		{
			events[eventIndex].event_callback = event_callback;
			events[eventIndex].priority = priority;
			return eventIndex;
		}
	}

	return INVALID_HANDLE;
}

void trigger_event(uint16_t hEvent)
{
	if (hEvent < MAX_EVENTS)
	{
		if (events[hEvent].event_callback != NULL)
		{
			events[hEvent].eventPending = true;
			switch(events[hEvent].priority)
			{
			case EVENT_PRIORITY_LOW:
				_EVENTL_TRIGGERIF = 1 ;  // trigger the interrupt
				break;
			case EVENT_PRIORITY_MEDIUM:
				_EVENTM_TRIGGERIF = 1 ;  // trigger the interrupt
				break;
//			case EVENT_PRIORITY_HIGH:
//				_EVENTH_TRIGGERIF = 1 ;  // trigger the interrupt
//				break;
			}
		}
	}
}


void init_events(void)	/* initialize events handler */
{
	// The TTRIGGER interrupt is used a software interrupt event trigger
	_EVENTL_TRIGGERIP = 1 ;		// priority 1
	_EVENTL_TRIGGERIF = 0 ;		// clear the interrupt
	_EVENTL_TRIGGERIE = 1 ;		// enable the interrupt

	_EVENTM_TRIGGERIP = 2 ;		// priority 2
	_EVENTM_TRIGGERIF = 0 ;		// clear the interrupt
	_EVENTM_TRIGGERIE = 1 ;		// enable the interrupt

	int16_t eventIndex;

	for(eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++)
	{
		events[eventIndex].event_callback 	= NULL;
		events[eventIndex].eventPending 	= false;
		events[eventIndex].priority 		= EVENT_PRIORITY_LOW;
	}

	event_init_done = true;
	
	return ;
}


//  process EVENT TRIGGER interrupt = software interrupt
void __attribute__((__interrupt__,__no_auto_psv__)) _EVENTL_INTERUPT(void) 
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;

	int16_t eventIndex;
	EVENT* pEvent;

	_EVENTL_TRIGGERIF = 0 ;			// clear the interrupt

	if(event_init_done)
	{
		for(eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++)
		{
			pEvent = &events[eventIndex];
			if( (pEvent->eventPending == true) && (pEvent->priority == EVENT_PRIORITY_LOW) )
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


void __attribute__((__interrupt__,__no_auto_psv__)) _EVENTM_INTERUPT(void) 
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;

	int16_t eventIndex;
	EVENT* pEvent;

	_EVENTM_TRIGGERIF = 0 ;			// clear the interrupt

	if(event_init_done)
	{
		for(eventIndex = 0; eventIndex < MAX_EVENTS; eventIndex++)
		{
			pEvent = &events[eventIndex];
			if( (pEvent->eventPending == true) && (pEvent->priority == EVENT_PRIORITY_MEDIUM) )
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

