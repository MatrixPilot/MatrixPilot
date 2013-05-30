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
#include "../libUDB/interrupt.h"
#include "AT45D.h"
#include <stdio.h>


extern void DF_reset(void);
extern uint8_t DF_SPI_RW(uint8_t output);
extern uint8_t ReadDFStatus(void);


#ifdef USE_AT45D_DMA

#define SPI2_DMA_SIZE 512
__eds__ unsigned char Spi2RxBuffA[SPI2_DMA_SIZE+8] __attribute__((eds,space(dma)));
__eds__ unsigned char Spi2TxBuffA[SPI2_DMA_SIZE+8] __attribute__((eds,space(dma)));

volatile int16_t IsBusy __attribute__ ((near)) = 0;


void init_AT45D_DMA(void)
{
	unsigned int i;

	for (i = 0; i < (SPI2_DMA_SIZE); i++)
		Spi2TxBuffA[i] = (i + 16) % 8;
	for (i = 0; i < (SPI2_DMA_SIZE); i++) {
		Spi2RxBuffA[i] = 0x00;
	}

	DMA2CON = 0x6001;	// Reads from DPSRAM (or RAM) address, writes to peripheral address, 
						// Byte data transfer size
						// Initiates interrupt when all of the data has been moved
						// Register Indirect with Post-Increment mode,
						// One-Shot, Ping-Pong modes are disabled
	DMA2CNT = SPI2_DMA_SIZE - 1;
	DMA2REQ = 0x021;				// SPI2
	DMA2PAD = (volatile unsigned int)&SPI2BUF;
	DMA2STAH = 0x0000;
	DMA2STAL = __builtin_dmaoffset(&Spi2TxBuffA);
//	_DMA2IP = INT_PRI_DMA2;			// Set the DMA2 ISR priority
//	IFS0bits.DMA2IF  = 0;			// Clear DMA interrupt
//	IEC0bits.DMA2IE  = 1;			// Enable DMA interrupt
//	DMA2CONbits.CHEN = 1;			// Enable DMA Channel	

	DMA1CON = 0x4001;	// Reads from peripheral address, writes to DPSRAM (or RAM) address, 
						// Byte data transfer size
						// Initiates interrupt when all of the data has been moved
						// Register Indirect with Post-Increment mode,
						// One-Shot, Ping-Pong modes are disabled
	DMA1CNT = SPI2_DMA_SIZE;
	DMA1REQ = 0x021;				// SPI2
	DMA1PAD = (volatile unsigned int)&SPI2BUF;
	DMA1STAH = 0x0000;
	DMA1STAL = __builtin_dmaoffset(&Spi2RxBuffA);
//	_DMA1IP = INT_PRI_DMA1;			// Set the DMA1 ISR priority
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
	DMA2REQbits.FORCE = 1;
	while (DMA2REQbits.FORCE == 1);
}
 */

void __attribute__((__interrupt__, __no_auto_psv__)) _DMA2Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;

	_DMA2IF = 0;

	interrupt_restore_corcon;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _DMA1Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;

	_DMA1IF = 0;

#define DF_CS			_LATE7
	DF_CS = 1 ;
	IsBusy = 0;

	interrupt_restore_corcon;
}
/*
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
 */
////////////////////////////////////////////////////////////////////////////////

//#define SPI_VERBOSE

static int AT45D_WriteSector(unsigned int sector)
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
	DMA2CONbits.CHEN = 1;				 // enable DMA Channel
	SPI2BUF = 0;						 // start the DMA transaction with the don't care byte
//	DMA2REQbits.FORCE = 1;
	return 1;
}

static int AT45D_ReadSector(unsigned int sector)
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
//	DMA1REQbits.FORCE = 1;
	return 1;
}

#define USE_FAST_COPY

static void AT45D_GetBuffer(uint8_t* buffer)
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

static void AT45D_PutBuffer(uint8_t* buffer)
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

void ReadSector(uint16_t sector, uint8_t* buffer)
{
//printf("rs %u\r\n", sector);
	AT45D_ReadSector(sector);
	AT45D_GetBuffer(buffer);
}

void WriteSector(uint16_t sector, uint8_t* buffer)
{
//printf("ws %u\r\n", sector);
	AT45D_PutBuffer(buffer);
	AT45D_WriteSector(sector);
}

#endif // USE_AT45D_DMA
