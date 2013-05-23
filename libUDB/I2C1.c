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

#if (USE_I2C1_DRIVER == 1)

#define I2C1_SDA 		_RG3
#define I2C1_SCL 		_RG2

#define I2C1_SDA_TRIS 	_TRISG3
#define I2C1_SCL_TRIS 	_TRISG2

#define _I2C1EN 		I2C1CONbits.I2CEN

I2C_callbackFunc pI2C_callback = NULL;

void I2C1_start(void) ;
void I2C1_idle(void) ;
void I2C1_doneRead(void);
void I2C1_recstore(void);
void I2C1_rerecen(void);
void I2C1_recen(void);
void I2C1_writeStop(void);
void I2C1_stopRead(void);
void I2C1_writeData(void);
void I2C1_readAddress(void);
void I2C1_writeAddress(void);
void I2C1_startWrite(void);
void I2C1_readStart(void);
void I2C1_Failed(void);
void I2C1_doneWrite(void);
void I2C1_writeCommandData(void);

void serviceI2C1(void);  	// service the I2C

// The mode for read or write.
uint16_t I2C1_mode;

int16_t I2C1ERROR = 0 ;

// Port busy flag.  Set true until initialized
boolean I2C1_Busy = true;

void (* I2C1_state ) ( void ) = &I2C1_idle ;

#define I2C1BRGVAL 60 // 200 Khz

#define I2C1_NORMAL ((( I2C1CON & 0b0000000000011111 ) == 0) && ( (I2C1STAT & 0b0100010011000001) == 0 ))


uint16_t I2C1_Index = 0;  		// index into the write buffer

uint8_t I2C1_AddressByte = 0;
uint16_t I2C1_tx_data_size = 0;		// tx data size
uint16_t I2C1_rx_data_size = 0;		// rx data size
uint16_t I2C1_command_data_size = 0;	// command data size

uint8_t* pI2C1Buffer = NULL;	// pointer to buffer
uint8_t* pI2C1commandBuffer = NULL;	// pointer to receive  buffer

uint16_t I2C1_service_handle = INVALID_HANDLE;


// Determine if the bus is normal
boolean I2C1_Normal(void)
{
	if ( _I2C1EN == 0 ) { // I2C is off
		I2C1_init();
	}
	if (I2C1_NORMAL) {
		return true;
	} else {
		I2C1ERROR = I2C1STAT;
		return false;
	}
}

// Reset the bus
void I2C1_reset(void)
{
    return ;
}

void I2C1_init(void)
{
//	I2C1_SDA_TRIS = I2C1_SCL_TRIS = 0 ;		// SDA and SCL as outputs
	I2C1BRG = I2C1BRGVAL ; 
	_I2C1EN = 1 ; 	 		// enable I2C1		

	_MI2C1IP = 5 ; 			// I2C1 at priority 5
	_MI2C1IF = 0 ; 			// clear the I2C1 master interrupt
	_MI2C1IE = 1 ; 			// enable the interrupt

	I2C1_service_handle = register_event(&serviceI2C1);

	I2C1_Busy = false;

	return ;
}

// Trigger the I2C1 service routine to run at low priority
void I2C1_trigger_service(void)
{
	trigger_event(I2C1_service_handle);
}


void serviceI2C1(void)  // service the I2C
{
	if ( _I2C1EN == 0 ) // I2C is off
	{
		I2C1_state = &I2C1_idle ; 	// disable response to any interrupts
		I2C1_init() ; 			// turn the I2C back on
		// Put something here to reset state machine.  Make sure attached servies exit nicely.
		return ;
	}

	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C1Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_MI2C1IF = 0 ; // clear the interrupt
	(* I2C1_state) () ; // execute the service routine
	
	interrupt_restore_corcon ;
	return ;
}

// Check if I2C port is available for use.
inline boolean I2C1_CheckAvailable(void)
{
	if( _I2C1EN == 0 ) return false;
	if (  !I2C1_NORMAL ) return false;

	if(I2C1_Busy == true) return false;
	I2C1_Busy = true;

	return true;
}


boolean I2C1_Write(uint8_t address, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* ptxData, uint16_t txSize, I2C_callbackFunc pCallback)
{
	if(!I2C1_CheckAvailable()) return false;

	pI2C_callback 	= pCallback;
	I2C1_mode		= I2C_MODE_WRITE;

	I2C1_command_data_size 	= commandDataSize;
	pI2C1commandBuffer		= pcommandData;
	I2C1_AddressByte 		= address;
	pI2C1Buffer 			= ptxData;

	I2C1_tx_data_size = txSize;		// tx data size
	I2C1_rx_data_size = 0;			// rx data size

	// Set ISR callback and trigger the ISR
	I2C1_state = &I2C1_startWrite;
	_MI2C1IF = 1 ;
	return true;
}


boolean I2C1_Read(uint8_t address, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* prxData, uint16_t rxSize, I2C_callbackFunc pCallback, uint16_t I2C_mode)
{
	if(!I2C1_CheckAvailable()) return false;

	pI2C_callback = pCallback;
	I2C1_mode = I2C_mode;

	I2C1_command_data_size 	= commandDataSize;
	pI2C1commandBuffer		= pcommandData;
	I2C1_AddressByte 		= address;
	pI2C1Buffer 			= prxData;

	I2C1_tx_data_size = 0;			// tx data size
	I2C1_rx_data_size = rxSize;		// rx data size

	// Set ISR callback and trigger the ISR
	I2C1_state = &I2C1_startWrite;
	_MI2C1IF = 1 ;
	return true;
}


