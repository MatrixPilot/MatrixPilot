// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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
//#include "libUDB.h"
#include "oscillator.h"
#include "interrupt.h"
#include "RFM22B.h"
#include "delay.h"

//#include <stdbool.h>
#include <spi.h>

// define which SPI port the RFM is using by defining RFM_SPI to be 1 or 2
// on UDB4, either SPI port can be used to connect RFM.
// on UDB5, SPI2 connects to MPU6000, SPI1 is available for off board.
// on AUAV3, SPI1 connects to MPU6000, SPI2 is available for off board.
// On UDB4/5:
// SPI1 interface uses INT1, RA12 for external interrupt
// SPI2 interface uses INT3, RA13 for external interrupt
// On AUAV3:
// SPI1 interface uses INT1, RG12 for external interrupt

#if (BOARD_TYPE == UDB4_BOARD)
#define RFM_SPI 1
#define _TRISRFMINT _TRISA12

#elif (BOARD_TYPE == UDB5_BOARD)
#define RFM_SPI 2
#define _TRISRFMINT _TRISA13

#elif (BOARD_TYPE == AUAV2_BOARD)
#define RFM_SPI 1
#define _TRISRFMINT _TRISE8

#elif (BOARD_TYPE == AUAV3_BOARD)
#define RFM_SPI 1
#define _TRISRFMINT _TRISG12

#elif (BOARD_TYPE == AUAV4_BOARD)
#define RFM_SPI 1
#define _TRISRFMINT _TRISG12

#else
#error "Only BOARD_TYPEs UDB5, UDB4 and AUAV3 supported"
#endif

#if (RFM_SPI == 1)

#define RFM_SS       SPI1_SS
#define RFM_SS_TRIS  SPI1_TRIS
#define _SPIRBF      SPI1STATbits.SPIRBF
#define _SPIROV      SPI1STATbits.SPIROV
#define _SRXMPT      SPI1STATbits.SRXMPT
#define _SPIBEC      SPI1STATbits.SPIBEC
#define _SRMPT       SPI1STATbits.SRMPT
#define _SPIIF       _SPI1IF
#define _SPIIP       _SPI1IP
#define _SPIIE       _SPI1IE
#define SPIBUF       SPI1BUF
#define SPISTAT      SPI1STAT
#define OpenSPI      OpenSPI1
#define CloseSPI     CloseSPI1
#define ConfigIntSPI ConfigIntSPI1
#define SPIInterrupt _SPI1Interrupt

#elif (RFM_SPI == 2)

#define RFM_SS       SPI2_SS
#define RFM_SS_TRIS  SPI2_TRIS
#undef  _SPIRBF
#define _SPIRBF      SPI2STATbits.SPIRBF
#undef  _SPIROV
#define _SPIROV      SPI2STATbits.SPIROV
#define _SRXMPT      SPI2STATbits.SRXMPT
#define _SPIBEC      SPI2STATbits.SPIBEC
#define _SRMPT       SPI2STATbits.SRMPT
#define _SPIIF       _SPI2IF
#define _SPIIP       _SPI2IP
#define _SPIIE       _SPI2IE
#define SPIBUF       SPI2BUF
#define SPISTAT      SPI2STAT
#define OpenSPI      OpenSPI2
#define CloseSPI     CloseSPI2
#define ConfigIntSPI ConfigIntSPI2
#define SPIInterrupt _SPI2Interrupt

#else
#error "Select either 1 or 2 for RFM SPI."
#endif

// RFM22B registers
#define RFM_DEVICE_TYPE      0x00
#define RFM_DEVICE_VERSION   0x01
#define RFM_DEVICE_STATUS    0x02
#define RFM_INT1_STATUS      0x03
#define RFM_INT2_STATUS      0x04
#define RFM_INT1_ENABLE      0x05
#define RFM_INT2_ENABLE      0x06


#ifdef ENABLE_RFM_SPI_INT
static void no_call_back(void);
static void (*_call_back)(void) = &no_call_back;
static uint16_t* SPI_data;
static uint8_t SPI_high;
static uint8_t SPI_low;
#ifndef __dsPIC33E__
static int16_t SPI_i;
static int16_t SPI_j;
static int16_t SPI_n;
#endif
#endif // ENABLE_RFM_SPI_INT

