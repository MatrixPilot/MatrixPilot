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
#include "heartbeat.h"

// Baud Rate Generator -- See section 19.3.1 of datasheet.
// Fcy = FREQOSC / CLK_PHASES
// UXBRG = (Fcy/(16*BaudRate))-1
// UXBRG = ((32000000/2)/(16*9600))-1
// UXBRG = 103

#define UDB_BAUD(x) ((int16_t)(FCY / ((int32_t)4 * x) - 1))

// to be used with OpenLog for software flow control
// Warning: imcompatible with mavlink binary uplink
boolean pauseSerial = false;


////////////////////////////////////////////////////////////////////////////////
//
// GPS

void udb_init_GPS(void)
{
	// configure U1MODE
	U1MODEbits.UARTEN = 0;      // Bit15 TX, RX DISABLED, ENABLE at end of func
	//                          // Bit14
	U1MODEbits.USIDL = 0;       // Bit13 Continue in Idle
	U1MODEbits.IREN = 0;        // Bit12 No IR translation
	U1MODEbits.RTSMD = 0;       // Bit11 Simplex Mode
	//                          // Bit10
	U1MODEbits.UEN = 0;         // Bits8,9 TX,RX enabled, CTS,RTS not
	U1MODEbits.WAKE = 0;        // Bit7 No Wake up (since we don't sleep here)
	U1MODEbits.LPBACK = 0;      // Bit6 No Loop Back
	U1MODEbits.ABAUD = 0;       // Bit5 No Autobaud (would require sending '55')
	U1MODEbits.URXINV = 0;      // Bit4 IdleState = 1  (for dsPIC)
	U1MODEbits.BRGH = 1;        // Bit3 4 clocks per bit period
	U1MODEbits.PDSEL = 0;       // Bits1,2 8bit, No Parity
	U1MODEbits.STSEL = 0;       // Bit0 One Stop Bit

	// Load all values in for U1STA SFR
	U1STAbits.UTXISEL1 = 0;     //Bit15 Int when Char is transferred (1/2 config!)
	U1STAbits.UTXINV = 0;       //Bit14 N/A, IRDA config
	U1STAbits.UTXISEL0 = 1;     //Bit13 Other half of Bit15
	//                          //Bit12
	U1STAbits.UTXBRK = 0;       //Bit11 Disabled
	//U1STAbits.UTXEN = 1;        //Bit10 TX pins controlled by periph (handled below)
	//U1STAbits.UTXBF = 0;        //Bit9 *Read Only Bit*
	//U1STAbits.TRMT = 0;         //Bit8 *Read Only bit*
	U1STAbits.URXISEL = 0;      //Bits6,7 Int. on character recieved
	U1STAbits.ADDEN = 0;        //Bit5 Address Detect Disabled
	//U1STAbits.RIDLE = 0;        //Bit4 *Read Only Bit*
	//U1STAbits.PERR = 0;         //Bit3 *Read Only Bit*
	//U1STAbits.FERR = 0;         //Bit2 *Read Only Bit*
	U1STAbits.OERR = 0;         //Bit1 *Read Only Bit*
	//U1STAbits.URXDA = 0;        //Bit0 *Read Only Bit*

	_U1TXIP = INT_PRI_U1TX;     // Mid Range Interrupt Priority level, no urgent reason
	_U1RXIP = INT_PRI_U1RX;     // Mid Range Interrupt Priority level, no urgent reason

	_U1TXIF = 0;                // Clear the Transmit Interrupt Flag
	_U1TXIE = 1;                // Disable Transmit Interrupts
	_U1RXIF = 0;                // Clear the Receive Interrupt Flag
	_U1RXIE = 1;                // Enable Receive Interrupts

	U1MODEbits.UARTEN = 1;      // And turn the peripheral on
	U1STAbits.UTXEN = 1;
}

void udb_gps_set_rate(int32_t rate)
{
	U1BRG = UDB_BAUD(rate);
}

boolean udb_gps_check_rate(int32_t rate)
{
	return (U1BRG == UDB_BAUD(rate));
}

