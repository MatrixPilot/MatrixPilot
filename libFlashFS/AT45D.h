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


#ifndef DATAFLASH_H
#define DATAFLASH_H


#define USE_DMA

void init_dataflash(void);

void Page_To_Buffer(uint16_t PageAdr, uint8_t BufferNo);
void Buffer_To_Page(uint8_t BufferNo, uint16_t PageAdr);

uint8_t Buffer_Read_Byte(uint8_t BufferNo, uint16_t IntPageAdr);
void Buffer_Write_Byte(uint8_t BufferNo, uint16_t IntPageAdr, uint8_t Data);

void BufferReadStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);
void BufferWriteStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);

void BufferToPage(uint8_t BufferNo, uint16_t PageAdr);
void PageToBuffer(uint16_t PageAdr, uint8_t BufferNo);


#endif // DATAFLASH_H
