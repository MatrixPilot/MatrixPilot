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

#include "defines.h"
#include "AT45D.h"
#include <stdio.h>


#ifdef USE_DMA

#define SPI2_DMA_SIZE 512
__eds__ unsigned char Spi2RxBuffA[SPI2_DMA_SIZE+8] __attribute__((eds,space(dma)));
__eds__ unsigned char Spi2TxBuffA[SPI2_DMA_SIZE+8] __attribute__((eds,space(dma)));

int IsBusy = 0;

int dma0intcnt = 0;
int dma1intcnt = 0;

void cfgDma0SpiTx(void)
{
	DMA0CON = 0x6001;	// Reads from DPSRAM (or RAM) address, writes to peripheral address, 
						// Byte data transfer size
						// Initiates interrupt when all of the data has been moved
						// Register Indirect with Post-Increment mode,
						// One-Shot, Ping-Pong modes are disabled
//	DMA0CNT = SPI2_DMA_SIZE - 2;
	DMA0CNT = SPI2_DMA_SIZE - 1;
	DMA0REQ = 0x021;				// SPI2
	DMA0PAD = (volatile unsigned int)&SPI2BUF;
	DMA0STAH = 0x0000;
	DMA0STAL = __builtin_dmaoffset(&Spi2TxBuffA);

//	_DMA0IP = 5 ;		// Set the DMA0 ISR priority

//	IFS0bits.DMA0IF  = 0;			// Clear DMA interrupt
//	IEC0bits.DMA0IE  = 1;			// Enable DMA interrupt
//	DMA0CONbits.CHEN = 1;			// Enable DMA Channel	
}

void cfgDma1SpiRx(void)
{
	DMA1CON = 0x4001;	// Reads from peripheral address, writes to DPSRAM (or RAM) address, 
						// Byte data transfer size
						// Initiates interrupt when all of the data has been moved
						// Register Indirect with Post-Increment mode,
						// One-Shot, Ping-Pong modes are disabled
//	DMA1CNT = SPI2_DMA_SIZE - 1;
	DMA1CNT = SPI2_DMA_SIZE;
	DMA1REQ = 0x021;				// SPI2
	DMA1PAD = (volatile unsigned int)&SPI2BUF;
	DMA1STAH = 0x0000;
	DMA1STAL = __builtin_dmaoffset(&Spi2RxBuffA);

//	_DMA1IP = 5 ;		// Set the DMA1 ISR priority

	IFS0bits.DMA1IF  = 0;			// Clear DMA interrupt
	IEC0bits.DMA1IE  = 1;			// Enable DMA interrupt
//	DMA1CONbits.CHEN = 1;			// Enable DMA Channel		
}
/*
void cfgSpi2Master(void)
{
// Configure SPI2CON register to the following
// •	Idle state for clock is a low level (SPI2CON1bits.CKP=?)
// •	Data out on Active to Idle Edge (SPI2CON1bits.CKE=?)
// •	16-bit data transfer mode (SPI2CON1bits.MODE16=?)
// •	Enable Master mode (SPI2CON1bits.MSTEN=?)
// •	Set Primary Pre-scalar for 4:1 ratio (SPI2CON1bits.PPRE=?)
// •	Set Secondary Pre-scalar for 2:1 ratio (SPI2CON1bits.SPRE=?)
// •	Enable SDO output (SPI2CON1bits.DISSDO=?)
// •	Enable SCK output (SPI2CON1bits.DISSCK=?)
	SPI2CON1bits.CKP = 0; 
	SPI2CON1bits.CKE = 1; 
	SPI2CON1bits.MODE16 = 0; 
	SPI2CON1bits.MSTEN = 1; 
	SPI2CON1bits.SPRE = 6; 
	SPI2CON1bits.PPRE = 2;
 	SPI2CON1bits.DISSDO = 0;
	SPI2CON1bits.DISSCK = 0;	

//	SPI2CON1 = b0000000100111010;
//	SPI2CON1 = 0x013A;

// •	Enable SPI module (SPI2STATbits.SPIEN=?)
	SPI2STATbits.SPIEN = 1; 

// Force First word after Enabling SPI
    DMA0REQbits.FORCE = 1;
    while (DMA0REQbits.FORCE == 1);
}
 */
void initSPIBuff(void)
{
	unsigned int i;
    
	for (i = 0; i < (SPI2_DMA_SIZE); i++)
        Spi2TxBuffA[i] = (i + 16) % 8;
   	for (i = 0; i < (SPI2_DMA_SIZE); i++) {
        Spi2RxBuffA[i] = 0x00;
	}	
}

void __attribute__((interrupt, no_auto_psv)) _DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;
	dma0intcnt++;
}

void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0;
	dma1intcnt++;

