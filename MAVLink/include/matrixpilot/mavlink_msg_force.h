#pragma once
// MESSAGE FORCE PACKING

#define MAVLINK_MSG_ID_FORCE 183

MAVPACKED(
typedef struct __mavlink_force_t {
 uint32_t time_boot_ms; /*< Timestamp (milliseconds since system boot)*/
 int16_t aero_x; /*< Aeroforce in UDB X Axis, in units of gravity * 2000*/
 int16_t aero_y; /*< Aeroforce in UDB Y Axis,  in units of gravity * 2000 */
 int16_t aero_z; /*< Aeroforce in UDB Z axis (Wing loading), in units of gravity * 2000*/
}) mavlink_force_t;

#define MAVLINK_MSG_ID_FORCE_LEN 10
#define MAVLINK_MSG_ID_FORCE_MIN_LEN 10
#define MAVLINK_MSG_ID_183_LEN 10
#define MAVLINK_MSG_ID_183_MIN_LEN 10

#define MAVLINK_MSG_ID_FORCE_CRC 221
#define MAVLINK_MSG_ID_183_CRC 221



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_FORCE { \
	183, \
	"FORCE", \
	4, \
	{  { "time_boot_ms", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_force_t, time_boot_ms) }, \
         { "aero_x", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_force_t, aero_x) }, \
         { "aero_y", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_force_t, aero_y) }, \
         { "aero_z", NULL, MAVLINK_TYPE_INT16_T, 0, 8, offsetof(mavlink_force_t, aero_z) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_FORCE { \
	"FORCE", \
	4, \
	{  { "time_boot_ms", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_force_t, time_boot_ms) }, \
         { "aero_x", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_force_t, aero_x) }, \
         { "aero_y", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_force_t, aero_y) }, \
         { "aero_z", NULL, MAVLINK_TYPE_INT16_T, 0, 8, offsetof(mavlink_force_t, aero_z) }, \
         } \
}
#endif

