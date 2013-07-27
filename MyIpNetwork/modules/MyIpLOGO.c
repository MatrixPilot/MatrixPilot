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
    bytesAvailable = TCPIsGetReady(MyIpData[s].socket);
  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    StringToSrc(eSourceDebug, "\r\n\r\nRecieved ");
    uitoaSrc(eSourceDebug, bytesAvailable);
    StringToSrc(eSourceDebug, " bytes");
  #endif

    if (bytesAvailable >= LOGO_HEADER_SIZE)
    {
      if (bytesAvailable > LOGO_MAX_RECIEVE_BUFFER)
        bytesAvailable = LOGO_MAX_RECIEVE_BUFFER;
      if (TCPGetArray(MyIpData[s].socket, payload, bytesAvailable))
      {
        processLogoDataMsg(s, payload);
      }
    }
  }
  else //if (eUDP == MyIpData[s].type)
  {
    bytesAvailable = UDPIsGetReady(MyIpData[s].socket);
    if (bytesAvailable >= LOGO_HEADER_SIZE)
    {
      if (bytesAvailable > LOGO_MAX_RECIEVE_BUFFER)
        bytesAvailable = LOGO_MAX_RECIEVE_BUFFER;
      if (UDPGetArray(payload, bytesAvailable))
      {
        processLogoDataMsg(s, payload);
      }
    }
  }

}

void processLogoDataMsg(const uint8_t s, const uint8_t* packet)
{
  uint8_t i=0;
  uint8_t offset;
  struct logoInstructionDef cmd;
  LogoDataMsg header;
  header.cmdtype = packet[0];
  header.mission = packet[1];
  header.indexCmd = packet[2];
  header.length = packet[3];

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
  StringToSrc(eSourceDebug, "\r\ncmdtype : ");  uitoaSrc(eSourceDebug, header.cmdtype);
  StringToSrc(eSourceDebug, "\r\nmission : ");  uitoaSrc(eSourceDebug, header.mission);
  StringToSrc(eSourceDebug, "\r\nindexCmd: ");  uitoaSrc(eSourceDebug, header.indexCmd);
  StringToSrc(eSourceDebug, "\r\nlength  : ");  uitoaSrc(eSourceDebug, header.length);
//itoaSrc
#endif

  if (header.mission >= LOGO_USER_MISSION_MAX)
    return;
  if (header.indexCmd >= LOGO_USER_INSTRUCTIONS_MAX_LENGTH)
    return;

  switch (header.cmdtype)
  {
  case Clear_Mission:
    cmd.arg = 0;
    cmd.cmd = 0;
    cmd.do_fly = 0;
    cmd.subcmd = 0;
    cmd.use_param = 0;
    for (i=0;i<LOGO_USER_INSTRUCTIONS_MAX_LENGTH;i++)
    {
      setLogoCmd(header.mission, i, cmd);
    }
    break;

  case LogoCmd:
  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    StringToSrc(eSourceDebug, "\r\ncmd, fly, p, sub, arg\r\n");
  #endif
    for (i=0;(i<header.length) && (i<LOGO_USER_INSTRUCTIONS_MAX_LENGTH);i++)
    {
      // protect array bounds
      offset = LOGO_HEADER_SIZE + i*LOGO_INST_SIZE;
      cmd.cmd = packet[offset + 0];
      cmd.do_fly = packet[offset + 1];
      cmd.use_param = packet[offset + 2];
      cmd.subcmd = packet[offset + 3];
      cmd.arg = (packet[offset + 4]); // MSB first
      cmd.arg <<= 8;
      cmd.arg |= packet[offset + 5];

  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    ByteToSrc(eSourceDebug, ' ');
    if (cmd.cmd < 10)
      ByteToSrc(eSourceDebug, ' ');
    uitoaSrc(eSourceDebug, cmd.cmd);    StringToSrc(eSourceDebug, ",   ");
    uitoaSrc(eSourceDebug, cmd.do_fly); StringToSrc(eSourceDebug, ", ");
    uitoaSrc(eSourceDebug, cmd.use_param); StringToSrc(eSourceDebug, ",   ");
    uitoaSrc(eSourceDebug, cmd.subcmd); StringToSrc(eSourceDebug, ",  ");
    itoaSrc(eSourceDebug, cmd.arg);
  #endif

      setLogoCmd(header.mission, header.indexCmd+i, cmd);
    }
    break;

  case Execute_Mission:
    // This will override the current hardcoded waypoint mission.
    // You must be in Waypoint mode (not RTL) *BEFORE* this is recived to execute.
    // Disabling waypoint mode and re-enabling it will start the hardcoded waypoint as usual
    init_flightplan(header.mission, header.indexCmd);
    break;

  case Resume_Mission:
    // TODO: This is not yet implemented correctly. The LogoStack and stackIndex but
    // be converted into an array so all states of all missions have their own stack
    init_flightplan(header.mission, getLogoCmdIndex(header.mission));
    break;

  case Read_Mission:
    sendLogoResponse_ReadMission(s, header.mission);
    MyIpData[s].sendPacket = true;
    break;

  case Read_Cmd:
    sendLogoResponse_ReadCmd(s, header.mission);
    MyIpData[s].sendPacket = true;
    break;

  case Read_Cmd_Response:
  case Read_Mission_Response:
  default:
    break;
  }
}

