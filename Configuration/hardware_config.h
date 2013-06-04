// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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
// Set Up Board Type
// See the MatrixPilot wiki for more details on different board types.
#ifdef UDB4
#define BOARD_TYPE							UDB4_BOARD
#endif
#ifdef UDB5
#define BOARD_TYPE							UDB5_BOARD
#endif
#ifdef AUAV3
#define BOARD_TYPE							AUAV3_BOARD
#endif

#ifndef BOARD_TYPE
#define BOARD_TYPE							UDB5_BOARD
#endif


////////////////////////////////////////////////////////////////////////////////
// Use board orientation to change the mounting direction of the board.
// Note:
//	  For UDB4, X arrow points to the front, GPS connectors are on the front.
//	  For AUAV3, airplane symbol points to the front, GPS connector is at rear.
//
// The following 6 orientations have the board parallel with the ground.
// ORIENTATION_FORWARDS:  Component-side up,   GPS connector front
// ORIENTATION_BACKWARDS: Component-side up,   GPS connector back
// ORIENTATION_INVERTED:  Component-side down, GPS connector front
// ORIENTATION_FLIPPED:   Component-side down, GPS connector back
// ORIENTATION_YAWCW:	 Component-side up,   GPS connector to the right
// ORIENTATION_YAWCCW:	Component-side up,   GPS connector to the left
//
// The following 2 orientations are "knife edge" mountings
// ORIENTATION_ROLLCW: board rolled 90 degrees clockwise,
//		from point of view of the pilot
// ORIENTATION_ROLLCW180: board rolled 90 degrees clockwise,
//		from point of view of the pilot, then rotate the board 180 around the Z axis of the plane,
#define BOARD_ORIENTATION					ORIENTATION_FORWARDS


////////////////////////////////////////////////////////////////////////////////
// Choose your airframe type:
//	AIRFRAME_STANDARD		 	Elevator, and Ailerons and/or Rudder control
//	AIRFRAME_VTAIL			Ailerons(optional), and Elevator and Rudder as V-tail controls
//	AIRFRAME_DELTA			Aileron and Elevator as Elevons, and Rudder(optional)
// (Note that although AIRFRAME_HELI is also recognized, the code for this airframe type is not ready.)
#define AIRFRAME_TYPE						AIRFRAME_STANDARD


////////////////////////////////////////////////////////////////////////////////
// Set this value to your GPS type.  (Set to GPS_STD, GPS_UBX_2HZ, GPS_UBX_4HZ, or GPS_MTEK)
#define GPS_TYPE							GPS_UBX_4HZ


////////////////////////////////////////////////////////////////////////////////
// AUAV3 only options

////////////////////////////////////////////////////////////////////////////////
// At present, the AUAV3 schematic and 'installation & basic connections' document
// are drafts and hence there is some inconsistency in labelling conventions.
//
// The following standard labelling convention is proposed.
//
// AUAV3 schematic:
//		TLM		-	PORT1
//		OSD		-	PORT2
//		UART3	-	PORT3
//		GPS		-	PORT4
//
// 'AUAV3 Installation and Basic Connections' document:
//		OUART1	-	PORT1
//		OUART2	-	PORT2
//		UART3	-	PORT3
//		GPS		-	PORT4
//
////////////////////////////////////////////////////////////////////////////////
// On the AUAV3, the external UART connections are known as ports 1 through 4.
// The definitions below specifies which feature maps to an external port.
//
// NOTE: on the AUAV3, do not confuse the CONSOLE_UART definition with the 
// external port assignment.
// Assign the console to an internal UART with CONSOLE_UART, map this console to
// external port connection with DBG_PORT.
#define GPS_PORT							2
#define TLM_PORT							3
#define DBG_PORT							1

