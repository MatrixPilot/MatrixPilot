// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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

union fbts_int flags ;
union fbts_int old_rtl_flags ;
int waggle = 0 ;
int calib_timer, standby_timer ;

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
	dcm_flags._.dead_reckon_enable = 0 ;
	stateS = &startS ;
	return ;
}

void udb_background_callback_periodic(void)
{
	//	Configure the GPS for binary if there is a request to do so.
	//	Determine whether the radio is on.
	
	if ( udb_flags._.radio_on )
	{
		//	Select manual, automatic, or come home, based on pulse width of the switch input channel as defined in options.h.
		if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_HIGH )
		{
			flags._.man_req = 0 ;
			flags._.auto_req = 0 ;
			flags._.home_req = 1 ;
		}
		else if ( udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] > MODE_SWITCH_THRESHOLD_LOW )
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
		
		// With Failsafe Hold enabled: After losing RC signal, and then regaining it, you must manually
		// change the mode switch position in order to exit RTL mode.
		if (flags._.rtl_hold)
		{
			if (flags._.man_req  == old_rtl_flags._.man_req &&
				flags._.auto_req == old_rtl_flags._.auto_req &&
				flags._.home_req == old_rtl_flags._.home_req)
			{
				flags._.man_req = 0 ;
				flags._.auto_req = 0 ;
				flags._.home_req = 0 ;
			}
			else
			{
				old_rtl_flags.WW = flags.WW ;
				flags._.rtl_hold = 0 ;
			}
		}
	}
	else
	{
		flags._.man_req = 0 ;
		flags._.auto_req = 0 ;
		flags._.home_req = 1 ;
	}
	
	//	Update the nav capable flag. If the GPS has a lock, gps_data_age will be small.
	//	For now, nav_capable will always be 0 when the Airframe type is AIRFRAME_HELI.
#if (AIRFRAME_TYPE != AIRFRAME_HELI)
	if (gps_data_age < GPS_DATA_MAX_AGE) gps_data_age++ ;
	dcm_flags._.nav_capable = (gps_data_age < GPS_DATA_MAX_AGE) ;
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
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ; // turn on mode led
#endif
	return ;
}

//	Acquire state is used to wait for the GPS to achieve lock.
void ent_acquiringS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold_throttle = 0 ;
	flags._.altitude_hold_pitch = 0 ;
	
	// almost ready to turn the control on, save the trims and sensor offsets
	udb_servo_record_trims() ;
	dcm_calibrate() ;
	
	waggle = WAGGLE_SIZE ;
	throttleFiltered._.W1 = 0 ;
	stateS = &acquiringS ;
	standby_timer = STANDBY_PAUSE ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_OFF ;
#endif
	
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
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_OFF ;
#endif
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
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
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
	
	if ( !(FAILSAFE_TYPE == FAILSAFE_MAIN_FLIGHTPLAN && stateS == &returnS) )
	{
		init_flightplan( 0 ) ; // Only reset non-rtl waypoints if not already following waypoints
	}
	
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &waypointS ;
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
	init_flightplan( 1 ) ;
#elif ( FAILSAFE_TYPE == FAILSAFE_MAIN_FLIGHTPLAN )
	if ( stateS != &waypointS )
	{
		init_flightplan( 0 ) ; // Only reset non-rtl waypoints if not already following waypoints
	}
#endif
	
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &returnS ;
	return ;
}


void startS(void)
{
	ent_calibrateS() ;
	return ;
}

void calibrateS(void)
{
	if ( udb_flags._.radio_on )
	{
#if ( LED_RED_MAG_CHECK == 0 )
		udb_led_toggle(LED_RED) ;
#endif
		
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
	
	if ( dcm_flags._.nav_capable && ( ( MAG_YAW_DRIFT == 0 ) || ( magMessage == 7 ) ) )
	{
		if ( udb_flags._.radio_on )
		{
			if (standby_timer == NUM_WAGGLES+1)
				waggle = WAGGLE_SIZE ;
			else if (standby_timer <= NUM_WAGGLES)
				waggle = - waggle ;
			else
				waggle = 0 ;
			
			standby_timer-- ;
			if ( standby_timer == 6 )
			{
				flags._.save_origin = 1 ;
			}
			else if ( standby_timer == 2 )
			{
				dcm_flags._.dead_reckon_enable = 1 ;
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
	if ( udb_flags._.radio_on )
	{
		if ( flags._.home_req & dcm_flags._.nav_capable )
			ent_waypointS() ;
		else if ( flags._.auto_req )
			ent_stabilizedS() ;
	}
	else
	{
		if ( dcm_flags._.nav_capable )
			ent_returnS() ;
		else
			ent_stabilizedS() ;
	}
	return ;
}


void stabilizedS(void) 
{
	if ( udb_flags._.radio_on )
	{
		if ( flags._.home_req & dcm_flags._.nav_capable )
			ent_waypointS() ;
		else if ( flags._.man_req )
			ent_manualS() ;
	}
	else
	{
		if ( dcm_flags._.nav_capable )
			ent_returnS() ;
	}
	return ;
}

void waypointS(void)
{
#if ( LED_RED_MAG_CHECK == 0 )
	udb_led_toggle(LED_RED) ;
#endif
	
	if ( udb_flags._.radio_on )
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
	if ( udb_flags._.radio_on )
	{
		if ( flags._.man_req )
			ent_manualS() ;
		else if ( flags._.auto_req )
			ent_stabilizedS() ;
		else if ( flags._.home_req & dcm_flags._.nav_capable )
			ent_waypointS() ;
	}
	else
	{
#if (FAILSAFE_HOLD == 1)
		flags._.rtl_hold = 1 ;
#endif
	}		
	return ;
}
