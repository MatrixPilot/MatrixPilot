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

#if (BOARD_IS_CLASSIC_UDB == 1)

////////////////////////////////////////////////////////////////////////////////
//
// GPS

void udb_init_GPS(void)
{
	//	Initialize the USART that communicates with the GPS
	
	U2MODE = 0b1010000000000000 ; // turn on
	U2STA  = 0b0000010100010000 ;
	
	U2STAbits.UTXEN = 1 ; // turn on transmitter
	
	_U2RXIF = 0 ; // clear the interrupt
	_U2RXIP = 4 ; // priority 4
	_U2RXIE = 1 ; // turn on the interrupt
	
	_U2TXIF = 0 ; // clear the interrupt 
	_U2TXIP = 4 ; // priority 4 
	_U2TXIE = 1 ; // turn on the interrupt
	
	return ;
}


void udb_gps_set_rate(long rate)
{
	U2BRG = UDB_BAUD(rate) ;
	return ;
}


boolean udb_gps_check_rate(long rate)
{
	return ( U2BRG == UDB_BAUD(rate) ) ;
}


void udb_gps_start_sending_data(void)
{
	_U2TXIF = 1 ; // fire the tx interrupt
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_U2RXIF = 0 ; // clear the interrupt
	while ( U2STAbits.URXDA )
	{
		unsigned char rxchar = U2RXREG ;
		udb_gps_callback_received_byte(rxchar) ;
	}

	U2STAbits.OERR = 0 ; // clear the overrun bit, just in case
	
	interrupt_restore_corcon ;
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U2TXInterrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_U2TXIF = 0 ; // clear the interrupt 
	
	int txchar = udb_gps_callback_get_byte_to_send() ;
	
	if ( txchar != -1 )
	{
		U2TXREG = (unsigned char)txchar ;
	}
	
	interrupt_restore_corcon ;
	return ;
}


////////////////////////////////////////////////////////////////////////////////
//
// Serial

void udb_init_USART(void)
{
	//	debugging/telemetry USART, runs at 19200 baud
	U1MODE = 0b1010000000000000 ; // turn on
	U1STA  = 0b0000010100010000 ;
	
	U1MODEbits.ALTIO = 1 ; // use alternate pins
	
	U1STAbits.UTXEN = 1 ; // turn on transmitter	
	
	_U1RXIF = 0 ; // clear the interrupt
	_U1RXIP = 4 ; // priority 4
	_U1RXIE = 1 ; // turn on the interrupt
	
	_U1TXIF = 0 ; // clear the interrupt 
	_U1TXIP = 4 ; // priority 4 
	_U1TXIE = 1 ; // turn on the interrupt
	
	return ;
}


void udb_serial_set_rate(long rate)
{
	U1BRG = UDB_BAUD(rate) ;
	return ;
}


boolean udb_serial_check_rate(long rate)
{
	return ( U1BRG == UDB_BAUD(rate) ) ;
}


void udb_serial_start_sending_data(void)
{
	_U1TXIF = 1 ; // fire the tx interrupt
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_U1RXIF = 0 ; // clear the interrupt
	while ( U1STAbits.URXDA )
	{
		unsigned char rxchar = U1RXREG ;	
		udb_serial_callback_received_byte(rxchar) ;
	}

	U1STAbits.OERR = 0 ;	// clear the overrun bit, just in case	
	
	interrupt_restore_corcon ;
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U1TXInterrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	_U1TXIF = 0 ; // clear the interrupt 
	
	int txchar = udb_serial_callback_get_byte_to_send() ;
	
	if ( txchar != -1 )
	{
		U1TXREG = (unsigned char)txchar ;
	}
	
	interrupt_restore_corcon ;
	return ;
}

#endif
