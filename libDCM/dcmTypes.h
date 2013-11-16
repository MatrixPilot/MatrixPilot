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


#ifndef DCM_TYPES_H
#define DCM_TYPES_H

#include "../libUDB/udbTypes.h"

// DCM Types
struct relative2D    { int16_t x; int16_t y; };
struct absolute2D    { int32_t x; int32_t y; };
struct relative3D    { int16_t x; int16_t y; int16_t z; };
struct relative3D_32 { int32_t x; int32_t y; int16_t z; };
struct absolute3D    { int32_t x; int32_t y; int32_t z; };
struct waypoint3D    { int32_t x; int32_t y; int16_t z; };
//struct waypoint3D    { int32_t x; int32_t y; int32_t z; };
struct fixedOrigin3D { int32_t x; int32_t y; float z; };

#endif // DCM_TYPES_H
