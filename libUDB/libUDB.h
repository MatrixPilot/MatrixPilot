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

#include <stdint.h>
#define _ADDED_C_LIB 1 // Needed to get vsnprintf()
#include <stdio.h>

#include "options.h"

////////////////////////////////////////////////////////////////////////////////
// Set Up Board Type
// The UDB4, UDB5, or AUAV3 definition now comes from the project, or if not
// set in the project can be specified here.
// See the MatrixPilot wiki for more details on different board types.
#ifdef UDB4
#define BOARD_TYPE                          UDB4_BOARD
#endif
#ifdef UDB5
#define BOARD_TYPE                          UDB5_BOARD
#endif
#ifdef AUAV3
#define BOARD_TYPE                          AUAV3_BOARD
#endif

#ifndef BOARD_TYPE
#pragma warning BOARD_TYPE defaulting to UDB4_BOARD
#define BOARD_TYPE                          UDB4_BOARD
#endif


#ifdef USE_DEBUG_IO
#define DPRINT printf
#else
#define DPRINT(args...)
#endif

#if (SILSIM == 1)
#include "SIL-udb.h"
#else
#if (BOARD_TYPE == AUAV4_BOARD)
//#define fractional float
//#define fractional int
#include "..\Tools\MatrixPilot-SIL\SIL-dsp.h"
#else
#include <dsp.h>
#endif
#endif

#include "fixDeps.h"
#include "libUDB_defines.h"
#include "nv_memory_options.h"

////////////////////////////////////////////////////////////////////////////////
// libUDB.h defines the API for accessing the UDB hardware through libUDB.
// 
// This is the lowest-level component of MatrixPilot, and should not reference
// anything from the higher-level components.  This library is designed to be
// useful in its own right, independent of libDCM or MatrixPilot.
//
// libUDB requires an options.h file be provided that defines at least the
// following constants:
// 
// #define NUM_INPUTS
// #define NUM_OUTPUTS
// 
// #define FAILSAFE_INPUT_CHANNEL
// #define FAILSAFE_INPUT_MIN
// #define FAILSAFE_INPUT_MAX
// 
// #define NORADIO
// #define SERVOSAT


////////////////////////////////////////////////////////////////////////////////
// Initialize the UDB

// Call this first soon after the board boots up
void mcu_init(void);

// Call this once soon after the board boots up
void udb_init(void);

// Start the UDB running
// Once you have everything else set up, call udb_run().
// This function will not return.
// From this point on, everything is event-driven.
// Your code should respond to the Callbacks below.
void udb_run(void);


////////////////////////////////////////////////////////////////////////////////
// Run Background Tasks

// Implement this callback to perform periodic background tasks (high priority).
// It is called once every 0.5 seconds, and must return quickly. (No printf!)
void udb_background_callback_periodic(void);

// Call this function to trigger the udb_background_callback_triggered() function
// from a low priority ISR.
void udb_background_trigger(void);

// Implement this callback to respond to udb_background_trigger() in
// the background (low priority)
void udb_background_callback_triggered(void);

// This function returns the current CPU load as an integer percentage value
// from 0-100.
uint8_t udb_cpu_load(void);


////////////////////////////////////////////////////////////////////////////////
// Radio Inputs / Servo Outputs

// These are the values of the radio input channels.  Each channel will be a
// value between approximately 2000 and 4000, with 3000 being the center.
// Treat udb_pwIn values as readonly.
extern int16_t udb_pwIn[];                  // pulse widths of radio inputs

// These are the recorded trim values of the radio input channels.
// These values are recorded when you call the udb_servo_record_trims()
// function.
// Each channel will be a value between approximately 2000 and 4000.
// Treat udb_pwTrim values as readonly.
extern int16_t udb_pwTrim[];                // initial pulse widths for trimming

// These are the servo channel values that will be sent out to the servos.
// Set these values in your implementation of the udb_servo_callback_prepare_outputs()
// callback.
// Each channel should be set to a value between 2000 and 4000.
extern int16_t udb_pwOut[];                 // pulse widths for servo outputs

// This read-only value holds flags that tell you, among other things,
// whether the receiver is currently receiving values from the transmitter.
extern union udb_fbts_byte { struct udb_flag_bits _; int8_t B; } udb_flags;

// This takes a servo out value, and clips it to be within
// 3000-1000*SERVOSAT and 3000+1000*SERVOSAT (2000-4000 by default).
int16_t  udb_servo_pulsesat(int32_t pw);

