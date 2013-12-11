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


#include "libUDB_internal.h"
#include "interrupt.h"
#include "I2C.h"
#include "NV_memory.h"
#include "events.h"

#ifndef I2C_SOURCE_INCLUDED
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
#define I2C_CheckAck    I2C1_CheckACK
#define I2C_Trigger     I2C1_trigger_service
#endif // I2C_SOURCE_INCLUDED

// Calculate the BRGvalue automatically - TODO: check and enable this
//#define I2CBRGVAL ((int)(((1/100e3) - 130E-9) * FCY)-2) // 100 Khz
#define I2CBRGVAL 60 // 200 Khz
#define I2C_NORMAL (((I2C_CON  & 0b0000000000011111) == 0) && \
                    ((I2C_STAT & 0b0100010011000001) == 0))

static void I2C_Init(void);
static void serviceI2C(void);

static void isr_failed(void);
//static void isr_start(void);
static void isr_idle(void);
static void isr_doneRead(void);
static void isr_recstore(void);
static void isr_rerecen(void);
static void isr_recen(void);
static void isr_writeStop(void);
static void isr_stopRead(void);
static void isr_writeData(void);
static void isr_readAddr(void);
static void isr_writeAddr(void);
static void isr_startWrite(void);
static void isr_readStart(void);
static void isr_doneWrite(void);
static void isr_writeCmdData(void);

struct I2C_xfer {
	boolean  busy;
	uint8_t  addr;
	const uint8_t* cmd;
	uint16_t cmd_len;
	uint8_t* data;
//	uint16_t data_len;
	uint16_t tx_data_len;
	uint16_t rx_data_len;
	I2C_callbackFunc callback;
	uint16_t mode;
	uint16_t index;
	int16_t error;
	void (*state)(void);
};
static struct I2C_xfer x;
static uint16_t I2C_srv_handle = INVALID_HANDLE;

// Determine if the bus is normal
boolean I2C_Normal(void)
{
	if (I2C_EN == 0)        // I2C is off
	{
		I2C_Init();
	}
	if (I2C_NORMAL)
	{
		return true;
	}
	else
	{
		x.error = I2C_STAT;
		return false;
	}
}

// Reset the bus
void I2C_Reset(void)
{
	x.state = &isr_idle;    // disable the response to any more interrupts
	x.error = I2C_STAT;     // record the error for diagnostics
	I2C_EN = 0;             // turn off the I2C
	I2C_IF = 0;             // clear the I2C master interrupt
	I2C_IE = 0;             // disable the interrupt
	I2C_SDA = 0;            // pull SDA and SCL low
	I2C_SCL = 0;
	Nop();
	I2C_SDA = 1;            // pull SDA and SCL high
	I2C_SCL = 1;
	I2C_Init();             // enable the bus again
}

static void I2C_Init(void)
{
	I2C2BRG  = I2CBRGVAL;
	I2C_EN   = 1;           // enable I2C peripheral
	I2C_IP   = INT_PRI_I2C; // set interrupt priority
	I2C_IF   = 0;           // clear the I2C master interrupt
	I2C_IE   = 1;           // enable the interrupt
	if (I2C_srv_handle == INVALID_HANDLE)
	{
		I2C_srv_handle = register_event(&serviceI2C);
	}
	x.busy = false;
	x.addr          = 0;
	x.cmd           = NULL;
	x.cmd_len       = 0;
	x.data          = NULL;
	x.tx_data_len   = 0;
	x.rx_data_len   = 0;
	x.callback      = NULL;
//	x.mode          = ;
	x.state         = &isr_idle;
}

// Trigger the I2C service routine to run at low priority
void I2C_Trigger(void)
{
	if (I2C_srv_handle == INVALID_HANDLE)
	{
		trigger_event(I2C_srv_handle);
	}
}

static void serviceI2C(void)    // service the I2C
{
	if (I2C_EN == 0)            // I2C is off
	{
		x.state = &isr_idle;    // disable response to any interrupts
		I2C_Init();             // turn the I2C back on
		// Put something here to reset state machine.
		// Make sure attached services exit nicely.
	}
}

//void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C1Interrupt(void)
//void ATTRIB_INT _MI2C1Interrupt(void)
#define _INTERRUPT(x) INTERRUPT(x)
_INTERRUPT(MI2C)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;
	I2C_IF = 0;                 // clear the interrupt
	(*x.state)();               // execute the service routine
	interrupt_restore_corcon;
}

// Check if I2C port is available for use.
static inline boolean I2C_CheckAvailable(void)
{
	if (I2C_EN == 0) return false;
	if (!I2C_NORMAL) return false;
	if (x.busy == true) return false;
	x.busy = true;
	return true;
}

