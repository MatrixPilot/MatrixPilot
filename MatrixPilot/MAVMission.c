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

#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)

#include "MAVLink.h"
#include "MAVMission.h"

extern mavlink_flags_t mavlink_flags;

extern int8_t calculated_heading;
extern uint16_t number_of_waypoints;
extern int16_t waypointIndex;

uint16_t waypoint_request_i;
uint16_t mav_waypoint_reached;
uint16_t mav_waypoint_changed;
uint16_t mavlink_waypoint_requested_sequence_number;
uint8_t  mavlink_waypoint_dest_sysid;
uint8_t  mavlink_waypoint_dest_compid;
uint16_t mavlink_waypoint_timeout = 0;
uint8_t  number_of_waypoint_retries = 2;
uint8_t  mavlink_waypoint_frame = MAV_FRAME_GLOBAL;
boolean  mavlink_waypoint_current = true;


#define MAX_WAYPOINTS 4

#define MAX_PARAMS 10
static uint16_t params[MAX_PARAMS];
#define PARAM_WP_INDEX 1
#define PARAM_WP_TOTAL 2

void mavlink_waypoint_reached(int16_t waypoint)
{
	mav_waypoint_reached = waypoint;
	mavlink_flags.mavlink_send_waypoint_reached = 1;
}

void mavlink_waypoint_changed(int16_t waypoint)
{
	mav_waypoint_changed = waypoint;
	mavlink_flags.mavlink_send_waypoint_changed = 1;
}

void set(uint16_t index, uint16_t data)
{
	if (index < MAX_PARAMS)
		params[index] = data;
}
uint16_t get(uint16_t index)
{
	uint16_t data = 0;
	if (index < MAX_PARAMS)
		data = params[index];
	return data;
}

