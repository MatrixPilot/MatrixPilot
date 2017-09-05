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


// The Coordinate Frame and Dimensional Units of Mavlink are
// explained in detail at this web URL:-
// http://pixhawk.ethz.ch/wiki/software/coordinate_frame
// An abreviated summary is:
// Mavlink Aviation  X Axis is the UDB Aviation Y axis which is the fuselage axis.
// Mavlink Avitation Y axis is out of the right wing, and so is the negative of the UDB X Axis
// Mavlink Aviation  Z axis is downward from the plane, and so is the same as UDB Z axis.
// Mavlink Yaw is positive to the right (same as UDB)
// Pitch is positive when the front of the plane pitches up from horizontal (opposite of UDB)
// Roll is positive to the right of the plane (same as UDB)
// So angles follow the "right hand rule"

// MatrixPilot uses the extra data streams as
//    MAV_DATA_STREAM_EXTRA1 = SERIAL UDB EXTRA formated data
//    MAV_DATA_STREAM_EXTRA2 = Scaled position sensor messages (ALTITUDES / AIRSPEEDS)
//    MAV_DATA_STREAM_EXTRA3 not assigned yet


#include "../MatrixPilot/defines.h"
#include "../MatrixPilot/states.h"
#include "options_mavlink.h"

#if (USE_MAVLINK == 1)

#ifndef MAVLINK_BAUD
#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)             //support the current method for configuring Mavlink
#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE 57600 // default
#warning SERIAL_BAUDRATE set to default value of 57600 bps
#endif
#define MAVLINK_BAUD                        SERIAL_BAUDRATE
#endif
#endif

#include "MAVLink.h"
#include "MAVParams.h"
#include "MAVMission.h"
#include "MAVFlexiFunctions.h"
#include "MAVUDBExtra.h"
#include "../MAVLink/MAVFTP.h"

//#if (SILSIM != 1)
#include "../libUDB/libUDB.h" // Needed for access to RCON
//#endif
//#include "../libDCM/libDCM_internal.h" // Needed for access to internal DCM value
#include "../libDCM/rmat.h" // Needed for access to internal DCM value
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/estAltitude.h"
#include "../libDCM/mathlibNAV.h"
#include "../libUDB/servoOut.h"
#include "../libUDB/serialIO.h"
#include "../libUDB/ADchannel.h"
#include "../libUDB/events.h"
#include "telemetry_log.h"
#include "euler_angles.h"
#include "config.h"
#include <string.h>
#include <stdarg.h>
#include <math.h>

#if (USE_NV_MEMORY == 1)
#include "parameter_datatypes.h"
#include "data_services.h"
#endif

#if (MAVLINK_TEST_ENCODE_DECODE == 1)
mavlink_message_t last_msg;
#define _ADDED_C_LIB 1 // Needed to get vsnprintf()
#include <stdio.h>
#include <stdarg.h>
#define MAVLINK_TEST_MESSAGE_SIZE 100
uint8_t mavlink_test_message_buffer[MAVLINK_TEST_MESSAGE_SIZE];
int16_t mavlink_tests_pass = 0;
int16_t mavlink_tests_fail = 0;
char mavlink_test_first_pass_flag = 1;
mavlink_status_t r_mavlink_status;

#define MAVLINK_ASSERT(exp) \
	if (!(exp)) \
	{ \
		printf("MAVLink Test Fail: " \
		       "at %s, line %d.\r\n", __FILE__, __LINE__); \
		mavlink_tests_fail++; \
	} else { \
		mavlink_tests_pass++; \
	}

#include "../MAVLink/include/matrixpilot/testsuite.h"
#endif // (MAVLINK_TEST_ENCODE_DECODE == 1)

mavlink_status_t m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS];

#define SERIAL_BUFFER_SIZE  MAVLINK_MAX_PACKET_LEN
#define BYTE_CIR_16_TO_RAD  ((2.0 * 3.14159265) / 65536.0) // Convert 16 bit byte circular to radians

mavlink_flags_t mavlink_flags;
mavlink_system_t mavlink_system;

static uint16_t mavlink_process_message_handle = INVALID_HANDLE;
static uint8_t handling_of_message_completed = true;

static uint8_t mavlink_counter_40hz = 0;
static uint64_t usec = 0; // A measure of time in microseconds (should be from Unix Epoch).
static uint32_t msec = 0; // A measure of time in microseconds (should be from Unix Epoch).

static int16_t sb_index = 0;
static int16_t end_index = 0;
static char serial_interrupt_stopped = 1;
static uint8_t serial_buffer[SERIAL_BUFFER_SIZE];

static uint8_t streamRates[MAV_DATA_STREAM_ENUM_END];
static uint16_t mavlink_command_ack_command = 0;
static boolean mavlink_send_command_ack = false;
static uint16_t mavlink_command_ack_result = 0;

static void handleMessage(void);
#if (USE_NV_MEMORY == 1)
// callback for when nv memory storage is complete
static inline void preflight_storage_complete_callback(boolean success);
#endif // (USE_NV_MEMORY == 1)


void mavlink_init(void)
{
	int16_t index;

	udb_init_USART(&mavlink_callback_get_byte_to_send, &mavlink_callback_received_byte);
	udb_serial_set_rate(MAVLINK_BAUD);
	mavlink_process_message_handle = register_event_p(&handleMessage, EVENT_PRIORITY_MEDIUM);
	mavlink_system.sysid = MAVLINK_SYSID; // System ID, 1-255, ID of your Plane for GCS
	mavlink_system.compid = 1; // Component/Subsystem ID,  (1-255) MatrixPilot on UDB is component 1.

	// Fill stream rates array with zeros to default all streams off;
	for (index = 0; index < MAV_DATA_STREAM_ENUM_END; index++)
		streamRates[index] = 0;

	// QGroundControl GCS lets user send message to increase stream rate
	streamRates[MAV_DATA_STREAM_RC_CHANNELS] = MAVLINK_RATE_RC_CHAN;
	streamRates[MAV_DATA_STREAM_RAW_SENSORS] = MAVLINK_RATE_RAW_SENSORS;
	streamRates[MAV_DATA_STREAM_POSITION]    = MAVLINK_RATE_POSITION;
	streamRates[MAV_DATA_STREAM_EXTRA1]      = MAVLINK_RATE_SUE;
	streamRates[MAV_DATA_STREAM_EXTRA2]      = MAVLINK_RATE_POSITION_SENSORS;
}

