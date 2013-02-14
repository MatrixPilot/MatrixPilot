// This file is only for the alpha1 version of AUAV2
// It replaces serialIO_udb4.c
//
// MatrixPilot expects the GPS on UART1 and telemetry on UART2
// udb_init_GPS init's U1 and udb_init_USART init's U2
// But AUAV2_ALPHA2 has the GPS connector on U2
// Instead of udb_init_GPS, we have udb_init_UART(GPS_UART) with
// #define GPS_UART 1 or #define GPS_UART 2


#include <stdbool.h>
#include "libUDB_internal.h"
#include "debug.h"

#if (BOARD_TYPE == AUAV2_BOARD)

/////////////////////////////////////////////////////////////////////////////////////////
// Serial

//TODO: unimplemented
//#define SBUS_UART 1

#define SERIAL_UART 1
#define GPS_UART 2

volatile UART *pgps_uart = NULL;
volatile UART *pserial_uart = NULL;
volatile UART *psbus_uart = NULL;

void (*pgps_startTX)(void) = NULL;
void (*pserial_startTX)(void) = NULL;

int (*U1txCallback)(void) = NULL;
int (*U2txCallback)(void) = NULL;
void (*U1rxCallback)(char) = NULL;
void (*U2rxCallback)(char) = NULL;

void gps_rxCallback(char rxchar);
void sbus_rxCallback(char rxchar);

void uart1_fire_txi(void) {
    _U1TXIF = 1; // fire the tx interrupt
}

void uart2_fire_txi(void) {
    _U2TXIF = 1; // fire the tx interrupt
}

int sbus_txCallback(void) {
    return -1;
}

void udb_init_UART1(bool highSpeed, long baud,
        void (*rx_callback)(char), int (*tx_callback)(void),
        int urxinv, int parity,
        int ipl, bool rxie, bool txie) {
    U1rxCallback = rx_callback;
    U1txCallback = tx_callback;

    // disable module and set configuration bits
    // configure mode register for 8 data bits, no parity, 1 stop bit, no flow control
    U1MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
    U1MODEbits.USIDL = 0; // Bit13 Continue in Idle
    U1MODEbits.IREN = 0; // Bit12 No IR translation
    U1MODEbits.RTSMD = 0; // Bit11 Simplex Mode
    U1MODEbits.UEN = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
    U1MODEbits.WAKE = 0; // Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0; // Bit6 No Loop Back
    U1MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')
    U1MODEbits.URXINV = urxinv; // Bit4 0->IdleState=1, 1->Idlestate=0
    U1MODEbits.PDSEL = parity; // Bits1,2_ 0:8N, 1:8E, 2:8O, 3:9N
    U1MODEbits.STSEL = 0; // Bit0 One Stop Bit

    // configure status register
    U1STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U1STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
    U1STAbits.UTXISEL0 = 1; //Bit13 Other half of Bit15
    U1STAbits.UTXBRK = 0; //Bit11 Disabled
    U1STAbits.URXISEL = 0; //Bits6,7 Int. on character recieved
    U1STAbits.ADDEN = 0; //Bit5 Address Detect Disabled

    // set baud rate
    if (highSpeed) {
        U1MODEbits.BRGH = 1; // Bit3 4 clocks per bit period
        U1BRG = ((int) ((FREQOSC / CLK_PHASES) / ((long) 4 * baud) - 1));
    } else {
        U1MODEbits.BRGH = 0; // Bit3 16 clocks per bit period
        U1BRG = ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * baud) - 1));
    }

    // set IPL and interrupt enables
    _U1TXIP = ipl;
    _U1RXIP = ipl;

    _U1TXIF = 0; // Clear the Transmit Interrupt Flag
    _U1TXIE = txie;
    _U1RXIF = 0; // Clear the Receive Interrupt Flag
    _U1RXIE = rxie;

    // turn the UART on
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
}

