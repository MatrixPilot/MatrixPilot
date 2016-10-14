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


#ifndef MAGNETOMETER_H
#define MAGNETOMETER_H


extern int16_t udb_magFieldBody[];  // magnetic field in the body frame of reference 
extern int16_t udb_magOffset[];     // magnetic offset in the body frame of reference
extern int16_t magGain[];           // magnetometer calibration gains
extern int16_t rawMagCalib[];
extern int16_t magFieldRaw[];
extern int16_t magMessage;          // message type
extern int16_t I2messages;

typedef void (*magnetometer_callback_funcptr)(void);

void rxMagnetometer(magnetometer_callback_funcptr); // service the magnetometer
void HILSIM_MagData(magnetometer_callback_funcptr);


#endif // MAGNETOMETER_H
