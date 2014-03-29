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
#include "mode_switch.h"

#define MAX_PAUSE_TOGGLE 20  // 20 frames at 40Hz is 1/2 second.

enum AUTOPILOT_MODE
{
	FLIGHT_MODE_SWITCH_MANUAL,
	FLIGHT_MODE_SWITCH_STABILIZED,
	FLIGHT_MODE_SWITCH_AUTONOMOUS,
};

enum FLIGHT_MODE_SWITCH_STATE
{
	ENT_MANUAL_LONG_TERM,
	MANUAL_LONG_TERM,
	ENT_STABILIZED_LONG_TERM,
	STABILIZED_LONG_TERM,
	ENT_MANUAL_T1,
	MANUAL_T1,
	ENT_STABILIZED_T1,
	STABILIZED_T1,
	MANUAL_T2,
	ENT_MANUAL_T2,
	ENT_AUTONOMOUS,
	AUTONOMOUS,
};
 
#if (MODE_SWITCH_TWO_POSITION == 1)
static int16_t mode_switch_state = MANUAL_LONG_TERM;
static uint8_t mode_switch_request_mode = FLIGHT_MODE_SWITCH_MANUAL;
static uint16_t mode_switch_toggle_counter_40hz = 0;
#endif // MODE_SWITCH_TWO_POSITION

struct mode_switch_flags_bits {
	uint16_t man_req                    : 1;
	uint16_t auto_req                   : 1;
	uint16_t home_req                   : 1;
	uint16_t rtl_hold                   : 1;
};

union mode_switch_flags_int { struct mode_switch_flags_bits _; int16_t WW; };

static union mode_switch_flags_int mode_switch_flags;
static union mode_switch_flags_int mode_switch_flags_old;

void mode_switch_init(void)
{
	mode_switch_flags.WW = 0;
	mode_switch_flags_old.WW = 0;
}

boolean mode_switch_manual_chk(void)
{
	return mode_switch_flags._.man_req;
}

boolean mode_switch_stabilize_chk(void)
{
	return mode_switch_flags._.auto_req;
}

boolean mode_switch_waypoints_chk(void)
{
	return mode_switch_flags._.home_req;
}

boolean mode_switch_rtl_hold_chk(void)
{
	return mode_switch_flags._.rtl_hold;
}

void mode_switch_rtl_hold_set(void)
{
	mode_switch_flags._.rtl_hold = 1;
}

// The functionality of this code allows a two state mode switch on the transmitter, to be used
// to create three flight modes. When switch is "Down" the plane always reverts to Manual. When "Up" the plane moves to Stabilized".
// If the user is in stabilized ("Up"), and then the user toggles the switch to Down, Up, Down, Up, then the plane moves to autonomous.
// Each toggle must be achieved with a limited time period (MAX_PAUSE_TOGGLE) and not faster than 1/40th of a second.
// When in Autonomous, a move to "Down" puts the switch state back to Manual. And a futher move to "UP, will put the
// switch state back in stabilized. The important design concept is that Manual is always Manual immediately.
// Stabilized is Stabilized unless you try quite hard to reach Autonomous.
void mode_switch_2pos_poll(void) // this is called at 40 hertz
{
// TODO: potential bug below when the mode switch input channel is exactly equal to MODE_SWITCH_THRESHOLD_LOW - RobD
#if (MODE_SWITCH_TWO_POSITION == 1)
	switch (mode_switch_state)
	{
		case ENT_MANUAL_LONG_TERM:
			mode_switch_toggle_counter_40hz = 0;
			mode_switch_request_mode = FLIGHT_MODE_SWITCH_MANUAL;
			mode_switch_state = MANUAL_LONG_TERM;
			break;
		case MANUAL_LONG_TERM:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW)
			{
				mode_switch_state = ENT_STABILIZED_LONG_TERM;
			}
			break;
		case ENT_STABILIZED_LONG_TERM:
			mode_switch_toggle_counter_40hz = 0;
			mode_switch_request_mode = FLIGHT_MODE_SWITCH_STABILIZED;
			mode_switch_state = STABILIZED_LONG_TERM;
			break;
		case STABILIZED_LONG_TERM:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
			{
				#if (FLY_BY_DATALINK_ENABLED == 0)
				// when using fbdl, we are *always* in stabilized mode
				mode_switch_state = ENT_MANUAL_T1;
				#endif
			}
			break;
		case ENT_MANUAL_T1:
			mode_switch_toggle_counter_40hz = 0;
			#if (FLY_BY_DATALINK_ENABLED == 1)
			// when using fbdl, we are *always* in stabilized mode
			mode_switch_state = FLIGHT_MODE_SWITCH_STABILIZED;
			mode_switch_state = STABILIZED_T1;
			#else
			mode_switch_request_mode = FLIGHT_MODE_SWITCH_MANUAL;
			mode_switch_state = MANUAL_T1;
			#endif
			break;
		case MANUAL_T1:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW)
			{
				mode_switch_state = ENT_STABILIZED_T1;
			}
			else
			{
				if (mode_switch_toggle_counter_40hz > MAX_PAUSE_TOGGLE)
				{
					mode_switch_state = ENT_MANUAL_LONG_TERM;
				}
			}
			mode_switch_toggle_counter_40hz++;
			break;
		case ENT_STABILIZED_T1:
			mode_switch_toggle_counter_40hz = 0;
			mode_switch_request_mode = FLIGHT_MODE_SWITCH_STABILIZED;
			mode_switch_state = STABILIZED_T1;
			break;
		case STABILIZED_T1:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
			{
				#if (FLY_BY_DATALINK_ENABLED == 0)
				// when using fbdl, we are *always* in stabilized mode
				mode_switch_state = ENT_MANUAL_T2;
				#endif
			}
			else
			{
				if (mode_switch_toggle_counter_40hz > MAX_PAUSE_TOGGLE)
				{
					mode_switch_state = ENT_STABILIZED_LONG_TERM;
				}
			}
			mode_switch_toggle_counter_40hz++;
			break;
		case ENT_MANUAL_T2:
			mode_switch_toggle_counter_40hz = 0;
			#if (FLY_BY_DATALINK_ENABLED == 1)
			// when using fbdl, we are *always* in stabilized mode
			mode_switch_request_mode = FLIGHT_MODE_SWITCH_STABILIZED;
			mode_switch_state = STABILIZED_T1;
			#else
			mode_switch_request_mode = FLIGHT_MODE_SWITCH_MANUAL;
			mode_switch_state = MANUAL_T2;
			#endif
			break;
		case MANUAL_T2:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW)
			{
				mode_switch_state = ENT_AUTONOMOUS;
			}
			else
			{
				if ( mode_switch_toggle_counter_40hz > MAX_PAUSE_TOGGLE) 
				{
					mode_switch_state = ENT_MANUAL_LONG_TERM;
				}
			}
			mode_switch_toggle_counter_40hz++;
			break;
		case ENT_AUTONOMOUS:
			mode_switch_toggle_counter_40hz = 0;
			mode_switch_request_mode = FLIGHT_MODE_SWITCH_AUTONOMOUS;
			mode_switch_state = AUTONOMOUS;
			break;
		case AUTONOMOUS:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
			{
				mode_switch_state = ENT_MANUAL_LONG_TERM;
			}
			break;
		default:
			mode_switch_state = ENT_MANUAL_LONG_TERM;
			break;
	}
