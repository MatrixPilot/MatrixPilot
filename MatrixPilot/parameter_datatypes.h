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
    STORAGE_FLAG_STORE_CALIB = 16,
    } storage_flags_e;

typedef enum
    {
    STORAGE_HANDLE_IMU = 0,
    STORAGE_HANDLE_MAG = 1,
    STORAGE_HANDLE_RTL = 2,
    STORAGE_HANDLE_PWTRIM = 3,
    STORAGE_HANDLE_PID = 4,
    STORAGE_HANDLE_TH = 5,
    STORAGE_HANDLE_ASPD = 6,
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
    } udb_internal_type_e;

extern void mavlink_send_param_int16( int16_t i ) ;
extern void mavlink_set_param_int16(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_param_Q14( int16_t i ) ;
extern void mavlink_set_param_Q14(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_param_pwtrim( int16_t i ) ;
extern void mavlink_set_param_pwtrim(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_param_gyroscale_Q14( int16_t i ) ;
extern void mavlink_set_param_gyroscale_Q14(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_int_circular( int16_t i ) ;
extern void mavlink_set_int_circular(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_dm_airspeed_in_cm( int16_t i ) ;
extern void mavlink_set_dm_airspeed_from_cm(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_dm_airspeed_in_m( int16_t i ) ;
extern void mavlink_set_dm_airspeed_from_m(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_cm_airspeed_in_m( int16_t i ) ;
extern void mavlink_set_cm_airspeed_from_m(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_frame_anglerate( int16_t i ) ;
extern void mavlink_set_frame_anglerate(mavlink_param_union_t setting, int16_t i ) ;

extern void mavlink_send_dcm_angle( int16_t i ) ;
extern void mavlink_set_dcm_angle(mavlink_param_union_t setting, int16_t i ) ;

#endif    // PARAMETER_DATATYPES_H
