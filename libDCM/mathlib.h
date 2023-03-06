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


void vect2_16x16_rotate(vect2_16t* vector, const vect2_16t* rotate);
void vect2_32x16_rotate(vect2_32t* vector, const vect2_16t* rotate);
void vect2_16phi_rotate(vect2_16t* vector, int8_t angle);

int8_t vect2_polar(polar_16t* polar, const vect2_16t* vector);
int16_t vect2_polar_16(polar_32t* polar, const vect2_16t* vector);

//uint16_t vect2_polar_16(polar_16t* polar, const vect2_16t* vector); // often we only want the angle result
//uint16_t vect2_polar_32(polar_32t* polar, const vect2_16t* vector); // ?

uint16_t vect2_16_mag(const vect2_16t* vector);
uint16_t vect3_16_mag(const vect3_16t* vector);
uint16_t vect2_16_norm(vect2_16t* result, const vect2_16t* vector);
uint16_t vect3_16_norm(vect3_16t* result, const vect3_16t* vector);

int16_t add_saturate(int16_t x , int16_t y , int16_t margin) ;
int16_t subtract_saturate(int16_t x , int16_t y , int16_t margin) ;

