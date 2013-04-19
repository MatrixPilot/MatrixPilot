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
extern int16_t height_target_min ;
extern int16_t height_target_max ;
extern int16_t height_margin ;
extern fractional alt_hold_throttle_min ;
extern fractional alt_hold_throttle_max ;
extern int16_t alt_hold_pitch_min ;
extern int16_t alt_hold_pitch_max ;
extern int16_t minimum_groundspeed ;
extern int16_t maximum_airspeed ;
extern int16_t minimum_airspeed ;
extern int16_t minimum_groundspeed ;
extern int16_t maximum_airspeed ;
extern int16_t minimum_airspeed ;
extern int16_t cruise_airspeed ;
extern int16_t desiredSpeed ;
extern int16_t fbw_rollPositionMax ;
extern int32_t nav_rollPositionMax ;
extern int32_t nav_roll_rate ;
extern uint16_t loiter_radius ;
extern fractional auto_nav_roll_gain ;
extern uint32_t rate_error_load_gain ;
extern uint32_t pitch_error_rate_gain ;
extern uint32_t yaw_damping ;
extern uint32_t yaw_ff_correction_rate ;
extern uint32_t yaw_ff_correct_thresh ;
extern uint32_t yaw_ffgain_Q16 ;
extern uint32_t roll_damping ;
extern uint32_t roll_error_rate_gain ;
extern uint32_t aoa_offset_correction ;
extern uint32_t aoa_offset_correction_rate ;
extern uint32_t aoa_offset_correction_max_load ;
extern uint32_t aoa_offset_correction_min_load ;
extern uint32_t aoa_offset_correction_max_Cl ;
extern uint32_t aoa_offset_correction_min_Cl ;


const mavlink_parameter_parser    mavlink_parameter_parsers[] = {
    { &mavlink_send_param_int16, &mavlink_set_param_int16, MAVLINK_TYPE_INT32_T},
    { &mavlink_send_param_Q14, &mavlink_set_param_Q14, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_param_Q16, &mavlink_set_param_Q16, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_param_Q16_angle, &mavlink_set_param_Q16_angle, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_param_pwtrim, &mavlink_set_param_pwtrim, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_param_gyroscale_Q14, &mavlink_set_param_gyroscale_Q14, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_int_circular, &mavlink_set_int_circular, MAVLINK_TYPE_INT32_T},
    { &mavlink_send_dm_airspeed_in_cm, &mavlink_set_dm_airspeed_from_cm, MAVLINK_TYPE_INT32_T},
    { &mavlink_send_dm_airspeed_in_m, &mavlink_set_dm_airspeed_from_m, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_cm_airspeed_in_m, &mavlink_set_cm_airspeed_from_m, MAVLINK_TYPE_FLOAT},
    { &mavlink_send_frame_anglerate, &mavlink_set_frame_anglerate, MAVLINK_TYPE_INT32_T},
    { &mavlink_send_dcm_angle, &mavlink_set_dcm_angle, MAVLINK_TYPE_INT32_T},
    };

