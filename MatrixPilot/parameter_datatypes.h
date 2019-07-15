#ifndef PARAMETER_DATATYPES_H
#define PARAMETER_DATATYPES_H
// pyparam generated file - DO NOT EDIT

#include "../MAVLink/include/mavlink_types.h"

typedef enum
	{
	STORAGE_FLAG_ALL = 1,
	STORAGE_FLAG_LOAD_AT_STARTUP = 2,
	STORAGE_FLAG_LOAD_AT_REBOOT = 4,
	STORAGE_FLAG_SAVE_AFTER_CAL = 8,
	STORAGE_FLAG_STORE_WAYPOINTS = 16,
	STORAGE_FLAG_STORE_CALIB = 32,
	STORAGE_FLAG_SAVE_MISSION = 64,
	} storage_flags_e;

typedef enum
	{
	STORAGE_HANDLE_NULL = 0,
	STORAGE_HANDLE_FAILURE_DATALOG = 1,
	STORAGE_HANDLE_IMU_CALIB = 2,
	STORAGE_HANDLE_MAG_CALIB = 3,
	STORAGE_HANDLE_ANALOG_CALIB = 4,
	STORAGE_HANDLE_MIXER = 5,
	STORAGE_HANDLE_RTL_SETTINGS = 6,
	STORAGE_HANDLE_WAYPOINTS = 7,
	STORAGE_HANDLE_OSD_SETUP = 8,
	STORAGE_HANDLE_RADIO_TRIM = 9,
	STORAGE_HANDLE_CONTROL_GAINS = 10,
	STORAGE_HANDLE_THROTTLE_HEIGHT_OPTIONS = 11,
	STORAGE_HANDLE_AIRSPEED_OPTIONS = 12,
	STORAGE_HANDLE_TURNS_OPTIONS = 13,
	} data_storage_handles_e;

typedef enum
	{
	UDB_TYPE_INT,
	UDB_TYPE_Q14,
	UDB_TYPE_PWTRIM,
	UDB_TYPE_GYROSCALE_Q14,
	UDB_TYPE_INT_CIRCULAR,
	UDB_TYPE_CM_AIRSPEED_TO_DM,
	UDB_TYPE_M_AIRSPEED_TO_DM,
	UDB_TYPE_M_AIRSPEED_TO_CM,
	UDB_TYPE_FRAME_ANGLERATE,
	UDB_TYPE_DCM_ANGLE,
	UDB_TYPE_FLOAT,
// modif gfm declare UDB_TYPE_GPS_Position
	UDB_TYPE_GPS_POSITION,
// fin modif gfm
	} udb_internal_type_e;

extern void mavlink_send_param_int16(int16_t i);
extern void mavlink_set_param_int16(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_param_Q14(int16_t i);
extern void mavlink_set_param_Q14(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_param_pwtrim(int16_t i);
extern void mavlink_set_param_pwtrim(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_param_gyroscale_Q14(int16_t i);
extern void mavlink_set_param_gyroscale_Q14(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_int_circular(int16_t i);
extern void mavlink_set_int_circular(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_dm_airspeed_in_cm(int16_t i);
extern void mavlink_set_dm_airspeed_from_cm(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_dm_airspeed_in_m(int16_t i);
extern void mavlink_set_dm_airspeed_from_m(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_cm_airspeed_in_m(int16_t i);
extern void mavlink_set_cm_airspeed_from_m(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_frame_anglerate(int16_t i);
extern void mavlink_set_frame_anglerate(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_dcm_angle(int16_t i);
extern void mavlink_set_dcm_angle(mavlink_param_union_t setting, int16_t i);

extern void mavlink_send_param_float(int16_t i);
extern void mavlink_set_param_float(mavlink_param_union_t setting, int16_t i);

// modif gfm declare Mavlink send & get GPS Position
//extern void mavlink_send_gps_position(int16_t i);
//extern void mavlink_set_gps_position(mavlink_param_union_t setting, int16_t i);
// fin modif gfm
#endif // PARAMETER_DATATYPES_H