//void init_serial(void)
//{
//#ifndef SERIAL_BAUDRATE
//#define SERIAL_BAUDRATE 57600 // default
//#pragma warning ("SERIAL_BAUDRATE set to default value of 57600 bps for MAVLink") // VC warns 'unknown user warning type', arm-gnu-eabi warns, unknown pragma
////#warning "SERIAL_BAUDRATE set to default value of 57600 bps for MAVLink" // xc16 uses this syntax (but VC throws fatal error)
//#endif
//	udb_serial_set_rate(SERIAL_BAUDRATE);
//	mavlink_init();
//}

//void telemetry_restart(void)
//{
//
//}

int16_t mavlink_callback_get_byte_to_send(void)
{
	if (sb_index < end_index && sb_index < SERIAL_BUFFER_SIZE) // ensure never end up racing thru memory.
	{
		uint8_t txchar = serial_buffer[sb_index++];
		return txchar;
	}
	else
	{
		serial_interrupt_stopped = 1;
	}
	return -1;
}

//int16_t mavlink_serial_send(mavlink_channel_t UNUSED(chan), uint8_t buf[], uint16_t len)
int16_t mavlink_serial_send(mavlink_channel_t UNUSED(chan), const uint8_t buf[], uint16_t len) // RobD
// Note: Channel Number, chan, is currently ignored.
{
	int16_t start_index;
	int16_t remaining;

#if (USE_TELELOG == 1)
//printf("calling log_telemetry with %u bytes\r\n", len);
	log_telemetry((const char*)buf, len);
#endif // USE_TELELOG

	// Note at the moment, all channels lead to the one serial port
	if (serial_interrupt_stopped == 1)
	{
		sb_index = 0;
		end_index = 0;
	}
	start_index = end_index;
	remaining = SERIAL_BUFFER_SIZE - start_index;

//	printf("%u\r\n", remaining);

	if (len > remaining)
	{
		// Chuck away the entire packet, as sending partial packet
		// will break MAVLink CRC checks, and so receiver will throw it away anyway.
		return (-1);
	}
	if (remaining > 1)
	{
		memcpy(&serial_buffer[start_index], buf, len);
		end_index = start_index + len;
	}
	if (serial_interrupt_stopped == 1)
	{
		serial_interrupt_stopped = 0;
#if (SILSIM == 1)
		mavlink_start_sending_data();
#else
		udb_serial_start_sending_data();
#endif
	}
	return (1);
}

void mav_printf(const char* format, ...)
{
	char buf[200];
	va_list arglist;

	va_start(arglist, format);
	vsnprintf(buf, sizeof(buf), format, arglist);
	// mavlink_msg_statustext_send(MAVLINK_COMM_1, severity, text);
	// severity: Severity of status, 0 = info message, 255 = critical fault (uint8_t)
	mavlink_msg_statustext_send(MAVLINK_COMM_0, 0, buf);
	va_end(arglist);
}

#if (MAVLINK_TEST_ENCODE_DECODE == 1)
// add printf library when running tests to output ascii messages of test results
static void serial_output(const char* format, ...)
{
	int16_t remaining = 0;
	int16_t wrote = 0;
	va_list arglist;

	va_start(arglist, format);
	remaining = MAVLINK_TEST_MESSAGE_SIZE;
	wrote = vsnprintf((char*)(&mavlink_test_message_buffer[0]), (size_t)remaining, format, arglist);
	if (wrote > 0)
	{
		mavlink_serial_send(MAVLINK_COMM_0, &mavlink_test_message_buffer[0], (uint16_t)wrote);
//		printf("%s\r\n", mavlink_test_message_buffer);
	}
}
#endif // (MAVLINK_TEST_ENCODE_DECODE == 1)

#if (MAVLINK_TEST_ENCODE_DECODE == 1)
void mp_mavlink_transmit(uint8_t ch)
// This is a special version of the routine for testing MAVLink routines
// The incoming serial stream is parsed to reproduce a mavlink message.
// This will then be checked against the original message and results recorded
// using the MAVLINK_ASSERT macro.
{
	mavlink_parse_char(0, ch, &last_msg, &r_mavlink_status);
}
#else
void mp_mavlink_transmit(uint8_t ch)
// routine to send a single character used by MAVlink standard include routines.
// We forward to multi-byte sending routine so that firmware can interleave
// ascii debug messages with MAVLink binary messages without them overwriting the buffer.
{
//printf("mp_mavlink_transmit(%u)\r\n", ch);
	mavlink_serial_send(MAVLINK_COMM_0, &ch, 1);
}
#endif

void send_text(uint8_t text[])
{
	uint16_t index = 0;

	while (text[index++] != 0 && index < 80)
	{
		; // Do nothing, just measuring the length of the text
	}
//printf("send_text(%s) %u\r\n", text, index);
	mavlink_serial_send(MAVLINK_COMM_0, text, index - 1);
}

// A simple routine for sending a uint8_t number as 2 bytes of hexadecimal text
//static void send_uint8(uint8_t value)
//{
//	uint8_t temp;
//	temp = value >> 4; // Take upper half of hex int.
//	if (temp < 10)
//	{
//		mp_mavlink_transmit(temp + 0x30); //1,2,3,4,5,6,7,8,9
//	}
//	else
//	{
//		mp_mavlink_transmit(temp - 10 + 0x41); // A,B,C,D,E,F
//	}
//	temp = value & 0x0f; // Take lower half of hex int
//	if (temp < 10)
//	{
//		mp_mavlink_transmit(temp + 0x30); //1,2,3,4,5,6,7,8,9
//	}
//	else
//	{
//		mp_mavlink_transmit(temp - 10 + 0x41); // A,B,C,D,E,F
//	}
//}


