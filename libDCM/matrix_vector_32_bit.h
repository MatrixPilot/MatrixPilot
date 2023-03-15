/* 
 * File:   matrix_vector_32_bit.h
 * Author: bill
 *
 * Created on March 15, 2023, 1:22 PM
 */

#ifndef MATRIX_VECTOR_32_BIT_H
#define	MATRIX_VECTOR_32_BIT_H

int32_t fract_32_mpy( int32_t x , int32_t y ) ;
void cross_product_32(union longww result[], union longww vectorx[] ,union longww vectory[] ) ;
int32_t row_col_dot_fract_32( int32_t row[], int32_t col[], int16_t row_index , int16_t col_index ) ;
int32_t VectorDotProduct_32( int32_t vector1 , int32_t vector2 ) ;
void MatrixMultiply_32( int32_t dest[] , int32_t arg1[] , int32_t arg2[] ) ;

#endif	/* MATRIX_VECTOR_32_BIT_H */

