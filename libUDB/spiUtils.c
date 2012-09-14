#include <stdint.h>          /* For uint32_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include <spi.h>

#include "libUDB_internal.h"
#include "spiUtils.h"
//#include "ConfigUDB4.h"

#define LED_ON		0
#define LED_OFF		1

void initSPI1_master16(unsigned int priPre, unsigned int secPre)
{
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

void writeSPI1reg16(unsigned int addr, unsigned int data)
{
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

    // TODO: this delay is necessary for MPU6000_init16 to work; why?
    __delay_us(100);
}

// blocking 8 bit read from MPU6000 register

unsigned char readSPI1reg16(unsigned int addr)
{
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

void readSPI1_burst16n(unsigned int data[], int n, unsigned int addr)
{
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

    for (i = 0, j = 0; i < n; i++)
    {
        SPI1BUF = 0x0000; // generate clock pulses for next 2 bytes

        // wait for TXBUF doesn't work
        //        while (!SPI1STATbits.SPITBF);
        //        while (SPI1STATbits.SPITBF);
#if BOARD_TYPE == AUAV2_BOARD_ALPHA1
        // this results in no clock cycles between words at 8MHz
        // (hangs if k init'ed less than 6)
        for (k = 6; k > 0; k--)
            asm("clrwdt");

#elif BOARD_TYPE == UDB4_BOARD
        // this results in only one missed clock cycle between words at 10MHz
        for (k = 4; k > 0; k--)
            asm("clrwdt");
#endif

        SPI1BUF = 0x0000; // generate clock pulses for next 2 bytes

        // wait for data read
        //        while (!SPI1STATbits.SPIRBF);
        for (k = 0; k < 20; k++)
            if (SPI1STATbits.SPIRBF) break;
        low = SPI1BUF >> 8;
        data[j++] = high << 8 | low;
        high = 0xFF & SPI1BUF;
    }

    // deassert chip select
    SPI1_SS = 1;
}

#if 0
// Data received at SPI1,2
unsigned int spi1Data, spi2Data;

// flags set by ISRs
bool spi1DAV = false, spi2DAV = false;

void __attribute__((__interrupt__, no_auto_psv)) _SPI1Interrupt(void)
{
    IFS0bits.SPI1IF = 0;
    spi1Data = SPI1BUF;
    spi1DAV = true;
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI2Interrupt(void)
{
    IFS2bits.SPI2IF = 0;
    spi2Data = SPI1BUF;

    // stop timer1
    T1CON = 0;
    spi2DAV = true;
}

void initSPI2_slave(unsigned int priPre, unsigned int secPre)
{
    /* Holds the information about SPI configuartion */
    unsigned int SPICON1Value, SPICON2Value;
    /* Holds the information about SPI Enable/Disable */
    unsigned int SPISTATValue;

    /* Turn off SPI module */
    CloseSPI2();

    /* Configure SPI2 interrupt */
    ConfigIntSPI2(SPI_INT_EN & SPI_INT_PRI_6);

    /* Configure SPI2 module in slave mode */
    SPICON1Value =
            ENABLE_SDO_PIN & SPI_MODE16_OFF & ENABLE_SCK_PIN &
            SPI_SMP_OFF & SPI_CKE_OFF &
            SLAVE_ENABLE_ON &
            CLK_POL_ACTIVE_LOW &
            MASTER_ENABLE_OFF &
            secPre &
            priPre;
    SPISTATValue = SPI_ENABLE & SPI_IDLE_CON &
            SPI_RX_OVFLOW_CLR;
    OpenSPI2(SPICON1Value, SPICON2Value, SPISTATValue);

    SPI2STATbits.SPIROV = 0; // Clear SPI2 receive overflow

}

void initSPI1_master(unsigned int priPre, unsigned int secPre)
{
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
            ENABLE_SDO_PIN & SPI_MODE16_OFF & ENABLE_SCK_PIN &
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

void initSPI2_slave16(unsigned int priPre, unsigned int secPre)
{
    /* Holds the information about SPI configuartion */
    unsigned int SPICON1Value, SPICON2Value;
    /* Holds the information about SPI Enable/Disable */
    unsigned int SPISTATValue;

    /* Turn off SPI module */
    CloseSPI2();

    /* Configure SPI2 interrupt */
    ConfigIntSPI2(SPI_INT_EN & SPI_INT_PRI_6);

    /* Configure SPI2 module in slave mode */
    SPICON1Value =
            ENABLE_SDO_PIN & SPI_MODE16_ON & ENABLE_SCK_PIN &
            SPI_SMP_OFF & SPI_CKE_OFF &
            SLAVE_ENABLE_ON &
            CLK_POL_ACTIVE_LOW &
            MASTER_ENABLE_OFF &
            secPre &
            priPre;
    SPISTATValue = SPI_ENABLE & SPI_IDLE_CON &
            SPI_RX_OVFLOW_CLR;
    OpenSPI2(SPICON1Value, SPICON2Value, SPISTATValue);

    SPI2STATbits.SPIROV = 0; // Clear SPI2 receive overflow

}

// blocking write to MPU6000 register

void writeSPI1reg(unsigned int addr, unsigned int mpu_data)
{
    int dump;
    // assert chip select
    SPI1_SS = 0;

    // send address
    dump = SPI1BUF;
    //    WriteSPI1(addr);
    SPI1BUF = addr;

    // FIXME: this should preload the TXBUF, but doesn't work at all
    // workaround suggested by Microchip
    //    while (!_SPITBF); // just hangs
    // wait for !TBF and stage next write
    //    while (_SPITBF);

    // this kludge is SCLK rate dependent
    //    int timeout = 0;
    //    while (timeout < 7)
    //    {
    //        timeout++;
    //    }
    //    // load data into TXBUF
    //    SPI1BUF = data;

    // wait for address write
    while (!SPI1STATbits.SPIRBF);
    dump = SPI1BUF;

    // load data into TXBUF
    //    WriteSPI1(data);
    SPI1BUF = mpu_data;

    // wait for data write
    while (!SPI1STATbits.SPIRBF);
    dump = SPI1BUF;

    // deassert chip select
    SPI1_SS = 1;
}

// blocking read from MPU6000 register

unsigned int readSPI1reg(unsigned int addr)
{
    int dump;
    // assert chip select
    SPI1_SS = 0;

    addr |= 0x80;
    dump = SPI1BUF;
    //    WriteSPI1(addr); // issue read command
    SPI1BUF = addr; // issue read command

    // wait for address write
    while (!SPI1STATbits.SPIRBF);
    dump = SPI1BUF;
    //    WriteSPI1(0x00); // generate clock pulses for response
    SPI1BUF = 0x00; // generate clock pulses for response

    // wait for data read
    while (!SPI1STATbits.SPIRBF);

    // deassert chip select
    SPI1_SS = 1;

    return SPI1BUF;
}

// burst read

unsigned int readSPI1_burst16()
{
    int dump;
    // assert chip select
    SPI1_SS = 0;

    dump = SPI1BUF;
    SPI1BUF = 0x0000; // generate clock pulses for response

    // wait for TXBUF
    while (!SPI1STATbits.SPITBF);
    SPI1BUF = 0x0000; // generate clock pulses for response

    // wait for data read
    while (!SPI1STATbits.SPIRBF);

    // deassert chip select
    //    SPI1_SS = 1;

    return SPI1BUF;
}

unsigned int readSPI1_burst()
{
    int dump;
    // assert chip select
    SPI1_SS = 0;

    dump = SPI1BUF; // avoid ROF
    //    WriteSPI1(0x00); // generate clock pulses for response
    SPI1BUF = 0x00; // generate clock pulses for response

    // wait for data read
    while (!SPI1STATbits.SPIRBF);

    // deassert chip select
    SPI1_SS = 1;

    return SPI1BUF;
}


#endif