// Only send command byte to check for ACK.
boolean I2C1_checkACK(uint16_t address, I2C_callbackFunc pCallback)
{
	if(!I2C1_CheckAvailable()) return false;

	pI2C_callback 	= pCallback;
	I2C1_mode		= I2C_MODE_WRITE;

	I2C1_command_data_size 	= 0;
	I2C1_AddressByte 		= address;
	pI2C1Buffer 			= NULL;

	I2C1_tx_data_size = 0;	// tx data size
	I2C1_rx_data_size = 0;	// rx data size

	// Set ISR callback and trigger the ISR
	I2C1_state = &I2C1_startWrite;
	_MI2C1IF = 1 ;
	return true;
}


void I2C1_startWrite(void)
{
	I2C1_Index = 0;  			// Reset index into buffer

	if(I2C1_mode == I2C_MODE_READ_ONLY)
		I2C1_state = &I2C1_readAddress ;	
	else
		I2C1_state = &I2C1_writeAddress ;
	I2C1CONbits.SEN = 1 ;
	return ;
}

// Write command byte without checking ACK first.
void I2C1_writeAddress(void)
{
	I2C1TRN = I2C1_AddressByte & 0xFE ;
	I2C1_state = &I2C1_writeCommandData ;
	return;
}


// Write command data (address or similar)
void I2C1_writeCommandData(void)
{
	if ( I2C1STATbits.ACKSTAT == 1 )  	// Device not responding
	{
		I2C1_Failed(); 
		return ;
	}

	// If there is no command data, do not send any, do a stop.
	if(I2C1_command_data_size == 0)
	{
		I2C1_writeStop() ;
		return;
	}

	I2C1TRN = pI2C1commandBuffer[I2C1_Index++] ;

	if ( I2C1_Index >= I2C1_command_data_size)
	{
		I2C1_Index = 0; 			// Reset index into the buffer

		if(I2C1_rx_data_size > 0)
			I2C1_state = &I2C1_readStart ;			
		else
			I2C1_state = &I2C1_writeData ;
	}
	return ;
}


void I2C1_writeData(void)
{
	if ( I2C1STATbits.ACKSTAT == 1 )  	// Device not responding
	{
		I2C1_Failed();
		return ;
	}

	I2C1TRN = pI2C1Buffer[I2C1_Index++] ;

	if ( I2C1_Index >= I2C1_tx_data_size)
	{
		if(I2C1_rx_data_size == 0)
			I2C1_state = &I2C1_writeStop ;
		else
			I2C1_state = &I2C1_readStart ;			
	}
	return ;
}

// Stop a write
void I2C1_writeStop(void)
{
	I2C1_state = &I2C1_doneWrite ;
	I2C1CONbits.PEN = 1 ;
	return ;
}

void I2C1_doneWrite(void)
{
	I2C1_Busy = false;
	if(	pI2C_callback != NULL)
		pI2C_callback(true);
	return;
}

// Start a read after a write by settign the start bit again
void I2C1_readStart(void)
{
	I2C1_Index = 0;  			// Reset index into buffer
	I2C1_state = &I2C1_readAddress ;
	I2C1CONbits.SEN = 1 ;	
}

// Send the address to read
void I2C1_readAddress(void)
{
	I2C1TRN =  I2C1_AddressByte | 0x01;

	if(I2C1_mode == I2C_MODE_READ_ONLY)
	{
		I2C1_Index = 0; 							// Reset index into the buffer

		if(I2C1_command_data_size == 0)
			I2C1_state = &I2C1_recen ;				// Read the data
		else
			I2C1_state = &I2C1_writeCommandData ;	// Write the command data
	}
	else
		I2C1_state = &I2C1_recen ;
}

// Check for ACK.  If ok, start receive mode, otherwise abandon.
void I2C1_recen(void)
{
	if ( I2C1STATbits.ACKSTAT == 1 )  	// Device not responding
	{
		I2C1_Failed();
		return;
	}
	else
	{
		I2C1_state = &I2C1_recstore ;
		I2C1CONbits.RCEN = 1 ;
	}
	return ;
}

void I2C1_rerecen(void)
{
	I2C1_state = &I2C1_recstore ;
	I2C1CONbits.RCEN = 1 ;
	return ;
}

void I2C1_recstore(void)
{
	pI2C1Buffer[I2C1_Index++] = I2C1RCV ;
	if ( I2C1_Index >= I2C1_rx_data_size )
	{
		I2C1_state = &I2C1_stopRead ;
		I2C1CONbits.ACKDT = 1 ;
	}
	else
	{
		I2C1_state = &I2C1_rerecen ;
		I2C1CONbits.ACKDT = 0 ;
	}
	I2C1CONbits.ACKEN = 1 ;
	return ;
}


void I2C1_stopRead(void)
{
	I2C1CONbits.PEN = 1;
	I2C1_state = &I2C1_doneRead ;
	return ;
}


void I2C1_idle(void)
{
	return ;
}

void I2C1_doneRead(void)
{
	I2C1_Busy = false;
	if(	pI2C_callback != NULL)
		pI2C_callback(true);
}

// On failure, stop the bus, go into idle and callback with failure
void I2C1_Failed(void)
{
	I2C1_state = &I2C1_idle ;
	I2C1CONbits.PEN = 1;
	I2C1_Busy = false;
	if(	pI2C_callback != NULL)
		pI2C_callback(false);
}

#endif  // USE_I2C1_DRIVER
