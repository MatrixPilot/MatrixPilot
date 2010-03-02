#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

int current_orientation ;
union bfbts_word desired_behavior ;

int cyclesUntilStartTriggerAction = 0 ;
int cyclesUntilStopTriggerAction = 0 ;
boolean currentTriggerActionValue = 0 ;

void triggerActionSetValue( boolean newValue ) ;



void init_behavior( void )
{
	current_orientation = F_NORMAL ;
	desired_behavior.W = current_orientation ;
	
	setBehavior( current_orientation ) ;
	
	if ( TRIGGER_TYPE != TRIGGER_TYPE_NONE )
	{
		triggerActionSetValue( TRIGGER_ACTION != TRIGGER_PULSE_HIGH ) ;
	}
	
	return ;
}


void setBehavior(int newBehavior)
{
	desired_behavior.W = newBehavior ;
	
	if ( desired_behavior.W & F_TRIGGER )
	{
		if ( cyclesUntilStartTriggerAction == 0 )
		{
			cyclesUntilStartTriggerAction = 1 ;
		}
	}
	else
	{
		cyclesUntilStartTriggerAction = 0 ;
	}
	
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
	
	if ( !flags._.radio_on )
	{
		setBehavior( F_NORMAL ) ;
	}
	
	return ;
}


// This function is called every 25ms
void updateTriggerAction( void )
{
	if ( cyclesUntilStopTriggerAction == 1 )
	{
		triggerActionSetValue( TRIGGER_ACTION != TRIGGER_PULSE_HIGH ) ;
		cyclesUntilStopTriggerAction = 0 ;
	}
	else if ( cyclesUntilStopTriggerAction > 0 )
	{
		cyclesUntilStopTriggerAction-- ;
	}
	
	if ( cyclesUntilStartTriggerAction == 1 && ( desired_behavior.W & F_TRIGGER ) )
	{
		if ( TRIGGER_ACTION == TRIGGER_PULSE_HIGH || TRIGGER_ACTION == TRIGGER_PULSE_LOW )
		{
			triggerActionSetValue( TRIGGER_ACTION == TRIGGER_PULSE_HIGH ) ;
			
			cyclesUntilStopTriggerAction = TRIGGER_PULSE_DURATION / (long)25 ;
			cyclesUntilStartTriggerAction = 0 ;
		}
		else if ( TRIGGER_ACTION == TRIGGER_TOGGLE )
		{
			triggerActionSetValue( !currentTriggerActionValue ) ;
			
			cyclesUntilStopTriggerAction = 0 ;
			cyclesUntilStartTriggerAction = 0 ;
		}
		else if ( TRIGGER_ACTION == TRIGGER_REPEATING )
		{
			triggerActionSetValue( TRIGGER_ACTION == TRIGGER_PULSE_HIGH ) ;
			
			cyclesUntilStopTriggerAction = TRIGGER_PULSE_DURATION / (long)25 ;
			cyclesUntilStartTriggerAction = TRIGGER_REPEAT_PERIOD / (long)25 ;
		}
	}
	else if ( cyclesUntilStartTriggerAction > 0 )
	{
		cyclesUntilStartTriggerAction-- ;
	}
	
	return ;
}


void triggerActionSetValue( boolean newValue )
{
	if ( TRIGGER_TYPE == TRIGGER_TYPE_SERVO )
	{
		pwOut[TRIGGER_OUTPUT_CHANNEL] = (newValue) ? TRIGGER_SERVO_HIGH : TRIGGER_SERVO_LOW ;
	}
	else if ( TRIGGER_TYPE == TRIGGER_TYPE_DIGITAL )
	{
		if ( NUM_OUTPUTS < 6 )
		{
			LATEbits.LATE4 = newValue ;
		}
	}
	
	currentTriggerActionValue = newValue ;
	
	return ;
}