static void rfmInitSPI(uint16_t priPre, uint16_t secPre);


boolean init_RFM22B(void)
{
// RFM22B maximum SPI clock is specified as 10 MHz
//    NOTE!!: the SPI limit on the dsPIC is 9 Mhz

// 10 MHz is the maximum specified for the RFM-22B SPI SCLK
// however 9 MHz is the maximum specified for the dsPIC33EP
// Primary prescaler options   1:1/4/16/64
// Secondary prescaler options 1:1 to 1:8
#if (MIPS == 70)
	// set prescaler for FCY/32 = 2.2 MHz at 70MIPS
	rfmInitSPI(SEC_PRESCAL_2_1, PRI_PRESCAL_16_1);
#elif (MIPS == 64)
	// set prescaler for FCY/8 = 8 MHz at 64 MIPS
	rfmInitSPI(SEC_PRESCAL_2_1, PRI_PRESCAL_4_1);
#elif (MIPS == 40)
	// UDB5 only
	// set prescaler for FCY/5 = 8 MHz at 40MIPS
	rfmInitSPI(SEC_PRESCAL_5_1, PRI_PRESCAL_1_1);
#elif (MIPS == 32)
	// set prescaler for FCY/4 = 8 MHz at 32 MIPS
	rfmInitSPI(SEC_PRESCAL_1_1, PRI_PRESCAL_4_1);
#elif (MIPS == 16)
	// set prescaler for FCY/2 = 8 MHz at 16 MIPS
	rfmInitSPI(SEC_PRESCAL_2_1, PRI_PRESCAL_1_1);
#else
#error Invalid MIPS Configuration
#endif // MIPS

	DPRINT("RFM Device Type: %04X Version: %04X\r\n", rfmReadRegister(RFM_DEVICE_TYPE), rfmReadRegister(RFM_DEVICE_VERSION));
	if (rfmReadRegister(RFM_DEVICE_TYPE) != 0x0008 && rfmReadRegister(RFM_DEVICE_VERSION) != 0x0006)
	{
		return false;
	}
	_TRISRFMINT = 1; // this is probably already taken care of in mcu.c for most boards
#if (RFM_SPI == 1)
	_INT1EP = 1; // Setup INT1 pin to interrupt on falling edge
	_INT1IP = INT_PRI_INT1;
	_INT1IF = 0; // Reset INT1 interrupt flag
	_INT1IE = 1; // Enable INT1 Interrupt Service Routine 
#elif (RFM_SPI == 2)
	_INT3EP = 1; // Setup INT3 pin to interrupt on falling edge
	_INT3IP = INT_PRI_INT3;
	_INT3IF = 0; // Reset INT3 interrupt flag
	_INT3IE = 1; // Enable INT3 Interrupt Service Routine 
#endif

	return true;
}

#if (RFM_SPI == 1)
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
	_INT1IF = 0; // Clear the INT1 interrupt flag
	indicate_loading_inter;
	interrupt_save_set_corcon;
//	RFM22B_Int();
	interrupt_restore_corcon;
}
#elif (RFM_SPI == 2)
void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void)
{
	_INT3IF = 0; // Clear the INT3 interrupt flag
	indicate_loading_inter;
	interrupt_save_set_corcon;
	RFM22B_Int();
	interrupt_restore_corcon;
}
#else
#error("invalid selection for RFM SPI port, must be 1 or 2")
#endif

