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

#ifndef SPIUTILS_H
#define SPIUTILS_H


//	initialize SPI in master mode, 16 bit
void initSPI1_master16(uint16_t, uint16_t);
void initSPI2_master16(uint16_t, uint16_t);

//	16 bit SPI blocking write
void writeSPI1reg16(uint16_t addr, uint16_t data);
void writeSPI2reg16(uint16_t addr, uint16_t data);

//	n-word, non-blocking SPI read, followed by call_back
void readSPI1_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (* call_back)(void));
void readSPI2_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (* call_back)(void));


#endif // SPIUTILS_H
