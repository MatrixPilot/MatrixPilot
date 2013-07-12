#ifndef _MYIPAIRCRAFTCONFIG_C_
#define _MYIPAIRCRAFTCONFIG_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_AIRCRAFT_CONFIG == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpAircraftConfig.h"
#include "MyIpHelpers.h"
//#include "../libUDB/libUDB_internal.h"

void SendOptionsH(const uint8_t s);

//////////////////////////
// Module Variables
uint16_t sendOptionsIndex[MAX_NUM_INSTANCES_OF_MODULES];
#define SEND_OPTIONS_INDEX_START    0
#define SEND_OPTIONS_INDEX_DONE     999

void MyIpOnConnect_AircraftConfig(const uint8_t s)
{
  uint8_t i = MyIpData[s].instance;
  sendOptionsIndex[i] = SEND_OPTIONS_INDEX_START;
}

void MyIpInit_AircraftConfig(const uint8_t s)
{
  // This gets called once for every socket we're configured to use for this module.
  BYTE i = MyIpData[s].instance;
  sendOptionsIndex[i] = SEND_OPTIONS_INDEX_DONE;
}

void MyIpService_AircraftConfig(const uint8_t s)
{
  // don't bother queuing data if no one is listening
  if (FALSE == MyIpIsConnectedSocket(s))
    return;

  uint8_t i = MyIpData[s].instance;
  if ((sendOptionsIndex[i] != SEND_OPTIONS_INDEX_DONE) && (IsMyIpBufferReady(s) >= TX_BUFFER_SIZE))
  {
    // if we're not done sending (more to send) and there's space in the output buffer
    SendOptionsH(s);
  }
}

boolean MyIpThreadSafeSendPacketCheck_AircraftConfig(const uint8_t s, boolean doClearFlag)
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


int MyIpThreadSafeReadBufferHead_AircraftConfig(const uint8_t s)
{
  // since this data comes from, and goes to, the idle thread we
  // don't need to deal with any thread issues
  return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_AircraftConfig(const uint8_t s)
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
      if ('?' == rxData)
      {
        // if we ever recieve a "?" then start the optionsH dump
        uint8_t i = MyIpData[s].instance;
        sendOptionsIndex[i] = SEND_OPTIONS_INDEX_START;
      }
    }
  } while (successfulRead);
}