// Configure SPI module in 16-bit master mode
static void rfmInitSPI(uint16_t priPre, uint16_t secPre)
{
	uint16_t SPICON1Value;      // holds the information about SPI configuration
	uint16_t SPICON2Value;
	uint16_t SPISTATValue;      // holds the information about SPI Enable/Disable

	RFM_SS = 1;                 // deassert RFM SS
	RFM_SS_TRIS = 0;            // make RFM SS an output
	CloseSPI();                 // turn off SPI module
	ConfigIntSPI(SPI_INT_DIS & SPI_INT_PRI_6);

#if defined(__dsPIC33E__)
	SPICON1Value =
	    ENABLE_SDO_PIN & SPI_MODE16_ON & ENABLE_SCK_PIN &
	    SPI_SMP_OFF & SPI_CKE_OFF &
	    SLAVE_ENABLE_OFF &
	    CLK_POL_ACTIVE_LOW &
	    MASTER_ENABLE_ON &
	    secPre & priPre;
	SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT; // & FIFO_BUFFER_DISABLE;
	SPISTATValue = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR & BUF_INT_SEL_5;
	// BUF_INT_SEL_5 == Interrupt when the last bit is shifted out of SPIxSR, and the transmit is complete
#else
	SPICON1Value =
	    ENABLE_SDO_PIN & SPI_MODE16_ON & ENABLE_SCK_PIN &
	    SPI_SMP_ON & SPI_CKE_OFF &
	    SLAVE_ENABLE_OFF &
	    CLK_POL_ACTIVE_LOW &
	    MASTER_ENABLE_ON &
	    secPre & priPre;
	SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT;
	SPISTATValue = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR;
#endif

#ifdef __PIC32MX__
//	 * Example: OpenSPI1(SPI_MODE32_ON|SPI_SMP_ON|MASTER_ENABLE_ON|SEC_PRESCAL_1_1|PRI_PRESCAL_1_1, SPI_ENABLE);
	OpenSPI(SPICON1Value, SPISTATValue);
#else
	OpenSPI(SPICON1Value, SPICON2Value, SPISTATValue);
//	printf("SPI1STAT %04X, SPI1CON1 %04X, SPI1CON2 %04X\r\n", SPI1STAT, SPI1CON1, SPI1CON2);
#endif

	_SPIROV = 0;                // clear SPI receive overflow
	_SPIIF  = 0;                // clear any pending interrupts
	_SPIIP  = INT_PRI_RFMSPI;   // set interrupt priority
#ifdef ENABLE_RFM_SPI_INT
//	_SPIIE  = 1;                // turn on SPI interrupts
#endif
}

void rfmWriteRegister(uint8_t addr, uint8_t data)
{
	int16_t k;

	RFM_SS = 0;                 // assert chip select
	k = SPIBUF;
	_SPIIE = 0;                 // ensure the interrupt is disabled
	_SPIIF = 0;                 // ensure the interrupt flag is clear
	addr |= 0x80;               // set the write bit in addr byte
	SPIBUF = addr << 8 | data;  // send address and data
#if 1
	while (!_SPIIF);            // wait for transfer to complete
	_SPIIF = 0;                 // clear interrupt flag
#else
	delay_us(32+2);             // allow 16 cycles at 500kHz for the write
#endif
	k = SPIBUF;                 // dump received data
	RFM_SS = 1;                 // deassert chip select
	// this delay is necessary; it appears that SS must be deasserted for one or
	// more SPI clock cycles between writes
}

uint8_t rfmReadRegister(uint8_t addr)
{
	int16_t data;

#if defined(__dsPIC33E__)
	while (_SRXMPT == 0)        // clear receive FIFO
#endif
	{
		data = SPIBUF;          // empty receive buffer
	}
	RFM_SS = 0;                 // assert chip select
	_SPIIE = 0;                 // ensure the interrupt is disabled
	_SPIIF = 0;                 // ensure the interrupt flag is clear
//	addr |= 0x80;               // set the read bit in addr byte
	SPIBUF = addr << 8;         // issue read command
//	while (!_SRMPT);            // wait for transfer to complete
	while (!_SPIIF);            // wait for transfer to complete
	_SPIIF = 0;                 // clear interrupt flag
	data = SPIBUF;
	RFM_SS = 1;
	return data;
}

