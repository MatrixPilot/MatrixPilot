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
#include "I2C.h"
#include "NV_memory.h"
#include "events.h"

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == MADRE_BOARD)

#define I2C1_SDA 		_RB9
#define I2C1_SCL 		_RB8

#define I2C1_SDA_TRIS 	_TRISB9
#define I2C1_SCL_TRIS 	_TRISB8

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
void I2C1_readCommand(void);
void I2C1_writeCommand(void);
void I2C1_startWrite(void);
void I2C1_readStart(void);
void I2C1_Failed(void);
void I2C1_doneWrite(void);
void I2C1_writeCommandData(void);

void serviceI2C1(void);  // service the I2C

int I2C1ERROR = 0 ;

// Port busy flag.  Set true until initialized
boolean I2C1_Busy = true;

void (* I2C1_state ) ( void ) = &I2C1_idle ;

#define I2C1FSCL 400000						// Bus speed measured in Hz
#define I2C1BRGVAL ((FREQOSC/(CLK_PHASES *I2C1FSCL))-(FREQOSC/(CLK_PHASES * 10000000)))-1

//#define I2C1_NORMAL ((( I2C1CON & 0b0000000000011111 ) == 0) && ( (I2C1STAT & 0b0100010011000001) == 0 ))
#define I2C1_NORMAL ( (I2C1STAT & 0b0000010011000000) == 0 )	// There is the queue, it's ok if the module is reading

#define I2C1_QUEUE_DEPTH	3

I2Cqueue		i2c1_queue[I2C1_QUEUE_DEPTH];

unsigned int	I2C1_Index = 0;  				// index into the write buffer

unsigned char	I2C1_CommandByte 	= 0;
unsigned int	I2C1_tx_data_size = 0;			// tx data size
unsigned int	I2C1_rx_data_size = 0;			// rx data size
unsigned int	I2C1_command_data_size = 0;		// command data size

unsigned char*	pI2C1Buffer = NULL;				// pointer to buffer
unsigned char*	pI2C1commandBuffer = NULL;		// pointer to receive  buffer

unsigned int	I2C1_service_handle = INVALID_HANDLE;

unsigned int	I2C1_ERROR = 0;


void I2C1_init(void)
{
	int queueIndex;

	for(queueIndex = 0; queueIndex < I2C1_QUEUE_DEPTH; queueIndex++)
	{
		i2c1_queue[queueIndex].pending = false;
		i2c1_queue[queueIndex].rW = 0;
		i2c1_queue[queueIndex].command = 0;
		i2c1_queue[queueIndex].pcommandData = NULL;
		i2c1_queue[queueIndex].commandDataSize = 0;
		i2c1_queue[queueIndex].pData = NULL;
		i2c1_queue[queueIndex].Size = 0;
		i2c1_queue[queueIndex].pCallback = NULL;	

	}	

	I2C1BRG = I2C1BRGVAL ; 
	_I2C1EN = 1 ; 	 		// enable I2C1		

	_MI2C1IP = 5 ; 			// I2C1 at priority 5
	_MI2C1IF = 0 ; 			// clear the I2C1 master interrupt
	_MI2C1IE = 1 ; 			// enable the interrupt

//	I2C1_service_handle = register_event(&serviceI2C1);
	
	I2C1_Busy = false;

	return ;
}

void I2C1_reset(void)
{
	I2C1_state = &I2C1_idle ;		// disable the response to any more interrupts
	I2C1_ERROR = I2C1STAT ; 		// record the error for diagnostics
	
	_I2C1EN = 0 ;  					// turn off the I2C
	_MI2C1IF = 0 ; 					// clear the I2C master interrupt
	_MI2C1IE = 0 ; 					// disable the interrupt
	I2C1_SCL = I2C1_SDA = 0 ; 		// pull SDA and SCL low
	Nop();
	I2C1_SCL = I2C1_SDA = 1 ; 		// pull SDA and SCL high

	I2C1CON = 0x1000;
	I2C1STAT = 0x0000;
	I2C1_init() ;
	return ;
}

// Trigger the I2C1 service routine to run at low priority
void I2C1_trigger_service(void)
{
	trigger_event(I2C1_service_handle);
};


