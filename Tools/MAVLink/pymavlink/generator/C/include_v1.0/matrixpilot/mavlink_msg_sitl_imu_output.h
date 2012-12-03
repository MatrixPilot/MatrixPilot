// MESSAGE SITL_IMU_OUTPUT PACKING

#define MAVLINK_MSG_ID_SITL_IMU_OUTPUT 183

typedef struct __mavlink_sitl_imu_output_t
{
 uint64_t time_usec; ///< Timestamp (microseconds since UNIX epoch or microseconds since system boot)
 int32_t imu_latitude; ///< IMU Latitude
 int32_t imu_longitude; ///< IMU Longitude
 int32_t imu_altitude; ///< IMU Altitude
 int16_t vx; ///< Ground X Speed (Latitude), expressed as m/s * 100
 int16_t vy; ///< Ground Y Speed (Longitude), expressed as m/s * 100
 int16_t vz; ///< Ground Z Speed (Altitude), expressed as m/s * 100
 int16_t rmat0; ///< Rmat 0
 int16_t rmat1; ///< Rmat 1
 int16_t rmat2; ///< Rmat 2
 int16_t rmat3; ///< Rmat 3
 int16_t rmat4; ///< Rmat 4
 int16_t rmat5; ///< Rmat 5
 int16_t rmat6; ///< Rmat 6
 int16_t rmat7; ///< Rmat 7
 int16_t rmat8; ///< Rmat 8
 int16_t estimated_wind_x; ///< Estimated Wind 0, m/s*100
 int16_t estimated_wind_y; ///< Estimated Wind 1, m/s*100
 int16_t estimated_wind_z; ///< Estimated Wind 2, m/s*100
 int16_t magFieldEarth0; ///< Magnetic Field Earth 0 
 int16_t magFieldEarth1; ///< Magnetic Field Earth 1 
 int16_t magFieldEarth2; ///< Magnetic Field Earth 2 
 int16_t xacc; ///< X acceleration (mg)
 int16_t yacc; ///< Y acceleration (mg)
 int16_t zacc; ///< Z acceleration (mg)
 int16_t xgyro; ///< Angular speed around X axis (millirad /sec)
 int16_t ygyro; ///< Angular speed around Y axis (millirad /sec)
 int16_t zgyro; ///< Angular speed around Z axis (millirad /sec)
} mavlink_sitl_imu_output_t;

#define MAVLINK_MSG_ID_SITL_IMU_OUTPUT_LEN 68
#define MAVLINK_MSG_ID_183_LEN 68



