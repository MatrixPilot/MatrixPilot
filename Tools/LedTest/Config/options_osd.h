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


// define the OSD types
#define OSD_NONE            0   // OSD disabled
#define OSD_NATIVE          1   // native OSD
#define OSD_REMZIBI         2   // Output data formatted to use as input to a Remzibi OSD
#define OSD_MINIM           3   // Output data formatted for minim OSD


// define USE_OSD to one of the types above to enables that system
#define USE_OSD           OSD_NONE
//#define USE_OSD         OSD_NATIVE
//#define USE_OSD         OSD_REMZIBI
//#define USE_OSD         OSD_MINIM


// OSD_NATIVE development options:
// Set OSD_LEGACY_SPI_PINOUT to 1 if you wired up your UDB4 or UDB5 for the Native OSD
// before October 2016; In this case, The wiring does not follow the labels on the UDB boards.
// Otherwise, leave OSD_LEGACY_SPI_PINOUT_UDB as 0, for the pin out labels on the UDB boards
// to agree with the SPI signals of the OSD.
#define OSD_LEGACY_SPI_PINOUT   0   // Default setting is 0
#define USE_OSD_SPI             0   // set this to 1 to use the SPI peripheral, 0 to bit-bash
#define OSD_SF                  5   // scale factor for SPI delays - TODO: get rid of
