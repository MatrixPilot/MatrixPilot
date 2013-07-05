// This file is part of MatrixPilot FlashOSD.
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


////////////////////////////////////////////////////////////////////////////////
// options.h
// Bill Premerlani's UAV Dev Board
//
// This file includes all of the user-configuration for this firmware,
//


////////////////////////////////////////////////////////////////////////////////
// Set Up Board Type
// GREEN_BOARD - Board is green and includes 2 vertical gyro daugter-boards.
// RED_BOARD   - Board is red, and includes 2 vertical gyro daugter-boards.
// UDB3_BOARD  - Board is red, and includes a single, flat, multi-gyro daugter-board.
// See the MatrixPilot wiki for more details on different UDB boards.
// If building for UDB4, use the RollPitchYaw-udb4.mcp project file.
#define BOARD_TYPE                          AUAV3_BOARD


////////////////////////////////////////////////////////////////////////////////
// Configure Input and Output Channels
//
// NUM_INPUTS: Set to 0-5 
//   1-4 enables only the first 1-4 of the 4 standard input channels
//   5 also enables E8 as the 5th input channel
#define NUM_INPUTS                          0

// NUM_OUTPUTS: Set to 3, 4, 5, or 6
//   3 enables only the standard 3 output channels
//   4 also enables E0 as the 4th output channel
//   5 also enables E2 as the 5th output channel
//   6 also enables E4 as the 6th output channel
#define NUM_OUTPUTS                         3


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.
// Proportional gains should be less than 4.0.
// Rate gains should be less than 0.8.
// Proportional gains include ROLLKP, YAWKP_AILERON, AILERON_BOOST, PITCHGAIN,
// RUDDER_ELEV_MIX, ROLL_ELEV_MIX, ELEVATOR_BOOST, YAWKP_RUDDER, ROLLKP_RUDDER,
// MANUAL_AILERON_RUDDER_MIX, RUDDER_BOOST, HOVER_ROLLKP, HOVER_PITCHGAIN, HOVER_YAWKP
// Rate gains include ROLLKD, YAWKD_AILERON, PITCHKD, YAWKD_RUDDER, ROLLKD_RUDDER,
// HOVER_ROLLKD, HOVER_PITCHKD, HOVER_YAWKD

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT                            1.0


////////////////////////////////////////////////////////////////////////////////
// On Screen Display
#define USE_OSD                             1
