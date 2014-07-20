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


#ifndef EEPROM_UDB4_H
#define EEPROM_UDB4_H

////////////////////////////////////////////////////////////////////////////////
// EEPROM (Supported on UDB4 and UDB5 only)

void udb_eeprom_init(void);

// Write 1 byte to eeprom at address, or read 1 byte from address in eeprom into data
void eeprom_ByteWrite(uint16_t address, uint8_t data);
void eeprom_ByteRead(uint16_t address, uint8_t* data);

// Write numbytes of data to eeprom, starting at address. The write area can not span a
// page boundry.  Pages start on addresses of multiples of 64.
// Read numbytes of data from address in eeprom into data.  Note that there is no 1-page
// limit for sequential reads as there is for page writes.
void eeprom_PageWrite(uint16_t address, uint8_t* data, uint8_t numbytes);
void eeprom_SequentialRead(uint16_t address, uint8_t* data, uint16_t numbytes);

#endif // EEPROM_UDB4_H
