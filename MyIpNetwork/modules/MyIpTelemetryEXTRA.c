#ifndef _MYIPTELEMETRYEXTRA_C_
#define _MYIPTELEMETRYEXTRA_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_TELEMETRY_EXTRA == 1)

#include "TCPIP Stack/TCPIP.h"
#include "MyIpTelemetryEXTRA.h"
#include "MyIpHelpers.h"
#include "../libUDB/libUDB_internal.h"
#include "../libDCM/libDCM_internal.h"

//////////////////////////
// Module Variables
uint32_t taskTimer_TelemetryEXTRA[MAX_NUM_INSTANCES_OF_MODULES];
boolean _show_static_data[MAX_NUM_INSTANCES_OF_MODULES];
boolean _flag_print_f13[MAX_NUM_INSTANCES_OF_MODULES];
extern int16_t waypointIndex;
extern union intbb voltage_milis;

#if (CATAPULT_LAUNCH_INPUT_CHANNEL != CHANNEL_UNUSED)
extern int16_t launch_throttle_delay_timeout, launch_trim_timeout, launch_success_check_timeout;
#endif

void SendTelemetryEXTRAPacket(uint8_t s);


void MyIpOnConnect_TelemetryEXTRA(const BYTE s)
{
    // Print any one-time connection annoucement text
  StringToSocket(s, "\r\nYou've connected to TelemetryEXTRA on "); // 33 chars
  StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
  StringToSocket(s, "'s aircraft. More info at "); // 26 chars
  StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
  StringToSocket(s, "\r\n"); // 2 chars

  uint8_t si = MyIpData[s].instance;
  _show_static_data[si] = TRUE;
  _flag_print_f13[si] = TRUE;
}

