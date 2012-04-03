// MESSAGE FLEXIFUNCTION_BUFFER_FUNCTION PACKING

#define MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION 152

typedef struct __mavlink_flexifunction_buffer_function_t
{
 uint16_t func_index; ///< Function index
 uint16_t out_index; ///< Output register index
 uint8_t target_system; ///< System ID
 uint8_t target_component; ///< Component ID
 uint8_t function_type; ///< Function type index
 uint8_t Action; ///< Output action, 0,1,2 = set, add or clear
 int8_t settings_data[48]; ///< Settings data
} mavlink_flexifunction_buffer_function_t;

#define MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION_LEN 56
#define MAVLINK_MSG_ID_152_LEN 56

#define MAVLINK_MSG_FLEXIFUNCTION_BUFFER_FUNCTION_FIELD_SETTINGS_DATA_LEN 48

#define MAVLINK_MESSAGE_INFO_FLEXIFUNCTION_BUFFER_FUNCTION { \
	"FLEXIFUNCTION_BUFFER_FUNCTION", \
	7, \
	{  { "func_index", NULL, MAVLINK_TYPE_UINT16_T, 0, 0, offsetof(mavlink_flexifunction_buffer_function_t, func_index) }, \
         { "out_index", NULL, MAVLINK_TYPE_UINT16_T, 0, 2, offsetof(mavlink_flexifunction_buffer_function_t, out_index) }, \
         { "target_system", NULL, MAVLINK_TYPE_UINT8_T, 0, 4, offsetof(mavlink_flexifunction_buffer_function_t, target_system) }, \
         { "target_component", NULL, MAVLINK_TYPE_UINT8_T, 0, 5, offsetof(mavlink_flexifunction_buffer_function_t, target_component) }, \
         { "function_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 6, offsetof(mavlink_flexifunction_buffer_function_t, function_type) }, \
         { "Action", NULL, MAVLINK_TYPE_UINT8_T, 0, 7, offsetof(mavlink_flexifunction_buffer_function_t, Action) }, \
         { "settings_data", NULL, MAVLINK_TYPE_INT8_T, 48, 8, offsetof(mavlink_flexifunction_buffer_function_t, settings_data) }, \
         } \
}


/**
 * @brief Pack a flexifunction_buffer_function message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param target_system System ID
 * @param target_component Component ID
 * @param func_index Function index
 * @param function_type Function type index
 * @param Action Output action, 0,1,2 = set, add or clear
 * @param out_index Output register index
 * @param settings_data Settings data
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_flexifunction_buffer_function_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t target_system, uint8_t target_component, uint16_t func_index, uint8_t function_type, uint8_t Action, uint16_t out_index, const int8_t *settings_data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[56];
	_mav_put_uint16_t(buf, 0, func_index);
	_mav_put_uint16_t(buf, 2, out_index);
	_mav_put_uint8_t(buf, 4, target_system);
	_mav_put_uint8_t(buf, 5, target_component);
	_mav_put_uint8_t(buf, 6, function_type);
	_mav_put_uint8_t(buf, 7, Action);
	_mav_put_int8_t_array(buf, 8, settings_data, 48);
        memcpy(_MAV_PAYLOAD(msg), buf, 56);
#else
	mavlink_flexifunction_buffer_function_t packet;
	packet.func_index = func_index;
	packet.out_index = out_index;
	packet.target_system = target_system;
	packet.target_component = target_component;
	packet.function_type = function_type;
	packet.Action = Action;
	mav_array_memcpy(packet.settings_data, settings_data, sizeof(int8_t)*48);
        memcpy(_MAV_PAYLOAD(msg), &packet, 56);
#endif

	msg->msgid = MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION;
	return mavlink_finalize_message(msg, system_id, component_id, 56, 66);
}

/**
 * @brief Pack a flexifunction_buffer_function message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message was sent over
 * @param msg The MAVLink message to compress the data into
 * @param target_system System ID
 * @param target_component Component ID
 * @param func_index Function index
 * @param function_type Function type index
 * @param Action Output action, 0,1,2 = set, add or clear
 * @param out_index Output register index
 * @param settings_data Settings data
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_flexifunction_buffer_function_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t target_system,uint8_t target_component,uint16_t func_index,uint8_t function_type,uint8_t Action,uint16_t out_index,const int8_t *settings_data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[56];
	_mav_put_uint16_t(buf, 0, func_index);
	_mav_put_uint16_t(buf, 2, out_index);
	_mav_put_uint8_t(buf, 4, target_system);
	_mav_put_uint8_t(buf, 5, target_component);
	_mav_put_uint8_t(buf, 6, function_type);
	_mav_put_uint8_t(buf, 7, Action);
	_mav_put_int8_t_array(buf, 8, settings_data, 48);
        memcpy(_MAV_PAYLOAD(msg), buf, 56);
#else
	mavlink_flexifunction_buffer_function_t packet;
	packet.func_index = func_index;
	packet.out_index = out_index;
	packet.target_system = target_system;
	packet.target_component = target_component;
	packet.function_type = function_type;
	packet.Action = Action;
	mav_array_memcpy(packet.settings_data, settings_data, sizeof(int8_t)*48);
        memcpy(_MAV_PAYLOAD(msg), &packet, 56);
#endif

	msg->msgid = MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION;
	return mavlink_finalize_message_chan(msg, system_id, component_id, chan, 56, 66);
}

/**
 * @brief Encode a flexifunction_buffer_function struct into a message
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param flexifunction_buffer_function C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_flexifunction_buffer_function_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_flexifunction_buffer_function_t* flexifunction_buffer_function)
{
	return mavlink_msg_flexifunction_buffer_function_pack(system_id, component_id, msg, flexifunction_buffer_function->target_system, flexifunction_buffer_function->target_component, flexifunction_buffer_function->func_index, flexifunction_buffer_function->function_type, flexifunction_buffer_function->Action, flexifunction_buffer_function->out_index, flexifunction_buffer_function->settings_data);
}

/**
 * @brief Send a flexifunction_buffer_function message
 * @param chan MAVLink channel to send the message
 *
 * @param target_system System ID
 * @param target_component Component ID
 * @param func_index Function index
 * @param function_type Function type index
 * @param Action Output action, 0,1,2 = set, add or clear
 * @param out_index Output register index
 * @param settings_data Settings data
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_flexifunction_buffer_function_send(mavlink_channel_t chan, uint8_t target_system, uint8_t target_component, uint16_t func_index, uint8_t function_type, uint8_t Action, uint16_t out_index, const int8_t *settings_data)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[56];
	_mav_put_uint16_t(buf, 0, func_index);
	_mav_put_uint16_t(buf, 2, out_index);
	_mav_put_uint8_t(buf, 4, target_system);
	_mav_put_uint8_t(buf, 5, target_component);
	_mav_put_uint8_t(buf, 6, function_type);
	_mav_put_uint8_t(buf, 7, Action);
	_mav_put_int8_t_array(buf, 8, settings_data, 48);
	_mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION, buf, 56, 66);
#else
	mavlink_flexifunction_buffer_function_t packet;
	packet.func_index = func_index;
	packet.out_index = out_index;
	packet.target_system = target_system;
	packet.target_component = target_component;
	packet.function_type = function_type;
	packet.Action = Action;
	mav_array_memcpy(packet.settings_data, settings_data, sizeof(int8_t)*48);
	_mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FLEXIFUNCTION_BUFFER_FUNCTION, (const char *)&packet, 56, 66);
#endif
}

#endif

// MESSAGE FLEXIFUNCTION_BUFFER_FUNCTION UNPACKING


/**
 * @brief Get field target_system from flexifunction_buffer_function message
 *
 * @return System ID
 */
static inline uint8_t mavlink_msg_flexifunction_buffer_function_get_target_system(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  4);
}

/**
 * @brief Get field target_component from flexifunction_buffer_function message
 *
 * @return Component ID
 */
static inline uint8_t mavlink_msg_flexifunction_buffer_function_get_target_component(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  5);
}

