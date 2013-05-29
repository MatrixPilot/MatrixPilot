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

#define USE_I2C_SECOND_PORT_DRIVER 1

#if (USE_I2C_SECOND_PORT_DRIVER == 1)

#define I2C2_SDA 		_RA3
#define I2C2_SCL 		_RA2
#define I2C2_SDA_TRIS 	_TRISA3
#define I2C2_SCL_TRIS 	_TRISA2
#define _I2C2EN 		I2C2CONbits.I2CEN

// Calculate the BRGvalue automatically
//#define I2C1FSCL 400000 // Bus speed measured in Hz
//#define I2C1BRGVAL ((FREQOSC/(CLK_PHASES *I2C1FSCL))-(FREQOSC/(CLK_PHASES * 10000000)))-1
#define I2C2BRGVAL 60 // 200 Khz
#define I2C2_NORMAL ((( I2C2CON & 0b0000000000011111 ) == 0) && ( (I2C2STAT & 0b0100010011000001) == 0 ))

static void I2C2_Init(void);
static void serviceI2C2(void);

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
static void isr_readAddress(void);
static void isr_writeAddress(void);
static void isr_startWrite(void);
static void isr_readStart(void);
static void isr_doneWrite(void);
static void isr_writeCommandData(void);

static int I2C2ERROR = 0;
static boolean I2C2_Busy = true;	// Port busy flag.  Set true until initialized
static uint16_t I2C2_service_handle = INVALID_HANDLE;

struct I2C_xfer {
	uint8_t  addr;
	const uint8_t* cmd;
	uint16_t cmd_len;
	uint8_t* data;
//	uint16_t data_len;
	uint16_t tx_data_len;
	uint16_t rx_data_len;
	I2C_callbackFunc callback;
	uint16_t mode;
//
	uint16_t index;
	void (*state)(void);
};
static struct I2C_xfer x;


// Determine if the bus is normal
boolean I2C2_Normal(void)
{
	if (_I2C2EN == 0)	// I2C is off
	{
		I2C2_Init();
	}
	if (I2C2_NORMAL)
	{
		return true;
	}
	else
	{
		I2C2ERROR = I2C2STAT;
		return false;
	}
}

// Reset the bus
void I2C2_Reset(void)
{
    x.state = &isr_idle;       // disable the response to any more interrupts
    I2C2ERROR = I2C2STAT;         // record the error for diagnostics

    _I2C2EN = 0;                   // turn off the I2C
    _MI2C2IF = 0;                  // clear the I2C master interrupt
    _MI2C2IE = 0;                  // disable the interrupt
//    I2C2_SCL = I2C2_SDA = 0;       // pull SDA and SCL low
	I2C2_SDA = 0;       // pull SDA and SCL low
    I2C2_SCL = 0;
    Nop();
//    I2C2_SCL = I2C2_SDA = 1;       // pull SDA and SCL high
	I2C2_SDA = 1;       // pull SDA and SCL high
    I2C2_SCL = 1;

    I2C2_Init();                   // enable the bus again
}

static void I2C2_Init(void)
{
//	I2C2_SDA_TRIS = I2C2_SCL_TRIS = 0;		// SDA and SCL as outputs
	I2C2BRG = I2C2BRGVAL; 
	_I2C2EN = 1; 	 		// enable I2C2		

	_MI2C2IP = INT_PRI_I2C2;// set interrupt priority
	_MI2C2IF = 0; 			// clear the I2C2 master interrupt
	_MI2C2IE = 1; 			// enable the interrupt

	I2C2_service_handle = register_event(&serviceI2C2);

	I2C2_Busy = false;

	x.addr		= 0;
	x.cmd		= NULL;
	x.cmd_len	= 0;
	x.data		= NULL;
	x.tx_data_len = 0;
	x.rx_data_len = 0;
	x.callback	= NULL;
//	x.mode		= ;
	x.state		= &isr_idle;
}

// Trigger the I2C2 service routine to run at low priority
void I2C2_trigger_service(void)
{
	trigger_event(I2C2_service_handle);
}

static void serviceI2C2(void)  // service the I2C
{
	if (_I2C2EN == 0) // I2C is off
	{
		x.state = &isr_idle; 	// disable response to any interrupts
		I2C2_Init(); 				// turn the I2C back on
		// Put something here to reset state machine.  Make sure attached servies exit nicely.
	}
}

void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C2Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;
	
	_MI2C2IF = 0; // clear the interrupt
	(*x.state)(); // execute the service routine
	
	interrupt_restore_corcon;
}

// Check if I2C port is available for use.
static inline boolean I2C2_CheckAvailable(void)
{
	if (_I2C2EN == 0) return false;
	if (!I2C2_NORMAL) return false;
	if (I2C2_Busy == true) return false;
	I2C2_Busy = true;
	return true;
}

