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
//#include "mode_switch.h"

union fbts_int flags ;
AUTOPILOT_MODE flight_mode = FLIGHT_MODE_MANUAL;
int waggle = 0 ;
int calib_timer = CALIB_PAUSE ;
int standby_timer = STANDBY_PAUSE ;

void startS(void) ;
void calibrateS(void) ;
void acquiringS(void) ;
void manualS(void) ;
void stabilizedS(void) ;
void assistedS(void) ;
void waypointS(void) ;
void returnS(void) ;

boolean  navigation_enabled = false;
boolean  autopilot_enabled = false;

inline AUTOPILOT_MODE get_flightmode(void) { return flight_mode;};

inline boolean mode_navigation_enabled(void) { return navigation_enabled;};

inline boolean mode_autopilot_enabled(void) { return autopilot_enabled;};


//	Implementation of state machine.
//	Examine the state of the radio and GPS and supervisory channel to decide how to control the plane.

void (* stateS ) ( void ) = &startS ;

void init_states(void)
{
	flags.WW = 0 ;
    flight_mode = FLIGHT_MODE_MANUAL;
	waggle = 0 ;
	gps_data_age = GPS_DATA_MAX_AGE+1 ;
	dcm_flags._.dead_reckon_enable = 0 ;
	stateS = &startS ;
	navigation_enabled = false;
	autopilot_enabled = false;
	return ;
}

void udb_background_callback_periodic(void)
{
	//	Configure the GPS for binary if there is a request to do so.

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
	
	return ;
}

//	Functions that are executed upon first entrance into a state.

//	Calibrate state is used to wait for the filters to settle before recording A/D offsets.
void ent_calibrateS()
{
    flight_mode = FLIGHT_MODE_MANUAL;
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
    flight_mode = FLIGHT_MODE_MANUAL;
	
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
	
	return ;
}

//	Manual state is used for direct pass-through control from radio to servos.
void ent_manualS()
{
    flight_mode = FLIGHT_MODE_MANUAL;
	navigation_enabled = false;
	autopilot_enabled = false;
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_OFF ;
#endif
	stateS = &manualS ;
	return ;
}

//	Stabilized state provides augmented control.
void ent_stabilizedS()
{
#if (ALTITUDEHOLD_STABILIZED == AH_PITCH_ONLY)
	// When using pitch_only in stabilized mode, maintain the altitude
	// that the plane was at when entering stabilized mode.
	setTargetAltitude(IMUlocationz._.W1) ;
#endif
	
    flight_mode = FLIGHT_MODE_STABILIZED;
	navigation_enabled = false;
	autopilot_enabled = true;
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &stabilizedS ;
	return ;
}


//	Assisted state provides assisted control. 
void ent_assistedS()
{
#if (ALTITUDEHOLD_STABILIZED == AH_PITCH_ONLY)
	// When using pitch_only in stabilized mode, maintain the altitude
	// that the plane was at when entering stabilized mode.
	setTargetAltitude(IMUlocationz._.W1) ;
#endif	
    flight_mode = FLIGHT_MODE_ASSISTED;
	navigation_enabled = false;
	autopilot_enabled = true;
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &assistedS ;
	return ;
}


//	Same as the come home state, except the radio is on.
//	Come home is commanded by the mode switch channel (defaults to channel 4).
void ent_waypointS()
{
    flight_mode = FLIGHT_MODE_AUTONOMOUS;
	navigation_enabled = true;
	autopilot_enabled = true;
	
	if ( !(FAILSAFE_TYPE == FAILSAFE_MAIN_FLIGHTPLAN && stateS == &returnS) )
	{
		init_flightplan( 0 ) ; // Only reset non-rtl waypoints if not already following waypoints
	}
	
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &waypointS ;
	return ;
}

//	Come home state, entered when the radio signal is lost, and gps is locked.
void ent_returnS()
{
	flight_mode = FLIGHT_MODE_NO_RADIO;

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

void udb_callback_radio_did_turn_off( void )
{
	// Only enter RTL mode if we are calibrated and acquired
	if (calib_timer <= 0 && standby_timer <= 0)
	{
		ent_returnS() ;
	}
	return ;
}

void startS(void)
{
	ent_calibrateS() ;
	return ;
}

void calibrateS(void)
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
	return ;
}