void udb_gps_start_sending_data(void)
{
	_U1TXIF = 1; // fire the tx interrupt
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1TXInterrupt(void)
{
	_U1TXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

#if (USE_HILSIM_USB != 1)
	int16_t txchar = udb_gps_callback_get_byte_to_send();
#else
	int16_t txchar = -1;
#endif
	if (txchar != -1)
	{
		U1TXREG = (uint8_t)txchar;
	}
	interrupt_restore_corcon;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(void)
{
	_U1RXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;
	
	while (U1STAbits.URXDA)
	{
		uint8_t rxchar = U1RXREG;
#if (USE_HILSIM_USB != 1)
		udb_gps_callback_received_byte(rxchar);
#endif // USE_HILSIM_USB
	}
	U1STAbits.OERR = 0;
	interrupt_restore_corcon;
}


/////////////////////////////////////////////////////////////////////////////////////////
// Serial

void udb_init_USART(void)
{
	// configure U2MODE
	U2MODEbits.UARTEN = 0;      // Bit15 TX, RX DISABLED, ENABLE at end of func
	//                          // Bit14
	U2MODEbits.USIDL = 0;       // Bit13 Continue in Idle
	U2MODEbits.IREN = 0;        // Bit12 No IR translation
	U2MODEbits.RTSMD = 0;       // Bit11 Simplex Mode
	//                          // Bit10
	U2MODEbits.UEN = 0;         // Bits8,9 TX,RX enabled, CTS,RTS not
	U2MODEbits.WAKE = 0;        // Bit7 No Wake up (since we don't sleep here)
	U2MODEbits.LPBACK = 0;      // Bit6 No Loop Back
	U2MODEbits.ABAUD = 0;       // Bit5 No Autobaud (would require sending '55')
	U2MODEbits.URXINV = 0;      // Bit4 IdleState = 1  (for dsPIC)
	U2MODEbits.BRGH = 1;        // Bit3 4 clocks per bit period
	U2MODEbits.PDSEL = 0;       // Bits1,2 8bit, No Parity
	U2MODEbits.STSEL = 0;       // Bit0 One Stop Bit

	// Load all values in for U1STA SFR
	U2STAbits.UTXISEL1 = 0;     //Bit15 Int when Char is transferred (1/2 config!)
	U2STAbits.UTXINV = 0;       //Bit14 N/A, IRDA config
	U2STAbits.UTXISEL0 = 1;     //Bit13 Other half of Bit15
	//                          //Bit12
	U2STAbits.UTXBRK = 0;       //Bit11 Disabled
	//U2STAbits.UTXEN = 1;        //Bit10 TX pins controlled by periph (handled below)
	//U2STAbits.UTXBF = 0;        //Bit9 *Read Only Bit*
	//U2STAbits.TRMT = 0;         //Bit8 *Read Only bit*
	U2STAbits.URXISEL = 0;      //Bits6,7 Int. on character recieved
	U2STAbits.ADDEN = 0;        //Bit5 Address Detect Disabled
	//U2STAbits.RIDLE = 0;        //Bit4 *Read Only Bit*
	//U2STAbits.PERR = 0;         //Bit3 *Read Only Bit*
	//U2STAbits.FERR = 0;         //Bit2 *Read Only Bit*
	U2STAbits.OERR = 0;         //Bit1 *Read Only Bit*
	//U2STAbits.URXDA = 0;        //Bit0 *Read Only Bit*

	_U2TXIP = INT_PRI_U2TX;     // Mid Range Interrupt Priority level, no urgent reason
	_U2RXIP = INT_PRI_U2RX;     // Mid Range Interrupt Priority level, no urgent reason

	_U2TXIF = 0;                // Clear the Transmit Interrupt Flag
	_U2TXIE = 1;                // Enable Transmit Interrupts
	_U2RXIF = 0;                // Clear the Receive Interrupt Flag
	_U2RXIE = 1;                // Enable Receive Interrupts

	U2MODEbits.UARTEN = 1;      // And turn the peripheral on
	U2STAbits.UTXEN = 1;
}

void udb_serial_set_rate(int32_t rate)
{
	U2BRG = UDB_BAUD(rate);
}

boolean udb_serial_check_rate(int32_t rate)
{
	return (U2BRG == UDB_BAUD(rate));
}

void udb_serial_start_sending_data(void)
{
	_U2TXIF = 1; // fire the tx interrupt
}

#ifndef USE_RING_BUFFER
void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void)
{
	_U2TXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

	int16_t txchar = udb_serial_callback_get_byte_to_send();
	if (txchar != -1)
	{
		U2TXREG = (uint8_t)txchar;
	}
	interrupt_restore_corcon;
}
#else

void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void)
{
	_U2TXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

	char txchar;
	boolean status = udb_serial_callback_get_binary_to_send(&txchar);

	if (status)
		U2TXREG = (unsigned char) txchar;

	interrupt_restore_corcon;
	return;
}
#endif  // USE_RING_BUFFER

