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


#include "../MatrixPilot/defines.h"
#include "options_mavlink.h"

#if (USE_MAVLINK == 1)

#include "MAVLink.h"
#include "MAVParams.h"
//#include "../libDCM/libDCM_internal.h" // Needed for access to internal DCM value
#include "../libDCM/libDCM.h" // Needed for access to internal DCM value
#include "../libDCM/rmat.h"
//#include <string.h>
//#include <stdarg.h>
#include <math.h>

//#if (DECLINATIONANGLE_VARIABLE != 1)
//union intbb dcm_declination_angle = { .BB = 0};
//union intbb dcm_declination_angle = { 0 };
//#endif

#include "../MatrixPilot/parameter_table.h"


/****************************************************************************/
// Variables to support compilation

#if (USE_NV_MEMORY == 1)
#include "data_services.h"
#endif

/****************************************************************************/

int16_t send_variables_counter = 0;
int16_t send_by_index = 0;

extern uint16_t maxstack;
static boolean mavlink_parameter_out_of_bounds(mavlink_param_union_t parm, int16_t i);

// ROUTINES FOR CHANGING UAV ONBOARD PARAMETERS
// All paramaters are sent as type (mavlink_param_union_t) between Ground Control Station and MatrixPilot.
// So paramaters have to be converted between type (mavlink_param_union_t) and their normal representation.
// An explanation of the MAVLink protocol for changing paramaters can be found at:
// http://www.qgroundcontrol.org/parameter_interface

#if (RECORD_FREE_STACK_SPACE ==  1)
static void mavlink_send_param_maxstack(int16_t);
static void mavlink_set_maxstack(float setting, int16_t i);

static void mavlink_send_param_maxstack(int16_t i)
{
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    (4096 - maxstack), MAVLINK_TYPE_FLOAT,  count_of_parameters_list, i);
	//mavlink_msg_param_value_send(mavlink_channel_t chan, const char *param_id, float param_value, uint8_t param_type, uint16_t param_count, uint16_t param_index)
}

static void mavlink_set_maxstack(float setting, int16_t i)
{
	mavlink_param_union_t param;
	param.type = MAVLINK_TYPE_FLOAT;
	param.param_float = setting;

	//send_text((uint8_t*)"Setting maxstack \r\n");
	if ((mavlink_parameters_list[i].readonly == true) ||
		(mavlink_parameter_out_of_bounds(param, i) == true)) return;
	maxstack = (int16_t)(4096 - setting);
}
#endif // RECORD_FREE_STACK_SPACE

static boolean mavlink_parameter_out_of_bounds(mavlink_param_union_t parm, int16_t i)
{
	switch (mavlink_parameter_parsers[mavlink_parameters_list[i].udb_param_type].mavlink_type)
	{
		case MAVLINK_TYPE_FLOAT:
			if (parm.param_float < mavlink_parameters_list[i].min.param_float)
				return true;
			if (parm.param_float > mavlink_parameters_list[i].max.param_float)
				return true;
			break;
		case MAVLINK_TYPE_UINT32_T:
			if (parm.param_int32 < mavlink_parameters_list[i].min.param_int32)
				return true;
			if (parm.param_int32 > mavlink_parameters_list[i].max.param_int32)
				return true;
			break;
		case MAVLINK_TYPE_INT32_T:
			if (parm.param_int32 < mavlink_parameters_list[i].min.param_int32)
				return true;
			if (parm.param_int32 > mavlink_parameters_list[i].max.param_int32)
				return true;
			break;
		default:
			return true;
			break;
	}
	return false;
}

void mavlink_send_param_gyroscale_Q14(int16_t i)
{
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    (float)(*((int16_t*) mavlink_parameters_list[i].pparam) / (SCALEGYRO * 16384.0)),
	    MAVLINK_TYPE_FLOAT, count_of_parameters_list, i); // 16384.0 is RMAX defined as a float.
}

