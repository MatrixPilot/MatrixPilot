#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

int current_orientation ;
union bfbts_word desired_behavior ;


void initBehavior( void )
{
	current_orientation = F_NORMAL ;
	desired_behavior.W = 0 ;
	
	return ;
}


void updateBehavior(void)
{
	if ( current_orientation == F_INVERTED )
	{
		current_orientation = (rmat[8] < 6000) ? F_INVERTED : F_NORMAL ;
	}
	else
	{
		current_orientation = (rmat[8] < -6000) ? F_INVERTED : F_NORMAL ;
	}
	
	if (flags._.pitch_feedback && !flags._.GPS_steering)
	{
		desired_behavior.W = current_orientation ;
	}
	
	return ;
}