/*
#ifdef ENABLE_RFM_SPI_INT

static void no_call_back(void)
{
}

#if defined(__dsPIC33E__)

// SPI module has 8 word FIFOs
// burst read 2n bytes starting at addr;
// Since first byte is address, max of 15 data bytes may be transferred with n=7
void rfmReadBuffer(uint16_t data[], int16_t n, uint16_t addr, void (*call_back)(void))
{
	uint16_t i;

	RFM_SS = 0;                 // assert chip select
	_call_back = call_back;  // store the address of the call back routine
	SPI_data = &data[0];        // store address of data buffer
	i = SPIBUF;                 // empty read buffer
//	addr |= 0x80;               // write address-1 in high byte + n-1 dummy words to TX FIFO
	SPIBUF = addr << 8;         // issue read command
	for (i = 0; i < n; i++) {
		SPIBUF = 0;             // queue 'n' null words into the SPI transmit buffer
	}
	_SPIIE = 1;                 // turn on SPI interrupts
}

// this ISR empties the RX FIFO into the SPI_data buffer
// no possibility of overrun if buffer length is at least 8 words
void __attribute__((__interrupt__, __no_auto_psv__)) SPIInterrupt(void)
{
	uint16_t spibuf;

	_SPIIF = 0;                 // clear interrupt flag as soon as possible so as to not miss any interrupts
	indicate_loading_inter;
	interrupt_save_set_corcon;
	_SPIIE = 0;                 // turn off SPI interrupts
	spibuf = SPIBUF;            // get first byte from first word
	SPI_high = 0xFF & spibuf;

	while (!_SRXMPT) {          // empty the FIFO
		spibuf = SPIBUF;
		SPI_low = spibuf >> 8;
		*SPI_data++ = SPI_high << 8 | SPI_low;
		SPI_high = 0xFF & spibuf;
	}
	RFM_SS = 1;
	(*_call_back)();
	interrupt_restore_corcon;
}

#else // no SPI FIFO

// burst read 2n bytes starting at addr
void rfmReadBuffer(uint16_t data[], int16_t n, uint16_t addr, void (*call_back)(void))
{
	uint16_t spibuf;

	RFM_SS = 0;                 // assert chip select
	_call_back = call_back;  // save address of call back routine
	SPI_i = 0;                  // initialize indices
	SPI_j = 0;
	SPI_n = n;
	SPI_data = &data[0];        // save address of data buffer
//	addr |= 0x80;
	spibuf = SPIBUF;
	SPIBUF = addr << 8;         // issue read command
	_SPIIE = 1;                 // turn on SPI interrupts
}

void __attribute__((__interrupt__, __no_auto_psv__)) SPIInterrupt(void)
{
	uint16_t spibuf;

	_SPIIF = 0;                 // clear interrupt flag as soon as possible so as to not miss any interrupts
	indicate_loading_inter;
	interrupt_save_set_corcon;
#if 1
	if (SPI_i == 0) {
		spibuf = SPIBUF;                               // could move this to before the conditional
		SPIBUF = 0x0000;
		SPI_high = 0xFF & spibuf;                      // could move this to after the conditional
		SPI_i = 1;
	} else if (SPI_i < SPI_n) {
		spibuf = SPIBUF;                               // could move this to before the conditional
		SPIBUF = 0x0000;
		SPI_low = spibuf >> 8;                         // could move this to before the conditional
		*(SPI_data + SPI_j) = SPI_high << 8 | SPI_low; // could move this to before the conditional
		SPI_high = 0xFF & spibuf;                      // could move this to after the conditional
		SPI_i++;
		SPI_j++;
	} else {
		spibuf = SPIBUF;                               // could move this to before the conditional
		SPI_low = spibuf >> 8;                         // could move this to before the conditional
		*(SPI_data + SPI_j) = SPI_high << 8 | SPI_low; // could move this to before the conditional
		RFM_SS = 1;
		_SPIIE = 0;             // turn off SPI interrupts
		(*_call_back)();
	}
#else
	spibuf = SPIBUF;
	SPI_low = spibuf >> 8;
	*(SPI_data + SPI_j) = SPI_high << 8 | SPI_low;
	SPI_i++;
	if (SPI_i == 1) {
		SPIBUF = 0x0000;
	} else if (SPI_i <= SPI_n) {
		SPIBUF = 0x0000;
		SPI_j++;
	} else {
		RFM_SS = 1;
		_SPIIE = 0;             // turn off SPI interrupts
		(*_call_back)();
	}
	SPI_high = 0xFF & spibuf;
#endif // 0
	interrupt_restore_corcon;
}
#endif // ENABLE_RFM_SPI_INT

#endif // (__dsPIC33E__)
 */
