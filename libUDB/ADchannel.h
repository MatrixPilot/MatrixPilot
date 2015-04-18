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


#ifndef AD_CHANNEL_H
#define AD_CHANNEL_H


struct ADchannel {
	int16_t input;  // raw input
	int16_t value;  // average of the sum of inputs between report outs
	int16_t offset; // baseline at power up 
	int32_t sum;    // used as an integrator
}; // variables for processing an AD channel


extern struct ADchannel udb_xaccel;
extern struct ADchannel udb_yaccel;
extern struct ADchannel udb_zaccel;
extern struct ADchannel udb_xrate;
extern struct ADchannel udb_yrate;
extern struct ADchannel udb_zrate;

extern struct ADchannel udb_vref;

extern struct ADchannel udb_analogInputs[];

void udb_a2d_record_offsets(void);


#endif // AD_CHANNEL_H