boolean I2C2_Write(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback)
{
	if (!I2C2_CheckAvailable()) return false;

	x.addr		= addr;
	x.cmd		= cmd;
	x.cmd_len	= cmd_len;
	x.data		= data;
	x.tx_data_len = data_len;
	x.rx_data_len = 0;
	x.callback	= callback;

	x.mode		= I2C_MODE_WRITE;
	x.state		= &isr_startWrite;
	_MI2C2IF	= 1;
	return true;
}

boolean I2C2_Read(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback, uint16_t mode)
{
	if (!I2C2_CheckAvailable()) return false;

	x.addr		= addr;
	x.cmd		= cmd;
	x.cmd_len	= cmd_len;
	x.data		= data;
	x.tx_data_len = 0;
	x.rx_data_len = data_len;
	x.callback	= callback;
	x.mode		= mode;

	x.state		= &isr_startWrite;
	_MI2C2IF	= 1;
	return true;
}

// Only send command byte to check for ACK.
boolean I2C2_checkACK(uint16_t addr, I2C_callbackFunc callback)
{
	if (!I2C2_CheckAvailable()) return false;

	x.addr		= addr;
	x.callback	= callback;


	x.cmd_len	= 0;
	x.data		= NULL;
	x.tx_data_len = 0;
	x.rx_data_len = 0;

	x.mode		= I2C_MODE_WRITE;
	x.state		= &isr_startWrite;
	_MI2C2IF	= 1;
	return true;
}

static void isr_startWrite(void)
{
	x.index = 0;  			// Reset index into buffer

	if (x.mode == I2C_MODE_READ_ONLY)
		x.state = &isr_readAddress;	
	else
		x.state = &isr_writeAddress;
	I2C2CONbits.SEN = 1;
}

// Write command byte without checking ACK first.
static void isr_writeAddress(void)
{
	I2C2TRN = x.addr & 0xFE;
	x.state = &isr_writeCommandData;
}

// Write command data (address or similar)
static void isr_writeCommandData(void)
{
	if (I2C2STATbits.ACKSTAT == 1)  	// Device not responding
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
		I2C2TRN = x.cmd[x.index++];
		if (x.index >= x.cmd_len)
		{
			x.index = 0; 			// Reset index into the buffer
			if (x.rx_data_len > 0)
				x.state = &isr_readStart;			
			else
				x.state = &isr_writeData;
		}
	}
}

static void isr_writeData(void)
{
	if (I2C2STATbits.ACKSTAT == 1)  	// Device not responding
	{
		isr_failed();
	}
	else
	{
		I2C2TRN = x.data[x.index++];
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
	I2C2CONbits.PEN = 1;
}

static void isr_doneWrite(void)
{
	I2C2_Busy = false;
	if (x.callback != NULL)
		x.callback(true);
}

// Start a read after a write by setting the start bit again
static void isr_readStart(void)
{
	x.index = 0;  			// Reset index into buffer
	x.state = &isr_readAddress;
	I2C2CONbits.SEN = 1;	
}

// Send the address to read
static void isr_readAddress(void)
{
	I2C2TRN = x.addr | 0x01;

	if (x.mode == I2C_MODE_READ_ONLY)
	{
		x.index = 0; 							// Reset index into the buffer

		if (x.cmd_len == 0)
			x.state = &isr_recen;				// Read the data
		else
			x.state = &isr_writeCommandData;	// Write the command data
	}
	else
	{
		x.state = &isr_recen;
	}
}

// Check for ACK.  If ok, start receive mode, otherwise abandon.
static void isr_recen(void)
{
	if (I2C2STATbits.ACKSTAT == 1)  	// Device not responding
	{
		isr_failed();
	}
	else
	{
		x.state = &isr_recstore;
		I2C2CONbits.RCEN = 1;
	}
}

static void isr_rerecen(void)
{
	x.state = &isr_recstore;
	I2C2CONbits.RCEN = 1;
}

static void isr_recstore(void)
{
	x.data[x.index++] = I2C2RCV;
	if (x.index >= x.rx_data_len)
	{
		x.state = &isr_stopRead;
		I2C2CONbits.ACKDT = 1;
	}
	else
	{
		x.state = &isr_rerecen;
		I2C2CONbits.ACKDT = 0;
	}
	I2C2CONbits.ACKEN = 1;
}

static void isr_stopRead(void)
{
	I2C2CONbits.PEN = 1;
	x.state = &isr_doneRead;
}

static void isr_idle(void)
{
}

static void isr_doneRead(void)
{
	I2C2_Busy = false;
	if (x.callback != NULL)
		x.callback(true);
}

// On failure, stop the bus, go into idle and callback with failure
static void isr_failed(void)
{
	x.state = &isr_idle;
	I2C2CONbits.PEN = 1;
	I2C2_Busy = false;
	if (x.callback != NULL)
		x.callback(false);
}

#endif	// USE_I2C_SECOND_PORT_DRIVER