void serviceI2C1(void)
{

	if ( _I2C1EN == 0 ) 			// I2C is off
	{
		I2C1_state = &I2C1_idle ; 	// disable response to any interrupts
//		I2C1_SDA = I2C1_SCL = 1 ; 	// pull SDA and SCL high
		I2C1_init() ; 				// turn the I2C back on
		// Put something here to reset state machine.  Make sure attached servies exit nicely.
		return ;
	}

	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C1Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;

	_MI2C1IF = 0 ; 					// clear the interrupt
	(* I2C1_state) () ; 			// execute the service routine

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

boolean I2C1_Normal(void)
{
/*	if ( _I2C1EN == 0 ) // I2C is off
	{ 
		I2C1_init();
	}
*/	if(I2C1_NORMAL )
		return true;
	else
	{
		I2C1_ERROR = I2C1STAT;
		return false;
	}	
}

boolean I2C1_serve_queue()
{
	int queueIndex;
	
	for(queueIndex = 0; queueIndex < I2C1_QUEUE_DEPTH; queueIndex++)
	{
		if(i2c1_queue[queueIndex].pending == true)
		{
			if(!I2C1_CheckAvailable())
			{
				return false;
			}	
			else i2c1_queue[queueIndex].pending = false;

			pI2C_callback			= i2c1_queue[queueIndex].pCallback;
			I2C1_command_data_size 	= i2c1_queue[queueIndex].commandDataSize;
			pI2C1commandBuffer		= i2c1_queue[queueIndex].pcommandData;
			I2C1_CommandByte 		= i2c1_queue[queueIndex].command;
			pI2C1Buffer 			= i2c1_queue[queueIndex].pData;
			
			if(i2c1_queue[queueIndex].rW == 0)
			{
				I2C1_tx_data_size = i2c1_queue[queueIndex].Size;		// tx data size
				I2C1_rx_data_size = 0;					// rx data size
			}
			else
			{
				I2C1_tx_data_size = 0;					// tx data size
				I2C1_rx_data_size = i2c1_queue[queueIndex].Size;		// rx data size
			}		
			
			// Set ISR callback and trigger the ISR
			I2C1_state = &I2C1_startWrite;
			LED_RED = LED_OFF;			
			_MI2C1IF = 1 ;
			return true;	
			
		}
	}
	return false;
}	
boolean I2C1_Write(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* ptxData, unsigned int txSize, I2C_callbackFunc pCallback)
{
	int queueIndex;

	for(queueIndex = 0; queueIndex < I2C1_QUEUE_DEPTH; queueIndex++)
	{
		if(i2c1_queue[queueIndex].pending == false)
		{
			i2c1_queue[queueIndex].pending = true;
			i2c1_queue[queueIndex].rW = 0;
			i2c1_queue[queueIndex].command = command;
			i2c1_queue[queueIndex].pcommandData = pcommandData;
			i2c1_queue[queueIndex].commandDataSize = commandDataSize;
			i2c1_queue[queueIndex].pData = ptxData;
			i2c1_queue[queueIndex].Size = txSize;
			i2c1_queue[queueIndex].pCallback = pCallback;
			return I2C1_serve_queue();
		}
	}

/*	while(1)	// STOP HERE ON FAILURE.
	{
		LED_GREEN = LED_ON;	
	}			
*/
	I2C1_reset();
	return false;
}


boolean I2C1_Read(unsigned char command, unsigned char* pcommandData, unsigned char commandDataSize, unsigned char* prxData, unsigned int rxSize, I2C_callbackFunc pCallback)
{
	int queueIndex;

	for(queueIndex = 0; queueIndex < I2C1_QUEUE_DEPTH; queueIndex++)
	{
		if(i2c1_queue[queueIndex].pending == false)
		{
			i2c1_queue[queueIndex].pending = true;
			i2c1_queue[queueIndex].rW = 1;
			i2c1_queue[queueIndex].command = command;
			i2c1_queue[queueIndex].pcommandData = pcommandData;
			i2c1_queue[queueIndex].commandDataSize = commandDataSize;
			i2c1_queue[queueIndex].pData = prxData;
			i2c1_queue[queueIndex].Size = rxSize;
			i2c1_queue[queueIndex].pCallback = pCallback;
			return I2C1_serve_queue();
		}
	}

/*	while(1)		// STOP HERE ON FAILURE.
	{
		LED_GREEN = LED_ON;	
	}			
*/
	I2C1_reset();
	return false;
	
}


// Only send command byte to check for ACK.
boolean I2C1_checkACK(unsigned int command, I2C_callbackFunc pCallback)
{
	if(!I2C1_CheckAvailable()) return false;

	pI2C_callback = pCallback;

	I2C1_command_data_size 	= 0;
	I2C1_CommandByte 		= command;
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

	I2C1_state = &I2C1_writeCommand ;
	I2C1CONbits.SEN = 1 ;
	return ;
}

// Write command byte without checking ACK first.
void I2C1_writeCommand(void)
{
	I2C1_state = &I2C1_writeCommandData ;
	I2C1TRN = I2C1_CommandByte & 0xFE ;
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
	I2C1_serve_queue();	
//	return;
}

// Start a read after a write by settign the start bit again
void I2C1_readStart(void)
{
	I2C1_Index = 0;  			// Reset index into buffer
	I2C1_state = &I2C1_readCommand ;
	I2C1CONbits.SEN = 1 ;	
}

// Send the command to read
void I2C1_readCommand(void)
{
	I2C1_state = &I2C1_recen ;
	I2C1TRN =  I2C1_CommandByte | 0x01;
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
	I2C1_serve_queue();	
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

#endif  // UDB4 BOARD
