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


#ifndef __RFM22B_H__
#define __RFM22B_H__

void RFM22B_Int(); // this must be defined for the external interrupt handler to call

boolean init_RFM22B(void);
//void rfmInitSPI(uint16_t priPre, uint16_t secPre);
void rfmWriteRegister(uint8_t reg, uint8_t data);
uint8_t rfmReadRegister(uint8_t reg);

//#define ENABLE_RFM_SPI_INT
#ifdef ENABLE_RFM_SPI_INT
// n-word, non-blocking SPI read, followed by call_back
void rfmReadBuffer(uint16_t data[], int16_t n, uint16_t addr, void (*call_back)(void));
#endif // ENABLE_RFM_SPI_INT

#endif // __RFM22B_H__
