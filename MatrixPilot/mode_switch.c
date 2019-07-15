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
#include "states.h"
#include "mode_switch.h"
#include "../libUDB/heartbeat.h"
#include "../Config/options_multicopter.h"

#define  MAX_PAUSE_TOGGLE  PID_HZ/2  // 20 frames at 40Hz is 1/2 second.

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
static int16_t flight_mode_switch_state = MANUAL_LONG_TERM;
static uint8_t request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL;
static uint16_t toggle_switch_counter_40hz = 0;
#endif // MODE_SWITCH_TWO_POSITION
static union state_flags_int old_rtl_flags;


int16_t flight_mode_switch_manual(void)
{
	return state_flags._.man_req;
}

int16_t flight_mode_switch_stabilize(void)
{
	return state_flags._.auto_req;
}

int16_t flight_mode_switch_waypoints(void)
{
	return state_flags._.home_req;
}

// The functionality of this code allows a two state mode switch on the transmitter, to be used
// to create three flight modes. When switch is "Down" the plane always reverts to Manual. When "Up" the plane moves to Stabilized".
// If the user is in stabilized ("Up"), and then the user toggles the switch to Down, Up, Down, Up, then the plane moves to autonomous.
// Each toggle must be achieved with a limited time period (MAX_PAUSE_TOGGLE) and not faster than 1/40th of a second.
// When in Autonomous, a move to "Down" puts the switch state back to Manual. And a futher move to "UP, will put the
// switch state back in stabilized. The important design concept is that Manual is always Manual immediately.
// Stabilized is Stabilized unless you try quite hard to reach Autonomous.
void flight_mode_switch_2pos_poll(void) // this is called at 40 hertz
{
// TODO: potential bug below when the mode switch input channel is exactly equal to MODE_SWITCH_THRESHOLD_LOW - RobD
#if (MODE_SWITCH_TWO_POSITION == 1)
	switch (flight_mode_switch_state)
	{
		case ENT_MANUAL_LONG_TERM:
			toggle_switch_counter_40hz = 0;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL;
			flight_mode_switch_state = MANUAL_LONG_TERM;
			break;
		case MANUAL_LONG_TERM:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW)
			{
				flight_mode_switch_state = ENT_STABILIZED_LONG_TERM;
			}
			break;
		case ENT_STABILIZED_LONG_TERM:
			toggle_switch_counter_40hz = 0;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_STABILIZED;
			flight_mode_switch_state = STABILIZED_LONG_TERM;
			break;
		case STABILIZED_LONG_TERM:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
			{
				#if (FLY_BY_DATALINK_ENABLED == 0)
				// when using fbdl, we are *always* in stabilized mode
				flight_mode_switch_state = ENT_MANUAL_T1;
				#endif
			}
			break;
		case ENT_MANUAL_T1:
			toggle_switch_counter_40hz = 0;
			#if (FLY_BY_DATALINK_ENABLED == 1)
			// when using fbdl, we are *always* in stabilized mode
			flight_mode_switch_state = FLIGHT_MODE_SWITCH_STABILIZED;
			flight_mode_switch_state = STABILIZED_T1;
			#else
			request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL;
			flight_mode_switch_state = MANUAL_T1;
			#endif
			break;
		case MANUAL_T1:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW)
			{
				flight_mode_switch_state = ENT_STABILIZED_T1;
			}
			else
			{
				if (toggle_switch_counter_40hz > MAX_PAUSE_TOGGLE)
				{
					flight_mode_switch_state = ENT_MANUAL_LONG_TERM;
				}
			}
			toggle_switch_counter_40hz++;
			break;
		case ENT_STABILIZED_T1:
			toggle_switch_counter_40hz = 0;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_STABILIZED;
			flight_mode_switch_state = STABILIZED_T1;
			break;
		case STABILIZED_T1:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
			{
				#if (FLY_BY_DATALINK_ENABLED == 0)
				// when using fbdl, we are *always* in stabilized mode
				flight_mode_switch_state = ENT_MANUAL_T2;
				#endif
			}
			else
			{
				if (toggle_switch_counter_40hz > MAX_PAUSE_TOGGLE)
				{
					flight_mode_switch_state = ENT_STABILIZED_LONG_TERM;
				}
			}
			toggle_switch_counter_40hz++;
			break;
		case ENT_MANUAL_T2:
			toggle_switch_counter_40hz = 0;
			#if (FLY_BY_DATALINK_ENABLED == 1)
			// when using fbdl, we are *always* in stabilized mode
			request_autopilot_mode = FLIGHT_MODE_SWITCH_STABILIZED;
			flight_mode_switch_state = STABILIZED_T1;
			#else
			request_autopilot_mode = FLIGHT_MODE_SWITCH_MANUAL;
			flight_mode_switch_state = MANUAL_T2;
			#endif
			break;
		case MANUAL_T2:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW)
			{
				flight_mode_switch_state = ENT_AUTONOMOUS;
			}
			else
			{
				if ( toggle_switch_counter_40hz > MAX_PAUSE_TOGGLE) 
				{
					flight_mode_switch_state = ENT_MANUAL_LONG_TERM;
				}
			}
			toggle_switch_counter_40hz++;
			break;
		case ENT_AUTONOMOUS:
			toggle_switch_counter_40hz = 0;
			request_autopilot_mode = FLIGHT_MODE_SWITCH_AUTONOMOUS;
			flight_mode_switch_state = AUTONOMOUS;
			break;
		case AUTONOMOUS:
			if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
			{
				flight_mode_switch_state = ENT_MANUAL_LONG_TERM;
			}
			break;
		default:
			flight_mode_switch_state = ENT_MANUAL_LONG_TERM;
			break;
	}