void __attribute__((__interrupt__, __no_auto_psv__)) _U2RXInterrupt(void)
{
	_U2RXIF = 0; // clear the interrupt
	indicate_loading_inter;
	interrupt_save_set_corcon;

	while (U2STAbits.URXDA)
	{
		uint8_t rxchar = U2RXREG;
		udb_serial_callback_received_byte(rxchar);
	}
	U2STAbits.OERR = 0;
	interrupt_restore_corcon;
}

#if (USE_SBUS_INPUT == 1)
// S.bus and Spektrum satellite input will be on (internal) UART3, if enabled
int (*U3txCallback)(void) = NULL;
void (*U3rxCallback)(char) = NULL;

unsigned char sbuff[24];
unsigned int byteIndex = 0;
unsigned int prevSbusFrame = 0;
boolean sbusDAV = false;
#ifdef SBUS_DIAGNOSTICS
unsigned int sbusPerr = 0, sbusFerr = 0, sbusFint = 0, sbusCount = 0;
#endif
unsigned int sFrameLost = 0, sFailSafe = 0;

int sbus_txCallback(void) {
    return -1;
}

void udb_init_UART3(boolean highSpeed, long baud,
        void (*rx_callback)(char), int (*tx_callback)(void),
        int urxinv, int parity,
        int ipl, boolean rxie, boolean txie) {
    U3rxCallback = rx_callback;
    U3txCallback = tx_callback;

    // disable module and set configuration bits
    // configure mode register for 8 data bits, no parity, 1 stop bit, no flow control
    U3MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
    U3MODEbits.USIDL = 0; // Bit13 Continue in Idle
    U3MODEbits.IREN = 0; // Bit12 No IR translation
    U3MODEbits.RTSMD = 0; // Bit11 Simplex Mode
    U3MODEbits.UEN = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
    U3MODEbits.WAKE = 0; // Bit7 No Wake up (since we don't sleep here)
    U3MODEbits.LPBACK = 0; // Bit6 No Loop Back
    U3MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')
    U3MODEbits.URXINV = urxinv; // Bit4 0->IdleState=1, 1->Idlestate=0
    U3MODEbits.PDSEL = parity; // Bits1,2_ 0:8N, 1:8E, 2:8O, 3:9N
    U3MODEbits.STSEL = 0; // Bit0 One Stop Bit

    // configure status register
    U3STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U3STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
    U3STAbits.UTXISEL0 = 1; //Bit13 Other half of Bit15
    U3STAbits.UTXBRK = 0; //Bit11 Disabled
    U3STAbits.URXISEL = 0; //Bits6,7 Int. on character recieved
    U3STAbits.ADDEN = 0; //Bit5 Address Detect Disabled

    // set baud rate
    if (highSpeed) {
        U3MODEbits.BRGH = 1; // Bit3 4 clocks per bit period
        U3BRG = ((int) ((FREQOSC / CLK_PHASES) / ((long) 4 * baud) - 1));
    } else {
        U3MODEbits.BRGH = 0; // Bit3 16 clocks per bit period
        U3BRG = ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * baud) - 1));
    }

    // set IPL and interrupt enables
    _U3TXIP = ipl;
    _U3RXIP = ipl;

    _U3TXIF = 0; // Clear the Transmit Interrupt Flag
    _U3TXIE = txie;
    _U3RXIF = 0; // Clear the Receive Interrupt Flag
    _U3RXIE = rxie;

    // turn the UART on
    U3MODEbits.UARTEN = 1;
    U3STAbits.UTXEN = 1;
}

