// MESSAGE SITL_AP_CONTROL PACKING

#define MAVLINK_MSG_ID_SITL_AP_CONTROL 184

typedef struct __mavlink_sitl_ap_control_t
{
 int16_t ap_control[12]; ///< Control output -1 .. 1
 uint8_t sitl_control_mode; ///< Overide mode, See SITL_OVERIDE_MODE enumeration
} mavlink_sitl_ap_control_t;

#define MAVLINK_MSG_ID_SITL_AP_CONTROL_LEN 25
#define MAVLINK_MSG_ID_184_LEN 25

#define MAVLINK_MSG_SITL_AP_CONTROL_FIELD_AP_CONTROL_LEN 12

#define MAVLINK_MESSAGE_INFO_SITL_AP_CONTROL { \
	"SITL_AP_CONTROL", \
	2, \
	{  { "ap_control", NULL, MAVLINK_TYPE_INT16_T, 12, 0, offsetof(mavlink_sitl_ap_control_t, ap_control) }, \
         { "sitl_control_mode", NULL, MAVLINK_TYPE_UINT8_T, 0, 24, offsetof(mavlink_sitl_ap_control_t, sitl_control_mode) }, \
         } \
}


/**
 * @brief Pack a sitl_ap_control message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param ap_control Control output -1 .. 1
 * @param sitl_control_mode Overide mode, See SITL_OVERIDE_MODE enumeration
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_sitl_ap_control_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       const int16_t *ap_control, uint8_t sitl_control_mode)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[25];
	_mav_put_uint8_t(buf, 24, sitl_control_mode);
	_mav_put_int16_t_array(buf, 0, ap_control, 12);
        memcpy(_MAV_PAYLOAD(msg), buf, 25);
#else
	mavlink_sitl_ap_control_t packet;
	packet.sitl_control_mode = sitl_control_mode;
	mav_array_memcpy(packet.ap_control, ap_control, sizeof(int16_t)*12);
        memcpy(_MAV_PAYLOAD(msg), &packet, 25);
#endif

	msg->msgid = MAVLINK_MSG_ID_SITL_AP_CONTROL;
	return mavlink_finalize_message(msg, system_id, component_id, 25, 90);
}

/**
 * @brief Pack a sitl_ap_control message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message was sent over
 * @param msg The MAVLink message to compress the data into
 * @param ap_control Control output -1 .. 1
 * @param sitl_control_mode Overide mode, See SITL_OVERIDE_MODE enumeration
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_sitl_ap_control_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           const int16_t *ap_control,uint8_t sitl_control_mode)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[25];
	_mav_put_uint8_t(buf, 24, sitl_control_mode);
	_mav_put_int16_t_array(buf, 0, ap_control, 12);
        memcpy(_MAV_PAYLOAD(msg), buf, 25);
#else
	mavlink_sitl_ap_control_t packet;
	packet.sitl_control_mode = sitl_control_mode;
	mav_array_memcpy(packet.ap_control, ap_control, sizeof(int16_t)*12);
        memcpy(_MAV_PAYLOAD(msg), &packet, 25);
#endif

	msg->msgid = MAVLINK_MSG_ID_SITL_AP_CONTROL;
	return mavlink_finalize_message_chan(msg, system_id, component_id, chan, 25, 90);
}

/**
 * @brief Encode a sitl_ap_control struct into a message
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param sitl_ap_control C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_sitl_ap_control_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_sitl_ap_control_t* sitl_ap_control)
{
	return mavlink_msg_sitl_ap_control_pack(system_id, component_id, msg, sitl_ap_control->ap_control, sitl_ap_control->sitl_control_mode);
}

/**
 * @brief Send a sitl_ap_control message
 * @param chan MAVLink channel to send the message
 *
 * @param ap_control Control output -1 .. 1
 * @param sitl_control_mode Overide mode, See SITL_OVERIDE_MODE enumeration
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_sitl_ap_control_send(mavlink_channel_t chan, const int16_t *ap_control, uint8_t sitl_control_mode)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[25];
	_mav_put_uint8_t(buf, 24, sitl_control_mode);
	_mav_put_int16_t_array(buf, 0, ap_control, 12);
	_mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SITL_AP_CONTROL, buf, 25, 90);
#else
	mavlink_sitl_ap_control_t packet;
	packet.sitl_control_mode = sitl_control_mode;
	mav_array_memcpy(packet.ap_control, ap_control, sizeof(int16_t)*12);
	_mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SITL_AP_CONTROL, (const char *)&packet, 25, 90);
#endif
}

#endif

// MESSAGE SITL_AP_CONTROL UNPACKING


/**
 * @brief Get field ap_control from sitl_ap_control message
 *
 * @return Control output -1 .. 1
 */
static inline uint16_t mavlink_msg_sitl_ap_control_get_ap_control(const mavlink_message_t* msg, int16_t *ap_control)
{
	return _MAV_RETURN_int16_t_array(msg, ap_control, 12,  0);
}

/**
 * @brief Get field sitl_control_mode from sitl_ap_control message
 *
 * @return Overide mode, See SITL_OVERIDE_MODE enumeration
 */
static inline uint8_t mavlink_msg_sitl_ap_control_get_sitl_control_mode(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  24);
}

/**
 * @brief Decode a sitl_ap_control message into a struct
 *
 * @param msg The message to decode
 * @param sitl_ap_control C-struct to decode the message contents into
 */
static inline void mavlink_msg_sitl_ap_control_decode(const mavlink_message_t* msg, mavlink_sitl_ap_control_t* sitl_ap_control)
{
#if MAVLINK_NEED_BYTE_SWAP
	mavlink_msg_sitl_ap_control_get_ap_control(msg, sitl_ap_control->ap_control);
	sitl_ap_control->sitl_control_mode = mavlink_msg_sitl_ap_control_get_sitl_control_mode(msg);
#else
	memcpy(sitl_ap_control, _MAV_PAYLOAD(msg), 25);
#endif
}
