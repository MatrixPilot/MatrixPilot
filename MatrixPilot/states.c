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
#include "flightplan.h"
#include "config.h"
#include "states.h"
#include "altitudeCntrl.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/gpsParseCommon.h"

union state_flags_int state_flags;
int16_t waggle = 0;
static uint8_t counter = 0;

#define FSM_CLK 2                       // clock frequency for state machine
#if (HILSIM == 1) // implied when SILSIM == 1
#define CALIB_PAUSE   (2 * FSM_CLK)     // wait for 2 seconds of runs through the state machine
#define STANDBY_PAUSE (5 * FSM_CLK)     // pause for 5 seconds of runs through the state machine
#else
#define CALIB_PAUSE (10.5 * FSM_CLK)    // wait for 10.5 seconds of runs through the state machine
#define STANDBY_PAUSE (48 * FSM_CLK)    // pause for 48 seconds of runs through the state machine
                                        // This used to be 24 seconds, but that was not long enough
                                        // Standby pause was raised from 24 seconds to 48 seconds by BillP
                                        // to improve the accuracy of the origin during a fast warm start
#endif

#define NUM_WAGGLES 4                   // waggle 4 times during the end of the standby pause (this number must be less than STANDBY_PAUSE)
#define WAGGLE_SIZE 300

static int16_t calib_timer = CALIB_PAUSE;
static int16_t standby_timer = STANDBY_PAUSE;

static void startS(void);
static void calibrateS(void);
static void acquiringS(void);
static void manualS(void);
static void stabilizedS(void);
static void waypointS(void);
static void returnS(void);

#ifdef CATAPULT_LAUNCH_ENABLE
#define LAUNCH_DELAY (40)      // wait (x) * .25ms
static int16_t launch_timer = LAUNCH_DELAY;
static void cat_armedS(void);
static void cat_delayS(void);
#endif

static void ent_returnS(void);

//	Implementation of state machine.
//	Examine the state of the radio and GPS and supervisory channel to decide how to control the plane.

static void (*stateS)(void) = &startS;

void init_states(void)
{
	DPRINT("init_states()\r\n");
	state_flags.WW = 0;
	waggle = 0;
	gps_data_age = GPS_DATA_MAX_AGE+1;
	dcm_flags._.dead_reckon_enable = 0;
	state_flags._.update_autopilot_state_asap = 0;
	stateS = &startS;
}

void udb_callback_radio_did_turn_off(void)
{
	state_flags._.update_autopilot_state_asap = 1;
}

#ifdef CATAPULT_LAUNCH_ENABLE
static uint16_t delayCheck = 0;

// Called at 40Hz
void udb_heartbeat_40hz_callback(void)
{
	static uint16_t manualMode = 0;

	delayCheck++;

	// read flight mode switch (sets state_flags bits) at 40Hz
	flight_mode_switch_check_set();

	// respond immediately to change in manual mode or launch detection
	if ((state_flags._.man_req != manualMode)
#ifdef CATAPULT_LAUNCH_ENABLE
	    || ((dcm_flags._.launch_detected == 1) && (stateS == &cat_armedS))
	    || (stateS == &cat_delayS)
#endif
	    )
	{
		manualMode = state_flags._.man_req;
		state_flags._.update_autopilot_state_asap = 1;
	}

	if (counter++ >= 20) // 2Hz FSM clock
	{
		counter = 0;
		state_flags._.update_autopilot_state_asap = 0;
		// Update the nav capable flag. If the GPS has a lock, gps_data_age will be small.
		// For now, nav_capable will always be 0 when the Airframe type is AIRFRAME_HELI.
#if (AIRFRAME_TYPE != AIRFRAME_HELI)
#ifdef CATAPULT_LAUNCH_ENABLE
		if (stateS != &cat_delayS)
#endif
		{
			gps_nav_capable_check_set();
		}
#endif
		(*stateS)();        // Execute the activities for the current state.
	}
	else if (state_flags._.update_autopilot_state_asap == 1)   // async FSM clock
	{
//		DPRINT(":");
		state_flags._.update_autopilot_state_asap = 0;
		// reset 2Hz counter so that next synchronous clock pulse occurs in 0.5 seconds
		counter = 0;

		// Execute the activities for the current state.
		(*stateS)();
	}
}
#else
// Called at 40Hz
void udb_heartbeat_40hz_callback(void)
{
	// Determine whether a flight mode switch is commanded.
	flight_mode_switch_check_set();
	if (counter++ >= 20)    // 2Hz
	{
		counter = 0;
		// Update the nav capable flag. If the GPS has a lock, gps_data_age will be small.
		// For now, nav_capable will always be 0 when the Airframe type is AIRFRAME_HELI.
		gps_nav_capable_check_set();
		(*stateS)();        // Execute the activities for the current state.
	}
	else if (state_flags._.update_autopilot_state_asap == 1)
	{
		(*stateS)();
	}
	state_flags._.update_autopilot_state_asap = 0;
}
#endif // CATAPULT_LAUNCH_ENABLE