void mavlink_set_param_gyroscale_Q14(mavlink_param_union_t setting, int16_t i)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

	*((int16_t*)mavlink_parameters_list[i].pparam) = (int16_t)(setting.param_float * (SCALEGYRO * 16384.0));
}

void mavlink_send_param_float(int16_t i)
{
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    *((float*)mavlink_parameters_list[i].pparam),
	    MAVLINK_TYPE_FLOAT, count_of_parameters_list, i);
}

void mavlink_set_param_float(mavlink_param_union_t setting, int16_t i)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

	*((float*)mavlink_parameters_list[i].pparam) = setting.param_float;
}

void mavlink_send_param_Q14(int16_t i)
{
#if (QGROUNDCTONROL_PID_COMPATIBILITY == 1) // see mavlink_options.h for details
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    (floor((((float)(*((int16_t*)mavlink_parameters_list[i].pparam) / 16384.0)) * 10000) + 0.5) / 10000.0),
	    MAVLINK_TYPE_FLOAT, count_of_parameters_list, i); // 16384.0 is RMAX defined as a float.
#else
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    (float)(*((int16_t*) mavlink_parameters_list[i].pparam) / 16384.0),
	    MAVLINK_TYPE_FLOAT, count_of_parameters_list, i); // 16384.0 is RMAX defined as a float.
#endif
}

void mavlink_set_param_Q14(mavlink_param_union_t setting, int16_t i)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

	*((int16_t*)mavlink_parameters_list[i].pparam) = (int16_t)(setting.param_float * 16384.0);
}

void mavlink_send_param_pwtrim(int16_t i)
{
	// Check that the size of the udb_pwtrim array is not exceeded
	if (mavlink_parameters_list[i].pparam >= (uint8_t*)(&udb_pwTrim[0] + NUM_INPUTS))
		return;

	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    (float)(*((int16_t*) mavlink_parameters_list[i].pparam) / 2.0),
	    MAVLINK_TYPE_FLOAT, count_of_parameters_list, i); // 16384.0 is RMAX defined as a float.
}

void mavlink_set_param_pwtrim(mavlink_param_union_t setting, int16_t i)
{
	if (setting.type != MAVLINK_TYPE_FLOAT) return;

	// Check that the size of the udb_pwtrim array is not exceeded
	if (mavlink_parameters_list[i].pparam >= (uint8_t*)(&udb_pwTrim[0] + NUM_INPUTS))
		return;
	*((int16_t*)mavlink_parameters_list[i].pparam) = (int16_t)(setting.param_float * 2.0);
}

void mavlink_send_param_int16(int16_t i)
{
	param_union_t param;

	param.param_int32 = *((int16_t*)mavlink_parameters_list[i].pparam);
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    param.param_float, MAVLINK_TYPE_INT32_T, count_of_parameters_list, i); // 16384.0 is RMAX defined as a float.
}

void mavlink_set_param_int16(mavlink_param_union_t setting, int16_t i)
{
	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	*((int16_t*)mavlink_parameters_list[i].pparam) = (int16_t) setting.param_int32;
}

void mavlink_send_param_null(int16_t UNUSED(i))
{
}

void mavlink_set_param_null(float UNUSED(setting), int16_t UNUSED(i))
{
}

void mavlink_send_int_circular(int16_t i)
{
	param_union_t param;
	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int16_t*) mavlink_parameters_list[i].pparam);
	deg_angle.WW = __builtin_mulss(deg_angle._.W0, (int16_t)(RMAX * 180.0 / 256.0));
	deg_angle.WW >>= 5;
    deg_angle.WW += 0x8000 ; // Take care of the rounding error
	param.param_int32 = deg_angle._.W1; // >> 6;
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    param.param_float, MAVLINK_TYPE_INT32_T, count_of_parameters_list, i);
}

void mavlink_set_int_circular(mavlink_param_union_t setting, int16_t i)
{
	union longww dec_angle;

	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	dec_angle.WW = __builtin_mulss((int16_t)setting.param_int32, (int16_t)(RMAX * (256.0 / 180.0)));
	dec_angle.WW <<= 9;
	dec_angle.WW += 0x8000 ; // Take care of the rounding error
	*((int16_t*)mavlink_parameters_list[i].pparam) = dec_angle._.W1;
}

