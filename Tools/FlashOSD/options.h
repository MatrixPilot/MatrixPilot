// This file is part of MatrixPilot FlashOSD.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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
// Set Up Board Type (Set to RED_BOARD, GREEN_BOARD, UDB3_BOARD, RUSTYS_BOARD, or UDB4_BOARD)
// If building for UDB4, use the MatrixPilot-udb4.mcp project file.
#define BOARD_TYPE 							RED_BOARD

#define CLOCK_CONFIG						FRC8X_CLOCK

////////////////////////////////////////////////////////////////////////////////
// Configure Input and Output Channels
//
// NUM_INPUTS: Set to 0-5 
//   1-4 enables only the first 1-4 of the 4 standard input channels
//   5 also enables E8 as the 5th input channel
#define NUM_INPUTS							0

// NUM_OUTPUTS: Set to 3, 4, 5, or 6
//   3 enables only the standard 3 output channels
//   4 also enables E0 as the 4th output channel
//   5 also enables E2 as the 5th output channel
//   6 also enables E4 as the 6th output channel
#define NUM_OUTPUTS							3


////////////////////////////////////////////////////////////////////////////////
// Control gains.
// All gains should be positive real numbers.

// SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want
#define SERVOSAT							1.0


////////////////////////////////////////////////////////////////////////////////
// On Screen Display
#define USE_OSD								1
