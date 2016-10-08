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


#include "libUDB.h"
#include "options_osd.h"
#include "osd.h"

#if (USE_OSD == OSD_NATIVE && USE_OSD_SPI == 1)

#warning "Using OSD_NATIVE in SPI hardware mode"

#include "interrupt.h"
#include "oscillator.h"
#if (BOARD_TYPE != AUAV4_BOARD)
#include <spi.h>
#else
#include <plib.h>
#endif

uint8_t osd_spi_read(int8_t addr);

//  UDB4 uses SPI1 port
// AUAV3 uses SPI3 port

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD) 

#warning "OSD configured for SPI port 1"

#if ( OSD_LEGACY_SPI_PINOUT == 1)
#warning "Using legacy OSD pinout:- UDB4/5 board labels do not match signals"

#define OSD_CS          _LATF7 
#define OSD_SCK         _LATF8 
#define OSD_MOSI        _LATF6 
// OSD_MISO was not defined in legacy pinout

#define OSD_CS_TRIS     _TRISF7
#define OSD_SCK_TRIS    _TRISF8
#define OSD_MOSI_TRIS   _TRISF6
#define OSD_MISO_TRIS   _TRISD12

#else // OSD_LEGACY_SPI_PINOUT not being used
#warning "Using OSD SPI pinout that matches labelling on UDB4 and UDB5 boards"
#define OSD_CS          _LATB2
#define OSD_SCK         _LATF6 
#define OSD_MOSI        _LATF7 
#define OSD_MISO        _LATF8

#define OSD_CS_TRIS     _TRISB2
#define OSD_SCK_TRIS    _TRISF6
#define OSD_MOSI_TRIS   _TRISF7
#define OSD_MISO_TRIS   _TRISF8

#endif //( OSD_LEGACY_SPI_PINOUT == 1)

#define SPIxSTAT        SPI1STAT
#define SPIxCON1        SPI1CON1
#define SPIxCON2        SPI1CON2
#define SPIxBUF         SPI1BUF
#define SPIxSTATbits    SPI1STATbits
#define SPIxCON1bits    SPI1CON1bits

#define _SPIEN          SPI1STATbits.SPIEN

#elif (BOARD_TYPE == AUAV3_BOARD)

#warning "OSD configured for SPI port 3"

#define OSD_CS          _LATD2
#define OSD_SCK         _LATD1
#define OSD_MOSI        _LATD3
#define OSD_MISO        _RD12

#define OSD_CS_TRIS     _TRISD2
#define OSD_SCK_TRIS    _TRISD1
#define OSD_MOSI_TRIS   _TRISD3
#define OSD_MISO_TRIS   _TRISD12

#define SPIxSTAT        SPI3STAT
#define SPIxCON1        SPI3CON1
#define SPIxCON2        SPI3CON2
#define SPIxBUF         SPI3BUF
#define SPIxSTATbits    SPI3STATbits
#define SPIxCON1bits    SPI3CON1bits

#define _SPIEN          SPI3STATbits.SPIEN

#elif (BOARD_TYPE == AUAV4_BOARD)

#warning "OSD configured for SPI port 2"

#define OSD_CS          _LATD2
#define OSD_SCK         _LATD1
#define OSD_MOSI        _LATD3
#define OSD_MISO        _RD12

#define OSD_CS_TRIS     _TRISD2
#define OSD_SCK_TRIS    _TRISD1
#define OSD_MOSI_TRIS   _TRISD3
#define OSD_MISO_TRIS   _TRISD12

#define SPIxSTAT        SPI2STAT
#define SPIxCON1        SPI2CON
#define SPIxCON2        SPI2CON2
#define SPIxBUF         SPI2BUF
#define SPIxSTATbits    SPI2STATbits
#define SPIxCON1bits    SPI2CONbits

#define _SPIEN          SPI2CONbits.ON

#else
#warning "OSD not configured for SPI port"
#endif


extern void __delay32(unsigned long cycles);

