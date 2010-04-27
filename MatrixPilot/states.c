#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

void startS(void) ;
void calibrateS(void) ;
void acquiringS(void) ;
void manualS(void) ;
void stabilizedS(void) ;
void waypointS(void) ;
void returnS(void) ;

//	Implementation of state machine.
//	Examine the state of the radio and GPS and supervisory channel to decide how to control the plane.

void (* stateS ) ( void ) = &startS ;

void init_states(void)
{
	flags.WW = 0 ;
	waggle = 0 ;
	gps_data_age = GPS_DATA_MAX_AGE+1 ;
	stateS = &startS ;
	return ;
}

void state_machine(void)
{
	//	Configure the GPS for binary if there is a request to do so.
	//	Determine whether the radio is on.
	
#if (NORADIO == 1)
	pulsesselin = 100 ;
#endif
	
	if ( pulsesselin > 10 )
	{
		flags._.radio_on = 1 ;
		LED_GREEN = LED_ON ; // indicate radio on
		
		//	Select manual, automatic, or come home, based on pulse width of the switch input channel as defined in options.h.
		if ( pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_HIGH )
		{
			flags._.man_req = 0 ;
			flags._.auto_req = 0 ;
			flags._.home_req = 1 ;
		}
		else if ( pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW )
		{
			flags._.man_req = 0 ;
			flags._.auto_req = 1 ;
			flags._.home_req = 0 ;
		}
		else
		{
			flags._.man_req = 1 ;
			flags._.auto_req = 0 ;
			flags._.home_req = 0 ;
		}
	}
	else
	{
		flags._.radio_on = 0 ;
		LED_GREEN = LED_OFF ; // indicate radio off
		
		flags._.man_req = 0 ;
		flags._.auto_req = 0 ;
		flags._.home_req = 1 ;
	}
	
	pulsesselin = 0 ;
	
	//	Update the nav capable flag. If the GPS has a lock, gps_data_age will be small.
	//	For now, nav_capable will always be 0 when the Airframe type is AIRFRAME_HELI.
#if (AIRFRAME_TYPE != AIRFRAME_HELI)
	if (gps_data_age < GPS_DATA_MAX_AGE) gps_data_age++ ;
	flags._.nav_capable = (gps_data_age < GPS_DATA_MAX_AGE) ;
#endif
	
	//	Execute the activities for the current state.
	(* stateS) () ;
	
	return ;
}

//	Functions that are executed upon first entrance into a state.

//	Calibrate state is used to wait for the filters to settle before recording A/D offsets.
void ent_calibrateS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold_throttle = 0 ;
	flags._.altitude_hold_pitch = 0 ;
	waggle = 0 ;
	stateS = &calibrateS ;
	calib_timer = CALIB_PAUSE ;
	LED_RED = LED_ON ; // turn on mode led
	return ;
}

//	Acquire state is used to wait for the GPS to achieve lock.
void ent_acquiringS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold_throttle = 0 ;
	flags._.altitude_hold_pitch = 0 ;
	waggle = WAGGLE_SIZE ;
	throttleFiltered._.W1 = 0 ;
	stateS = &acquiringS ;
	standby_timer = STANDBY_PAUSE ;
	LED_RED = LED_OFF ;
	
	int i;
	for (i=1; i <= NUM_INPUTS; i++)
		pwTrim[i] = pwIn[i] ;
	
	return ;
}

//	Manual state is used for direct pass-through control from radio to servos.
void ent_manualS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold_throttle = 0 ;
	flags._.altitude_hold_pitch = 0 ;
	waggle = 0 ;
	LED_RED = LED_OFF ;
	stateS = &manualS ;
	return ;
}

//	Auto state provides augmented control. 
void ent_stabilizedS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold_throttle = (ALTITUDEHOLD_STABILIZED == AH_FULL) ;
	flags._.altitude_hold_pitch = (ALTITUDEHOLD_STABILIZED == AH_FULL || ALTITUDEHOLD_STABILIZED == AH_PITCH_ONLY) ;
	waggle = 0 ;
	LED_RED = LED_ON ;
	stateS = &stabilizedS ;
	return ;
}

