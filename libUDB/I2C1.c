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

#include "I2C.h"
/*
#include "libUDB_internal.h"
#include "interrupt.h"
#include "I2C.h"
#include "NV_memory.h"
#include "events.h"
 */

//#undef USE_I2C1_DRIVER
#define USE_I2C1_DRIVER 1

#if (USE_I2C1_DRIVER == 1)

#if (BOARD_TYPE == AUAV3_BOARD)
#define I2C_SDA         _RD9
#define I2C_SCL         _RD10
#else // UDB4 or 5
#define I2C_SDA         _RG3
#define I2C_SCL         _RG2
//#define I2C_SDA_TRIS    _TRISG3
//#define I2C_SCL_TRIS    _TRISG2
#endif // BOARD_TYPE

#define I2C_IP          _MI2C1IP
#define I2C_IE          _MI2C1IE
#define I2C_IF          _MI2C1IF
#define I2C_CON         I2C1CON
#define I2C_PEN         I2C1CONbits.PEN
#define I2C_SEN         I2C1CONbits.SEN
#define I2C_RCEN        I2C1CONbits.RCEN
#define I2C_ACKDT       I2C1CONbits.ACKDT
#define I2C_ACKEN       I2C1CONbits.ACKEN
#define I2C_EN          I2C1CONbits.I2CEN
#define I2C_ACKSTAT     I2C1STATbits.ACKSTAT
#define I2C_STAT        I2C1STAT
#define I2C_RCV         I2C1RCV
#define I2C_TRN         I2C1TRN
#define I2C_RCV         I2C1RCV

#define INT_PRI_I2C     INT_PRI_I2C1
#define MI2C            MI2C1
#define I2C_Normal      I2C1_Normal
#define I2C_Reset       I2C1_Reset
#define I2C_Write       I2C1_Write
#define I2C_Read        I2C1_Read
#define I2C_CheckAck    I2C1_CheckAck
#define I2C_Trigger     I2C1_Trigger

#define I2C_SOURCE_INCLUDED
#include "I2C.c"

#endif // USE_I2C1_DRIVER