void MyIpInit_TelemetryEXTRA(const BYTE s)
{
  // This gets called once for every socket we're configured to use for this module.
  uint8_t si = MyIpData[s].instance;
  taskTimer_TelemetryEXTRA[si] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_TelemetryEXTRA(const BYTE s)
{
  // don't bother queuing data if no one is listening
  if (FALSE == MyIpIsConnectedSocket(s))
    return;

  uint8_t si = MyIpData[s].instance;

  if (((TickGet() - taskTimer_TelemetryEXTRA[si]) > ((TICK_SECOND)/8)) && // 8Hz
      (IsMyIpBufferReady(s) >= (TX_BUFFER_SIZE/2)))
  {
    taskTimer_TelemetryEXTRA[si] = TickGet();
    SendTelemetryEXTRAPacket(s);
  }
}

boolean MyIpThreadSafeSendPacketCheck_TelemetryEXTRA(const uint8_t s, boolean doClearFlag)
{
  // since this data comes from, and goes to, the idle thread we
  //  don't need to deal with any thread issues
  boolean sendpacket = MyIpData[s].sendPacket;
  if (doClearFlag)
  {
    MyIpData[s].sendPacket = FALSE;
  }
  return sendpacket;
}


int MyIpThreadSafeReadBufferHead_TelemetryEXTRA(const uint8_t s)
{
  // since this data comes from, and goes to, the idle thread we
  //  don't need to deal with any thread issues
  return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_TelemetryEXTRA(const uint8_t s)
{
  uint8_t rxData;
  boolean successfulRead;

  do
  {
    if (eTCP == MyIpData[s].type)
    {
      successfulRead = TCPGet(MyIpData[s].socket, &rxData);
    }
    else //if (eUDP == MyIpData[s].type)
    {
      successfulRead = UDPGet(&rxData);
    }

    if (successfulRead)
    {
      // No Rx data parsing implemented
    }
  } while (successfulRead);

}


#if (RECORD_FREE_STACK_SPACE == 1)
extern uint16_t _maxstack ;
#endif

void SetTelemetryEXTRA_PrintOrigin()
{
  uint8_t si;
  for (si=0; si<MAX_NUM_INSTANCES_OF_MODULES; si++)
  {
    _flag_print_f13[si] = TRUE;
  }
}

void SendTelemetryEXTRAPacket(uint8_t s)
{
  // SERIAL_UDB_EXTRA expected to be used with the OpenLog which can take greater transfer speeds than Xbee
  // F2: SERIAL_UDB_EXTRA format is printed out every other time, although it is being called at 8Hz, this
  //		version will output four F2 lines every second (4Hz updates)
  int16_t i;
  uint8_t si = MyIpData[s].instance;

  if (_show_static_data[si])
  {
    _show_static_data[si] = FALSE;

    StringToSocket(s, "\r\nF14");
    StringToSocket(s, ":WIND_EST=");      uitoaSocket(s, WIND_ESTIMATION);
    StringToSocket(s, ":GPS_TYPE=");      uitoaSocket(s, GPS_TYPE);
    StringToSocket(s, ":DR=");            uitoaSocket(s, DEADRECKONING);
//    StringToSocket(s, ":BOARD_TYPE=");    uitoaSocket(s, BOARD_TYPE);
    StringToSocket(s, ":BOARD_TYPE=");    uitoaSocket(s, 0);
    StringToSocket(s, ":AIRFRAME=");      uitoaSocket(s, AIRFRAME_TYPE);
    StringToSocket(s, ":RCON=0x");        ToHexToSocket(s, get_reset_flags(), 16);
    StringToSocket(s, ":TRAP_FLAGS=0x");  ToHexToSocket(s, trap_flags, 16);
    StringToSocket(s, ":TRAP_SOURCE=0x"); ToHexToSocket(s, trap_source, 32);
    StringToSocket(s, ":ALARMS=");        uitoaSocket(s, osc_fail_count);
    StringToSocket(s, ":CLOCK=");         uitoaSocket(s, CLOCK_CONFIG);
    StringToSocket(s, ":FP=");            uitoaSocket(s, FLIGHT_PLAN_TYPE);
    StringToSocket(s, ":\r\n");

    StringToSocket(s, "F4");
    StringToSocket(s, ":R_STAB_A=");  itoaSocket(s, ROLL_STABILIZATION_AILERONS);
    StringToSocket(s, ":R_STAB_RD="); itoaSocket(s, ROLL_STABILIZATION_RUDDER);
    StringToSocket(s, ":P_STAB=");    itoaSocket(s, PITCH_STABILIZATION );
    StringToSocket(s, ":Y_STAB_R=");  itoaSocket(s, YAW_STABILIZATION_RUDDER);
    StringToSocket(s, ":Y_STAB_A=");  itoaSocket(s, YAW_STABILIZATION_AILERON);
    StringToSocket(s, ":AIL_NAV=");   itoaSocket(s, AILERON_NAVIGATION);
    StringToSocket(s, ":RUD_NAV=");   itoaSocket(s, RUDDER_NAVIGATION);
    StringToSocket(s, "i:AH_STAB=");  itoaSocket(s, ALTITUDEHOLD_STABILIZED);
    StringToSocket(s, ":AH_WP=");     itoaSocket(s, ALTITUDEHOLD_WAYPOINT);
    StringToSocket(s, ":RACE=");      itoaSocket(s, RACING_MODE);
    StringToSocket(s, ":\r\n");

    StringToSocket(s, "F5");
    StringToSocket(s, ":YAWKP_A=");   ftoaSocket(s, YAWKP_AILERON, 3);
    StringToSocket(s, ":YAWKD_A=");   ftoaSocket(s, YAWKD_AILERON, 3);
    StringToSocket(s, ":ROLLKP=");    ftoaSocket(s, ROLLKP , 3);
    StringToSocket(s, ":ROLLKD=");    ftoaSocket(s, ROLLKD, 3);
    StringToSocket(s, ":A_BOOST=");   ftoaSocket(s, AILERON_BOOST, 1);
    StringToSocket(s, ":\r\n");

    StringToSocket(s, "F6");
    StringToSocket(s, ":P_GAIN=");    ftoaSocket(s, PITCHGAIN, 3);
    StringToSocket(s, ":P_KD=");      ftoaSocket(s, PITCHKD, 3);
    StringToSocket(s, ":RUD_E_MIX="); ftoaSocket(s, RUDDER_ELEV_MIX, 3);
    StringToSocket(s, ":ROL_E_MIX="); ftoaSocket(s, ROLL_ELEV_MIX, 3);
    StringToSocket(s, ":E_BOOST=");   ftoaSocket(s, ELEVATOR_BOOST, 1);
    StringToSocket(s, ":\r\n");

    StringToSocket(s, "F7:");
    StringToSocket(s, ":Y_KP_R=");    ftoaSocket(s, YAWKP_RUDDER, 4);
    StringToSocket(s, ":Y_KD_R=");    ftoaSocket(s, YAWKD_RUDDER, 3);
    StringToSocket(s, ":RLKP_RUD=");  ftoaSocket(s, ROLLKP_RUDDER, 3);
    StringToSocket(s, ":RLKD_RUD=");  ftoaSocket(s, ROLLKD_RUDDER, 3);
    StringToSocket(s, ":RUD_BOOST="); ftoaSocket(s, RUDDER_BOOST, 3);
    StringToSocket(s, ":RTL_PITCH_DN=");  ftoaSocket(s, RTL_PITCH_DOWN, 3);
    StringToSocket(s, ":\r\n");

    StringToSocket(s, "F8");
    StringToSocket(s, ":H_MAX=");     ftoaSocket(s, HEIGHT_TARGET_MAX, 1);
    StringToSocket(s, ":H_MIN=");     ftoaSocket(s, HEIGHT_TARGET_MIN, 1);
    StringToSocket(s, ":MIN_THR=");   ftoaSocket(s, ALT_HOLD_THROTTLE_MIN, 2);
    StringToSocket(s, ":MAX_THR=");   ftoaSocket(s, ALT_HOLD_THROTTLE_MAX, 2);
    StringToSocket(s, ":PITCH_MIN_THR="); ftoaSocket(s, ALT_HOLD_PITCH_MIN, 1);
    StringToSocket(s, ":PITCH_MAX_THR="); ftoaSocket(s, ALT_HOLD_PITCH_MAX, 1);
    StringToSocket(s, ":PITCH_ZERO_THR="); ftoaSocket(s, ALT_HOLD_PITCH_HIGH, 1);
    StringToSocket(s, ":\r\n");
  }
  else if (_flag_print_f13[si])
  {
    _flag_print_f13[si] = FALSE;

    StringToSocket(s, "F13");
    StringToSocket(s, ":week"); itoaSocket(s, week_no.BB);
    StringToSocket(s, ":origN"); ltoaSocket(s, lat_origin.WW);
    StringToSocket(s, ":origE"); ltoaSocket(s, lon_origin.WW);
    StringToSocket(s, ":origA"); ltoaSocket(s, alt_origin.WW);
    StringToSocket(s, ":\r\n");
  }
  else
  {
    // F2 below means "Format Revision 2: and is used by a Telemetry parser to invoke the right pattern matching
    // F2 is a compromise between easy reading of raw data in a file and not droppping chars in transmission.
    StringToSocket(s, "F2");
    StringToSocket(s, ":T"); ltoaSocket(s, tow.WW);
    StringToSocket(s, ":S"); uitoaSocket(s, udb_flags._.radio_on); uitoaSocket(s, dcm_flags._.nav_capable); uitoaSocket(s, flags._.GPS_steering);
    StringToSocket(s, ":N"); ltoaSocket(s, lat_gps.WW);
    StringToSocket(s, ":E"); ltoaSocket(s, lon_gps.WW);
    StringToSocket(s, ":A"); ltoaSocket(s, alt_sl_gps.WW );
    StringToSocket(s, ":W"); itoaSocket(s, waypointIndex);
    StringToSocket(s, ":a"); itoaSocket(s, rmat[0]);
    StringToSocket(s, ":b"); itoaSocket(s, rmat[1]);
    StringToSocket(s, ":c"); itoaSocket(s, rmat[2]);
    StringToSocket(s, ":d"); itoaSocket(s, rmat[3]);
    StringToSocket(s, ":e"); itoaSocket(s, rmat[4]);
    StringToSocket(s, ":f"); itoaSocket(s, rmat[5]);
    StringToSocket(s, ":g"); itoaSocket(s, rmat[6]);
    StringToSocket(s, ":h"); itoaSocket(s, rmat[7]);
    StringToSocket(s, ":i"); itoaSocket(s, rmat[8]);
    StringToSocket(s, ":c"); uitoaSocket(s, (uint16_t)cog_gps.BB);
    StringToSocket(s, ":s"); itoaSocket(s, sog_gps.BB);
    StringToSocket(s, ":cpu"); uitoaSocket(s, (uint16_t)udb_cpu_load());
    StringToSocket(s, ":bmv"); itoaSocket(s, voltage_milis.BB);
    StringToSocket(s, ":as"); uitoaSocket(s, air_speed_3DIMU);
    StringToSocket(s, ":wvx"); itoaSocket(s, estimatedWind[0]);
    StringToSocket(s, ":wvy"); itoaSocket(s, estimatedWind[1]);
    StringToSocket(s, ":wvz"); itoaSocket(s, estimatedWind[2]);
#if (MAG_YAW_DRIFT == 1)
    StringToSocket(s, ":ma"); itoaSocket(s, magFieldEarth[0]);
    StringToSocket(s, ":mb"); itoaSocket(s, magFieldEarth[1]);
    StringToSocket(s, ":mc"); itoaSocket(s, magFieldEarth[2]);
#else
    StringToSocket(s, ":ma0:mb0:mc0");
#endif
    StringToSocket(s, ":svs"); itoaSocket(s, svs);
    StringToSocket(s, ":hd"); itoaSocket(s, hdop);

    // Approximate time passing between each telemetry line, even though
    // we may not have new GPS time data each time through.
    if (tow.WW > 0)
      tow.WW += 250;

    for (i= 1; i <= NUM_INPUTS; i++)
    {
      StringToSocket(s, ":p"); itoaSocket(s, i);
      StringToSocket(s, "i"); itoaSocket(s, udb_pwIn[i]);
    }
    for (i= 1; i <= NUM_OUTPUTS; i++)
    {
      StringToSocket(s, ":p"); itoaSocket(s, i);
      StringToSocket(s, "o"); itoaSocket(s, udb_pwOut[i]);
    }

    StringToSocket(s, ":imx"); itoaSocket(s, IMUlocationx._.W1);
    StringToSocket(s, ":imy"); itoaSocket(s, IMUlocationy._.W1);
    StringToSocket(s, ":imz"); itoaSocket(s, IMUlocationz._.W1);
    StringToSocket(s, ":lex"); itoaSocket(s, locationErrorEarth[0]);
    StringToSocket(s, ":ley"); itoaSocket(s, locationErrorEarth[1]);
    StringToSocket(s, ":lez"); itoaSocket(s, locationErrorEarth[2]);
    StringToSocket(s, ":fgs"); ToHexToSocket(s, flags.WW, 16);
    StringToSocket(s, ":ofc"); itoaSocket(s, osc_fail_count);
    StringToSocket(s, ":tx"); itoaSocket(s, IMUvelocityx._.W1);
    StringToSocket(s, ":ty"); itoaSocket(s, IMUvelocityy._.W1);
    StringToSocket(s, ":tz"); itoaSocket(s, IMUvelocityz._.W1);
    StringToSocket(s, ":G"); itoaSocket(s, goal.x);
    StringToSocket(s, ","); itoaSocket(s, goal.y);
    StringToSocket(s, ","); itoaSocket(s, goal.height);

#if (RECORD_FREE_STACK_SPACE == 1)
    StringToSocket(s, ":stk"); itoaSocket(s, (int16_t)(4096-maxstack));
#endif

#if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
    StringToSocket(s, ":pitot"); itoaSocket(s, airspeedPitot.value);
#endif

    StringToSocket(s, ","); itoaSocket(s, goal.height);

#if (CATAPULT_LAUNCH_INPUT_CHANNEL != CHANNEL_UNUSED)
    StringToSocket(s, "\r\n");
    StringToSocket(s, ":disable_throttle");  uitoaSocket(s, flags._.disable_throttle);
    StringToSocket(s, ":launch_detected");  uitoaSocket(s, dcm_flags._.launch_detected);
    StringToSocket(s, ":launching");  uitoaSocket(s, flags._.launching);
    StringToSocket(s, ":launch_throttle_delay_timeout"); uitoaSocket(s, launch_throttle_delay_timeout);
    StringToSocket(s, ":launch_trim_timeout"); uitoaSocket(s, launch_trim_timeout);
    StringToSocket(s, ":launch_success_check_timeout"); uitoaSocket(s, launch_success_check_timeout);
#endif

#if (1)
    StringToSocket(s, "\r\n:AircraftState");
    printAircraftState(s);
#endif
    
    StringToSocket(s, ":\r\n");
  }
  MyIpData[s].sendPacket = TRUE; // send right away
}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPTELEMETRYEXTRA_C_

