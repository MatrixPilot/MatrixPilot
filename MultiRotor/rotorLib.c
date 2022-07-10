#include "rotorLib.h"
#include "../libDCM/libDCM.h"
#include "../libDCM/mathlib.h"
#include "../libDCM/mathlibNAV.h"

int16_t multiply_saturate ( int16_t x , int16_t y , int16_t maximum )
{
	int32_t product , max_32 ;
	max_32 = (int32_t) maximum ;
	product = __builtin_mulss( x , y ) ;
	if (product > max_32 ) product = maximum ;
	if (product <  (-max_32 )) product =(-maximum) ;
	return ((int16_t) product) ;
}

int16_t fractional_product ( int16_t x , int16_t y )
{
	union longww long_accum ;
	long_accum.WW = __builtin_mulss( x , y ) ;
	long_accum.WW = long_accum.WW << 2 ;
	return long_accum._.W1 ;
}

//	normalization algorithm
//  TODO this is an older version of the algorithm, it will work well enough where it is used,
//  but it should eventually be updated
#define RMAX15 24576 //0b0110000000000000   // 1.5 in 2.14 format
#define RMAX 16384
void matrix_normalize(int16_t matrix[])
//	This is the routine that maintains the orthogonality of the
//	direction cosine matrix, which is expressed by the identity
//	relationship that the cosine matrix multiplied by its
//	transpose should equal the identity matrix.
//	Small adjustments are made at each time step to assure orthogonality.
{

	int norm;    // actual magnitude
	int renorm;  // renormalization factor
	int rbuff[9];
	VectorCopy( 9 , rbuff , matrix ); // copy direction cosine matrix into buffer
	
	// Leave the bottom (tilt) row alone, it is usually the most accurate.
	// Compute the first row as the cross product of second row with third row.
	VectorCross(&rbuff[0], &rbuff[3] , &rbuff[6]);
	// First row is now perpendicular to the second and third row.
	// Compute the second row as the cross product of the third row with the first row.
	VectorCross(&rbuff[3], &rbuff[6] , &rbuff[0]);
	// All three rows are now mutually perpendicular.

	// Use a Taylor's expansion for 1/sqrt(X*X) to avoid division in the renormalization

	// rescale row1
	norm = VectorPower(3, &rbuff[0]); // Scalegain of 0.5
	renorm = RMAX15 - norm;
	VectorScale(3, &rbuff[0], &rbuff[0], renorm);
	VectorAdd(3, &matrix[0], &rbuff[0], &rbuff[0]);
	// rescale row2
	norm = VectorPower(3, &rbuff[3]);
	renorm = RMAX15 - norm;
	VectorScale(3, &rbuff[3], &rbuff[3], renorm);
	VectorAdd(3, &matrix[3], &rbuff[3], &rbuff[3]);
	// rescale row3
	norm = VectorPower(3, &rbuff[6]);
	renorm = RMAX15 - norm;
	VectorScale(3, &rbuff[6], &rbuff[6], renorm);
	VectorAdd(3, &matrix[6], &rbuff[6], &rbuff[6]);
}


void MatrixRotate( int16_t matrix[] , int16_t angle[] )
{
	int rup[9] ;
	int rbuff[9] ;
	unsigned long thetaSquare ;
	unsigned nonlinearAdjust ;
	// diagonal elements of the update matrix:
	rup[0] = rup[4] = rup[8]= RMAX ;

	// compute the square of rotation

	thetaSquare = 	__builtin_mulss ( angle[0] , angle[0] ) +
					__builtin_mulss ( angle[1] , angle[1] ) +
					__builtin_mulss ( angle[2] , angle[2] ) ;

	// adjust gain by rotation_squared divided by 3

	nonlinearAdjust = RMAX + ((unsigned int ) ( thetaSquare >>14 ))/3 ;	

	angle[0] = __builtin_mulsu ( angle[0] , nonlinearAdjust )>>14 ;
	angle[1] = __builtin_mulsu ( angle[1] , nonlinearAdjust )>>14 ;
	angle[2] = __builtin_mulsu ( angle[2] , nonlinearAdjust )>>14 ;

	//	construct the off-diagonal elements of the update matrix:
	rup[1] = -angle[2] ;
	rup[2] =  angle[1] ;
	rup[3] =  angle[2] ;
	rup[5] = -angle[0] ;
	rup[6] = -angle[1] ;
	rup[7] =  angle[0] ;

	//	matrix multiply the rmatrix by the update matrix
	MatrixMultiply( 3 , 3 , 3 , rbuff , matrix , rup ) ;
	//	multiply by 2 and copy back from rbuff to rmat:
	MatrixAdd( 3 , 3 , matrix , rbuff , rbuff ) ; 
	return ;
}

extern int16_t rmat[];
extern int16_t yaw_rmat[];
void initialize_yaw_rmat(void)
{
	int16_t sine , cosine ;
	cosine = __builtin_divsd(__builtin_mulsu(rmat[0]/2 + rmat[4]/2, RMAX),RMAX/2+rmat[8]/2);
	sine = __builtin_divsd(__builtin_mulsu(rmat[1]/2 - rmat[3]/2, RMAX),RMAX/2+rmat[8]/2);
	yaw_rmat[0]=cosine;
	yaw_rmat[4]=cosine;
	yaw_rmat[1]=sine ;
	yaw_rmat[3]=-sine ;
	yaw_rmat[2]= 0 ;
	yaw_rmat[5]= 0 ;
	yaw_rmat[6]= 0 ;
	yaw_rmat[7]= 0 ;
	yaw_rmat[8]= RMAX ;
}