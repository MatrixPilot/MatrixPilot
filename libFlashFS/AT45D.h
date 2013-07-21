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


#ifndef AT45D_H
#define AT45D_H


#define USE_AT45D_DMA

void init_dataflash(int);
void AT45D_FormatFS(void);

void ReadSector(uint16_t sector, uint8_t* buffer);
void WriteSector(uint16_t sector, uint8_t* buffer);

// configuration for the Atmel AT45DB321D device
#define PAGE_BITS 10
//#define PAGE_SIZE 528
//#define PageBits 9
//#define PageSize 512

// DataFlash commands
#define StatusReg           0xD7    // Read Status register
#define ReadMfgID           0x9F    // Read Manufacturer and Device ID

#define FlashPageRead       0xD2    // Main memory page read
#define Buf1Read            0xD4    // Buffer 1 read, use 0xD1 for 33MHz read
#define Buf2Read            0xD6    // Buffer 2 read, use 0xD3 for 33MHz read
#define Buf1ToFlashWE       0x83    // Buffer 1 to main memory page program with built-in erase
#define Buf2ToFlashWE       0x86    // Buffer 2 to main memory page program with built-in erase
#define Buf1ToFlash         0x88    // Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash         0x89    // Buffer 2 to main memory page program without built-in erase
#define Buf1Write           0x84    // Buffer 1 write
#define Buf2Write           0x87    // Buffer 2 write
#define FlashToBuf1Transfer 0x53    // Main memory page to buffer 1 transfer
#define FlashToBuf2Transfer 0x55    // Main memory page to buffer 2 transfer
#define FlashToBuf1Compare  0x60    // Main memory page to buffer 1 compare
#define FlashToBuf2Compare  0x61    // Main memory page to buffer 2 compare
//#define AutoPageReWrBuf1    0x58    // Auto page rewrite through buffer 1
//#define AutoPageReWrBuf2    0x59    // Auto page rewrite through buffer 2
#define FlashProgBuf1       0x82    // Main memory page program through buffer 1
#define FlashProgBuf2       0x85    // Main memory page program through buffer 2

#define PageEraseCmd        0x81    // Page erase
#define BlockEraseCmd       0x50    // Block (eight pages) erase
#define SectorEraseCmd      0x7C    // Sector erase

#define ContArrayRead       0x68    // Continuous Array Read (Note : Only A/B-parts supported)
#define EnterDeepPowerdown  0xB9    // Enter Deep Powerdown mode
#define ExitDeepPowerdown   0xAB    // Exit Deep powerdown mode


#endif // AT45D_H