void mavlink_send_dm_airspeed_in_cm(int16_t i)
{
	param_union_t param;
	union longww airspeed;

	airspeed.WW = 0;
	airspeed._.W0 = *((int16_t*)mavlink_parameters_list[i].pparam);
	airspeed.WW = __builtin_mulss(airspeed._.W0, 10.0);
	param.param_int32 = airspeed._.W0;
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    param.param_float, MAVLINK_TYPE_INT32_T, count_of_parameters_list, i);
}

void mavlink_set_dm_airspeed_from_cm(mavlink_param_union_t setting, int16_t i)
{
	union longww airspeed;

	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	airspeed.WW = __builtin_mulss((int16_t)setting.param_int32, (RMAX / 10.0));
	airspeed.WW <<= 2;
	*((int16_t*)mavlink_parameters_list[i].pparam) = airspeed._.W1;
}

void mavlink_send_cm_airspeed_in_m(int16_t i)
{
	param_union_t param;

	param.param_float = (float)*((int16_t*)mavlink_parameters_list[i].pparam);
	param.param_float *= 0.01;
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    param.param_float, MAVLINK_TYPE_FLOAT, count_of_parameters_list, i);
}

void mavlink_set_cm_airspeed_from_m(mavlink_param_union_t setting, int16_t i)
{
	union longww airspeed;

	if (setting.type != MAVLINK_TYPE_FLOAT) return;

	airspeed.WW = __builtin_mulss((int16_t)setting.param_int32, (RMAX / 10.0));
	airspeed.WW <<= 2;
	*((int16_t*)mavlink_parameters_list[i].pparam) = (int16_t)(setting.param_float * 100.0);
}

void mavlink_send_dm_airspeed_in_m(int16_t i)
{
	param_union_t param;

	param.param_float = (float)*((int16_t*)mavlink_parameters_list[i].pparam);
	param.param_float *= 0.1;
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    param.param_float, MAVLINK_TYPE_FLOAT, count_of_parameters_list, i);
}

void mavlink_set_dm_airspeed_from_m(mavlink_param_union_t setting, int16_t i)
{
	union longww airspeed;

	if (setting.type != MAVLINK_TYPE_FLOAT) return;

	airspeed.WW = __builtin_mulss((int16_t)setting.param_int32, (RMAX / 10.0));
	airspeed.WW <<= 2;
	*((int16_t*)mavlink_parameters_list[i].pparam) = (int16_t)(setting.param_float * 10.0);
}

// send angle in dcm units
void mavlink_send_dcm_angle(int16_t i)
{
	param_union_t param;
	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int16_t*)mavlink_parameters_list[i].pparam);
//	deg_angle.WW = __builtin_mulss(deg_angle._.W0, 40);
	deg_angle.WW = __builtin_mulss(deg_angle._.W0, (int16_t)(57.3 * 16.0)); //(RMAX * 180.0 / 256.0));
	deg_angle.WW >>= 2;
	deg_angle.WW += 0x8000 ; // Take care of the rounding error
	param.param_int32 = deg_angle._.W1; // >> 6;
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    param.param_float, MAVLINK_TYPE_INT32_T, count_of_parameters_list, i);
}

// set angle in dcm units
void mavlink_set_dcm_angle(mavlink_param_union_t setting, int16_t i)
{
	union longww dec_angle;

	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	dec_angle.WW = __builtin_mulss((int16_t)setting.param_int32, (RMAX * (16.0 / 57.3))); //(int16_t)(RMAX * 64 / 57.3)
	dec_angle.WW <<= 12;
	dec_angle.WW += 0x8000 ; // Take care of the rounding error
	*((int16_t*)mavlink_parameters_list[i].pparam) = dec_angle._.W1;
}

