// pyparam generated file - DO NOT EDIT

#include "defines.h" 

#if(SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) 

#include "parameter_table.h"
#include "data_storage.h"


extern struct ADchannel udb_xaccel ;
extern struct ADchannel udb_yaccel ;
extern struct ADchannel udb_zaccel ;
extern struct ADchannel udb_xrate ;
extern struct ADchannel udb_yrate ;
extern struct ADchannel udb_zrate ;


const mavlink_parameter_parser    mavlink_parameter_parsers[] = {
    { &mavlink_send_param_int16, &mavlink_set_param_int16, MAVLINK_TYPE_INT32_T},
    { &mavlink_send_param_Q14, &mavlink_set_param_Q14, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_param_pwtrim, &mavlink_set_param_pwtrim, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_param_gyroscale_Q14, &mavlink_set_param_gyroscale_Q14, MAVLINK_TYPE_FLOAT},
    };

const mavlink_parameter mavlink_parameters_list[] = {
     {"PID_ROLLKP" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &rollkp, sizeof(rollkp) },
    {"PID_ROLLKD" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*) &rollkd, sizeof(rollkd) },
    {"PID_YAWKPAIL" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &yawkpail, sizeof(yawkpail) },
    {"PID_YAWKDAIL" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*) &yawkdail, sizeof(yawkdail) },
    {"PID_YAWKPRUD" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &yawkprud, sizeof(yawkprud) },
    {"PID_ROLLKPRUD" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &rollkprud, sizeof(rollkprud) },
    {"PID_PITCHGAIN" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &pitchgain, sizeof(pitchgain) },
    {"PID_RUDELEVGAIN" , {.param_float=0.0} , {.param_float=0.4} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &rudderElevMixGain, sizeof(rudderElevMixGain) },


    {"PWTRIM_ROLL" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[AILERON_INPUT_CHANNEL], sizeof(udb_pwTrim[AILERON_INPUT_CHANNEL]) },
    {"PWTRIM_PITCH" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[ELEVATOR_INPUT_CHANNEL], sizeof(udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) },
    {"PWTRIM_THROTTLE" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[3], sizeof(udb_pwTrim[3]) },
    {"PWTRIM_YAW" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[RUDDER_INPUT_CHANNEL], sizeof(udb_pwTrim[RUDDER_INPUT_CHANNEL]) },
    {"PWTRIM_CAMPITCH" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL]) },
    {"PWTRIM_CAM_YAW" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL]) },

    {"IMU_XACCEL_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_xaccel.offset, sizeof(udb_xaccel.offset) },
    {"IMU_YACCEL_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_yaccel.offset, sizeof(udb_yaccel.offset) },
    {"IMU_ZACCEL_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_zaccel.offset, sizeof(udb_zaccel.offset) },
    {"IMU_XGYRO_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_xrate.offset, sizeof(udb_xrate.offset) },
    {"IMU_YGYRO_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_yrate.offset, sizeof(udb_yrate.offset) },
    {"IMU_ZGYRO_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_zrate.offset, sizeof(udb_zrate.offset) },

    };

const int count_of_parameters_list = sizeof(mavlink_parameters_list) / sizeof(mavlink_parameter);


#endif 

