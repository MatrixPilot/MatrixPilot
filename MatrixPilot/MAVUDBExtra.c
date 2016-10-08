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
#include "flightplan_waypoints.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/estWind.h"
#include "../libDCM/rmat.h"
#if (SILSIM != 1)
#include "../libUDB/libUDB.h" // Needed for access to RCON
#endif

#include "../libUDB/mcu.h"
#include "../libUDB/servoOut.h"

extern uint16_t maxstack;

static union intbb voltage_milis = {0};
int16_t mavlink_sue_telemetry_counter = 8; // Countdown counter, for use with SERIAL_UDB_EXTRA compatibility
boolean mavlink_sue_telemetry_f2_a = true;

// Following are required for saving state of PWM variables for SERIAL_UDB_EXTRA compatibility
#define MAVLINK_SUE_CHANNEL_MAX_SIZE 10 //  MatrixPilot.xml MAVLink has fixed SUE protocol for 10 channels
int16_t pwIn_save[MAVLINK_SUE_CHANNEL_MAX_SIZE + 1];
int16_t pwOut_save[MAVLINK_SUE_CHANNEL_MAX_SIZE + 1];

void MAVUDBExtraOutput_40hz(void)
{
	// SEND SERIAL_UDB_EXTRA (SUE) VIA MAVLINK FOR BACKWARDS COMPATIBILITY with FLAN.PYW (FLIGHT ANALYZER)
	// The MAVLink messages for this section of code are unique to MatrixPilot and are defined in matrixpilot.xml
//	spread_transmission_load = 10;
//	if (mavlink_frequency_send(streamRates[MAV_DATA_STREAM_EXTRA1], mavlink_counter_40hz + spread_transmission_load)) // SUE code historically ran at 8HZ
	{
		switch (mavlink_sue_telemetry_counter)
		{
			case 8:
				mavlink_msg_serial_udb_extra_f14_send(MAVLINK_COMM_0, WIND_ESTIMATION, GPS_TYPE, DEADRECKONING, BOARD_TYPE, AIRFRAME_TYPE,
				    get_reset_flags(), trap_flags, trap_source, osc_fail_count, CLOCK_CONFIG, FLIGHT_PLAN_TYPE);
				mavlink_sue_telemetry_counter--;
				break;
			case 7:
				mavlink_msg_serial_udb_extra_f15_send(MAVLINK_COMM_0, (uint8_t*)ID_VEHICLE_MODEL_NAME, (uint8_t*)ID_VEHICLE_REGISTRATION);
				mavlink_sue_telemetry_counter--;
				break;
			case 6:
				mavlink_msg_serial_udb_extra_f16_send(MAVLINK_COMM_0, (uint8_t*)ID_LEAD_PILOT, (uint8_t*)ID_DIY_DRONES_URL);
				mavlink_sue_telemetry_counter--;
				break;
			case 5:
				mavlink_msg_serial_udb_extra_f4_send(MAVLINK_COMM_0, settings._.RollStabilizaionAilerons, settings._.RollStabilizationRudder, settings._.PitchStabilization,
				    settings._.YawStabilizationRudder, settings._.YawStabilizationAileron, settings._.AileronNavigation, settings._.RudderNavigation, settings._.AltitudeholdStabilized,
				    settings._.AltitudeholdWaypoint, settings._.RacingMode);
				mavlink_sue_telemetry_counter--;
				break;
			case 4:
				mavlink_msg_serial_udb_extra_f5_send(MAVLINK_COMM_0, gains.YawKPAileron, gains.YawKDAileron, gains.RollKP, gains.RollKD,
				    settings._.YawStabilizationAileron, 0);
				mavlink_sue_telemetry_counter--;
				break;
			case 3:
				mavlink_msg_serial_udb_extra_f6_send(MAVLINK_COMM_0, gains.Pitchgain, gains.PitchKD, 0, 0, gains.ElevatorBoost);
				mavlink_sue_telemetry_counter--;
				break;
			case 2:
				mavlink_msg_serial_udb_extra_f7_send(MAVLINK_COMM_0, gains.YawKPRudder, gains.YawKDRudder, gains.RollKPRudder, gains.RollKDRudder,
				    gains.RudderBoost, gains.RtlPitchDown);
				mavlink_sue_telemetry_counter--;
				break;
			case 1:
				mavlink_msg_serial_udb_extra_f8_send(MAVLINK_COMM_0, altit.HeightTargetMax, altit.HeightTargetMin, altit.AltHoldThrottleMin,
				    altit.AltHoldThrottleMax, altit.AltHoldPitchMin, altit.AltHoldPitchMax, altit.AltHoldPitchHigh);
				mavlink_sue_telemetry_counter--;
				break;
			default:
			{
				if (mavlink_sue_telemetry_f2_a == true)
				{
					int16_t i;

					mavlink_sue_telemetry_f2_a = false;
					// Approximate time passing between each telemetry line, even though
					// we may not have new GPS time data each time through.
					// This line is important when GPS lock is lost during flight
					// It allows telemetry to have a time reference when the GPS time reference is lost
					// Note this does increment the official Time of Week (TOW) for the entire system.
					// It is not changed for now, to preserve close compatibility with origin SERIAL_UDB_EXTRA code.
					if (tow.WW > 0) tow.WW += 250;

					if (state_flags._.f13_print_req == 1)
					{
						// The F13 line of telemetry is printed just once  when origin has been captured after GPS lock
						mavlink_msg_serial_udb_extra_f13_send(MAVLINK_COMM_0, week_no.BB, lat_origin.WW, lon_origin.WW, alt_origin.WW);
						state_flags._.f13_print_req = 0;
					}
#if (MAG_YAW_DRIFT == 1)
					mavlink_msg_serial_udb_extra_f2_a_send(MAVLINK_COMM_0, tow.WW,
					    ((udb_flags._.radio_on << 2) + (dcm_flags._.nav_capable << 1) + state_flags._.GPS_steering),
					    lat_gps.WW, lon_gps.WW, alt_sl_gps.WW, waypointIndex,
					    rmat[0], rmat[1], rmat[2], rmat[3], rmat[4], rmat[5], rmat[6], rmat[7], rmat[8],
					    (uint16_t) cog_gps.BB, sog_gps.BB, (uint16_t) udb_cpu_load(), voltage_milis.BB,
					    air_speed_3DIMU, estimatedWind[0], estimatedWind[1], estimatedWind[2],
					    magFieldEarth[0], magFieldEarth[1], magFieldEarth[2],
					    svs, hdop);
#else
					mavlink_msg_serial_udb_extra_f2_a_send(MAVLINK_COMM_0, tow.WW,
					    ((udb_flags._.radio_on << 2) + (dcm_flags._.nav_capable << 1) + state_flags._.GPS_steering),
					    lat_gps.WW, lon_gps.WW, alt_sl_gps.WW, waypointIndex,
					    rmat[0], rmat[1], rmat[2], rmat[3], rmat[4], rmat[5], rmat[6], rmat[7], rmat[8],
					    (uint16_t) cog_gps.BB, sog_gps.BB, (uint16_t) udb_cpu_load(), voltage_milis.BB,
					    air_speed_3DIMU, estimatedWind[0], estimatedWind[1], estimatedWind[2],
					    0, 0, 0,
					    svs, hdop);
#endif // (MAG_YAW_DRIFT == 1)
					// Save  pwIn and PwOut buffers for sending next time around in f2_b format message
					for (i = 0; i <= (NUM_INPUTS > MAVLINK_SUE_CHANNEL_MAX_SIZE ? MAVLINK_SUE_CHANNEL_MAX_SIZE : NUM_INPUTS); i++)
						pwIn_save[i] = udb_pwIn[i];
					for (i = 0; i <= (NUM_OUTPUTS > MAVLINK_SUE_CHANNEL_MAX_SIZE ? MAVLINK_SUE_CHANNEL_MAX_SIZE : NUM_OUTPUTS); i++)
						pwOut_save[i] = udb_pwOut[i];
					}
					else
					{
						vect3_16t goal;
						int16_t stack_free = 0;
						mavlink_sue_telemetry_f2_a = true;
#if (RECORD_FREE_STACK_SPACE == 1)
						stack_free = (int16_t)(4096-maxstack); // This is actually wrong for the UDB4, but currently left the same as for telemetry.c
#endif // (RECORD_FREE_STACK_SPACE == 1)

//void navigate_get_goal(vect3_16t* goal);
						navigate_get_goal(&goal);

						mavlink_msg_serial_udb_extra_f2_b_send(MAVLINK_COMM_0, tow.WW,
						    pwIn_save[1], pwIn_save[2], pwIn_save[3], pwIn_save[4], pwIn_save[5],
						    pwIn_save[6], pwIn_save[7], pwIn_save[8], pwIn_save[9], pwIn_save[10],
						    pwOut_save[1], pwOut_save[2], pwOut_save[3], pwOut_save[4], pwOut_save[5],
						    pwOut_save[6], pwOut_save[7], pwOut_save[8], pwOut_save[9], pwOut_save[10],
						    IMUlocationx._.W1, IMUlocationy._.W1, IMUlocationz._.W1, state_flags.WW,
#if (SILSIM != 1)
						    osc_fail_count,
#else
						    0,
#endif // (SILSIM != 1)
						    IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1,
						    goal.x, goal.y, goal.z, stack_free);
					}
				}
		}
	}
}


#endif // (USE_MAVLINK == 1)