static void initSPI_OSD(uint16_t priPre, uint16_t secPre)
{
	uint16_t SPICON1Value = 0, SPICON2Value = 0;
	uint16_t SPISTATValue = 0;

#if defined(__dsPIC33E__)
	SPICON1Value =
	    ENABLE_SDO_PIN & SPI_MODE16_OFF & ENABLE_SCK_PIN &
	    SPI_SMP_OFF & SPI_CKE_ON &
	    SLAVE_ENABLE_OFF &
	    CLK_POL_ACTIVE_HIGH &
	    MASTER_ENABLE_ON &
	    secPre & priPre;
//	SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT; // & FIFO_BUFFER_DISABLE;
//	SPISTATValue = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR; // & BUF_INT_SEL_5;
	SPISTATValue = SPI_ENABLE;
#else
	SPICON1Value =
	    ENABLE_SDO_PIN & SPI_MODE16_OFF & ENABLE_SCK_PIN &
	    SPI_SMP_ON & SPI_CKE_OFF &
	    SLAVE_ENABLE_OFF &
	    CLK_POL_ACTIVE_LOW &
	    MASTER_ENABLE_ON &
	    secPre & priPre;
	SPICON2Value = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT;
	SPISTATValue = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR;
#endif

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
	CloseSPI1();
//	ConfigIntSPI1(SPI_INT_DIS & SPI_INT_PRI_6);
	OpenSPI1(SPICON1Value, SPICON2Value, SPISTATValue);
	SPI1STATbits.SPIROV = 0;    // clear SPI receive overflow
	_SPI1IF = 0;                // clear any pending interrupts
//	_SPI1IP = INT_PRI_SPI1;     // set interrupt priority
//	_SPI1IE = 1;                // turn on SPI interrupts
#elif (BOARD_TYPE == AUAV3_BOARD)
	CloseSPI3();
//	ConfigIntSPI3(SPI_INT_DIS & SPI_INT_PRI_6);
	OpenSPI3(SPICON1Value, SPICON2Value, SPISTATValue);
	SPI3STATbits.SPIROV = 0;    // clear SPI receive overflow
	_SPI3IF = 0;                // clear any pending interrupts
//	_SPI3IP = INT_PRI_SPI3;     // set interrupt priority
//	_SPI3IE = 1;                // turn on SPI interrupts

// SPIxSTAT 801C, SPIxCON1 0079, SPIxCON2 0000
#endif
}

void osd_spi_init(void)
{
#if 0
	SPIxSTAT = 0x0;               // disable the SPI module (just in case)
	SPIxCON1 = 0x0122;            // DISSCK = 0, DISSDO = 0, MODE16 = 0; SMP = 0;      CKE = 1;
	                              //   SSEN = 0;    CKP = 0;  MSTEN = 1; SPRE = 0b000, PPRE = 0b10
	SPIxSTAT = 0x8000;            // enable the SPI module
#else

#if (MIPS == 64)
	// set prescaler for FCY/128 = 500 kHz at 64MIPS
	initSPI_OSD(SEC_PRESCAL_2_1, PRI_PRESCAL_64_1);
#elif (MIPS == 32)
	// set prescaler for FCY/64 = 500 kHz at 32 MIPS
	initSPI_OSD(SEC_PRESCAL_4_1, PRI_PRESCAL_16_1);
#elif (MIPS == 16)
	// set prescaler for FCY/32 = 500 kHz at 16MIPS
	initSPI_OSD(SEC_PRESCAL_2_1, PRI_PRESCAL_16_1);
#else
#error Invalid MIPS Configuration
#endif // MIPS

#endif // 1/0

//	printf("SPIxSTAT %04X, SPIxCON1 %04X, SPIxCON2 %04X\r\n", SPIxSTAT, SPIxCON1, SPIxCON2);

	OSD_MISO_TRIS = 1;
	OSD_CS_TRIS = 0;
	OSD_SCK_TRIS = 0;
	OSD_MOSI_TRIS = 0;
	OSD_SCK = 1;
	OSD_MOSI = 1;
	OSD_CS = 1;

//	uint8_t vm0 = osd_spi_read(0x80);
//	if (vm0 & 0x40) {
//		printf("OSD PAL mode\r\n");
//	} else {
//		printf("OSD NTSC mode\r\n");
//	}
}

