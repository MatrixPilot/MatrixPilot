//

#include "options.h"
#include "libUDB_defines.h"

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
//
// Based on code from Microchip AppNote1100, by Martin Bowman.
//    http://ww1.microchip.com/downloads/en/AppNotes/AN1100.zip

#if (BOARD_TYPE & AUAV2_BOARD)

#include "libUDB_internal.h"

// eeprom is 24AA64 8Kx8 bits in SOT23 package, no address pins
// It shares the I2C1 bus with an HMC5883 (address 0x3C) and LPS331 (address 0xB8)

// eeprom address with read asserted
#define EEPROM_R (0xA1)
// eeprom address with write asserted
#define EEPROM_W (0xA0)

#define I2C1BRGVAL ( (int)(((1/100e3) - 130E-9) * FREQOSC / CLK_PHASES)-2 ) // 100 Khz


/** V A R I A B L E S **********************************************/
//unsigned char eeprom_control = CONTROLBYTE ;	// Control byte variable

#ifdef DEBUG_EEPROM

void test_eeprom(unsigned char dataByte) {
    unsigned char odata[32], status;
    unsigned int i;
    for (i = 0x0; i < 0x20; i++) {
        odata[i] = i;
        //        status = eeprom_ByteWrite(i, odata[i]);
        //        if (status) LED_RED = LED_ON;
        //        else LED_RED = LED_OFF;
    }
    eeprom_PageWrite(0x400, odata, 0x20);
    __delay_ms(10);

    unsigned char data = 0;
    unsigned int address = 0x400;
    while (true) {
//        status = eeprom_ByteRead(address++, &data);
//        if (address >= 0x420) address = 0x400;
        status = eeprom_SequentialRead(address, &data, 32);
        if (status) LED_RED = LED_ON;
        else LED_RED = LED_OFF;
        __delay_ms(5);
    }
}
#endif

void udb_eeprom_init(void) {
    // enabling an I2C module causes it to take control of its SCL and SDA pins
    // "The module overrides port state and direction"

    // set baud rate generator speed
    I2C1BRG = I2C1BRGVAL; // 100KHz at 40mips
    _I2CEN = 1; // enable I2C1 module

    _MI2C1IP = 5; // I2C interrupt at priority 5
    _MI2C1IF = 0; // clear the I2C master interrupt
    _MI2C1IE = 0; // disable the interrupt since all methods block till completion

    return;
}



// Function Prototypes
void bstart(void); // Start condition
void bstop(void); // Stop condition
unsigned char byte_out(unsigned char); // Byte output
unsigned char byte_in(unsigned char*, unsigned char); // Byte input
void ACK_Poll(void); // Acknowledge polling

/********************************************************************
 * Function:        void bstart(void)
 *
 * Description:     This function generates an I2C Start condition.
 *******************************************************************/
void bstart(void) {
    // assert send
    I2C1CONbits.SEN = 1;
    // poll for completion
    while (I2C1CONbits.SEN) {
    };
}

/********************************************************************
 * Function:        void bstop(void)
 *
 * Description:     This function generates an I2C Stop condition.
 *******************************************************************/
void bstop(void) {
    // assert stop
    I2C1CONbits.PEN = 1;
    // poll for completion
    while (I2C1CONbits.PEN) {
    };
}

/********************************************************************
 * Function:        unsigned char byte_out(unsigned char data)
 *
 * Description:     This function outputs a byte to the I2C bus.
 *                  It also receives the ACK bit and returns 0 if
 *                  successfully received, or 1 if not.
 *******************************************************************/
unsigned char byte_out(unsigned char data) {
    // just write a byte to the output register
    I2C1TRN = data;
    // poll for completion
    while (I2C1STATbits.TBF) {}
    while (I2C1STATbits.TRSTAT) {}
    //    int count = 0;
    //    while (count++ < 400 && (I2C1STATbits.TBF || I2C1STATbits.TRSTAT)) {}
    // get ACK
    if (I2C1STATbits.ACKSTAT)
        return 1;
    else
        return 0;
}

// poll for flag bit set with timeout at maxTime in polling cycles

unsigned int poll_set(volatile unsigned int* flag, int mask, unsigned int maxTime) {
    while (((*flag & mask) == 0) && maxTime > 0) {
        maxTime--;
    }
    return maxTime;
}

// poll for flag bit clear with timeout at maxTime in polling cycles

unsigned int poll_clr(volatile unsigned int* flag, int mask, unsigned int maxTime) {
    while (((*flag & mask) != 0) && maxTime > 0) {
        maxTime--;
    }
    return maxTime;
}

