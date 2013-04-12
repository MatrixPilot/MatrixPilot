#ifndef _MYIPTELEMETRYEXTRA_C_
#define _MYIPTELEMETRYEXTRA_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_TELEMETRY_EXTRA == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpTelemetryEXTRA.h"
#include "MyIpHelpers.h"
#include "../libUDB/libUDB_internal.h"
//#include "flightplan-logo.h"
//#include "gpsParseCommon.h"

//////////////////////////
// Module Variables
void SendTelemetryEXTRAPacket(uint8_t s);
uint32_t taskTimer_TelemetryEXTRA[MAX_NUM_INSTANCES_OF_MODULES];
uint16_t _udb_heartbeat_counter[MAX_NUM_INSTANCES_OF_MODULES];

void MyIpOnConnect_TelemetryEXTRA(const BYTE s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to TelemetryEXTRA on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away    
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

    if ((TickGet() - taskTimer_TelemetryEXTRA[si]) > ((TICK_SECOND)/8)) // 8Hz
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


int16_t _pwIn_save[NUM_INPUTS + 1] ;
int16_t _pwOut_save[NUM_OUTPUTS + 1] ;
int16_t _telemetry_counter = 8 ;
#if (RECORD_FREE_STACK_SPACE == 1)
extern uint16_t _maxstack ;
#endif

void SendTelemetryEXTRAPacket(uint8_t s)
{
    // SERIAL_UDB_EXTRA expected to be used with the OpenLog which can take greater transfer speeds than Xbee
    // F2: SERIAL_UDB_EXTRA format is printed out every other time, although it is being called at 8Hz, this
    //		version will output four F2 lines every second (4Hz updates)
    uint8_t si = MyIpData[s].instance;

    switch (_telemetry_counter)
    {
    // The first lines of telemetry contain info about the compile-time settings from the options.h file
    case 8:
        StringToSocket(s, "\r\nF14:WIND_EST="); uitoaSocket(s, WIND_ESTIMATION);
        StringToSocket(s, ":GPS_TYPE=");        uitoaSocket(s, GPS_TYPE);
        StringToSocket(s, ":DR=");              uitoaSocket(s, DEADRECKONING);
        StringToSocket(s, ":BOARD_TYPE=");      uitoaSocket(s, BOARD_TYPE);
        StringToSocket(s, ":AIRFRAME=");        uitoaSocket(s, AIRFRAME_TYPE);
        StringToSocket(s, ":RCON=0x");          ToHexToSrc(s, udb_get_reset_flags(), 16);
        StringToSocket(s, ":TRAP_FLAGS=0x");    ToHexToSrc(s, trap_flags, 16);
        StringToSocket(s, ":TRAP_SOURCE=0x");   ToHexToSrc(s, trap_source, 32);
        StringToSocket(s, ":ALARMS=");          uitoaSocket(s, osc_fail_count);
        StringToSocket(s, ":CLOCK=");           uitoaSocket(s, CLOCK_CONFIG);
        StringToSocket(s, ":FP=");              uitoaSocket(s, FLIGHT_PLAN_TYPE);
        StringToSocket(s, "\r\n");

        //serial_output("\r\nF14:WIND_EST=%i:GPS_TYPE=%i:DR=%i:BOARD_TYPE=%i:AIRFRAME=%i:RCON=0x%X:TRAP_FLAGS=0x%X:TRAP_SOURCE=0x%lX:ALARMS=%i:"  
        //"CLOCK=%i:FP=%d:\r\n",
        //WIND_ESTIMATION, GPS_TYPE, DEADRECKONING, BOARD_TYPE, AIRFRAME_TYPE, udb_get_reset_flags() , trap_flags , trap_source , osc_fail_count, CLOCK_CONFIG, FLIGHT_PLAN_TYPE ) ;
        break ;

    case 7:
        StringToSocket(s, "F15:IDA="); StringToSocket(s, ID_VEHICLE_MODEL_NAME);
        StringToSocket(s, ":IDB=");    StringToSocket(s, ID_VEHICLE_REGISTRATION);
        StringToSocket(s, "\r\n");

        //serial_output("F15:IDA=");
        //serial_output(ID_VEHICLE_MODEL_NAME );
        //serial_output(":IDB=");
        //serial_output(ID_VEHICLE_REGISTRATION );
        //serial_output(":\r\n" );
        break ;

    case 6:
        StringToSocket(s, "F16:IDC="); StringToSocket(s, ID_LEAD_PILOT);
        StringToSocket(s, ":IDD=");    StringToSocket(s, ID_DIY_DRONES_URL);
        StringToSocket(s, "\r\n");

        //serial_output("F16:IDC=" );
        //serial_output( ID_LEAD_PILOT );
        //serial_output( ":IDD=");
        //serial_output( ID_DIY_DRONES_URL );
        //serial_output(":\r\n") ;
        break ;

    case 5:
        StringToSocket(s, "F4:R_STAB_A="); itoaSocket(s, ROLL_STABILIZATION_AILERONS);
        StringToSocket(s, ":R_STAB_RD="); itoaSocket(s, ROLL_STABILIZATION_RUDDER);
        StringToSocket(s, ":P_STAB="); itoaSocket(s,PITCH_STABILIZATION );
        StringToSocket(s, ":Y_STAB_R="); itoaSocket(s, YAW_STABILIZATION_RUDDER);
        StringToSocket(s, ":Y_STAB_A="); itoaSocket(s, YAW_STABILIZATION_AILERON);
        StringToSocket(s, ":AIL_NAV="); itoaSocket(s, AILERON_NAVIGATION);
        StringToSocket(s, ":RUD_NAV="); itoaSocket(s, RUDDER_NAVIGATION);
        StringToSocket(s, "i:AH_STAB="); itoaSocket(s, ALTITUDEHOLD_STABILIZED);
        StringToSocket(s, ":AH_WP="); itoaSocket(s, ALTITUDEHOLD_WAYPOINT);
        StringToSocket(s, ":RACE="); itoaSocket(s, RACING_MODE);
        StringToSocket(s, "\r\n");

        //serial_output("F4:R_STAB_A=%i:R_STAB_RD=%i:P_STAB=%i:Y_STAB_R=%i:Y_STAB_A=%i:AIL_NAV=%i:RUD_NAV=%i:AH_STAB=%i:AH_WP=%i:RACE=%i:\r\n",
        //ROLL_STABILIZATION_AILERONS, ROLL_STABILIZATION_RUDDER, PITCH_STABILIZATION, YAW_STABILIZATION_RUDDER, YAW_STABILIZATION_AILERON,
        //AILERON_NAVIGATION, RUDDER_NAVIGATION, ALTITUDEHOLD_STABILIZED, ALTITUDEHOLD_WAYPOINT, RACING_MODE) ;
        break ;

    case 4:
        StringToSocket(s, "F5:YAWKP_A="); ftoaSocket(s, YAWKP_AILERON, 3);
        StringToSocket(s, ":YAWKD_A="); ftoaSocket(s, YAWKD_AILERON, 3);
        StringToSocket(s, ":ROLLKP="); ftoaSocket(s,ROLLKP , 3);
        StringToSocket(s, ":ROLLKD="); ftoaSocket(s, ROLLKD, 3);
        StringToSocket(s, ":A_BOOST="); ftoaSocket(s, AILERON_BOOST, 1);
        StringToSocket(s, "\r\n");

        //serial_output("F5:YAWKP_A=%5.3f:YAWKD_A=%5.3f:ROLLKP=%5.3f:ROLLKD=%5.3f:A_BOOST=%3.1f:\r\n",
        //YAWKP_AILERON, YAWKD_AILERON, ROLLKP, ROLLKD, AILERON_BOOST ) ;
        break ;
    
    case 3:
        StringToSocket(s, "F6:P_GAIN="); ftoaSocket(s, PITCHGAIN, 3);
        StringToSocket(s, ":P_KD="); ftoaSocket(s, PITCHKD, 3);
        StringToSocket(s, ":RUD_E_MIX="); ftoaSocket(s, RUDDER_ELEV_MIX, 3);
        StringToSocket(s, ":ROL_E_MIX="); ftoaSocket(s, ROLL_ELEV_MIX, 3);
        StringToSocket(s, ":E_BOOST="); ftoaSocket(s, ELEVATOR_BOOST, 1);
        StringToSocket(s, "\r\n");

        //serial_output("F6:P_GAIN=%5.3f:P_KD=%5.3f:RUD_E_MIX=%5.3f:ROL_E_MIX=%5.3f:E_BOOST=%3.1f:\r\n",
        //PITCHGAIN, PITCHKD, RUDDER_ELEV_MIX, ROLL_ELEV_MIX, ELEVATOR_BOOST) ;
        break ;

    case 2:
        StringToSocket(s, "F7:Y_KP_R="); ftoaSocket(s, YAWKP_RUDDER, 4);
        StringToSocket(s, ":Y_KD_R="); ftoaSocket(s, YAWKD_RUDDER, 3);
        StringToSocket(s, ":RLKP_RUD="); ftoaSocket(s, ROLLKP_RUDDER, 3);
        StringToSocket(s, ":RLKD_RUD="); ftoaSocket(s, ROLLKD_RUDDER, 3);
        StringToSocket(s, ":RUD_BOOST="); ftoaSocket(s, RUDDER_BOOST, 3);
        StringToSocket(s, ":RTL_PITCH_DN="); ftoaSocket(s, RTL_PITCH_DOWN, 3);
        StringToSocket(s, "\r\n");

        //serial_output("F7:Y_KP_R=%5.4f:Y_KD_R=%5.3f:RLKP_RUD=%5.3f:RLKD_RUD=%5.3f:RUD_BOOST=%5.3f:RTL_PITCH_DN=%5.3f:\r\n",
        //YAWKP_RUDDER, YAWKD_RUDDER, ROLLKP_RUDDER , ROLLKD_RUDDER , RUDDER_BOOST, RTL_PITCH_DOWN) ;
        break ;

    case 1:
        StringToSocket(s, "F8:H_MAX="); ftoaSocket(s, HEIGHT_TARGET_MAX, 1);
        StringToSocket(s, ":H_MIN="); ftoaSocket(s, HEIGHT_TARGET_MIN, 1);
        StringToSocket(s, ":MIN_THR="); ftoaSocket(s, ALT_HOLD_THROTTLE_MIN, 2);
        StringToSocket(s, ":MAX_THR="); ftoaSocket(s, ALT_HOLD_THROTTLE_MAX, 2);
        StringToSocket(s, ":PITCH_MIN_THR="); ftoaSocket(s, ALT_HOLD_PITCH_MIN, 1);
        StringToSocket(s, ":PITCH_MAX_THR="); ftoaSocket(s, ALT_HOLD_PITCH_MAX, 1);
        StringToSocket(s, ":PITCH_ZERO_THR="); ftoaSocket(s, ALT_HOLD_PITCH_HIGH, 1);
        StringToSocket(s, "\r\n");

        //serial_output("F8:H_MAX=%6.1f:H_MIN=%6.1f:MIN_THR=%3.2f:MAX_THR=%3.2f:PITCH_MIN_THR=%4.1f:PITCH_MAX_THR=%4.1f:PITCH_ZERO_THR=%4.1f:\r\n",
        //HEIGHT_TARGET_MAX, HEIGHT_TARGET_MIN, ALT_HOLD_THROTTLE_MIN, ALT_HOLD_THROTTLE_MAX,
        //ALT_HOLD_PITCH_MIN, ALT_HOLD_PITCH_MAX, ALT_HOLD_PITCH_HIGH) ;
        break ;

    default:
        // F2 below means "Format Revision 2: and is used by a Telemetry parser to invoke the right pattern matching
        // F2 is a compromise between easy reading of raw data in a file and not droppping chars in transmission.
        if (_udb_heartbeat_counter[si] % 10 != 0)  // Every 2 runs (5 heartbeat counts per 8Hz)
        {
            StringToSocket(s, "F2:T"); itoaSocket(s, tow.WW);
            StringToSocket(s, ":S"); uitoaSocket(s, udb_flags._.radio_on); uitoaSocket(s, dcm_flags._.nav_capable); uitoaSocket(s, flags._.GPS_steering);
            StringToSocket(s, ":N"); itoaSocket(s, lat_gps.WW);
            StringToSocket(s, ":E"); itoaSocket(s, long_gps.WW);
            StringToSocket(s, ":A"); itoaSocket(s, alt_sl_gps.WW );
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

            //serial_output("F2:T%li:S%d%d%d:N%li:E%li:A%li:W%i:a%i:b%i:c%i:d%i:e%i:f%i:g%i:h%i:i%i:c%u:s%i:cpu%u:bmv%i:"
            //"as%u:wvx%i:wvy%i:wvz%i:ma%i:mb%i:mc%i:svs%i:hd%i:",
            //tow.WW, udb_flags._.radio_on, dcm_flags._.nav_capable, flags._.GPS_steering,
            //lat_gps.WW , long_gps.WW , alt_sl_gps.WW, waypointIndex,
            //rmat[0] , rmat[1] , rmat[2] ,
            //rmat[3] , rmat[4] , rmat[5] ,
            //rmat[6] , rmat[7] , rmat[8] ,
            //(uint16_t)cog_gps.BB, sog_gps.BB, (uint16_t)udb_cpu_load(), voltage_milis.BB,
            //air_speed_3DIMU,
            //estimatedWind[0], estimatedWind[1], estimatedWind[2],
        #if (MAG_YAW_DRIFT == 1)
            //magFieldEarth[0],magFieldEarth[1],magFieldEarth[2],
        #else
            //(int16_t)0, (int16_t)0, (int16_t)0,
        #endif

            //svs, hdop ) ;

            // Approximate time passing between each telemetry line, even though
            // we may not have new GPS time data each time through.
            if (tow.WW > 0)
                tow.WW += 250 ;

            // Save  pwIn and PwOut buffers for printing next time around
            int16_t i ;
            for (i=0; i <= NUM_INPUTS; i++)
                _pwIn_save[si][i] = udb_pwIn[si][i] ;
            for (i=0; i <= NUM_OUTPUTS; i++)
                _pwOut_save[si][i] = udb_pwOut[si][i] ;
        }
        else
        {
            int16_t i ;
            for (i= 1; i <= NUM_INPUTS; i++)
            {
                StringToSocket(s, ":p"); itoaSocket(s, i);
                StringToSocket(s, "i"); itoaSocket(s, _pwIn_save[si][i]);
                //serial_output("p%ii%i:",i,_pwIn_save[i]);
            }
            for (i= 1; i <= NUM_OUTPUTS; i++)
            {
                StringToSocket(s, ":p"); itoaSocket(s, i);
                StringToSocket(s, "o"); itoaSocket(s, _pwOut_save[si][i]);
                //serial_output("p%io%i:",i,_pwOut_save[i]);
            }

            StringToSocket(s, ":imx"); itoaSocket(s, IMUlocationx._.W1);
            StringToSocket(s, ":imy"); itoaSocket(s, IMUlocationy._.W1);
            StringToSocket(s, ":imz"); itoaSocket(s, IMUlocationz._.W1);
            StringToSocket(s, ":lex"); itoaSocket(s, locationErrorEarth[0]);
            StringToSocket(s, ":ley"); itoaSocket(s, locationErrorEarth[1]);
            StringToSocket(s, ":lez"); itoaSocket(s, locationErrorEarth[2]);
            StringToSocket(s, ":fgs"); ToHexToSocket(s, flags.WW,16);
            StringToSocket(s, ":ofc"); itoaSocket(s, osc_fail_count);
            StringToSocket(s, ":tx"); itoaSocket(s, IMUvelocityx._.W1);
            StringToSocket(s, ":ty"); itoaSocket(s, IMUvelocityy._.W1);
            StringToSocket(s, ":tz"); itoaSocket(s, IMUvelocityz._.W1);
            StringToSocket(s, ":G"); itoaSocket(s, goal.x);
            StringToSocket(s, ","); itoaSocket(s, goal.y);
            StringToSocket(s, ","); itoaSocket(s, goal.height);

            //serial_output("imx%i:imy%i:imz%i:lex%i:ley%i:lez%i:fgs%X:ofc%i:tx%i:ty%i:tz%i:G%d,%d,%d:",IMUlocationx._.W1 ,IMUlocationy._.W1 ,IMUlocationz._.W1,
            //locationErrorEarth[0] , locationErrorEarth[1] , locationErrorEarth[2] ,
            //flags.WW, osc_fail_count,
            //IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1, goal.x, goal.y, goal.height );

        #if (RECORD_FREE_STACK_SPACE == 1)
            StringToSocket(s, ":stk"); itoaSocket(s, (int16_t)(4096-maxstack));
            //serial_output("stk%d:", (int16_t)(4096-maxstack));
        #endif

        #if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
            StringToSocket(s, ":pitot"); itoaSocket(s, airspeedPitot.value);
            //serial_output("pitot%i:", airspeedPitot.value) ;
        #endif

            StringToSocket(s, ":\r\n");
            //serial_output("\r\n");
        }
        if (flags._.f13_print_req == 1)
        {
            // The F13 line of telemetry is printed when origin has been captured and inbetween F2 lines in SERIAL_UDB_EXTRA
            if (_udb_heartbeat_counter[si] % 10 != 0)
                return ;
            StringToSocket(s, "F13:week"); itoaSocket(s, week_no);
            StringToSocket(s, ":origN"); ltoaSocket(s, lat_origin.WW);
            StringToSocket(s, ":origE"); ltoaSocket(s, long_origin.WW);
            StringToSocket(s, ":origA"); ltoaSocket(s, alt_origin);

            //serial_output("F13:week%i:origN%li:origE%li:origA%li:\r\n", week_no, lat_origin.WW, long_origin.WW, alt_origin) ;
            flags._.f13_print_req = 0 ;
        }

        return ;
    } // switch
    _telemetry_counter[si]-- ;

}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPTELEMETRYEXTRA_C_

