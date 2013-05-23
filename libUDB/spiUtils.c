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
#include "libUDB.h"
#include "spiUtils.h"
#include "interrupt.h"
#include "oscillator.h"

#if (BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD)

#include <libpic30.h>
#include <stdbool.h>
#include <spi.h>

void initSPI1_master16(uint16_t priPre, uint16_t secPre) {
    /* Holds the information about SPI configuration */
    uint16_t SPICON1Value, SPICON2Value;
    /* Holds the information about SPI Enable/Disable */
    uint16_t SPISTATValue;

    /* Turn off SPI module */
    CloseSPI1();

    /* Configure SPI1 interrupt */
    ConfigIntSPI1(SPI_INT_DIS & SPI_INT_PRI_6);

    /* Configure SPI1 module in master mode  */
#if defined(__dsPIC33E__)
    SPICON1Value =
            ENABLE_SDO_PIN & SPI_MODE16_ON & ENABLE_SCK_PIN &
            SPI_SMP_OFF & SPI_CKE_OFF &
            SLAVE_ENABLE_OFF &
            CLK_POL_ACTIVE_LOW &
            MASTER_ENABLE_ON &
            secPre &
            priPre;
    SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT; // & FIFO_BUFFER_DISABLE;
    SPISTATValue = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR & BUF_INT_SEL_5;
#else
    SPICON1Value =
            ENABLE_SDO_PIN & SPI_MODE16_ON & ENABLE_SCK_PIN &
            SPI_SMP_ON & SPI_CKE_OFF &
            SLAVE_ENABLE_OFF &
            CLK_POL_ACTIVE_LOW &
            MASTER_ENABLE_ON &
            secPre &
            priPre;
    SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT;
    SPISTATValue = SPI_ENABLE & SPI_IDLE_CON &
            SPI_RX_OVFLOW_CLR;
#endif

    OpenSPI1(SPICON1Value, SPICON2Value, SPISTATValue);

    SPI1STATbits.SPIROV = 0; // Clear SPI1 receive overflow

    _SPI1IF = 0; // clear any pending interrupts
    _SPI1IP = 6; // priority 6
	_SPI1IE = 1 ;	// turn on SPI1 interrupts

}

// blocking 16 bit write to SPI1

void writeSPI1reg16(uint16_t addr, uint16_t data) {
    int16_t k;
    // assert chip select
    SPI1_SS = 0;

    // send address and data
    k = SPI1BUF;
    SPI1BUF = addr << 8 | data;

    // wait for write
    __delay_us(32);  // allow 16 cycles at 500KHz

    k = SPI1BUF;    // dump received data

    // deassert chip select
    SPI1_SS = 1;

    // this delay is necessary; it appears that SS must be deasserted for one or
    // more SPI clock cycles between writes
    __delay_us(1);
}

void no_call_back(void) {
    return;
}

// Global control block shared by SPI1 routines
uint16_t * SPI1_data;
uint8_t SPI1_high, SPI1_low;
int16_t SPI1_i, SPI1_j, SPI1_n;

void (* SPI1_read_call_back) (void) = &no_call_back;

#if defined(__dsPIC33E__)
// SPI module has 8 word FIFOs
// burst read 2n bytes starting at addr;
// Since first byte is address, max of 15 data bytes may be transferred with n=7

void readSPI1_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (* call_back)(void)) {
    uint16_t i;
    // assert chip select
    SPI1_SS = 0;
    // store the address of the call back routine
    SPI1_read_call_back = call_back;

    // store address of data buffer
    SPI1_data = &data[0];

    // empty read buffer
    i = SPI1BUF;

    // write address-1 in high byte + n-1 dummy words to TX FIFO
    addr |= 0x80;
    SPI1BUF = addr << 8; // issue read command

    for (i = 0; i < n; i++) {
        SPI1BUF = 0;
    }
    _SPI1IE = 1; // turn on SPI1 interrupts
    return;
}