// send angle rate in units of angle per frame
void mavlink_send_frame_anglerate(int16_t i)
{
	param_union_t param;
	union longww deg_angle;

	deg_angle.WW = 0;
	deg_angle._.W0 = *((int16_t*)mavlink_parameters_list[i].pparam);
//	deg_angle.WW = __builtin_mulss(deg_angle._.W0, 40);
	deg_angle.WW = __builtin_mulss(deg_angle._.W0, (int16_t)(57.3 * 40.0)); //(RMAX * 180.0 / 256.0));
	deg_angle.WW <<= 2;
	deg_angle.WW += 0x8000 ; // Take care of the rounding error
	param.param_int32 = deg_angle._.W1; // >> 6;
	mavlink_msg_param_value_send(MAVLINK_COMM_0, mavlink_parameters_list[i].name,
	    param.param_float, MAVLINK_TYPE_INT32_T, count_of_parameters_list, i);
}

// set angle rate in units of angle per frame
void mavlink_set_frame_anglerate(mavlink_param_union_t setting, int16_t i)
{
	union longww dec_angle;

	if (setting.type != MAVLINK_TYPE_INT32_T) return;

	dec_angle.WW = __builtin_mulss((int16_t)setting.param_int32, (128.0 * 7.15)); //(int16_t)(RMAX * 128 / (57.3 * 40.0))
	dec_angle.WW <<= 9;
	dec_angle.WW += 0x8000 ; // Take care of the rounding error
	*((int16_t*)mavlink_parameters_list[i].pparam) = dec_angle._.W1;
}

// END OF GENERAL ROUTINES FOR CHANGING UAV ONBOARD PARAMETERS

static int16_t get_param_index(const char* key)
{
	int16_t i;

	// iterate known parameters
	for (i = 0; i < count_of_parameters_list; i++)
	{
		// compare key with parameter name
		if (!strcmp(key, (const char*)mavlink_parameters_list[i].name)) // TODO: why are we casting this to const, should not be required - RobD
		{
			return i;
		}
	}
	DPRINT("unknown parameter name: %s\r\n", key);
	return -1;
}

/*
typedef struct param_union {
	union {
		float param_float;
		int32_t param_int32;
		uint32_t param_uint32;
		uint8_t param_uint8;
		uint8_t bytes[4];
	};
	uint8_t type;
} mavlink_param_union_t;
 */
static void MAVParamsSet(const mavlink_message_t* handle_msg)
{
	mavlink_param_set_t packet;
	int16_t i;

	mavlink_msg_param_set_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component) == true)
	{
		DPRINT("\tfailed target system check on parameter set\r\n");
//		send_text((uint8_t*)"failed target system check on parameter set \r\n");
//		break;
	}
	else
	{
		// set parameter
//		const char* key = (const char*)packet.param_id;
		i = get_param_index((const char*)packet.param_id);
		if (i != -1)
		{
			mavlink_param_union_t param;
			param.type = packet.param_type;
			param.param_float = packet.param_value;

			if ((mavlink_parameters_list[i].readonly == false) &&
			    (mavlink_parameter_out_of_bounds(param, i) == false))
			{
				mavlink_parameter_parsers[mavlink_parameters_list[i].udb_param_type].set_param(param, i);
				DPRINT("parameter[%i] %s, %f set\r\n", i, (const char*)packet.param_id, (double)param.param_float);
			}
			else
			{
				DPRINT("parameter[%i] %s, %f out of bounds\r\n", i, (const char*)packet.param_id, (double)param.param_float);
			}
			// Send the parameter back to GCS as acknowledgement of success, or otherwise
			if (mavlink_flags.mavlink_send_specific_variable == 0)
			{
				send_by_index = i;
				mavlink_flags.mavlink_send_specific_variable = 1;
			}
		}
		else
		{
			DPRINT("Attempt to set unknown parameter name: %s\r\n", (const char*)packet.param_id);
		}
/*
		// iterate known parameters
		for (i = 0; i < count_of_parameters_list; i++)
		{
			// compare key with parameter name
			if (!strcmp(key, (const char*)mavlink_parameters_list[i].name)) // TODO: why are we casting this to const, should not be required - RobD
			{
				mavlink_param_union_t param;
				param.type = packet.param_type;
				param.param_float = packet.param_value;

				DPRINT("found parameter[%i] %s, %f\r\n", i, key, param.param_float);

				if ((mavlink_parameters_list[i].readonly == false) &&
				    (mavlink_parameter_out_of_bounds(param, i) == false))
				{
					mavlink_parameter_parsers[mavlink_parameters_list[i].udb_param_type].set_param(param, i);
					DPRINT("parameter set\r\n");
				}
				{
					// After setting parameter, re-send it to GCS as acknowledgement of success.
					if (mavlink_flags.mavlink_send_specific_variable == 0)
					{
						send_by_index = i;
						mavlink_flags.mavlink_send_specific_variable = 1;
					}
				}
			}
		}
 */
	}
}

