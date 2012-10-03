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

#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE == AUAV2_BOARD))

#ifndef I2C_H
#define I2C_H

//******************************************************************
// I2C driver for UDB4/UDB5
//
// To write 
//		Set I2C1_rx_data_size to zero
//		Set I2C1_tx_data_size to data size plus address size
//		Set I2C1_writeCommandByte
//		Set pI2C1txBuffer to transmit data buffer
//
// To read 
//		set I2C1_tx_data_size to the size of address in bytes
//		Set I2C1_writeCommandByte
//		Set I2C1_rx_data_size to number of bytes to read
//		Set pI2C1txBuffer to transmit data buffer
//		Set pI2C1rxBuffer to receive data buffer
// Read will send the address as a write followed by the read
//

#include "libUDB_internal.h"

// callback type for I2C user
typedef void (*I2C_callbackFunc)(boolean);

// Start a transaction and take ownership of I2C bus.
// returns false if I2C is busy or not initialized
// command = command specific to device
// ptxData = pointer to transmit data buffer
// prxData = pointer to recieve data buffer
// txSize = size of transmited data in bytes
// rxSize = size of received data in bytes
// pCallback = pointer to callback function for finish or error.
extern boolean I2C1_Write(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* ptxData, unsigned int txSize, I2C_callbackFunc pCallback);

extern boolean I2C1_Read(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* prxData, unsigned int rxSize, I2C_callbackFunc pCallback);

// Check for I2C ACK on command
extern boolean I2C1_checkACK(unsigned int command, I2C_callbackFunc pCallback);

// Trigger the I2C1 service routine to run at low priority
extern void I2C1_trigger_service(void);

// Trigger the I2C1 service routine to run at low priority
extern void I2C1_init(void);

#endif	//I2C_H

#endif	// UDB4 BOARD