/**
 * @brief Pack a force message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_boot_ms Timestamp (milliseconds since system boot)
 * @param aero_x Aeroforce in UDB X Axis, in units of gravity * 2000
 * @param aero_y Aeroforce in UDB Y Axis,  in units of gravity * 2000 
 * @param aero_z Aeroforce in UDB Z axis (Wing loading), in units of gravity * 2000
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_force_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint32_t time_boot_ms, int16_t aero_x, int16_t aero_y, int16_t aero_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_FORCE_LEN];
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, aero_x);
	_mav_put_int16_t(buf, 6, aero_y);
	_mav_put_int16_t(buf, 8, aero_z);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_FORCE_LEN);
#else
	mavlink_force_t packet;
	packet.time_boot_ms = time_boot_ms;
	packet.aero_x = aero_x;
	packet.aero_y = aero_y;
	packet.aero_z = aero_z;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_FORCE_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_FORCE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_FORCE_MIN_LEN, MAVLINK_MSG_ID_FORCE_LEN, MAVLINK_MSG_ID_FORCE_CRC);
}

/**
 * @brief Pack a force message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_boot_ms Timestamp (milliseconds since system boot)
 * @param aero_x Aeroforce in UDB X Axis, in units of gravity * 2000
 * @param aero_y Aeroforce in UDB Y Axis,  in units of gravity * 2000 
 * @param aero_z Aeroforce in UDB Z axis (Wing loading), in units of gravity * 2000
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_force_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint32_t time_boot_ms,int16_t aero_x,int16_t aero_y,int16_t aero_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_FORCE_LEN];
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, aero_x);
	_mav_put_int16_t(buf, 6, aero_y);
	_mav_put_int16_t(buf, 8, aero_z);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_FORCE_LEN);
#else
	mavlink_force_t packet;
	packet.time_boot_ms = time_boot_ms;
	packet.aero_x = aero_x;
	packet.aero_y = aero_y;
	packet.aero_z = aero_z;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_FORCE_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_FORCE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_FORCE_MIN_LEN, MAVLINK_MSG_ID_FORCE_LEN, MAVLINK_MSG_ID_FORCE_CRC);
}

/**
 * @brief Encode a force struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param force C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_force_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_force_t* force)
{
	return mavlink_msg_force_pack(system_id, component_id, msg, force->time_boot_ms, force->aero_x, force->aero_y, force->aero_z);
}

/**
 * @brief Encode a force struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param force C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_force_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_force_t* force)
{
	return mavlink_msg_force_pack_chan(system_id, component_id, chan, msg, force->time_boot_ms, force->aero_x, force->aero_y, force->aero_z);
}

/**
 * @brief Send a force message
 * @param chan MAVLink channel to send the message
 *
 * @param time_boot_ms Timestamp (milliseconds since system boot)
 * @param aero_x Aeroforce in UDB X Axis, in units of gravity * 2000
 * @param aero_y Aeroforce in UDB Y Axis,  in units of gravity * 2000 
 * @param aero_z Aeroforce in UDB Z axis (Wing loading), in units of gravity * 2000
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_force_send(mavlink_channel_t chan, uint32_t time_boot_ms, int16_t aero_x, int16_t aero_y, int16_t aero_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_FORCE_LEN];
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, aero_x);
	_mav_put_int16_t(buf, 6, aero_y);
	_mav_put_int16_t(buf, 8, aero_z);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FORCE, buf, MAVLINK_MSG_ID_FORCE_MIN_LEN, MAVLINK_MSG_ID_FORCE_LEN, MAVLINK_MSG_ID_FORCE_CRC);
#else
	mavlink_force_t packet;
	packet.time_boot_ms = time_boot_ms;
	packet.aero_x = aero_x;
	packet.aero_y = aero_y;
	packet.aero_z = aero_z;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FORCE, (const char *)&packet, MAVLINK_MSG_ID_FORCE_MIN_LEN, MAVLINK_MSG_ID_FORCE_LEN, MAVLINK_MSG_ID_FORCE_CRC);
#endif
}

/**
 * @brief Send a force message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_force_send_struct(mavlink_channel_t chan, const mavlink_force_t* force)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_force_send(chan, force->time_boot_ms, force->aero_x, force->aero_y, force->aero_z);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FORCE, (const char *)force, MAVLINK_MSG_ID_FORCE_MIN_LEN, MAVLINK_MSG_ID_FORCE_LEN, MAVLINK_MSG_ID_FORCE_CRC);
#endif
}

#if MAVLINK_MSG_ID_FORCE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_force_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t time_boot_ms, int16_t aero_x, int16_t aero_y, int16_t aero_z)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint32_t(buf, 0, time_boot_ms);
	_mav_put_int16_t(buf, 4, aero_x);
	_mav_put_int16_t(buf, 6, aero_y);
	_mav_put_int16_t(buf, 8, aero_z);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FORCE, buf, MAVLINK_MSG_ID_FORCE_MIN_LEN, MAVLINK_MSG_ID_FORCE_LEN, MAVLINK_MSG_ID_FORCE_CRC);
#else
	mavlink_force_t *packet = (mavlink_force_t *)msgbuf;
	packet->time_boot_ms = time_boot_ms;
	packet->aero_x = aero_x;
	packet->aero_y = aero_y;
	packet->aero_z = aero_z;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_FORCE, (const char *)packet, MAVLINK_MSG_ID_FORCE_MIN_LEN, MAVLINK_MSG_ID_FORCE_LEN, MAVLINK_MSG_ID_FORCE_CRC);
#endif
}
#endif

#endif

// MESSAGE FORCE UNPACKING


/**
 * @brief Get field time_boot_ms from force message
 *
 * @return Timestamp (milliseconds since system boot)
 */
static inline uint32_t mavlink_msg_force_get_time_boot_ms(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field aero_x from force message
 *
 * @return Aeroforce in UDB X Axis, in units of gravity * 2000
 */
static inline int16_t mavlink_msg_force_get_aero_x(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  4);
}

/**
 * @brief Get field aero_y from force message
 *
 * @return Aeroforce in UDB Y Axis,  in units of gravity * 2000 
 */
static inline int16_t mavlink_msg_force_get_aero_y(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  6);
}

/**
 * @brief Get field aero_z from force message
 *
 * @return Aeroforce in UDB Z axis (Wing loading), in units of gravity * 2000
 */
static inline int16_t mavlink_msg_force_get_aero_z(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  8);
}

/**
 * @brief Decode a force message into a struct
 *
 * @param msg The message to decode
 * @param force C-struct to decode the message contents into
 */
static inline void mavlink_msg_force_decode(const mavlink_message_t* msg, mavlink_force_t* force)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	force->time_boot_ms = mavlink_msg_force_get_time_boot_ms(msg);
	force->aero_x = mavlink_msg_force_get_aero_x(msg);
	force->aero_y = mavlink_msg_force_get_aero_y(msg);
	force->aero_z = mavlink_msg_force_get_aero_z(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_FORCE_LEN? msg->len : MAVLINK_MSG_ID_FORCE_LEN;
        memset(force, 0, MAVLINK_MSG_ID_FORCE_LEN);
	memcpy(force, _MAV_PAYLOAD(msg), len);
#endif
}
