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

/*
#include "libUDB_internal.h"
#include "interrupt.h"
#include "I2C.h"
//#include "NV_memory.h"
#include "events.h"
 */
#define USE_I2C_SECOND_PORT_DRIVER 1
#if (USE_I2C_SECOND_PORT_DRIVER == 1)

#if (BOARD_TYPE == AUAV3_BOARD)
#define I2C_SDA         _RA3
#define I2C_SCL         _RA2
#else // UDB4 or 5
#define I2C_SDA         _RA3
#define I2C_SCL         _RA2
//#define I2C_SDA_TRIS    _TRISA3
//#define I2C_SCL_TRIS    _TRISA2
#endif // BOARD_TYPE

#define I2C_IP          _MI2C2IP
#define I2C_IE          _MI2C2IE
#define I2C_IF          _MI2C2IF
#define I2C_CON         I2C2CON
#define I2C_PEN         I2C2CONbits.PEN
#define I2C_SEN         I2C2CONbits.SEN
#define I2C_RCEN        I2C2CONbits.RCEN
#define I2C_ACKDT       I2C2CONbits.ACKDT
#define I2C_ACKEN       I2C2CONbits.ACKEN
#define I2C_EN          I2C2CONbits.I2CEN
#define I2C_ACKSTAT     I2C2STATbits.ACKSTAT
#define I2C_STAT        I2C2STAT
#define I2C_RCV         I2C2RCV
#define I2C_TRN         I2C2TRN
#define I2C_RCV         I2C2RCV

#define INT_PRI_I2C     INT_PRI_I2C2
#define MI2C            MI2C2
#define I2C_Normal      I2C2_Normal
#define I2C_Reset       I2C2_Reset
#define I2C_Write       I2C2_Write
#define I2C_Read        I2C2_Read
#define I2C_CheckAck    I2C2_CheckACK
#define I2C_Trigger     I2C2_trigger_service

#define I2C_SOURCE_INCLUDED
#include "I2C.c"

#endif // USE_I2C_SECOND_PORT_DRIVER