// Call this funtion once at some point soon after
// the UDB has booted up and the radio is on.
void udb_servo_record_trims(void);

// Functions only included with nv memory.
#if(USE_NV_MEMORY == 1)
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
#endif

// Implement this callback to prepare the pwOut values.
// It is called at HEARTBEAT_HZ at a low priority.
void udb_servo_callback_prepare_outputs(void);

// Called immediately whenever the radio_on flag is set to 0
void udb_callback_radio_did_turn_off(void);

// Call this function to set the digital output to 0 or 1.
// This can be used to do things like triggering cameras, turning on
// lights, etc.
void udb_set_action_state(boolean newValue);


////////////////////////////////////////////////////////////////////////////////
// Raw Accelerometer and Gyroscope(rate) Values
extern struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel;// x, y, and z accelerometer channels
extern struct ADchannel udb_xrate, udb_yrate, udb_zrate;   // x, y, and z gyro channels
extern struct ADchannel udb_vref;                          // reference voltage
extern struct ADchannel udb_analogInputs[];
//extern struct ADchannel udb_rssi;
//extern struct ADchannel udb_vcc;
//extern struct ADchannel udb_5v;

#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
extern union longww battery_current;        // battery_current._.W1 is in tenths of Amps
extern union longww battery_mAh_used;       // battery_mAh_used._.W1 is in mAh
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
extern union longww battery_voltage;        // battery_voltage._.W1 is in tenths of Volts
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
extern uint8_t rc_signal_strength;          // rc_signal_strength is 0-100 as percent of full signal
#endif

#if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
#include "airspeedPitot.h"
extern AirspeedPitot airspeedPitot;
#endif


// Calibrate the sensors
// Call this function once, soon after booting up, after a few seconds of
// holding the UDB very still.
void udb_a2d_record_offsets(void);
void udb_callback_read_sensors(void);


////////////////////////////////////////////////////////////////////////////////
// LEDs
// Use this to toggle an LED.  Use the LED definition from the Config*.h files,
// for example udb_led_toggle(LED_RED);
#define udb_led_toggle(x)                   ((x) = !(x))


////////////////////////////////////////////////////////////////////////////////
// GPS IO

// Set the GPS serial data rate.
void udb_gps_set_rate(int32_t rate);
boolean udb_gps_check_rate(int32_t rate);  // returns true if the rate arg is the current rate

// Call this function to initiate sending a data to the GPS
void udb_gps_start_sending_data(void);

// Implement this callback to tell the UDB what byte is next to send on the GPS.
// Return -1 to stop sending data.
int16_t udb_gps_callback_get_byte_to_send(void);

// Implement this callback to handle receiving a byte from the GPS
void udb_gps_callback_received_byte(uint8_t rxchar);


////////////////////////////////////////////////////////////////////////////////
// Serial IO

// Set the serial port data rate.  Use the UDB_BAUD_* constants defined in the Config*.h
// files.
void udb_serial_set_rate(int32_t rate);
boolean udb_serial_check_rate(int32_t rate);// returns true if the rate arg is the current rate

// Call this function to initiate sending a data to the serial port
void udb_serial_start_sending_data(void);

// Implement this callback to tell the UDB what byte is next to send on the serial port.
// Return -1 to stop sending data.
int16_t udb_serial_callback_get_byte_to_send(void);

// Implement this callback to handle receiving a byte from the serial port
void udb_serial_callback_received_byte(uint8_t rxchar);


////////////////////////////////////////////////////////////////////////////////
// EEPROM (Supported on UDB4 and UDB5 only)

// Write 1 byte to eeprom at address, or read 1 byte from address in eeprom into data
void eeprom_ByteWrite(uint16_t address, uint8_t data);
void eeprom_ByteRead(uint16_t address, uint8_t *data);

// Write numbytes of data to eeprom, starting at address. The write area can not span a
// page boundry.  Pages start on addresses of multiples of 64.
// Read numbytes of data from address in eeprom into data.  Note that there is no 1-page
// limit for sequential reads as there is for page writes.
void eeprom_PageWrite(uint16_t address, uint8_t *data, uint8_t numbytes);
void eeprom_SequentialRead(uint16_t address, uint8_t *data, uint16_t numbytes);


#endif // LIB_UDB_H