/**
 * @brief Get field func_index from flexifunction_buffer_function message
 *
 * @return Function index
 */
static inline uint16_t mavlink_msg_flexifunction_buffer_function_get_func_index(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  0);
}

/**
 * @brief Get field function_type from flexifunction_buffer_function message
 *
 * @return Function type index
 */
static inline uint8_t mavlink_msg_flexifunction_buffer_function_get_function_type(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  6);
}

/**
 * @brief Get field Action from flexifunction_buffer_function message
 *
 * @return Output action, 0,1,2 = set, add or clear
 */
static inline uint8_t mavlink_msg_flexifunction_buffer_function_get_Action(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  7);
}

/**
 * @brief Get field out_index from flexifunction_buffer_function message
 *
 * @return Output register index
 */
static inline uint16_t mavlink_msg_flexifunction_buffer_function_get_out_index(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint16_t(msg,  2);
}

/**
 * @brief Get field settings_data from flexifunction_buffer_function message
 *
 * @return Settings data
 */
static inline uint16_t mavlink_msg_flexifunction_buffer_function_get_settings_data(const mavlink_message_t* msg, int8_t *settings_data)
{
	return _MAV_RETURN_int8_t_array(msg, settings_data, 48,  8);
}

/**
 * @brief Decode a flexifunction_buffer_function message into a struct
 *
 * @param msg The message to decode
 * @param flexifunction_buffer_function C-struct to decode the message contents into
 */
static inline void mavlink_msg_flexifunction_buffer_function_decode(const mavlink_message_t* msg, mavlink_flexifunction_buffer_function_t* flexifunction_buffer_function)
{
#if MAVLINK_NEED_BYTE_SWAP
	flexifunction_buffer_function->func_index = mavlink_msg_flexifunction_buffer_function_get_func_index(msg);
	flexifunction_buffer_function->out_index = mavlink_msg_flexifunction_buffer_function_get_out_index(msg);
	flexifunction_buffer_function->target_system = mavlink_msg_flexifunction_buffer_function_get_target_system(msg);
	flexifunction_buffer_function->target_component = mavlink_msg_flexifunction_buffer_function_get_target_component(msg);
	flexifunction_buffer_function->function_type = mavlink_msg_flexifunction_buffer_function_get_function_type(msg);
	flexifunction_buffer_function->Action = mavlink_msg_flexifunction_buffer_function_get_Action(msg);
	mavlink_msg_flexifunction_buffer_function_get_settings_data(msg, flexifunction_buffer_function->settings_data);
#else
	memcpy(flexifunction_buffer_function, _MAV_PAYLOAD(msg), 56);
#endif
}
