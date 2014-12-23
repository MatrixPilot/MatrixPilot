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


#ifndef I2C_H
#define I2C_H

#include "libUDB.h"

typedef void (*I2C_callbackFunc)(boolean);

typedef enum
{
	I2C_MODE_READ_ONLY,
	I2C_MODE_WRITE_ADDR_READ,
	I2C_MODE_WRITE,
} I2C_MODES; 

boolean I2C1_Write(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback);
boolean I2C1_Read(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback, uint16_t mode);
boolean I2C1_CheckAck(uint8_t addr, I2C_callbackFunc pCallback);
boolean I2C1_Normal(void);
void I2C1_Trigger(void);

boolean I2C2_Write(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback);
boolean I2C2_Read(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback, uint16_t mode);
boolean I2C2_CheckAck(uint8_t addr, I2C_callbackFunc callback);
boolean I2C2_Normal(void);
void I2C2_Trigger(void);
void I2C2_Reset(void);

#endif // I2C_H
