// pyparam generated file - DO NOT EDIT

#include "defines.h"
#include "options_mavlink.h"

#if (SILSIM == 0 && USE_MAVLINK == 1)

#include "parameter_table.h"
#include "data_storage.h"
#include "../libUDB/magnetometer.h"
#include "../libUDB/ADchannel.h"
#include "altitudeCntrl.h"
#include "airspeedCntrl.h"
#include "config.h"

#include "gain_variables.h"

const mavlink_parameter_parser mavlink_parameter_parsers[] = {
	{ &mavlink_send_param_int16, &mavlink_set_param_int16, MAVLINK_TYPE_INT32_T},
	{ &mavlink_send_param_Q14, &mavlink_set_param_Q14, MAVLINK_TYPE_FLOAT},
	{ &mavlink_send_param_pwtrim, &mavlink_set_param_pwtrim, MAVLINK_TYPE_FLOAT},
	{ &mavlink_send_param_gyroscale_Q14, &mavlink_set_param_gyroscale_Q14, MAVLINK_TYPE_FLOAT},
	{ &mavlink_send_int_circular, &mavlink_set_int_circular, MAVLINK_TYPE_INT32_T},
	{ &mavlink_send_dm_airspeed_in_cm, &mavlink_set_dm_airspeed_from_cm, MAVLINK_TYPE_INT32_T},
	{ &mavlink_send_dm_airspeed_in_m, &mavlink_set_dm_airspeed_from_m, MAVLINK_TYPE_FLOAT},
	{ &mavlink_send_cm_airspeed_in_m, &mavlink_set_cm_airspeed_from_m, MAVLINK_TYPE_FLOAT},
	{ &mavlink_send_frame_anglerate, &mavlink_set_frame_anglerate, MAVLINK_TYPE_INT32_T},
	{ &mavlink_send_dcm_angle, &mavlink_set_dcm_angle, MAVLINK_TYPE_INT32_T},
	{ &mavlink_send_param_float, &mavlink_set_param_float, MAVLINK_TYPE_FLOAT},
	};

