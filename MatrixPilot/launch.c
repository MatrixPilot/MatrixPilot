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

#ifdef USE_DEBUG_IO
#define DPRINT printf
#else
#define DPRINT(args...)
#endif

//extern union fbts_int flags;


void assignFlightModePerModeSwitch(void);

AIRCRAFT_FLIGHT_MODE_STATE getAircraftState(void);

#if (CATAPULT_LAUNCH_INPUT_CHANNEL != CHANNEL_UNUSED)
uint16_t launch_throttle_delay_timeout = 0, launch_trim_timeout = 0, launch_success_check_timeout = 0;
static void udb_background_callback_launch(void);
boolean isLauncherArmed(void);
boolean isLauncherLaunching(void);
static void initLauncher(void);
static boolean wasLaunchSwitchJustEnabled(boolean doClearSwitchState);

static boolean isInFlightState(void)
{
  AIRCRAFT_FLIGHT_MODE_STATE flightMode = getAircraftState();
  return ((flightMode >= smFLYING_MANUAL) && (flightMode <= smFLYING_SIGNAL_LOST__RETURNING_HOME));
}

static boolean wasLaunchSwitchJustEnabled(boolean doClearSwitchState)
{
 // returns true if the launch switch was just enabled anytime after calibration
  static boolean hasBeenTrueBefore = false;
  if (doClearSwitchState)
  {
    hasBeenTrueBefore = false;
    return false;
  }

  if (launch_mode_switch_enabled() && (hasBeenTrueBefore == false))
  {
    hasBeenTrueBefore = true;
    return true;
  }
  return false;
}

static void initLauncher(void)
{
  flags._.disable_throttle = 0;
  dcm_flags._.launch_detected = 0;
  launch_throttle_delay_timeout = 0;
  launch_trim_timeout = 0;
  launch_success_check_timeout = 0;
  wasLaunchSwitchJustEnabled(true);
}

boolean isLauncherArmed(void)
{
  // are we on the launch pad
  return ((flags._.disable_throttle == 1) &&
          (flags._.launching == 0) &&
          (dcm_flags._.launch_detected == 0) &&
          (launch_throttle_delay_timeout == LAUNCH_TIMER_THROTTLE_DELAY) &&
          (launch_trim_timeout == LAUNCH_TIMER_PITCH_UP));
}

boolean isLauncherLaunching(void)
{
  // are we in the process of launching off the pad
  boolean launchingNoThrottle = ((flags._.disable_throttle == 1) && (launch_throttle_delay_timeout > 0));
  boolean launchingUpTrim = ((flags._.launching == 1) && (launch_trim_timeout > 0));
  
  return ((dcm_flags._.launch_detected == 1) && (launchingNoThrottle || launchingUpTrim) );
}

static void udb_background_callback_launch(void)
{
  // called at 40Hz
  if (wasLaunchSwitchJustEnabled(false))
  {
    // anytime after calibration, if launch is enabled then we're armed.
    // this can only happen once per boot and only after a cal
    flags._.disable_throttle = 1;
    flags._.launching = 0;
    dcm_flags._.launch_detected  = 0;
    launch_throttle_delay_timeout = LAUNCH_TIMER_THROTTLE_DELAY;
    launch_trim_timeout = LAUNCH_TIMER_PITCH_UP;
    launch_success_check_timeout = LAUNCH_TIMER_SUCCESS_CHECK;
  }
  else if (isLauncherLaunching())
  {
    // We are currently in the act of launching
    
    if (launch_throttle_delay_timeout == LAUNCH_TIMER_THROTTLE_DELAY)
    {
      // launch was just detected
      flags._.launching = 1;
    }

    // in the process of launching, delaying the throttle
    launch_throttle_delay_timeout--;
    if (launch_throttle_delay_timeout <= 0)
    {
      //  get that motor turn'n!
      flags._.disable_throttle = 0;
    }

    launch_trim_timeout--;
    if (launch_trim_timeout <= 0)
    {
      // we are now safely in the air
      flags._.launching = 0;
    }
  }
  else if ((dcm_flags._.launch_detected == 1) && (launch_success_check_timeout > 0))
  {
    launch_success_check_timeout--;
    if (launch_success_check_timeout == 0)
    {
      // TODO: do a sanity check of ground speed != 0 to determine if launch was successful. If launch
      // TODO: was not successful, it is presumed we ate dirt so lets shut the throttle off again.
      // if (forward_ground_speed.WW < ???)
      //  flags._.disable_throttle = 1;
    }
  }
}
#endif
