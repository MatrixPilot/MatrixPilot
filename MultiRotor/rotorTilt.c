
#include "../libDCM/libDCM.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/rmat.h"
#include "../libDCM/mathlib.h"
#include "../libDCM/mathlibNAV.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/ADchannel.h"
#include "rotorLib.h"

// Used for serial debug output
#include <stdio.h>

int commanded_tilt[3] ;

void compute_tilt_rmat( int tilt_mat[] , int roll , int pitch )
{
	int Z ;
	int one_minus_Z ;
	int nx , ny ;
	commanded_tilt[0] = pitch ;
	commanded_tilt[1] = roll ;
	commanded_tilt[2] = RMAX ;
	vector3_normalize( commanded_tilt , commanded_tilt ) ;
	tilt_mat[2] = commanded_tilt[1] ;
	tilt_mat[5] = - commanded_tilt[0] ;
	tilt_mat[6] = -commanded_tilt[1] ;
	tilt_mat[7] = commanded_tilt[0] ;
	tilt_mat[8] = commanded_tilt[2] ;
	Z = commanded_tilt[2] ;
	one_minus_Z = RMAX - Z ;
	commanded_tilt[2] = 0 ;
	vector3_normalize( commanded_tilt , commanded_tilt ) ;
	nx = commanded_tilt[0] ;
	ny = commanded_tilt[1] ;
	tilt_mat[0] = fractional_product( Z , fractional_product ( ny , ny ))
				+ fractional_product( nx , nx ) ;
	tilt_mat[4] = fractional_product( Z , fractional_product ( nx , nx ))
				+ fractional_product( ny , ny ) ;
	tilt_mat[1] = fractional_product( one_minus_Z , fractional_product ( nx , ny )) ;
	tilt_mat[3] = tilt_mat[1] ;
}

/*

		commanded_roll = commanded_tilt[0] ;
		commanded_pitch = commanded_tilt[1] ;

		// build the commanded tilt matrix from commanded tilt vector
		tilt_rmat[6] = commanded_tilt[0] ;
		tilt_rmat[7] = commanded_tilt[1] ;
		tilt_rmat[8] = commanded_tilt[2] ;
		tilt_rmat[0] = commanded_tilt[2] ;
		tilt_rmat[1] = 0 ;
		tilt_rmat[2] = -commanded_tilt[0] ;
		tilt_rmat[4] = vector3_normalize(tilt_rmat , tilt_rmat ) ;
		long_accum.WW = __builtin_mulss( tilt_rmat[2], tilt_rmat[7] ) ;
		long_accum.WW = long_accum.WW << 2 ;
		tilt_rmat[3] = long_accum._.W1 ;
		long_accum.WW = __builtin_mulss( tilt_rmat[0], -tilt_rmat[7] ) ;
		long_accum.WW = long_accum.WW << 2 ;
		tilt_rmat[5] = long_accum._.W1 ;

		roll_error = -( correction_matrix[6]- correction_matrix[2])/2 ;
		pitch_error = -( correction_matrix[5]- correction_matrix[7])/2 ;
		yaw_error = -( correction_matrix[1]- correction_matrix[3])/2 ;

*/