void manualS(void)
{
    if (udb_flags._.radio_on)
    {
        switch (get_requested_flightmode())
        {
            case FLIGHT_MODE_MANUAL:
                break;
            case FLIGHT_MODE_STABILIZED:
                ent_stabilizedS();
                break;
            case FLIGHT_MODE_ASSISTED:
                if (dcm_flags._.nav_capable)
                {
	                ent_assistedS();
                }
	            break;
            case FLIGHT_MODE_AUTONOMOUS:
                if (dcm_flags._.nav_capable)
                {
                    ent_waypointS();
                }
	            break;
            case FLIGHT_MODE_NO_RADIO:
		        ent_returnS();
	            break;
        }

    } else
    {
        if (dcm_flags._.nav_capable)
            ent_returnS();
        else
            ent_stabilizedS();
    }
    return;
}


void stabilizedS(void)
{
    if (udb_flags._.radio_on)
    {
        switch (get_requested_flightmode())
        {
            case FLIGHT_MODE_MANUAL:
                ent_manualS();
                break;
            case FLIGHT_MODE_STABILIZED:
                break;
            case FLIGHT_MODE_ASSISTED:
                if (dcm_flags._.nav_capable)
                {
	                ent_assistedS();
                }
	            break;
            case FLIGHT_MODE_AUTONOMOUS:
                if (dcm_flags._.nav_capable)
                {
                    ent_waypointS();
                }
	            break;
            case FLIGHT_MODE_NO_RADIO:
		        ent_returnS();
	            break;
        }

    } else
    {
        if (dcm_flags._.nav_capable)
            ent_returnS();
    }
    return;
}

void assistedS(void)
{
    if (udb_flags._.radio_on)
    {
        switch (get_requested_flightmode())
        {
            case FLIGHT_MODE_MANUAL:
                ent_manualS();
                break;
            case FLIGHT_MODE_STABILIZED:
                ent_stabilizedS();
                break;
            case FLIGHT_MODE_ASSISTED:
	            break;
            case FLIGHT_MODE_AUTONOMOUS:
                if (dcm_flags._.nav_capable)
                {
                    ent_waypointS();
                }
	            break;
            case FLIGHT_MODE_NO_RADIO:
		        ent_returnS();
	            break;
        }

    } else
    {
        if (dcm_flags._.nav_capable)
            ent_returnS();
    }
    return;
}

void waypointS(void)
{
#if ( LED_RED_MAG_CHECK == 0 )
	udb_led_toggle(LED_RED) ;
#endif

    if (udb_flags._.radio_on)
    {
        switch (get_requested_flightmode())
        {
            case FLIGHT_MODE_MANUAL:
                ent_manualS();
                break;
            case FLIGHT_MODE_STABILIZED:
                ent_stabilizedS();
                break;
            case FLIGHT_MODE_ASSISTED:
                if (dcm_flags._.nav_capable)
                {
	                ent_assistedS();
                }
	            break;
            case FLIGHT_MODE_AUTONOMOUS:
	            break;
            case FLIGHT_MODE_NO_RADIO:
		        ent_returnS();
	            break;
        }
    } else
    {
        ent_returnS();
    }
    return;
}

void returnS(void)
{
    if (udb_flags._.radio_on)
    {
        switch (get_requested_flightmode())
        {
            case FLIGHT_MODE_MANUAL:
                ent_manualS();
                break;
            case FLIGHT_MODE_STABILIZED:
                ent_stabilizedS();
                break;
            case FLIGHT_MODE_ASSISTED:
                if (dcm_flags._.nav_capable)
                {
	                ent_assistedS();
                }
	            break;
            case FLIGHT_MODE_AUTONOMOUS:
            {
                if (dcm_flags._.nav_capable)
                {
                    ent_waypointS();
                }
            }
            case FLIGHT_MODE_NO_RADIO:
	            break;
        }


    } else
    {
#if (FAILSAFE_HOLD == 1)
        flags._.rtl_hold = 1;
#endif
    }
    return;
}
