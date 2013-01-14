#include <stdint.h>          /* For uint32_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include <spi.h>

#include "libUDB_internal.h"
#include "spiUtils.h"

#define LED_ON		0
#define LED_OFF		1

void initSPI1_master16(unsigned int priPre, unsigned int secPre) {
    /* Holds the information about SPI configuration */
    unsigned int SPICON1Value, SPICON2Value;
    /* Holds the information about SPI Enable/Disable */
    unsigned int SPISTATValue;

    /* Turn off SPI module */
    CloseSPI1();

    /* Configure SPI1 interrupt */
    ConfigIntSPI1(SPI_INT_DIS & SPI_INT_PRI_6);

    /* Configure SPI1 module in master mode  */
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
    OpenSPI1(SPICON1Value, SPICON2Value, SPISTATValue);

    SPI1STATbits.SPIROV = 0; // Clear SPI1 receive overflow
}

// blocking 8 bit write to MPU6000 register

void writeSPI1reg16(unsigned int addr, unsigned int data) {
    int k;
    // assert chip select
    SPI1_SS = 0;

        // send address and data
        k = SPI1BUF;
        SPI1BUF = addr << 8 | data;

        // wait for write
        //    while (!SPI1STATbits.SPIRBF);
        for (k = 0; k < 200; k++)
            if (SPI1STATbits.SPIRBF) break;

        k = SPI1BUF;

    // deassert chip select
    SPI1_SS = 1;

    // this delay is necessary; it appears that SS must be deasserted for one or
    // more SPI clock cycles between writes
    __delay_us(1);
}

// blocking 8 bit read from MPU6000 register

unsigned char readSPI1reg16(unsigned int addr) {
    int k;
    // assert chip select
    SPI1_SS = 0;

    addr |= 0x80;
    k = SPI1BUF;
    //    WriteSPI1(addr); // issue read command
    SPI1BUF = addr << 8; // issue read command

    // wait for address write and data read
    //    while (!SPI1STATbits.SPIRBF);
    for (k = 0; k < 200; k++)
        if (SPI1STATbits.SPIRBF) break;

    // deassert chip select
    SPI1_SS = 1;

    return 0xFF & SPI1BUF;
}
// burst read 2n bytes starting at addr

void readSPI1_burst16n(unsigned int data[], int n, unsigned int addr) {
    uint8_t high, low;
    int i, j, k = 0;
    // assert chip select
    SPI1_SS = 0;

    addr |= 0x80;
    i = SPI1BUF;
    SPI1BUF = addr << 8; // issue read command

    // wait for address write and data read
    // assuming 10MHz SPI clock and 1.6usec per 16 bit transfer,
    // 1.25MHz => 12.8usec per 16 bit transfer
    // timeout after 40 (20) polling cycles or ~5 (2.5) usec
    //    while (!SPI1STATbits.SPIRBF);
    for (k = 0; k < 40; k++)
        if (SPI1STATbits.SPIRBF) break;

    high = 0xFF & SPI1BUF; // high byte of first reg. is in low byte of buf

    for (i = 0, j = 0; i < n; i++) {
        SPI1BUF = 0x0000; // generate clock pulses for next 2 bytes

        // wait for data read
        for (k = 0; k < 20; k++)
            if (SPI1STATbits.SPIRBF) break;

        low = SPI1BUF >> 8;
        
        data[j++] = high << 8 | low;
        high = 0xFF & SPI1BUF;
    }

    // deassert chip select
    SPI1_SS = 1;
}

