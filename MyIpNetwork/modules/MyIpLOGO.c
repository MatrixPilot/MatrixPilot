#ifndef _MYIPLOGO_C_
#define _MYIPLOGO_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_LOGO == 1) && (FLIGHT_PLAN_TYPE == FP_LOGO)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpLOGO.h"
#include "euler_angles.h"
#include "MyIpHelpers.h"
#include "flightplan-logoMsgFormat.h"

//////////////////////////
// Module Variables

void MyIpOnConnect_LOGO(const uint8_t s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to LOGO on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_LOGO(const uint8_t s)
{

    // This gets called once for every socket we're configured to use for this module.
}

void MyIpService_LOGO(const uint8_t s)
{
    
}

boolean MyIpThreadSafeSendPacketCheck_LOGO(const uint8_t s, const boolean doClearFlag)
{
    // since this data comes from, and goes to, the idle thread we
    // don't need to deal with any thread issues
    boolean sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag)
    {
        MyIpData[s].sendPacket = FALSE;
    }
    return sendpacket;
}

int MyIpThreadSafeReadBufferHead_LOGO(const uint8_t s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_LOGO(const uint8_t s)
{
  uint8_t payload[LOGO_MAX_RECIEVE_BUFFER];
  uint16_t bytesAvailable;

  if (eTCP == MyIpData[s].type)
  {
    bytesAvailable = TCPIsGetReady(s);
    if (bytesAvailable >= LOGO_HEADER_SIZE)
    {
      if (bytesAvailable > LOGO_MAX_RECIEVE_BUFFER)
        bytesAvailable = LOGO_MAX_RECIEVE_BUFFER;
      if (TCPGetArray(s, payload, bytesAvailable))
      {
        processLogoDataMsg(payload);
      }
    }
  }
  else //if (eUDP == MyIpData[s].type)
  {
    bytesAvailable = UDPIsGetReady(s);
    if (bytesAvailable >= LOGO_HEADER_SIZE)
    {
      if (bytesAvailable > LOGO_MAX_RECIEVE_BUFFER)
        bytesAvailable = LOGO_MAX_RECIEVE_BUFFER;
      if (UDPGetArray(payload, bytesAvailable))
      {
        processLogoDataMsg(payload);
      }
    }
  }

}

void processLogoDataMsg(uint8_t* packet)
{
  uint8_t i=0;
  uint8_t offset;
  struct logoInstructionDef cmd;
  LogoDataMsg header;
  header.cmdtype = packet[0];
  header.mission = packet[1];
  header.indexCmd = packet[2];
  header.length = packet[3];

  if (header.mission >= LOGO_REMOTE_MISSION_MAX)
    return;
  if (header.indexCmd >= LOGO_REMOTE_INSTRUCTIONS_MAX_LENGTH)
    return;

  switch (header.cmdtype)
  {
  case Clear_Mission:
    cmd.arg = 0;
    cmd.cmd = 0;
    cmd.do_fly = 0;
    cmd.subcmd = 0;
    cmd.use_param = 0;
    for (i=0;i<LOGO_REMOTE_INSTRUCTIONS_MAX_LENGTH;i++)
    {
      remoteInstructions[header.mission][i] = cmd;
    }
    break;

  case LogoCmd:
    for (i=0;i<header.length;i++)
    {
      // protect array bounds
      if ((header.indexCmd+i) >= LOGO_REMOTE_INSTRUCTIONS_MAX_LENGTH)
        break;
      
      offset = LOGO_HEADER_SIZE + i*LOGO_INST_SIZE;
      cmd.cmd = packet[offset + 0];
      cmd.do_fly = packet[offset + 1];
      cmd.use_param = packet[offset + 2];
      cmd.subcmd = packet[offset + 3];
      cmd.arg = (packet[offset + 4]); // MSB first
      cmd.arg <<= 8;
      cmd.arg |= packet[offset + 5];
      remoteInstructions[header.mission][header.indexCmd+i] = cmd;
    }
    break;

  case Execute_Mission:
    // This will override the current hardcoded waypoint mission.
    // You must be in Waypoint mode (not RTL) *BEFORE* this is recived to execute.
    // Disabling waypoint mode and re-enabling it will start the hardcoded waypoint as usual
    init_flightplan(FP_REMOTE, header.mission, header.indexCmd);
    break;

  }
}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPLOGO_C_