////////////////////////////////////////////////////////////////////////////////////////
//
// MAIN MATRIXPILOT MAVLINK CODE FOR RECEIVING COMMANDS FROM THE GROUND CONTROL STATION
//

static mavlink_message_t msg[2];
static uint8_t mavlink_message_index = 0;
static mavlink_status_t r_mavlink_status;

//void udb_serial_callback_received_byte(uint8_t rxchar)
void mavlink_callback_received_byte(uint8_t rxchar)
{
//	DPRINT("%u \r\n", rxchar);

	if (mavlink_parse_char(0, rxchar, &msg[mavlink_message_index], &r_mavlink_status))
	{
		// Check that handling of previous message has completed before calling again
		if (handling_of_message_completed == true)
		{
			// Switch between incoming message buffers
			if (mavlink_message_index == 0) mavlink_message_index = 1;
			else mavlink_message_index = 0;
			handling_of_message_completed = false;
			trigger_event(mavlink_process_message_handle);
		}
	}
}

boolean mavlink_check_target(uint8_t target_system, uint8_t target_component)
{
	if ((target_system == mavlink_system.sysid)
	    // QGroundControl sends parameter refresh list to component 25 (regardless)
	    // But "Transmit" of parameter updates are sent using a specific component ID of 1 by QGroundControl.
	    // Only use mavlink_check_target if you expect all of the sysid, and compid to be correct.
	    && (target_component == mavlink_system.compid))
	{
		return false;
	}
	else
	{
		return false;
//		return true;        // TODO: fix this - RobD
	}
}

static void command_ack(uint16_t command, uint16_t result)
{
	if (mavlink_send_command_ack == false)
	{
		mavlink_command_ack_result = result;
		mavlink_command_ack_command = command;
		mavlink_send_command_ack = true;
	}
}

static void MAVLinkRequestDataStream(mavlink_message_t* handle_msg) // MAVLINK_MSG_ID_REQUEST_DATA_STREAM
{
	int16_t freq = 0; // packet frequency
	mavlink_request_data_stream_t packet;
	mavlink_msg_request_data_stream_decode(handle_msg, &packet);

	DPRINT("MAVLINK_MSG_ID_REQUEST_DATA_STREAM %u\r\n", handle_msg->msgid);
	//send_text((const uint8_t*) "Action: Request data stream\r\n");
	// QgroundControl sends data stream request to component ID 1, which is not our component for UDB.
	if (packet.target_system != mavlink_system.sysid) return;

	if (packet.start_stop == 0) freq = 0; // stop sending
	else if (packet.start_stop == 1) freq = packet.req_message_rate; // start sending
	else return;
	if (packet.req_stream_id == MAV_DATA_STREAM_ALL)
	{
		// Warning: mavproxy automatically sets all.  Do not include all here, it will overide defaults.
		streamRates[MAV_DATA_STREAM_RAW_SENSORS] = freq;
		streamRates[MAV_DATA_STREAM_RC_CHANNELS] = freq;
	}
	else
	{
		if (packet.req_stream_id < MAV_DATA_STREAM_ENUM_END)
			streamRates[packet.req_stream_id] = freq;
	}
}

void MAVLinkCommandLong(mavlink_message_t* handle_msg) // MAVLINK_MSG_ID_COMMAND_LONG
{
	mavlink_command_long_t packet;
	mavlink_msg_command_long_decode(handle_msg, &packet);

	DPRINT("MAVLINK_MSG_ID_COMMAND_LONG %u\r\n", handle_msg->msgid);
	//if (mavlink_check_target(packet.target, packet.target_component) == false) break;
	{
		switch (packet.command)
		{
			case MAV_CMD_PREFLIGHT_CALIBRATION:
				DPRINT("MAV_CMD_PREFLIGHT_CALIBRATION %u\r\n", packet.command);
				if (packet.param1 == 1)
				{
#if (USE_NV_MEMORY ==1)
					udb_skip_flags.skip_imu_cal = 0;
#endif // (USE_NV_MEMORY == 1)
					udb_a2d_record_offsets();
				}
				else if (packet.param4 == 1) //param4 = radio calibration
				{
					if (udb_flags._.radio_on == 1)
					{
						udb_servo_record_trims();
						command_ack(packet.command, MAV_CMD_ACK_OK);
					}
					else
						command_ack(packet.command, MAV_CMD_ACK_ERR_FAIL);
				}
				else
					command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
				break;
#if (USE_NV_MEMORY == 1)
			case MAV_CMD_PREFLIGHT_STORAGE:
				DPRINT("MAV_CMD_PREFLIGHT_STORAGE %u\r\n", packet.command);
				if (packet.param1 == MAV_PFS_CMD_WRITE_ALL) // 1
				{
					if (packet.param2 == MAV_PFS_CMD_WRITE_ALL)
						data_services_save_all(STORAGE_FLAG_STORE_CALIB | STORAGE_FLAG_STORE_WAYPOINTS, &preflight_storage_complete_callback);
					else
						data_services_save_all(STORAGE_FLAG_STORE_CALIB, &preflight_storage_complete_callback);
				}
				else if (packet.param1 == MAV_PFS_CMD_READ_ALL) // 0
				{
					if (packet.param2 == MAV_PFS_CMD_READ_ALL)
						data_services_load_all(STORAGE_FLAG_STORE_CALIB | STORAGE_FLAG_STORE_WAYPOINTS, &preflight_storage_complete_callback);
					else
						data_services_load_all(STORAGE_FLAG_STORE_CALIB, &preflight_storage_complete_callback);
				}
				else
					command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
				break;
			case MAV_CMD_PREFLIGHT_STORAGE_ADVANCED:
				DPRINT("MAV_CMD_PREFLIGHT_STORAGE_ADVANCED %u\r\n", packet.command);
				switch ((uint16_t)packet.param1)
				{
					case MAV_PFS_CMD_CLEAR_SPECIFIC:
						storage_clear_area(packet.param2, &preflight_storage_complete_callback);
						break;
					case MAV_PFS_CMD_WRITE_SPECIFIC:
						data_services_save_specific(packet.param2, &preflight_storage_complete_callback);
						break;
					case MAV_PFS_CMD_READ_SPECIFIC:
						data_services_load_specific(packet.param2, &preflight_storage_complete_callback);
						break;
					default:
						command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
						break;
				}
				break;
#else
			case 245: // MAV_CMD_PREFLIGHT_STORAGE:
				switch ((uint16_t)packet.param1)
				{
					case 0: // Read
						DPRINT("Read (ROM)\r\n");
						config_load();
						break;
					case 1: // Write
						DPRINT("Write (ROM)\r\n");
						config_save();
						break;
					default:
						DPRINT("245 packet.param1 %f packet.param2 %f\r\n", (double)packet.param1, (double)packet.param2);
						break;
				}
				break;
#endif // (USE_NV_MEMORY == 1)
			case 246: // halt
				DPRINT("Halt - packet.command %u\r\n", packet.command);
				break;
			case 22: // start
				DPRINT("Start - packet.command %u\r\n", packet.command);
				break;
			case 252: // land
				DPRINT("Land - packet.command %u\r\n", packet.command);
				break;
			default:
				DPRINT("packet.command %u\r\n", packet.command);
				command_ack(packet.command, MAV_CMD_ACK_ERR_NOT_SUPPORTED);
				break;
		}
	}
}

