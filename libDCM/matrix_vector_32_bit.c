

//#include "libUDB.h"
//#include "oscillator.h"
//#include "interrupt.h"
//#include "heartbeat.h"
#include "../libUDB/udbTypes.h"
#include "../libDCM/matrix_vector_32_bit.h"


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

void VectorCross_32(union longww result[], union longww vectorx[] ,union longww vectory[] )
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

int32_t row_col_dot_fract_32( union longww row[], union longww col[], int16_t row_index , int16_t col_index )
{
	// primitive operation used in matrix multiply
	// it computes the dot product of one row and one column, divided by 2^32
	// the rows and columns are embedded in a linear array of 9 elements
	int32_t result ;
	result = fract_32_mpy(row[row_index].WW,col[col_index].WW)
			+fract_32_mpy(row[row_index+1].WW,col[col_index+3].WW)
			+fract_32_mpy(row[row_index+2].WW,col[col_index+6].WW) ;
	return result ;
}



void MatrixMultiply_32( union longww dest[] , union longww arg1[] , union longww arg2[] )
{
	// computes the matrix product of two 3X3 matrices, arg1 times arg2,
	// divides by 2^32 and places the result in dest
	// for fractional computations, the result must eventually be multiplied by 4
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

int32_t VectorDotProduct_32( union longww vector1 , union longww vector2 )
{
	return 0 ;
}

void MatrixAdd_32(union longww result[], union longww vectorx[] ,union longww vectory[] )
{
	
}

void VectorCopy_32(int16_t size , union longww dest[] , union longww source[] )
{
	
}

void VectorScale_32(int16_t size , union longww dest[] , union longww source[] , int32_t scale )
{
	
}

void VectorAdd_32(union longww result[], union longww vectorx[] ,union longww vectory[] )
{
	
}

int32_t VectorPower_32(union longww source[])
{
	return 0 ;
}