#define MAVLINK_MESSAGE_INFO_SITL_IMU_OUTPUT { \
	"SITL_IMU_OUTPUT", \
	28, \
	{  { "time_usec", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_sitl_imu_output_t, time_usec) }, \
         { "imu_latitude", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_sitl_imu_output_t, imu_latitude) }, \
         { "imu_longitude", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_sitl_imu_output_t, imu_longitude) }, \
         { "imu_altitude", NULL, MAVLINK_TYPE_INT32_T, 0, 16, offsetof(mavlink_sitl_imu_output_t, imu_altitude) }, \
         { "vx", NULL, MAVLINK_TYPE_INT16_T, 0, 20, offsetof(mavlink_sitl_imu_output_t, vx) }, \
         { "vy", NULL, MAVLINK_TYPE_INT16_T, 0, 22, offsetof(mavlink_sitl_imu_output_t, vy) }, \
         { "vz", NULL, MAVLINK_TYPE_INT16_T, 0, 24, offsetof(mavlink_sitl_imu_output_t, vz) }, \
         { "rmat0", NULL, MAVLINK_TYPE_INT16_T, 0, 26, offsetof(mavlink_sitl_imu_output_t, rmat0) }, \
         { "rmat1", NULL, MAVLINK_TYPE_INT16_T, 0, 28, offsetof(mavlink_sitl_imu_output_t, rmat1) }, \
         { "rmat2", NULL, MAVLINK_TYPE_INT16_T, 0, 30, offsetof(mavlink_sitl_imu_output_t, rmat2) }, \
         { "rmat3", NULL, MAVLINK_TYPE_INT16_T, 0, 32, offsetof(mavlink_sitl_imu_output_t, rmat3) }, \
         { "rmat4", NULL, MAVLINK_TYPE_INT16_T, 0, 34, offsetof(mavlink_sitl_imu_output_t, rmat4) }, \
         { "rmat5", NULL, MAVLINK_TYPE_INT16_T, 0, 36, offsetof(mavlink_sitl_imu_output_t, rmat5) }, \
         { "rmat6", NULL, MAVLINK_TYPE_INT16_T, 0, 38, offsetof(mavlink_sitl_imu_output_t, rmat6) }, \
         { "rmat7", NULL, MAVLINK_TYPE_INT16_T, 0, 40, offsetof(mavlink_sitl_imu_output_t, rmat7) }, \
         { "rmat8", NULL, MAVLINK_TYPE_INT16_T, 0, 42, offsetof(mavlink_sitl_imu_output_t, rmat8) }, \
         { "estimated_wind_x", NULL, MAVLINK_TYPE_INT16_T, 0, 44, offsetof(mavlink_sitl_imu_output_t, estimated_wind_x) }, \
         { "estimated_wind_y", NULL, MAVLINK_TYPE_INT16_T, 0, 46, offsetof(mavlink_sitl_imu_output_t, estimated_wind_y) }, \
         { "estimated_wind_z", NULL, MAVLINK_TYPE_INT16_T, 0, 48, offsetof(mavlink_sitl_imu_output_t, estimated_wind_z) }, \
         { "magFieldEarth0", NULL, MAVLINK_TYPE_INT16_T, 0, 50, offsetof(mavlink_sitl_imu_output_t, magFieldEarth0) }, \
         { "magFieldEarth1", NULL, MAVLINK_TYPE_INT16_T, 0, 52, offsetof(mavlink_sitl_imu_output_t, magFieldEarth1) }, \
         { "magFieldEarth2", NULL, MAVLINK_TYPE_INT16_T, 0, 54, offsetof(mavlink_sitl_imu_output_t, magFieldEarth2) }, \
         { "xacc", NULL, MAVLINK_TYPE_INT16_T, 0, 56, offsetof(mavlink_sitl_imu_output_t, xacc) }, \
         { "yacc", NULL, MAVLINK_TYPE_INT16_T, 0, 58, offsetof(mavlink_sitl_imu_output_t, yacc) }, \
         { "zacc", NULL, MAVLINK_TYPE_INT16_T, 0, 60, offsetof(mavlink_sitl_imu_output_t, zacc) }, \
         { "xgyro", NULL, MAVLINK_TYPE_INT16_T, 0, 62, offsetof(mavlink_sitl_imu_output_t, xgyro) }, \
         { "ygyro", NULL, MAVLINK_TYPE_INT16_T, 0, 64, offsetof(mavlink_sitl_imu_output_t, ygyro) }, \
         { "zgyro", NULL, MAVLINK_TYPE_INT16_T, 0, 66, offsetof(mavlink_sitl_imu_output_t, zgyro) }, \
         } \
}


