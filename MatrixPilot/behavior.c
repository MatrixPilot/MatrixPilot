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


#include "defines.h"

int16_t current_orientation ;
union bfbts_word desired_behavior ;

int16_t cyclesUntilStartTriggerAction = 0 ;
int16_t cyclesUntilStopTriggerAction = 0 ;
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


void setBehavior(int16_t newBehavior)
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


// Limit inverted flight to specific modes
boolean canStabilizeInverted(void)
{
	switch( get_flightmode())
	{
	case FLIGHT_MODE_MANUAL: 
	case FLIGHT_MODE_STABILIZED:
	case FLIGHT_MODE_NO_RADIO: 
		return false;
	case FLIGHT_MODE_ASSISTED: 
		return INVERTED_FLIGHT_STABILIZED_MODE;
		break;
	case FLIGHT_MODE_AUTONOMOUS:
		return INVERTED_FLIGHT_WAYPOINT_MODE;
		break;
	}
	return false;
}

// Limit hovering flight to specific modes
boolean canStabilizeHover(void)
{
	switch( get_flightmode())
	{
	case FLIGHT_MODE_MANUAL: 
	case FLIGHT_MODE_STABILIZED:
	case FLIGHT_MODE_NO_RADIO: 
		return false;
	case FLIGHT_MODE_ASSISTED: 
		return HOVERING_STABILIZED_MODE;
		break;
	case FLIGHT_MODE_AUTONOMOUS:
		return HOVERING_WAYPOINT_MODE;
		break;
	}

	return false;
}


void updateBehavior(void)
{
	if ( current_orientation == F_INVERTED )
	{
		if ( canStabilizeHover() && rmat[7] < -14000 )
		{
			current_orientation = F_HOVER ;
		}
		else if ( canStabilizeInverted() && rmat[8] < 6000 )
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
		if ( canStabilizeHover() && rmat[7] < -8000 )
		{
			current_orientation = F_HOVER ;
		}
		else if ( canStabilizeInverted() && rmat[8] < -6000 )
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
		if ( canStabilizeInverted() && rmat[8] < -6000 )
		{
			current_orientation = F_INVERTED ;
		}
		else if ( canStabilizeHover() && rmat[7] < -14000 )
		{
			current_orientation = F_HOVER ;
		}
		else
		{
			current_orientation = F_NORMAL ;
		}
	}


	// COMMENTS PLEASE - WHAT DOES THIS DO?
	switch( get_flightmode())
	{
	case FLIGHT_MODE_STABILIZED:
		desired_behavior.W = current_orientation ;
		break;
	case FLIGHT_MODE_MANUAL:
	case FLIGHT_MODE_NO_RADIO: 
	case FLIGHT_MODE_ASSISTED: 
	case FLIGHT_MODE_AUTONOMOUS:
		break;
	}
		
	dcm_enable_yaw_drift_correction(current_orientation != F_HOVER) ;
	
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
			
			cyclesUntilStopTriggerAction = TRIGGER_PULSE_DURATION / (int32_t)25 ;
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
			
			cyclesUntilStopTriggerAction = TRIGGER_PULSE_DURATION / (int32_t)25 ;
			cyclesUntilStartTriggerAction = TRIGGER_REPEAT_PERIOD / (int32_t)25 ;
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
		udb_pwOut[TRIGGER_OUTPUT_CHANNEL] = (newValue) ? TRIGGER_SERVO_HIGH : TRIGGER_SERVO_LOW ;
	}
	else if ( TRIGGER_TYPE == TRIGGER_TYPE_DIGITAL )
	{
		if ( NUM_OUTPUTS < 6 )
		{
			udb_set_action_state(newValue) ;
		}
	}
	
	currentTriggerActionValue = newValue ;
	
	return ;
}
