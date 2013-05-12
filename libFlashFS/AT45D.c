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

//#include "defines.h"
#include <stdint.h>
#include "AT45D.h"
#include <spi.h>
#include <stdio.h>


void init_AT45D_DMA(void);


#define DF_CS			_LATE7
#define DF_SCK 			_LATG6
#define DF_MOSI 		_LATG8
#define DF_MISO 		_RG7

#define DF_CS_TRIS		_TRISE7
#define DF_SCK_TRIS 	_TRISG6
#define DF_MOSI_TRIS 	_TRISG8
#define DF_MISO_TRIS 	_TRISG7


static void DF_CS_inactive(void)
{
	DF_CS = 1 ;
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
}

static void DF_CS_active(void)
{
	DF_CS = 0 ;
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time with SCK low to make a more solid pulse
}

void DF_reset(void) 
{
	DF_CS_inactive();
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time
	DF_CS_active();
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time
}

/*
static uint8_t DF_SPI_RW(uint8_t output)
{
	unsigned char result;

	result = SPI2BUF;					// dummy read of the SPIBUF register to clear the SPIRBF flag
	SPI2BUF = output;					// write the data out to the SPI peripheral
	Nop(); Nop(); Nop(); 
    while (!SPI2STATbits.SPIRBF) ;		 // wait for the data to be sent out
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
	result = SPI2BUF;
	return result;
}
 */
uint8_t DF_SPI_RW(uint8_t output)
{
	int timeout = 500;
	unsigned char result;

	while (SPI2STATbits.SPIRBF) {
		result = SPI2BUF;					 // dummy read of the SPIBUF register to clear the SPIRBF flag
//		printf("discarding byte = %x\r\n", result);
	}
//	result = SPI2BUF;					 // dummy read of the SPIBUF register to clear the SPIRBF flag
	SPI2STATbits.SPIROV = 0;

	SPI2BUF = output;					 // write the data out to the SPI peripheral
	Nop(); Nop(); Nop(); 
//    while (!SPI2STATbits.SPIRBF) ;	     // wait for the data to be sent out
    while (!SPI2STATbits.SPIRBF) {
		timeout--;
		if (!timeout) {
			printf("Timeout SPI2STAT = %x\r\n", SPI2STAT);
			SPI2STATbits.SPIROV = 0;
			break;
		}
	}
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time with SCK high to make a more solid pulse
	result = SPI2BUF;
	return result;
}

static void Read_DF_ID(void)
{
	unsigned char manufacturer;
	unsigned char deviceID_1;
	unsigned char deviceID_2;
	unsigned char ext_str_len;

    DF_CS_active();
    DF_SPI_RW(ReadMfgID);
	manufacturer = DF_SPI_RW(0x00);
    deviceID_1   = DF_SPI_RW(0x00);
    deviceID_2   = DF_SPI_RW(0x00);
    ext_str_len  = DF_SPI_RW(0x00);
    DF_CS_inactive();

    if (manufacturer == 0x1f) 
	{
		printf("Atmel ");
	}
    if (deviceID_1 == 0x27) 
	{
		printf("32 Mb DataFlash");
	}
	printf("\r\n");
}

void init_dataflash(void)
{
	SPI2STAT = 0x0;				// disable the SPI module (just in case)
#if 0
	SPI2CON1 = 0x0121;			// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 0; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b000, PPRE = 0b01
//	SPI2CON1bits.CKE = 0x01;
//	SPI2CON1bits.CKP = 0x00;
#else
//	SPI2CON1 = 0x0121;			// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 0; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b000, PPRE = 0b01
//	SPI2CON1bits.CKE = 0x01;
//	SPI2CON1bits.CKP = 0x00;
	SPI2CON1 = 0x013F;			// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 0; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b111, PPRE = 0b11
//	SPI2CON1bits.CKE = b111;
//	SPI2CON1bits.CKP = b11;
#endif
	SPI2STAT = 0x8000;			// enable the SPI module

//	printf("SPI2STAT 0x%04x, SPI2CON1 0x%04x\r\n", SPI2STAT, SPI2CON1);

	DF_MISO_TRIS = 1;
	DF_CS_TRIS = 0;
	DF_SCK_TRIS = 0;
	DF_MOSI_TRIS = 0;
	DF_SCK = 1;
	DF_MOSI  = 1;
	DF_CS_inactive();

    Read_DF_ID();

#ifdef USE_AT45D_DMA
	init_AT45D_DMA();
#endif
}

uint8_t ReadDFStatus(void)
{
	uint8_t result;

	DF_reset();
	result = DF_SPI_RW(StatusReg);
	result = DF_SPI_RW(0x00);
//	device_id = ((result & 0x3C) >> 2);
	return result;
}

