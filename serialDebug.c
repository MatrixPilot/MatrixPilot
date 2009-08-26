#include "p30f4011.h"
#include "defines.h"
#include <stdio.h>

int db_index = 0 ;

char debug_buffer[128] ;

void init_USART1(void)
{	
//	debugging USART, runs at 19200 baud
	U1MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U1STA  = 0b0000010100010000 ;

//	U1BRG =  51 ; // 4800 baud
	U1BRG =  12 ; // 19200 baud

	U1MODEbits.UARTEN = 1 ; // turn on uart
	U1MODEbits.ALTIO = 1 ; // alternate pins
	
	U1STAbits.UTXEN = 1 ; // turn on transmitter

	IFS0bits.U1RXIF = 0 ; // clear the interrupt
	IPC2bits.U1RXIP = 3 ; // priority 3
	IEC0bits.U1RXIE = 1 ; // turn on the interrupt

	db_index = 0 ;

	IFS0bits.U1TXIF = 0 ; // clear the interrupt 
 	IPC2bits.U1TXIP = 4 ; // priority 4 
 	IEC0bits.U1TXIE = 1 ; // turn on the interrupt

	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt(void)
{
	char rxchar ;
	indicate_loading_inter ;
	rxchar = U1RXREG ;
	if ( U2STAbits.FERR ) {  init_USART1(); }
	else if ( U2STAbits.OERR ) {  init_USART1(); }
	else { __builtin_btg ( &LATE , 8 ) ; }

	IFS0bits.U1RXIF = 0 ; // clear the interrupt
	return ;
}


void debug_output( void )
{
	unsigned char txchar ;
	db_index = 0 ;
	sprintf( debug_buffer , "lat: %li, long: %li, alt: %li\r\nrmat: %i, %i, %i, %i, %i, %i, %i, %i, %i\r\n" , 
		lat_gps.WW , long_gps.WW , alt_sl_gps.WW , 
		rmat[0] , rmat[1] , rmat[2] , 
		rmat[3] , rmat[4] , rmat[5] , 
		rmat[6] , rmat[7] , rmat[8]  ) ;
	txchar = debug_buffer[ db_index++ ] ;
	U1TXREG = txchar ;
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1TXInterrupt(void)
{
	unsigned char txchar ;
	IFS0bits.U1TXIF = 0 ; // clear the interrupt 
	txchar = debug_buffer[ db_index++ ] ;
	if ( txchar ) U1TXREG = txchar ;
	return ;
}