////////////////////////////////////////////////////////////////////////////////
//	Functions that are executed upon first entrance into a state.

//	Calibrate state is used to wait for the filters to settle before recording A/D offsets.
static void ent_calibrateS(void)
{
	DPRINT("ent_calibrateS\r");

	state_flags._.GPS_steering = 0;
	state_flags._.pitch_feedback = 0;
	state_flags._.altitude_hold_throttle = 0;
	state_flags._.altitude_hold_pitch = 0;
	waggle = 0;
	stateS = &calibrateS;
	calib_timer = CALIB_PAUSE;
	led_on(LED_RED); // turn on mode led
}

// Acquire state is used to wait for the GPS to achieve lock.
static void ent_acquiringS(void)
{
	DPRINT("\r\nent_acquiringS\r\n");

	state_flags._.GPS_steering = 0;
	state_flags._.pitch_feedback = 0;
	state_flags._.altitude_hold_throttle = 0;
	state_flags._.altitude_hold_pitch = 0;

	// almost ready to turn the control on, save the trims and sensor offsets
#if (FIXED_TRIMPOINT != 1)	// Do not alter trims from preset when they are fixed
 #if (USE_NV_MEMORY == 1)
	if (udb_skip_flags.skip_radio_trim == 0)
	{
		udb_servo_record_trims();
	}
 #else
		udb_servo_record_trims();
 #endif
#endif
	dcm_calibrate();

	waggle = WAGGLE_SIZE;
	throttleFiltered._.W1 = 0;
	stateS = &acquiringS;
	standby_timer = STANDBY_PAUSE;
	led_off(LED_RED);
}

//	Manual state is used for direct pass-through control from radio to servos.
static void ent_manualS(void)
{
	DPRINT("ent_manualS\r\n");

	state_flags._.GPS_steering = 0;
	state_flags._.pitch_feedback = 0;
	state_flags._.altitude_hold_throttle = 0;
	state_flags._.altitude_hold_pitch = 0;
	state_flags._.disable_throttle = 0;
	waggle = 0;
	led_off(LED_RED);
	stateS = &manualS;
}

//	Auto state provides augmented control.
static void ent_stabilizedS(void)
{
	DPRINT("ent_stabilizedS\r\n");

	if (settings._.AltitudeholdStabilized == AH_PITCH_ONLY)
	{
		// When using pitch_only in stabilized mode, maintain the altitude
		// that the plane was at when entering stabilized mode.
		setTargetAltitude(IMUlocationz._.W1);
	}

	state_flags._.GPS_steering = 0;
	state_flags._.pitch_feedback = 1;
	state_flags._.altitude_hold_throttle = (settings._.AltitudeholdStabilized == AH_FULL);
	state_flags._.altitude_hold_pitch = (settings._.AltitudeholdStabilized == AH_FULL || settings._.AltitudeholdStabilized == AH_PITCH_ONLY);
	waggle = 0;
	led_on(LED_RED);
	stateS = &stabilizedS;
}

#ifdef CATAPULT_LAUNCH_ENABLE
//  State: catapult launch armed
//  entered from manual or stabilize if launch_enabled()
static void ent_cat_armedS(void)
{
	DPRINT("ent_cat_armedS\r\n");

	// this flag is only relevant in cat_armed state
	// and is cleared here and in dcm_init
	dcm_flags._.launch_detected = 0;

	// must suppress throttle in cat_armed state
	state_flags._.disable_throttle = 1;

	led_on(LED_ORANGE);

	stateS = &cat_armedS;
}

// State: catapult launch delay
// entered from cat_armed if launch_detected()
static void ent_cat_delayS(void)
{
	DPRINT("ent_cat_delayS\r\n");

	launch_timer = LAUNCH_DELAY;
	stateS = &cat_delayS;
	delayCheck = 0;
}
#endif // CATAPULT_LAUNCH_ENABLE

// Same as the come home state, except the radio is on.
// Come home is commanded by the mode switch channel (defaults to channel 4).
static void ent_waypointS(void)
{
	DPRINT("ent_waypointS\r\n");

	state_flags._.GPS_steering = 1;
	state_flags._.pitch_feedback = 1;
	state_flags._.altitude_hold_throttle = (settings._.AltitudeholdWaypoint == AH_FULL);
	state_flags._.altitude_hold_pitch = (settings._.AltitudeholdWaypoint == AH_FULL || settings._.AltitudeholdWaypoint == AH_PITCH_ONLY);
	state_flags._.disable_throttle = 0;

	if (!(FAILSAFE_TYPE == FAILSAFE_MAIN_FLIGHTPLAN && stateS == &returnS))
	{
		//init_flightplan(0); // Only reset non-rtl waypoints if not already following waypoints
		flightplan_begin(0); // Only reset non-rtl waypoints if not already following waypoints
	}

	waggle = 0;
	led_on(LED_RED);
	stateS = &waypointS;
}

//	Come home state, entered when the radio signal is lost, and gps is locked.
static void ent_returnS(void)
{
	DPRINT("ent_returnS\r\n");

	state_flags._.GPS_steering = 1;
	state_flags._.pitch_feedback = 1;
	state_flags._.altitude_hold_throttle = (settings._.AltitudeholdWaypoint == AH_FULL);
	state_flags._.altitude_hold_pitch = (settings._.AltitudeholdWaypoint == AH_FULL || settings._.AltitudeholdWaypoint == AH_PITCH_ONLY);
#if (FAILSAFE_HOLD == 1)
	state_flags._.rtl_hold = 1;
#endif
#if (FAILSAFE_TYPE == FAILSAFE_RTL)
	flightplan_begin(1); // init_flightplan(1);
#elif (FAILSAFE_TYPE == FAILSAFE_MAIN_FLIGHTPLAN)
	if (stateS != &waypointS)
	{
		init_flightplan(0); // Only reset non-rtl waypoints if not already following waypoints
	}
#endif

	waggle = 0;
	led_on(LED_RED);
	stateS = &returnS;
}

////////////////////////////////////////////////////////////////////////////////
// State handlers - these are polled

static void startS(void)
{
	DPRINT("startS()\r\n");
	ent_calibrateS();
}

static void calibrateS(void)
{
#if (NORADIO == 1)
	if (1)
#else
	if (udb_flags._.radio_on)
#endif
	{
		udb_led_toggle(LED_RED);
		calib_timer--;
		DPRINT("calib_timer %u  \r", calib_timer);
		if (calib_timer <= 0)
			ent_acquiringS();
	}
	else
	{
//		DPRINT("radio is not on %u\r\n", calib_timer);
		ent_calibrateS();
	}
}

static void acquiringS(void)
{
#if (AIRFRAME_TYPE == AIRFRAME_HELI)
	ent_manualS();
	return;
#endif

	if (dcm_flags._.nav_capable && ((MAG_YAW_DRIFT == 0) || (magMessage == 7)))
	{
#if (NORADIO == 1)
		if (1)
#else
		if (udb_flags._.radio_on)
#endif
		{
			if (gps_check_startup_metrics())
			{
				if (standby_timer == NUM_WAGGLES+1)
					waggle = WAGGLE_SIZE;
				else if (standby_timer <= NUM_WAGGLES)
					waggle = - waggle;
				else
					waggle = 0;

				standby_timer--;
				DPRINT("standby_timer %u  \r", standby_timer);
				if (standby_timer == 6)
				{
					state_flags._.save_origin = 1;
				}
				else if (standby_timer == 2)
				{
					dcm_flags._.dead_reckon_enable = 1;
				}
				else if (standby_timer <= 0)
				{
					ent_manualS();
				}
			}
		}
		else
		{
			waggle = 0;
		}
	}
	else
	{
		waggle = 0;
	}
}

