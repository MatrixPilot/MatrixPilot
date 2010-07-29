// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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
	U2MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U2STA  = 0b0000010100010000 ;
	
	U2MODE = 0b1010000000000000 ; // turn on
	U2STA  = 0b0000010100010000 ;
	
	_U2RXIF = 0 ; // clear the interrupt
	_U2RXIP = 4 ; // priority 4
	_U2RXIE = 1 ; // turn on the interrupt
	
	return ;
}


void udb_gps_set_rate(int rate)
{
	U2BRG = UDB_BAUD(rate) ;
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U2RXInterrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	if ( U2STAbits.FERR ) udb_init_GPS();
	else if ( U2STAbits.OERR ) udb_init_GPS();

	_U2RXIF = 0 ; // clear the interrupt
	while ( U2STAbits.URXDA )
	{
		unsigned char rxchar = U2RXREG ;
		udb_gps_callback_received_char(rxchar) ;
	}
	
	interrupt_restore_extended_state ;
	return ;
}


void udb_gps_send_char ( char outchar ) // output one character to the GPS
{
	//bin_out(outchar);
	while ( U2STAbits.UTXBF ) { }
	U2TXREG = outchar ;
	return ;
}



////////////////////////////////////////////////////////////////////////////////
//
// Serial

void udb_init_USART(void)
{	
	//	debugging/telemetry USART, runs at 19200 baud
	U1MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U1STA  = 0b0000010100010000 ;
	
	U1MODEbits.UARTEN = 1 ; // turn on uart
	U1MODEbits.ALTIO = 1 ; // use alternate pins
	
	U1STAbits.UTXEN = 1 ; // turn on transmitter
	
	_U1RXIF = 0 ; // clear the interrupt
	_U1RXIP = 3 ; // priority 3
	_U1RXIE = 1 ; // turn on the interrupt
	
	_U1TXIF = 0 ; // clear the interrupt 
 	_U1TXIP = 4 ; // priority 4 
 	_U1TXIE = 1 ; // turn on the interrupt
	
	return ;
}


void udb_serial_set_rate(int rate)
{
	U1BRG = UDB_BAUD(rate) ;
	return ;
}


void udb_serial_start_sending(void)
{
	_U1TXIF = 1 ; // fire the tx interrupt
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt(void)
{
	// interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	if ( U1STAbits.FERR ) udb_init_USART();
	else if ( U1STAbits.OERR ) udb_init_USART();
	
	char rxchar = U1RXREG ;
	
	_U1RXIF = 0 ; // clear the interrupt
	
	udb_serial_callback_received_char(rxchar) ;
	
	// interrupt_restore_extended_state ;
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _U1TXInterrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	_U1TXIF = 0 ; // clear the interrupt 
	
	unsigned char txchar = udb_serial_callback_get_char_to_send() ;
	
	if ( txchar )
	{
		U1TXREG = txchar ;
	}
	
	interrupt_restore_extended_state ;
	return ;
}

#endif