#if 0
boolean MAVHandleMissionMessage(mavlink_message_t* handle_msg)
{
	switch (handle_msg->msgid)
	{
#if (FLIGHT_PLAN_TYPE == FP_WAYPOINTS)

		case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
			// BULDING
//			DPRINT("mission request list\r\n");
			{
			// decode
			mavlink_mission_request_list_t packet;

			mavlink_msg_mission_request_list_decode(handle_msg, &packet);
			DPRINT("mission request list: target_system %u, target_component %u\r\n", packet.target_system, packet.target_component);
			if (mavlink_check_target(packet.target_system, packet.target_component)) break;
			mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;
			mavlink_flags.mavlink_sending_waypoints = true;
			mavlink_flags.mavlink_receiving_waypoints = false;
			mavlink_waypoint_dest_sysid = handle_msg->sysid;
			mavlink_waypoint_dest_compid = handle_msg->compid;
			// Start sending waypoints
			mavlink_flags.mavlink_send_waypoint_count = 1;
			DPRINT("mission request list: sysid %u compid %u\r\n", handle_msg->sysid, handle_msg->compid);
			}
			break;

		case MAVLINK_MSG_ID_MISSION_REQUEST:
			//send_text((uint8_t*)"waypoint request\r\n");
			DPRINT("mission request\r\n");

			// Check if in sending waypoint mode ...
			if (!mavlink_flags.mavlink_sending_waypoints)
			{
				DPRINT("mission request not valid, no longer sending\r\n");
				break;
			}
			// decode
			mavlink_mission_request_t packet;
			mavlink_msg_mission_request_decode(handle_msg, &packet);
			if (mavlink_check_target(packet.target_system, packet.target_component)) break;
			mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;
			mavlink_waypoint_requested_sequence_number = packet.seq;
			DPRINT("mission request: packet.seq %u\r\n", packet.seq);
			mavlink_waypoint_frame = MAV_FRAME_GLOBAL; // reference frame
			if (mavlink_waypoint_requested_sequence_number == waypointIndex)
			{
				mavlink_waypoint_current = true;
			}
			else
			{
				mavlink_waypoint_current = false;
			}
			// send waypoint
			mavlink_flags.mavlink_send_specific_waypoint = 1;

//		/************** Not converted to MAVLink wire protocol 1.0 yet *******************
			//uint8_t action = MAV_ACTION_NAVIGATE; // action
			//uint8_t orbit_direction = 0; // clockwise(0), counter-clockwise(1)
			//float orbit = 0; // loiter radius
			//float param1 = 0, param2 = 0;

			//switch(tell_command.id)
			//{

			//case CMD_WAYPOINT: // navigate
				//action = MAV_ACTION_NAVIGATE; // action
				//break;

		// case CMD_LOITER_TIME: // loiter
				//orbit = get(PARAM_WP_RADIUS); // XXX setting loiter radius as waypoint acceptance radius
				//action = MAV_ACTION_LOITER; // action
				//param1 = get(PARAM_WP_RADIUS);
				//param2 = tell_command.p1*100; // loiter time
				//break;

		// case CMD_TAKEOFF: // takeoff
				//action = MAV_ACTION_TAKEOFF;
				//break;

			//case CMD_LAND: // land
				//action = MAV_ACTION_LAND;
				//break;

			//defaut:
				//gcs.send_text("command not handled");
				//break;
			//}

			// time that the mav should loiter in milliseconds
			//uint8_t current = 0; // 1 (true), 0 (false)
			//if (packet.seq == get(PARAM_WP_INDEX)) current = 1;
			//float yaw_dir = 0; // yaw orientation in radians, 0 = north XXX: what does this do?
			//uint8_t autocontinue = 1; // 1 (true), 0 (false)
			//float x = tell_command.lng/1.0e7; // local (x), global (longitude)
			//float y = tell_command.lat/1.0e7; // local (y), global (latitude)
			//float z = tell_command.alt/1.0e2; // local (z), global (altitude)
			// note XXX: documented x,y,z order does not match with gps raw
			//mavlink_msg_waypoint_send(chan,handle_msg->sysid,
				//handle_msg->compid,packet.seq,frame,action,
				//orbit,orbit_direction,param1,param2,current,x,y,z,yaw_dir,autocontinue);

			// update last waypoint comm stamp
			//global_data.waypoint_timelast_send = millis();
			break;

		case MAVLINK_MSG_ID_MISSION_ACK:
			//send_text((uint8_t*)"waypoint ack\r\n");
			DPRINT("mission ack\r\n");
			{
			// decode
			mavlink_mission_ack_t packet;
			mavlink_msg_mission_ack_decode(handle_msg, &packet);
			if (mavlink_check_target(packet.target_system, packet.target_component)) break;

			// parse for error - although we do nothing about an error.
			//uint8_t type = packet.type; // ok (0), error(1)

			// turn off waypoint send
			mavlink_flags.mavlink_sending_waypoints = false;
			mavlink_waypoint_timeout  = 0;
			}
			break;

		case MAVLINK_MSG_ID_MISSION_CLEAR_ALL:
			//send_text((uint8_t*)"waypoint clear all\r\n");
			DPRINT("mission clear all\r\n");
			{
			// decode
			mavlink_mission_clear_all_t packet;
			mavlink_msg_mission_clear_all_decode(handle_msg, &packet);
			if (mavlink_check_target(packet.target_system, packet.target_component)) break;

			// clear all waypoints
			uint8_t type = 0; // ok (0), error(1)
			set(PARAM_WP_TOTAL, 0);

			// send acknowledgement 3 times to makes sure it is received
			int16_t i;
			for (i=0;i<3;i++) mavlink_msg_mission_ack_send(MAVLINK_COMM_0, handle_msg->sysid, handle_msg->compid, type);
			}
			break;

		case MAVLINK_MSG_ID_MISSION_SET_CURRENT:
			//send_text((uint8_t*)"waypoint set current\r\n");
			{
			// decode
			mavlink_mission_set_current_t packet;
			mavlink_msg_mission_set_current_decode(handle_msg, &packet);
			DPRINT("mission set current: %u\r\n", packet.seq);
			if (mavlink_check_target(packet.target_system, packet.target_component)) break;

			// set current waypoint
			set(PARAM_WP_INDEX, packet.seq);
			{
				//Location temp;	// XXX this is gross
				//temp = get_wp_with_index(packet.seq);
				//set_next_WP(&temp);
			}
			mavlink_msg_mission_current_send(MAVLINK_COMM_0, get(PARAM_WP_INDEX));
			}
			break;

		case MAVLINK_MSG_ID_MISSION_COUNT:
			//send_text((uint8_t*)"waypoint count\r\n");
			DPRINT("mission count\r\n");
			{
			// decode
			mavlink_mission_count_t packet;
			mavlink_msg_mission_count_decode(handle_msg, &packet);
			if (mavlink_check_target(packet.target_system, packet.target_component)) break;

			DPRINT("mission count: %u\r\n", packet.count);
			// start waypoint receiving
			set(PARAM_WP_TOTAL, packet.count);
			if (get(PARAM_WP_TOTAL) > MAX_WAYPOINTS)
				set(PARAM_WP_TOTAL, MAX_WAYPOINTS);
			//mavlink_flags.waypoint_timelast_receive = millis();
			mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;

			mavlink_flags.mavlink_receiving_waypoints = true;
			mavlink_flags.mavlink_sending_waypoints = false;
			mavlink_flags.mavlink_request_specific_waypoint = 1;
			waypoint_request_i = 0;
			}
			break;

		case MAVLINK_MSG_ID_MISSION_ITEM:
			//send_text((uint8_t*)"waypoint\r\n");
//			DPRINT("mission item\r\n");
			{
			// Check if receiving waypoint
			if (!mavlink_flags.mavlink_receiving_waypoints) break;

			// decode
			mavlink_mission_item_t packet;
			mavlink_msg_mission_item_decode(handle_msg, &packet);
			if (mavlink_check_target(packet.target_system, packet.target_component)) break;

			DPRINT("mission item: %u\r\n", packet.seq);

			// check if this is the requested waypoint
			if (packet.seq != waypoint_request_i) break;

			// store waypoint
			//uint8_t loadAction = 0; // 0 insert in list, 1 exec now

			switch (packet.frame)
			{
				case MAV_FRAME_GLOBAL:
				{
					DPRINT("FRAME_GLOBAL\r\n");
					//tell_command.lng = 1.0e7*packet.x;
					//tell_command.lat = 1.0e7*packet.y;
					//tell_command.alt = packet.z*1.0e2;
					break;
				}
				case MAV_FRAME_LOCAL_NED: // local (relative to home position)
				{
					DPRINT("FRAME_LOCAL\r\n");
					//tell_command.lng = 1.0e7*ToDeg(packet.x/
							//(radius_of_earth*cos(ToRad(home.lat/1.0e7)))) + home.lng;
					//tell_command.lat = 1.0e7*ToDeg(packet.y/radius_of_earth) + home.lat;
					//tell_command.alt = -packet.z*1.0e2 + home.alt;
					break;
				}
			}

			// defaults
			//tell_command.id = CMD_BLANK;

			switch (packet.command)
			{
				case MAV_CMD_NAV_TAKEOFF:
					DPRINT("NAV_TAKEOFF\r\n");
					//tell_command.id = CMD_TAKEOFF;
					break;
				case MAV_CMD_NAV_LAND:
					DPRINT("NAV_LAND\r\n");
					//tell_command.id = CMD_LAND;
					break;
				case MAV_CMD_NAV_WAYPOINT:
					DPRINT("NAV_WAYPOINT\r\n");
					//tell_command.id = CMD_WAYPOINT;
					break;
				case MAV_CMD_NAV_LOITER_UNLIM:
					DPRINT("NAV_LOITER\r\n");
					//tell_command.id = CMD_LOITER_TIME;
					//tell_command.p1 = packet.param2/1.0e2;
					break;
			}

			// save waypoint
			//set_wp_with_index(tell_command, packet.seq);

			// update waypoint receiving state machine
			//global_data.waypoint_timelast_receive = millis();
			mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;
			waypoint_request_i++;

			if (waypoint_request_i == get(PARAM_WP_TOTAL))
			{
				//gcs.send_text("flight plane received");
				DPRINT("flight plan received\r\n");
				uint8_t type = 0; // ok (0), error(1)
				mavlink_msg_mission_ack_send(MAVLINK_COMM_0, handle_msg->sysid, handle_msg->compid, type);
				mavlink_flags.mavlink_receiving_waypoints = false;
				// XXX ignores waypoint radius for individual waypoints, can
				// only set WP_RADIUS parameter

// MAVLINK_MSG_ID_MISSION_ACK


			}
			else
			{
				mavlink_flags.mavlink_request_specific_waypoint = 1;
			}
			}
			break;
#endif // (FLIGHT_PLAN_TYPE == FP_WAYPOINTS)
		default:
			return false;
	}
	return true;
}