void MAVLinkSetMode(mavlink_message_t* handle_msg) // MAVLINK_MSG_ID_SET_MODE:
{
	mavlink_set_mode_t packet;

	DPRINT("MAVLINK_MSG_ID_SET_MODE %u\r\n", handle_msg->msgid);
	// send_text((uint8_t*) "Action: Specific Action Required\r\n");
	// decode
	mavlink_msg_set_mode_decode(handle_msg, &packet);
//	if (mavlink_check_target(packet.target_system, packet.target_component) == false) break;
	{
		switch (packet.base_mode)
		{
			case 192: // Manual
				DPRINT("Manual %u\r\n", packet.base_mode);
				break;
			case 208: // Manual/Stabilised
				DPRINT("Manual/Stabilised %u\r\n", packet.base_mode);
				break;
			case 216: // Manual/Guided
				DPRINT("Manual/Guided %u\r\n", packet.base_mode);
				break;
			case 156: // Auto
				DPRINT("Auto %u\r\n", packet.base_mode);
				break;
			case 65: // Disarm System
				DPRINT("Disarm System %u\r\n", packet.base_mode);
				break;
/*
			case MAV_ACTION_LAUNCH:
				//send_text((uint8_t*) "Action: Launch !\r\n");
				//DPRINT("Action: Launch !\r\n");
				//set_mode(TAKEOFF);
				break;
			case MAV_ACTION_RETURN:
				//send_text((uint8_t*) "Action: Return !\r\n");
				//DPRINT("Action: Return !\r\n");
				//set_mode(RTL);
				break;
			case MAV_ACTION_EMCY_LAND:
				//send_text((uint8_t*) "Action: Emergency Land !\r\n");
				//DPRINT("Action: Emergency Land !\r\n");
				//set_mode(LAND);
				break;
			case MAV_ACTION_HALT:
				//send_text((uint8_t*) "Action: Halt !\r\n");
				//DPRINT("Action: Halt !\r\n");
				//loiter_at_location();
				break;
			case MAV_ACTION_MOTORS_START:
			case MAV_ACTION_CONFIRM_KILL:
			case MAV_ACTION_EMCY_KILL:
			case MAV_ACTION_MOTORS_STOP:
			case MAV_ACTION_SHUTDOWN:
				//set_mode(MANUAL);
				break;
			case MAV_ACTION_CONTINUE:
				//process_next_command();
				break;
			case MAV_ACTION_SET_MANUAL:
				//set_mode(MANUAL);
				break;
			case MAV_ACTION_SET_AUTO:
				//set_mode(AUTO);
				break;
			case MAV_ACTION_STORAGE_READ:
				//send_text((uint8_t*)"Action: Storage Read\r\n");
				//DPRINT("Action: Storage Read\r\n");
				break;
			case MAV_ACTION_STORAGE_WRITE:
				//send_text((uint8_t*)"Action: Storage Write\r\n");
				//DPRINT("Action: Storage Write\r\n");
				break;
			case MAV_ACTION_CALIBRATE_RC:
				//send_text((uint8_t*)"Action: Calibrate RC\r\n");
				//DPRINT("Action: Calibrate RC\r\n");
				break;
			case MAV_ACTION_CALIBRATE_GYRO:
			case MAV_ACTION_CALIBRATE_MAG:
			case MAV_ACTION_CALIBRATE_ACC:
			case MAV_ACTION_CALIBRATE_PRESSURE:
			case MAV_ACTION_REBOOT:
				//startup_IMU_ground();
				break;
			case MAV_ACTION_REC_START: break;
			case MAV_ACTION_REC_PAUSE: break;
			case MAV_ACTION_REC_STOP: break;
			case MAV_ACTION_TAKEOFF:
				//send_text((uint8_t*)"Action: Take Off !\r\n");
				//DPRINT("Action: Take Off !\r\n");
				//set_mode(TAKEOFF);
				break;
			case MAV_ACTION_NAVIGATE:
				// send_text((uint8_t*)"Action: Navigate !\r\n");
				// DPRINT("Action: Navigate !\r\n");
				//set_mode(AUTO);
				break;
			case MAV_ACTION_LAND:
				//set_mode(LAND);
				break;
			case MAV_ACTION_LOITER:
				//set_mode(LOITER);
				break;
 */
			default:
				DPRINT("action: Specific Action Required %u\r\n", packet.base_mode);
				break;
		}
	}
}

// Portions of the following code in handlesmessage() are templated off source code written by James Goppert for the
// ArdupilotMega, and are used by his kind permission and also in accordance with the GPS V3 licensing
// of that code.

