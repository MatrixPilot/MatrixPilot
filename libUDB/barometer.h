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


#ifndef BAROMETER_H
#define BAROMETER_H

typedef void (*barometer_callback_funcptr)(int32_t pressure, int16_t temperature, uint8_t status);

void rxBarometer(barometer_callback_funcptr);  	// service the barometer

#define I2C_Normal		I2C2_Normal
#define I2C_Read		I2C2_Read
#define I2C_Write		I2C2_Write
#define I2C_reset		I2C2_reset

#define BPXD                    0

//inline boolean bar_DataReady(void);

#endif // BAROMETER_H
