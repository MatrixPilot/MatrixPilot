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

	_SPI1IE = 0 ;	// turn off SPI1 interrupts
	_SPI1IP = 6 ;	// priority 6
	_SPI1IF = 0 ;	// clear any pending interrupts

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

unsigned int * SPI_data ;
uint8_t SPI_high, SPI_low ;
int SPI_i, SPI_j , SPI_n ;

// burst read 2n bytes starting at addr

void readSPI1_burst16n(unsigned int data[], int n, unsigned int addr) {
//    uint8_t high, low;
 //   int i, j, k = 0;
	int i ;
	SPI_i = 0 ;
	SPI_j = 0 ;
	SPI_n = n ;
	SPI_data = &data[0] ;
    // assert chip select
    SPI1_SS = 0;

    addr |= 0x80;
    i = SPI1BUF;
	_SPI1IF = 0 ;
	_SPI1IE = 1 ;

    SPI1BUF = addr << 8; // issue read command
	return ;
}

extern bool mpuDAV ;
extern unsigned int mpu_data[];
extern void doT1Interrupt(void);

#if 0
void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1Interrupt(void)
{
	_SPI1IF = 0 ;
	if ( SPI_i++ < SPI_n )
	{
		SPI1BUF = 0x0000 ;
		SPI_high = SPI1BUF ;
	}
	else
	{
		SPI_high = SPI1BUF ;
		_SPI1IE = 0 ;
	}
}
#endif

#if 1
void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1Interrupt(void)
{
//    interrupt_save_set_corcon;
	_SPI1IF = 0 ;
	if ( SPI_i == 0 )
	{
		SPI1BUF = 0x0000 ;
		SPI_high = 0xFF & SPI1BUF;
		SPI_i = 1 ;
	}
	else if ( SPI_i < SPI_n )
	{
		SPI1BUF = 0x0000 ;
		SPI_low = SPI1BUF >> 8 ;
		* ( SPI_data + SPI_j ) = SPI_high << 8 | SPI_low;
		SPI_high = 0xFF & SPI1BUF;
		SPI_i++ ;
		SPI_j++ ;
	}
	else
	{
		SPI_low = SPI1BUF >> 8 ;
		* ( SPI_data + SPI_j ) = SPI_high << 8 | SPI_low;
		SPI1_SS = 1;
		_SPI1IE = 0 ;
   		mpuDAV = true;
    //LED_BLUE = LED_OFF;

#if DUAL_IMU == 1
//    	mpu_xaccel.value = mpu_data[0];
//   	mpu_yaccel.value = mpu_data[1];
//    	mpu_zaccel.value = mpu_data[2];

//    	mpu_xrate.value = mpu_data[4];
//    	mpu_yrate.value = mpu_data[5];
//    	mpu_zrate.value = mpu_data[6];

//    	doT1Interrupt();

#elif (BOARD_TYPE & AUAV2_BOARD)
    // this board has only the MPU-6000
    // filtering is done onboard the MPU-6000, so input field is unused
    	udb_xaccel.value = mpu_data[0];
    	udb_yaccel.value = mpu_data[1];
    	udb_zaccel.value = mpu_data[2];

    	udb_xrate.value = mpu_data[4];
    	udb_yrate.value = mpu_data[5];
    	udb_zrate.value = mpu_data[6];

    	mpu_temp.value = mpu_data[3];

#if (USE_MPU == 1)    // execute heartbeat code
    	doT1Interrupt();
#endif
#endif
//    interrupt_restore_corcon;
	}
	return ;
}

#endif
