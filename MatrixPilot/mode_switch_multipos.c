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

//	Select manual, automatic, or come home, based on pulse width of the switch input channel as defined in mode_switch_options.h.

#include "defines.h"
#include "mode_switch_options.h"

#define SWITCH_POSITION_COUNT sizeof(switch_positions) / sizeof(unsigned int)
#define SWITCH_MODE_COUNT sizeof(switch_positions) / sizeof(AUTOPILOT_MODE)
#define SWITCH_SUBMODE_COUNT sizeof(switch_positions) / sizeof(AUTOPILOT_SUBMODE)

//#if(SWITCH_POSITION_COUNT != SWITCH_MODE_COUNT)
//    #error("switch_pos_state does not match the size of switch_positions")
//#endif
//
//#if(SWITCH_POSITION_COUNT != SWITCH_SUBMODE_COUNT)
//    #error("switch_pos_sub does not match the size of switch_positions")
//#endif

AUTOPILOT_MODE      request_mode        = FLIGHT_MODE_MANUAL;
AUTOPILOT_SUBMODE   request_submode     = FLIGHT_SUBMODE_DEFAULT;

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

AUTOPILOT_MODE get_requested_flightmode(void)
{
    return request_mode;
}


void flight_mode_switch_2pos_poll(void) // this is called at 40 hertz
{
    return;
}

// Called at 2 Hz

void flight_mode_switch_check_set(void)
{
        unsigned int mode       = FLIGHT_MODE_MANUAL;
        unsigned int submode    = FLIGHT_MODE_MANUAL;

    if (udb_flags._.radio_on)
    {
        unsigned int index;
        unsigned int threshold;
        unsigned int switchpos = udb_pwIn[MODE_SWITCH_INPUT_CHANNEL];
        unsigned int found_index = 0;

        for (index = 0; index < (SWITCH_POSITION_COUNT - 1); index++)
        {
            // Take the average of two switch positions to find the threshold
            threshold = switch_positions[index] + switch_positions[index + 1];
            threshold >>= 1;
            if (switchpos > threshold)
            {
                found_index = index + 1;
            }
        };

        // Collect the correct state for the switch position
        mode = switch_pos_state[found_index];
        submode = switch_pos_sub[found_index];


        // With Failsafe Hold enabled: After losing RC signal, and then regaining it, you must manually
        // change the mode switch position in order to exit RTL mode.
        if (flags._.rtl_hold)
        {
            if ((mode != request_mode) || (submode != request_submode))
            {
                request_mode = mode;
                request_submode = submode;
                flags._.rtl_hold = 0;
            }
        } else
        {
			if(request_mode == mode)
			{
	            request_submode = submode;
				set_submode( request_submode );
			}
			else
	            request_mode = mode;
        }
    } else
    {
        request_mode = FLIGHT_MODE_NO_RADIO;
    }
}


AUTOPILOT_SUBMODE get_requested_submode(void)
{
	return request_submode;
}