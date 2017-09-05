// pyparam generated file - DO NOT EDIT

// this module is a horrible hack to work around VC++ not allowing
// static initialisation of named union member variables
#ifdef _MSC_VER

#include "defines.h" 
#include "options_mavlink.h"

#if (USE_MAVLINK == 1)

#include "parameter_table.h"
#include "data_storage.h"

void parameter_table_init(void)
{
	mavlink_parameters_list[0].min.param_float=0.0; mavlink_parameters_list[0].max.param_float=0.5; // rollkp - PID_ROLLKP
	mavlink_parameters_list[1].min.param_float=0.0; mavlink_parameters_list[1].max.param_float=0.5; // rollkd - PID_ROLLKD
	mavlink_parameters_list[2].min.param_float=0.0; mavlink_parameters_list[2].max.param_float=0.5; // yawkpail - PID_YAWKPAIL
	mavlink_parameters_list[3].min.param_float=0.0; mavlink_parameters_list[3].max.param_float=0.5; // yawkdail - PID_YAWKDAIL
	mavlink_parameters_list[4].min.param_float=0.0; mavlink_parameters_list[4].max.param_float=0.5; // pitchgain - PID_PITCHGAIN
	mavlink_parameters_list[5].min.param_float=0.0; mavlink_parameters_list[5].max.param_float=0.5; // pitchkd - PID_PITCHKD
	mavlink_parameters_list[6].min.param_float=0.0; mavlink_parameters_list[6].max.param_float=0.5; // rollkprud - PID_ROLLKPRUD
	mavlink_parameters_list[7].min.param_float=0.0; mavlink_parameters_list[7].max.param_float=0.5; // yawkprud - PID_YAWKPRUD
	mavlink_parameters_list[8].min.param_float=0.0; mavlink_parameters_list[8].max.param_float=0.5; // yawkdrud - PID_YAWKDRUD
	mavlink_parameters_list[9].min.param_float=0.0; mavlink_parameters_list[9].max.param_float=0.5; // rollkprud - PID_ROLLKPRUD
	mavlink_parameters_list[10].min.param_float=0.0; mavlink_parameters_list[10].max.param_float=0.5; // rollkdrud - PID_ROLLKDRUD

	mavlink_parameters_list[11].min.param_int32=-32767; mavlink_parameters_list[11].max.param_int32=32767; // rawMagCalib[0] - MAG_CAL_RAW0
	mavlink_parameters_list[12].min.param_int32=-32767; mavlink_parameters_list[12].max.param_int32=32767; // rawMagCalib[1] - MAG_CAL_RAW1
	mavlink_parameters_list[13].min.param_int32=-32767; mavlink_parameters_list[13].max.param_int32=32767; // rawMagCalib[2] - MAG_CAL_RAW2
	mavlink_parameters_list[14].min.param_int32=-32767; mavlink_parameters_list[14].max.param_int32=32767; // magGain[0] - MAG_GAIN0
	mavlink_parameters_list[15].min.param_int32=-32767; mavlink_parameters_list[15].max.param_int32=32767; // magGain[1] - MAG_GAIN1
	mavlink_parameters_list[16].min.param_int32=-32767; mavlink_parameters_list[16].max.param_int32=32767; // magGain[2] - MAG_GAIN2
	mavlink_parameters_list[17].min.param_int32=-32767; mavlink_parameters_list[17].max.param_int32=32767; // udb_magOffset[0] - MAG_OFFSET0
	mavlink_parameters_list[18].min.param_int32=-32767; mavlink_parameters_list[18].max.param_int32=32767; // udb_magOffset[1] - MAG_OFFSET1
	mavlink_parameters_list[19].min.param_int32=-32767; mavlink_parameters_list[19].max.param_int32=32767; // udb_magOffset[2] - MAG_OFFSET2
	mavlink_parameters_list[20].min.param_int32=-180; mavlink_parameters_list[20].max.param_int32=180; // dcm_declination_angle.BB - MAG_DECLINATION

	mavlink_parameters_list[21].min.param_float=800.0; mavlink_parameters_list[21].max.param_float=2200.0; // udb_pwTrim[AILERON_INPUT_CHANNEL] - PWTRIM_AILERON
	mavlink_parameters_list[22].min.param_float=800.0; mavlink_parameters_list[22].max.param_float=2200.0; // udb_pwTrim[ELEVATOR_INPUT_CHANNEL] - PWTRIM_ELEVATOR
	mavlink_parameters_list[23].min.param_float=800.0; mavlink_parameters_list[23].max.param_float=2200.0; // udb_pwTrim[RUDDER_INPUT_CHANNEL] - PWTRIM_RUDDER
	mavlink_parameters_list[24].min.param_float=800.0; mavlink_parameters_list[24].max.param_float=2200.0; // udb_pwTrim[AILERON_SECONDARY_INPUT_CHANNEL] - PWTRIM_AILERON2
	mavlink_parameters_list[25].min.param_float=800.0; mavlink_parameters_list[25].max.param_float=2200.0; // udb_pwTrim[ROLL_INPUT_CHANNEL] - PWTRIM_ROLL
	mavlink_parameters_list[26].min.param_float=800.0; mavlink_parameters_list[26].max.param_float=2200.0; // udb_pwTrim[PITCH_INPUT_CHANNEL] - PWTRIM_PITCH
	mavlink_parameters_list[27].min.param_float=800.0; mavlink_parameters_list[27].max.param_float=2200.0; // udb_pwTrim[THROTTLE_INPUT_CHANNEL] - PWTRIM_THROTTLE
	mavlink_parameters_list[28].min.param_float=800.0; mavlink_parameters_list[28].max.param_float=2200.0; // udb_pwTrim[YAW_INPUT_CHANNEL] - PWTRIM_YAW
	mavlink_parameters_list[29].min.param_float=800.0; mavlink_parameters_list[29].max.param_float=2200.0; // udb_pwTrim[FLAP_INPUT_CHANNEL] - PWTRIM_FLAP
	mavlink_parameters_list[30].min.param_float=800.0; mavlink_parameters_list[30].max.param_float=2200.0; // udb_pwTrim[BRAKE_INPUT_CHANNEL] - PWTRIM_AIRBRAKE
	mavlink_parameters_list[31].min.param_float=800.0; mavlink_parameters_list[31].max.param_float=2200.0; // udb_pwTrim[SPOILER_INPUT_CHANNEL] - PWTRIM_SPOILER
	mavlink_parameters_list[32].min.param_float=800.0; mavlink_parameters_list[32].max.param_float=2200.0; // udb_pwTrim[CAMBER_INPUT_CHANNEL] - PWTRIM_CAMBER
	mavlink_parameters_list[33].min.param_float=800.0; mavlink_parameters_list[33].max.param_float=2200.0; // udb_pwTrim[CROW_INPUT_CHANNEL] - PWTRIM_CROW
	mavlink_parameters_list[34].min.param_float=800.0; mavlink_parameters_list[34].max.param_float=2200.0; // udb_pwTrim[CAMERA_PITCH_INPUT_CHANNEL] - PWTRIM_CAMPITCH
	mavlink_parameters_list[35].min.param_float=800.0; mavlink_parameters_list[35].max.param_float=2200.0; // udb_pwTrim[CAMERA_YAW_INPUT_CHANNEL] - PWTRIM_CAM_YAW

	mavlink_parameters_list[36].min.param_int32=-32767; mavlink_parameters_list[36].max.param_int32=32767; // udb_xaccel.offset - IMU_XACCEL_OFF
	mavlink_parameters_list[37].min.param_int32=-32767; mavlink_parameters_list[37].max.param_int32=32767; // udb_yaccel.offset - IMU_YACCEL_OFF
	mavlink_parameters_list[38].min.param_int32=-32767; mavlink_parameters_list[38].max.param_int32=32767; // udb_zaccel.offset - IMU_ZACCEL_OFF
	mavlink_parameters_list[39].min.param_int32=-32767; mavlink_parameters_list[39].max.param_int32=32767; // udb_xrate.offset - IMU_XGYRO_OFF
	mavlink_parameters_list[40].min.param_int32=-32767; mavlink_parameters_list[40].max.param_int32=32767; // udb_yrate.offset - IMU_YGYRO_OFF
	mavlink_parameters_list[41].min.param_int32=-32767; mavlink_parameters_list[41].max.param_int32=32767; // udb_zrate.offset - IMU_ZGYRO_OFF
	mavlink_parameters_list[42].min.param_int32=-32767; mavlink_parameters_list[42].max.param_int32=32767; // udb_vref.offset - IMU_VREF_OFF

	mavlink_parameters_list[43].min.param_int32=0; mavlink_parameters_list[43].max.param_int32=5000; // height_target_min - TH_H_TARGET_MIN
	mavlink_parameters_list[44].min.param_int32=0; mavlink_parameters_list[44].max.param_int32=5000; // height_target_max - TH_H_TARGET_MAX
	mavlink_parameters_list[45].min.param_int32=1; mavlink_parameters_list[45].max.param_int32=500; // height_margin - TH_H_MARGIN
	mavlink_parameters_list[46].min.param_float=0; mavlink_parameters_list[46].max.param_float=1; // alt_hold_throttle_min - TH_T_HOLD_MIN
	mavlink_parameters_list[47].min.param_float=0; mavlink_parameters_list[47].max.param_float=1; // alt_hold_throttle_max - TH_T_HOLD_MAX
	mavlink_parameters_list[48].min.param_int32=-89; mavlink_parameters_list[48].max.param_int32=0; // alt_hold_pitch_min - TH_P_HOLD_MIN
	mavlink_parameters_list[49].min.param_int32=0; mavlink_parameters_list[49].max.param_int32=89; // alt_hold_pitch_max - TH_P_HOLD_MAX
	mavlink_parameters_list[50].min.param_int32=0; mavlink_parameters_list[50].max.param_int32=89; // alt_hold_pitch_high - TH_P_HIGH
	mavlink_parameters_list[51].min.param_int32=0; mavlink_parameters_list[51].max.param_int32=89; // rtl_pitch_down - TH_P_RTL_DOWN

	mavlink_parameters_list[52].min.param_float=0; mavlink_parameters_list[52].max.param_float=300.0; // desiredSpeed - ASPD_DESIRED
	mavlink_parameters_list[53].min.param_float=0; mavlink_parameters_list[53].max.param_float=20000; // minimum_groundspeed - ASPD_MIN_GSPD
	mavlink_parameters_list[54].min.param_float=0; mavlink_parameters_list[54].max.param_float=300.0; // minimum_airspeed - ASPD_MIN
	mavlink_parameters_list[55].min.param_float=0; mavlink_parameters_list[55].max.param_float=300.0; // maximum_airspeed - ASPD_MAX
	mavlink_parameters_list[56].min.param_float=0; mavlink_parameters_list[56].max.param_float=300.0; // cruise_airspeed - ASPD_CRUISE
	mavlink_parameters_list[57].min.param_int32=-90; mavlink_parameters_list[57].max.param_int32=90.0; // airspeed_pitch_min_aspd - ASPD_P_MIN_ASPD
	mavlink_parameters_list[58].min.param_int32=-90; mavlink_parameters_list[58].max.param_int32=90.0; // airspeed_pitch_max_aspd - ASPD_P_MAX_ASPD
	mavlink_parameters_list[59].min.param_int32=1.0; mavlink_parameters_list[59].max.param_int32=720.0; // airspeed_pitch_adjust_rate - ASPD_P_RATE_LIM
	mavlink_parameters_list[60].min.param_float=0.0; mavlink_parameters_list[60].max.param_float=1.0; // airspeed_pitch_ki - ASPD_P_KI
	mavlink_parameters_list[61].min.param_int32=0.0; mavlink_parameters_list[61].max.param_int32=45.0; // airspeed_pitch_ki_limit - ASPD_P_KI_LIMIT

	mavlink_parameters_list[62].min.param_float=-1.0; mavlink_parameters_list[62].max.param_float=1.0; // turns.ElevatorTrimNormal - TURN_ELE_TR_NRM
	mavlink_parameters_list[63].min.param_float=-1.0; mavlink_parameters_list[63].max.param_float=1.0; // turns.ElevatorTrimInverted - TURN_ELE_TR_INV
	mavlink_parameters_list[64].min.param_float=0.0; mavlink_parameters_list[64].max.param_float=999.0; // turns.CruiseSpeed - TURN_CRUISE_SPD
	mavlink_parameters_list[65].min.param_float=-90.0; mavlink_parameters_list[65].max.param_float=90.0; // turns.AngleOfAttackNormal - TURN_AOA_NORMAL
	mavlink_parameters_list[66].min.param_float=-90.0; mavlink_parameters_list[66].max.param_float=90.0; // turns.AngleOfAttackInverted - TURN_AOA_INV
	mavlink_parameters_list[67].min.param_float=0.0; mavlink_parameters_list[67].max.param_float=100.0; // turns.FeedForward - TURN_FEED_FWD
	mavlink_parameters_list[68].min.param_float=0.0; mavlink_parameters_list[68].max.param_float=100.0; // turns.TurnRateNav - TURN_RATE_NAV
	mavlink_parameters_list[69].min.param_float=0.0; mavlink_parameters_list[69].max.param_float=100.0; // turns.TurnRateFBW - TURN_RATE_FBW

};

#endif // (USE_MAVLINK == 1)
#endif // _MSC_VER
