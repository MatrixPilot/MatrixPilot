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

// Internal MPU6000 axis definition
// X axis pointing to right, Y axis pointing forward and Z axis pointing up


#ifndef NV_MEMORY_OPTIONS_H
#define NV_MEMORY_OPTIONS_H

////////////////////////////////////////////////////////////////////////////////
// Use I2C port 1 drivers
// Set to 1 to use and 0 to not include
// WARNING: Use on UDB4+ only
//#ifndef USE_I2C1_DRIVER
//#define USE_I2C1_DRIVER                 0
//#endif


////////////////////////////////////////////////////////////////////////////////
// Use non volatile memory to store and recall settings
// Set to 1 to include non volatile memory service in compilation
// WARNING: Use on UDB4+ only
// WARNING: Can only be used with SERIAL_MAVLINK and USE_I2C1_DRIVER
#define USE_NV_MEMORY                   0


////////////////////////////////////////////////////////////////////////////////
// Manual erase / format of non volatile memory table
// If set to 1, does manual erase of memory table when it runs.
// Use to go back to all hard coded defaults.
// Reset to 0 after clearing to use the non volatile memory again.
#define MANUAL_ERASE_TABLE              0


////////////////////////////////////////////////////////////////////////////////
// Use variable data width in HILSIM for output channels
//  This is used to support NUM_OUTPUTS > 8
// NOTE: Must have correct version of HILSIM to support this
#define USE_VARIABLE_HILSIM_CHANNELS    0

#endif // NV_MEMORY_OPTIONS_H