// This is the main routine for taking action against a parsed message from the GCS
static void handleMessage(void)
{
	mavlink_message_t* handle_msg;

	if (mavlink_message_index == 0)
	{
		handle_msg = &msg[1];
	}
	else
	{
		handle_msg = &msg[0];
	}

//	DPRINT("MAV MSG 0x%x\r\n", handle_msg->msgid);

	handling_of_message_completed |= MAVParamsHandleMessage(handle_msg);
	handling_of_message_completed |= MAVMissionHandleMessage(handle_msg);
	handling_of_message_completed |= MAVFlexiFunctionsHandleMessage(handle_msg);
//	handling_of_message_completed |= MAVFTPHandleMessage(handle_msg); // WIP - RobD

	if (handling_of_message_completed != false)
	{
		return;
	}

	switch (handle_msg->msgid)
	{
		case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:
			MAVLinkRequestDataStream(handle_msg);
			break;
		case MAVLINK_MSG_ID_COMMAND_LONG:
			MAVLinkCommandLong(handle_msg);
			break;
//		case MAVLINK_MSG_ID_COMMAND:
//			DPRINT("MAVLINK_MSG_ID_COMMAND %u\r\n", handle_msg->msgid);
//			break;
//		case MAVLINK_MSG_ID_ACTION:
//			DPRINT("MAVLINK_MSG_ID_ACTION %u\r\n", handle_msg->msgid);
//		case 11:
		case MAVLINK_MSG_ID_SET_MODE:
			MAVLinkSetMode(handle_msg);
			break;
		default:
//			DPRINT("handle_msg->msgid %u NOT HANDLED\r\n", handle_msg->msgid);
			break;
	}
	handling_of_message_completed = true;
}

////////////////////////////////////////////////////////////////////////////////
//
// Callbacks for triggering command complete messaging
//

