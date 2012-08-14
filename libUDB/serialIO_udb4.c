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

#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE == AUAV2_BOARD_ALPHA1))

////////////////////////////////////////////////////////////////////////////////
//
// GPS

void udb_init_GPS(void)
{
    // configure U2MODE
    U1MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
    //						// Bit14
    U1MODEbits.USIDL = 0; // Bit13 Continue in Idle
    U1MODEbits.IREN = 0; // Bit12 No IR translation
    U1MODEbits.RTSMD = 0; // Bit11 Simplex Mode
    //						// Bit10
    U1MODEbits.UEN = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
    U1MODEbits.WAKE = 0; // Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0; // Bit6 No Loop Back
    U1MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')
    U1MODEbits.URXINV = 0; // Bit4 IdleState = 1  (for dsPIC)
    ///	U1MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
    U1MODEbits.BRGH = 1; // Bit3 4 clocks per bit period
    U1MODEbits.PDSEL = 0; // Bits1,2 8bit, No Parity
    U1MODEbits.STSEL = 0; // Bit0 One Stop Bit

    // Load all values in for U1STA SFR
    U1STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U1STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
    U1STAbits.UTXISEL0 = 1; //Bit13 Other half of Bit15
    //						//Bit12
    U1STAbits.UTXBRK = 0; //Bit11 Disabled
    //U1STAbits.UTXEN = 1;	//Bit10 TX pins controlled by periph (handled below)
    //U1STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
    //U1STAbits.TRMT = 0;	//Bit8 *Read Only bit*
    U1STAbits.URXISEL = 0; //Bits6,7 Int. on character recieved
    U1STAbits.ADDEN = 0; //Bit5 Address Detect Disabled
    //U1STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
    //U1STAbits.PERR = 0;	//Bit3 *Read Only Bit*
    //U1STAbits.FERR = 0;	//Bit2 *Read Only Bit*
    U1STAbits.OERR = 0; //Bit1 *Read Only Bit*
    //U1STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

    _U1TXIP = 4; // Mid Range Interrupt Priority level, no urgent reason
    _U1RXIP = 4; // Mid Range Interrupt Priority level, no urgent reason

    _U1TXIF = 0; // Clear the Transmit Interrupt Flag
    _U1TXIE = 1; // Disable Transmit Interrupts
    _U1RXIF = 0; // Clear the Recieve Interrupt Flag
    _U1RXIE = 1; // Enable Recieve Interrupts

    U1MODEbits.UARTEN = 1; // And turn the peripheral on

    U1STAbits.UTXEN = 1;

    return;
}

void udb_gps_set_rate(long rate)
{
    U1BRG = UDB_BAUD(rate);
    return;
}

boolean udb_gps_check_rate(long rate)
{
    return ( U1BRG == UDB_BAUD(rate));
}

void udb_gps_start_sending_data(void)
{
    _U1TXIF = 1; // fire the tx interrupt
    return;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1TXInterrupt(void)
{
    indicate_loading_inter;
    interrupt_save_set_corcon;

    _U1TXIF = 0; // clear the interrupt

    int txchar = udb_gps_callback_get_byte_to_send();

    if (txchar != -1)
    {
        U1TXREG = (unsigned char) txchar;
    }

    interrupt_restore_corcon;
    return;
}

#if BOARD_TYPE == AUAV2_BOARD_ALPHA1
unsigned char sbuff[24];
unsigned int byteIndex = 0;
unsigned int prevSbusFrame = 0;
boolean sbusDAV = false;
#ifdef SBUS_DIAGNOSTICS
unsigned int sbusPerr = 0, sbusFerr = 0, sbusFint = 0, sbusCount = 0;
#endif
unsigned int sFrameLost = 0, sFailSafe = 0;

// configure UART1 for 100K baud communication with Futaba S.bus protocol

void udb_init_Sbus(void)
{
    // configure U1MODE
    U1MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
    //						// Bit14
    U1MODEbits.USIDL = 0; // Bit13 Continue in Idle
    U1MODEbits.IREN = 0; // Bit12 No IR translation
    U1MODEbits.RTSMD = 0; // Bit11 Simplex Mode
    //						// Bit10
    U1MODEbits.UEN = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
    U1MODEbits.WAKE = 0; // Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0; // Bit6 No Loop Back
    U1MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')
    U1MODEbits.URXINV = 1; // Bit4 IdleState = 0  (for S.bus)
    U1MODEbits.BRGH = 0; // Bit3 16 clocks per bit period
    U1BRG = ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * 100000) - 1)); // S.bus rate 100K baud
    U1MODEbits.PDSEL = 1; // Bits1,2 8bit, even Parity
    U1MODEbits.STSEL = 1; // Bit0 two Stop Bits

    // Load all values in for U1STA SFR
    U1STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U1STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
    U1STAbits.UTXISEL0 = 1; //Bit13 Other half of Bit15
    //						//Bit12
    U1STAbits.UTXBRK = 0; //Bit11 Disabled
    U1STAbits.URXISEL = 0; //Bits6,7 Int. on character recieved
    U1STAbits.ADDEN = 0; //Bit5 Address Detect Disabled

    _U1TXIF = 0; // Clear the Transmit Interrupt Flag
    _U1TXIE = 0; // Disable Transmit Interrupts
    _U1RXIF = 0; // Clear the Recieve Interrupt Flag
    _U1RXIE = 1; // Enable Recieve Interrupts

    U1MODEbits.UARTEN = 1; // And turn the peripheral on
    U1STAbits.UTXEN = 1; //Bit10 TX pins controlled by periph (handled below)

    return;
}

#endif