//	Same as the come home state, except the radio is on.
//	Come home is commanded by the mode switch channel (defaults to channel 4).
void ent_waypointS()
{
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold_throttle = (ALTITUDEHOLD_WAYPOINT == AH_FULL) ;
	flags._.altitude_hold_pitch = (ALTITUDEHOLD_WAYPOINT == AH_FULL || ALTITUDEHOLD_WAYPOINT == AH_PITCH_ONLY) ;
	waggle = 0 ;
	
	if ( !(FAILSAFE_TYPE == FAILSAFE_WAYPOINTS && stateS == &returnS) )
	{
		init_waypoints( 0 ) ; // Only reset non-rtl waypoints if not already following waypoints
	}
	
	LED_RED = LED_ON ;
	stateS = &waypointS ;
	// IFS0bits.T3IF = 1 ;			// trigger navigation immediately
	return ;
}

//	Come home state, entered when the radio signal is lost, and gps is locked.
void ent_returnS()
{
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold_throttle = (ALTITUDEHOLD_WAYPOINT == AH_FULL) ;
	flags._.altitude_hold_pitch = (ALTITUDEHOLD_WAYPOINT == AH_FULL || ALTITUDEHOLD_WAYPOINT == AH_PITCH_ONLY) ;
	
#if ( FAILSAFE_TYPE == FAILSAFE_RTL )
	init_waypoints( 1 ) ;
#elif ( FAILSAFE_TYPE == FAILSAFE_WAYPOINTS )
	if ( stateS != &waypointS )
	{
		init_waypoints( 0 ) ; // Only reset non-rtl waypoints if not already following waypoints
	}
#endif
	
	waggle = 0 ;
	LED_RED = LED_ON ;
	stateS = &returnS ;
	// IFS0bits.T3IF = 1 ;			// trigger navigation immediately
	return ;
}


void startS(void)
{
	ent_calibrateS() ;
	return ;
}

void calibrateS(void)
{
	if ( flags._.radio_on )
	{
		LED_RED_DO_TOGGLE ;
		
		calib_timer--;
		if (calib_timer <= 0)
			ent_acquiringS() ;
	}
	else
	{
		ent_calibrateS() ;
	}
	return ;
}

void acquiringS(void)
{
#if ( AIRFRAME_TYPE == AIRFRAME_HELI )
	ent_manualS();
	return;
#endif
		
	if ( flags._.nav_capable )
	{
		if ( flags._.radio_on )
		{
			if (standby_timer == NUM_WAGGLES+1)
				waggle = WAGGLE_SIZE ;
			else if (standby_timer <= NUM_WAGGLES)
				waggle = - waggle ;
			else
				waggle = 0 ;
			
			standby_timer-- ;
			if ( standby_timer == 2 )
			{
				flags._.save_origin = 1 ;
			}
			else if ( standby_timer <= 0)
			{
				ent_manualS() ;
			}
		}
		else {
			waggle = 0 ;
		}
	}
	else
	{
		waggle = 0 ;
	}
	return ;
}

void manualS(void) 
{
	if ( flags._.radio_on )
	{
		if ( flags._.home_req & flags._.nav_capable )
			ent_waypointS() ;
		else if ( flags._.auto_req )
			ent_stabilizedS() ;
	}
	else
	{
		if ( flags._.nav_capable )
			ent_returnS() ;
		else
			ent_stabilizedS() ;
	}
	return ;
}


void stabilizedS(void) 
{
	if ( flags._.radio_on )
	{
		if ( flags._.home_req & flags._.nav_capable )
			ent_waypointS() ;
		else if ( flags._.man_req )
			ent_manualS() ;
	}
	else
	{
		if ( flags._.nav_capable )
			ent_returnS() ;
	}
	return ;
}

void waypointS(void)
{
	LED_RED_DO_TOGGLE ;
	
	if (flags._.radio_on )
	{
		if ( flags._.man_req )
			ent_manualS() ;
		else if ( flags._.auto_req )
			ent_stabilizedS() ;
	}
	else
	{
		ent_returnS() ;
	}
	return ;
}

void returnS(void)
{
	if ( flags._.radio_on )
	{
		if ( flags._.man_req )
			ent_manualS() ;
		else if ( flags._.auto_req )
			ent_stabilizedS() ;
		else if ( flags._.home_req & flags._.nav_capable )
			ent_waypointS() ;
	}		
	return ;
}
