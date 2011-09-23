// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "libDCM_internal.h"

int groundVelocityHistory[3] = { 0 , 0 , 0 } ;
int fuselageDirectionHistory[3] = { 0 , 0 , 0 } ;

int estimatedWind[3] = { 0 , 0 , 0 } ;

#define MINROTATION 	( (int)( 0.2 * RMAX ) )

void estimateWind( void )
{
#if ( WIND_ESTIMATION == 1 )

	if ( dcm_flags._.skip_yaw_drift ) return ;
	
	int index ;
	int groundVelocity[3] ;
	int groundVelocitySum[3] ;
	int groundVelocityDiff[3] ;
	int fuselageDirection[3] ;
	int fuselageDirectionSum[3] ;
	int fuselageDirectionDiff[3] ;
	unsigned int magVelocityDiff ;
	unsigned int magDirectionDiff ;
	signed char angleVelocityDiff ;
	signed char angleDirectionDiff ;
	signed char thetaDiff ;
	int costhetaDiff ;
	int sinthetaDiff ;
	union longww longaccum ;
	struct relative2D xy ;
	unsigned int estimatedAirspeed ;

	groundVelocity[0] = GPSvelocity.x ;
	groundVelocity[1] = GPSvelocity.y ;
	groundVelocity[2] = GPSvelocity.z ;

	fuselageDirection[0] = -rmat[1] ;
	fuselageDirection[1] =  rmat[4] ;
	fuselageDirection[2] = -rmat[7] ;

	for ( index = 0 ; index < 3 ; index++ )
	{
		groundVelocity[index] >>= 1 ;
		fuselageDirection[index] >>= 1 ;
		groundVelocitySum[index] = groundVelocity[index] + groundVelocityHistory[index] ;
		groundVelocityDiff[index] = groundVelocity[index] - groundVelocityHistory[index] ;
		fuselageDirectionSum[index] = fuselageDirection[index] + fuselageDirectionHistory[index] ;
		fuselageDirectionDiff[index] = fuselageDirection[index] - fuselageDirectionHistory[index] ;
	}

	xy.x = fuselageDirectionDiff[0] ;
	xy.y = fuselageDirectionDiff[1] ;
	angleDirectionDiff = rect_to_polar( &xy) ;

	xy.x = groundVelocityDiff[0] ;
	xy.y = groundVelocityDiff[1] ;
	angleVelocityDiff = rect_to_polar( &xy ) ;

	thetaDiff = angleVelocityDiff - angleDirectionDiff ;
	costhetaDiff = cosine(thetaDiff) ;
	sinthetaDiff = sine(thetaDiff) ;

	magDirectionDiff = vector3_mag( 
						fuselageDirectionDiff[0] , 
						fuselageDirectionDiff[1] ,
						fuselageDirectionDiff[2] ) ;

	magVelocityDiff = vector3_mag( 
						groundVelocityDiff[0] , 
						groundVelocityDiff[1] ,
						groundVelocityDiff[2] ) ;

	if ( magDirectionDiff > MINROTATION )
	{
		longaccum._.W1 = magVelocityDiff >> 2 ;
		longaccum._.W0 = 0 ;
		estimatedAirspeed = __builtin_divud( longaccum.WW , magDirectionDiff ) ;

		longaccum.WW = (		__builtin_mulss( costhetaDiff , fuselageDirectionSum[0] ) 
						-	__builtin_mulss( sinthetaDiff , fuselageDirectionSum[1] )) << 2 ;
		longaccum.WW =  ( __builtin_mulus( estimatedAirspeed , longaccum._.W1 ) ) << 2 ;
		estimatedWind[0] = estimatedWind[0] + 
							( ( groundVelocitySum[0] - longaccum._.W1 - estimatedWind[0] ) >> 4 ) ;

		longaccum.WW = (		__builtin_mulss( sinthetaDiff , fuselageDirectionSum[0] ) 
						+	__builtin_mulss( costhetaDiff , fuselageDirectionSum[1] )) << 2 ;
		longaccum.WW =  ( __builtin_mulus( estimatedAirspeed , longaccum._.W1 ) ) << 2 ;
		estimatedWind[1] = estimatedWind[1] +
						  ( ( groundVelocitySum[1] - longaccum._.W1 - estimatedWind[1] ) >> 4 ) ;

		for ( index = 0 ; index < 3 ; index++ )
		{
			groundVelocityHistory[index] = groundVelocity[index] ;
			fuselageDirectionHistory[index] = fuselageDirection[index] ;
		}
	}	
#endif
	return ;	
}