void spi_write_raw_byte(uint8_t byte)
{
	short temp;

	temp = SPIxBUF;                    // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = byte;                    // write the data out to the SPI peripheral
	while (!SPIxSTATbits.SPIRBF) {}    // wait for the data to be sent out
}

void osd_spi_write_byte(int8_t byte)
{
	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(byte);   // Send the data
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 
}

void osd_spi_write(int8_t addr, int8_t byte)
{
	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr);   // Send the Address
	spi_write_raw_byte(byte);   // Send the data
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 
}

uint8_t spi_xfer_raw_byte(uint8_t data)
{
	uint8_t SPIData;

	SPIData = SPIxBUF;              // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = data;                 // write the data out to the SPI peripheral
	while (!SPIxSTATbits.SPIRBF) {} // wait for the data to be transferred
	SPIData = SPIxBUF;
	return SPIData;
}

uint8_t osd_spi_read(int8_t addr)
{
	uint8_t SPIData;

	OSD_CS = 0;                     // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr);       // Send the Address
	__delay32(20000UL * OSD_SF);
	SPIData = spi_xfer_raw_byte(0);
	OSD_CS = 1;                     // Set active-low CS high to end the SPI cycle 
	return SPIData;
}

void osd_set_xy(int x, int y)
{
	int loc = OSD_LOC(x, y);
	osd_spi_write(0x05, (uint8_t)(loc >> 8));   // DMAH
	osd_spi_write(0x06, (uint8_t)(loc & 0xFF)); // DMAL
}

#if 0
void osd_print(int8_t *str)
{
	osd_spi_write(0x04,1);             // DMM: Enable auto-increment mode

	while (*str)
	{
		osd_spi_write_byte((*str) - 60);
		str++;
	}
	osd_spi_write_byte(0xFF);          // Disables auto-increment mode when sending 0xFF at the end of a string
}
#else

void spi_write_raw_short(unsigned short data)
{
	unsigned short temp;

	temp = SPIxBUF;                    // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = data;                    // write the data out to the SPI peripheral
	while (!SPIxSTATbits.SPIRBF) {}    // wait for the data to be sent out
}

void spi_write_raw_fast(unsigned short byte)
{
	unsigned short temp;

	temp = SPIxBUF;                    // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = byte;                    // write the data out to the SPI peripheral 
	while (!SPIxSTATbits.SPIRBF) {}    // wait for the data to be sent out
}

void osd_print(int8_t *str)
{
//	uint8_t ch;
	unsigned short ch;

	osd_spi_write(0x04,1);      // DMM: Enable auto-increment mode

	SPIxSTATbits.SPIEN = 0;     // disable the SPI module
	SPIxCON1bits.MODE16 = 1;
	SPIxSTATbits.SPIEN = 1;     // enable the SPI module

	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 

	while (*str)
	{
		if (*str == ' ') {
			ch = 138;
		} else {
//			ch = ((*str) - 60); // write the data out to the SPI peripheral
			ch = ((*str) + 42); // write the data out to the SPI peripheral
		}
		ch = ch * 256;

//		OSD_CS = 0;             // Set active-low CS low to start the SPI cycle 
//		spi_write_raw_fast(ch);
		spi_write_raw_short(ch);
//		spi_write_raw_fast(0);
//		OSD_CS = 1;             // Set active-low CS low to start the SPI cycle 

		str++;
	}
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 

	SPIxSTATbits.SPIEN = 0;     // disable the SPI module
	SPIxCON1bits.MODE16 = 0;
	SPIxSTATbits.SPIEN = 1;     // enable the SPI module

	osd_spi_write_byte(0xFF);   // Disables auto-increment mode
}

#endif // 0

#endif // (USE_OSD == OSD_NATIVE && USE_OSD_SPI == 1)