static inline void preflight_storage_complete_callback(boolean success)
{
	if (mavlink_send_command_ack == false)
	{
		if (success == true)
			mavlink_command_ack_result = MAV_CMD_ACK_OK;
		else
			mavlink_command_ack_result = MAV_CMD_ACK_ERR_FAIL;
		mavlink_command_ack_command = MAV_CMD_PREFLIGHT_STORAGE;
		mavlink_send_command_ack = true;
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// MAIN MAVLINK CODE FOR SENDING COMMANDS TO THE GROUND CONTROL STATION
//

#if (MAVLINK_TEST_ENCODE_DECODE != 1)
const uint8_t mavlink_freq_table[] = {0, 40, 20, 13, 10, 8, 7, 6, 5, 4, 4};

static boolean is_this_the_moment_to_send(uint8_t counter, uint8_t max_counter)
{
	if (counter % max_counter == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Decide whether it is the correct moment to send a given telemetry update,
//   depending on requested frequency
static boolean mavlink_frequency_send(uint8_t frequency, uint8_t counter)
{
	uint8_t max_counter;

	if (frequency > 0 && frequency < 11)
	{
		max_counter = mavlink_freq_table[frequency];
		return is_this_the_moment_to_send(counter, max_counter);
	}
	else if (frequency > 10  && frequency < 14)
	{
		max_counter = 4;
		return is_this_the_moment_to_send(counter, max_counter);
	}
	else if (frequency > 13 && frequency < 17)
	{
		max_counter = 3;
		return is_this_the_moment_to_send(counter, max_counter);
	}
	else if (frequency > 16 && frequency < 24)
	{
		max_counter = 2;
		return is_this_the_moment_to_send(counter, max_counter);
	}
	else if (frequency > 23)
	{
		return true; // send data on every call
	}
	return false;
}
#endif // (MAVLINK_TEST_ENCODE_DECODE != 1)

void mavlink_output_40hz(void)
#if (MAVLINK_TEST_ENCODE_DECODE == 1)
{
	if (mavlink_test_first_pass_flag == 1)
	{
		serial_output("\r\nRunning MAVLink encode / decode Tests.\r\n");
		// reset serial buffer in preparation for testing against buffer
		mavlink_tests_pass = 0;
		mavlink_tests_fail = 0;
		mavlink_test_all(mavlink_system.sysid, mavlink_system.compid, &last_msg);
		serial_output("\r\nMAVLink Tests Pass: %d\r\nMAVLink Tests Fail: %d\r\n", mavlink_tests_pass, mavlink_tests_fail);
		mavlink_test_first_pass_flag = 0;
	}
}
#else
{
	static float previous_earth_pitch = 0.0;
	static float previous_earth_roll = 0.0;
	static float previous_earth_yaw = 0.0;

	struct relative2D matrix_accum;
	float earth_pitch;              // pitch in radians with respect to earth
	float earth_roll;               // roll in radians of the plane with respect to earth frame
	float earth_yaw;                // yaw in radians with respect to earth frame
	float earth_pitch_velocity;     // radians / sec with respect to earth
	float earth_roll_velocity;      // radians / sec with respect to earth
	float earth_yaw_velocity;       // radians / sec with respect to earth
	int16_t accum;                  // general purpose temporary storage
	union longbbbb accum_A_long;    // general purpose temporary storage
	union longbbbb accum_B_long;    // general purpose temporary storage
	uint8_t mavlink_base_mode;      // System mode, see MAV_MODE ENUM in mavlink/include/mavlink_types.h
	uint32_t mavlink_custom_mode;   // Custom Status mode specific to the UDB / MatrixPilot
	int32_t lat, lon, alt, relative_alt = 0;
	uint16_t mavlink_heading = 0;

	enum MAV_CUSTOM_UDB_MODE_FLAG
	{
		MAV_CUSTOM_UDB_MODE_MANUAL = 1,     // Manual Mode. MatrixPilot passes all PWM signals from Receiver straight out to Servos
		MAV_CUSTOM_UDB_MODE_STABILIZE = 2,  // Stabilzed Mode. MatrixPilot assists in flying plane. Pilot still commands plane from transmitter.
		MAV_CUSTOM_UDB_MODE_AUTONOMOUS = 3, // Autonmous Mode. Plane is primarily flying using waypoints or Logo flight language (alghouth pilot can mix in control from transmitter).
		MAV_CUSTOM_UDB_MODE_RTL = 4,        // Return to Launch or Failsafe Mode. This mode means plane has lost contact with pilot's control transmitter.
	};

	uint8_t spread_transmission_load = 0;   // Used to spread sending of different message types over a period of 1 second.

	if (++mavlink_counter_40hz >= 40) mavlink_counter_40hz = 0;

	usec += 25000;  // Frequency sensitive code
	msec += 25;     // Frequency sensitive code

	// Note that message types are arranged in order of importance so that if the serial buffer fills up,
	// critical message types are more likely to still be transmitted.

	// HEARTBEAT
	spread_transmission_load = 1;
	if (mavlink_frequency_send(MAVLINK_RATE_HEARTBEAT, mavlink_counter_40hz + spread_transmission_load))
	{
		if (state_flags._.GPS_steering == 0 && state_flags._.pitch_feedback == 0)
		{
			mavlink_base_mode = MAV_MODE_MANUAL_ARMED | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
			mavlink_custom_mode = MAV_CUSTOM_UDB_MODE_MANUAL;
		}
		else if (state_flags._.GPS_steering == 0 && state_flags._.pitch_feedback == 1)
		{
			mavlink_base_mode = MAV_MODE_GUIDED_ARMED | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
			mavlink_custom_mode = MAV_CUSTOM_UDB_MODE_STABILIZE;
		}
		else if (state_flags._.GPS_steering == 1 && state_flags._.pitch_feedback == 1 && udb_flags._.radio_on == 1)
		{
			mavlink_base_mode = MAV_MODE_AUTO_ARMED | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;
			mavlink_custom_mode = MAV_CUSTOM_UDB_MODE_AUTONOMOUS;
		}
		else if (state_flags._.GPS_steering == 1 && state_flags._.pitch_feedback == 1 && udb_flags._.radio_on == 0)
		{
			mavlink_base_mode = MAV_MODE_AUTO_ARMED | MAV_MODE_FLAG_CUSTOM_MODE_ENABLED; // Return to Landing (lost contact with transmitter)
			mavlink_custom_mode = MAV_CUSTOM_UDB_MODE_RTL;
		}
		else
		{
			mavlink_base_mode = MAV_MODE_TEST_ARMED; // Unknown state
			mavlink_custom_mode = MAV_CUSTOM_UDB_MODE_MANUAL;
		}
		mavlink_msg_heartbeat_send(MAVLINK_COMM_0, MAV_TYPE_FIXED_WING, MAV_AUTOPILOT_UDB, mavlink_base_mode, mavlink_custom_mode, MAV_STATE_ACTIVE);
		//mavlink_msg_heartbeat_send(mavlink_channel_t chan, uint8_t type, uint8_t autopilot, uint8_t base_mode, uint32_t custom_mode, uint8_t system_status)
	}
	// GPS RAW INT - Data from GPS Sensor sent as raw integers.
	spread_transmission_load = 4;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_RAW_SENSORS], mavlink_counter_40hz + spread_transmission_load))
	{
		int16_t gps_fix_type;
		if (gps_nav_valid())
			gps_fix_type = 3;
		else
			gps_fix_type = 0;
		mavlink_msg_gps_raw_int_send(MAVLINK_COMM_0, usec, gps_fix_type, lat_gps.WW, lon_gps.WW, alt_sl_gps.WW, hdop, 65535, sog_gps.BB, cog_gps.BB, svs);
	}

	// GLOBAL POSITION INT - derived from fused sensors
	// Note: This code assumes that Dead Reckoning is running.
	spread_transmission_load = 6;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_POSITION], mavlink_counter_40hz + spread_transmission_load))
	{
		accum_A_long.WW = IMUlocationy._.W1 + (int32_t)(lat_origin.WW / 90.0); // meters North from Equator
		lat = (int32_t) accum_A_long.WW * 90; // degrees North from Equator
		if (cos_lat == 0)
		{
			// We are at the north or south poles, where there is no longitude
			lon = 0;
		}
		else
		{
			accum_A_long.WW = IMUlocationx._.W1;
			accum_A_long.WW = accum_A_long.WW * 16384;      // Compiler uses (shift left 14) for this multiplication
			accum_B_long.WW = (accum_A_long.WW + 8192) / cos_lat;   // 8192 improves rounding accuracy
			lon = lon_origin.WW + (accum_B_long.WW * 90);   // degrees
		}
		accum_A_long.WW = IMUlocationz._.W1;
		relative_alt = accum_A_long.WW * 1000;
		alt = relative_alt + (alt_origin.WW * 10);          // In millimeters; more accurate if used IMUlocationz._.W0

		mavlink_heading = get_geo_heading_angle() * 100;    // mavlink global position expects heading value x 100
		mavlink_msg_global_position_int_send(MAVLINK_COMM_0, msec, lat, lon, alt, relative_alt,
		    IMUvelocityy._.W1, IMUvelocityx._.W1, -IMUvelocityz._.W1, //  IMUVelocity upper word gives V in cm / second
		        // MAVLink is using North,East,Down Frame (NED). MatrixPilot IMUVelocity is in earth frame (X is East, Y is North, Z is Up)
		    mavlink_heading); // heading should be from 0 to 35999 meaning 0 to 359.99 degrees.
		// mavlink_msg_global_position_int_send(mavlink_channel_t chan, uint32_t time_boot_ms, int32_t lat, int32_t lon, int32_t alt,
		//   int32_t relative_alt, int16_t vx, int16_t vy, int16_t vz, uint16_t hdg)
	}

	// ATTITUDE
	//  Roll: Earth Frame of Reference
	spread_transmission_load = 12;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_POSITION], mavlink_counter_40hz + spread_transmission_load))
	{
		matrix_accum.x = rmat[8];
		matrix_accum.y = rmat[6];
		accum = rect_to_polar16(&matrix_accum);     // binary angle (0 to 65536 = 360 degrees)
		earth_roll = (-accum) * BYTE_CIR_16_TO_RAD; // Convert to Radians

		//  Pitch: Earth Frame of Reference
		//  Note that we are using the matrix_accum.x
		//  left over from previous rect_to_polar in this calculation.
		//  so this Pitch calculation must follow the Roll calculation
		matrix_accum.y = rmat[7];
		accum = -rect_to_polar16(&matrix_accum);    // binary angle (0 to 65536 = 360 degrees)
		earth_pitch = (accum) * BYTE_CIR_16_TO_RAD; // Convert to Radians

		// Yaw: Earth Frame of Reference
		matrix_accum.x = rmat[4];
		matrix_accum.y = rmat[1];
		accum = rect_to_polar16(&matrix_accum);     // binary angle (0 to 65536 = 360 degrees)
		earth_yaw = (-accum) * BYTE_CIR_16_TO_RAD;  // Convert to Radians

		// Beginning of frequency sensitive code
		earth_pitch_velocity = (earth_pitch - previous_earth_pitch) * streamRates[MAV_DATA_STREAM_POSITION];
		earth_roll_velocity  = (earth_roll  - previous_earth_roll)  * streamRates[MAV_DATA_STREAM_POSITION];
		earth_yaw_velocity   = (earth_yaw   - previous_earth_yaw)   * streamRates[MAV_DATA_STREAM_POSITION];
		// End of frequency sensitive code

// TODO: investigate why earth_yaw_velocity occasionally spikes with a value of over 50 or below 50..
//		if (earth_yaw_velocity > 40.0 || earth_yaw_velocity < -40.0) {
//			time_t ltime;
//			time(&ltime);
//			DPRINT("earth_yaw_velocity %f earth_yaw %f  previous_earth_yaw %f ", earth_yaw_velocity, earth_yaw, previous_earth_yaw);
//			DPRINT("streamRates %u ", (unsigned int)streamRates[MAV_DATA_STREAM_POSITION]);
//			DPRINT("%s\r\n", ctime(&ltime));
//		}

		previous_earth_pitch = earth_pitch;
		previous_earth_roll  = earth_roll;
		previous_earth_yaw   = earth_yaw;

		mavlink_msg_attitude_send(MAVLINK_COMM_0,msec, earth_roll, earth_pitch, earth_yaw,
		    earth_roll_velocity, earth_pitch_velocity, earth_yaw_velocity);
		//    mavlink_msg_attitude_send(mavlink_channel_t chan, uint32_t time_boot_ms, float roll, float pitch, float yaw,
		//    float rollspeed, float pitchspeed, float yawspeed)
	}

