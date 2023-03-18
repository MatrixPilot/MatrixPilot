

//#include "libUDB.h"
//#include "oscillator.h"
//#include "interrupt.h"
//#include "heartbeat.h"
#include "../libUDB/udbTypes.h"
#include "../libDCM/matrix_vector_32_bit.h"

// 32 bit vector and matrix routines.
// Scaling of integers into real values is an extension of the values used
// in the 16 bit routines that had an unused "guard" bit to prevent integer overflow.
// In the 16 bit routines, 0x4000 represented a real value of 1, such as 1 radian
// In the 32 bit routines, 0x40000000 represents a real value of 1.
// As a consequence of the representation, multiplication of two 32 bit values is achieved by
// starting with a 32 by 32 bit integer multiply producing a 64 bit result. The final result is then
// produced by left shifting the 64 bit value by 2 and taking the most significant 32 bits

void convert_16_bit_to_32_bit(int16_t size , union longww dest[] , int16_t source[])
{
	int16_t index ;
	for ( index = 0 ; index < size ; index++ )
	{
		dest[index]._.W1 = source[index] ;
		dest[index]._.W0 = 0 ;
	}
}

void convert_32_bit_to_16_bit(int16_t size , int16_t dest[] , union longww source[])
{
	int16_t index ;
	union longww temporary ;
	for ( index = 0 ; index < size ; index++ )
	{
		// rounding rather than truncation
		temporary.WW = source[index].WW + 0x00008000 ;
		dest[index] = temporary._.W1 ;
	}	
}

// This routine can be used after any multiplication to produce the left shift by 2.
// It can be used on vectors or matrices that are stored as arrays by specifying 3
// for a vector and 9 for a matrix.
void scale_32_by_4 ( int16_t size , union longww result[] )
{
	int16_t index ;
	for ( index = 0 ; index < size ; index++ )
	{
		result[index].WW = ((result[index].WW)<<2) ;
	}
}

// returns the upper 32 bits of a 32 bit by 32 bit integer multiplication
int32_t fract_32_mpy( int32_t x , int32_t y )
{
	// computes x*y/(2^32), which is useful in fractional calculations
	// in 32 bit DCM calculations in which 1 radian equals (2^14)*(2^16),
	// the result must be eventually multiplied by 4 by the calling routine
	union longww result ;
	union longww arg1 , arg2 ;
	arg1.WW = x ;
	arg2.WW = y ;
	int16_t sign = 1 ;
	if ( arg1.WW < 0 )
	{
		sign = - sign ;
		arg1.WW = - (arg1.WW ) ;
	}
	if ( arg2.WW < 0)
	{
		sign = - sign ;
		arg2.WW = - (arg2.WW ) ;
	}	
	result.WW = __builtin_muluu( arg1._.W1 , arg2._.W1 )
			+  (__builtin_muluu( arg1._.W1 , arg2._.W0 )>>16 )
			+  (__builtin_muluu( arg1._.W0 , arg2._.W1 )>>16 ) ;
	
	if ( sign < 0)
	{
		result.WW = - ( result.WW ) ;
	}
	return result.WW ;
}

// Performs an integer cross product.
// If the arguments are in 0x40000000 format, the result needs to be left shifted by 2
void VectorCross_32(union longww result[] , union longww vectorx[] , union longww vectory[] )
{
	// computes the cross product of vectorx and vectory, divides by 2^32,
	// and places the result in result
	result[0].WW = ( fract_32_mpy(vectorx[1].WW,vectory[2].WW)
			- fract_32_mpy(vectorx[2].WW,vectory[1].WW));

	result[1].WW = ( fract_32_mpy(vectorx[2].WW,vectory[0].WW)
			- fract_32_mpy(vectorx[0].WW,vectory[2].WW));

	result[2].WW = ( fract_32_mpy(vectorx[0].WW,vectory[1].WW)
			- fract_32_mpy(vectorx[1].WW,vectory[0].WW));
}