/**
 * @brief Pack a sitl_imu_output message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_usec Timestamp (microseconds since UNIX epoch or microseconds since system boot)
 * @param imu_latitude IMU Latitude
 * @param imu_longitude IMU Longitude
 * @param imu_altitude IMU Altitude
 * @param vx Ground X Speed (Latitude), expressed as m/s * 100
 * @param vy Ground Y Speed (Longitude), expressed as m/s * 100
 * @param vz Ground Z Speed (Altitude), expressed as m/s * 100
 * @param rmat0 Rmat 0
 * @param rmat1 Rmat 1
 * @param rmat2 Rmat 2
 * @param rmat3 Rmat 3
 * @param rmat4 Rmat 4
 * @param rmat5 Rmat 5
 * @param rmat6 Rmat 6
 * @param rmat7 Rmat 7
 * @param rmat8 Rmat 8
 * @param estimated_wind_x Estimated Wind 0, m/s*100
 * @param estimated_wind_y Estimated Wind 1, m/s*100
 * @param estimated_wind_z Estimated Wind 2, m/s*100
 * @param magFieldEarth0 Magnetic Field Earth 0 
 * @param magFieldEarth1 Magnetic Field Earth 1 
 * @param magFieldEarth2 Magnetic Field Earth 2 
 * @param xacc X acceleration (mg)
 * @param yacc Y acceleration (mg)
 * @param zacc Z acceleration (mg)
 * @param xgyro Angular speed around X axis (millirad /sec)
 * @param ygyro Angular speed around Y axis (millirad /sec)
 * @param zgyro Angular speed around Z axis (millirad /sec)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_sitl_imu_output_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint64_t time_usec, int32_t imu_latitude, int32_t imu_longitude, int32_t imu_altitude, int16_t vx, int16_t vy, int16_t vz, int16_t rmat0, int16_t rmat1, int16_t rmat2, int16_t rmat3, int16_t rmat4, int16_t rmat5, int16_t rmat6, int16_t rmat7, int16_t rmat8, int16_t estimated_wind_x, int16_t estimated_wind_y, int16_t estimated_wind_z, int16_t magFieldEarth0, int16_t magFieldEarth1, int16_t magFieldEarth2, int16_t xacc, int16_t yacc, int16_t zacc, int16_t xgyro, int16_t ygyro, int16_t zgyro)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[68];
	_mav_put_uint64_t(buf, 0, time_usec);
	_mav_put_int32_t(buf, 8, imu_latitude);
	_mav_put_int32_t(buf, 12, imu_longitude);
	_mav_put_int32_t(buf, 16, imu_altitude);
	_mav_put_int16_t(buf, 20, vx);
	_mav_put_int16_t(buf, 22, vy);
	_mav_put_int16_t(buf, 24, vz);
	_mav_put_int16_t(buf, 26, rmat0);
	_mav_put_int16_t(buf, 28, rmat1);
	_mav_put_int16_t(buf, 30, rmat2);
	_mav_put_int16_t(buf, 32, rmat3);
	_mav_put_int16_t(buf, 34, rmat4);
	_mav_put_int16_t(buf, 36, rmat5);
	_mav_put_int16_t(buf, 38, rmat6);
	_mav_put_int16_t(buf, 40, rmat7);
	_mav_put_int16_t(buf, 42, rmat8);
	_mav_put_int16_t(buf, 44, estimated_wind_x);
	_mav_put_int16_t(buf, 46, estimated_wind_y);
	_mav_put_int16_t(buf, 48, estimated_wind_z);
	_mav_put_int16_t(buf, 50, magFieldEarth0);
	_mav_put_int16_t(buf, 52, magFieldEarth1);
	_mav_put_int16_t(buf, 54, magFieldEarth2);
	_mav_put_int16_t(buf, 56, xacc);
	_mav_put_int16_t(buf, 58, yacc);
	_mav_put_int16_t(buf, 60, zacc);
	_mav_put_int16_t(buf, 62, xgyro);
	_mav_put_int16_t(buf, 64, ygyro);
	_mav_put_int16_t(buf, 66, zgyro);

        memcpy(_MAV_PAYLOAD(msg), buf, 68);
#else
	mavlink_sitl_imu_output_t packet;
	packet.time_usec = time_usec;
	packet.imu_latitude = imu_latitude;
	packet.imu_longitude = imu_longitude;
	packet.imu_altitude = imu_altitude;
	packet.vx = vx;
	packet.vy = vy;
	packet.vz = vz;
	packet.rmat0 = rmat0;
	packet.rmat1 = rmat1;
	packet.rmat2 = rmat2;
	packet.rmat3 = rmat3;
	packet.rmat4 = rmat4;
	packet.rmat5 = rmat5;
	packet.rmat6 = rmat6;
	packet.rmat7 = rmat7;
	packet.rmat8 = rmat8;
	packet.estimated_wind_x = estimated_wind_x;
	packet.estimated_wind_y = estimated_wind_y;
	packet.estimated_wind_z = estimated_wind_z;
	packet.magFieldEarth0 = magFieldEarth0;
	packet.magFieldEarth1 = magFieldEarth1;
	packet.magFieldEarth2 = magFieldEarth2;
	packet.xacc = xacc;
	packet.yacc = yacc;
	packet.zacc = zacc;
	packet.xgyro = xgyro;
	packet.ygyro = ygyro;
	packet.zgyro = zgyro;

        memcpy(_MAV_PAYLOAD(msg), &packet, 68);
#endif

	msg->msgid = MAVLINK_MSG_ID_SITL_IMU_OUTPUT;
	return mavlink_finalize_message(msg, system_id, component_id, 68, 55);
}

/**
 * @brief Pack a sitl_imu_output message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message was sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_usec Timestamp (microseconds since UNIX epoch or microseconds since system boot)
 * @param imu_latitude IMU Latitude
 * @param imu_longitude IMU Longitude
 * @param imu_altitude IMU Altitude
 * @param vx Ground X Speed (Latitude), expressed as m/s * 100
 * @param vy Ground Y Speed (Longitude), expressed as m/s * 100
 * @param vz Ground Z Speed (Altitude), expressed as m/s * 100
 * @param rmat0 Rmat 0
 * @param rmat1 Rmat 1
 * @param rmat2 Rmat 2
 * @param rmat3 Rmat 3
 * @param rmat4 Rmat 4
 * @param rmat5 Rmat 5
 * @param rmat6 Rmat 6
 * @param rmat7 Rmat 7
 * @param rmat8 Rmat 8
 * @param estimated_wind_x Estimated Wind 0, m/s*100
 * @param estimated_wind_y Estimated Wind 1, m/s*100
 * @param estimated_wind_z Estimated Wind 2, m/s*100
 * @param magFieldEarth0 Magnetic Field Earth 0 
 * @param magFieldEarth1 Magnetic Field Earth 1 
 * @param magFieldEarth2 Magnetic Field Earth 2 
 * @param xacc X acceleration (mg)
 * @param yacc Y acceleration (mg)
 * @param zacc Z acceleration (mg)
 * @param xgyro Angular speed around X axis (millirad /sec)
 * @param ygyro Angular speed around Y axis (millirad /sec)
 * @param zgyro Angular speed around Z axis (millirad /sec)
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_sitl_imu_output_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint64_t time_usec,int32_t imu_latitude,int32_t imu_longitude,int32_t imu_altitude,int16_t vx,int16_t vy,int16_t vz,int16_t rmat0,int16_t rmat1,int16_t rmat2,int16_t rmat3,int16_t rmat4,int16_t rmat5,int16_t rmat6,int16_t rmat7,int16_t rmat8,int16_t estimated_wind_x,int16_t estimated_wind_y,int16_t estimated_wind_z,int16_t magFieldEarth0,int16_t magFieldEarth1,int16_t magFieldEarth2,int16_t xacc,int16_t yacc,int16_t zacc,int16_t xgyro,int16_t ygyro,int16_t zgyro)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[68];
	_mav_put_uint64_t(buf, 0, time_usec);
	_mav_put_int32_t(buf, 8, imu_latitude);
	_mav_put_int32_t(buf, 12, imu_longitude);
	_mav_put_int32_t(buf, 16, imu_altitude);
	_mav_put_int16_t(buf, 20, vx);
	_mav_put_int16_t(buf, 22, vy);
	_mav_put_int16_t(buf, 24, vz);
	_mav_put_int16_t(buf, 26, rmat0);
	_mav_put_int16_t(buf, 28, rmat1);
	_mav_put_int16_t(buf, 30, rmat2);
	_mav_put_int16_t(buf, 32, rmat3);
	_mav_put_int16_t(buf, 34, rmat4);
	_mav_put_int16_t(buf, 36, rmat5);
	_mav_put_int16_t(buf, 38, rmat6);
	_mav_put_int16_t(buf, 40, rmat7);
	_mav_put_int16_t(buf, 42, rmat8);
	_mav_put_int16_t(buf, 44, estimated_wind_x);
	_mav_put_int16_t(buf, 46, estimated_wind_y);
	_mav_put_int16_t(buf, 48, estimated_wind_z);
	_mav_put_int16_t(buf, 50, magFieldEarth0);
	_mav_put_int16_t(buf, 52, magFieldEarth1);
	_mav_put_int16_t(buf, 54, magFieldEarth2);
	_mav_put_int16_t(buf, 56, xacc);
	_mav_put_int16_t(buf, 58, yacc);
	_mav_put_int16_t(buf, 60, zacc);
	_mav_put_int16_t(buf, 62, xgyro);
	_mav_put_int16_t(buf, 64, ygyro);
	_mav_put_int16_t(buf, 66, zgyro);

        memcpy(_MAV_PAYLOAD(msg), buf, 68);
#else
	mavlink_sitl_imu_output_t packet;
	packet.time_usec = time_usec;
	packet.imu_latitude = imu_latitude;
	packet.imu_longitude = imu_longitude;
	packet.imu_altitude = imu_altitude;
	packet.vx = vx;
	packet.vy = vy;
	packet.vz = vz;
	packet.rmat0 = rmat0;
	packet.rmat1 = rmat1;
	packet.rmat2 = rmat2;
	packet.rmat3 = rmat3;
	packet.rmat4 = rmat4;
	packet.rmat5 = rmat5;
	packet.rmat6 = rmat6;
	packet.rmat7 = rmat7;
	packet.rmat8 = rmat8;
	packet.estimated_wind_x = estimated_wind_x;
	packet.estimated_wind_y = estimated_wind_y;
	packet.estimated_wind_z = estimated_wind_z;
	packet.magFieldEarth0 = magFieldEarth0;
	packet.magFieldEarth1 = magFieldEarth1;
	packet.magFieldEarth2 = magFieldEarth2;
	packet.xacc = xacc;
	packet.yacc = yacc;
	packet.zacc = zacc;
	packet.xgyro = xgyro;
	packet.ygyro = ygyro;
	packet.zgyro = zgyro;

        memcpy(_MAV_PAYLOAD(msg), &packet, 68);
#endif

	msg->msgid = MAVLINK_MSG_ID_SITL_IMU_OUTPUT;
	return mavlink_finalize_message_chan(msg, system_id, component_id, chan, 68, 55);
}

/**
 * @brief Encode a sitl_imu_output struct into a message
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param sitl_imu_output C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_sitl_imu_output_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_sitl_imu_output_t* sitl_imu_output)
{
	return mavlink_msg_sitl_imu_output_pack(system_id, component_id, msg, sitl_imu_output->time_usec, sitl_imu_output->imu_latitude, sitl_imu_output->imu_longitude, sitl_imu_output->imu_altitude, sitl_imu_output->vx, sitl_imu_output->vy, sitl_imu_output->vz, sitl_imu_output->rmat0, sitl_imu_output->rmat1, sitl_imu_output->rmat2, sitl_imu_output->rmat3, sitl_imu_output->rmat4, sitl_imu_output->rmat5, sitl_imu_output->rmat6, sitl_imu_output->rmat7, sitl_imu_output->rmat8, sitl_imu_output->estimated_wind_x, sitl_imu_output->estimated_wind_y, sitl_imu_output->estimated_wind_z, sitl_imu_output->magFieldEarth0, sitl_imu_output->magFieldEarth1, sitl_imu_output->magFieldEarth2, sitl_imu_output->xacc, sitl_imu_output->yacc, sitl_imu_output->zacc, sitl_imu_output->xgyro, sitl_imu_output->ygyro, sitl_imu_output->zgyro);
}

/**
 * @brief Send a sitl_imu_output message
 * @param chan MAVLink channel to send the message
 *
 * @param time_usec Timestamp (microseconds since UNIX epoch or microseconds since system boot)
 * @param imu_latitude IMU Latitude
 * @param imu_longitude IMU Longitude
 * @param imu_altitude IMU Altitude
 * @param vx Ground X Speed (Latitude), expressed as m/s * 100
 * @param vy Ground Y Speed (Longitude), expressed as m/s * 100
 * @param vz Ground Z Speed (Altitude), expressed as m/s * 100
 * @param rmat0 Rmat 0
 * @param rmat1 Rmat 1
 * @param rmat2 Rmat 2
 * @param rmat3 Rmat 3
 * @param rmat4 Rmat 4
 * @param rmat5 Rmat 5
 * @param rmat6 Rmat 6
 * @param rmat7 Rmat 7
 * @param rmat8 Rmat 8
 * @param estimated_wind_x Estimated Wind 0, m/s*100
 * @param estimated_wind_y Estimated Wind 1, m/s*100
 * @param estimated_wind_z Estimated Wind 2, m/s*100
 * @param magFieldEarth0 Magnetic Field Earth 0 
 * @param magFieldEarth1 Magnetic Field Earth 1 
 * @param magFieldEarth2 Magnetic Field Earth 2 
 * @param xacc X acceleration (mg)
 * @param yacc Y acceleration (mg)
 * @param zacc Z acceleration (mg)
 * @param xgyro Angular speed around X axis (millirad /sec)
 * @param ygyro Angular speed around Y axis (millirad /sec)
 * @param zgyro Angular speed around Z axis (millirad /sec)
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_sitl_imu_output_send(mavlink_channel_t chan, uint64_t time_usec, int32_t imu_latitude, int32_t imu_longitude, int32_t imu_altitude, int16_t vx, int16_t vy, int16_t vz, int16_t rmat0, int16_t rmat1, int16_t rmat2, int16_t rmat3, int16_t rmat4, int16_t rmat5, int16_t rmat6, int16_t rmat7, int16_t rmat8, int16_t estimated_wind_x, int16_t estimated_wind_y, int16_t estimated_wind_z, int16_t magFieldEarth0, int16_t magFieldEarth1, int16_t magFieldEarth2, int16_t xacc, int16_t yacc, int16_t zacc, int16_t xgyro, int16_t ygyro, int16_t zgyro)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[68];
	_mav_put_uint64_t(buf, 0, time_usec);
	_mav_put_int32_t(buf, 8, imu_latitude);
	_mav_put_int32_t(buf, 12, imu_longitude);
	_mav_put_int32_t(buf, 16, imu_altitude);
	_mav_put_int16_t(buf, 20, vx);
	_mav_put_int16_t(buf, 22, vy);
	_mav_put_int16_t(buf, 24, vz);
	_mav_put_int16_t(buf, 26, rmat0);
	_mav_put_int16_t(buf, 28, rmat1);
	_mav_put_int16_t(buf, 30, rmat2);
	_mav_put_int16_t(buf, 32, rmat3);
	_mav_put_int16_t(buf, 34, rmat4);
	_mav_put_int16_t(buf, 36, rmat5);
	_mav_put_int16_t(buf, 38, rmat6);
	_mav_put_int16_t(buf, 40, rmat7);
	_mav_put_int16_t(buf, 42, rmat8);
	_mav_put_int16_t(buf, 44, estimated_wind_x);
	_mav_put_int16_t(buf, 46, estimated_wind_y);
	_mav_put_int16_t(buf, 48, estimated_wind_z);
	_mav_put_int16_t(buf, 50, magFieldEarth0);
	_mav_put_int16_t(buf, 52, magFieldEarth1);
	_mav_put_int16_t(buf, 54, magFieldEarth2);
	_mav_put_int16_t(buf, 56, xacc);
	_mav_put_int16_t(buf, 58, yacc);
	_mav_put_int16_t(buf, 60, zacc);
	_mav_put_int16_t(buf, 62, xgyro);
	_mav_put_int16_t(buf, 64, ygyro);
	_mav_put_int16_t(buf, 66, zgyro);

	_mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SITL_IMU_OUTPUT, buf, 68, 55);
#else
	mavlink_sitl_imu_output_t packet;
	packet.time_usec = time_usec;
	packet.imu_latitude = imu_latitude;
	packet.imu_longitude = imu_longitude;
	packet.imu_altitude = imu_altitude;
	packet.vx = vx;
	packet.vy = vy;
	packet.vz = vz;
	packet.rmat0 = rmat0;
	packet.rmat1 = rmat1;
	packet.rmat2 = rmat2;
	packet.rmat3 = rmat3;
	packet.rmat4 = rmat4;
	packet.rmat5 = rmat5;
	packet.rmat6 = rmat6;
	packet.rmat7 = rmat7;
	packet.rmat8 = rmat8;
	packet.estimated_wind_x = estimated_wind_x;
	packet.estimated_wind_y = estimated_wind_y;
	packet.estimated_wind_z = estimated_wind_z;
	packet.magFieldEarth0 = magFieldEarth0;
	packet.magFieldEarth1 = magFieldEarth1;
	packet.magFieldEarth2 = magFieldEarth2;
	packet.xacc = xacc;
	packet.yacc = yacc;
	packet.zacc = zacc;
	packet.xgyro = xgyro;
	packet.ygyro = ygyro;
	packet.zgyro = zgyro;

	_mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_SITL_IMU_OUTPUT, (const char *)&packet, 68, 55);
#endif
}

#endif

// MESSAGE SITL_IMU_OUTPUT UNPACKING


/**
 * @brief Get field time_usec from sitl_imu_output message
 *
 * @return Timestamp (microseconds since UNIX epoch or microseconds since system boot)
 */
