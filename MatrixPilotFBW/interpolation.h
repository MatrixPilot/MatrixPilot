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


#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <libq.h>

// Succesive interpolation between X inputs of Y outputs
int16_t successive_interpolation(int16_t X, int16_t X1, int16_t X2, int16_t Y1, int16_t Y2);
_Q16 successive_interpolation_Q16(_Q16 X, _Q16 X1, _Q16 X2, _Q16 Y1, _Q16 Y2);

#endif  // INTERPOLATION_H