void udb_init_UART2(bool highSpeed, long baud, void (*rx_callback)(char), int (*tx_callback)(void),
        int ipl, bool rxie, bool txie) {
    U2rxCallback = rx_callback;
    U2txCallback = tx_callback;

    // disable module and set configuration bits
    // configure mode register for 8 data bits, no parity, 1 stop bit, no flow control
    U2MODEbits.UARTEN = 0; // Bit15 TX, RX DISABLED, ENABLE at end of func
    U2MODEbits.USIDL = 0; // Bit13 Continue in Idle
    U2MODEbits.IREN = 0; // Bit12 No IR translation
    U2MODEbits.RTSMD = 0; // Bit11 Simplex Mode
    U2MODEbits.UEN = 0; // Bits8,9 TX,RX enabled, CTS,RTS not
    U2MODEbits.WAKE = 0; // Bit7 No Wake up (since we don't sleep here)
    U2MODEbits.LPBACK = 0; // Bit6 No Loop Back
    U2MODEbits.ABAUD = 0; // Bit5 No Autobaud (would require sending '55')
    U2MODEbits.URXINV = 0; // Bit4 IdleState = 1  (for dsPIC)
    U2MODEbits.PDSEL = 0; // Bits1,2 8bit, No Parity
    U2MODEbits.STSEL = 0; // Bit0 One Stop Bit

    // configure status register
    U2STAbits.UTXISEL1 = 0; //Bit15 Int when Char is transferred (1/2 config!)
    U2STAbits.UTXINV = 0; //Bit14 N/A, IRDA config
    U2STAbits.UTXISEL0 = 1; //Bit13 Other half of Bit15
    U2STAbits.UTXBRK = 0; //Bit11 Disabled
    U2STAbits.URXISEL = 0; //Bits6,7 Int. on character recieved
    U2STAbits.ADDEN = 0; //Bit5 Address Detect Disabled

    if (highSpeed) {
        U2MODEbits.BRGH = 1; // Bit3 4 clocks per bit period
        U2BRG = ((int) ((FREQOSC / CLK_PHASES) / ((long) 4 * baud) - 1));
    } else {
        U2MODEbits.BRGH = 0; // Bit3 16 clocks per bit period
        U2BRG = ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * baud) - 1));
    }
    _U2TXIP = ipl;
    _U2RXIP = ipl;

    _U2TXIF = 0; // Clear the Transmit Interrupt Flag
    _U2TXIE = txie;
    _U2RXIF = 0; // Clear the Receive Interrupt Flag
    _U2RXIE = rxie;

    U2MODEbits.UARTEN = 1; // And turn the peripheral on
    U2STAbits.UTXEN = 1;
}

////////////////////////////////////////////////////////////////////////////////
//
// GPS
//TODO: untested
void udb_init_GPS(void) {
    if (GPS_UART == 1) {
        pgps_uart = &UART1;
        pgps_startTX = &uart1_fire_txi;
        udb_init_UART1(true, 4800L,
                &udb_gps_callback_received_byte, &udb_gps_callback_get_byte_to_send,
                0, 0,
                4, true, true);
    } else if (GPS_UART == 2) {
        pgps_uart = &UART2;
        pgps_startTX = &uart2_fire_txi;
        udb_init_UART2(true, 4800L,
                &udb_gps_callback_received_byte, &udb_gps_callback_get_byte_to_send,
                4, true, true);
    }
    return;
}

void udb_gps_set_rate(long rate) {
    if (pgps_uart) {
        U1MODEBITS mode = *(U1MODEBITS*) &(pgps_uart->uxmode);
        if (mode.BRGH) // highspeed mode
            pgps_uart->uxbrg = ((int) ((FREQOSC / CLK_PHASES) / ((long) 4 * rate) - 1));
        else // lowspeed mode
            pgps_uart->uxbrg = ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * rate) - 1));
    }
    return;
}

boolean udb_gps_check_rate(long rate) {
    boolean status = false;
    if (pgps_uart) {
        U1MODEBITS mode = *(U1MODEBITS*) &(pgps_uart->uxmode);
        if (mode.BRGH) // highspeed mode
            status = (pgps_uart->uxbrg == ((int) ((FREQOSC / CLK_PHASES) / ((long) 4 * rate) - 1)));
        else // lowspeed mode
            status = (pgps_uart->uxbrg == ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * rate) - 1)));

    }
    return status;
}

void udb_gps_start_sending_data(void) {
    if (pgps_startTX) pgps_startTX();
}

#ifndef USE_DEBUG_U1

void __attribute__((__interrupt__, __no_auto_psv__)) _U1TXInterrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    _U1TXIF = 0; // clear the interrupt

    int txchar = U1txCallback();

    if (txchar != -1) {
        U1TXREG = (unsigned char) txchar;
    }

    interrupt_restore_corcon;
    return;
}

#endif // USE_DEBUG_U1

void udb_init_USART(void) {
    if (SERIAL_UART == 1) {
        pserial_uart = &UART1;
        pserial_startTX = &uart1_fire_txi;
//        udb_init_UART1(true, 4800L,
        udb_init_UART1(true, 115200L,
	            &udb_serial_callback_received_byte, &udb_serial_callback_get_byte_to_send,
//                &udb_gps_callback_received_byte, &udb_gps_callback_get_byte_to_send,
                0, 0,
                4, true, true);
    } else if (SERIAL_UART == 2) {
        pserial_uart = &UART2;
        pserial_startTX = &uart2_fire_txi;
        udb_init_UART2(true, 4800L,
	            &udb_serial_callback_received_byte, &udb_serial_callback_get_byte_to_send,
//                &udb_gps_callback_received_byte, &udb_gps_callback_get_byte_to_send,
                4, true, true);
    }
    return;
}

