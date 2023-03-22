#include "../libDCM/matrix_vector_32_bit.h"
#include "../libUDB/udbTypes.h"

extern union longww theta_32[];

int32_t theta_square ;
union longww theta_cross[] = {{0},{0},{0},{0},{0},{0},{0},{0},{0}};
union longww theta_cross_theta_cross[9];
int32_t f1,f2 ;
int32_t txtx, txty, txtz, tyty, tytz, tztz ;
union longww rupdate_32[9] ;
int16_t rupdate_16[9];
union longww rmat_32[] = {{0x40000000},{0},{0},{0},{0x40000000},{0},{0},{0},{0x40000000}};
int16_t rmat_16[] = {0x4000, 0 , 0 , 0 , 0x4000 , 0 , 0 , 0 , 0x4000 } ;

extern int16_t accelOn ;
extern int16_t rmat[];

void normalize_32(void);

void rmat_32_update(void)
{
	if (accelOn == 1 )
	{
		convert_16_bit_to_32_bit(9,rmat_32,rmat) ;
	}
	else
	{
		theta_square = ((VectorPower_32(3,theta_32))<<2);
		f1 = (fract_32_mpy(theta_square,(0x40000000/120))<<2) ;
		f1 = f1 - (0x40000000/6) ;
		f1 = ((fract_32_mpy(f1,theta_square))<<2) ;
		f1 = f1 + 0x40000000 ;

		f2 = 0x20000000 - (fract_32_mpy(theta_square,(0x40000000/24))<<2) ;
	
		theta_cross[7].WW =(fract_32_mpy(f1,theta_32[0].WW)<<2 );
		theta_cross[2].WW =(fract_32_mpy(f1,theta_32[1].WW)<<2 );
		theta_cross[3].WW =(fract_32_mpy(f1,theta_32[2].WW)<<2 );
		theta_cross[1].WW = - theta_cross[3].WW ;
		theta_cross[5].WW = - theta_cross[7].WW ;
		theta_cross[6].WW = - theta_cross[2].WW ;
	
		txtx = (fract_32_mpy(theta_32[0].WW,theta_32[0].WW)<<2 );
		txty = (fract_32_mpy(theta_32[0].WW,theta_32[1].WW)<<2 );
		txtz = (fract_32_mpy(theta_32[0].WW,theta_32[2].WW)<<2 );
		tyty = (fract_32_mpy(theta_32[1].WW,theta_32[1].WW)<<2 );
		tytz = (fract_32_mpy(theta_32[1].WW,theta_32[2].WW)<<2 );
		tztz = (fract_32_mpy(theta_32[2].WW,theta_32[2].WW)<<2 );
	
		theta_cross_theta_cross[0].WW = txtx - theta_square ;
		theta_cross_theta_cross[1].WW = txty ;
		theta_cross_theta_cross[2].WW = txtz ;
		theta_cross_theta_cross[3].WW = txty ;
		theta_cross_theta_cross[4].WW = tyty - theta_square ;
		theta_cross_theta_cross[5].WW = tytz ;
		theta_cross_theta_cross[6].WW = txtz ; 
		theta_cross_theta_cross[7].WW = tytz ;
		theta_cross_theta_cross[8].WW = tztz - theta_square ;
	
		VectorScale_32(9,theta_cross_theta_cross,theta_cross_theta_cross,f2) ;
		scale_32_by_4(9,theta_cross_theta_cross);
	
		MatrixAdd_32(rupdate_32,theta_cross,theta_cross_theta_cross);
	
		rupdate_32[0].WW += 0x40000000 ;
		rupdate_32[4].WW += 0x40000000 ;
		rupdate_32[8].WW += 0x40000000 ;
		
		convert_32_bit_to_16_bit(9 , rupdate_16 , rupdate_32) ;
		
		MatrixMultiply_32(rmat_32,rmat_32,rupdate_32);
		
		normalize_32();
		
		convert_32_bit_to_16_bit(9,rmat_16,rmat_32) ;
	}
}

int32_t norm_32 ;
int32_t renorm_32 ;
int32_t error_32 ;
union longww rbuff_32[9];
int32_t renorm_32_row_3 ;

void normalize_32(void)
{
		//  This is the routine that maintains the orthogonality of the
	//  direction cosine matrix, which is expressed by the identity
	//  relationship that the cosine matrix multiplied by its
	//  transpose should equal the identity matrix.
	//  Small adjustments are made at each time step to assure orthogonality.

	// take the tilt row without any adjustments
	VectorCopy_32( 3 , &rbuff_32[6] , &rmat_32[6] ) ;
	// compute the negative of the dot product between rows 2 and 3
	error_32 =  - 4*VectorDotProduct_32(&rmat_32[3], &rmat_32[6]);
	// compute adjustment to row 2 that will make it more perpendicular to row 3
	VectorScale_32( 3 , &rbuff_32[3] , &rbuff_32[6] , error_32 ) ;
	VectorAdd_32( 3, &rbuff_32[3] , &rbuff_32[3] , &rmat_32[3] ) ;
	// use the cross product of row 2 and 3 to get the first row
	VectorCross_32(&rbuff_32[0] , &rbuff_32[3] , &rbuff_32[6] ) ;
	scale_32_by_4(3,rbuff_32) ;

	// Use a Taylor's expansion for 1/sqrt(X*X) to avoid division in the renormalization

	// rescale row1
	norm_32 = 2*VectorPower_32(3, &rbuff_32[0]); // Scalegain of 0.25
	renorm_32 = 0x60000000 - norm_32;
	VectorScale_32(3, &rbuff_32[0], &rbuff_32[0], renorm_32);
	scale_32_by_4(3,&rbuff_32[0]);
	VectorCopy_32(3,&rmat_32[0],&rbuff_32[0]);
	// rescale row2
	norm_32 = 2*VectorPower_32(3, &rbuff_32[3]); // Scalegain of 0.25
	renorm_32 = 0x60000000 - norm_32;
	VectorScale_32(3, &rbuff_32[3], &rbuff_32[3], renorm_32);
	scale_32_by_4(3,&rbuff_32[3]);
	VectorCopy_32(3,&rmat_32[3],&rbuff_32[3]);
	// rescale row3
	norm_32 = 2*VectorPower_32(3, &rbuff_32[6]); // Scalegain of 0.25
	renorm_32 = 0x60000000 - norm_32;
	renorm_32_row_3 = renorm_32 ; // for debugging purposes
	VectorScale_32(3, &rbuff_32[6], &rbuff_32[6], renorm_32);
	scale_32_by_4(3,&rbuff_32[6]);
	VectorCopy_32(3,&rmat_32[6],&rbuff_32[6]);
}
