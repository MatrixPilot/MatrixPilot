// SEE END OF FILE FOR LICENSE TERMS

#include "libCAN_defines.h"

#ifndef CAN_INTERFACE_CONFIG_H
#define CAN_INTERFACE_CONFIG_H

//-----------------------------------------------------------------------------------------------------------------------
			// TIMING SETUP
//-----------------------------------------------------------------------------------------------------------------------

// Baud rate settings.
// Set at ^600kBaud which is non standard.
// Aim is to set for 1Mbaud

// Using 4FCY setting = crystal frequency = 16MHz (RED_BOARD)
#define FCY 		16000000             		// 30 MHz
#define BITRATE 	500000			 			// 500kbps
#define NTQ 		16							// Number of Tq cycles which will make the
												// CAN Bit Timing.
#define BRP_VAL		((FCY/(2*NTQ*BITRATE))-1)  //Formulae used for C1CFG1bits.BRP 

#define CAN_TIMING_CONFIG2 0x05E3;	//0x03D2;
									// SEG1PH=4Tq, SEG2PH=5Tq, PRSEG=3Tq 
                                	// Sample 3 times
                                	// Each bit time is 16Tq??


#endif


/****************************************************************************/
// This is part of the servo and radio interface software
//
// ServoInterface source code
//	http://code.google.com/p/rc-servo-interface
//
// Copyright 2010 ServoInterface Team
// See the AUTHORS.TXT file for a list of authors of ServoInterface.
//
// ServoInterface is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ServoInterface is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with ServoInterface.  If not, see <http://www.gnu.org/licenses/>.
//
// Many parts of ServoInterface use either modified or unmodified code
// from the MatrixPilot pilot project.
// The project also contains code for modifying MatrixPilot to operate
// with ServoInterface.
// For details, credits and licenses of MatrixPilot see the AUTHORS.TXT file.
// or see this website: http://code.google.com/p/gentlenav
/****************************************************************************/