// this ISR empties the RX FIFO into the SPI1_data buffer
// no possibility of overrun if buffer length is at least 8 words

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1Interrupt(void) {
    uint16_t SPIBUF;
    // clear interrupt flag as soon as possible so as to not miss any interrupts
    _SPI1IF = 0;

    indicate_loading_inter;
    interrupt_save_set_corcon;

    _SPI1IE = 0; // turn off SPI1 interrupts

    // get first byte from first word
    SPIBUF = SPI1BUF;
    SPI1_high = 0xFF & SPIBUF;

    // empty the FIFO
    while (!SPI1STATbits.SRXMPT) {
        SPIBUF = SPI1BUF;
        SPI1_low = SPIBUF >> 8;
        *SPI1_data++ = SPI1_high << 8 | SPI1_low;
        SPI1_high = 0xFF & SPIBUF;
    }
    SPI1_SS = 1;
    (* SPI1_read_call_back) (); // execute the call back

    interrupt_restore_corcon;
    return;
}

#else
// no SPI FIFO
// burst read 2n bytes starting at addr

void readSPI1_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (* call_back)(void)) {
    uint16_t SPIBUF;
    // assert chip select
    SPI1_SS = 0;
    // store the address of the call back routine
    SPI1_read_call_back = call_back;
    // initialize indices
    SPI1_i = 0;
    SPI1_j = 0;
    SPI1_n = n;
    // store address of data buffer
    SPI1_data = &data[0];
    addr |= 0x80;

    SPIBUF = SPI1BUF;
    SPI1BUF = addr << 8; // issue read command
    _SPI1IE = 1; // turn on SPI1 interrupts
    return;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1Interrupt(void) {
    uint16_t SPIBUF;
    // clear interrupt flag as soon as possible so as to not miss any interrupts
    _SPI1IF = 0;
    _SPI1IE = 0; // turn off SPI1 interrupts

    indicate_loading_inter;
    interrupt_save_set_corcon;

    if (SPI1_i == 0) {
        SPIBUF = SPI1BUF;
        SPI1BUF = 0x0000;
        SPI1_high = 0xFF & SPIBUF;
        SPI1_i = 1;
    } else if (SPI1_i < SPI1_n) {
        SPIBUF = SPI1BUF;
        SPI1BUF = 0x0000;
        SPI1_low = SPIBUF >> 8;
        * (SPI1_data + SPI1_j) = SPI1_high << 8 | SPI1_low;
        SPI1_high = 0xFF & SPIBUF;
        SPI1_i++;
        SPI1_j++;
    } else {
        SPIBUF = SPI1BUF;
        SPI1_low = SPIBUF >> 8;
        * (SPI1_data + SPI1_j) = SPI1_high << 8 | SPI1_low;
        SPI1_SS = 1;
        (* SPI1_read_call_back) (); // execute the call back
    }
    interrupt_restore_corcon;
    return;
}
#endif

#if 0
// experimental blocking 8 bit read for dsPIC33EP
// FIXME: why doesn't this work? read FIFO is all zeros even though non-zero data is observed on MISO

uint8_t readSPI1reg16(uint16_t addr) {
    int16_t k, data[8];
    // clear receive FIFO
//    while (SPI1STATbits.SRXMPT == 0) {
//        data[k] = SPI1BUF;
//    }
    k = SPI1STAT;
    SPI1STATbits.SPIROV = 0;
    // assert chip select
    SPI1_SS = 0;

    addr |= 0x80;
    SPI1BUF = addr << 8; // issue read command

    while (SPI1STATbits.SPIBEC);    // wait for TX FIFO to empty
    while (!SPI1STATbits.SRMPT);    // wait for last transfer to complete
    SPI1_SS = 1;
    __delay_us(20);
    SPI1_SS = 0;

    SPI1BUF = addr << 8; // issue read command

    while (SPI1STATbits.SPIBEC);    // wait for TX FIFO to empty
    while (!SPI1STATbits.SRMPT);    // wait for last transfer to complete

    data[0] = SPI1BUF;
    data[1] = SPI1BUF;

//    for (k = 0; k < 8; k++) {
//        // read one word from FIFO
//        data[k] = SPI1BUF;
//    }

    // deassert chip select for a while
    SPI1_SS = 1;
    __delay_us(40);

    return 0xFF & data[0];
}
#endif

void initSPI2_master16(uint16_t priPre, uint16_t secPre) {
    /* Holds the information about SPI configuration */
    uint16_t SPICON1Value, SPICON2Value;
    /* Holds the information about SPI Enable/Disable */
    uint16_t SPISTATValue;

    /* Turn off SPI module */
    CloseSPI2();

    /* Configure SPI2 interrupt */
    ConfigIntSPI2(SPI_INT_DIS & SPI_INT_PRI_6);

    /* Configure SPI2 module in master mode  */
    SPICON1Value =
            ENABLE_SDO_PIN & SPI_MODE16_ON & ENABLE_SCK_PIN &
            SPI_SMP_ON & SPI_CKE_OFF &
            SLAVE_ENABLE_OFF &
            CLK_POL_ACTIVE_LOW &
            MASTER_ENABLE_ON &
            secPre &
            priPre;
    SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT;
    SPISTATValue = SPI_ENABLE & SPI_IDLE_CON &
            SPI_RX_OVFLOW_CLR;
    OpenSPI2(SPICON1Value, SPICON2Value, SPISTATValue);

    SPI2STATbits.SPIROV = 0; // Clear SPI2 receive overflow

    _SPI2IF = 0; // clear any pending interrupts
    _SPI2IP = 6; // priority 6
    _SPI2IE = 1; // turn on SPI2 interrupts

}

// blocking 16 bit write to SPI2

void writeSPI2reg16(uint16_t addr, uint16_t data) {
    int16_t k;
    // assert chip select
    SPI2_SS = 0;

    // send address and data
    k = SPI2BUF;
    SPI2BUF = addr << 8 | data;

    // wait for write
    //    while (!SPI2STATbits.SPIRBF);
    for (k = 0; k < 200; k++)
        if (SPI2STATbits.SPIRBF) break;

    k = SPI2BUF;

    // deassert chip select
    SPI2_SS = 1;

    // this delay is necessary; it appears that SS must be deasserted for one or
    // more SPI clock cycles between writes
    __delay_us(1);
}

// Global control block shared by SPI2 routines
uint16_t * SPI2_data;
uint8_t SPI2_high, SPI2_low;
int16_t SPI2_i, SPI2_j, SPI2_n;

void (* SPI2_read_call_back) (void) = &no_call_back;

// burst read 2n bytes starting at addr

void readSPI2_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (* call_back)(void)) {
    uint16_t SPIBUF;

    // assert chip select
    SPI2_SS = 0;
    // save address of call back routine
    SPI2_read_call_back = call_back;
    // initialize indices
    SPI2_i = 0;
    SPI2_j = 0;
    SPI2_n = n;
    // save address of data buffer
    SPI2_data = &data[0];
    addr |= 0x80;

    SPIBUF = SPI2BUF;
    SPI2BUF = addr << 8; // issue read command
    return;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI2Interrupt(void) {
    uint16_t SPIBUF;
    // clear the interrupt flag as soon as possible so as to not miss any interrupts
    _SPI2IF = 0;

    indicate_loading_inter;
    interrupt_save_set_corcon;

    if (SPI2_i == 0) {
        SPIBUF = SPI2BUF;
        SPI2BUF = 0x0000;
        SPI2_high = 0xFF & SPIBUF;
        SPI2_i = 1;
    } else if (SPI2_i < SPI2_n) {
        SPIBUF = SPI2BUF;
        SPI2BUF = 0x0000;
        SPI2_low = SPIBUF >> 8;
        * (SPI2_data + SPI2_j) = SPI2_high << 8 | SPI2_low;
        SPI2_high = 0xFF & SPIBUF;
        SPI2_i++;
        SPI2_j++;
    } else {
        SPIBUF = SPI2BUF;
        SPI2_low = SPIBUF >> 8;
        * (SPI2_data + SPI2_j) = SPI2_high << 8 | SPI2_low;
        SPI2_SS = 1;
        (* SPI2_read_call_back) (); // execute the call back
    }
    interrupt_restore_corcon;
    return;
}

#endif // BOARD_TYPE
