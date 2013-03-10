#include <stdint.h>          /* For uint32_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include <spi.h>

#include "libUDB_internal.h"
#include "libUDB.h"
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
    
#if (BOARD_TYPE == AUAV3_BOARD)
    SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT & FIFO_BUFFER_DISABLE;
    SPISTATValue = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR;
#else
    SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT;
    SPISTATValue = SPI_ENABLE & SPI_IDLE_CON &
            SPI_RX_OVFLOW_CLR;
#endif

    OpenSPI1(SPICON1Value, SPICON2Value, SPISTATValue);

    SPI1STATbits.SPIROV = 0; // Clear SPI1 receive overflow

	_SPI1IF = 0 ;	// clear any pending interrupts
	_SPI1IP = 6 ;	// priority 6
	_SPI1IE = 1 ;	// turn on SPI1 interrupts

}

// blocking 16 bit write to SPI1

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

void no_call_back(void)
{
	return ;
}

// Global control block shared by SPI1 routines
unsigned int * SPI1_data ;  
uint8_t SPI1_high, SPI1_low ;
int SPI1_i, SPI1_j , SPI1_n ;

void (* SPI1_read_call_back ) ( void ) = &no_call_back ;

// burst read 2n bytes starting at addr

void readSPI1_burst16n(unsigned int data[], int n, unsigned int addr , void (* call_back )( void)) {
	unsigned int SPIBUF ;
    // assert chip select
    SPI1_SS = 0;
	// store the address of the call back routine
	SPI1_read_call_back = call_back ;
	// initialize indices
	SPI1_i = 0 ;
	SPI1_j = 0 ;
	SPI1_n = n ;
	// store address of data buffer
	SPI1_data = &data[0] ;
    addr |= 0x80;

    SPIBUF = SPI1BUF;
    SPI1BUF = addr << 8; // issue read command
	return ;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI1Interrupt(void)
{
	unsigned int SPIBUF ;
	// clear interrupt flag as soon as possible so as to not miss any interrupts
	_SPI1IF = 0 ;

    indicate_loading_inter;
    interrupt_save_set_corcon;

	if ( SPI1_i == 0 )
	{
		SPIBUF = SPI1BUF ;
		SPI1BUF = 0x0000 ;
		SPI1_high = 0xFF & SPIBUF;
		SPI1_i = 1 ;
	}
	else if ( SPI1_i < SPI1_n )
	{
		SPIBUF = SPI1BUF ;
		SPI1BUF = 0x0000 ;
		SPI1_low = SPIBUF >> 8 ;
		* ( SPI1_data + SPI1_j ) = SPI1_high << 8 | SPI1_low;
		SPI1_high = 0xFF & SPIBUF;
		SPI1_i++ ;
		SPI1_j++ ;
	}
	else
	{
		SPIBUF = SPI1BUF ;
		SPI1_low = SPIBUF >> 8 ;
		* ( SPI1_data + SPI1_j ) = SPI1_high << 8 | SPI1_low;
		SPI1_SS = 1;
		(* SPI1_read_call_back ) () ; // execute the call back
	}
    interrupt_restore_corcon;
	return ;
}



void initSPI2_master16(unsigned int priPre, unsigned int secPre) {
    /* Holds the information about SPI configuration */
    unsigned int SPICON1Value, SPICON2Value;
    /* Holds the information about SPI Enable/Disable */
    unsigned int SPISTATValue;

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

	_SPI2IF = 0 ;	// clear any pending interrupts
	_SPI2IP = 6 ;	// priority 6
	_SPI2IE = 1 ;	// turn on SPI2 interrupts

}

// blocking 16 bit write to SPI2

void writeSPI2reg16(unsigned int addr, unsigned int data) {
    int k;
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

// Global control block shared by SPI1 routines
unsigned int * SPI2_data ;
uint8_t SPI2_high, SPI2_low ;
int SPI2_i, SPI2_j , SPI2_n ;

void (* SPI2_read_call_back ) ( void ) = &no_call_back ;

// burst read 2n bytes starting at addr

void readSPI2_burst16n(unsigned int data[], int n, unsigned int addr , void (* call_back )( void)) {
	unsigned int SPIBUF ;

    // assert chip select
    SPI2_SS = 0;
	// save address of call back routine
	SPI2_read_call_back = call_back ;
	// initialize indices
	SPI2_i = 0 ;
	SPI2_j = 0 ;
	SPI2_n = n ;
	// save address of data buffer
	SPI2_data = &data[0] ;
    addr |= 0x80;

    SPIBUF = SPI2BUF;
    SPI2BUF = addr << 8; // issue read command
	return ;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _SPI2Interrupt(void)
{
	unsigned int SPIBUF ;
	// clear the interrupt flag as soon as possible so as to not miss any interrupts
	_SPI2IF = 0 ;

    indicate_loading_inter;
    interrupt_save_set_corcon;

	if ( SPI2_i == 0 )
	{
		SPIBUF = SPI2BUF ;
		SPI2BUF = 0x0000 ;
		SPI2_high = 0xFF & SPIBUF;
		SPI2_i = 1 ;
	}
	else if ( SPI2_i < SPI2_n )
	{
		SPIBUF = SPI2BUF ;
		SPI2BUF = 0x0000 ;
		SPI2_low = SPIBUF >> 8 ;
		* ( SPI2_data + SPI2_j ) = SPI2_high << 8 | SPI2_low;
		SPI2_high = 0xFF & SPIBUF;
		SPI2_i++ ;
		SPI2_j++ ;
	}
	else
	{
		SPIBUF = SPI2BUF ;
		SPI2_low = SPIBUF >> 8 ;
		* ( SPI2_data + SPI2_j ) = SPI2_high << 8 | SPI2_low;
		SPI2_SS = 1;
		(* SPI2_read_call_back ) () ; // execute the call back
	}
    interrupt_restore_corcon;
	return ;
}

