/* 
 * File:   matrix_vector_32_bit.h
 * Author: bill
 *
 * Created on March 15, 2023, 1:22 PM
 */

#ifndef MATRIX_VECTOR_32_BIT_H
#define	MATRIX_VECTOR_32_BIT_H

void scale_32_by_4 ( int16_t size , union longww result[] ) ;
int32_t fract_32_mpy( int32_t x , int32_t y ) ;
void VectorCross_32(union longww result[], union longww vectorx[] ,union longww vectory[] ) ;
int32_t row_col_dot_fract_32( union longww row[], union longww col[], int16_t row_index , int16_t col_index ) ;
int32_t VectorDotProduct_32( union longww vector1[] , union longww vector2[] ) ;
void MatrixMultiply_32( union longww dest[] , union longww arg1[] , union longww arg2[] ) ;
void MatrixAdd_32(union longww result[], union longww vectorx[] ,union longww vectory[] ) ;
void VectorCopy_32(int16_t size , union longww dest[] , union longww source[] ) ;
void VectorScale_32(int16_t size , union longww dest[] , union longww source[] , int32_t scale );
void VectorAdd_32(int16_t size , union longww result[], union longww vectorx[] ,union longww vectory[] ) ;
int32_t VectorPower_32(int16_t size , union longww source[]) ;

#endif	/* MATRIX_VECTOR_32_BIT_H */