#if BOARD_TYPE != AUAV2_BOARD_ALPHA1

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(void)
{
    indicate_loading_inter;
    interrupt_save_set_corcon;

    while (U1STAbits.URXDA)
    {
        unsigned char rxchar = U1RXREG;
        udb_gps_callback_received_byte(rxchar);
    }

    U1STAbits.OERR = 0;

    _U1RXIF = 0; // clear the interrupt

    interrupt_restore_corcon;
    return;
}
#else

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(void)
{
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned char rxchar = U1RXREG;

#ifdef SBUS_DIAGNOSTICS
    // check for framing and parity errors
    // U1STAbits.FERR bit 2
    // U1STAbits.PERR bit 3
    if (0b1100 & U1STA)
    {
        // log error
        if (U1STA & 0b0100) sbusFerr++;
        else sbusPerr++;
    }
    else // no framing or parity error
#endif
    {
        // frames are 22.5 msec apart (Spektrum DX7)
        // Frame starts with sync byte 0x0F and is followed by
        // is this the start of a new frame?
        unsigned int dt = udb_heartbeat_counter - prevSbusFrame;

        if ((dt >= (int) (.02 * HEARTBEAT_HZ)) && (rxchar == 0x0F))
        {
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
            if (rxchar == 0)
            {
                sbusDAV = true;
                sbusCount++;
            }
#else
            sbusDAV = true;
#endif
    }

    U1STAbits.OERR = 0; // clear overflow error flag
    _U1RXIF = 0; // clear the interrupt

    interrupt_restore_corcon;
    return;
}

void parseSbusData()
{
    // MatrixPilot expects PCM values of 3000 +/-1000
    // channel numbers start at 1, not zero

    // S.bus channels 1-16 are 11 bit digital
    // with range 0-2047, center ~1020
    // move center to 3000 by adding 1980 for range [1980, 4028]

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
    if (sbuff[23] & 0x4) sFrameLost++;
    if (sbuff[23] & 0x8) sFailSafe++;

}
#endif





/////////////////////////////////////////////////////////////////////////////////////////
// Serial

boolean pauseSerial = false;

void udb_init_USART(void)
{
    // configure U2MODE
    U2MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
    //						// Bit14
    U2MODEbits.USIDL = 0; // Bit13 Continue in Idle
    U2MODEbits.IREN = 0; // Bit12 No IR translation
    U2MODEbits.RTSMD = 0; // Bit11 Simplex Mode
    //						// Bit10
    U2MODEbits.UEN = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
    U2MODEbits.WAKE = 0; // Bit7 No Wake up (since we don't sleep here)
    U2MODEbits.LPBACK = 0; // Bit6 No Loop Back
    U2MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')
    U2MODEbits.URXINV = 0; // Bit4 IdleState = 1  (for dsPIC)
    //	U2MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
    U2MODEbits.BRGH = 1; // Bit3 4 clocks per bit period
    U2MODEbits.PDSEL = 0; // Bits1,2 8bit, No Parity
    U2MODEbits.STSEL = 0; // Bit0 One Stop Bit

    // Load all values in for U1STA SFR
    U2STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U2STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
    U2STAbits.UTXISEL0 = 1; //Bit13 Other half of Bit15
    //				 		//Bit12
    U2STAbits.UTXBRK = 0; //Bit11 Disabled
    //U2STAbits.UTXEN = 1;	//Bit10 TX pins controlled by periph (handled below)
    //U2STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
    //U2STAbits.TRMT = 0;	//Bit8 *Read Only bit*
    U2STAbits.URXISEL = 0; //Bits6,7 Int. on character recieved
    U2STAbits.ADDEN = 0; //Bit5 Address Detect Disabled
    //U2STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
    //U2STAbits.PERR = 0;	//Bit3 *Read Only Bit*
    //U2STAbits.FERR = 0;	//Bit2 *Read Only Bit*
    U2STAbits.OERR = 0; //Bit1 *Read Only Bit*
    //U2STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

    _U2TXIP = 4; // Mid Range Interrupt Priority level, no urgent reason
    _U2RXIP = 4; // Mid Range Interrupt Priority level, no urgent reason

    _U2TXIF = 0; // Clear the Transmit Interrupt Flag
    _U2TXIE = 1; // Enable Transmit Interrupts
    _U2RXIF = 0; // Clear the Recieve Interrupt Flag
    _U2RXIE = 1; // Enable Recieve Interrupts

    U2MODEbits.UARTEN = 1; // And turn the peripheral on

    U2STAbits.UTXEN = 1;

    return;
}

void udb_serial_set_rate(long rate)
{
    U2BRG = UDB_BAUD(rate);
    return;
}

boolean udb_serial_check_rate(long rate)
{
    return ( U2BRG == UDB_BAUD(rate));
}

void udb_serial_start_sending_data(void)
{
    _U2TXIF = 1; // fire the tx interrupt
    return;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void)
{
    indicate_loading_inter;
    interrupt_save_set_corcon;

    _U2TXIF = 0; // clear the interrupt

    int txchar = udb_serial_callback_get_byte_to_send();

    if (txchar != -1)
    {
        U2TXREG = (unsigned char) txchar;
    }

    interrupt_restore_corcon;
    return;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U2RXInterrupt(void)
{
    indicate_loading_inter;
    interrupt_save_set_corcon;

    while (U2STAbits.URXDA)
    {
        unsigned char rxchar = U2RXREG;
        udb_serial_callback_received_byte(rxchar);
    }

    U2STAbits.OERR = 0;

    _U2RXIF = 0; // clear the interrupt

    interrupt_restore_corcon;
    return;
}


#endif
