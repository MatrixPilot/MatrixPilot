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
#include "oscillator.h"
#include "interrupt.h"
#include "I2C.h"
//#include "NV_memory.h"
#include "events.h"

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == AUAV3_BOARD)

#define USE_I2C_SECOND_PORT_DRIVER 1

#if(USE_I2C_SECOND_PORT_DRIVER == 1)

#define I2C2_SDA 		_RA3
#define I2C2_SCL 		_RA2

#define I2C2_SDA_TRIS 	_TRISA3
#define I2C2_SCL_TRIS 	_TRISA2

#define _I2C2EN 		I2C2CONbits.I2CEN

I2C_callbackFunc pI2C2_callback = NULL;

void I2C2_start(void) ;
void I2C2_idle(void) ;
void I2C2_doneRead(void);
void I2C2_recstore(void);
void I2C2_rerecen(void);
void I2C2_recen(void);
void I2C2_writeStop(void);
void I2C2_stopRead(void);
void I2C2_writeData(void);
void I2C2_readAddress(void);
void I2C2_writeAddress(void);
void I2C2_startWrite(void);
void I2C2_readStart(void);
void I2C2_Failed(void);
void I2C2_doneWrite(void);
void I2C2_writeCommandData(void);

void serviceI2C2(void);  	// service the I2C

// The mode for read or write.
unsigned int I2C2_mode;

int I2C2ERROR = 0 ;

// Port busy flag.  Set true until initialized
boolean I2C2_Busy = true;

void (* I2C2_state ) ( void ) = &I2C2_idle ;

// Calculate the BRGvalue automatically
//#define I2C1FSCL 400000 // Bus speed measured in Hz
//#define I2C1BRGVAL ((FREQOSC/(CLK_PHASES *I2C1FSCL))-(FREQOSC/(CLK_PHASES * 10000000)))-1

#define I2C2BRGVAL 60 // 200 Khz

#define I2C2_NORMAL ((( I2C2CON & 0b0000000000011111 ) == 0) && ( (I2C2STAT & 0b0100010011000001) == 0 ))


unsigned int I2C2_Index = 0;  		// index into the write buffer

unsigned char I2C2_AddressByte 	= 0;
unsigned int I2C2_tx_data_size = 0;		// tx data size
unsigned int I2C2_rx_data_size = 0;		// rx data size
unsigned int I2C2_command_data_size = 0;	// command data size

unsigned char* pI2C2Buffer = NULL;	// pointer to buffer
const unsigned char* pI2C2commandBuffer = NULL;	// pointer to receive buffer

unsigned int I2C2_service_handle = INVALID_HANDLE;


// Determine if the bus is normal
boolean I2C2_Normal(void)
{
	if ( _I2C2EN == 0 ) { // I2C is off
		I2C2_init();
	}
	if (I2C2_NORMAL) {
		return true;
	} else {
		I2C2ERROR = I2C2STAT;
		return false;
	}
}

// Reset the bus
void I2C2_reset(void)
{
    I2C2_state = &I2C2_idle ;       // disable the response to any more interrupts
    I2C2ERROR = I2C2STAT ;         // record the error for diagnostics

    _I2C2EN = 0 ;                   // turn off the I2C
    _MI2C2IF = 0 ;                  // clear the I2C master interrupt
    _MI2C2IE = 0 ;                  // disable the interrupt
//    I2C2_SCL = I2C2_SDA = 0 ;       // pull SDA and SCL low
	I2C2_SDA = 0 ;       // pull SDA and SCL low
    I2C2_SCL = 0;
    Nop();
//    I2C2_SCL = I2C2_SDA = 1 ;       // pull SDA and SCL high
	I2C2_SDA = 1 ;       // pull SDA and SCL high
    I2C2_SCL = 1;

    I2C2_init() ;                   // enable the bus again
    return ;
}


void I2C2_init(void)
{
//	I2C2_SDA_TRIS = I2C2_SCL_TRIS = 0 ;		// SDA and SCL as outputs
	I2C2BRG = I2C2BRGVAL ; 
	_I2C2EN = 1 ; 	 		// enable I2C2		

	_MI2C2IP = 5 ; 			// I2C2 at priority 5
	_MI2C2IF = 0 ; 			// clear the I2C2 master interrupt
	_MI2C2IE = 1 ; 			// enable the interrupt

	I2C2_service_handle = register_event(&serviceI2C2);

	I2C2_Busy = false;

	return ;
}

