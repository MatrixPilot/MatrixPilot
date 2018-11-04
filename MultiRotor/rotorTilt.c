
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
	int one_plus_Z ;
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
	one_plus_Z = RMAX + Z ;
	if ( one_plus_Z > 0 )
	{
		tilt_mat[0] = Z+__builtin_divsd( __builtin_mulss( commanded_tilt[0], commanded_tilt[0]),one_plus_Z );
		tilt_mat[4] = Z+__builtin_divsd( __builtin_mulss( commanded_tilt[1], commanded_tilt[1]),one_plus_Z );
		tilt_mat[1] = __builtin_divsd( __builtin_mulss( commanded_tilt[0], commanded_tilt[1]),one_plus_Z );
		tilt_mat[3] = tilt_mat[1];
	}
	else
	{
		// this case cannot happen right now, but we may eventually want to control inverted
		tilt_mat[0] = Z ;
		tilt_mat[4] = Z ;
		tilt_mat[1] = 0 ;
		tilt_mat[3] = 0 ;
	}
}
	
