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
  

#include "defines.h"
#define  MAX_PAUSE_TOGGLE  20  // 20 frames at 40Hz is 1/2 second.

enum FLIGHT_MODE_SWITCH_STATE
{
	ENT_MANUAL_LONG_TERM ,
	MANUAL_LONG_TERM ,
	ENT_STABILIZED_LONG_TERM ,
	STABILIZED_LONG_TERM ,
	ENT_MANUAL_T1 ,
	MANUAL_T1 ,
	ENT_STABILIZED_T1 ,
	STABILIZED_T1 ,
	MANUAL_T2 ,
	ENT_MANUAL_T2 ,
	ENT_AUTONOMOUS ,
	AUTONOMOUS ,
} ;
 
int flight_mode_switch_state = MANUAL_LONG_TERM ;
unsigned char request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL ;
unsigned int counter_40hz = 0 ;

void set_requested_flight_mode(void) 
// The functionality of this code allows a two state mode switch on the transmitter, to be used
// to create three flight modes. When switch is "Down" the plane always reverts to Manual. When "Up" the plane moves to Stabilized".
// If the user is in stabilized ("Up"), and then the user toggles the switch to Down, Up, Down, Up, then the plane moves to autonomous.
// Each toggle must be achieved with a limited time period (MAX_PAUSE_TOGGLE) and not faster than 1/40th of a second.
// When in Autonomous, a move to "Down" puts the switch state  back to Manual. And a futher move to "UP, will put the
// switch state back in stabilized. The important design concept is that Manual is always Manual immediately.
// Stabilized is Stabilized unless you try quite hard to reach Autonomous.
{
	switch ( flight_mode_switch_state )
    {
		case ENT_MANUAL_LONG_TERM:
			counter_40hz = 0 ;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL ;
			flight_mode_switch_state = MANUAL_LONG_TERM ;
			break ;	
		case MANUAL_LONG_TERM :
			if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW )
			{
				flight_mode_switch_state = ENT_STABILIZED_LONG_TERM ;
			}
			break ;
		case ENT_STABILIZED_LONG_TERM :
			counter_40hz = 0 ;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_STABILIZED ;
			flight_mode_switch_state = STABILIZED_LONG_TERM ;
			break ;
		case STABILIZED_LONG_TERM :
			if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW )
			{
				flight_mode_switch_state = ENT_MANUAL_T1 ;
			}
			break ;
		case ENT_MANUAL_T1 :
			counter_40hz = 0 ;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL ;
			flight_mode_switch_state = MANUAL_T1 ;
			break ;
		case MANUAL_T1 :
			if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW )
			{
				flight_mode_switch_state = ENT_STABILIZED_T1 ;
			}
			else
			{
				if (  counter_40hz > MAX_PAUSE_TOGGLE ) 
				{
					flight_mode_switch_state = ENT_MANUAL_LONG_TERM ;
				}
			}
			counter_40hz++ ;
			break ;
		case ENT_STABILIZED_T1 :
			counter_40hz = 0 ;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_STABILIZED ;
			flight_mode_switch_state = STABILIZED_T1 ;
			break ;
		case STABILIZED_T1 :
			if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW )
			{
				flight_mode_switch_state = ENT_MANUAL_T2 ;
			}
			else
			{
				if (  counter_40hz > MAX_PAUSE_TOGGLE ) 
				{
					flight_mode_switch_state = ENT_STABILIZED_LONG_TERM ;
				}
			}
			counter_40hz++ ;
			break ;
		case ENT_MANUAL_T2 :
			counter_40hz = 0 ;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL ;
			flight_mode_switch_state = MANUAL_T2 ;
			break ;	
		case MANUAL_T2 :
			if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW )
			{
				flight_mode_switch_state = ENT_AUTONOMOUS ;
			}
			else
			{
				if (  counter_40hz > MAX_PAUSE_TOGGLE ) 
				{
					flight_mode_switch_state = ENT_MANUAL_LONG_TERM ;
				}
			}
			counter_40hz++ ;
			break ;
		case ENT_AUTONOMOUS :
			counter_40hz = 0 ;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_AUTONOMOUS ;
			flight_mode_switch_state = AUTONOMOUS ;
			break ;
		case AUTONOMOUS :
			if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW )
			{
					flight_mode_switch_state = ENT_MANUAL_LONG_TERM ;
			}
			break ;
		default:
			flight_mode_switch_state = ENT_MANUAL_LONG_TERM ;
			break ;
	}
	return ;
}
