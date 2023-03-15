

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

void cross_product_32(union longww result[], union longww vectorx[] ,union longww vectory[] )
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

int32_t VectorDotProduct_32( int32_t vector1 , int32_t vector2 )
{
	return 0 ;
}

int32_t row_col_dot_fract_32( int32_t row[], int32_t col[], int16_t row_index , int16_t col_index )
{
	// primitive operation used in matrix multiply
	// it computes the dot product of one row and one column, divided by 2^32
	// the rows and columns are embedded in a linear array of 9 elements
	int32_t result ;
	result = fract_32_mpy(row[row_index],col[col_index])
			+fract_32_mpy(row[row_index+1],col[col_index+3])
			+fract_32_mpy(row[row_index+2],col[col_index+6]) ;
	return result ;
}



void MatrixMultiply_32( int32_t dest[] , int32_t arg1[] , int32_t arg2[] )
{
	// computes the matrix product of two 3X3 matrices, arg1 times arg2,
	// divides by 2^32 and places the result in dest
	// for fractional computations, the result must eventually be multiplied by 4
	dest[0] = row_col_dot_fract_32(arg1,arg2,0,0);
	dest[1] = row_col_dot_fract_32(arg1,arg2,0,1);
	dest[2] = row_col_dot_fract_32(arg1,arg2,0,2);
	dest[3] = row_col_dot_fract_32(arg1,arg2,3,0);
	dest[4] = row_col_dot_fract_32(arg1,arg2,3,1);
	dest[5] = row_col_dot_fract_32(arg1,arg2,3,2);
	dest[6] = row_col_dot_fract_32(arg1,arg2,6,0);
	dest[7] = row_col_dot_fract_32(arg1,arg2,6,1);
	dest[8] = row_col_dot_fract_32(arg1,arg2,6,2);
}

