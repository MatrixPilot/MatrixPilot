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


// USE_OSD enables the OSD system
#define USE_OSD         OSD_NONE
//#define USE_OSD         OSD_NATIVE
//#define USE_OSD         OSD_REMZIBI

#define USE_OSD_SPI     1   // set this to 1 to use the SPI peripheral, 0 to bit-bash
#define OSD_SF          5   // scale factor for SPI delays - TODO: get rid of

#define OSD_NTSC        0
#define OSD_PAL         1
