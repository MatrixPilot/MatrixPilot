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
#include "telemetry_config.h"

#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)

#include "MAVLinkMP.h"
#include "MAVFlexiFunctions.h"
#if (USE_FLEXIFUNCTION_MIXING == 1)
#include "../libFlexiFunctions/flexiFunctionServices.h"
#endif

extern mavlink_flags_t mavlink_flags;


boolean MAVFlexiFunctionsHandleMessage(mavlink_message_t* handle_msg)
{
	switch (handle_msg->msgid)
	{
#if (USE_FLEXIFUNCTION_MIXING == 1)
		case MAVLINK_MSG_ID_FLEXIFUNCTION_SET:
			// Do nothing with this funciton since it is obsolete
			// Must keep function defined to activate flexifunction mavlink libraries
			break;
		case MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION:
		{
			mavlink_flexifunction_buffer_function_t packet;
			mavlink_msg_flexifunction_buffer_function_decode(handle_msg, &packet);

			if (mavlink_check_target(packet.target_system, packet.target_component)) break;

			// can't respond if busy doing something
			if (flexiFunctionState != FLEXIFUNCTION_WAITING) break;

			flexiFunction_write_buffer_function(&packet.data[0],
				packet.func_index,
				packet.data_address,
				packet.data_size,
				packet.func_count);
			break;
		}
		case MAVLINK_MSG_ID_FLEXIFUNCTION_DIRECTORY:
		{
			mavlink_flexifunction_directory_t packet;
			mavlink_msg_flexifunction_directory_decode(handle_msg, &packet);

			// can't respond if busy doing something
			if (flexiFunctionState != FLEXIFUNCTION_WAITING) break;

			flexiFunction_write_directory(packet.directory_type, packet.start_index, packet.count, packet.directory_data);
			break;
		}
		case MAVLINK_MSG_ID_FLEXIFUNCTION_COMMAND:
		{
			mavlink_flexifunction_command_t packet;
			mavlink_msg_flexifunction_command_decode(handle_msg, &packet);

			// can't respond if busy doing something
			if (flexiFunctionState != FLEXIFUNCTION_WAITING) break;

			switch (packet.command_type)
			{
				case FLEXIFUNCTION_COMMAND_COMMIT_BUFFER:
					flexiFunctionState = FLEXIFUNCTION_COMMIT_BUFFER;
					break;
				case FLEXIFUNCTION_COMMAND_WRITE_NVMEMORY:
					flexiFunctionState = FLEXIFUNCTION_WRITE_NVMEMORY;
					break;
			}
			break;
		}
#endif // (USE_FLEXIFUNCTION_MIXING == 1)
		default:
			return false;
	}
	return true;
}

void MAVFlexiFunctionsOutput_40hz(void)
{
#if (USE_FLEXIFUNCTION_MIXING == 1)
	switch (flexiFunctionState)
	{
		case FLEXIFUNCTION_BUFFER_FUNCTION_ACKNOWLEDGE:
			mavlink_msg_flexifunction_buffer_function_ack_send(MAVLINK_COMM_0, 0, 0, flexifunction_ref_index, flexifunction_ref_result);
			flexiFunctionState = FLEXIFUNCTION_WAITING;
			break;
		case FLEXIFUNCTION_INPUT_DIRECTORY_ACKNOWLEDGE:
			mavlink_msg_flexifunction_directory_ack_send(MAVLINK_COMM_0, 0, 0, 1, 0, 32, flexifunction_ref_result);
			flexiFunctionState = FLEXIFUNCTION_WAITING;
			break;
		case FLEXIFUNCTION_OUTPUT_DIRECTORY_ACKNOWLEDGE:
			mavlink_msg_flexifunction_directory_ack_send(MAVLINK_COMM_0, 0, 0, 0, 0, 32, flexifunction_ref_result);
			flexiFunctionState = FLEXIFUNCTION_WAITING;
			break;
		case FLEXIFUNCTION_COMMAND_ACKNOWLEDGE:
			mavlink_msg_flexifunction_command_ack_send(MAVLINK_COMM_0, flexifunction_ref_command, flexifunction_ref_result);
			flexiFunctionState = FLEXIFUNCTION_WAITING;
			break;

	}
#endif // (USE_FLEXIFUNCTION_MIXING == 1)
}

#endif // (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