static inline uint64_t mavlink_msg_sitl_imu_output_get_time_usec(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field imu_latitude from sitl_imu_output message
 *
 * @return IMU Latitude
 */
static inline int32_t mavlink_msg_sitl_imu_output_get_imu_latitude(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int32_t(msg,  8);
}

/**
 * @brief Get field imu_longitude from sitl_imu_output message
 *
 * @return IMU Longitude
 */
static inline int32_t mavlink_msg_sitl_imu_output_get_imu_longitude(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int32_t(msg,  12);
}

/**
 * @brief Get field imu_altitude from sitl_imu_output message
 *
 * @return IMU Altitude
 */
static inline int32_t mavlink_msg_sitl_imu_output_get_imu_altitude(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int32_t(msg,  16);
}

/**
 * @brief Get field vx from sitl_imu_output message
 *
 * @return Ground X Speed (Latitude), expressed as m/s * 100
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_vx(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  20);
}

/**
 * @brief Get field vy from sitl_imu_output message
 *
 * @return Ground Y Speed (Longitude), expressed as m/s * 100
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_vy(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  22);
}

/**
 * @brief Get field vz from sitl_imu_output message
 *
 * @return Ground Z Speed (Altitude), expressed as m/s * 100
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_vz(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  24);
}

/**
 * @brief Get field rmat0 from sitl_imu_output message
 *
 * @return Rmat 0
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat0(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  26);
}

/**
 * @brief Get field rmat1 from sitl_imu_output message
 *
 * @return Rmat 1
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat1(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  28);
}

/**
 * @brief Get field rmat2 from sitl_imu_output message
 *
 * @return Rmat 2
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat2(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  30);
}

/**
 * @brief Get field rmat3 from sitl_imu_output message
 *
 * @return Rmat 3
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat3(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  32);
}

/**
 * @brief Get field rmat4 from sitl_imu_output message
 *
 * @return Rmat 4
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat4(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  34);
}

/**
 * @brief Get field rmat5 from sitl_imu_output message
 *
 * @return Rmat 5
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat5(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  36);
}

/**
 * @brief Get field rmat6 from sitl_imu_output message
 *
 * @return Rmat 6
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat6(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  38);
}

/**
 * @brief Get field rmat7 from sitl_imu_output message
 *
 * @return Rmat 7
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat7(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  40);
}

/**
 * @brief Get field rmat8 from sitl_imu_output message
 *
 * @return Rmat 8
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_rmat8(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  42);
}

/**
 * @brief Get field estimated_wind_x from sitl_imu_output message
 *
 * @return Estimated Wind 0, m/s*100
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_estimated_wind_x(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  44);
}

/**
 * @brief Get field estimated_wind_y from sitl_imu_output message
 *
 * @return Estimated Wind 1, m/s*100
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_estimated_wind_y(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  46);
}

/**
 * @brief Get field estimated_wind_z from sitl_imu_output message
 *
 * @return Estimated Wind 2, m/s*100
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_estimated_wind_z(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  48);
}

/**
 * @brief Get field magFieldEarth0 from sitl_imu_output message
 *
 * @return Magnetic Field Earth 0 
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_magFieldEarth0(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  50);
}

/**
 * @brief Get field magFieldEarth1 from sitl_imu_output message
 *
 * @return Magnetic Field Earth 1 
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_magFieldEarth1(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  52);
}

/**
 * @brief Get field magFieldEarth2 from sitl_imu_output message
 *
 * @return Magnetic Field Earth 2 
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_magFieldEarth2(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  54);
}

/**
 * @brief Get field xacc from sitl_imu_output message
 *
 * @return X acceleration (mg)
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_xacc(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  56);
}

/**
 * @brief Get field yacc from sitl_imu_output message
 *
 * @return Y acceleration (mg)
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_yacc(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  58);
}

/**
 * @brief Get field zacc from sitl_imu_output message
 *
 * @return Z acceleration (mg)
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_zacc(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  60);
}

/**
 * @brief Get field xgyro from sitl_imu_output message
 *
 * @return Angular speed around X axis (millirad /sec)
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_xgyro(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  62);
}

/**
 * @brief Get field ygyro from sitl_imu_output message
 *
 * @return Angular speed around Y axis (millirad /sec)
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_ygyro(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  64);
}

/**
 * @brief Get field zgyro from sitl_imu_output message
 *
 * @return Angular speed around Z axis (millirad /sec)
 */
static inline int16_t mavlink_msg_sitl_imu_output_get_zgyro(const mavlink_message_t* msg)
{
	return _MAV_RETURN_int16_t(msg,  66);
}

/**
 * @brief Decode a sitl_imu_output message into a struct
 *
 * @param msg The message to decode
 * @param sitl_imu_output C-struct to decode the message contents into
 */
static inline void mavlink_msg_sitl_imu_output_decode(const mavlink_message_t* msg, mavlink_sitl_imu_output_t* sitl_imu_output)
{
#if MAVLINK_NEED_BYTE_SWAP
	sitl_imu_output->time_usec = mavlink_msg_sitl_imu_output_get_time_usec(msg);
	sitl_imu_output->imu_latitude = mavlink_msg_sitl_imu_output_get_imu_latitude(msg);
	sitl_imu_output->imu_longitude = mavlink_msg_sitl_imu_output_get_imu_longitude(msg);
	sitl_imu_output->imu_altitude = mavlink_msg_sitl_imu_output_get_imu_altitude(msg);
	sitl_imu_output->vx = mavlink_msg_sitl_imu_output_get_vx(msg);
	sitl_imu_output->vy = mavlink_msg_sitl_imu_output_get_vy(msg);
	sitl_imu_output->vz = mavlink_msg_sitl_imu_output_get_vz(msg);
	sitl_imu_output->rmat0 = mavlink_msg_sitl_imu_output_get_rmat0(msg);
	sitl_imu_output->rmat1 = mavlink_msg_sitl_imu_output_get_rmat1(msg);
	sitl_imu_output->rmat2 = mavlink_msg_sitl_imu_output_get_rmat2(msg);
	sitl_imu_output->rmat3 = mavlink_msg_sitl_imu_output_get_rmat3(msg);
	sitl_imu_output->rmat4 = mavlink_msg_sitl_imu_output_get_rmat4(msg);
	sitl_imu_output->rmat5 = mavlink_msg_sitl_imu_output_get_rmat5(msg);
	sitl_imu_output->rmat6 = mavlink_msg_sitl_imu_output_get_rmat6(msg);
	sitl_imu_output->rmat7 = mavlink_msg_sitl_imu_output_get_rmat7(msg);
	sitl_imu_output->rmat8 = mavlink_msg_sitl_imu_output_get_rmat8(msg);
	sitl_imu_output->estimated_wind_x = mavlink_msg_sitl_imu_output_get_estimated_wind_x(msg);
	sitl_imu_output->estimated_wind_y = mavlink_msg_sitl_imu_output_get_estimated_wind_y(msg);
	sitl_imu_output->estimated_wind_z = mavlink_msg_sitl_imu_output_get_estimated_wind_z(msg);
	sitl_imu_output->magFieldEarth0 = mavlink_msg_sitl_imu_output_get_magFieldEarth0(msg);
	sitl_imu_output->magFieldEarth1 = mavlink_msg_sitl_imu_output_get_magFieldEarth1(msg);
	sitl_imu_output->magFieldEarth2 = mavlink_msg_sitl_imu_output_get_magFieldEarth2(msg);
	sitl_imu_output->xacc = mavlink_msg_sitl_imu_output_get_xacc(msg);
	sitl_imu_output->yacc = mavlink_msg_sitl_imu_output_get_yacc(msg);
	sitl_imu_output->zacc = mavlink_msg_sitl_imu_output_get_zacc(msg);
	sitl_imu_output->xgyro = mavlink_msg_sitl_imu_output_get_xgyro(msg);
	sitl_imu_output->ygyro = mavlink_msg_sitl_imu_output_get_ygyro(msg);
	sitl_imu_output->zgyro = mavlink_msg_sitl_imu_output_get_zgyro(msg);
#else
	memcpy(sitl_imu_output, _MAV_PAYLOAD(msg), 68);
#endif
}
