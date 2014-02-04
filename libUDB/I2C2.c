
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
#include "interrupt.h"
#include "NV_memory.h"
#include "events.h"

//void I2C2_init(void);
#if  (I2C2_QUEUED == 1)   					// /////////////////    I2C2_QUEUED (options.h)   /////////////////

	#include "oscillator.h"
	#define I2C2_SDA 		_LATA3    //  from _RA3, _RA2, mods per Bill P.
	#define I2C2_SCL 		_LATA2 
	
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
	void I2C2_readCommand(void);
	void I2C2_writeCommand(void);
	void I2C2_startWrite(void);
	void I2C2_readStart(void);
	void I2C2_Failed(void);
	void I2C2_doneWrite(void);
	void I2C2_writeCommandData(void);
	
	void serviceI2C2(void);  // service the I2C
	//void I2C2_init(void);
	
	int16_t I2C2ERROR	= 0 ;
	int16_t I2C2MAXS	= 0 ;
	int16_t I2C2MAXQ	= 0 ;
	
	// Port busy flag.  Set true until initialized
	boolean I2C2_Busy = true;
	
	void (* I2C2_state ) ( void ) = &I2C2_idle ;
	#define I2C2FSCL 400000						// Bus speed measured in Hz
	#define I2C2BRGVAL ((FREQOSC/(CLK_PHASES *I2C2FSCL))-(FREQOSC/(CLK_PHASES * 10000000)))-1
	
	#define I2C2_NORMAL ( (I2C2STAT & 0b0000010011000000) == 0 )	// There is the queue, it's ok if the module is reading
	
	#define I2C2_QUEUE_DEPTH	3									
	I2Cqueue		i2c2_queue[I2C2_QUEUE_DEPTH];
	
	uint16_t I2C2_Index = 0;  		// index into the write buffer
	
	uint8_t I2C2_CommandByte 	= 0;
	uint16_t I2C2_tx_data_size = 0;		// tx data size
	uint16_t I2C2_rx_data_size = 0;		// rx data size
	uint16_t I2C2_command_data_size = 0;	// command data size
	
	uint8_t* pI2C2Buffer = NULL;	// pointer to buffer
	uint8_t* pI2C2commandBuffer = NULL;	// pointer to receive  buffer
	
	uint16_t I2C2_service_handle = INVALID_HANDLE;
	
	uint16_t	I2C2_ERROR = 0;		
	
	void I2C2_init(void)
	{
		int16_t queueIndex;
	
		for(queueIndex = 0; queueIndex < I2C2_QUEUE_DEPTH; queueIndex++)
		{
			i2c2_queue[queueIndex].pending = false;
			i2c2_queue[queueIndex].rW = 0;
			i2c2_queue[queueIndex].command = 0;
			i2c2_queue[queueIndex].pcommandData = NULL;
			i2c2_queue[queueIndex].commandDataSize = 0;
			i2c2_queue[queueIndex].pData = NULL;
			i2c2_queue[queueIndex].Size = 0;
			i2c2_queue[queueIndex].pCallback = NULL;	
		}	
	
		I2C2BRG = I2C2BRGVAL ; 
		_I2C2EN = 1 ; 	 		// enable I2C2		
	
		_MI2C2IP = 5 ; 			// I2C2 at priority 5
		_MI2C2IF = 0 ; 			// clear the I2C2 master interrupt
		_MI2C2IE = 1 ; 			// enable the interrupt
	
	//	I2C2_service_handle = register_event(&serviceI2C2);
		
		I2C2_Busy = false;
	
		return ;
	}
	
	void I2C2_reset(void)
	{
		I2C2_state = &I2C2_idle ;		// disable the response to any more interrupts
		I2C2_ERROR = I2C2STAT ; 		// record the error for diagnostics
		
		_I2C2EN = 0 ;  					// turn off the I2C
		_MI2C2IF = 0 ; 					// clear the I2C master interrupt
		_MI2C2IE = 0 ; 					// disable the interrupt
		// pull SDA and SCL low
		I2C2_SCL = 0 ;
		I2C2_SDA = 0 ; 		
		Nop();
		// pull SDA and SCL high
		I2C2_SCL = 1 ;
		I2C2_SDA = 1 ; 		
	
		I2C2CON = 0x1000;
		I2C2STAT = 0x0000;
		I2C2_init() ;
		return ;
	}
	
	// Trigger the I2C2 service routine to run at low priority
	void I2C2_trigger_service(void)
	{
		trigger_event(I2C2_service_handle);
	};
	
	
	void serviceI2C2(void)  // service the I2C
	{
	//	unsigned int counter;
	
		if ( _I2C2EN == 0 ) // I2C is off
		{
			I2C2_state = &I2C2_idle ; 	// disable response to any interrupts
	 	// pull SDA and SCL high - preserved for posterity
	//		I2C2_SDA = 1 ;
	//	 	I2C2_SCL = 1 ;
			I2C2_init() ; 			// turn the I2C back on
			// Put something here to reset state machine.  Make sure attached servies exit nicely.
			return ;
		}
	
		return ;
	}
	
	
	void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C2Interrupt(void)
	{
		indicate_loading_inter ;
		interrupt_save_set_corcon ;
		
		_MI2C2IF = 0 ; // clear the interrupt
		(* I2C2_state) () ; // execute the service routine
		
		interrupt_restore_corcon ;
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
	
	boolean I2C2_Normal(void)
	{
		if(I2C2_NORMAL )
			return true;
		else
		{
			I2C2_ERROR = I2C2STAT;
			return false;
		}	
	}
	
	boolean I2C2_serve_queue()
	{
		int queueIndex;
		
		for(queueIndex = 0; queueIndex < I2C2_QUEUE_DEPTH; queueIndex++)
		{
			if(i2c2_queue[queueIndex].pending == true)
			{
				I2C2MAXS = queueIndex;
	
				if(!I2C2_CheckAvailable())
				{
					return false;
				}	
				else i2c2_queue[queueIndex].pending = false;
	
				pI2C2_callback			= i2c2_queue[queueIndex].pCallback;
				I2C2_command_data_size 	= i2c2_queue[queueIndex].commandDataSize;
				pI2C2commandBuffer		= i2c2_queue[queueIndex].pcommandData;
				I2C2_CommandByte 		= i2c2_queue[queueIndex].command;
				pI2C2Buffer 			= i2c2_queue[queueIndex].pData;
				
				if(i2c2_queue[queueIndex].rW == 0)
				{
					I2C2_tx_data_size = i2c2_queue[queueIndex].Size;		// tx data size
					I2C2_rx_data_size = 0;					// rx data size
				}
				else
				{
					I2C2_tx_data_size = 0;					// tx data size
					I2C2_rx_data_size = i2c2_queue[queueIndex].Size;		// rx data size
				}		
				
				// Set ISR callback and trigger the ISR
				I2C2_state = &I2C2_startWrite;
				_MI2C2IF = 1 ;
				return true;	
				
			}
		}
		return false;
	}
	
	boolean I2C2_Write(uint8_t command, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* ptxData, uint16_t txSize, I2C_callbackFunc pCallback)
	{
		uint16_t queueIndex;
	
		for(queueIndex = 0; queueIndex < I2C2_QUEUE_DEPTH; queueIndex++)
		{
			if(i2c2_queue[queueIndex].pending == false)
			{
				if(queueIndex >I2C2MAXQ) I2C2MAXQ = queueIndex;
				i2c2_queue[queueIndex].pending = true;
				i2c2_queue[queueIndex].rW = 0;
				i2c2_queue[queueIndex].command = command;
				i2c2_queue[queueIndex].pcommandData = pcommandData;
				i2c2_queue[queueIndex].commandDataSize = commandDataSize;
				i2c2_queue[queueIndex].pData = ptxData;
				i2c2_queue[queueIndex].Size = txSize;
				i2c2_queue[queueIndex].pCallback = pCallback;
				return I2C2_serve_queue();
			}
		}
	
	/*	while(1)	// STOP HERE ON FAILURE.
		{
			LED_GREEN = LED_ON;	
		}			
	*/
		I2C2_reset();
		return false;
	}
	
	boolean I2C2_Read(uint8_t command, uint8_t* pcommandData, uint8_t commandDataSize, uint8_t* prxData, uint16_t rxSize, I2C_callbackFunc pCallback)
	{
		int queueIndex;
	
		for(queueIndex = 0; queueIndex < I2C2_QUEUE_DEPTH; queueIndex++)
		{
			if(i2c2_queue[queueIndex].pending == false)
			{
				if(queueIndex >I2C2MAXQ) I2C2MAXQ = queueIndex;
				i2c2_queue[queueIndex].pending = true;
				i2c2_queue[queueIndex].rW = 1;
				i2c2_queue[queueIndex].command = command;
				i2c2_queue[queueIndex].pcommandData = pcommandData;
				i2c2_queue[queueIndex].commandDataSize = commandDataSize;
				i2c2_queue[queueIndex].pData = prxData;
				i2c2_queue[queueIndex].Size = rxSize;
				i2c2_queue[queueIndex].pCallback = pCallback;
				return I2C2_serve_queue();
			}
		}
	
	/*	while(1)		// STOP HERE ON FAILURE.
		{
			LED_GREEN = LED_ON;	
		}			
	*/
		I2C2_reset();
		return false;
		
	}
	
	
	// Only send command byte to check for ACK.
	boolean I2C2_checkACK(uint16_t command, I2C_callbackFunc pCallback)
	{
		if(!I2C2_CheckAvailable()) return false;
	
		pI2C2_callback = pCallback;
	
		I2C2_command_data_size 	= 0;
		I2C2_CommandByte 		= command;
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
	
		I2C2_state = &I2C2_writeCommand ;
		I2C2CONbits.SEN = 1 ;
		return ;
	}
	
	// Write command byte without checking ACK first.
	void I2C2_writeCommand(void)
	{
		I2C2TRN = I2C2_CommandByte & 0xFE ;
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
		I2C2_serve_queue();	  					//  **** NEW QUEUE FEATURE  *****
		return;
	}
	
	// Start a read after a write by settign the start bit again
	void I2C2_readStart(void)
	{
		I2C2_Index = 0;  			// Reset index into buffer
		I2C2_state = &I2C2_readCommand ;
		I2C2CONbits.SEN = 1 ;	
	}
	
	// Send the command to read
	void I2C2_readCommand(void)
	{
		I2C2_state = &I2C2_recen ;
		I2C2TRN =  I2C2_CommandByte | 0x01;
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
		I2C2_serve_queue();  
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

#else  									//  /////////////////   I2C2_QUEUED  //////////////////

	/*
	#define USE_I2C_SECOND_PORT_DRIVER 1
	#if (USE_I2C_SECOND_PORT_DRIVER == 1)
	*/
	
	#define I2C2_SDA        _RA3
	#define I2C2_SCL        _RA2
	#define I2C2_SDA_TRIS   _TRISA3
	#define I2C2_SCL_TRIS   _TRISA2
	#define _I2C2EN         I2C2CONbits.I2CEN
	
	// Calculate the BRGvalue automatically
	//#define I2C1FSCL 400000 // Bus speed measured in Hz
	//#define I2C1BRGVAL ((FREQOSC/(CLK_PHASES *I2C1FSCL))-(FREQOSC/(CLK_PHASES * 10000000)))-1
	#define I2C2BRGVAL 60 // 200 Khz
	#define I2C2_NORMAL (((I2C2CON & 0b0000000000011111) == 0) && ((I2C2STAT & 0b0100010011000001) == 0))
	
	//void I2C2_init(void);
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
	
	static int16_t I2C2ERROR = 0;
	static boolean I2C2_Busy = true;    // Port busy flag.  Set true until initialized
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
		if (_I2C2EN == 0)       // I2C is off
		{
			I2C2_init();
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
		x.state = &isr_idle;    // disable the response to any more interrupts
		I2C2ERROR = I2C2STAT;   // record the error for diagnostics
	
		_I2C2EN = 0;            // turn off the I2C
		_MI2C2IF = 0;           // clear the I2C master interrupt
		_MI2C2IE = 0;           // disable the interrupt
		I2C2_SDA = 0;           // pull SDA and SCL low
		I2C2_SCL = 0;
		Nop();
		I2C2_SDA = 1;           // pull SDA and SCL high
		I2C2_SCL = 1;
	
		I2C2_init();            // enable the bus again
	}
	
	void I2C2_init(void)
	{
		I2C2BRG = I2C2BRGVAL;
		_I2C2EN = 1;            // enable I2C2
	
		_MI2C2IP = INT_PRI_I2C2;// set interrupt priority
		_MI2C2IF = 0;           // clear the I2C2 master interrupt
		_MI2C2IE = 1;           // enable the interrupt
	
		I2C2_service_handle = register_event(&serviceI2C2);
	
		I2C2_Busy = false;
	
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
	
	// Trigger the I2C2 service routine to run at low priority
	void I2C2_trigger_service(void)
	{
		trigger_event(I2C2_service_handle);
	}
	
	static void serviceI2C2(void)   // service the I2C
	{
		if (_I2C2EN == 0)           // I2C is off
		{
			x.state = &isr_idle;    // disable response to any interrupts
			I2C2_init();            // turn the I2C back on
			// Put something here to reset state machine.
			// Make sure attached services exit nicely.
		}
	}
	
	void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C2Interrupt(void)
	{
		indicate_loading_inter;
		interrupt_save_set_corcon;
	
		_MI2C2IF = 0;               // clear the interrupt
		(*x.state)();               // execute the service routine
	
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
	
		x.addr          = addr;
		x.cmd           = cmd;
		x.cmd_len       = cmd_len;
		x.data          = data;
		x.tx_data_len   = data_len;
		x.rx_data_len   = 0;
		x.callback      = callback;
	
		x.mode          = I2C_MODE_WRITE;
		x.state         = &isr_startWrite;
		_MI2C2IF        = 1;
		return true;
	}
	
	boolean I2C2_Read(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback, uint16_t mode)
	{
		if (!I2C2_CheckAvailable()) return false;
	
		x.addr          = addr;
		x.cmd           = cmd;
		x.cmd_len       = cmd_len;
		x.data          = data;
		x.tx_data_len   = 0;
		x.rx_data_len   = data_len;
		x.callback      = callback;
		x.mode          = mode;
	
		x.state         = &isr_startWrite;
		_MI2C2IF        = 1;
		return true;
	}
	
	// Only send command byte to check for ACK.
	boolean I2C2_checkACK(uint16_t addr, I2C_callbackFunc callback)
	{
		if (!I2C2_CheckAvailable()) return false;
	
		x.addr          = addr;
		x.callback      = callback;
	
		x.cmd_len       = 0;
		x.data          = NULL;
		x.tx_data_len   = 0;
		x.rx_data_len   = 0;
	
		x.mode          = I2C_MODE_WRITE;
		x.state         = &isr_startWrite;
		_MI2C2IF        = 1;
		return true;
	}
	
	static void isr_startWrite(void)
	{
		x.index = 0;                            // Reset index into buffer
	
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
		if (I2C2STATbits.ACKSTAT == 1)          // Device not responding
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
				x.index = 0;                    // Reset index into the buffer
				if (x.rx_data_len > 0)
					x.state = &isr_readStart;
				else
					x.state = &isr_writeData;
			}
		}
	}
	
	static void isr_writeData(void)
	{
		if (I2C2STATbits.ACKSTAT == 1)          // Device not responding
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
		x.index = 0;                            // Reset index into buffer
		x.state = &isr_readAddress;
		I2C2CONbits.SEN = 1;
	}
	
	// Send the address to read
	static void isr_readAddress(void)
	{
		I2C2TRN = x.addr | 0x01;
	
		if (x.mode == I2C_MODE_READ_ONLY)
		{
			x.index = 0;                        // Reset index into the buffer
	
			if (x.cmd_len == 0)
				x.state = &isr_recen;           // Read the data
			else
				x.state = &isr_writeCommandData;// Write the command data
		}
		else
		{
			x.state = &isr_recen;
		}
	}
	
	// Check for ACK.  If ok, start receive mode, otherwise abandon.
	static void isr_recen(void)
	{
		if (I2C2STATbits.ACKSTAT == 1)          // Device not responding
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

#endif 								//  /////////////////   I2C2_QUEUED  //////////////////
