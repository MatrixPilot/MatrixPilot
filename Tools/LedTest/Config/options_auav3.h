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
// AUAV3 only options

////////////////////////////////////////////////////////////////////////////////
// At present, the AUAV3 schematic and 'installation & basic connections' document
// are drafts and hence there is some inconsistency in labelling conventions.
//
// The following standard labelling convention is proposed.
//
// AUAV3 schematic:
//        TLM      -    PORT1
//        OSD      -    PORT2
//        UART3    -    PORT3
//        GPS      -    PORT4
//
// 'AUAV3 Installation and Basic Connections' document:
//        OUART1   -    PORT1
//        OUART2   -    PORT2
//        UART3    -    PORT3
//        GPS      -    PORT4
//
////////////////////////////////////////////////////////////////////////////////
// On the AUAV3, the external UART connections are known as ports 1 through 4.
// The definitions below specifies which feature maps to an external port.
//
// NOTE: on the AUAV3, do not confuse the CONSOLE_UART definition with the 
// external port assignment.
// Assign the console to an internal UART with CONSOLE_UART, map this console to
// external port connection with DBG_PORT.
//#define GPS_PORT                            4
//#define TLM_PORT                            3
//#define DBG_PORT                            1

// Set this to 1 to enable the USB stack on AUAV3
//#define USE_USB                             1

// Set this to 1 to enable the Mass Storage Driver support over USB on AUAV3
//#define USE_MSD                             1

// Set this to 1 to enable the UART communications support over USB on AUAV3
//#define USE_CDC                             1