#ifdef _MSC_VER
mavlink_parameter mavlink_parameters_list[] = {
#else
const mavlink_parameter mavlink_parameters_list[] = {
#endif // _MSC_VER
	{"PID_ROLLKP", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkp, sizeof(rollkp) },
	{"PID_ROLLKD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*)&rollkd, sizeof(rollkd) },
	{"PID_YAWKPAIL", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&yawkpail, sizeof(yawkpail) },
	{"PID_YAWKDAIL", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*)&yawkdail, sizeof(yawkdail) },
	{"PID_PITCHGAIN", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&pitchgain, sizeof(pitchgain) },
	{"PID_PITCHKD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&pitchkd, sizeof(pitchkd) },
	{"PID_ROLLKPRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkprud, sizeof(rollkprud) },
	{"PID_YAWKPRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&yawkprud, sizeof(yawkprud) },
	{"PID_YAWKDRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_GYROSCALE_Q14, PARAMETER_READWRITE, (void*)&yawkdrud, sizeof(yawkdrud) },
	{"PID_ROLLKPRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkprud, sizeof(rollkprud) },
	{"PID_ROLLKDRUD", {.param_float=0.0}, {.param_float=0.5}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&rollkdrud, sizeof(rollkdrud) },

	{"MAG_CAL_RAW0", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rawMagCalib[0], sizeof(rawMagCalib[0]) },
	{"MAG_CAL_RAW1", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rawMagCalib[1], sizeof(rawMagCalib[1]) },
	{"MAG_CAL_RAW2", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rawMagCalib[2], sizeof(rawMagCalib[2]) },
	{"MAG_GAIN0", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&magGain[0], sizeof(magGain[0]) },
	{"MAG_GAIN1", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&magGain[1], sizeof(magGain[1]) },
	{"MAG_GAIN2", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&magGain[2], sizeof(magGain[2]) },
	{"MAG_OFFSET0", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_magOffset[0], sizeof(udb_magOffset[0]) },
	{"MAG_OFFSET1", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_magOffset[1], sizeof(udb_magOffset[1]) },
	{"MAG_OFFSET2", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_magOffset[2], sizeof(udb_magOffset[2]) },
	{"MAG_DECLINATION", {.param_int32=-180}, {.param_int32=180}, UDB_TYPE_INT_CIRCULAR, PARAMETER_READWRITE, (void*)&dcm_declination_angle.BB, sizeof(dcm_declination_angle.BB) },

	{"PWTRIM_AILERON", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[AILERON_INPUT_CHANNEL], sizeof(udb_pwTrim[AILERON_INPUT_CHANNEL]) },
	{"PWTRIM_ELEVATOR", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[ELEVATOR_INPUT_CHANNEL], sizeof(udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) },
	{"PWTRIM_RUDDER", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[RUDDER_INPUT_CHANNEL], sizeof(udb_pwTrim[RUDDER_INPUT_CHANNEL]) },
	{"PWTRIM_AILERON2", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[AILERON_SECONDARY_INPUT_CHANNEL], sizeof(udb_pwTrim[AILERON_SECONDARY_INPUT_CHANNEL]) },
	{"PWTRIM_ROLL", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[ROLL_INPUT_CHANNEL], sizeof(udb_pwTrim[ROLL_INPUT_CHANNEL]) },
	{"PWTRIM_PITCH", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[PITCH_INPUT_CHANNEL], sizeof(udb_pwTrim[PITCH_INPUT_CHANNEL]) },
	{"PWTRIM_THROTTLE", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[THROTTLE_INPUT_CHANNEL], sizeof(udb_pwTrim[THROTTLE_INPUT_CHANNEL]) },
	{"PWTRIM_YAW", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[YAW_INPUT_CHANNEL], sizeof(udb_pwTrim[YAW_INPUT_CHANNEL]) },
	{"PWTRIM_FLAP", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[FLAP_INPUT_CHANNEL], sizeof(udb_pwTrim[FLAP_INPUT_CHANNEL]) },
	{"PWTRIM_AIRBRAKE", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[BRAKE_INPUT_CHANNEL], sizeof(udb_pwTrim[BRAKE_INPUT_CHANNEL]) },
	{"PWTRIM_SPOILER", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[SPOILER_INPUT_CHANNEL], sizeof(udb_pwTrim[SPOILER_INPUT_CHANNEL]) },
	{"PWTRIM_CAMBER", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CAMBER_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMBER_INPUT_CHANNEL]) },
	{"PWTRIM_CROW", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CROW_INPUT_CHANNEL], sizeof(udb_pwTrim[CROW_INPUT_CHANNEL]) },
	{"PWTRIM_CAMPITCH", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL]) },
	{"PWTRIM_CAM_YAW", {.param_float=800.0}, {.param_float=2200.0}, UDB_TYPE_PWTRIM, PARAMETER_READWRITE, (void*)&udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL], sizeof(udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL]) },

	{"IMU_XACCEL_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_xaccel.offset, sizeof(udb_xaccel.offset) },
	{"IMU_YACCEL_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_yaccel.offset, sizeof(udb_yaccel.offset) },
	{"IMU_ZACCEL_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_zaccel.offset, sizeof(udb_zaccel.offset) },
	{"IMU_XGYRO_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_xrate.offset, sizeof(udb_xrate.offset) },
	{"IMU_YGYRO_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_yrate.offset, sizeof(udb_yrate.offset) },
	{"IMU_ZGYRO_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_zrate.offset, sizeof(udb_zrate.offset) },
	{"IMU_VREF_OFF", {.param_int32=-32767}, {.param_int32=32767}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&udb_vref.offset, sizeof(udb_vref.offset) },

	{"TH_H_TARGET_MIN", {.param_int32=0}, {.param_int32=5000}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&height_target_min, sizeof(height_target_min) },
	{"TH_H_TARGET_MAX", {.param_int32=0}, {.param_int32=5000}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&height_target_max, sizeof(height_target_max) },
	{"TH_H_MARGIN", {.param_int32=1}, {.param_int32=500}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&height_margin, sizeof(height_margin) },
	{"TH_T_HOLD_MIN", {.param_float=0}, {.param_float=1}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&alt_hold_throttle_min, sizeof(alt_hold_throttle_min) },
	{"TH_T_HOLD_MAX", {.param_float=0}, {.param_float=1}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&alt_hold_throttle_max, sizeof(alt_hold_throttle_max) },
	{"TH_P_HOLD_MIN", {.param_int32=-89}, {.param_int32=0}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&alt_hold_pitch_min, sizeof(alt_hold_pitch_min) },
	{"TH_P_HOLD_MAX", {.param_int32=0}, {.param_int32=89}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&alt_hold_pitch_max, sizeof(alt_hold_pitch_max) },
	{"TH_P_HIGH", {.param_int32=0}, {.param_int32=89}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&alt_hold_pitch_high, sizeof(alt_hold_pitch_high) },
	{"TH_P_RTL_DOWN", {.param_int32=0}, {.param_int32=89}, UDB_TYPE_INT, PARAMETER_READWRITE, (void*)&rtl_pitch_down, sizeof(rtl_pitch_down) },

	{"ASPD_DESIRED", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_DM, PARAMETER_READWRITE, (void*)&desiredSpeed, sizeof(desiredSpeed) },
	{"ASPD_MIN_GSPD", {.param_float=0}, {.param_float=20000}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&minimum_groundspeed, sizeof(minimum_groundspeed) },
	{"ASPD_MIN", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&minimum_airspeed, sizeof(minimum_airspeed) },
	{"ASPD_MAX", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&maximum_airspeed, sizeof(maximum_airspeed) },
	{"ASPD_CRUISE", {.param_float=0}, {.param_float=300.0}, UDB_TYPE_M_AIRSPEED_TO_CM, PARAMETER_READWRITE, (void*)&cruise_airspeed, sizeof(cruise_airspeed) },
	{"ASPD_P_MIN_ASPD", {.param_int32=-90}, {.param_int32=90.0}, UDB_TYPE_DCM_ANGLE, PARAMETER_READWRITE, (void*)&airspeed_pitch_min_aspd, sizeof(airspeed_pitch_min_aspd) },
	{"ASPD_P_MAX_ASPD", {.param_int32=-90}, {.param_int32=90.0}, UDB_TYPE_DCM_ANGLE, PARAMETER_READWRITE, (void*)&airspeed_pitch_max_aspd, sizeof(airspeed_pitch_max_aspd) },
	{"ASPD_P_RATE_LIM", {.param_int32=1.0}, {.param_int32=720.0}, UDB_TYPE_FRAME_ANGLERATE, PARAMETER_READWRITE, (void*)&airspeed_pitch_adjust_rate, sizeof(airspeed_pitch_adjust_rate) },
	{"ASPD_P_KI", {.param_float=0.0}, {.param_float=1.0}, UDB_TYPE_Q14, PARAMETER_READWRITE, (void*)&airspeed_pitch_ki, sizeof(airspeed_pitch_ki) },
	{"ASPD_P_KI_LIMIT", {.param_int32=0.0}, {.param_int32=45.0}, UDB_TYPE_DCM_ANGLE, PARAMETER_READWRITE, (void*)&airspeed_pitch_ki_limit, sizeof(airspeed_pitch_ki_limit) },

	{"TURN_ELE_TR_NRM", {.param_float=-1.0}, {.param_float=1.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.ElevatorTrimNormal, sizeof(turns.ElevatorTrimNormal) },
	{"TURN_ELE_TR_INV", {.param_float=-1.0}, {.param_float=1.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.ElevatorTrimInverted, sizeof(turns.ElevatorTrimInverted) },
	{"TURN_CRUISE_SPD", {.param_float=0.0}, {.param_float=999.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.RefSpeed, sizeof(turns.RefSpeed) },
	{"TURN_AOA_NORMAL", {.param_float=-90.0}, {.param_float=90.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.AngleOfAttackNormal, sizeof(turns.AngleOfAttackNormal) },
	{"TURN_AOA_INV", {.param_float=-90.0}, {.param_float=90.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.AngleOfAttackInverted, sizeof(turns.AngleOfAttackInverted) },
	{"TURN_FEED_FWD", {.param_float=0.0}, {.param_float=100.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.FeedForward, sizeof(turns.FeedForward) },
	{"TURN_RATE_NAV", {.param_float=0.0}, {.param_float=100.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.TurnRateNav, sizeof(turns.TurnRateNav) },
	{"TURN_RATE_FBW", {.param_float=0.0}, {.param_float=100.0}, UDB_TYPE_FLOAT, PARAMETER_READWRITE, (void*)&turns.TurnRateFBW, sizeof(turns.TurnRateFBW) },

};

const uint16_t count_of_parameters_list = sizeof(mavlink_parameters_list) / sizeof(mavlink_parameter);


#endif  // (SILSIM == 0 && USE_MAVLINK == 1)
