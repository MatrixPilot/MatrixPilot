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

#define CALIB_PAUSE 21		// wait for 10.5 seconds of runs through the state machine
#define STANDBY_PAUSE 48	// pause for 24 seconds of runs through the state machine
#define NUM_WAGGLES 4		// waggle 4 times during the end of the standby pause (this number must be less than STANDBY_PAUSE)
#define WAGGLE_SIZE 300

AIRCRAFT_FLIGHT_MODE_STATE flightModeState = smBOOTING;
union fbts_int flags ;
int16_t waggle = 0 ;
int16_t calib_timer = CALIB_PAUSE ;
int16_t standby_timer = STANDBY_PAUSE ;

void startS(void) ;
void calibrateS(void) ;
void acquiringS(void) ;
void manualS(void) ;
void stabilizedS(void) ;
void waypointS(void) ;
void returnS(void) ;
void setFlightModeState(AIRCRAFT_FLIGHT_MODE_STATE newState);
AIRCRAFT_FLIGHT_MODE_STATE getFlightModeState();
void assignFlightModePerModeSwitch();

#if (CATAPULT_LAUNCH_ENABLE == 1)
#define LAUNCH_DELAY (2)      // delay @ 2Hz
static int16_t launch_timer = LAUNCH_DELAY;
static void cat_armedS(void) ;
static void cat_delayS(void) ;
boolean launch_enabled(void);
#endif
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
}

void udb_background_callback_periodic(void)
{
	//	Configure the GPS for binary if there is a request to do so.

	//	Determine whether a flight mode switch is commanded.	
	flight_mode_switch_check_set();
	
	//	Update the nav capable flag. If the GPS has a lock, gps_data_age will be small.
	//	For now, nav_capable will always be 0 when the Airframe type is AIRFRAME_HELI.
#if (AIRFRAME_TYPE != AIRFRAME_HELI)
        {
            if (gps_data_age < GPS_DATA_MAX_AGE) gps_data_age++ ;
            dcm_flags._.nav_capable = (gps_data_age < GPS_DATA_MAX_AGE) ;
        }
#endif
	
	//	Execute the activities for the current state.
	(* stateS) () ;
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
	setFlightModeState(smCALIBRATING);
	calib_timer = CALIB_PAUSE ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ; // turn on mode led
#endif
}

//	Acquire state is used to wait for the GPS to achieve lock.
void ent_acquiringS()
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
	setFlightModeState(smWAITING_FOR_GPS_LOCK);
	standby_timer = STANDBY_PAUSE ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_OFF ;
#endif
}

//	Manual state is used for direct pass-through control from radio to servos.
void ent_manualS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold_throttle = 0 ;
	flags._.altitude_hold_pitch = 0 ;
  flags._.disable_throttle = 0 ;
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_OFF ;
#endif
	stateS = &manualS ;
  setFlightModeState(smFLYING_MANUAL);
}

//	Auto state provides augmented control. 
void ent_stabilizedS()
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
  flags._.disable_throttle = 0 ;
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &stabilizedS ;
  setFlightModeState(smFLYING_STABILIZED);
}

#if (CATAPULT_LAUNCH_ENABLE == 1)
//  State: catapult launch armed
//  entered from manual or stabilize if launch_enabled()
static void ent_cat_armedS(void)
{
  // this flag is only relevant in cat_armed state
  // and is cleared here and in dcm_init
  dcm_flags._.launch_detected = 0;

  // must suppress throttle in cat_armed state
  flags._.disable_throttle = 1;

  stateS = &cat_armedS;
  setFlightModeState(smARMED_FOR_LAUNCH);
}

// State: catapult launch delay
// entered from cat_armed if launch_detected()
static void ent_cat_delayS(void)
{
  launch_timer = LAUNCH_DELAY;
  stateS = &cat_delayS;
  setFlightModeState(smLAUNCHING);
  stateS(); // cal it right away to decrement
}
#endif