#ifdef CATAPULT_LAUNCH_ENABLE
boolean launch_enabled(void)
{
	return (udb_pwIn[LAUNCH_ARM_INPUT_CHANNEL] > 3000);
}

//  State: catapult launch armed
//  entered only from manualS if (radio_on and gear_up and nav_capable and switch_home)
static void cat_armedS(void)
{
	// transition to manual if flight_mode_switch no longer in waypoint mode
	// or link lost or gps lost
	if (flight_mode_switch_manual() | !udb_flags._.radio_on | !dcm_flags._.nav_capable) {
		led_off(LED_ORANGE);
		ent_manualS();
	}

	// transition to waypointS iff launch detected
	else if (dcm_flags._.launch_detected) {
		led_off(LED_ORANGE);
		ent_cat_delayS();
	}
}

// State: catapult launch delay
// entered from cat_armedS when launch_detected
static void cat_delayS(void)
{
	// transition to manual if flight_mode_switch no longer in waypoint mode
	// or link lost or gps lost
	if (flight_mode_switch_manual() | !udb_flags._.radio_on | !dcm_flags._.nav_capable)
	{
		led_off(LED_ORANGE);
		ent_manualS();
	}
	else if (--launch_timer == 0)
	{
		DPRINT("delayCheck = %u\r\n", delayCheck);
		ent_waypointS();
	}
}
#endif // CATAPULT_LAUNCH_ENABLE

static void manualS(void)
{
	if (udb_flags._.radio_on)
	{
#ifdef CATAPULT_LAUNCH_ENABLE
		if (launch_enabled() & flight_mode_switch_waypoints() & dcm_flags._.nav_capable)
			ent_cat_armedS();
		else
#endif
		if (flight_mode_switch_waypoints() & dcm_flags._.nav_capable)
			ent_waypointS();
		else if (flight_mode_switch_stabilize())
			ent_stabilizedS();
	}
	else
	{
		if (dcm_flags._.nav_capable)
		{
			DPRINT("manualS() calling ent_returnS()\r\n");
			ent_returnS();
		}
		else
		{
			ent_stabilizedS();
		}
	}
}

static void stabilizedS(void)
{
	if (udb_flags._.radio_on)
	{
#ifdef CATAPULT_LAUNCH_ENABLE
		if (launch_enabled() & flight_mode_switch_waypoints() & dcm_flags._.nav_capable)
			ent_cat_armedS();
		else
#endif
		if (flight_mode_switch_waypoints() & dcm_flags._.nav_capable)
			ent_waypointS();
		else if (flight_mode_switch_manual())
			ent_manualS();
	}
	else
	{
		if (dcm_flags._.nav_capable)
		{
			DPRINT("stabilizedS() calling ent_returnS()\r\n");
			ent_returnS();
		}
	}
}

static void waypointS(void)
{
	udb_led_toggle(LED_RED);

	if (udb_flags._.radio_on)
	{
		if (flight_mode_switch_manual())
			ent_manualS();
		else if (flight_mode_switch_stabilize())
			ent_stabilizedS();
	}
	else
	{
		DPRINT("waypointS() calling ent_returnS()\r\n");
		ent_returnS();
	}
}

static void returnS(void)
{
	if (udb_flags._.radio_on)
	{
		if (flight_mode_switch_manual())
			ent_manualS();
		else if (flight_mode_switch_stabilize())
			ent_stabilizedS();
		else if (flight_mode_switch_waypoints() & dcm_flags._.nav_capable)
			ent_waypointS();
	}
	else
	{
#if (FAILSAFE_HOLD == 1)
		state_flags._.rtl_hold = 1;
#endif
	}
}