/********************************************************************
 * Function:        unsigned char byte_in(unsigned char* data, unsigned char ack)
 *
 * Description:     This function inputs a byte from the I2C bus.
 *                  Depending on the value of ack, it will also
 *                  transmit either an ACK or a NAK bit.
 * return status: non-zero indicates success and remaining time, zero indicates timeout
 *******************************************************************/
unsigned char byte_in(unsigned char* data, unsigned char ack) {
    unsigned int remTime;

    I2C1CONbits.RCEN = 1; // start read
    // wait for RBF
    //    while (I2C1STATbits.RBF == 0) {}  // this can hang
    remTime = poll_set(&I2C1STAT, 0x02, 1000);
    if (remTime) {
        *data = I2CRCV;
        I2C1CONbits.ACKDT = ack; // send ACK/NACK
        I2C1CONbits.ACKEN = 1;
        // wait for ACKEN clear
        //        while (I2C1CONbits.ACKEN) {};  // this can hang
        remTime = poll_clr(&I2C1CON, 0X10, 1000);
    }
    return (remTime > 0);
}
/********************************************************************
 * Function:        void ACK_Poll(void)
 *
 * Description:     This function implements Acknowledge polling.
 *                  while EEPROM writes are in progress, control bytes are not acknowledged
 *******************************************************************/
void ACK_Poll(void) {
    unsigned char result; // Polling result

    //    __delay_ms(5);
    do {
        bstart(); // Generate Start condition
        result = byte_out(EEPROM_W); // Output control byte
        bstop(); // Generate Stop condition
    } while (result == 1);
}



// Below are the eeprom functions exported as part of libUDB

// returns true for success

unsigned char eeprom_ByteWrite(unsigned int address, unsigned char data) {
    unsigned char status;

    bstart();
    status = byte_out(EEPROM_W);
    if (status == 0) {
        byte_out(address >> 8);
        byte_out(address);
        //        byte_out(data);
        status = byte_out(data);
    }
    bstop();
    if (status == 0) ACK_Poll(); // wait for write to complete
    if (status)
        return false;
    else
        return true;
}


// returns true for success
// numbytes must be <= 32 and page boundaries must not be crossed
// This means numbytes <= (32 - address % 32): e.g. address = 32, numbytes <= 32
//FIXME: not tested

unsigned char eeprom_PageWrite(unsigned int address, unsigned char *data, unsigned char numbytes) {
    unsigned int i; // Loop counter
    unsigned char status;

    // max allowed numbytes
    i = 32 - (address & 0x1F);
    if (numbytes > i) numbytes = i;

    bstart(); // Generate Start condition
    status = byte_out(EEPROM_W);
    if (status == 0) {
        byte_out(address >> 8);
        byte_out(address);
        for (i = 0; i < numbytes; i++) // Loop through data bytes
            status = byte_out(data[i]); // Output next data byte
    }
    bstop(); // Generate Stop condition
    //TODO: poll for write completion hangs on AUAV2 (when no breakpoint on this line)
    if (status == 0) ACK_Poll(); // wait for write to complete
    if (status)
        return false;
    else
        return true;
}

// returns true for success

unsigned char eeprom_ByteRead(unsigned int address, unsigned char *data) {
    unsigned char status;

    bstart();
    status = byte_out(EEPROM_W);
    status = byte_out(address >> 8);
    status = byte_out(address);

    if (status == 0) {
        I2C1CONbits.RSEN = 1; // generate Repeated Start sequence
        // wait for RSEN to clear
        poll_clr(&I2C1CON, 0x02, 1000);
        //        while (I2C1CONbits.RSEN) {}; // this could hang
        status = byte_out(EEPROM_R);
        status = byte_in(data, 1); // receive byte and send NACK
    }
    bstop();
    if (status)
        return false;
    else
        return true;
}


// returns true for success

unsigned char eeprom_SequentialRead(unsigned int address, unsigned char *data, unsigned int numbytes) {
    unsigned char status;
    unsigned int i;

    bstart(); // Generate Start condition
    status = byte_out(EEPROM_W); // Output control byte
    status = byte_out(address >> 8);
    status = byte_out(address);
    if (status == 0) {
        I2C1CONbits.RSEN = 1; // generate Repeated Start sequence
        // wait for RSEN to clear
        poll_clr(&I2C1CON, 0x02, 1000);
        byte_out(EEPROM_R); // Output control byte + read
        for (i = 0; i < numbytes - 1; i++) // read first N-1 data bytes
            byte_in(&(data[i]), 0); // and ACK

        status = byte_in(&(data[i]), 1); // read last byte with NAK
    }
    bstop(); // Generate Stop condition
    return status;
}

#endif // BOARD_TYPE
