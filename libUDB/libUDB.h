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


#ifndef LIB_UDB_H
#define LIB_UDB_H

#define _ADDED_C_LIB 1 // Needed to get vsnprintf()
#include <stdio.h>

#include "options.h"
//#include "options_quad.h"

#ifdef PX4
#include "../libSTM/libSTM.h"
#endif

#if (SIL == 1)
#define SILSIM                              1
#endif

#if (WIN == 1 || NIX == 1)
#define inline __inline
#define SILSIM                              1
#undef  HILSIM
#define HILSIM                              1
#undef  MODE_SWITCH_TWO_POSITION
#define MODE_SWITCH_TWO_POSITION            0
//#undef  USE_TELELOG
//#define USE_TELELOG                         0
#undef  USE_USB
#define USE_USB                             0
#undef  USE_MSD
#define USE_MSD                             0
#undef  RECORD_FREE_STACK_SPACE
#define RECORD_FREE_STACK_SPACE             0
#undef  FAILSAFE_INPUT_MIN
#define FAILSAFE_INPUT_MIN                  1500
#include "../Tools/MatrixPilot-SIL/SIL-udb.h"
#else
#define SILSIM                              0
#include <dsp.h>
#endif // (WIN == 1 || NIX == 1)

////////////////////////////////////////////////////////////////////////////////
// Set Up Board Type
// The UDB4, UDB5, or AUAV3 definition now comes from the project, or if not
// set in the project can be specified here.
// See the MatrixPilot wiki for more details on different board types.
#ifdef UDB4
#define BOARD_TYPE                          UDB4_BOARD
#undef  USE_USB
#define USE_USB                             0
#undef  USE_MSD
#define USE_MSD                             0
#endif
#ifdef UDB5
#define BOARD_TYPE                          UDB5_BOARD
#undef  USE_USB
#define USE_USB                             0
#undef  USE_MSD
#define USE_MSD                             0
#endif
#ifdef AUAV3
#define BOARD_TYPE                          AUAV3_BOARD
#include "options_auav3.h"
#endif
#ifdef PX4
#define BOARD_TYPE                          PX4_BOARD
#include "libSTM.h"
#endif

#ifndef BOARD_TYPE
#if (SILSIM == 0)
#warning BOARD_TYPE defaulting to UDB4_BOARD
#endif // SILSIM
#define BOARD_TYPE                          UDB4_BOARD
#endif // BOARD_TYPE

#ifdef USE_DEBUG_IO
#ifdef USE_MAVLINK_IO
void mav_printf(const char * format, ...);
#define DPRINT mav_printf
#else
#define DPRINT printf
#endif // USE_MAVLINK_IO
#else
#define DPRINT(args, ...)
#endif // USE_DEBUG_IO

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "fixDeps.h"
#endif // DOXYGEN_SHOULD_SKIP_THIS
#include "libUDB_defines.h"
#include "options_nv_memory.h"

////////////////////////////////////////////////////////////////////////////////
/// libUDB.h defines the API for accessing the UDB hardware through libUDB.
/// 
/// This is the lowest-level component of MatrixPilot, and should not reference
/// anything from the higher-level components.  This library is designed to be
/// useful in its own right, independent of libDCM or MatrixPilot.
///
/// libUDB requires an options.h file be provided that defines at least the
/// following constants:
/// 
/// #define NUM_INPUTS
/// #define NUM_OUTPUTS
/// 
/// #define FAILSAFE_INPUT_CHANNEL
/// #define FAILSAFE_INPUT_MIN
/// #define FAILSAFE_INPUT_MAX
/// 
/// #define NORADIO
/// #define SERVOSAT
///


////////////////////////////////////////////////////////////////////////////////
// Initialize the UDB

//! Call this first soon after the board boots up
void mcu_init(void);

//! Call this once soon after the board boots up
void udb_init(void);

//! Call this to clear the main interrupt mask  (start the irq's)
void udb_init_irq(void);

// Start the UDB running
// Once you have everything else set up, call udb_run().
// This function will not return.
// From this point on, everything is event-driven.
// Your code should respond to the Callbacks below.
void udb_run(void);

//int setjmp(void);