void udb_serial_set_rate(long rate) {
    if (pserial_uart) {
        U1MODEBITS mode = *(U1MODEBITS*) &(pserial_uart->uxmode);
        if (mode.BRGH) // highspeed mode
            pserial_uart->uxbrg = ((int) ((FREQOSC / CLK_PHASES) / ((long) 4 * rate) - 1));
        else // lowspeed mode
            pserial_uart->uxbrg = ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * rate) - 1));
    }
    return;
}

boolean udb_serial_check_rate(long rate) {
    boolean status = false;
    if (pserial_uart) {
        U1MODEBITS mode = *(U1MODEBITS*) &(pserial_uart->uxmode);
        if (mode.BRGH) // highspeed mode
            status = (pserial_uart->uxbrg == ((int) ((FREQOSC / CLK_PHASES) / ((long) 4 * rate) - 1)));
        else // lowspeed mode
            status = (pserial_uart->uxbrg == ((int) ((FREQOSC / CLK_PHASES) / ((long) 16 * rate) - 1)));

    }
    return status;
}

void udb_serial_start_sending_data(void) {
    if (pserial_startTX) pserial_startTX();
}

#ifndef USE_DEBUG_U2

void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    _U2TXIF = 0; // clear the interrupt

    int txchar = U2txCallback();

    if (txchar != -1) {
        U2TXREG = (unsigned char) txchar;
    }

    interrupt_restore_corcon;
    return;
}

#endif // USE_DEBUG_U2

#if (USE_SBUSDATA == 1)


// Alpha1 must use S.bus inputs
#if BOARD_TYPE == AUAV2_BOARD_ALPHA1
int udb_pwIn[NUM_INPUTS + 1]; // pulse widths of radio inputs
int udb_pwTrim[NUM_INPUTS + 1] = {0, 2200, 3000, 3000, 3000, 3000, 3000, 3000}; // initial pulse widths for trimming
int failSafePulses = 0;

unsigned char sbuff[24];
unsigned int byteIndex = 0;
unsigned int prevSbusFrame = 0;
boolean sbusDAV = false;
#ifdef SBUS_DIAGNOSTICS
unsigned int sbusPerr = 0, sbusFerr = 0, sbusFint = 0, sbusCount = 0;
#endif
unsigned int sFrameLost = 0, sFailSafe = 0;

void udb_init_capture(void) {

    int i;
#if (HARD_TRIMS != 0)
#warning("initial udb_pwTrim values set to NEUTRAL_TRIM and THROTTLE_IDLE")
    for (i = 0; i <= NUM_INPUTS; i++) {
        udb_pwIn[i] = 0;
        udb_pwTrim[i] = NEUTRAL_TRIM;
    }
    udb_pwTrim[THROTTLE_INPUT_CHANNEL] = THROTTLE_IDLE;
#else
    // At the end of the calibration interval, udb_servo_record_trims is called to set
    // udb_pwTrim values to whatever is coming from the receiver at that instant.
    // This will be about 1500usec if the TX trims are centered.

    // trim values of zero are never correct for channels 1-4; init to 1500usec instead
    //FIXME for channels 5-8, trim values are unused in MPQpid
    for (i = 0; i <= NUM_INPUTS; i++) {
        udb_pwIn[i] = 0;
        udb_pwTrim[i] = 3000;
    }
    udb_pwTrim[THROTTLE_INPUT_CHANNEL] = 2000;
#endif

}

// configure UART1 for 100K baud communication with Futaba S.bus protocol

void udb_init_Sbus(void) {
    // S.bus signal is on uart 1, receive only
    psbus_uart = &UART1;
    udb_init_UART1(false, 100000L,
            &sbus_rxCallback, &sbus_txCallback,
            1, 1,
            4, true, false);

    // telemetry out on uart 2 (gps port)
    pserial_uart = &UART2;
    pserial_startTX = &uart2_fire_txi;
    udb_init_UART2(false, TELEMETRY_BAUD,
            &udb_serial_callback_received_byte, &udb_serial_callback_get_byte_to_send,
            4, true, true);
    return;
}

#ifndef USE_DEBUG_U1

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    while (U1STAbits.URXDA) {
        unsigned char rxchar = U1RXREG;
        U1rxCallback(rxchar);
    }

    U1STAbits.OERR = 0; // clear overflow error flag
    _U1RXIF = 0; // clear the interrupt

    interrupt_restore_corcon;
    return;
}

#endif // USE_DEBUG_U1

void sbus_rxCallback(char rxchar) {
#ifdef SBUS_DIAGNOSTICS
    // check for framing and parity errors
    // U1STAbits.FERR bit 2
    // U1STAbits.PERR bit 3
    if (0b1100 & U1STA) {
        // log error
        if (U1STA & 0b0100) sbusFerr++;
        else sbusPerr++;
    } else // no framing or parity error
#endif
    {
        // frames are 22.5 msec apart (Spektrum DX7)
        // Frame starts with sync byte 0x0F and is followed by
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
        udb_flags._.radio_on = 1;
    }
}
#endif // (USE_SBUSDATA == 1)

#endif

#endif // BOARD_TYPE