#else

static inline void MissionRequestList(mavlink_message_t* handle_msg)
{
	// BULDING
//	DPRINT("mission request list\r\n");

	// decode
	mavlink_mission_request_list_t packet;

	mavlink_msg_mission_request_list_decode(handle_msg, &packet);
	DPRINT("mission request list: target_system %u, target_component %u\r\n", packet.target_system, packet.target_component);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;
	mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;
	mavlink_flags.mavlink_sending_waypoints = true;
	mavlink_flags.mavlink_receiving_waypoints = false;
	mavlink_waypoint_dest_sysid = handle_msg->sysid;
	mavlink_waypoint_dest_compid = handle_msg->compid;
	// Start sending waypoints
	mavlink_flags.mavlink_send_waypoint_count = 1;
	DPRINT("mission request list: sysid %u compid %u\r\n", handle_msg->sysid, handle_msg->compid);
}

static inline void MissionRequest(mavlink_message_t* handle_msg)
{
	//send_text((uint8_t*)"waypoint request\r\n");
	DPRINT("mission request\r\n");

	// Check if in sending waypoint mode ...
	if (!mavlink_flags.mavlink_sending_waypoints)
	{
		DPRINT("mission request not valid, no longer sending\r\n");
		return;
	}
	// decode
	mavlink_mission_request_t packet;
	mavlink_msg_mission_request_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;
	mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;
	mavlink_waypoint_requested_sequence_number = packet.seq;
	DPRINT("mission request: packet.seq %u\r\n", packet.seq);
	mavlink_waypoint_frame = MAV_FRAME_GLOBAL; // reference frame
	if (mavlink_waypoint_requested_sequence_number == waypointIndex)
	{
		mavlink_waypoint_current = true;
	}
	else
	{
		mavlink_waypoint_current = false;
	}
	// send waypoint
	mavlink_flags.mavlink_send_specific_waypoint = 1;

	/************** Not converted to MAVLink wire protocol 1.0 yet *******************/
	//uint8_t action = MAV_ACTION_NAVIGATE; // action
	//uint8_t orbit_direction = 0; // clockwise(0), counter-clockwise(1)
	//float orbit = 0; // loiter radius
	//float param1 = 0, param2 = 0;

	//switch(tell_command.id)
	//{
		//case CMD_WAYPOINT: // navigate
			//action = MAV_ACTION_NAVIGATE; // action
			//break;

		// case CMD_LOITER_TIME: // loiter
			//orbit = get(PARAM_WP_RADIUS); // XXX setting loiter radius as waypoint acceptance radius
			//action = MAV_ACTION_LOITER; // action
			//param1 = get(PARAM_WP_RADIUS);
			//param2 = tell_command.p1*100; // loiter time
			//break;

		// case CMD_TAKEOFF: // takeoff
			//action = MAV_ACTION_TAKEOFF;
			//break;

		//case CMD_LAND: // land
			//action = MAV_ACTION_LAND;
			//break;

		//defaut:
			//gcs.send_text("command not handled");
			//break;
	//}

	// time that the mav should loiter in milliseconds
	//uint8_t current = 0; // 1 (true), 0 (false)
	//if (packet.seq == get(PARAM_WP_INDEX)) current = 1;
	//float yaw_dir = 0; // yaw orientation in radians, 0 = north XXX: what does this do?
	//uint8_t autocontinue = 1; // 1 (true), 0 (false)
	//float x = tell_command.lng/1.0e7; // local (x), global (longitude)
	//float y = tell_command.lat/1.0e7; // local (y), global (latitude)
	//float z = tell_command.alt/1.0e2; // local (z), global (altitude)
	// note XXX: documented x,y,z order does not match with gps raw
	//mavlink_msg_waypoint_send(chan,handle_msg->sysid,
		//handle_msg->compid,packet.seq,frame,action,
		//orbit,orbit_direction,param1,param2,current,x,y,z,yaw_dir,autocontinue);

	// update last waypoint comm stamp
	//global_data.waypoint_timelast_send = millis();
}

static inline void MissionAck(mavlink_message_t* handle_msg)
{
	//send_text((uint8_t*)"waypoint ack\r\n");
	DPRINT("mission ack\r\n");

	// decode
	mavlink_mission_ack_t packet;
	mavlink_msg_mission_ack_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;

	// parse for error - although we do nothing about an error.
	//uint8_t type = packet.type; // ok (0), error(1)

	// turn off waypoint send
	mavlink_flags.mavlink_sending_waypoints = false;
	mavlink_waypoint_timeout  = 0;
}

static inline void MissionClearAll(mavlink_message_t* handle_msg)
{
	//send_text((uint8_t*)"waypoint clear all\r\n");
	DPRINT("mission clear all\r\n");

	// decode
	mavlink_mission_clear_all_t packet;
	mavlink_msg_mission_clear_all_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;

	// clear all waypoints
	uint8_t type = 0; // ok (0), error(1)
	set(PARAM_WP_TOTAL, 0);

	// send acknowledgement 3 times to makes sure it is received
	int16_t i;
	for (i = 0; i < 3; i++)
	{
		mavlink_msg_mission_ack_send(MAVLINK_COMM_0, handle_msg->sysid, handle_msg->compid, type);
	}
}

static inline void MissionSetCurrent(mavlink_message_t* handle_msg)
{
	//send_text((uint8_t*)"waypoint set current\r\n");
	DPRINT("mission set current\r\n");

	// decode
	mavlink_mission_set_current_t packet;
	mavlink_msg_mission_set_current_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;

	DPRINT("mission set current: %u\r\n", packet.seq);

	// set current waypoint
	set(PARAM_WP_INDEX, packet.seq);
	{
		//Location temp;	// XXX this is gross
		//temp = get_wp_with_index(packet.seq);
		//set_next_WP(&temp);
	}
	mavlink_msg_mission_current_send(MAVLINK_COMM_0, get(PARAM_WP_INDEX));
}

static inline void MissionCount(mavlink_message_t* handle_msg)
{
	//send_text((uint8_t*)"waypoint count\r\n");
	DPRINT("mission count\r\n");
	// decode
	mavlink_mission_count_t packet;
	mavlink_msg_mission_count_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;

	DPRINT("mission count: %u\r\n", packet.count);
	// start waypoint receiving
	set(PARAM_WP_TOTAL, packet.count);
	if (get(PARAM_WP_TOTAL) > MAX_WAYPOINTS)
		set(PARAM_WP_TOTAL, MAX_WAYPOINTS);
	//mavlink_flags.waypoint_timelast_receive = millis();
	mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;

	mavlink_flags.mavlink_receiving_waypoints = true;
	mavlink_flags.mavlink_sending_waypoints = false;
	mavlink_flags.mavlink_request_specific_waypoint = 1;
	waypoint_request_i = 0;
}

static inline void MissionItem(mavlink_message_t* handle_msg)
{
	//send_text((uint8_t*)"waypoint\r\n");
//	DPRINT("mission item\r\n");

	// Check if receiving waypoint
	if (!mavlink_flags.mavlink_receiving_waypoints) return;

	// decode
	mavlink_mission_item_t packet;
	mavlink_msg_mission_item_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;

	DPRINT("mission item: %u\r\n", packet.seq);

	// check if this is the requested waypoint
	if (packet.seq != waypoint_request_i) return;

	// store waypoint
	//uint8_t loadAction = 0; // 0 insert in list, 1 exec now

	switch (packet.frame)
	{
		case MAV_FRAME_GLOBAL:
		{
			DPRINT("FRAME_GLOBAL\r\n");
			//tell_command.lng = 1.0e7*packet.x;
			//tell_command.lat = 1.0e7*packet.y;
			//tell_command.alt = packet.z*1.0e2;
			break;
		}
		case MAV_FRAME_LOCAL_NED: // local (relative to home position)
		{
			DPRINT("FRAME_LOCAL\r\n");
			//tell_command.lng = 1.0e7*ToDeg(packet.x/
					//(radius_of_earth*cos(ToRad(home.lat/1.0e7)))) + home.lng;
			//tell_command.lat = 1.0e7*ToDeg(packet.y/radius_of_earth) + home.lat;
			//tell_command.alt = -packet.z*1.0e2 + home.alt;
			break;
		}
	}

	// defaults
	//tell_command.id = CMD_BLANK;

	switch (packet.command)
	{
		case MAV_CMD_NAV_TAKEOFF:
			DPRINT("NAV_TAKEOFF\r\n");
			//tell_command.id = CMD_TAKEOFF;
			break;
		case MAV_CMD_NAV_LAND:
			DPRINT("NAV_LAND\r\n");
			//tell_command.id = CMD_LAND;
			break;
		case MAV_CMD_NAV_WAYPOINT:
			DPRINT("NAV_WAYPOINT\r\n");
			//tell_command.id = CMD_WAYPOINT;
			break;
		case MAV_CMD_NAV_LOITER_UNLIM:
			DPRINT("NAV_LOITER\r\n");
			//tell_command.id = CMD_LOITER_TIME;
			//tell_command.p1 = packet.param2/1.0e2;
			break;
	}

	// save waypoint
	//set_wp_with_index(tell_command, packet.seq);

	// update waypoint receiving state machine
	//global_data.waypoint_timelast_receive = millis();
	mavlink_waypoint_timeout = MAVLINK_WAYPOINT_TIMEOUT;
	waypoint_request_i++;

	if (waypoint_request_i == get(PARAM_WP_TOTAL))
	{
		//gcs.send_text("flight plane received");
		DPRINT("flight plan received\r\n");
		uint8_t type = 0; // ok (0), error(1)
		mavlink_msg_mission_ack_send(MAVLINK_COMM_0, handle_msg->sysid, handle_msg->compid, type);
		mavlink_flags.mavlink_receiving_waypoints = false;
		// XXX ignores waypoint radius for individual waypoints, can
		// only set WP_RADIUS parameter

		// send MAVLINK_MSG_ID_MISSION_ACK ?
	}
	else
	{
		mavlink_flags.mavlink_request_specific_waypoint = 1;
	}
}

boolean MAVMissionHandleMessage(mavlink_message_t* handle_msg)
{
	switch (handle_msg->msgid)
	{
#if (FLIGHT_PLAN_TYPE == FP_WAYPOINTS)
		case MAVLINK_MSG_ID_MISSION_REQUEST_LIST:
			MissionRequestList(handle_msg);
			break;
		case MAVLINK_MSG_ID_MISSION_REQUEST:
			MissionRequest(handle_msg);
			break;
		case MAVLINK_MSG_ID_MISSION_ACK:
			MissionAck(handle_msg);
			break;
		case MAVLINK_MSG_ID_MISSION_CLEAR_ALL:
			MissionClearAll(handle_msg);
			break;
		case MAVLINK_MSG_ID_MISSION_SET_CURRENT:
			MissionSetCurrent(handle_msg);
			break;
		case MAVLINK_MSG_ID_MISSION_COUNT:
			MissionCount(handle_msg);
			break;
		case MAVLINK_MSG_ID_MISSION_ITEM:
			MissionItem(handle_msg);
			break;
#endif // (FLIGHT_PLAN_TYPE == FP_WAYPOINTS)
		default:
			return false;
	}
	return true;
}

#endif

void MAVMissionOutput_40hz(void)
{
#if (FLIGHT_PLAN_TYPE == FP_WAYPOINTS) // LOGO_WAYPOINTS cannot be uploaded / downloaded

	if (mavlink_flags.mavlink_send_waypoint_reached == 1)
	{
		mavlink_flags.mavlink_send_waypoint_reached = 0;
		mavlink_msg_mission_item_reached_send(MAVLINK_COMM_0, mav_waypoint_reached);
	}

	if (mavlink_flags.mavlink_send_waypoint_changed == 1)
	{
		mavlink_flags.mavlink_send_waypoint_changed = 0;
		mavlink_msg_mission_current_send(MAVLINK_COMM_0, mav_waypoint_changed);
	}

//static inline void mavlink_msg_mission_item_reached_send(mavlink_channel_t chan, uint16_t seq)
//static inline void mavlink_msg_mission_current_send(mavlink_channel_t chan, uint16_t seq)

	// CHECK WHETHER WAYPOINT PROTOCOL HAS TIMED OUT WAITING ON A RESPONSE
	if (mavlink_waypoint_timeout  <= 0)
	{
		if (mavlink_flags.mavlink_sending_waypoints ||  mavlink_flags.mavlink_receiving_waypoints)
		{
			//send_text((uint8_t *)"Timeout on waypoint protocol.\r\n");
			DPRINT("Timeout on waypoint protocol.\r\n");
		}
		mavlink_flags.mavlink_sending_waypoints   = false;
		mavlink_flags.mavlink_receiving_waypoints = false;
	}

//	if (mavlink_flags.mavlink_receiving_waypoints == 1)
	if (mavlink_flags.mavlink_request_specific_waypoint == 1)
	{
		DPRINT("requesting waypoint: %u\r\n", waypoint_request_i);
			//mavlink_flags.waypoint_request_i = 0;
//static inline void mavlink_msg_mission_request_send(mavlink_channel_t chan, uint8_t target_system, uint8_t target_component, uint16_t seq)
		mavlink_msg_mission_request_send(MAVLINK_COMM_0, mavlink_waypoint_dest_sysid, mavlink_waypoint_dest_compid, waypoint_request_i);
		mavlink_flags.mavlink_request_specific_waypoint = 0;
	}

	// SEND NUMBER OF WAYPOINTS IN WAYPOINTS LIST
	if (mavlink_flags.mavlink_send_waypoint_count == 1)
	{
		//send_text((uint8_t *)"Sending waypoint count\r\n");
		DPRINT("Sending waypoint count: %u\r\n", number_of_waypoints);
		mavlink_msg_mission_count_send(MAVLINK_COMM_0, mavlink_waypoint_dest_sysid, mavlink_waypoint_dest_compid, number_of_waypoints);
		mavlink_flags.mavlink_send_waypoint_count = 0;
	}

	// SEND DETAILS OF A SPECIFIC WAYPOINT
	if (mavlink_flags.mavlink_send_specific_waypoint == 1)
	{
			//send_text((uint8_t *)"Time to send a specific waypoint\r\n");
			DPRINT("Time to send a specific waypoint: %u\r\n", mavlink_waypoint_requested_sequence_number);

//			mavlink_msg_mission_item_send(mavlink_channel_t chan, uint8_t target_system, uint8_t target_component, 
//			    uint16_t seq, uint8_t frame, uint16_t command, uint8_t current, uint8_t autocontinue, 
//			    float param1, float param2, float param3, float param4, 
//			    float x, float y, float z)

			//BUILDING

			//extern struct waypointDef *currentWaypointSet = (struct waypointDef*)waypoints;
			//struct waypoint3D    { int32_t x; int32_t y; int16_t z; };
			struct waypoint3D getWaypoint3D(uint16_t wp);
			struct waypoint3D wp;
			wp = getWaypoint3D(mavlink_waypoint_requested_sequence_number);

			//float lat_float, lon_float, alt_float = 0.0;
			//uint32_t accum_long = IMUlocationy._.W1 + (lat_origin.WW / 90); //  meters North from Equator
			//lat_float  = (float)((accum_long * 90) / 10000000.0); // degrees North from Equator
			//lon_float = (float)((float) lon_origin.WW  + ((float)(IMUlocationx._.W1) * 90.0) / (float)(cos_lat / 16384.0)) / 10000000.0;
			//extern struct relWaypointDef wp_to_relative(struct waypointDef wp);
			//struct relWaypointDef current_waypoint = wp_to_relative(waypoints[waypointIndex]);
			//alt_float =  ((float)(IMUlocationz._.W1)) + (float)(alt_origin.WW / 100.0);
			mavlink_msg_mission_item_send(MAVLINK_COMM_0, mavlink_waypoint_dest_sysid, mavlink_waypoint_dest_compid, \
				mavlink_waypoint_requested_sequence_number, mavlink_waypoint_frame, MAV_CMD_NAV_WAYPOINT, mavlink_waypoint_current, true, \
				0.0, 0.0, 0.0, 0.0, \
				(float)wp.y / 10000000.0, (float)wp.x / 10000000.0, wp.z);

			mavlink_flags.mavlink_send_specific_waypoint = 0;
	}
	if (mavlink_waypoint_timeout  > 0) mavlink_waypoint_timeout--;

#endif // (FLIGHT_PLAN_TYPE == FP_WAYPOINTS)
/*
	// Acknowledge a command if flaged to do so.
	if (mavlink_send_command_ack == true)
	{
		mavlink_msg_command_ack_send(MAVLINK_COMM_0, mavlink_command_ack_command, mavlink_command_ack_result);
		mavlink_send_command_ack = false;
	}
 */
}

#endif // (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