void sendLogoResponse_ReadCmd(const uint8_t s, const uint8_t mission)
{
  uint8_t packet[LOGO_HEADER_SIZE + LOGO_INST_SIZE];
  uint8_t cmdIndex = getLogoCmdIndex(mission);
  struct logoInstructionDef* cmdList = getLogoMission(mission);

  // header
  packet[0] = Read_Cmd_Response; // header.cmdtype
  packet[1] = mission; // header.mission
  packet[2] = cmdIndex; // header.indexCmd

  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    StringToSrc(eSourceDebug, "\r\nsendLogoResponse_ReadCmd");
  #endif


  if (cmdList == NULL)
  {
  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    StringToSrc(eSourceDebug, "\r\ncmdList == NULL. returning empty packet");
  #endif
    packet[3] = 0; // header.length
    ArrayToSocket(s,packet,LOGO_HEADER_SIZE);
  }
  else
  {
    packet[3] = 1; // header.length

    // one Logo instruction
    packet[4] = cmdList[cmdIndex].cmd;
    packet[5] = cmdList[cmdIndex].do_fly;
    packet[6] = cmdList[cmdIndex].use_param;
    packet[7] = cmdList[cmdIndex].subcmd;
    packet[8] = cmdList[cmdIndex].arg >> 8;
    packet[9] = cmdList[cmdIndex].arg & 0xFF;

    ArrayToSocket(s,packet,LOGO_HEADER_SIZE + LOGO_INST_SIZE);

  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    StringToSrc(eSourceDebug, "\r\ncmdIndex: "); uitoaSrc(eSourceDebug,cmdIndex);
    StringToSrc(eSourceDebug, "  mission: "); uitoaSrc(eSourceDebug,mission); StringToSrc(eSourceDebug, "  instr: ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].cmd); StringToSrc(eSourceDebug, ",  ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].do_fly); StringToSrc(eSourceDebug, ",  ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].use_param); StringToSrc(eSourceDebug, ",  ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].subcmd); StringToSrc(eSourceDebug, ", ");
    itoaSrc(eSourceDebug, cmdList[cmdIndex].arg);
  #endif
  }
}

void sendLogoResponse_ReadMission(const uint8_t s, const uint8_t mission)
{
  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
  StringToSrc(eSourceDebug, "\r\nsendLogoResponse_ReadMission");
  #endif

  uint8_t packet[LOGO_HEADER_SIZE + LOGO_INST_SIZE*LOGO_USER_INSTRUCTIONS_MAX_LENGTH];
  struct logoInstructionDef* cmdList = getLogoMission(mission);
  uint8_t missionLength = getLogoMissionLength(mission);
  uint8_t cmdIndex;
  uint8_t i = 0;

  if (cmdList == NULL)
  {
    missionLength = 0;
  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    StringToSrc(eSourceDebug, "\r\ncmdList == NULL.");
  #endif
  }

  if (missionLength >= LOGO_USER_INSTRUCTIONS_MAX_LENGTH)
    missionLength = LOGO_USER_INSTRUCTIONS_MAX_LENGTH;

  // header
  packet[0] = Read_Mission_Response; // header.cmdtype
  packet[1] = mission; // header.mission
  packet[2] = 0; // header.indexCmd
  packet[3] = missionLength; // header.length

  i = 4;
  for (cmdIndex=0;cmdIndex<missionLength;cmdIndex++)
  {
    // one Logo instruction
    packet[i++] = cmdList[cmdIndex].cmd;
    packet[i++] = cmdList[cmdIndex].do_fly;
    packet[i++] = cmdList[cmdIndex].use_param;
    packet[i++] = cmdList[cmdIndex].subcmd;
    packet[i++] = cmdList[cmdIndex].arg >> 8; // MSB first
    packet[i++] = cmdList[cmdIndex].arg & 0xFF;

  #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
    StringToSrc(eSourceDebug, "\r\nmission[");  uitoaSrc(eSourceDebug,cmdIndex); StringToSrc(eSourceDebug, "].instr: ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].cmd); StringToSrc(eSourceDebug, ",  ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].do_fly); StringToSrc(eSourceDebug, ",  ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].use_param); StringToSrc(eSourceDebug, ",  ");
    uitoaSrc(eSourceDebug, cmdList[cmdIndex].subcmd); StringToSrc(eSourceDebug, ", ");
    itoaSrc(eSourceDebug, cmdList[cmdIndex].arg);
  #endif
}
  ArrayToSocket(s,packet,i);
}
#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPLOGO_C_