#define DF_CS			_LATE7
	DF_CS = 1 ;
	IsBusy = 0;
//	ProcessSpiRxSamples(&Spi2RxBuffA[0]);
}

//void ProcessSpiRxSamples(__eds__ unsigned char* SpiRxBuffer)
//{
	// Do something with SPI Samples
//}

unsigned char GetRxByte(unsigned int i)
{
	unsigned char data;

	data = Spi2RxBuffA[i];
	return data; 
}

void DumpRxData(void)
{
	int i;

	for (i = 0; i < SPI2_DMA_SIZE; i++) {
	   	printf("%02x ", GetRxByte(i));
	}
   	printf(":: ");
	for (i = SPI2_DMA_SIZE; i < (SPI2_DMA_SIZE+4); i++) {
	   	printf("%02x ", GetRxByte(i));
	}
   	printf("\r\n");
}

void ShowIntCnt(void)
{
   	printf("dma0intcnt %u dma1intcnt %u\r", dma0intcnt, dma1intcnt);
}

////////////////////////////////////////////////////////////////////////////////
#define DF_CS			_LATE7
#define DF_SCK 			_LATG6
#define DF_MOSI 		_LATG8
#define DF_MISO 		_RG7

#define DF_CS_TRIS		_TRISE7
#define DF_SCK_TRIS 	_TRISG6
#define DF_MOSI_TRIS 	_TRISG8
#define DF_MISO_TRIS 	_TRISG7

// configuration for the Atmel AT45DB321D device
#define PAGE_BITS 10
#define PAGE_SIZE 528
//#define PageBits 9
//#define PageSize 512

// DataFlash commands
#define StatusReg			0xD7	// Read Status register
#define ReadMfgID			0x9F	// Read Manufacturer and Device ID

#define FlashPageRead		0xD2	// Main memory page read
#define Buf1Read			0xD4	// Buffer 1 read, use 0xD1 for 33MHz read
#define Buf2Read			0xD6	// Buffer 2 read, use 0xD3 for 33MHz read
#define Buf1ToFlashWE		0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf2ToFlashWE		0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf1ToFlash			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash			0x89	// Buffer 2 to main memory page program without built-in erase
#define Buf1Write			0x84	// Buffer 1 write
#define Buf2Write			0x87	// Buffer 2 write
#define FlashToBuf1Transfer 0x53	// Main memory page to buffer 1 transfer
#define FlashToBuf2Transfer 0x55	// Main memory page to buffer 2 transfer
#define FlashToBuf1Compare	0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	0x61	// Main memory page to buffer 2 compare
//#define AutoPageReWrBuf1	0x58	// Auto page rewrite through buffer 1
//#define AutoPageReWrBuf2	0x59	// Auto page rewrite through buffer 2
#define FlashProgBuf1		0x82	// Main memory page program through buffer 1
#define FlashProgBuf2		0x85	// Main memory page program through buffer 2

#define PageEraseCmd		0x81	// Page erase
#define BlockEraseCmd		0x50	// Block (eight pages) erase
#define SectorEraseCmd		0x7C	// Sector erase

#define ContArrayRead		0x68    // Continuous Array Read (Note : Only A/B-parts supported)
#define EnterDeepPowerdown	0xB9    // Enter Deep Powerdown mode
#define ExitDeepPowerdown	0xAB    // Exit Deep powerdown mode


static void DF_CS_inactive(void)
{
	DF_CS = 1 ;
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time with SCK high to make a more solid pulse
}

static void DF_CS_active(void)
{
	DF_CS = 0 ;
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time with SCK low to make a more solid pulse
}

static void DF_reset(void) 
{
	DF_CS_inactive();
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time
	DF_CS_active();
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); // Kill some time
}

static unsigned char DF_SPI_RW(unsigned char output)
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
//	SPI2CON1 = 0x0121;			// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 0; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b000, PPRE = 0b01
//	SPI2CON1bits.CKE = 0x01;
//	SPI2CON1bits.CKP = 0x00;
	SPI2CON1 = 0x013F;			// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 0; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b111, PPRE = 0b11
//	SPI2CON1bits.CKE = b111;
//	SPI2CON1bits.CKP = b11;
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
}

