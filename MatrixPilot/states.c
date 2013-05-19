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
#include "mode_switch.h"

union fbts_int flags ;
int16_t waggle = 0 ;
uint8_t counter = 0;

#define CALIB_PAUSE 21		// wait for 10.5 seconds of runs through the state machine
#define STANDBY_PAUSE 48	// pause for 24 seconds of runs through the state machine
#define NUM_WAGGLES 4		// waggle 4 times during the end of the standby pause (this number must be less than STANDBY_PAUSE)
#define WAGGLE_SIZE 300

static int16_t calib_timer = CALIB_PAUSE ;
static int16_t standby_timer = STANDBY_PAUSE ;

static void startS(void) ;
static void calibrateS(void) ;
static void acquiringS(void) ;
static void manualS(void) ;
static void stabilizedS(void) ;
static void waypointS(void) ;
static void returnS(void) ;

static void ent_returnS(void) ;

//	Implementation of state machine.
//	Examine the state of the radio and GPS and supervisory channel to decide how to control the plane.

void (* stateS ) ( void ) = &startS ;

void init_states(void)
{
	flags.WW = 0 ;
	waggle = 0 ;
	gps_data_age = GPS_DATA_MAX_AGE+1 ;
	dcm_flags._.dead_reckon_enable = 0 ;
	flags._.update_autopilot_state_asap = 0 ;
	stateS = &startS ;
}

void udb_callback_radio_did_turn_off( void )
{
	flags._.update_autopilot_state_asap = 1 ;
}
// Called at 40Hz
void udb_background_callback_periodic(void)
{
        if ( counter++ >= 20 ) // 2Hz
        {
            counter = 0 ;
            //	Determine whether a flight mode switch is commanded.
            flight_mode_switch_check_set();
            //	Update the nav capable flag. If the GPS has a lock, gps_data_age will be small.
            //	For now, nav_capable will always be 0 when the Airframe type is AIRFRAME_HELI.
#if (AIRFRAME_TYPE != AIRFRAME_HELI)
            if (gps_data_age < GPS_DATA_MAX_AGE) gps_data_age++ ;
            dcm_flags._.nav_capable = (gps_data_age < GPS_DATA_MAX_AGE) ;
#endif
            //	Execute the activities for the current state.
            (* stateS) () ;
        }
        else if (flags._.update_autopilot_state_asap == 1 )
        {
            flight_mode_switch_check_set();
            (* stateS) () ;
        }
        flags._.update_autopilot_state_asap = 0 ;
}

//	Functions that are executed upon first entrance into a state.

//	Calibrate state is used to wait for the filters to settle before recording A/D offsets.
static void ent_calibrateS(void)
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
}

//	Acquire state is used to wait for the GPS to achieve lock.
static void ent_acquiringS(void)
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold_throttle = 0 ;
	flags._.altitude_hold_pitch = 0 ;
	
	// almost ready to turn the control on, save the trims and sensor offsets
#if (FIXED_TRIMPOINT != 1)	// Do not alter trims from preset when they are fixed
 #if(USE_NV_MEMORY == 1)
	if(udb_skip_flags.skip_radio_trim == 0)
	{
		udb_servo_record_trims() ;
	}
 #else
		udb_servo_record_trims() ;
 #endif
#endif
	dcm_calibrate() ;
	
	waggle = WAGGLE_SIZE ;
	throttleFiltered._.W1 = 0 ;
	stateS = &acquiringS ;
	standby_timer = STANDBY_PAUSE ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_OFF ;
#endif
}

//	Manual state is used for direct pass-through control from radio to servos.
static void ent_manualS(void)
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
}

//	Auto state provides augmented control. 
static void ent_stabilizedS(void)
{
#if (ALTITUDEHOLD_STABILIZED == AH_PITCH_ONLY)
	// When using pitch_only in stabilized mode, maintain the altitude
	// that the plane was at when entering stabilized mode.
	setTargetAltitude(IMUlocationz._.W1) ;
#endif
	
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold_throttle = (ALTITUDEHOLD_STABILIZED == AH_FULL) ;
	flags._.altitude_hold_pitch = (ALTITUDEHOLD_STABILIZED == AH_FULL || ALTITUDEHOLD_STABILIZED == AH_PITCH_ONLY) ;
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &stabilizedS ;
}