#endif // MODE_SWITCH_TWO_POSITION
}

void mode_switch_check_set(boolean radio_on)
{
	if (radio_on)
	{
#if (MODE_SWITCH_TWO_POSITION == 1)
		switch  (mode_switch_request_mode)
		{
			case FLIGHT_MODE_SWITCH_AUTONOMOUS:
				mode_switch_flags._.man_req = 0;
				mode_switch_flags._.auto_req = 0;
				mode_switch_flags._.home_req = 1;
				break;
			case FLIGHT_MODE_SWITCH_STABILIZED:
				mode_switch_flags._.man_req = 0;
				mode_switch_flags._.auto_req = 1;
				mode_switch_flags._.home_req = 0;
				break;
			case FLIGHT_MODE_SWITCH_MANUAL:
				mode_switch_flags._.man_req = 1;
				mode_switch_flags._.auto_req = 0;
				mode_switch_flags._.home_req = 0;
				break;
			default: // Put autopilot in Manual Mode
				mode_switch_flags._.man_req = 1;
				mode_switch_flags._.auto_req = 0;
				mode_switch_flags._.home_req = 0;
				break;
		}
#else // Three Mode Switch
		// Select manual, automatic, or come home, based on pulse width of the switch input channel as defined in options.h.
		if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_HIGH)
		{
			mode_switch_flags._.man_req = 0;
			mode_switch_flags._.auto_req = 0;
			mode_switch_flags._.home_req = 1;
		}
		else if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW)
		{
			mode_switch_flags._.man_req = 0;
			mode_switch_flags._.auto_req = 1;
			mode_switch_flags._.home_req = 0;
		}
		else
		{
			#if (FLY_BY_DATALINK_ENABLED == 1)
			// when using fbdl, we are *always* in stabilized mode
			mode_switch_flags._.man_req = 0;
			mode_switch_flags._.auto_req = 1;
			mode_switch_flags._.home_req = 0;
			
			#else
			mode_switch_flags._.man_req = 1;
			mode_switch_flags._.auto_req = 0;
			mode_switch_flags._.home_req = 0;
			#endif
		}
#endif // MODE_SWITCH_TWO_POSITION
		// With Failsafe Hold enabled: After losing RC signal, and then regaining it, you must manually
		// change the mode switch position in order to exit RTL mode.
		if (mode_switch_flags._.rtl_hold)
		{
			if (mode_switch_flags._.man_req  == mode_switch_flags_old._.man_req &&
				mode_switch_flags._.auto_req == mode_switch_flags_old._.auto_req &&
				mode_switch_flags._.home_req == mode_switch_flags_old._.home_req)
			{
				mode_switch_flags._.man_req = 0;
				mode_switch_flags._.auto_req = 0;
				mode_switch_flags._.home_req = 0;
			}
			else
			{
				mode_switch_flags_old.WW = mode_switch_flags.WW;
				mode_switch_flags._.rtl_hold = 0;
			}
		}
		else
		{
			mode_switch_flags_old.WW = mode_switch_flags.WW;
		}
	}
	else
	{
		mode_switch_flags._.man_req = 0;
		mode_switch_flags._.auto_req = 0;
		mode_switch_flags._.home_req = 1;
	}
}