#endif // MODE_SWITCH_TWO_POSITION
}
extern int control_mode;

void control_mode_switch_check_set(void)
{
	if (udb_flags._.radio_on)
	{
		if (udb_pwIn[CTRL_MODE_SWITCH_INPUT_CHANNEL] < CTRL_MODE_SWITCH_THRESHOLD_LOW)
		{
			control_mode = TILT_MODE;
		}
		else if (udb_pwIn[CTRL_MODE_SWITCH_INPUT_CHANNEL] < CTRL_MODE_SWITCH_THRESHOLD_HIGH )
		{
			control_mode = RATE_MODE;
		}
		else
		{
			#if (FLY_BY_DATALINK_ENABLED == 1)
			// when using fbdl, we are *always* in stabilized mode
			control_mode = TILT_MODE;

			#else
			control_mode = COMPASS_MODE;
			#endif
		}

        }
}

void flight_mode_switch_check_set(void)
{
	if (udb_flags._.radio_on)
	{
#if (MODE_SWITCH_TWO_POSITION == 1)
		switch  (request_autopilot_mode)
		{
			case FLIGHT_MODE_SWITCH_AUTONOMOUS:
				state_flags._.man_req = 0;
				state_flags._.auto_req = 0;
				state_flags._.home_req = 1;
				break;
			case FLIGHT_MODE_SWITCH_STABILIZED:
				state_flags._.man_req = 0;
				state_flags._.auto_req = 1;
				state_flags._.home_req = 0;
				break;
			case FLIGHT_MODE_SWITCH_MANUAL:
				state_flags._.man_req = 1;
				state_flags._.auto_req = 0;
				state_flags._.home_req = 0;
				break;
			default: // Put autopilot in Manual Mode
				state_flags._.man_req = 1;
				state_flags._.auto_req = 0;
				state_flags._.home_req = 0;
				break;
		}
#else // Three Mode Switch
		// Select manual, automatic, or come home, based on pulse width of the switch input channel as defined in options.h.
		if (udb_pwIn[FLIGHT_MODE_SWITCH_INPUT_CHANNEL] > FLIGHT_MODE_SWITCH_THRESHOLD_HIGH)
		{
			state_flags._.man_req = 0;
			state_flags._.auto_req = 0;
			state_flags._.home_req = 1;
		}
		else if (udb_pwIn[FLIGHT_MODE_SWITCH_INPUT_CHANNEL] > FLIGHT_MODE_SWITCH_THRESHOLD_LOW)
		{
			state_flags._.man_req = 0;
			state_flags._.auto_req = 1;
			state_flags._.home_req = 0;
		}
		else
		{
			#if (FLY_BY_DATALINK_ENABLED == 1)
			// when using fbdl, we are *always* in stabilized mode
			state_flags._.man_req = 0;
			state_flags._.auto_req = 1;
			state_flags._.home_req = 0;
			
			#else
			state_flags._.man_req = 1;
			state_flags._.auto_req = 0;
			state_flags._.home_req = 0;
			#endif
		}
#endif // MODE_SWITCH_TWO_POSITION
		// With Failsafe Hold enabled: After losing RC signal, and then regaining it, you must manually
		// change the mode switch position in order to exit RTL mode.
		if (state_flags._.rtl_hold)
		{
			if (state_flags._.man_req  == old_rtl_flags._.man_req &&
				state_flags._.auto_req == old_rtl_flags._.auto_req &&
				state_flags._.home_req == old_rtl_flags._.home_req)
			{
				state_flags._.man_req = 0;
				state_flags._.auto_req = 0;
				state_flags._.home_req = 0;
			}
			else
			{
				old_rtl_flags.WW = state_flags.WW;
				state_flags._.rtl_hold = 0;
			}
		}
		else
		{
			old_rtl_flags.WW = state_flags.WW;
		}
	}
	else
	{
		state_flags._.man_req = 0;
		state_flags._.auto_req = 0;
		state_flags._.home_req = 1;
	}
}