static void MAVParamsRequestList(const mavlink_message_t* handle_msg)
{
	mavlink_param_request_list_t packet;

	//send_text((uint8_t*)"param request list\r\n");
	DPRINT("param request list\r\n");
	mavlink_msg_param_request_list_decode(handle_msg, &packet);
	if (packet.target_system == mavlink_system.sysid)
	{
		// Start sending parameters
		send_variables_counter = 0;
		mavlink_flags.mavlink_send_variables = 1;
	}
}

static void MAVParamsRequestRead(const mavlink_message_t* handle_msg)
{
	mavlink_param_request_read_t packet;
//	int16_t index;

	//send_text((uint8_t*)"Requested specific parameter\r\n");
	//DPRINT("Requested specific parameter\r\n");
	mavlink_msg_param_request_read_decode(handle_msg, &packet);
	if (packet.target_system == mavlink_system.sysid)
	{
//		const char* key = (const char*)packet.param_id;
		packet.param_index = get_param_index((const char*)packet.param_id);
		if ((packet.param_index >= 0) && (packet.param_index <= count_of_parameters_list))
		{
//			DPRINT("Requested specific parameter %u %u\r\n", packet.param_index, count_of_parameters_list);
			DPRINT("Requested specific parameter %u %s\r\n", packet.param_index, (const char*)packet.param_id);
			send_by_index = packet.param_index;
			mavlink_flags.mavlink_send_specific_variable = 1;
			DPRINT("Sending specific parameter\r\n");
		}
	}
}

boolean MAVParamsHandleMessage(mavlink_message_t* handle_msg)
{
	switch (handle_msg->msgid)
	{
		case MAVLINK_MSG_ID_PARAM_REQUEST_LIST:
		{
			MAVParamsRequestList(handle_msg);
			break;
		}
		case MAVLINK_MSG_ID_PARAM_REQUEST_READ:
		{
			MAVParamsRequestRead(handle_msg);
			break;
		}
		case MAVLINK_MSG_ID_PARAM_SET:
		{
			// decode
			//send_text((uint8_t*)"Param Set\r\n");
			//DPRINT("Param Set\r\n");
			MAVParamsSet(handle_msg);
			break;
		}
		default:
			return false;
	}
	return true;
}

void MAVParamsOutput_40hz(void)
{
	// SEND VALUES OF PARAMETERS IF THE LIST HAS BEEN REQUESTED
	if (mavlink_flags.mavlink_send_variables == 1)
	{
		if (send_variables_counter < count_of_parameters_list)
		{
			mavlink_parameter_parsers[mavlink_parameters_list[send_variables_counter].udb_param_type].send_param(send_variables_counter);
			send_variables_counter++;
		}
		else
		{
			send_variables_counter = 0;
			mavlink_flags.mavlink_send_variables = 0;
		}
	}

	// SEND SPECIFICALLY REQUESTED PARAMETER
	if (mavlink_flags.mavlink_send_specific_variable == 1)
	{
		mavlink_parameter_parsers[mavlink_parameters_list[send_by_index].udb_param_type].send_param(send_by_index);
		mavlink_flags.mavlink_send_specific_variable = 0;
	}
}

#endif // (USE_MAVLINK == 1)
