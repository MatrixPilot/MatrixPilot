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
#include "mpu_spi.h"
#include "interrupt.h"
#include "oscillator.h"
#include "delay.h"

#if (BOARD_TYPE != UDB4_BOARD)

#include <stdbool.h>
#include <spi.h>

#if (MPU_SPI == 1)

#define MPU_SS       SPI1_SS
#define MPU_SS_TRIS  SPI1_TRIS
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

#elif (MPU_SPI == 2)

#define MPU_SS       SPI2_SS
#define MPU_SS_TRIS  SPI2_TRIS
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
#error "Select either 1 or 2 for MPU SPI."
#endif


static void no_call_back(void);

static void (*mpu_call_back)(void) = &no_call_back;
static uint16_t* SPI_data;
static uint8_t SPI_high;
static uint8_t SPI_low;
#ifndef __dsPIC33E__
static int16_t SPI_i;
static int16_t SPI_j;
static int16_t SPI_n;
#endif


// Configure SPI module in 16-bit master mode
void initMPUSPI_master16(uint16_t priPre, uint16_t secPre)
{
	uint16_t SPICON1Value;      // holds the information about SPI configuration
	uint16_t SPICON2Value;
	uint16_t SPISTATValue;      // holds the information about SPI Enable/Disable

	MPU_SS = 1;                 // deassert MPU SS
	MPU_SS_TRIS = 0;            // make MPU SS an output
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
	_SPIIP  = INT_PRI_MPUSPI;   // set interrupt priority
//	_SPIIE  = 1;                // turn on SPI interrupts
}

// Blocking 16 bit write to SPI
void writeMPUSPIreg16(uint16_t addr, uint16_t data)
{
	int16_t k;

	MPU_SS = 0;                 // assert chip select
	k = SPIBUF;
	_SPIIE = 0;                 // ensure the interrupt is disabled
	_SPIIF = 0;                 // ensure the interrupt flag is clear
	SPIBUF = addr << 8 | data;  // send address and data
#if 1
	while (!_SPIIF);            // wait for transfer to complete
	_SPIIF = 0;                 // clear interrupt flag
#else
	delay_us(32+2);             // allow 16 cycles at 500kHz for the write
#endif
	k = SPIBUF;                 // dump received data
	MPU_SS = 1;                 // deassert chip select
	// this delay is necessary; it appears that SS must be deasserted for one or
	// more SPI clock cycles between writes
//	delayUs(1);
	delay_us(1);
}

static void no_call_back(void)
{
}

#if defined(__dsPIC33E__)

// SPI module has 8 word FIFOs
// burst read 2n bytes starting at addr;
// Since first byte is address, max of 15 data bytes may be transferred with n=7
void readMPUSPI_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (*call_back)(void))
{
	uint16_t i;

	MPU_SS = 0;                 // assert chip select
	mpu_call_back = call_back;  // store the address of the call back routine
	SPI_data = &data[0];        // store address of data buffer
	i = SPIBUF;                 // empty read buffer
	addr |= 0x80;               // write address-1 in high byte + n-1 dummy words to TX FIFO
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
	MPU_SS = 1;
	(*mpu_call_back)();
	interrupt_restore_corcon;
}

#else // no SPI FIFO

// burst read 2n bytes starting at addr
void readMPUSPI_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (*call_back)(void))
{
	uint16_t spibuf;

	MPU_SS = 0;                 // assert chip select
	mpu_call_back = call_back;  // save address of call back routine
	SPI_i = 0;                  // initialize indices
	SPI_j = 0;
	SPI_n = n;
	SPI_data = &data[0];        // save address of data buffer
	addr |= 0x80;
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
		MPU_SS = 1;
		_SPIIE = 0;             // turn off SPI interrupts
		(*mpu_call_back)();
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
		MPU_SS = 1;
		_SPIIE = 0;             // turn off SPI interrupts
		(*mpu_call_back)();
	}
	SPI_high = 0xFF & spibuf;
#endif // 0
	interrupt_restore_corcon;
}

#endif // (__dsPIC33E__)

uint16_t readMPUSPIreg16(uint16_t addr)
{
	int16_t data;

#if defined(__dsPIC33E__)
	while (_SRXMPT == 0)        // clear receive FIFO
#endif
	{
		data = SPIBUF;          // empty receive buffer
	}
	MPU_SS = 0;                 // assert chip select
	_SPIIE = 0;                 // ensure the interrupt is disabled
	_SPIIF = 0;                 // ensure the interrupt flag is clear
	addr |= 0x80;               // set the read bit in addr byte
	SPIBUF = addr << 8;         // issue read command
//	while (!_SRMPT);            // wait for transfer to complete
	while (!_SPIIF);            // wait for transfer to complete
	_SPIIF = 0;                 // clear interrupt flag
	data = SPIBUF;
	MPU_SS = 1;
//	delayUs(40);
	delay_us(40);
	return data;
}

#if 0 // experimental blocking 8 bit read for dsPIC33EP

// FIXME: why doesn't this work? read FIFO is all zeros even though non-zero data is observed on MISO
uint8_t readMPUSPIreg16(uint16_t addr)
{
	int16_t k, data[8];

//	while (_SRXMPT == 0) {
//		data[k] = SPIBUF;       // clear receive FIFO
//	}
	k = SPISTAT;
	_SPIROV = 0;
	MPU_SS = 0;                 // assert chip select
	addr |= 0x80;               // set the read bit in addr byte
	SPIBUF = addr << 8;         // issue read command
	while (_SPIBEC);            // wait for TX FIFO to empty
	while (!_SRMPT);            // wait for last transfer to complete
	MPU_SS = 1;
	delay_us(20);
	MPU_SS = 0;
	SPIBUF = addr << 8;         // issue read command
	while (_SPIBEC);            // wait for TX FIFO to empty
	while (!_SRMPT);            // wait for last transfer to complete
	data[0] = SPIBUF;
	data[1] = SPIBUF;
//	for (k = 0; k < 8; k++) {
//		data[k] = SPIBUF;       // read one word from FIFO
//	}
	MPU_SS = 1;                 // deassert chip select for a while
	delay_us(40);
	return 0xFF & data[0];
}

#endif // 0

#endif // BOARD_TYPE