void sbus_rxCallback(char rxchar) {
#ifdef SBUS_DIAGNOSTICS
    // check for framing and parity errors
    // U3STAbits.FERR bit 2
    // U3STAbits.PERR bit 3
    if (0b1100 & U3STA) {
        // log error
        if (U3STA & 0b0100) sbusFerr++;
        else sbusPerr++;
    } else // no framing or parity error
#endif
    {
        // frames are 22.5 msec apart (Spektrum DX7)
        // Frame starts with sync byte 0x0F and is followed by 24 bytes
        // is this the start of a new frame?
        unsigned int dt = udb_heartbeat_counter - prevSbusFrame;

        if ((dt >= (int) (.02 * HEARTBEAT_HZ)) && (rxchar == 0x0F)) {
            byteIndex = 0; // start of new frame
            prevSbusFrame = udb_heartbeat_counter;
#ifdef SBUS_DIAGNOSTICS
            sbusFint = dt;
#endif
        }

        // buffer current character
        if (byteIndex < 24)
            sbuff[byteIndex++] = rxchar;
        else // last byte
            // received full frame, indicate data available
#ifdef SBUS_DIAGNOSTICS
            if (rxchar == 0) {
            sbusDAV = true;
            sbusCount++;
        }
#else
            sbusDAV = true;
#endif
    }

}

void parseSbusData() {
    // MatrixPilot expects PCM values in range [2000, 4000]
    // channel indexes start at 1, not zero

    // S.bus channels 1-16 are 11 bit digital
    // with range [0,2047], center ~1024
    // move center to ~3000 by adding 1980 for range [1980, 4027]

    // 8 bits from 1 + lower 3 bits from 2
    udb_pwIn[1] = 1980 + sbuff[1] + ((sbuff[2] & 0x7) << 8);
    // upper 5 bits from 2 + lower 6 bits from 3
    udb_pwIn[2] = 1980 + (sbuff[2] >> 3) + ((sbuff[3] & 0x3F) << 5);
    // upper 2 bits from 3 + 8 bits from 4 + lower 1 bit from 5
    udb_pwIn[3] = 1980 + (sbuff[3] >> 6) + (sbuff[4] << 2) + ((int) (sbuff[5] & 0x01) << 10);
    // upper 7 bits from 5 + lower 4 bits from 6
    udb_pwIn[4] = 1980 + (sbuff[5] >> 1) + ((sbuff[6] & 0xF) << 7);
    // upper 4 bits from 6 + lower 7 bits from 7
    udb_pwIn[5] = 1980 + (sbuff[6] >> 4) + ((int) (sbuff[7] & 0x7F) << 4);
    // upper 1 bit from 7 + 8 bits from 8 + lower 2 bits from 9
    udb_pwIn[6] = 1980 + (sbuff[7] >> 7) + (sbuff[8] << 1) + ((int) (sbuff[9] & 0x03) << 9);
    // upper 6 bits from 9 + lower 5 bits from 10
    udb_pwIn[7] = 1980 + (sbuff[9] >> 2) + ((int) (sbuff[10] & 0x1F) << 6);
    // digital channels and flags are in byte 23
    if (sbuff[23] & 0x4) sFrameLost++; // FrameLost flag set
    if (sbuff[23] & 0x8) {
        // failsafe flag is set
        sFailSafe++;
        failSafePulses = 0;
        udb_flags._.radio_on = 0;
    } else {
        // failsafe flag is clear
        failSafePulses = 44;
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX))
            udb_flags._.radio_on = 1;
        else {
            udb_flags._.radio_on = 0;
        }
    }
}

// configure UART3 for 100K baud communication with Futaba S.bus protocol

void udb_init_Sbus(void) {
    // S.bus signal is on SBUS_PORT, receive only
    udb_init_UART3(false, 100000L,
            &sbus_rxCallback, &sbus_txCallback,
            1, 1,
            4, true, false);

    return;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U3RXInterrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    while (U3STAbits.URXDA) {
        unsigned char rxchar = U3RXREG;
        U3rxCallback(rxchar);
    }

    U3STAbits.OERR = 0; // clear overflow error flag
    _U3RXIF = 0; // clear the interrupt

    interrupt_restore_corcon;
    return;
}
#endif  // USE_SBUS_INPUT
