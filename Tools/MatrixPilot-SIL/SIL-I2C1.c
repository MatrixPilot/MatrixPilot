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

#if (WIN == 1 || NIX == 1)

#include "../../libUDB/libUDB_internal.h"
#include "../../libUDB/interrupt.h"
#include "../../libUDB/I2C.h"
#include "../../libUDB/NV_memory.h"
#include "../../libUDB/events.h"

#if (USE_I2C1_DRIVER == 1)

void I2C1_Init(void)
{
}

// Trigger the I2C1 service routine to run at low priority
void I2C1_Trigger(void)
{
}

boolean I2C1_Write(uint8_t address, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* ptxData, uint16_t txSize, I2C_callbackFunc pCallback)
{
	return true;
}

boolean I2C1_Read(uint8_t address, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* prxData, uint16_t rxSize, I2C_callbackFunc pCallback, uint16_t I2C_mode)
{
	return true;
}
// Only send command byte to check for ACK.
boolean I2C1_CheckACK(uint16_t address, I2C_callbackFunc pCallback)
{
	return true;
}

#endif // USE_I2C1_DRIVER
#endif // (WIN == 1 || NIX == 1)