static uint8_t ReadDFStatus(void)
{
	uint8_t result;

	DF_reset();                          //reset dataflash command decoder
	result = DF_SPI_RW(StatusReg);       //send status register read op-code
	result = DF_SPI_RW(0x00);            //dummy write to get result
//	device_id = ((result & 0x3C) >> 2);  //get the device id bits, butterfly dataflash should be 0111
	return result;                       //return the read status register value
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

int AT45D_WriteSector(unsigned int sector)
{
//	printf("AT45D_WriteSector(%u)\r\n", sector);

#ifdef SPI_VERBOSE
	while (IsBusy) {
		printf(".");
	} 
	printf("\r\n");

	while (SPI2STATbits.SPIRBF) {
		int result = SPI2BUF;					 // dummy read of the SPIBUF register to clear the SPIRBF flag
		printf("AT45D_WriteSector discarding %x\r\n", result);
	}
	if (SPI2STATbits.SPIROV) {
		printf("AT45D_WriteSector SPI2STAT = %x\r\n", SPI2STAT);
		SPI2STATbits.SPIROV = 0;
	}
	while (!(ReadDFStatus() & 0x80)) {
		printf("@");
	}
#else
	while (IsBusy);
	while (!(ReadDFStatus() & 0x80));    // monitor the status register, wait until busy-flag is high
#endif

	IsBusy = 1;
	DF_reset();						     // reset dataflash command decoder
	DF_SPI_RW(FlashProgBuf1);	   	     // buffer 1 to flash with erase op-code
	DF_SPI_RW((unsigned char)(sector >> (16 - PAGE_BITS))); // upper part of page address
	DF_SPI_RW((unsigned char)(sector << (PAGE_BITS - 8)));  // lower part of page address
//	DF_SPI_RW(0x00);					 // don't cares

	DMA1CONbits.NULLW = 0;
	DMA1CONbits.CHEN = 1;				 // enable DMA Channel
	DMA0CONbits.CHEN = 1;				 // enable DMA Channel
	SPI2BUF = 0;						 // start the DMA transaction with the don't care byte
//	DMA0REQbits.FORCE = 1;
    return 1;
}

int AT45D_ReadSector(unsigned int sector)
{
//	printf("AT45D_ReadSector(%u)\r\n", sector);

#ifdef SPI_VERBOSE
	while (IsBusy) {
		printf(".");
	} 
	printf("\r\n");

	while (SPI2STATbits.SPIRBF) {
		int result = SPI2BUF;					 // dummy read of the SPIBUF register to clear the SPIRBF flag
		printf("AT45D_ReadSector discarding %x\r\n", result);
	}
	if (SPI2STATbits.SPIROV) {
		printf("AT45D_ReadSector SPI2STAT = %x\r\n", SPI2STAT);
		SPI2STATbits.SPIROV = 0;
	}
	while (!(ReadDFStatus() & 0x80)) {
		printf("#");
	}
#else
	while (IsBusy);
	while (!(ReadDFStatus() & 0x80));    // monitor the status register, wait until busy-flag is high
#endif

	IsBusy = 1;
	DF_reset();						     // reset dataflash command decoder
	DF_SPI_RW(FlashPageRead);			 // transfer to buffer 1 op-code
	DF_SPI_RW((unsigned char)(sector >> (16 - PAGE_BITS))); // upper part of page address
	DF_SPI_RW((unsigned char)(sector << (PAGE_BITS - 8)));  // lower part of page address
	DF_SPI_RW(0x00);                     // page starting address (lower 8 bits)
	DF_SPI_RW(0x00);                     // don't cares
	DF_SPI_RW(0x00);                     // don't cares
	DF_SPI_RW(0x00);                     // don't cares
	DF_SPI_RW(0x00);                     // don't cares

//	DF_SPI_RW(0x00);                     // why is this required - odd...
	DMA1CONbits.NULLW = 1;
	DMA1CONbits.CHEN = 1;				 // enable DMA Channel
	SPI2BUF = 0;						 // start the DMA transaction
//    DMA1REQbits.FORCE = 1;
    return 1;
}

#define USE_FAST_COPY

void AT45D_GetBuffer(uint8_t* buffer)
{
	while (IsBusy) ;

//void *memcpy(void *dst , const void *src , size_t n);
//	memcpy (buffer, (__eds__ unsigned char*)&Spi2RxBuffA, 512);
#ifdef USE_FAST_COPY
	int* dst = (int*)buffer;
	__eds__ int* src = (__eds__ int*)Spi2RxBuffA;

	int count = 512 / 2;
	while (count--) {
		*dst++ = *src++;
	}
#else
	int i;
	for (i = 0; i < 512; i++) {
		buffer[i] = Spi2RxBuffA[i];
	}
#endif
}

void AT45D_PutBuffer(uint8_t* buffer)
{
	while (IsBusy) ;

#ifdef USE_FAST_COPY
	int* src = (int*)buffer;
	__eds__ int* dst = (__eds__ int*)Spi2TxBuffA;

	int count = 512 / 2;
	while (count--) {
		*dst++ = *src++;
	}
#else
	int i;
	for (i = 0; i < 512; i++) {
		Spi2TxBuffA[i] = buffer[i];
	}
#endif
}

#endif // USE_DMA