// Performs a dot product of a row one matrix with the column of another matrix, including the left shift
int32_t row_col_dot_fract_32( union longww row[] , union longww col[] , int16_t row_index , int16_t col_index )
{
	// primitive operation used in fractional matrix multiply
	// it computes the dot product of one row and one column, divided by 2^32, multiplied by 4
	// the rows and columns are embedded in a linear array of 9 elements
	int32_t result ;
	result = fract_32_mpy(row[row_index].WW,col[col_index].WW)
			+fract_32_mpy(row[row_index+1].WW,col[col_index+3].WW)
			+fract_32_mpy(row[row_index+2].WW,col[col_index+6].WW) ;
	return (result<<2) ;
}

void MatrixMultiply_32( union longww dest[] , union longww arg1[] , union longww arg2[] )
{
	// computes the matrix product of two 3X3 matrices, arg1 times arg2,
	// divides by 2^32 and places the result in dest
	// it assumes the matrices are scaled such that 1 radian = 0x40000000,
	// so the dot product includes a multiply by 4
	dest[0].WW = row_col_dot_fract_32(arg1,arg2,0,0);
	dest[1].WW = row_col_dot_fract_32(arg1,arg2,0,1);
	dest[2].WW = row_col_dot_fract_32(arg1,arg2,0,2);
	dest[3].WW = row_col_dot_fract_32(arg1,arg2,3,0);
	dest[4].WW = row_col_dot_fract_32(arg1,arg2,3,1);
	dest[5].WW = row_col_dot_fract_32(arg1,arg2,3,2);
	dest[6].WW = row_col_dot_fract_32(arg1,arg2,6,0);
	dest[7].WW = row_col_dot_fract_32(arg1,arg2,6,1);
	dest[8].WW = row_col_dot_fract_32(arg1,arg2,6,2);
}

// Performs an integer dot product of two three-component, 32 bit vectors
int32_t VectorDotProduct_32( union longww vector1[] , union longww vector2[] )
{
	int32_t result = 0 ;
	int16_t index ;
	for ( index = 0 ; index < 3 ; index++ )
	{
		result +=  fract_32_mpy(vector1[index].WW , vector2[index].WW ) ;
	}
	return result ;
}

// Adds two 3X3 32 bit matrices
void MatrixAdd_32(union longww result[] , union longww vectorx[] , union longww vectory[] )
{
	int16_t index ;
	for ( index = 0 ; index < 9 ; index++ )
	{
		result[index].WW = vectorx[index].WW + vectory[index].WW ;
	}
}

// Copies a 32 bit vector of any size
void VectorCopy_32(int16_t size , union longww dest[] , union longww source[] )
{
	int16_t index ;
	for ( index = 0 ; index < size ; index++ )
	{
		dest[index].WW = source[index].WW ;
	}
}

// Scales a 32 bit vector of any size. It does not do the left shift.
void VectorScale_32(int16_t size, union longww dest[] , union longww source[] , int32_t scale )
{
	int16_t index ;
	for ( index = 0 ; index < size ; index++ )
	{
		dest[index].WW = fract_32_mpy(source[index].WW,scale) ;
	}
}

// Adds two 32 bit vectors of any size
void VectorAdd_32(int16_t size, union longww result[] , union longww vectorx[] , union longww vectory[] )
{
	int16_t index ;
	for ( index = 0 ; index < size ; index++ )
	{
		result[index].WW = vectorx[index].WW + vectory[index].WW ;
	}
}

//	Computes the integer square (power) of a 32 bit vector, any size, not including a left shift
int32_t VectorPower_32(int16_t size , union longww source[])
{
	int32_t result = 0 ;
	int16_t index ;
	for ( index = 0 ; index < size ; index++ )
	{
		result +=  fract_32_mpy(source[index].WW , source[index].WW ) ;
	}
	return result ;
}

