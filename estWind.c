#include "p30f4011.h"
#include "defines.h"
#include <dsp.h>
#include "optionsRmat.h"
#include "definesRmat.h"

int groundVelocityHistory[3] = { 0 , 0 , 0 } ;
int fuselageDirectionHistory[3] = { 0 , 0 , 0 } ;

unsigned int estimatedAirspeed = 0 ;
int estimatedWind[3] = { 0 , 0 , 0 } ;

#define MINROTATION 	( (int)( 0.2 * RMAX ) )


void estimateWind( void )
{
#if ( WIND_ESTIMATION == 1 )

	// Don't update wind estimation while hovering, since it doesn't work right yet
	if ( current_orientation == F_HOVER ) return ;
	
	int index ;
	int groundVelocity[3] ;
	int groundVelocitySum[3] ;
	int groundVelocityDiff[3] ;
	int fuselageDirection[3] ;
	int fuselageDirectionSum[3] ;
	int fuselageDirectionDiff[3] ;
	unsigned int magVelocityDiff ;
	unsigned int magDirectionDiff ;
	unsigned int magHorizontalVelocityDiff ;
	unsigned int magHorizontalDirectionDiff ;
	signed char angleVelocityDiff ;
	signed char angleDirectionDiff ;
	signed char thetaDiff ;
	int costhetaDiff ;
	int sinthetaDiff ;
	union longww longaccum ;
	struct relative2D xy ;

	groundVelocity[0] = IMUvelocityx._.W1 ;
	groundVelocity[1] = IMUvelocityy._.W1 ;
	groundVelocity[2] = IMUvelocityz._.W1 ;

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
	magHorizontalDirectionDiff = xy.x ;

	xy.x = groundVelocityDiff[0] ;
	xy.y = groundVelocityDiff[1] ;
	angleVelocityDiff = rect_to_polar( &xy ) ;
	magHorizontalVelocityDiff = xy.x ;

	thetaDiff = angleVelocityDiff - angleDirectionDiff ;
	costhetaDiff = cosine(thetaDiff) ;
	sinthetaDiff = sine(thetaDiff) ;

	xy.x = magHorizontalDirectionDiff ;
	xy.y = fuselageDirectionDiff[2] ;
	rect_to_polar( &xy ) ;
	magDirectionDiff = xy.x ;

	xy.x = magHorizontalVelocityDiff ;
	xy.y = groundVelocityDiff[2] ;
	rect_to_polar( &xy ) ;
	magVelocityDiff = xy.x ;

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

		longaccum.WW = ( __builtin_mulus( estimatedAirspeed , fuselageDirectionSum[2] ) ) << 2 ;
		estimatedWind[2] = groundVelocitySum[2] - longaccum._.W1 ;

		for ( index = 0 ; index < 3 ; index++ )
		{
			groundVelocityHistory[index] = groundVelocity[index] ;
			fuselageDirectionHistory[index] = fuselageDirection[index] ;
		}

	}	
#endif
	return ;	
}