void PageErase(uint16_t PageAdr)
{
    DF_reset();                          // reset dataflash command decoder
    DF_SPI_RW(PageEraseCmd);             // Page erase op-code
    DF_SPI_RW((uint8_t)(PageAdr >> 7));  // upper part of page address
    DF_SPI_RW((uint8_t)(PageAdr << 1));  // lower part of page address and MSB of int.page adr.
    DF_SPI_RW(0x00);                     // dont cares
    DF_reset();                          // initiate flash page erase
    while(!(ReadDFStatus() & 0x80));     // monitor the status register, wait until busy-flag is high
}

///////////////////////////////////////////////////////////////////////////////
#ifndef USE_AT45D_DMA

static void BufferToPage(uint8_t BufferNo, uint16_t PageAdr)
{
//	printf("BufferToPage(BufferNo %u, PageAdr %u)\r\n", BufferNo, PageAdr);
	DF_reset();						   // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)                 // program flash page from buffer 1
		DF_SPI_RW(Buf1ToFlashWE);	   // buffer 1 to flash with erase op-code
	else
		DF_SPI_RW(Buf2ToFlashWE);      // buffer 2 to flash with erase op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS))); //upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8))); //lower part of page address
	DF_SPI_RW(0x00);                   // don't cares
	DF_reset();                        // initiate flash page programming
	while(!(ReadDFStatus() & 0x80));   // monitor the status register, wait until busy-flag is high
}

static void PageToBuffer(uint16_t PageAdr, uint8_t BufferNo)
{
//	printf("PageToBuffer(PageAdr %u, BufferNo %u)\r\n", PageAdr, BufferNo);
	DF_reset();						     // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (BufferNo == 1)                   // transfer flash page to buffer 1
		DF_SPI_RW(FlashToBuf1Transfer);  // transfer to buffer 1 op-code
	else
		DF_SPI_RW(FlashToBuf2Transfer);  // transfer to buffer 2 op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS))); // upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8)));  // lower part of page address
	DF_SPI_RW(0x00);                     // don't cares
	DF_reset();                          // init transfer
	while(!(ReadDFStatus() & 0x80));     // monitor the status register, wait until busy-flag is high
}

static void BufferReadStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
{
	uint16_t i;

//	printf("BufferReadStr(BufferNo %u, IntPageAdr %u)\r\n", BufferNo, IntPageAdr);
	DF_reset();                          // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)                   // read uint8_t from buffer 1
		DF_SPI_RW(Buf1Read);             // buffer 1 read op-code
	else
		DF_SPI_RW(Buf2Read);             // buffer 2 read op-code
	DF_SPI_RW(0x00);                     // don't cares
	DF_SPI_RW((uint8_t)(IntPageAdr>>8)); // upper part of internal buffer address
	DF_SPI_RW((uint8_t)(IntPageAdr));    // lower part of internal buffer address
	DF_SPI_RW(0x00);                     // don't cares to initialize the read operation

    for (i = 0; i < No_of_bytes; i++) {
        *(BufferPtr) = DF_SPI_RW(0x00);      // read byte and put it in buffer pointed to by *BufferPtr
//		printf("%02x ", *(BufferPtr));
        BufferPtr++;                         // point to next element in buffer
    }
//	printf("\r\n");
}

static void BufferWriteStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
{
	uint16_t i;

//	printf("BufferWriteStr(BufferNo %u, IntPageAdr %u)\r\n", BufferNo, IntPageAdr);
	DF_reset();                          // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)                   // write enable to buffer 1
		DF_SPI_RW(Buf1Write);            // buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);            // buffer 2 write op-code
	DF_SPI_RW(0x00);                     // Don't care
	DF_SPI_RW((uint8_t)(IntPageAdr>>8)); // Upper part of internal buffer address
	DF_SPI_RW((uint8_t)(IntPageAdr));    // Lower part of internal buffer address

    for (i = 0; i < No_of_bytes; i++) {
//		printf("%02x ", (unsigned char)*BufferPtr);
        DF_SPI_RW(*BufferPtr);        //write byte pointed at by *BufferPtr to dataflash buffer location
        BufferPtr++;                            //point to next element in buffer
    }
//	printf("\r\n");
}

void ReadSector(uint16_t sector, uint8_t* buffer)
{
	PageToBuffer(sector, 2);
	BufferReadStr(2, 0, 512, buffer);
}

void WriteSector(uint16_t sector, uint8_t* buffer)
{
	BufferWriteStr(2, 0, 512, buffer);
	BufferToPage(2, sector);
}

#endif // !USE_AT45D_DMA