#if (MSG_VFR_HUD_WITH_POSITION == 1)
	// ATTITUDE
	//  Roll: Earth Frame of Reference
	spread_transmission_load = 14;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_POSITION], mavlink_counter_40hz + spread_transmission_load))
	{
		int16_t pwOut_max = 4000;
		mavlink_heading = get_geo_heading_angle();
		if (THROTTLE_CHANNEL_REVERSED == 1) pwOut_max = 2000;
		mavlink_msg_vfr_hud_send(MAVLINK_COMM_0,
		    (float)(air_speed_3DIMU / 100.0),
		    (float)(ground_velocity_magnitudeXY / 100.0),
		    (int16_t)mavlink_heading,
		    (uint16_t)(((float)((udb_pwOut[THROTTLE_OUTPUT_CHANNEL]) - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) * 100.0) / (float)(pwOut_max - udb_pwTrim[THROTTLE_INPUT_CHANNEL])),
		    ((float)(IMUlocationz._.W1 + (alt_origin.WW / 100.0))),
		    (float) IMUvelocityz._.W1 / 100.0);                                 // Current climb rate in meters/second
		//void mavlink_msg_vfr_hud_send(mavlink_channel_t chan, float airspeed, float groundspeed, int16_t heading, uint16_t throttle, float alt, float climb)
	}
#endif // (MSG_VFR_HUD_WITH_POSITION == 1)

	// SYSTEM STATUS
	spread_transmission_load = 18;
	if (mavlink_frequency_send(MAVLINK_RATE_SYSTEM_STATUS, mavlink_counter_40hz + spread_transmission_load))
	{
		mavlink_msg_sys_status_send(MAVLINK_COMM_0,
		    0,              // Sensors fitted
		    0,              // Sensors enabled
		    0,              // Sensor health
		    udb_cpu_load() * 10,
		    #if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
		        battery_voltage._.W1 * 100,     // Battery voltage, in millivolts (1 = 1 millivolt)
		    #else
		        (int16_t)0,
		    #endif
		    #if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)                        
		        battery_current._.W1 * 10,      // Battery current, in 10*milliamperes (1 = 10 milliampere), -1: autopilot does not measure the current
		    #else
		        (int16_t)0,
		    #endif
		    100,                               // Remaining battery energy: (0%: 0, 100%: 100), -1: autopilot estimate the remaining battery
		    r_mavlink_status.packet_rx_drop_count,
		    0,              // errors_comm
		    0,              // errors_count1
		    0,              // errors_count2
		    0,              // errors_count3
		    0);             // errors_count4

		//mavlink_msg_sys_status_send(mavlink_channel_t chan, uint32_t onboard_control_sensors_present, uint32_t onboard_control_sensors_enabled,
		//    uint32_t onboard_control_sensors_health, uint16_t load, uint16_t voltage_battery, int16_t current_battery, int8_t battery_remaining,
		//    uint16_t drop_rate_comm, uint16_t errors_comm, uint16_t errors_count1, uint16_t errors_count2, uint16_t errors_count3, uint16_t errors_count4)

		// Sensor Indices: 0: 3D gyro, 1: 3D acc, 2: 3D mag, 3: absolute pressure, 4: differential pressure, 5: GPS, 6: optical flow, 7: computer vision position, 8: laser based position, 9: external ground-truth (Vicon or Leica). Controllers: 10: 3D angular rate control 11: attitude stabilization, 12: yaw position, 13: z/altitude control, 14: x/y position control, 15: motor outputs / control
	}

	// RC CHANNELS
	// Channel values shifted left by 1, to divide by two, so values reflect PWM pulses in microseconds.
	// mavlink_msg_rc_channels_raw_send(mavlink_channel_t chan, uint16_t chan1_raw, uint16_t chan2_raw,
	//     uint16_t chan3_raw, uint16_t chan4_raw, uint16_t chan5_raw, uint16_t chan6_raw, uint16_t chan7_raw,
	//     uint16_t chan8_raw, uint8_t rssi)
	spread_transmission_load = 24;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_RAW_SENSORS], mavlink_counter_40hz + spread_transmission_load))
	{
		mavlink_msg_rc_channels_raw_send(MAVLINK_COMM_0, msec,
		    (uint16_t)((udb_pwIn[0]) >> 1),
		    (uint16_t)((udb_pwIn[1]) >> 1),
		    (uint16_t)((udb_pwIn[2]) >> 1),
		    (uint16_t)((udb_pwIn[3]) >> 1),
		    (uint16_t)((udb_pwIn[4]) >> 1),
		    (uint16_t)((udb_pwIn[5]) >> 1),
		    (uint16_t)((udb_pwIn[6]) >> 1),
		    (uint16_t)((udb_pwIn[7]) >> 1),
		    (uint8_t)0,     // port number for more than 8 servos
#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
		    (uint8_t)rc_signal_strength);
#else
		    (uint8_t)255);  // 255 denotes not in use