// Trigger the I2C2 service routine to run at low priority
void I2C2_trigger_service(void)
{
	trigger_event(I2C2_service_handle);
}


void serviceI2C2(void)  // service the I2C
{
	if ( _I2C2EN == 0 ) // I2C is off
	{
		I2C2_state = &I2C2_idle ; 	// disable response to any interrupts
		I2C2_init() ; 				// turn the I2C back on
		// Put something here to reset state machine.  Make sure attached servies exit nicely.
		return ;
	}

	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C2Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon(MI2C2_INT, 0) ;
	
	_MI2C2IF = 0 ; // clear the interrupt
	(* I2C2_state) () ; // execute the service routine
	
	interrupt_restore_corcon(MI2C2_INT, 0) ;
	return ;
}

// Check if I2C port is available for use.
inline boolean I2C2_CheckAvailable(void)
{
	if( _I2C2EN == 0 ) return false;
	if (  !I2C2_NORMAL ) return false;

	if(I2C2_Busy == true) return false;
	I2C2_Busy = true;

	return true;
}


boolean I2C2_Write(unsigned char address, const unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* ptxData, unsigned int txSize, I2C_callbackFunc pCallback)
{
	if(!I2C2_CheckAvailable()) return false;

	pI2C2_callback 	= pCallback;
	I2C2_mode		= I2C_MODE_WRITE;

	I2C2_command_data_size 	= commandDataSize;
	pI2C2commandBuffer		= pcommandData;
	I2C2_AddressByte 		= address;
	pI2C2Buffer 			= ptxData;

	I2C2_tx_data_size = txSize;		// tx data size
	I2C2_rx_data_size = 0;			// rx data size

	// Set ISR callback and trigger the ISR
	I2C2_state = &I2C2_startWrite;
	_MI2C2IF = 1 ;
	return true;
}


boolean I2C2_Read(unsigned char address, const unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* prxData, unsigned int rxSize, I2C_callbackFunc pCallback, unsigned int I2C_mode)
{
	if(!I2C2_CheckAvailable()) return false;

	pI2C2_callback = pCallback;
	I2C2_mode		= I2C_mode;

	I2C2_command_data_size 	= commandDataSize;
	pI2C2commandBuffer		= pcommandData;
	I2C2_AddressByte 		= address;
	pI2C2Buffer 			= prxData;

	I2C2_tx_data_size = 0;			// tx data size
	I2C2_rx_data_size = rxSize;		// rx data size

	// Set ISR callback and trigger the ISR
	I2C2_state = &I2C2_startWrite;
	_MI2C2IF = 1 ;
	return true;
}


// Only send command byte to check for ACK.
boolean I2C2_checkACK(unsigned int address, I2C_callbackFunc pCallback)
{
	if(!I2C2_CheckAvailable()) return false;

	pI2C2_callback 	= pCallback;
	I2C2_mode		= I2C_MODE_WRITE;

	I2C2_command_data_size 	= 0;
	I2C2_AddressByte 		= address;
	pI2C2Buffer 			= NULL;

	I2C2_tx_data_size = 0;	// tx data size
	I2C2_rx_data_size = 0;	// rx data size

	// Set ISR callback and trigger the ISR
	I2C2_state = &I2C2_startWrite;
	_MI2C2IF = 1 ;
	return true;
}


void I2C2_startWrite(void)
{
	I2C2_Index = 0;  			// Reset index into buffer

	if(I2C2_mode == I2C_MODE_READ_ONLY)
		I2C2_state = &I2C2_readAddress ;	
	else
		I2C2_state = &I2C2_writeAddress ;
	I2C2CONbits.SEN = 1 ;
	return ;
}

// Write command byte without checking ACK first.
void I2C2_writeAddress(void)
{
	I2C2TRN = I2C2_AddressByte & 0xFE ;
	I2C2_state = &I2C2_writeCommandData ;
	return;
}


