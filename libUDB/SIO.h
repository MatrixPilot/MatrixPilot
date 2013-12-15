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


#include "oscillator.h"
#include "interrupt.h"

//
// USAGE:
//
// SIO_DEFINE(UART, rx_callback, RX_INT_PRI, tx_callback, TX_INT_PRI)
//

typedef void (*sio_rx_callback)(uint8_t);
typedef int16_t (*sio_tx_callback)(void);

// Baud Rate Generator -- See section 19.3.1 of datasheet.
// Fcy = FREQOSC / CLK_PHASES
// UXBRG = (Fcy/(16*BaudRate))-1
// UXBRG = ((32000000/2)/(16*9600))-1
// UXBRG = 103

#define UDB_BAUD(x) ((int16_t)((FREQOSC / CLK_PHASES) / ((int32_t)4 * x) - 1))

#define SIO_RX_CALLBACK(w) static sio_rx_callback w##_rx_callback;
#define SIO_TX_CALLBACK(w) static sio_tx_callback w##_tx_callback;

#define SIO_INIT(w, x) \
void w##_sio_init(sio_rx_callback rx_callback, int rx_pri, sio_tx_callback tx_callback, int tx_pri) \
{ \
	w##_rx_callback = rx_callback; \
	w##_tx_callback = tx_callback; \
	                                \
	/* configure UxMODE */ \
	U##x##MODEbits.UARTEN = 0;      /* Bit15 TX, RX DISABLED, ENABLE at end of func */ \
	                                /* Bit14 */ \
	U##x##MODEbits.USIDL = 0;       /* Bit13 Continue in Idle */ \
	U##x##MODEbits.IREN = 0;        /* Bit12 No IR translation */ \
	U##x##MODEbits.RTSMD = 0;       /* Bit11 Simplex Mode */ \
	                                /* Bit10 */ \
	U##x##MODEbits.UEN = 0;         /* Bits8,9 TX,RX enabled, CTS,RTS not */ \
	U##x##MODEbits.WAKE = 0;        /* Bit7 No Wake up (since we don't sleep here) */ \
	U##x##MODEbits.LPBACK = 0;      /* Bit6 No Loop Back */ \
	U##x##MODEbits.ABAUD = 0;       /* Bit5 No Autobaud (would require sending '55') */ \
	U##x##MODEbits.URXINV = 0;      /* Bit4 IdleState = 1  (for dsPIC) */ \
	U##x##MODEbits.BRGH = 1;        /* Bit3 4 clocks per bit period */ \
	U##x##MODEbits.PDSEL = 0;       /* Bits1,2 8bit, No Parity */ \
	U##x##MODEbits.STSEL = 0;       /* Bit0 One Stop Bit */ \
	                                \
	/* Load all values in for UxSTA SFR */ \
	U##x##STAbits.UTXISEL1 = 0;     /*Bit15 Int when Char is transferred (1/2 config!) */ \
	U##x##STAbits.UTXINV = 0;       /*Bit14 N/A, IRDA config */ \
	U##x##STAbits.UTXISEL0 = 1;     /*Bit13 Other half of Bit15 */ \
	                                /*Bit12 */ \
	U##x##STAbits.UTXBRK = 0;       /*Bit11 Disabled */ \
	/*U##x##STAbits.UTXEN = 1;*/        /*Bit10 TX pins controlled by periph (handled below) */ \
	/*U##x##STAbits.UTXBF = 0;*/        /*Bit9 *Read Only Bit* */ \
	/*U##x##STAbits.TRMT = 0;*/         /*Bit8 *Read Only bit* */ \
	U##x##STAbits.URXISEL = 0;      /*Bits6,7 Int. on character recieved */ \
	U##x##STAbits.ADDEN = 0;        /*Bit5 Address Detect Disabled */ \
	/*U##x##STAbits.RIDLE = 0;*/        /*Bit4 *Read Only Bit* */ \
	/*U##x##STAbits.PERR = 0;*/         /*Bit3 *Read Only Bit* */ \
	/*U##x##STAbits.FERR = 0;*/         /*Bit2 *Read Only Bit* */ \
	U##x##STAbits.OERR = 0;         /*Bit1 *Read Only Bit* */ \
	/*U##x##STAbits.URXDA = 0;*/        /*Bit0 *Read Only Bit* */ \
 	                                \
	_U##x##RXIP = rx_pri;           /* Receive Interrupt Priority level */ \
	_U##x##TXIP = tx_pri;           /* Transmit Interrupt Priority level */ \
 	                                \
	_U##x##TXIF = 0;                /* Clear the Transmit Interrupt Flag */ \
	_U##x##TXIE = 1;                /* Disable Transmit Interrupts */ \
	_U##x##RXIF = 0;                /* Clear the Receive Interrupt Flag */ \
	_U##x##RXIE = 1;                /* Enable Receive Interrupts */ \
 	                                \
	U##x##MODEbits.UARTEN = 1;      /* And turn the peripheral on */ \
	U##x##STAbits.UTXEN = 1; \
}

#define SIO_SET_BAUD(w, x) \
void w##_sio_set_baud(int32_t rate) { \
	U##x##BRG = UDB_BAUD(rate); \
}

#define SIO_CHK_BAUD(w, x) \
boolean w##_sio_chk_baud(int32_t rate) { \
	return (U##x##BRG == UDB_BAUD(rate)); \
}

#define SIO_START_TX(w, x) \
void w##_sio_start_tx(void) { \
	_U##x##TXIF = 1; \
}

#define SIO_TX_INT(w, x) \
void __attribute__((__interrupt__,__no_auto_psv__)) _U##x##TXInterrupt(void) { \
	_U##x##TXIF = 0; \
	indicate_loading_inter; \
	interrupt_save_set_corcon; \
	int16_t txchar = w##_tx_callback(); \
	if (txchar != -1) { \
		U##x##TXREG = (uint8_t)txchar; \
	} \
	interrupt_restore_corcon; \
}

#define SIO_RX_INT(w, x) \
void __attribute__((__interrupt__,__no_auto_psv__)) _U##x##RXInterrupt(void) { \
	_U##x##RXIF = 0; \
	indicate_loading_inter; \
	interrupt_save_set_corcon; \
	while (U##x##STAbits.URXDA) { \
		uint8_t rxchar = U##x##RXREG; \
		w##_rx_callback(rxchar); \
	} \
	U##x##STAbits.OERR = 0; \
	interrupt_restore_corcon; \
}

#define SIO_DEFINE(w, x) \
	SIO_RX_CALLBACK(w) \
	SIO_TX_CALLBACK(w) \
	SIO_INIT(w, x) \
	SIO_SET_BAUD(w, x) \
	SIO_CHK_BAUD(w, x) \
	SIO_START_TX(w, x) \
	SIO_TX_INT(w, x) \
	SIO_RX_INT(w, x)

//SIO_DEFINE(gps, 1)
