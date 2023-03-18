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


void rmat_32_update(void)
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
}