void SendOptionsH(const uint8_t s)
{
  uint8_t i = MyIpData[s].instance;

  switch (sendOptionsIndex[i])
  {
  case 0:
    StringToSocket(s, ",BOARD_TYPE:"); itoaSocket(s, BOARD_TYPE);
    StringToSocket(s, ",BOARD_ORIENTATION:"); itoaSocket(s, BOARD_ORIENTATION);
    StringToSocket(s, ",AIRFRAME_TYPE:"); itoaSocket(s, AIRFRAME_TYPE);
    StringToSocket(s, ",GPS_TYPE:"); itoaSocket(s, GPS_TYPE);
    StringToSocket(s, ",ROLL_STABILIZATION_AILERONS:"); itoaSocket(s, ROLL_STABILIZATION_AILERONS);
    StringToSocket(s, ",ROLL_STABILIZATION_RUDDER:"); itoaSocket(s, ROLL_STABILIZATION_RUDDER);
    StringToSocket(s, ",PITCH_STABILIZATION:"); itoaSocket(s, PITCH_STABILIZATION);
    StringToSocket(s, ",YAW_STABILIZATION_RUDDER:"); itoaSocket(s, YAW_STABILIZATION_RUDDER);
    StringToSocket(s, ",YAW_STABILIZATION_AILERON:"); itoaSocket(s, YAW_STABILIZATION_AILERON);
    StringToSocket(s, ",AILERON_NAVIGATION:"); itoaSocket(s, AILERON_NAVIGATION);
    StringToSocket(s, ",RUDDER_NAVIGATION:"); itoaSocket(s,RUDDER_NAVIGATION);
    StringToSocket(s, ",CROSS_TRACK_MARGIN:"); itoaSocket(s, CROSS_TRACK_MARGIN);
    StringToSocket(s, ",WIND_GAIN_ADJUSTMENT:"); itoaSocket(s, WIND_GAIN_ADJUSTMENT);
    StringToSocket(s, ",ALTITUDEHOLD_STABILIZED:"); itoaSocket(s, ALTITUDEHOLD_STABILIZED);
    StringToSocket(s, ",ALTITUDEHOLD_WAYPOINT:"); itoaSocket(s, ALTITUDEHOLD_WAYPOINT);
    StringToSocket(s, ",SPEED_CONTROL:"); itoaSocket(s, SPEED_CONTROL);
    StringToSocket(s, ",DESIRED_SPEED:"); itoaSocket(s, DESIRED_SPEED);
    StringToSocket(s, ",INVERTED_FLIGHT_STABILIZED_MODE:"); itoaSocket(s, INVERTED_FLIGHT_STABILIZED_MODE);
    StringToSocket(s, ",INVERTED_FLIGHT_WAYPOINT_MODE:"); itoaSocket(s, INVERTED_FLIGHT_WAYPOINT_MODE);
    StringToSocket(s, ",HOVERING_STABILIZED_MODE:"); itoaSocket(s, HOVERING_STABILIZED_MODE);
    StringToSocket(s, ",HOVERING_WAYPOINT_MODE:"); itoaSocket(s, HOVERING_WAYPOINT_MODE);
    StringToSocket(s, ",USE_CAMERA_STABILIZATION:"); itoaSocket(s, USE_CAMERA_STABILIZATION);
    StringToSocket(s, ",MAG_YAW_DRIFT:"); itoaSocket(s, MAG_YAW_DRIFT);
    StringToSocket(s, ",RACING_MODE:"); itoaSocket(s, RACING_MODE);
    StringToSocket(s, ",RACING_MODE_WP_THROTTLE:"); ftoaSocket(s,RACING_MODE_WP_THROTTLE,3);
    StringToSocket(s, ",NORADIO:"); itoaSocket(s, NORADIO);
    break;

  case 1:
    StringToSocket(s, ",USE_PPM_INPUT:"); itoaSocket(s, USE_PPM_INPUT);
    StringToSocket(s, ",PPM_NUMBER_OF_CHANNELS:"); itoaSocket(s, PPM_NUMBER_OF_CHANNELS);
    StringToSocket(s, ",PPM_SIGNAL_INVERTED:"); itoaSocket(s,PPM_SIGNAL_INVERTED );
    StringToSocket(s, ",PPM_ALT_OUTPUT_PINS:"); itoaSocket(s, PPM_ALT_OUTPUT_PINS);
    StringToSocket(s, ",NUM_INPUTS:"); itoaSocket(s, NUM_INPUTS);
    StringToSocket(s, ",THROTTLE_INPUT_CHANNEL:"); itoaSocket(s, THROTTLE_INPUT_CHANNEL);
    StringToSocket(s, ",AILERON_INPUT_CHANNEL:"); itoaSocket(s, AILERON_INPUT_CHANNEL);
    StringToSocket(s, ",ELEVATOR_INPUT_CHANNEL:"); itoaSocket(s, ELEVATOR_INPUT_CHANNEL);
    StringToSocket(s, ",RUDDER_INPUT_CHANNEL:"); itoaSocket(s, RUDDER_INPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_A_INPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_A_INPUT_CHANNEL);
    StringToSocket(s, ",MODE_SWITCH_INPUT_CHANNEL:"); itoaSocket(s, MODE_SWITCH_INPUT_CHANNEL);
    StringToSocket(s, ",CAMERA_PITCH_INPUT_CHANNEL:"); itoaSocket(s, CAMERA_PITCH_INPUT_CHANNEL);
    StringToSocket(s, ",CAMERA_YAW_INPUT_CHANNEL:"); itoaSocket(s, CAMERA_YAW_INPUT_CHANNEL);
    StringToSocket(s, ",CAMERA_MODE_INPUT_CHANNEL:"); itoaSocket(s, CAMERA_MODE_INPUT_CHANNEL);
    StringToSocket(s, ",OSD_MODE_SWITCH_INPUT_CHANNEL:"); itoaSocket(s, OSD_MODE_SWITCH_INPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_B_INPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_B_INPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_C_INPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_C_INPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_D_INPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_D_INPUT_CHANNEL);
    StringToSocket(s, ",NUM_OUTPUTS:"); itoaSocket(s, NUM_OUTPUTS);
    StringToSocket(s, ",THROTTLE_OUTPUT_CHANNEL:"); itoaSocket(s, THROTTLE_OUTPUT_CHANNEL);
    StringToSocket(s, ",AILERON_OUTPUT_CHANNEL:"); itoaSocket(s, AILERON_OUTPUT_CHANNEL);
    StringToSocket(s, ",ELEVATOR_OUTPUT_CHANNEL:"); itoaSocket(s, ELEVATOR_OUTPUT_CHANNEL);
    StringToSocket(s, ",RUDDER_OUTPUT_CHANNEL:"); itoaSocket(s, RUDDER_OUTPUT_CHANNEL);
    StringToSocket(s, ",AILERON_SECONDARY_OUTPUT_CHANNEL:"); itoaSocket(s, AILERON_SECONDARY_OUTPUT_CHANNEL);
    StringToSocket(s, ",CAMERA_PITCH_OUTPUT_CHANNEL:"); itoaSocket(s, CAMERA_PITCH_OUTPUT_CHANNEL);
    StringToSocket(s, ",CAMERA_YAW_OUTPUT_CHANNEL:"); itoaSocket(s, CAMERA_YAW_OUTPUT_CHANNEL);
    StringToSocket(s, ",TRIGGER_OUTPUT_CHANNEL:"); itoaSocket(s, TRIGGER_OUTPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_A_OUTPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_A_OUTPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_B_OUTPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_B_OUTPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_C_OUTPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_C_OUTPUT_CHANNEL);
    StringToSocket(s, ",PASSTHROUGH_D_OUTPUT_CHANNEL:"); itoaSocket(s, PASSTHROUGH_D_OUTPUT_CHANNEL);
    break;

  case 2:
    StringToSocket(s, ",AILERON_CHANNEL_REVERSED:"); itoaSocket(s, AILERON_CHANNEL_REVERSED);
    StringToSocket(s, ",ELEVATOR_CHANNEL_REVERSED:"); itoaSocket(s, ELEVATOR_CHANNEL_REVERSED);
    StringToSocket(s, ",RUDDER_CHANNEL_REVERSED:"); itoaSocket(s, RUDDER_CHANNEL_REVERSED);
    StringToSocket(s, ",AILERON_SECONDARY_CHANNEL_REVERSED:"); itoaSocket(s, AILERON_SECONDARY_CHANNEL_REVERSED);
    StringToSocket(s, ",THROTTLE_CHANNEL_REVERSED:"); itoaSocket(s, THROTTLE_CHANNEL_REVERSED);
    StringToSocket(s, ",CAMERA_PITCH_CHANNEL_REVERSED:"); itoaSocket(s, CAMERA_PITCH_CHANNEL_REVERSED);
    StringToSocket(s, ",CAMERA_YAW_CHANNEL_REVERSED:"); itoaSocket(s, CAMERA_YAW_CHANNEL_REVERSED);
    StringToSocket(s, ",ELEVON_VTAIL_SURFACES_REVERSED:"); itoaSocket(s, ELEVON_VTAIL_SURFACES_REVERSED);
    StringToSocket(s, ",MODE_SWITCH_THRESHOLD_LOW:"); itoaSocket(s, MODE_SWITCH_THRESHOLD_LOW);
    StringToSocket(s, ",MODE_SWITCH_THRESHOLD_HIGH:"); itoaSocket(s, MODE_SWITCH_THRESHOLD_HIGH);
    StringToSocket(s, ",MODE_SWITCH_TWO_POSITION:"); itoaSocket(s, MODE_SWITCH_TWO_POSITION);
    StringToSocket(s, ",FAILSAFE_INPUT_CHANNEL:"); itoaSocket(s, FAILSAFE_INPUT_CHANNEL);
    StringToSocket(s, ",FAILSAFE_INPUT_MIN:"); itoaSocket(s, FAILSAFE_INPUT_MIN);
    StringToSocket(s, ",FAILSAFE_INPUT_MAX:"); itoaSocket(s, FAILSAFE_INPUT_MAX);
    StringToSocket(s, ",FAILSAFE_TYPE:"); itoaSocket(s, FAILSAFE_TYPE);
    StringToSocket(s, ",FAILSAFE_HOLD:"); itoaSocket(s, FAILSAFE_HOLD);
    StringToSocket(s, ",SERIAL_OUTPUT_FORMAT:"); itoaSocket(s, SERIAL_OUTPUT_FORMAT);
    StringToSocket(s, ",MAVLINK_SYSID:"); itoaSocket(s, MAVLINK_SYSID);
    StringToSocket(s, ",USE_OSD:"); itoaSocket(s, USE_OSD);
    StringToSocket(s, ",NUM_ANALOG_INPUTS:"); itoaSocket(s, NUM_ANALOG_INPUTS);
    StringToSocket(s, ",ANALOG_CURRENT_INPUT_CHANNEL:"); itoaSocket(s, ANALOG_CURRENT_INPUT_CHANNEL);
    StringToSocket(s, ",ANALOG_VOLTAGE_INPUT_CHANNEL:"); itoaSocket(s, ANALOG_VOLTAGE_INPUT_CHANNEL);
    StringToSocket(s, ",ANALOG_RSSI_INPUT_CHANNEL:"); itoaSocket(s, ANALOG_RSSI_INPUT_CHANNEL);
    StringToSocket(s, ",ANALOG_AIRSPEED_INPUT_CHANNEL:"); itoaSocket(s, ANALOG_AIRSPEED_INPUT_CHANNEL);
    StringToSocket(s, ",RSSI_MIN_SIGNAL_VOLTAGE:"); ftoaSocket(s,RSSI_MIN_SIGNAL_VOLTAGE,3);
    StringToSocket(s, ",RSSI_MAX_SIGNAL_VOLTAGE:"); ftoaSocket(s,RSSI_MAX_SIGNAL_VOLTAGE,3);
    StringToSocket(s, ",TRIGGER_TYPE:"); itoaSocket(s, TRIGGER_TYPE);
    StringToSocket(s, ",TRIGGER_ACTION:"); itoaSocket(s, TRIGGER_ACTION);
    StringToSocket(s, ",TRIGGER_SERVO_LOW:"); itoaSocket(s, TRIGGER_SERVO_LOW);
    StringToSocket(s, ",TRIGGER_SERVO_HIGH:"); itoaSocket(s, TRIGGER_SERVO_HIGH);
    StringToSocket(s, ",TRIGGER_PULSE_DURATION:"); itoaSocket(s, TRIGGER_PULSE_DURATION);
    StringToSocket(s, ",TRIGGER_REPEAT_PERIOD:"); itoaSocket(s, TRIGGER_REPEAT_PERIOD);
    StringToSocket(s, ",SERVOSAT:"); ftoaSocket(s,SERVOSAT,3);
    break;

  case 3:
    StringToSocket(s, ",ROLLKP:"); ftoaSocket(s,ROLLKP,3);
    StringToSocket(s, ",ROLLKD:"); ftoaSocket(s,ROLLKD,3);
    StringToSocket(s, ",YAWKP_AILERON:"); ftoaSocket(s,YAWKP_AILERON,3);
    StringToSocket(s, ",YAWKD_AILERON:"); ftoaSocket(s,YAWKD_AILERON,3);
    StringToSocket(s, ",AILERON_BOOST:"); ftoaSocket(s,AILERON_BOOST,3);
    StringToSocket(s, ",PITCHGAIN:"); ftoaSocket(s,PITCHGAIN,3);
    StringToSocket(s, ",PITCHKD:"); ftoaSocket(s,PITCHKD,3);
    StringToSocket(s, ",RUDDER_ELEV_MIX:"); ftoaSocket(s,RUDDER_ELEV_MIX,3);
    StringToSocket(s, ",ROLL_ELEV_MIX:"); ftoaSocket(s,ROLL_ELEV_MIX,3);
    StringToSocket(s, ",ELEVATOR_BOOST:"); ftoaSocket(s,ELEVATOR_BOOST,3);
    StringToSocket(s, ",INVERTED_NEUTRAL_PITCH:"); ftoaSocket(s,INVERTED_NEUTRAL_PITCH,3);
    StringToSocket(s, ",YAWKP_RUDDER:"); ftoaSocket(s,YAWKP_RUDDER,3);
    StringToSocket(s, ",YAWKD_RUDDER:"); ftoaSocket(s,YAWKD_RUDDER,3);
    StringToSocket(s, ",ROLLKP_RUDDER:"); ftoaSocket(s,ROLLKP_RUDDER,3);
    StringToSocket(s, ",ROLLKD_RUDDER:"); ftoaSocket(s,ROLLKD_RUDDER,3);
    StringToSocket(s, ",MANUAL_AILERON_RUDDER_MIX:"); ftoaSocket(s,MANUAL_AILERON_RUDDER_MIX,3);
    StringToSocket(s, ",RUDDER_BOOST:"); ftoaSocket(s,RUDDER_BOOST,3);
    StringToSocket(s, ",HOVER_ROLLKP:"); ftoaSocket(s,HOVER_ROLLKP,3);
    StringToSocket(s, ",HOVER_ROLLKD:"); ftoaSocket(s,HOVER_ROLLKD,3);
    StringToSocket(s, ",HOVER_PITCHGAIN:"); ftoaSocket(s,HOVER_PITCHGAIN,3);
    StringToSocket(s, ",HOVER_PITCHKD:"); ftoaSocket(s,HOVER_PITCHKD,3);
    StringToSocket(s, ",HOVER_PITCH_OFFSET:"); ftoaSocket(s,HOVER_PITCH_OFFSET,3);
    StringToSocket(s, ",HOVER_YAWKP:"); ftoaSocket(s,HOVER_YAWKP,3);
    StringToSocket(s, ",HOVER_YAWKD:"); ftoaSocket(s,HOVER_YAWKD,3);
    StringToSocket(s, ",HOVER_YAW_OFFSET:"); ftoaSocket(s,HOVER_YAW_OFFSET,3);
    StringToSocket(s, ",HOVER_PITCH_TOWARDS_WP:"); ftoaSocket(s,HOVER_PITCH_TOWARDS_WP,3);
    StringToSocket(s, ",HOVER_NAV_MAX_PITCH_RADIUS:"); ftoaSocket(s,HOVER_NAV_MAX_PITCH_RADIUS,3);
    break;

  case 4:
    StringToSocket(s, ",CAMERA_MODE_THRESHOLD_LOW:"); itoaSocket(s, CAMERA_MODE_THRESHOLD_LOW);
    StringToSocket(s, ",CAMERA_MODE_THRESHOLD_HIGH:"); itoaSocket(s, CAMERA_MODE_THRESHOLD_HIGH);
    StringToSocket(s, ",SERVOSAT_CAM:"); ftoaSocket(s,SERVOSAT_CAM,3);
    StringToSocket(s, ",CAM_TAN_PITCH_IN_STABILIZED_MODE:"); itoaSocket(s, CAM_TAN_PITCH_IN_STABILIZED_MODE);
    StringToSocket(s, ",CAM_YAW_IN_STABILIZED_MODE:"); itoaSocket(s, CAM_YAW_IN_STABILIZED_MODE);
    StringToSocket(s, ",CAM_PITCH_SERVO_THROW:"); itoaSocket(s, CAM_PITCH_SERVO_THROW);
    StringToSocket(s, ",CAM_PITCH_SERVO_MAX:"); itoaSocket(s, CAM_PITCH_SERVO_MAX);
    StringToSocket(s, ",CAM_PITCH_SERVO_MIN:"); itoaSocket(s, CAM_PITCH_SERVO_MIN);
    StringToSocket(s, ",CAM_PITCH_OFFSET_CENTRED:"); itoaSocket(s, CAM_PITCH_OFFSET_CENTRED);
    StringToSocket(s, ",CAM_YAW_SERVO_THROW:"); itoaSocket(s, CAM_YAW_SERVO_THROW);
    StringToSocket(s, ",CAM_YAW_SERVO_MAX:"); itoaSocket(s, CAM_YAW_SERVO_MAX);
    StringToSocket(s, ",CAM_YAW_SERVO_MIN:"); itoaSocket(s, CAM_YAW_SERVO_MIN);
    StringToSocket(s, ",CAM_YAW_OFFSET_CENTRED:"); itoaSocket(s, CAM_YAW_OFFSET_CENTRED);
    StringToSocket(s, ",CAM_TESTING_OVERIDE:"); itoaSocket(s, CAM_TESTING_OVERIDE);
    StringToSocket(s, ",CAM_TESTING_YAW_ANGLE:"); itoaSocket(s, CAM_TESTING_YAW_ANGLE);
    StringToSocket(s, ",CAM_TESTING_PITCH_ANGLE:"); itoaSocket(s, CAM_TESTING_PITCH_ANGLE);
    StringToSocket(s, ",CAM_USE_EXTERNAL_TARGET_DATA:"); itoaSocket(s, CAM_USE_EXTERNAL_TARGET_DATA);
    StringToSocket(s, ",HEIGHT_TARGET_MIN:"); ftoaSocket(s,HEIGHT_TARGET_MIN,3);
    StringToSocket(s, ",HEIGHT_TARGET_MAX:"); ftoaSocket(s,HEIGHT_TARGET_MAX,3);
    StringToSocket(s, ",HEIGHT_MARGIN:"); ftoaSocket(s,HEIGHT_MARGIN,3);
    StringToSocket(s, ",ALT_HOLD_THROTTLE_MIN:"); ftoaSocket(s,ALT_HOLD_THROTTLE_MIN,3);
    StringToSocket(s, ",ALT_HOLD_THROTTLE_MAX:"); ftoaSocket(s,ALT_HOLD_THROTTLE_MAX,3);
    StringToSocket(s, ",ALT_HOLD_PITCH_MIN:"); ftoaSocket(s,ALT_HOLD_PITCH_MIN,3);
    StringToSocket(s, ",ALT_HOLD_PITCH_MAX:"); ftoaSocket(s,ALT_HOLD_PITCH_MAX,3);
    StringToSocket(s, ",ALT_HOLD_PITCH_HIGH:"); ftoaSocket(s,ALT_HOLD_PITCH_HIGH,3);
    StringToSocket(s, ",RTL_PITCH_DOWN:"); ftoaSocket(s,RTL_PITCH_DOWN,3);
    break;

  case 5:
    StringToSocket(s, ",HILSIM:"); itoaSocket(s, HILSIM);
    StringToSocket(s, ",HILSIM_BAUD:"); itoaSocket(s, HILSIM_BAUD);
    StringToSocket(s, ",SILSIM:"); itoaSocket(s, SILSIM);
    StringToSocket(s, ",SILSIM_GPS_RUN_AS_SERVER:"); itoaSocket(s, SILSIM_GPS_RUN_AS_SERVER);
    StringToSocket(s, ",SILSIM_GPS_PORT:"); itoaSocket(s, SILSIM_GPS_PORT);
    StringToSocket(s, ",SILSIM_GPS_HOST:"); StringToSocket(s, SILSIM_GPS_HOST);
    StringToSocket(s, ",SILSIM_TELEMETRY_RUN_AS_SERVER:"); itoaSocket(s, SILSIM_TELEMETRY_RUN_AS_SERVER);
    StringToSocket(s, ",SILSIM_TELEMETRY_PORT:"); itoaSocket(s, SILSIM_TELEMETRY_PORT);
    StringToSocket(s, ",SILSIM_TELEMETRY_HOST:"); StringToSocket(s, SILSIM_TELEMETRY_HOST);
    StringToSocket(s, ",SILSIM_SERIAL_RC_INPUT_DEVICE:"); StringToSocket(s, SILSIM_SERIAL_RC_INPUT_DEVICE);
    StringToSocket(s, ",SILSIM_SERIAL_RC_INPUT_BAUD:"); itoaSocket(s, SILSIM_SERIAL_RC_INPUT_BAUD);
    StringToSocket(s, ",FLIGHT_PLAN_TYPE:"); itoaSocket(s, FLIGHT_PLAN_TYPE);
    StringToSocket(s, ",RECORD_FREE_STACK_SPACE:"); itoaSocket(s, RECORD_FREE_STACK_SPACE);
    StringToSocket(s, ",ID_VEHICLE_MODEL_NAME:"); StringToSocket(s, ID_VEHICLE_MODEL_NAME);
    StringToSocket(s, ",ID_VEHICLE_REGISTRATION:"); StringToSocket(s, ID_VEHICLE_REGISTRATION);
    StringToSocket(s, ",ID_LEAD_PILOT:"); StringToSocket(s, ID_LEAD_PILOT);
    StringToSocket(s, ",ID_DIY_DRONES_URL:"); StringToSocket(s, ID_DIY_DRONES_URL);
    StringToSocket(s, ",NETWORK_INTERFACE:"); itoaSocket(s, NETWORK_INTERFACE);
    StringToSocket(s, ",NETWORK_USE_UART1:"); itoaSocket(s, NETWORK_USE_UART1);
    StringToSocket(s, ",NETWORK_USE_UART2:"); itoaSocket(s, NETWORK_USE_UART2);
    StringToSocket(s, ",NETWORK_USE_FLYBYWIRE:"); itoaSocket(s, NETWORK_USE_FLYBYWIRE);
    StringToSocket(s, ",NETWORK_USE_MAVLINK:"); itoaSocket(s, NETWORK_USE_MAVLINK);
    StringToSocket(s, ",NETWORK_USE_DEBUG:"); itoaSocket(s, NETWORK_USE_DEBUG);
    StringToSocket(s, ",NETWORK_USE_ADSB:"); itoaSocket(s, NETWORK_USE_ADSB);
    StringToSocket(s, ",NETWORK_USE_LOGO:"); itoaSocket(s, NETWORK_USE_LOGO);
    StringToSocket(s, ",NETWORK_USE_CAM_TRACKING:"); itoaSocket(s, NETWORK_USE_CAM_TRACKING);
    StringToSocket(s, ",NETWORK_USE_GPSTEST:"); itoaSocket(s, NETWORK_USE_GPSTEST);
    StringToSocket(s, ",NETWORK_USE_PWMREPORT:"); itoaSocket(s, NETWORK_USE_PWMREPORT);
    StringToSocket(s, ",NETWORK_USE_XPLANE:"); itoaSocket(s, NETWORK_USE_XPLANE);
    StringToSocket(s, ",NETWORK_USE_TELEMETRY_EXTRA:"); itoaSocket(s, NETWORK_USE_TELEMETRY_EXTRA);
    StringToSocket(s, ",NETWORK_USE_GROUND_STATION:"); itoaSocket(s, NETWORK_USE_GROUND_STATION);
    StringToSocket(s, ",NETWORK_USE_AIRCRAFT_CONFIG:"); itoaSocket(s, NETWORK_USE_AIRCRAFT_CONFIG);
    StringToSocket(s, ",FLYBYWIRE_ENABLED:"); itoaSocket(s, FLYBYWIRE_ENABLED);
    StringToSocket(s, ",GROUND_TEST:"); itoaSocket(s, GROUND_TEST);
    StringToSocket(s, ",CATAPULT_LAUNCH_ENABLE:"); itoaSocket(s, CATAPULT_LAUNCH_ENABLE);
    break;

  default:
    sendOptionsIndex[i] = SEND_OPTIONS_INDEX_DONE;
    return;
  }

  StringToSocket(s, ",\r\n");
  MyIpData[s].sendPacket = TRUE; // send right away
  sendOptionsIndex[i]++;
}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPAIRCRAFTCONFIG_C_

