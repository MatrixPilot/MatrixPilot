/** @file
 *	@brief MAVLink comm protocol testsuite generated from matrixpilot.xml
 *	@see http://qgroundcontrol.org/mavlink/
 */
#ifndef MATRIXPILOT_TESTSUITE_H
#define MATRIXPILOT_TESTSUITE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAVLINK_TEST_ALL
#define MAVLINK_TEST_ALL
static void mavlink_test_common(uint8_t, uint8_t, mavlink_message_t *last_msg);
static void mavlink_test_matrixpilot(uint8_t, uint8_t, mavlink_message_t *last_msg);

static void mavlink_test_all(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_test_common(system_id, component_id, last_msg);
	mavlink_test_matrixpilot(system_id, component_id, last_msg);
}
#endif

#include "../common/testsuite.h"


static void mavlink_test_flexifunction_set(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_set_t packet_in = {
		5,
	72,
	};
	mavlink_flexifunction_set_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_set_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_set_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_set_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component );
	mavlink_msg_flexifunction_set_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_set_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component );
	mavlink_msg_flexifunction_set_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_set_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_set_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component );
	mavlink_msg_flexifunction_set_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_read_req(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_read_req_t packet_in = {
		17235,
	17339,
	17,
	84,
	};
	mavlink_flexifunction_read_req_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.read_req_type = packet_in.read_req_type;
        	packet1.data_index = packet_in.data_index;
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_read_req_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_read_req_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_read_req_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.read_req_type , packet1.data_index );
	mavlink_msg_flexifunction_read_req_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_read_req_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.read_req_type , packet1.data_index );
	mavlink_msg_flexifunction_read_req_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_read_req_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_read_req_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.read_req_type , packet1.data_index );
	mavlink_msg_flexifunction_read_req_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_buffer_function(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_buffer_function_t packet_in = {
		17235,
	17339,
	17,
	84,
	151,
	218,
	{ 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76 },
	};
	mavlink_flexifunction_buffer_function_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.func_index = packet_in.func_index;
        	packet1.out_index = packet_in.out_index;
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        	packet1.function_type = packet_in.function_type;
        	packet1.Action = packet_in.Action;
        
        	mav_array_memcpy(packet1.settings_data, packet_in.settings_data, sizeof(int8_t)*48);
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_buffer_function_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.func_index , packet1.function_type , packet1.Action , packet1.out_index , packet1.settings_data );
	mavlink_msg_flexifunction_buffer_function_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.func_index , packet1.function_type , packet1.Action , packet1.out_index , packet1.settings_data );
	mavlink_msg_flexifunction_buffer_function_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_buffer_function_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.func_index , packet1.function_type , packet1.Action , packet1.out_index , packet1.settings_data );
	mavlink_msg_flexifunction_buffer_function_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_buffer_function_ack(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_buffer_function_ack_t packet_in = {
		17235,
	17339,
	17,
	84,
	};
	mavlink_flexifunction_buffer_function_ack_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.func_index = packet_in.func_index;
        	packet1.result = packet_in.result;
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_ack_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_buffer_function_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_ack_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.func_index , packet1.result );
	mavlink_msg_flexifunction_buffer_function_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_ack_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.func_index , packet1.result );
	mavlink_msg_flexifunction_buffer_function_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_buffer_function_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_buffer_function_ack_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.func_index , packet1.result );
	mavlink_msg_flexifunction_buffer_function_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_statistics(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_statistics_t packet_in = {
		17235,
	17339,
	17443,
	17547,
	29,
	96,
	};
	mavlink_flexifunction_statistics_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.register_count = packet_in.register_count;
        	packet1.function_count = packet_in.function_count;
        	packet1.register_max_count = packet_in.register_max_count;
        	packet1.function_max_count = packet_in.function_max_count;
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_statistics_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_statistics_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_statistics_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count , packet1.register_max_count , packet1.function_max_count );
	mavlink_msg_flexifunction_statistics_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_statistics_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count , packet1.register_max_count , packet1.function_max_count );
	mavlink_msg_flexifunction_statistics_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_statistics_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_statistics_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count , packet1.register_max_count , packet1.function_max_count );
	mavlink_msg_flexifunction_statistics_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_sizes(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_sizes_t packet_in = {
		17235,
	17339,
	17,
	84,
	};
	mavlink_flexifunction_sizes_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.register_count = packet_in.register_count;
        	packet1.function_count = packet_in.function_count;
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_sizes_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count );
	mavlink_msg_flexifunction_sizes_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count );
	mavlink_msg_flexifunction_sizes_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_sizes_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count );
	mavlink_msg_flexifunction_sizes_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_sizes_ack(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_sizes_ack_t packet_in = {
		17235,
	17339,
	17443,
	151,
	218,
	};
	mavlink_flexifunction_sizes_ack_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.register_count = packet_in.register_count;
        	packet1.function_count = packet_in.function_count;
        	packet1.result = packet_in.result;
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_ack_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_sizes_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_ack_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count , packet1.result );
	mavlink_msg_flexifunction_sizes_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_ack_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count , packet1.result );
	mavlink_msg_flexifunction_sizes_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_sizes_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_sizes_ack_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.register_count , packet1.function_count , packet1.result );
	mavlink_msg_flexifunction_sizes_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_command(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_command_t packet_in = {
		5,
	72,
	139,
	};
	mavlink_flexifunction_command_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        	packet1.command_type = packet_in.command_type;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_command_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.command_type );
	mavlink_msg_flexifunction_command_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.command_type );
	mavlink_msg_flexifunction_command_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_command_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.command_type );
	mavlink_msg_flexifunction_command_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_command_ack(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_command_ack_t packet_in = {
		17235,
	17339,
	};
	mavlink_flexifunction_command_ack_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.command_type = packet_in.command_type;
        	packet1.result = packet_in.result;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_ack_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_command_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_ack_pack(system_id, component_id, &msg , packet1.command_type , packet1.result );
	mavlink_msg_flexifunction_command_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_ack_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.command_type , packet1.result );
	mavlink_msg_flexifunction_command_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_command_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_command_ack_send(MAVLINK_COMM_1 , packet1.command_type , packet1.result );
	mavlink_msg_flexifunction_command_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_directory(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_directory_t packet_in = {
		5,
	72,
	139,
	206,
	17,
	{ 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131 },
	};
	mavlink_flexifunction_directory_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        	packet1.directory_type = packet_in.directory_type;
        	packet1.start_index = packet_in.start_index;
        	packet1.count = packet_in.count;
        
        	mav_array_memcpy(packet1.directory_data, packet_in.directory_data, sizeof(int8_t)*48);
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_directory_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.directory_type , packet1.start_index , packet1.count , packet1.directory_data );
	mavlink_msg_flexifunction_directory_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.directory_type , packet1.start_index , packet1.count , packet1.directory_data );
	mavlink_msg_flexifunction_directory_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_directory_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.directory_type , packet1.start_index , packet1.count , packet1.directory_data );
	mavlink_msg_flexifunction_directory_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_flexifunction_directory_ack(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
	mavlink_flexifunction_directory_ack_t packet_in = {
		17235,
	139,
	206,
	17,
	84,
	151,
	};
	mavlink_flexifunction_directory_ack_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        	packet1.result = packet_in.result;
        	packet1.target_system = packet_in.target_system;
        	packet1.target_component = packet_in.target_component;
        	packet1.directory_type = packet_in.directory_type;
        	packet1.start_index = packet_in.start_index;
        	packet1.count = packet_in.count;
        
        

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_ack_encode(system_id, component_id, &msg, &packet1);
	mavlink_msg_flexifunction_directory_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_ack_pack(system_id, component_id, &msg , packet1.target_system , packet1.target_component , packet1.directory_type , packet1.start_index , packet1.count , packet1.result );
	mavlink_msg_flexifunction_directory_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_ack_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.target_system , packet1.target_component , packet1.directory_type , packet1.start_index , packet1.count , packet1.result );
	mavlink_msg_flexifunction_directory_ack_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
        	comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
	mavlink_msg_flexifunction_directory_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
	mavlink_msg_flexifunction_directory_ack_send(MAVLINK_COMM_1 , packet1.target_system , packet1.target_component , packet1.directory_type , packet1.start_index , packet1.count , packet1.result );
	mavlink_msg_flexifunction_directory_ack_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
}

static void mavlink_test_matrixpilot(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
	mavlink_test_flexifunction_set(system_id, component_id, last_msg);
	mavlink_test_flexifunction_read_req(system_id, component_id, last_msg);
	mavlink_test_flexifunction_buffer_function(system_id, component_id, last_msg);
	mavlink_test_flexifunction_buffer_function_ack(system_id, component_id, last_msg);
	mavlink_test_flexifunction_statistics(system_id, component_id, last_msg);
	mavlink_test_flexifunction_sizes(system_id, component_id, last_msg);
	mavlink_test_flexifunction_sizes_ack(system_id, component_id, last_msg);
	mavlink_test_flexifunction_command(system_id, component_id, last_msg);
	mavlink_test_flexifunction_command_ack(system_id, component_id, last_msg);
	mavlink_test_flexifunction_directory(system_id, component_id, last_msg);
	mavlink_test_flexifunction_directory_ack(system_id, component_id, last_msg);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MATRIXPILOT_TESTSUITE_H
