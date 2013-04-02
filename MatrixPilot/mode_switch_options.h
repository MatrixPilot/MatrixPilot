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

// Switch positions in rising order
// These are the positions of switches, not the position of thresholds between switches

// Add as many switch positions as you have or need

// Make sure there as as many defined states to match the switch positions
//  in the switch_pos_state and switch_pos_sub arrays

const uint16_t switch_positions[] = {2279, 2578, 2877, 3177, 3476, 3774};


const AUTOPILOT_MODE switch_pos_state[] = {
    FLIGHT_MODE_MANUAL,
    FLIGHT_MODE_MANUAL,
    FLIGHT_MODE_STABILIZED,
    FLIGHT_MODE_ASSISTED,
    FLIGHT_MODE_AUTONOMOUS,
    FLIGHT_MODE_AUTONOMOUS};


// Define the sub states used by each switch position
const AUTOPILOT_SUBMODE switch_pos_sub[] = {
    FLIGHT_SUBMODE_DEFAULT,
    FLIGHT_SUBMODE_DEFAULT,
    FLIGHT_SUBMODE_ASSIST,
    FLIGHT_SUBMODE_FBW,
    FLIGHT_SUBMODE_DEFAULT,
    FLIGHT_SUBMODE_DEFAULT};
