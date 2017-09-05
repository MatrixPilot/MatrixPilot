// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "../MatrixPilot/defines.h"
#include "../MatrixPilot/states.h"
#include "options_mavlink.h"

#if (USE_MAVLINK == 1)

#include "MAVLink.h"
#include "MAVUDBExtra.h"
#include "config.h"
#include "navigate.h"
#include "altitudeCntrl.h"
#include "telemetry_log.h"
#include "flightplan_waypoints.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/estWind.h"
#include "../libDCM/estAltitude.h"
#include "../libDCM/rmat.h"
#if (SILSIM != 1)
#include "../libUDB/libUDB.h" // Needed for access to RCON
#endif
#include "../libUDB/ADchannel.h"
#include "../libUDB/mcu.h"
#include "../libUDB/servoOut.h"

extern uint16_t maxstack;

int16_t mavlink_sue_telemetry_counter = 13; // Countdown counter, for use with SERIAL_UDB_EXTRA compatibility
boolean mavlink_sue_toggle = false;

#define MAVLINK_SUE_CHANNEL_MAX_SIZE 12 //  MatrixPilot.xml MAVLink has fixed SUE protocol for 10 channels

void MAVUDBExtraOutput(void)
{
	// SEND SERIAL_UDB_EXTRA (SUE) VIA MAVLINK FOR BACKWARDS COMPATIBILITY with FLAN.PYW (FLIGHT ANALYZER)
	// SUE messages have important MatrixPilot specific information like cause of reboots e.g. power brownout.
	// The MAVLink messages for this section of code are defined in
	// Tools/MAVLink/mavlink/pymavlink/message_definitions/V1.0/matrixpilot.xml
	int16_t i;
	static int mavlink_sue_toggle = 0;
	static boolean f13_print_prepare = false;
	
	// Following are required for saving state of PWM variables for SERIAL_UDB_EXTRA compatibility
	static int16_t pwIn_save[MAVLINK_SUE_CHANNEL_MAX_SIZE + 1];
	static int16_t pwOut_save[MAVLINK_SUE_CHANNEL_MAX_SIZE + 1];
	static int16_t pwTrim_save[MAVLINK_SUE_CHANNEL_MAX_SIZE + 1];
	
	switch  (mavlink_sue_telemetry_counter)
	{
		case 13:
//			serial_output("F22:Sensors=%i,%i,%i,%i,%i,%i\n",
//				UDB_XACCEL.value, UDB_YACCEL.value,
//				UDB_ZACCEL.value + (Z_GRAVITY_SIGN ((int16_t)(2*GRAVITY))),
//				udb_xrate.value, udb_yrate.value, udb_zrate.value);
			mavlink_msg_serial_udb_extra_f22_send(MAVLINK_COMM_0,
				UDB_XACCEL.value, UDB_YACCEL.value,
				UDB_ZACCEL.value + (Z_GRAVITY_SIGN ((int16_t)(2*GRAVITY))),
				udb_xrate.value, udb_yrate.value, udb_zrate.value);
			break;
		case 12: 
//			serial_output("F21:Offsets=%i,%i,%i,%i,%i,%i\n",
//				UDB_XACCEL.offset, UDB_YACCEL.offset, UDB_ZACCEL.offset,
//				udb_xrate.offset, udb_yrate.offset, udb_zrate.offset);
			mavlink_msg_serial_udb_extra_f21_send(MAVLINK_COMM_0,
				UDB_XACCEL.offset, UDB_YACCEL.offset, UDB_ZACCEL.offset,
				udb_xrate.offset, udb_yrate.offset, udb_zrate.offset);
			break;
		case 11:
//			serial_output("F15:IDA=");
//			serial_output(ID_VEHICLE_MODEL_NAME);
//			serial_output(":IDB=");
//			serial_output(ID_VEHICLE_REGISTRATION);
//			serial_output(":\r\n");
			mavlink_msg_serial_udb_extra_f15_send(MAVLINK_COMM_0,
				(uint8_t*)ID_VEHICLE_MODEL_NAME, 
				(uint8_t*)ID_VEHICLE_REGISTRATION);
			break;
		case 10:
//			serial_output("F16:IDC=");
//			serial_output(ID_LEAD_PILOT);
//			serial_output(":IDD=");
//			serial_output(ID_DIY_DRONES_URL);
//			serial_output(":\r\n");
			mavlink_msg_serial_udb_extra_f16_send(MAVLINK_COMM_0, 
				(uint8_t*)ID_LEAD_PILOT,
				(uint8_t*)ID_DIY_DRONES_URL);
			break;
		case 9:
//			serial_output("F17:FD_FWD=%5.3f:TR_NAV=%5.3f:TR_FBW=%5.3f:\r\n",
//				turns.FeedForward, turns.TurnRateNav, turns.TurnRateFBW);
			mavlink_msg_serial_udb_extra_f17_send(MAVLINK_COMM_0,
				turns.FeedForward, turns.TurnRateNav, turns.TurnRateFBW);
			break;
		case 8:
//			serial_output("F18:AOA_NRM=%5.3f:AOA_INV=%5.3f:EL_TRIM_NRM=%5.3f:EL_TRIM_INV=%5.3f:CRUISE_SPD=%5.3f:\r\n",
//				turns.AngleOfAttackNormal, turns.AngleOfAttackInverted, turns.ElevatorTrimNormal,
//				turns.ElevatorTrimInverted, turns.RefSpeed);
			mavlink_msg_serial_udb_extra_f18_send(MAVLINK_COMM_0,
				turns.AngleOfAttackNormal, turns.AngleOfAttackInverted, turns.ElevatorTrimNormal,
				turns.ElevatorTrimInverted, turns.RefSpeed);
			break;
		case 7:
//			serial_output("F19:AIL=%i,%i:ELEV=%i,%i:THROT=%i,%i:RUDD=%i,%i:\r\n",
//				AILERON_OUTPUT_CHANNEL, AILERON_CHANNEL_REVERSED, ELEVATOR_OUTPUT_CHANNEL,ELEVATOR_CHANNEL_REVERSED,
//				THROTTLE_OUTPUT_CHANNEL, THROTTLE_CHANNEL_REVERSED, RUDDER_OUTPUT_CHANNEL,RUDDER_CHANNEL_REVERSED );
			mavlink_msg_serial_udb_extra_f19_send(MAVLINK_COMM_0,
				AILERON_OUTPUT_CHANNEL, AILERON_CHANNEL_REVERSED, ELEVATOR_OUTPUT_CHANNEL,ELEVATOR_CHANNEL_REVERSED,
				THROTTLE_OUTPUT_CHANNEL, THROTTLE_CHANNEL_REVERSED, RUDDER_OUTPUT_CHANNEL,RUDDER_CHANNEL_REVERSED);
			break;
		case 6:
//			serial_output("F14:WIND_EST=%i:GPS_TYPE=%i:DR=%i:BOARD_TYPE=%i:AIRFRAME=%i:"
//			              "RCON=0x%X:TRAP_FLAGS=0x%X:TRAP_SOURCE=0x%lX:ALARMS=%i:"
//			              "CLOCK=%i:FP=%d:\r\n",
//				WIND_ESTIMATION, GPS_TYPE, DEADRECKONING, BOARD_TYPE, AIRFRAME_TYPE,
//				get_reset_flags(), trap_flags, trap_source, osc_fail_count,
//				CLOCK_CONFIG, FLIGHT_PLAN_TYPE);
			mavlink_msg_serial_udb_extra_f14_send(MAVLINK_COMM_0, 
				WIND_ESTIMATION, GPS_TYPE, DEADRECKONING, BOARD_TYPE, AIRFRAME_TYPE,
				get_reset_flags(), trap_flags, trap_source, osc_fail_count,
				CLOCK_CONFIG, FLIGHT_PLAN_TYPE);
			break;
		case 5:
//			serial_output("F4:R_STAB_A=%i:R_STAB_RD=%i:P_STAB=%i:Y_STAB_R=%i:Y_STAB_A=%i:AIL_NAV=%i:RUD_NAV=%i:AH_STAB=%i:AH_WP=%i:RACE=%i:\r\n",
//				settings._.RollStabilizaionAilerons, settings._.RollStabilizationRudder, 
//				settings._.PitchStabilization, settings._.YawStabilizationRudder, 
//				settings._.YawStabilizationAileron, settings._.AileronNavigation,
//				settings._.RudderNavigation, settings._.AltitudeholdStabilized,
//				settings._.AltitudeholdWaypoint, settings._.RacingMode);
			mavlink_msg_serial_udb_extra_f4_send(MAVLINK_COMM_0, 
				settings._.RollStabilizaionAilerons, settings._.RollStabilizationRudder,
				settings._.PitchStabilization, settings._.YawStabilizationRudder,
				settings._.YawStabilizationAileron, settings._.AileronNavigation,
				settings._.RudderNavigation, settings._.AltitudeholdStabilized,
				settings._.AltitudeholdWaypoint, settings._.RacingMode);
			break;
		case 4:
//			serial_output("F5:YAWKP_A=%5.3f:YAWKD_A=%5.3f:ROLLKP=%5.3f:ROLLKD=%5.3f:A_BOOST=%5.3f:A_BOOST=NULL\r\n",
//				gains.YawKPAileron, gains.YawKDAileron, gains.RollKP, gains.RollKD);
			mavlink_msg_serial_udb_extra_f5_send(MAVLINK_COMM_0, 
				gains.YawKPAileron, gains.YawKDAileron, gains.RollKP, gains.RollKD);
			break;
		case 3:
//			serial_output("F6:P_GAIN=%5.3f:P_KD=%5.3f:RUD_E_MIX=NULL:ROL_E_MIX=NULL:E_BOOST=%3.1f:\r\n",
//				gains.Pitchgain, gains.PitchKD, gains.ElevatorBoost);
			mavlink_msg_serial_udb_extra_f6_send(MAVLINK_COMM_0,
				gains.Pitchgain, gains.PitchKD, 0, 0, gains.ElevatorBoost);
			break;
		case 2:
//			serial_output("F7:Y_KP_R=%5.4f:Y_KD_R=%5.3f:RLKP_RUD=%5.3f:RLKD_RUD=%5.3f:RUD_BOOST=%5.3f:RTL_PITCH_DN=%5.3f:\r\n",
//				gains.YawKPRudder, gains.YawKDRudder, gains.RollKPRudder, gains.RollKDRudder, gains.RudderBoost, gains.RtlPitchDown);
			mavlink_msg_serial_udb_extra_f7_send(MAVLINK_COMM_0,
				gains.YawKPRudder, gains.YawKDRudder, gains.RollKPRudder, gains.RollKDRudder, gains.RudderBoost, gains.RtlPitchDown);
			break;
		case 1:
//			serial_output("F8:H_MAX=%6.1f:H_MIN=%6.1f:MIN_THR=%3.2f:MAX_THR=%3.2f:PITCH_MIN_THR=%4.1f:PITCH_MAX_THR=%4.1f:PITCH_ZERO_THR=%4.1f:\r\n",
//				altit.HeightTargetMax, altit.HeightTargetMin, altit.AltHoldThrottleMin, altit.AltHoldThrottleMax,
//				altit.AltHoldPitchMin, altit.AltHoldPitchMax, altit.AltHoldPitchHigh);
			mavlink_msg_serial_udb_extra_f8_send(MAVLINK_COMM_0,
				altit.HeightTargetMax, altit.HeightTargetMin, altit.AltHoldThrottleMin, altit.AltHoldThrottleMax,
				altit.AltHoldPitchMin, altit.AltHoldPitchMax, altit.AltHoldPitchHigh);
			break;
		default:
		{
			// F2 below means "Format Revision 2: and is used by a Telemetry parser to invoke the right pattern matching
			// F2 is a compromise between easy reading of raw data in an ascii file and minimising extraneous data in the stream.
			
			mavlink_sue_toggle = !mavlink_sue_toggle;
			if (state_flags._.f13_print_req == 1)
			{
				if (mavlink_sue_toggle && !f13_print_prepare)
				{
					f13_print_prepare = true;
					return;  //wait for next run
				}
 			}
			if (!f13_print_prepare)
			{
				if (mavlink_sue_toggle)
				{
//					serial_output("F2:T%li:S%d%d%d:N%li:E%li:A%li:W%i:"
//					              "a%i:b%i:c%i:d%i:e%i:f%i:g%i:h%i:i%i:"
//					              "c%u:s%i:cpu%u:"
//					              "as%u:wvx%i:wvy%i:wvz%i:ma%i:mb%i:mc%i:svs%i:hd%i:",
//						tow.WW, udb_flags._.radio_on, dcm_flags._.nav_capable, state_flags._.GPS_steering,
//						lat_gps.WW, lon_gps.WW, alt_sl_gps.WW, waypointIndex,
//						rmat[0], rmat[1], rmat[2],
//						rmat[3], rmat[4], rmat[5],
//						rmat[6], rmat[7], rmat[8],
//						(uint16_t)cog_gps.BB, sog_gps.BB, (uint16_t)udb_cpu_load(), 
//						air_speed_3DIMU,
//						estimatedWind[0], estimatedWind[1], estimatedWind[2],
//#if (MAG_YAW_DRIFT == 1)
//						magFieldEarth[0], magFieldEarth[1], magFieldEarth[2],
//#else
//						(int16_t)0, (int16_t)0, (int16_t)0,
//#endif // MAG_YAW_DRIFT
//						svs, hdop);
					
					mavlink_msg_serial_udb_extra_f2_a_send(MAVLINK_COMM_0, 
						tow.WW, ((udb_flags._.radio_on << 2) + (dcm_flags._.nav_capable << 1) + state_flags._.GPS_steering),
						lat_gps.WW, lon_gps.WW, alt_sl_gps.WW, waypointIndex,
						rmat[0], rmat[1], rmat[2],
						rmat[3], rmat[4], rmat[5],
						rmat[6], rmat[7], rmat[8],
						(uint16_t) cog_gps.BB, sog_gps.BB, (uint16_t) udb_cpu_load(),
						air_speed_3DIMU, estimatedWind[0], estimatedWind[1], estimatedWind[2],
#if (MAG_YAW_DRIFT == 1)
						magFieldEarth[0], magFieldEarth[1], magFieldEarth[2],
#else
						(int16_t)0, (int16_t)0, (int16_t)0,
#endif
						svs, hdop);

					
					// Approximate time passing between each telemetry line, even though
					// we may not have new GPS time data each time through.
					// This line is important when GPS lock is lost during flight
					// It allows telemetry to have a time reference when the GPS time reference is lost
					// Note this does increment the official Time of Week (TOW) for the entire system,
					
					// The following code line assumes an update rate of 4HZ, (MAVUDBExtra() called at 8 HZ))
					// It is not changed for now, to preserve close compatibility with SERIAL_UDB_EXTRA code.

					if (tow.WW > 0) tow.WW += 250; 

					// Save  pwIn and PwOut buffers for printing next time around
					// Save  pwIn and PwOut buffers for sending next time around in f2_b format message
					for (i = 0; i <= MAVLINK_SUE_CHANNEL_MAX_SIZE; i++)
					{
						if (i <= NUM_INPUTS) 
						{
							pwIn_save[i] = udb_pwIn[i];
							pwTrim_save[i] = udb_pwTrim[i];
						}
						else
						{
							pwIn_save[i] = 0;
							pwTrim_save[i] = 0;
						}
						if (i <= NUM_OUTPUTS) 
						{
							pwOut_save[i] = udb_pwOut[i];
						}
						else
						{
							pwOut_save[i] = 0;
						}
					}	
				}
				else
				{
					vect3_16t goal;
					navigate_get_goal(&goal);
					int16_t stack_free = 0;
//					for (i= 1; i <= NUM_INPUTS; i++)
//						serial_output("p%ii%i:",i,pwIn_save[i]);
//					for (i= 1; i <= NUM_OUTPUTS; i++)
//						serial_output("p%io%i:",i,pwOut_save[i]);
//					serial_output("imx%i:imy%i:imz%i:lex%i:ley%i:lez%i:fgs%X:ofc%i:tx%i:ty%i:tz%i:G%d,%d,%d:AF%i,%i,%i:",IMUlocationx._.W1,IMUlocationy._.W1,IMUlocationz._.W1,
//					    locationErrorEarth[0], locationErrorEarth[1], locationErrorEarth[2],
//					    state_flags.WW, osc_fail_count,
//					    IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1, goal.x, goal.y, goal.z, aero_force[0], aero_force[1], aero_force[2]);
//#if (USE_BAROMETER_ALTITUDE == 1)
//					serial_output("tmp%i:prs%li:alt%li:",
//					    get_barometer_temperature(), get_barometer_pressure(), 
//					    get_barometer_altitude());
//#endif
//					serial_output("bmv%i:mA%i:mAh%i:",
//#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
//	                battery_voltage._.W1,
//#else
//	                (int16_t)0,
//#endif
//#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)                        
//					battery_current._.W1, battery_mAh_used._.W1);
//#else
//					(int16_t)0, (int16_t)0);                    
//#endif
//					serial_output("DH%i:",desiredHeight);
#if (RECORD_FREE_STACK_SPACE == 1)
					extern uint16_t maxstack;
					stack_free = (int16_t)(4096-maxstack); // This is actually wrong for the UDB4, but currently left the same as for telemetry.c
#endif // (RECORD_FREE_STACK_SPACE == 1)
//					serial_output("stk%d:", (int16_t)(4096-maxstack));
//					serial_output("\r\n");

					mavlink_msg_serial_udb_extra_f2_b_send(MAVLINK_COMM_0,
						tow.WW,
						pwIn_save[1], pwIn_save[2], pwIn_save[3], pwIn_save[4], pwIn_save[5],pwIn_save[6],
						pwIn_save[7], pwIn_save[8], pwIn_save[9], pwIn_save[10], pwIn_save[11], pwIn_save[12],
						pwOut_save[1], pwOut_save[2], pwOut_save[3], pwOut_save[4], pwOut_save[5], pwOut_save[6],
						pwOut_save[7], pwOut_save[8], pwOut_save[9], pwOut_save[10], pwOut_save[11], pwOut_save[12],
						IMUlocationx._.W1, IMUlocationy._.W1, IMUlocationz._.W1,
						locationErrorEarth[0], locationErrorEarth[1], locationErrorEarth[2],
						state_flags.WW,
#if (SILSIM != 1)
						osc_fail_count,
#else
						0,
#endif // (SILSIM != 1)
						IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1,
						goal.x, goal.y, goal.z,
						aero_force[0], aero_force[1], aero_force[2],
#if (USE_BAROMETER_ALTITUDE == 1)
						get_barometer_temperature(), get_barometer_pressure(), 
						get_barometer_altitude(),
#else
						(int16_t)0, (int16_t)0, (int16_t)0,
#endif					
#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
				                battery_voltage._.W1,
#else
				                (int16_t)0,
#endif
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)                        
						battery_current._.W1, battery_mAh_used._.W1,
#else
						(int16_t)0, (int16_t)0,                   
#endif
						desiredHeight,
						stack_free);
						
				}
			}
			if (state_flags._.f13_print_req == 1)
			{
				// The F13 line of telemetry is printed when origin has been captured and in between F2 lines in SERIAL_UDB_EXTRA
				if (!f13_print_prepare)
				{
					return;
				}
				else
				{
					f13_print_prepare = false;
				}
//				serial_output("F13:week%i:origN%li:origE%li:origA%li:\r\n", week_no, lat_origin.WW, lon_origin.WW, alt_origin);
				mavlink_msg_serial_udb_extra_f13_send(MAVLINK_COMM_0, 
					week_no.BB, lat_origin.WW, lon_origin.WW, alt_origin.WW);
				
//				serial_output("F20:NUM_IN=%i:TRIM=",NUM_INPUTS);
				mavlink_msg_serial_udb_extra_f20_send(MAVLINK_COMM_0, 
					NUM_INPUTS,                                                         \
					pwTrim_save[1], pwTrim_save[2],  pwTrim_save[3],  pwTrim_save[4],   \
					pwTrim_save[5], pwTrim_save[6],  pwTrim_save[7],  pwTrim_save[8],   \
					pwTrim_save[9], pwTrim_save[10], pwTrim_save[11], pwTrim_save[12]);
//				for (i = 1; i <= NUM_INPUTS; i++)
//				{
//					serial_output("%i,",udb_pwTrim[i]);
//				}
//				serial_output(":\r\n");
				state_flags._.f13_print_req = 0;
			}
			break;
		}
	}
	if (mavlink_sue_telemetry_counter)
	{
		mavlink_sue_telemetry_counter--;
	}
#if (USE_TELELOG == 1)
	log_swapbuf();
#endif
}

#endif // (USE_MAVLINK == 1)