//	Same as the come home state, except the radio is on.
//	Come home is commanded by the mode switch channel (defaults to channel 4).
static void ent_waypointS(void)
{
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold_throttle = (ALTITUDEHOLD_WAYPOINT == AH_FULL) ;
	flags._.altitude_hold_pitch = (ALTITUDEHOLD_WAYPOINT == AH_FULL || ALTITUDEHOLD_WAYPOINT == AH_PITCH_ONLY) ;
	
	if ( !(FAILSAFE_TYPE == FAILSAFE_MAIN_FLIGHTPLAN && stateS == &returnS) )
	{
		init_flightplan( 0 ) ; // Only reset non-rtl waypoints if not already following waypoints
	}
	
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &waypointS ;
}

//	Come home state, entered when the radio signal is lost, and gps is locked.
static void ent_returnS(void)
{
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold_throttle = (ALTITUDEHOLD_WAYPOINT == AH_FULL) ;
	flags._.altitude_hold_pitch = (ALTITUDEHOLD_WAYPOINT == AH_FULL || ALTITUDEHOLD_WAYPOINT == AH_PITCH_ONLY) ;
#if (FAILSAFE_HOLD == 1)
	flags._.rtl_hold = 1 ;
#endif	
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
}

static void startS(void)
{
	ent_calibrateS() ;
}

static void calibrateS(void)
{
#if (NORADIO == 1)
	if ( 1 )
#else
	if ( udb_flags._.radio_on )
#endif
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
}

static void acquiringS(void)
{
#if ( AIRFRAME_TYPE == AIRFRAME_HELI )
	ent_manualS();
	return;
#endif
	
	if ( dcm_flags._.nav_capable && ( ( MAG_YAW_DRIFT == 0 ) || ( magMessage == 7 ) ) )
	{
#if (NORADIO == 1)
		if ( 1 )
#else
		if ( udb_flags._.radio_on )
#endif
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
}

static void manualS(void) 
{
	if ( udb_flags._.radio_on )
	{
		if ( flight_mode_switch_home() & dcm_flags._.nav_capable )
			ent_waypointS() ;
		else if ( flight_mode_switch_auto() )
			ent_stabilizedS() ;
	}
	else
	{
		if ( dcm_flags._.nav_capable )
			ent_returnS() ;
		else
			ent_stabilizedS() ;
	}
}

static void stabilizedS(void) 
{
	if ( udb_flags._.radio_on )
	{
		if ( flight_mode_switch_home() & dcm_flags._.nav_capable )
			ent_waypointS() ;
		else if ( flight_mode_switch_manual() )
			ent_manualS() ;
	}
	else
	{
		if ( dcm_flags._.nav_capable )
			ent_returnS() ;
	}
}

static void waypointS(void)
{
#if ( LED_RED_MAG_CHECK == 0 )
	udb_led_toggle(LED_RED) ;
#endif
	
	if ( udb_flags._.radio_on )
	{
		if ( flight_mode_switch_manual() )
			ent_manualS() ;
		else if ( flight_mode_switch_auto() )
			ent_stabilizedS() ;
	}
	else
	{
		ent_returnS() ;
	}
}

static void returnS(void)
{
	if ( udb_flags._.radio_on )
	{
		if ( flight_mode_switch_manual() )
			ent_manualS() ;
		else if ( flight_mode_switch_auto() )
			ent_stabilizedS() ;
		else if ( flight_mode_switch_home() & dcm_flags._.nav_capable )
			ent_waypointS() ;
	}
	else
	{
#if (FAILSAFE_HOLD == 1)
		flags._.rtl_hold = 1 ;
#endif
	}		
}