//	Same as the come home state, except the radio is on.
//	Come home is commanded by the mode switch channel (defaults to channel 4).
void ent_waypointS()
{
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold_throttle = (ALTITUDEHOLD_WAYPOINT == AH_FULL) ;
	flags._.altitude_hold_pitch = (ALTITUDEHOLD_WAYPOINT == AH_FULL || ALTITUDEHOLD_WAYPOINT == AH_PITCH_ONLY) ;
    flags._.disable_throttle = 0 ;

	if ( !(FAILSAFE_TYPE == FAILSAFE_MAIN_FLIGHTPLAN && stateS == &returnS) )
	{
		init_flightplan( 0 ) ; // Only reset non-rtl waypoints if not already following waypoints
	}
	
	waggle = 0 ;
#if ( LED_RED_MAG_CHECK == 0 )
	LED_RED = LED_ON ;
#endif
	stateS = &waypointS ;
  setFlightModeState(smFLYING_WAYPOINT);
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
  setFlightModeState(smLANDING);
}

void udb_callback_radio_did_turn_off( void )
{
	// Only enter RTL mode if we are calibrated and acquired
	if (calib_timer <= 0 && standby_timer <= 0)
	{
		ent_returnS() ;
	}
}

void startS(void)
{
	ent_calibrateS() ;
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
}

void acquiringS(void)
{
#if ( AIRFRAME_TYPE == AIRFRAME_HELI )
	assignFlightModePerModeSwitch();
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
        setFlightModeState(smREADY_FOR_LAUNCH);
        #if (CATAPULT_LAUNCH_ENABLE == 1)
        if (launch_enabled())
          ent_cat_armedS();
        else
        #endif
          assignFlightModePerModeSwitch();
			}
		}
		else {
			waggle = 0 ;
      setFlightModeState(smWAITING_FOR_RADIO_INPUT);
		}
	}
	else
	{
		waggle = 0 ;
	}
}

#if (CATAPULT_LAUNCH_ENABLE == 1)
boolean launch_enabled(void)
{
    return (udb_pwIn[LAUNCH_ARM_INPUT_CHANNEL] > 3000);
}
//  State: catapult launch armed
//  entered only from manualS iff (radio_on and gear_up and nav_capable and switch_home)
static void cat_armedS(void)
{
    if (!launch_enabled())
    {
      assignFlightModePerModeSwitch();
    }
    else if (dcm_flags._.launch_detected) {
        ent_cat_delayS();
    }
    else
    {
      if (udb_flags._.radio_on)
      {
        udb_led_toggle(LED_GREEN);
        LED_RED = !LED_GREEN;
      }
      else
      {
        LED_GREEN = LED_OFF;
      }
    }
}
// State: catapult launch delay
// entered from cat_armedS when launch_detected
static void cat_delayS(void)
{
  // transition to user mode after the launch delay
  if (launch_timer)
  {
    launch_timer--;
    if (launch_timer <= 0)
    {
      assignFlightModePerModeSwitch();
    }
  }
}
#endif

void assignFlightModePerModeSwitch()
{
  if ( flight_mode_switch_manual() && (stateS != &manualS))
    ent_manualS();
  else if ( flight_mode_switch_auto()  && (stateS != &stabilizedS))
    ent_stabilizedS();
  else if ( flight_mode_switch_home() & dcm_flags._.nav_capable  && (stateS != &waypointS))
    ent_waypointS();
}
void manualS(void) 
{
	if ( udb_flags._.radio_on )
	{
    assignFlightModePerModeSwitch();
	}
	else
	{
    if ( dcm_flags._.nav_capable )
			ent_returnS() ;
		else
			ent_stabilizedS() ;
	}
}


void stabilizedS(void) 
{
	if ( udb_flags._.radio_on )
	{
    assignFlightModePerModeSwitch();
  }
	else
	{
		if ( dcm_flags._.nav_capable )
			ent_returnS() ;
	}
}

void waypointS(void)
{
#if ( LED_RED_MAG_CHECK == 0 )
	udb_led_toggle(LED_RED) ;
#endif
	
	if ( udb_flags._.radio_on )
	{
    assignFlightModePerModeSwitch();
	}
	else
	{
		ent_returnS() ;
	}
}

void returnS(void)
{
	if ( udb_flags._.radio_on )
	{
    assignFlightModePerModeSwitch();
	}
	else
	{
#if (FAILSAFE_HOLD == 1)
		flags._.rtl_hold = 1 ;
#endif
	}
}

void setFlightModeState(AIRCRAFT_FLIGHT_MODE_STATE newState)
{
  flightModeState = newState;
}
AIRCRAFT_FLIGHT_MODE_STATE getFlightModeState()
{
  return flightModeState;
}