#endif
	}
	// mavlink_msg_rc_channels_raw_send(mavlink_channel_t chan, uint32_t time_boot_ms, uint8_t port, uint16_t chan1_raw, uint16_t chan2_raw, uint16_t chan3_raw, uint16_t chan4_raw,
	//    uint16_t chan5_raw, uint16_t chan6_raw, uint16_t chan7_raw, uint16_t chan8_raw, uint8_t rssi)
	// RAW SENSORS - ACCELOREMETERS and GYROS
	// It is expected that these values are graphed to allow users to check basic sensor operation,
	// and to graph noise on the signals. As this code if for testing and graphing basic hardware, it uses
	// UDB conventions coordinate conventions for X,Y and Z axis rather than MAVLink conventions.
	// See:- http://code.google.com/p/gentlenav/wiki/UDBCoordinateSystems and the "Aviation Convention" diagram.

	spread_transmission_load = 30;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_RAW_SENSORS], mavlink_counter_40hz + spread_transmission_load))
	{
#if (MAG_YAW_DRIFT == 1)    // Magnetometer is connected
		extern int16_t magFieldRaw[];
		mavlink_msg_raw_imu_send(MAVLINK_COMM_0, usec,
		    (int16_t)   udb_xaccel.value, (int16_t)   udb_yaccel.value, (int16_t) - udb_zaccel.value,
		    (int16_t) - udb_xrate.value,  (int16_t) - udb_yrate.value,  (int16_t) - udb_zrate.value,
		    (int16_t)   magFieldRaw[0],   (int16_t)   magFieldRaw[1],   (int16_t)   magFieldRaw[2]);
#else // magnetometer is not connected
		mavlink_msg_raw_imu_send(MAVLINK_COMM_0, usec,
		    (int16_t)   udb_xaccel.value, (int16_t)   udb_yaccel.value, (int16_t) - udb_zaccel.value,
		    (int16_t) - udb_xrate.value,  (int16_t) - udb_yrate.value,  (int16_t) - udb_zrate.value,
		    (int16_t)   0,                (int16_t)   0,                (int16_t)   0); // zero as mag not connected.
#endif //(MAG_YAW_DRIFT == 1)
		// mavlink_msg_raw_imu_send(mavlink_channel_t chan, uint64_t time_usec, int16_t xacc, int16_t yacc, int16_t zacc,
		//     int16_t xgyro, int16_t ygyro, int16_t zgyro, int16_t xmag, int16_t ymag, int16_t zmag)
	}

	// POSITION SENSOR DATA - Using STREAM_EXTRA2
	spread_transmission_load = 36;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_EXTRA2], mavlink_counter_40hz + spread_transmission_load))
	{
		mavlink_msg_altitudes_send(MAVLINK_COMM_0, msec, alt_sl_gps.WW, relative_alt, 0, 0, 0, 0);
		//mavlink_msg_altitudes_send(mavlink_channel_t chan, uint32_t time_boot_ms, int32_t alt_gps, int32_t alt_imu, int32_t alt_barometric, int32_t alt_optical_flow, int32_t alt_range_finder, int32_t alt_extra)
	}

	spread_transmission_load = 40;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_EXTRA2], mavlink_counter_40hz + spread_transmission_load))
	{
		mavlink_msg_airspeeds_send(MAVLINK_COMM_0, msec, 0, 0, 0, 0, 0, 0);
		//mavlink_msg_airspeeds_send(mavlink_channel_t chan, uint32_t time_boot_ms, int16_t airspeed_imu, int16_t airspeed_pitot, int16_t airspeed_hot_wire, int16_t airspeed_ultrasonic, int16_t aoa, int16_t aoy)
	}

	// SEND SERIAL_UDB_EXTRA (SUE) VIA MAVLINK FOR BACKWARDS COMPATIBILITY with FLAN.PYW (FLIGHT ANALYZER)
	// The MAVLink messages for this section of code are unique to MatrixPilot and are defined in matrixpilot.xml
	spread_transmission_load = 10;
	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_EXTRA1], mavlink_counter_40hz + spread_transmission_load)) // SUE code historically ran at 8HZ
	{
		MAVUDBExtraOutput(); // Designed to be called at 8Hz.
	}
	MAVParamsOutput_40hz();
	MAVMissionOutput_40hz();
	MAVFlexiFunctionsOutput_40hz();
//	MAVFTPOutput_40hz(); // WIP - RobD

	// Acknowledge a command if flaged to do so.
	if (mavlink_send_command_ack == true)
	{
		mavlink_msg_command_ack_send(MAVLINK_COMM_0, mavlink_command_ack_command, mavlink_command_ack_result);
		mavlink_send_command_ack = false;
	}
#if (USE_TELELOG == 1)
	log_swapbuf();
#endif
}
#endif // (MAVLINK_TEST_ENCODE_DECODE == 1)

#else

void mavlink_init(void)
{
}
int16_t mavlink_callback_get_byte_to_send(void)
{
	return -1;
}
void mavlink_callback_received_byte(uint8_t rxchar)
{
}

#endif // (USE_MAVLINK == 1)
