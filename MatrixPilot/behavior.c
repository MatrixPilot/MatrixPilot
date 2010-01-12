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
		if ( STABILIZE_HOVERING && rmat[7] < -14000 )
		{
			current_orientation = F_HOVER ;
		}
		else if ( STABILIZE_INVERTED_FLIGHT && rmat[8] < 6000 )
		{
			current_orientation = F_INVERTED ;
		}
		else
		{
			current_orientation = F_NORMAL ;
		}
	}
	else if ( current_orientation == F_HOVER )
	{
		if ( STABILIZE_HOVERING && rmat[7] < -8000 )
		{
			current_orientation = F_HOVER ;
		}
		else if ( STABILIZE_INVERTED_FLIGHT && rmat[8] < -6000 )
		{
			current_orientation = F_INVERTED ;
		}
		else
		{
			current_orientation = F_NORMAL ;
		}
	}
	else
	{
		if ( STABILIZE_INVERTED_FLIGHT && rmat[8] < -6000 )
		{
			current_orientation = F_INVERTED ;
		}
		else if ( STABILIZE_HOVERING && rmat[7] < -14000 )
		{
			current_orientation = F_HOVER ;
		}
		else
		{
			current_orientation = F_NORMAL ;
		}
	}
	
	if (flags._.pitch_feedback && !flags._.GPS_steering)
	{
		desired_behavior.W = current_orientation ;
	}
	
	return ;
}