////////////////////////////////////////////////////////////////////////////////
// Run Background Tasks

//! Implement this callback to perform periodic background tasks (high priority).
//! It is called at 40 Hertz and must return quickly. (No printf!)
void udb_heartbeat_40hz_callback(void);

//! Implement this callback to prepare the pwOut values.
//! It is called at HEARTBEAT_HZ at a low priority.
void udb_heartbeat_callback(void);

typedef void (*background_callback)(void);

//! Trigger the background_callback() functions from a low priority ISR.
void udb_background_trigger(background_callback callback);
void udb_background_trigger_pulse(background_callback callback);

//! Return the current CPU load as an integer percentage value from 0-100.
uint8_t udb_cpu_load(void);
void cpu_load_calc(void);


////////////////////////////////////////////////////////////////////////////////
// Radio Inputs / Servo Outputs

//! These are the values of the radio input channels.  Each channel will be a
//! value between approximately 2000 and 4000, with 3000 being the center.
//! Treat udb_pwIn values as readonly.
extern int16_t udb_pwIn[];                  // pulse widths of radio inputs

//! These are the recorded trim values of the radio input channels.
//! These values are recorded when you call the udb_servo_record_trims()
//! function.
//! Each channel will be a value between approximately 2000 and 4000.
//! Treat udb_pwTrim values as readonly.
extern int16_t udb_pwTrim[];                // initial pulse widths for trimming

//! These are the servo channel values that will be sent out to the servos.
//! Set these values in your implementation of the udb_heartbeat_callback()
//! Each channel should be set to a value between 2000 and 4000.
extern int16_t udb_pwOut[];                 // pulse widths for servo outputs

//! This read-only value holds flags that tell you, among other things,
//! whether the receiver is currently receiving values from the transmitter.
extern union udb_fbts_byte { struct udb_flag_bits _; int8_t B; } udb_flags;

//! Call this funtion once at some point soon after
//! the UDB has booted up and the radio is on.
void udb_servo_record_trims(void);

//! Called immediately whenever the radio_on flag is set to 0
void udb_callback_radio_did_turn_off(void);     // Callback

//! Call this function to set the digital output to 0 or 1.
//! This can be used to do things like triggering cameras, turning on lights, etc.
void udb_set_action_state(boolean newValue);

// Functions only included with nv memory.
#if (USE_NV_MEMORY == 1)
// Call this funtion to skip doing radio trim calibration
void udb_skip_radio_trim(boolean);
void udb_skip_imu_calibration(boolean);

typedef struct tagUDB_SKIP_FLAGS
{
	uint16_t skip_imu_cal           : 1;
	uint16_t skip_radio_trim        : 1;
	uint16_t unused                 : 6;
} UDB_SKIP_FLAGS;

extern UDB_SKIP_FLAGS udb_skip_flags;
#endif // (USE_NV_MEMORY == 1)

////////////////////////////////////////////////////////////////////////////////

#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)
extern union longww battery_current;        // battery_current._.W1 is in tenths of Amps
extern union longww battery_mAh_used;       // battery_mAh_used._.W1 is in mAh
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)
extern union longww battery_voltage;        // battery_voltage._.W1 is in tenths of Volts
#endif

#if (ANALOG_VOLTAGE2_INPUT_CHANNEL != CHANNEL_UNUSED)
extern union longww battery_voltage2;        // battery_voltage2._.W1 is in tenths of Volts
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED || RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
extern uint8_t rc_signal_strength;          // rc_signal_strength is 0-100 as percent of full signal
#endif


////////////////////////////////////////////////////////////////////////////////
// LEDs
// Use this to toggle an LED.  Use the LED definition from the Config*.h files,
// for example udb_led_toggle(LED_RED);
#if (PX4 == 1 || SILSIM == 1)
void udb_led_toggle(uint8_t x);
void led_on(uint8_t x);
void led_off(uint8_t x);
#else
#define udb_led_toggle(x)               ((x) = !(x))
//#define led_on(x)                       ((x) = 0)
//#define led_off(x)                      ((x) = 1)
#define led_on(x)                       ((x) = LED_ON)
#define led_off(x)                      ((x) = LED_OFF)
#endif


#endif // LIB_UDB_H