boolean I2C_Write(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback)
{
	if (!I2C_CheckAvailable()) return false;

	x.addr          = addr;
	x.cmd           = cmd;
	x.cmd_len       = cmd_len;
	x.data          = data;
	x.tx_data_len   = data_len;
	x.rx_data_len   = 0;
	x.callback      = callback;
	x.mode          = I2C_MODE_WRITE;
	x.state         = &isr_startWrite;
	I2C_IF          = 1;
	return true;
}

boolean I2C_Read(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback, uint16_t mode)
{
	if (!I2C_CheckAvailable()) return false;

	x.addr          = addr;
	x.cmd           = cmd;
	x.cmd_len       = cmd_len;
	x.data          = data;
	x.tx_data_len   = 0;
	x.rx_data_len   = data_len;
	x.callback      = callback;
	x.mode          = mode;
	x.state         = &isr_startWrite;
	I2C_IF          = 1;
	return true;
}

// Only send command byte to check for ACK.
static boolean I2C_CheckACK(uint16_t addr, I2C_callbackFunc callback)
{
	if (!I2C_CheckAvailable()) return false;

	x.addr          = addr;
	x.callback      = callback;
	x.cmd_len       = 0;
	x.data          = NULL;
	x.tx_data_len   = 0;
	x.rx_data_len   = 0;
	x.mode          = I2C_MODE_WRITE;
	x.state         = &isr_startWrite;
	I2C_IF          = 1;
	return true;
}

static void isr_startWrite(void)
{
	x.index = 0;            // Reset index into buffer
	if (x.mode == I2C_MODE_READ_ONLY)
		x.state = &isr_readAddr;
	else
		x.state = &isr_writeAddr;
	I2C_SEN = 1;
}

// Write command byte without checking ACK first.
static void isr_writeAddr(void)
{
	I2C_TRN = x.addr & 0xFE;
	x.state = &isr_writeCmdData;
}

// Write command data (address or similar)
static void isr_writeCmdData(void)
{
	if (I2C_ACKSTAT == 1)   // Device not responding
	{
		isr_failed();
	}
	// If there is no command data, do not send any, do a stop.
	else if (x.cmd_len == 0)
	{
		isr_writeStop();
	}
	else
	{
		I2C_TRN = x.cmd[x.index++];
		if (x.index >= x.cmd_len)
		{
			x.index = 0;    // Reset index into the buffer
			if (x.rx_data_len > 0)
				x.state = &isr_readStart;
			else
				x.state = &isr_writeData;
		}
	}
}

static void isr_writeData(void)
{
	if (I2C_ACKSTAT == 1)   // Device not responding
	{
		isr_failed();
	}
	else
	{
		I2C_TRN = x.data[x.index++];
		if (x.index >= x.tx_data_len)
		{
			if (x.rx_data_len == 0)
				x.state = &isr_writeStop;
			else
				x.state = &isr_readStart;
		}
	}
}

// Stop a write
static void isr_writeStop(void)
{
	x.state = &isr_doneWrite;
	I2C_PEN = 1;
}

static void isr_doneWrite(void)
{
	x.busy = false;
	if (x.callback != NULL)
		x.callback(true);
}

// Start a read after a write by setting the start bit again
static void isr_readStart(void)
{
	x.index = 0;            // Reset index into buffer
	x.state = &isr_readAddr;
	I2C_SEN = 1;
}

// Send the address to read
static void isr_readAddr(void)
{
	I2C_TRN = x.addr | 0x01;
	if (x.mode == I2C_MODE_READ_ONLY)
	{
		x.index = 0;                    // Reset index into the buffer
		if (x.cmd_len == 0)
			x.state = &isr_recen;       // Read the data
		else
			x.state = &isr_writeCmdData;// Write the command data
	}
	else
	{
		x.state = &isr_recen;
	}
}

// Check for ACK.  If ok, start receive mode, otherwise abandon.
static void isr_recen(void)
{
	if (I2C_ACKSTAT == 1)   // Device not responding
	{
		isr_failed();
	}
	else
	{
		x.state = &isr_recstore;
		I2C_RCEN = 1;
	}
}

static void isr_rerecen(void)
{
	x.state = &isr_recstore;
	I2C_RCEN = 1;
}

static void isr_recstore(void)
{
	x.data[x.index++] = I2C_RCV;
	if (x.index >= x.rx_data_len)
	{
		x.state = &isr_stopRead;
		I2C_ACKDT = 1;
	}
	else
	{
		x.state = &isr_rerecen;
		I2C_ACKDT = 0;
	}
	I2C_ACKEN = 1;
}

static void isr_stopRead(void)
{
	I2C_PEN = 1;
	x.state = &isr_doneRead;
}

static void isr_idle(void)
{
}

static void isr_doneRead(void)
{
	x.busy = false;
	if (x.callback != NULL)
		x.callback(true);
}

// On failure, stop the bus, go into idle and callback with failure
static void isr_failed(void)
{
	x.state = &isr_idle;
	I2C_PEN = 1;
	x.busy = false;
	if (x.callback != NULL)
		x.callback(false);
}