const mavlink_parameter mavlink_parameters_list[] = {
     {"MAG_CAL_RAW0" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &rawMagCalib[0], sizeof(rawMagCalib[0]) },
    {"MAG_CAL_RAW1" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &rawMagCalib[1], sizeof(rawMagCalib[1]) },
    {"MAG_CAL_RAW2" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &rawMagCalib[2], sizeof(rawMagCalib[2]) },
    {"MAG_GAIN0" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &magGain[0], sizeof(magGain[0]) },
    {"MAG_GAIN1" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &magGain[1], sizeof(magGain[1]) },
    {"MAG_GAIN2" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &magGain[2], sizeof(magGain[2]) },
    {"MAG_OFFSET0" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_magOffset[0], sizeof(udb_magOffset[0]) },
    {"MAG_OFFSET1" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_magOffset[1], sizeof(udb_magOffset[1]) },
    {"MAG_OFFSET2" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_magOffset[2], sizeof(udb_magOffset[2]) },
    {"MAG_DECLINATION" , {.param_int32=-180} , {.param_int32=180} , UDB_TYPE_INT_CIRCULAR, PARAMETER_READWRITE, (void*) &dcm_declination_angle.BB, sizeof(dcm_declination_angle.BB) },

    {"PWTRIM_AILERON" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[AILERON_INPUT_CHANNEL], sizeof(udb_pwTrim[AILERON_INPUT_CHANNEL]) },
    {"PWTRIM_ELEVATOR" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[ELEVATOR_INPUT_CHANNEL], sizeof(udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) },
    {"PWTRIM_RUDDER" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[RUDDER_INPUT_CHANNEL], sizeof(udb_pwTrim[RUDDER_INPUT_CHANNEL]) },
    {"PWTRIM_AILERON2" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[AILERON_SECONDARY_INPUT_CHANNEL], sizeof(udb_pwTrim[AILERON_SECONDARY_INPUT_CHANNEL]) },
    {"PWTRIM_ROLL" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[ROLL_INPUT_CHANNEL], sizeof(udb_pwTrim[ROLL_INPUT_CHANNEL]) },
    {"PWTRIM_PITCH" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[PITCH_INPUT_CHANNEL], sizeof(udb_pwTrim[PITCH_INPUT_CHANNEL]) },
    {"PWTRIM_THROTTLE" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[THROTTLE_INPUT_CHANNEL], sizeof(udb_pwTrim[THROTTLE_INPUT_CHANNEL]) },
    {"PWTRIM_YAW" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[YAW_INPUT_CHANNEL], sizeof(udb_pwTrim[YAW_INPUT_CHANNEL]) },
    {"PWTRIM_FLAP" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[FLAP_INPUT_CHANNEL], sizeof(udb_pwTrim[FLAP_INPUT_CHANNEL]) },
    {"PWTRIM_AIRBRAKE" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[BRAKE_INPUT_CHANNEL], sizeof(udb_pwTrim[BRAKE_INPUT_CHANNEL]) },
    {"PWTRIM_SPOILER" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[SPOILER_INPUT_CHANNEL], sizeof(udb_pwTrim[SPOILER_INPUT_CHANNEL]) },
    {"PWTRIM_CAMBER" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[CAMBER_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMBER_INPUT_CHANNEL]) },
    {"PWTRIM_CROW" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[CROW_INPUT_CHANNEL], sizeof(udb_pwTrim[CROW_INPUT_CHANNEL]) },
    {"PWTRIM_CAMPITCH" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL]) },
    {"PWTRIM_CAM_YAW" , {.param_float=800.0} , {.param_float=2200.0} , UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*) &udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL]) },

    {"IMU_XACCEL_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_xaccel.offset, sizeof(udb_xaccel.offset) },
    {"IMU_YACCEL_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_yaccel.offset, sizeof(udb_yaccel.offset) },
    {"IMU_ZACCEL_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_zaccel.offset, sizeof(udb_zaccel.offset) },
    {"IMU_XGYRO_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_xrate.offset, sizeof(udb_xrate.offset) },
    {"IMU_YGYRO_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_yrate.offset, sizeof(udb_yrate.offset) },
    {"IMU_ZGYRO_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_zrate.offset, sizeof(udb_zrate.offset) },
    {"IMU_VREF_OFF" , {.param_int32=-32767} , {.param_int32=32767} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &udb_vref.offset, sizeof(udb_vref.offset) },

    {"TH_H_TARGET_MIN" , {.param_int32=0} , {.param_int32=1} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &height_target_min, sizeof(height_target_min) },
    {"TH_H_TARGET_MAX" , {.param_int32=0} , {.param_int32=1} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &height_target_max, sizeof(height_target_max) },
    {"TH_H_MARGIN" , {.param_int32=1} , {.param_int32=500} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &height_margin, sizeof(height_margin) },
    {"TH_T_HOLD_MIN" , {.param_float=0} , {.param_float=1} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &alt_hold_throttle_min, sizeof(alt_hold_throttle_min) },
    {"TH_T_HOLD_MAX" , {.param_float=0} , {.param_float=1} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &alt_hold_throttle_max, sizeof(alt_hold_throttle_max) },
    {"TH_P_HOLD_MIN" , {.param_int32=-89} , {.param_int32=0} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &alt_hold_pitch_min, sizeof(alt_hold_pitch_min) },
    {"TH_P_HOLD_MAX" , {.param_int32=0} , {.param_int32=89} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &alt_hold_pitch_max, sizeof(alt_hold_pitch_max) },

    {"ASPD_DESIRED" , {.param_float=0} , {.param_float=300.0} , UDB_TYPE_M_AIRSPEED_TO_DM, PARAMETER_READWRITE, (void*) &desiredSpeed, sizeof(desiredSpeed) },
    {"ASPD_MIN_GSPD" , {.param_float=0} , {.param_float=20000} , UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*) &minimum_groundspeed, sizeof(minimum_groundspeed) },
    {"ASPD_MIN" , {.param_float=0} , {.param_float=300.0} , UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*) &minimum_airspeed, sizeof(minimum_airspeed) },
    {"ASPD_MAX" , {.param_float=0} , {.param_float=300.0} , UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*) &maximum_airspeed, sizeof(maximum_airspeed) },
    {"ASPD_CRUISE" , {.param_float=0} , {.param_float=300.0} , UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*) &cruise_airspeed, sizeof(cruise_airspeed) },

    {"FBW_MAX_R_ANGLE" , {.param_int32=20} , {.param_int32=180.0} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &fbw_rollPositionMax, sizeof(fbw_rollPositionMax) },

    {"NAV_MAX_R_ANGLE" , {.param_float=20} , {.param_float=180.0} , UDB_TYPE_Q16_ANGLE, PARAMETER_READWRITE, (void*) &nav_rollPositionMax, sizeof(nav_rollPositionMax) },
    {"NAV_ROLL_RATE" , {.param_float=1} , {.param_float=180.0} , UDB_TYPE_Q16_ANGLE, PARAMETER_READWRITE, (void*) &nav_roll_rate, sizeof(nav_roll_rate) },
    {"NAV_LOITER_RAD" , {.param_int32=0} , {.param_int32=10000.0} , UDB_TYPE_INT, PARAMETER_READWRITE, (void*) &loiter_radius, sizeof(loiter_radius) },
    {"NAV_ROLL_GAIN" , {.param_float=0.01} , {.param_float=1.0} , UDB_TYPE_Q14, PARAMETER_READWRITE, (void*) &auto_nav_roll_gain, sizeof(auto_nav_roll_gain) },

    {"GAIN_RATE_LOAD" , {.param_float=-100.0} , {.param_float=100.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &rate_error_load_gain, sizeof(rate_error_load_gain) },
    {"GAIN_PITCH_RATE" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &pitch_error_rate_gain, sizeof(pitch_error_rate_gain) },
    {"GAIN_YAW_DAMP" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &yaw_damping, sizeof(yaw_damping) },
    {"GAIN_YAWFFCORR" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &yaw_ff_correction_rate, sizeof(yaw_ff_correction_rate) },
    {"GAIN_YAWCORRTH" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &yaw_ff_correct_thresh, sizeof(yaw_ff_correct_thresh) },
    {"GAIN_YAWFF" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &yaw_ffgain_Q16, sizeof(yaw_ffgain_Q16) },
    {"GAIN_ROLL_RATE" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &roll_error_rate_gain, sizeof(roll_error_rate_gain) },
    {"GAIN_ROLL_DAMP" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &roll_damping, sizeof(roll_damping) },
    {"GAIN_ACOR" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &aoa_offset_correction, sizeof(aoa_offset_correction) },
    {"GAIN_ACOR_RAT" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &aoa_offset_correction_rate, sizeof(aoa_offset_correction_rate) },
    {"GAIN_ACOR_LDMAX" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &aoa_offset_correction_max_load, sizeof(aoa_offset_correction_max_load) },
    {"GAIN_ACOR_LDMIN" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &aoa_offset_correction_min_load, sizeof(aoa_offset_correction_min_load) },
    {"GAIN_ACOR_CLMAX" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &aoa_offset_correction_max_Cl, sizeof(aoa_offset_correction_max_Cl) },
    {"GAIN_ACOR_CLMIN" , {.param_float=-1000.0} , {.param_float=1000.0} , UDB_TYPE_Q16, PARAMETER_READWRITE, (void*) &aoa_offset_correction_min_Cl, sizeof(aoa_offset_correction_min_Cl) },

    };

const uint16_t count_of_parameters_list = sizeof(mavlink_parameters_list) / sizeof(mavlink_parameter);


#endif 

