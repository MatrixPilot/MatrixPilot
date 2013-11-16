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


#ifndef MAVLINKMP_H
#define MAVLINKMP_H


#include "mavlink_options.h"
#include "../MAVLink/include/matrixpilot_mavlink_bridge_header.h"

// Setting MAVLINK_TEST_ENCODE_DECODE to 1, will replace the normal code that sends MAVLink messages with
// a test suite.  The inserted code will self-test every message type to encode packets, de-code packets,
// and it will then check that the results match. The code reports a pass rate and fail rate
// out of the serial port (sent as normal ascii). There should never be any fails. The code
// runs purely within the UAV DevBoard so this purely tests software, not the communication links.
// Normal default is to set MAVLINK_TEST_ENCODE_DECODE to 0

// This testing section of code only compiles if you set the C-Compiler to use the "Large memory code model"
// In MPLAB IDE, select "Project / Build Options / Project", then select Tab MPLAB C30. Then select the
// drop down menu called "Categores" and select "Memory Model". Tick "Large Code Model" instead of
// "Default Code Model". i.e. The test code will need more than 28K of ROM.
#define MAVLINK_TEST_ENCODE_DECODE  0

#if (MAVLINK_TEST_ENCODE_DECODE == 0)
// The following Macro enables MAVLink packets to be sent in one call to the serial driver
// rather than character by character.
#define MAVLINK_SEND_UART_BYTES mavlink_serial_send
int16_t mavlink_serial_send(mavlink_channel_t chan, uint8_t buf[], uint16_t len);
#endif

#include "../MAVLink/include/matrixpilot/mavlink.h"


typedef struct mavlink_flag_bits {
//	uint16_t unused                         : 2;
	uint16_t mavlink_send_specific_variable : 1;
	uint16_t mavlink_send_variables         : 1;
	uint16_t mavlink_send_waypoint_count    : 1;
	uint16_t mavlink_sending_waypoints      : 1;
	uint16_t mavlink_receiving_waypoints    : 1;
	uint16_t mavlink_send_specific_waypoint : 1;

	uint16_t mavlink_request_specific_waypoint : 1;

	uint16_t mavlink_send_waypoint_reached  : 1;
	uint16_t mavlink_send_waypoint_changed  : 1;

} mavlink_flags_t;

//void send_text(uint8_t text[]);
boolean mavlink_check_target(uint8_t target_system, uint8_t target_component);


#endif // MAVLINKMP_H