// Write command data (address or similar)
void I2C2_writeCommandData(void)
{
	if ( I2C2STATbits.ACKSTAT == 1 )  	// Device not responding
	{
		I2C2_Failed(); 
		return ;
	}

	// If there is no command data, do not send any, do a stop.
	if(I2C2_command_data_size == 0)
	{
		I2C2_writeStop() ;
		return;
	}

	I2C2TRN = pI2C2commandBuffer[I2C2_Index++] ;

	if ( I2C2_Index >= I2C2_command_data_size)
	{
		I2C2_Index = 0; 			// Reset index into the buffer

		if(I2C2_rx_data_size > 0)
			I2C2_state = &I2C2_readStart ;			
		else
			I2C2_state = &I2C2_writeData ;
	}
	return ;
}


void I2C2_writeData(void)
{
	if ( I2C2STATbits.ACKSTAT == 1 )  	// Device not responding
	{
		I2C2_Failed();
		return ;
	}

	I2C2TRN = pI2C2Buffer[I2C2_Index++] ;

	if ( I2C2_Index >= I2C2_tx_data_size)
	{
		if(I2C2_rx_data_size == 0)
			I2C2_state = &I2C2_writeStop ;
		else
			I2C2_state = &I2C2_readStart ;			
	}
	return ;
}

// Stop a write
void I2C2_writeStop(void)
{
	I2C2_state = &I2C2_doneWrite ;
	I2C2CONbits.PEN = 1 ;
	return ;
}

void I2C2_doneWrite(void)
{
	I2C2_Busy = false;
	if(	pI2C2_callback != NULL)
		pI2C2_callback(true);
	return;
}

// Start a read after a write by settign the start bit again
void I2C2_readStart(void)
{
	I2C2_Index = 0;  			// Reset index into buffer
	I2C2_state = &I2C2_readAddress ;
	I2C2CONbits.SEN = 1 ;	
}

// Send the address to read
void I2C2_readAddress(void)
{
	I2C2TRN =  I2C2_AddressByte | 0x01;

	if(I2C2_mode == I2C_MODE_READ_ONLY)
	{
		I2C2_Index = 0; 							// Reset index into the buffer

		if(I2C2_command_data_size == 0)
			I2C2_state = &I2C2_recen ;				// Read the data
		else
			I2C2_state = &I2C2_writeCommandData ;	// Write the command data
	}
	else
		I2C2_state = &I2C2_recen ;
}

// Check for ACK.  If ok, start receive mode, otherwise abandon.
void I2C2_recen(void)
{
	if ( I2C2STATbits.ACKSTAT == 1 )  	// Device not responding
	{
		I2C2_Failed();
		return;
	}
	else
	{
		I2C2_state = &I2C2_recstore ;
		I2C2CONbits.RCEN = 1 ;
	}
	return ;
}

void I2C2_rerecen(void)
{
	I2C2_state = &I2C2_recstore ;
	I2C2CONbits.RCEN = 1 ;
	return ;
}

void I2C2_recstore(void)
{
	pI2C2Buffer[I2C2_Index++] = I2C2RCV ;
	if ( I2C2_Index >= I2C2_rx_data_size )
	{
		I2C2_state = &I2C2_stopRead ;
		I2C2CONbits.ACKDT = 1 ;
	}
	else
	{
		I2C2_state = &I2C2_rerecen ;
		I2C2CONbits.ACKDT = 0 ;
	}
	I2C2CONbits.ACKEN = 1 ;
	return ;
}


void I2C2_stopRead(void)
{
	I2C2CONbits.PEN = 1;
	I2C2_state = &I2C2_doneRead ;
	return ;
}


void I2C2_idle(void)
{
	return ;
}

void I2C2_doneRead(void)
{
	I2C2_Busy = false;
	if(	pI2C2_callback != NULL)
		pI2C2_callback(true);
}

// On failure, stop the bus, go into idle and callback with failure
void I2C2_Failed(void)
{
	I2C2_state = &I2C2_idle ;
	I2C2CONbits.PEN = 1;
	I2C2_Busy = false;
	if(	pI2C2_callback != NULL)
		pI2C2_callback(false);
}

#endif	// USE_I2C_SECOND_PORT_DRIVER == 1

#endif  // UDB4 BOARD || AUAV3_BOARD
