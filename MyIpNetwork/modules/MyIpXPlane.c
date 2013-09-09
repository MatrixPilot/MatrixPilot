#ifndef _MYIPXPLANE_C_
#define _MYIPXPLANE_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_XPLANE == 1)

#include "TCPIP Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpXPlane.h"
#include "MyIpHelpers.h"
#include "euler_angles.h"
#include <string.h>

void CalculateGPS_Orientation(uint8_t select);
double currentOrientation[3];
double currentGPS[3];

typedef struct __attribute__((aligned(4), packed))
{
  xint index; // (0->20 in the lsit below)
  xchr str_ipad_them[16]; // IP's we are sending to, in english
  xchr str_port_them[6]; // ports are easier to work with in STRINGS!
  xint use_ip; // to use this option, 0 not to.
}
iset_struct;

void SendXplanePacketSingle(const uint8_t s, const xint planeNumber);
void SendXplanePacketMulti(const uint8_t s);
void SendXplaneLoadAircraft(const uint8_t s, const uint8_t index, const char* path);

//////////////////////////
// Module Variables
uint32_t taskTimer_telemetry[MAX_NUM_INSTANCES_OF_MODULES];
uint32_t taskTimer_planeType[MAX_NUM_INSTANCES_OF_MODULES];
uint8_t sm_planeType[MAX_NUM_INSTANCES_OF_MODULES];

void MyIpXplane_ResetSM()
{
  uint8_t i;
  for (i=0;i<MAX_NUM_INSTANCES_OF_MODULES;i++)
    sm_planeType[i] = 0;
}
void MyIpOnConnect_XPlane(const BYTE s)
{
  BYTE i = MyIpData[s].instance;
  sm_planeType[i] = 0;
}


void MyIpInit_XPlane(const BYTE s)
{
  // This gets called once for every socket we're configured to use for this module.
  BYTE i = MyIpData[s].instance;
  taskTimer_telemetry[i] = GenerateRandomDWORD() % (TICK_SECOND);
  taskTimer_planeType[i] = GenerateRandomDWORD() % (TICK_SECOND);
  sm_planeType[i] = 0;
}

void MyIpService_XPlane(const BYTE s)
{
  // don't bother queuing data if no one is listening
  uint8_t i = MyIpData[s].instance;
  uint32_t tick = TickGet();
  if ((tick - taskTimer_telemetry[i]) < (TICK_SECOND/2))
    return;
  if (IsMyIpBufferReady(s) < 1200)
    return;
  if (UDPIsPutReady(s) < 1200)
    return;

  taskTimer_telemetry[i] = tick;

  switch (sm_planeType[i])
  {
  case 0:
    break;
  case 1:
    SendXplaneLoadAircraft(s, 2, "Aircraft/Fighters/F-4 Phantom/F-4.acf");
    break;
  case 2:
    //SendXplaneLoadAircraft(s, 2, "/Aircraft/Fighters/F-4 Phantom/F-4.acf");
    break;
  case 3:
    //SendXplaneLoadAircraft(s, 3, "Aircraft//Fighters//F-4 Phantom//F-4.acf");
    break;
  case 4:
   // SendXplaneLoadAircraft(s, 4, "//Aircraft//Fighters//F-4 Phantom//F-4.acf");
    break;
  case 5:
  default:
    CalculateGPS_Orientation(LOCATION_SELECT_SFO);
    //SendXplanePacketSingle(s);
    SendXplanePacketMulti(s);
    sm_planeType[i] = 99;
    break;
  } // switch sm_planeType
  sm_planeType[i]++;

}

boolean MyIpThreadSafeSendPacketCheck_XPlane(const uint8_t s, boolean doClearFlag)
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

int MyIpThreadSafeReadBufferHead_XPlane(const uint8_t s)
{
  // since this data comes from, and goes to, the idle thread we
  //  don't need to deal with any thread issues
  return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_XPlane(const uint8_t s)
{
  uint8_t rxData;
  boolean successfulRead;
  if (eTCP == MyIpData[s].type)
    return; // This is a UDP only module

  do
  {
    successfulRead = UDPGet(&rxData);
    if (successfulRead)
    {
      switch (rxData)
      {
      case 'r':
        sm_planeType[MyIpData[s].instance] = 0;
        break;
      }
      // No Rx data parsing implemented
    }
  }
  while (successfulRead);
}

void SendXplanePacketSingle(const uint8_t s, const xint planeNumber)
{
  VEH1_struct packet;

  StringToSocket(s, "VEH1");
  ByteToSocket(s, 0);
  packet.unknown = 0;
  packet.p = planeNumber;

  packet.lat_lon_ele[0] = currentGPS[0];
  packet.lat_lon_ele[1] = currentGPS[1];
  packet.lat_lon_ele[2] = currentGPS[2];

  packet.psi_the_phi[0] = currentOrientation[0]; // yaw
  packet.psi_the_phi[1] = currentOrientation[1]; // pitch
  packet.psi_the_phi[2] = currentOrientation[2]; // roll

  packet.gear_flap_vect[0] = -999;
  packet.gear_flap_vect[1] = -999;
  packet.gear_flap_vect[2] = -999;

  ArrayToSocket(s, (BYTE*)&packet, sizeof(packet));
  MyIpData[s].sendPacket = TRUE;
}

void SendXplanePacketMulti(const uint8_t s)
{
  vehA_struct packet;
  uint8_t i;

  StringToSocket(s, "VEHA"); ByteToSocket(s, 0);

  packet.num_p = 4; // # of planes
  packet.unknown1 = 0;
  packet.unknown2 = 0;

  packet.lat_view = 0; // 0 == use Xplane default view
  packet.lon_view = 0;
  packet.ele_view = 0;
  packet.phi_view = 0;
  packet.the_view = 0;
  packet.psi_view = 0;

  for (i = 0; i < vehDIM; i++)
  {
    packet.lat_lon_ele[i][0] = 0;
    packet.lat_lon_ele[i][1] = 0;
    packet.lat_lon_ele[i][2] = 0;

    packet.psi_the_phi[i][0] = 0; // yaw
    packet.psi_the_phi[i][1] = 0; // pitch
    packet.psi_the_phi[i][2] = 0; // roll

    packet.gear_flap_vect[i][0] = 0;
    packet.gear_flap_vect[i][1] = 0;
    packet.gear_flap_vect[i][2] = 0;
  }

  // main plane
  if (packet.num_p >= 1)
  {
    packet.lat_lon_ele[0][0] = currentGPS[0];
    packet.lat_lon_ele[0][1] = currentGPS[1];
    packet.lat_lon_ele[0][2] = currentGPS[2];

    packet.psi_the_phi[0][0] = currentOrientation[0];
    packet.psi_the_phi[0][1] = currentOrientation[1];
    packet.psi_the_phi[0][2] = currentOrientation[2];
  }


  // extra plane #1
  if (packet.num_p >= 2)
  {
    packet.lat_lon_ele[1][0] = currentGPS[0] + 0.0001;
    packet.lat_lon_ele[1][1] = currentGPS[1] + 0.0001;
    packet.lat_lon_ele[1][2] = currentGPS[2] + 5;
  }

  // extra plane #2
  if (packet.num_p >= 3)
  {
    packet.lat_lon_ele[2][0] = currentGPS[0] - 0.0001;
    packet.lat_lon_ele[2][1] = currentGPS[1] - 0.0001;
    packet.lat_lon_ele[2][2] = currentGPS[2] + 10;
  }

  // extra plane #3
  if (packet.num_p >= 4)
  {
    packet.lat_lon_ele[3][0] = currentGPS[0] + 0.0001;
    packet.lat_lon_ele[3][1] = currentGPS[1] - 0.0001;
    packet.lat_lon_ele[3][2] = currentGPS[2] + 15;
  }

  // extra plane #4
  if (packet.num_p >= 5)
  {
    packet.lat_lon_ele[4][0] = currentGPS[0] - 0.0001;
    packet.lat_lon_ele[4][1] = currentGPS[1] + 0.0001;
    packet.lat_lon_ele[4][2] = currentGPS[2] + 20;
  }

  ArrayToSocket(s, (BYTE*)&packet, sizeof(packet));
  MyIpData[s].sendPacket = TRUE;
}

void CalculateGPS_Orientation(const uint8_t select)
{
  euler_struct status = get_current_aircraft_orientation();

  currentOrientation[0] = status.yaw; // yaw (magnetic)
  currentOrientation[1] = status.pitch; // pitch
  currentOrientation[2] = status.roll; // roll


  switch (select)
  {
  case LOCATION_SELECT_ACTUAL: //
    currentGPS[0] = ((double) lat_gps.WW) / 10000000;
    currentGPS[1] = ((double) lon_gps.WW) / 10000000;
    currentGPS[2] = (double) alt_sl_gps.WW / 100; // meters
    break;

  default:
  case LOCATION_SELECT_SFO:
    currentGPS[0] = SFO_LAT;
    currentGPS[1] = SFO_LONG;
    currentGPS[2] = SFO_ALT;
    break;

  case LOCATION_SELECT_BAYLANDS:
    currentGPS[0] = BAYLANDS_LAT;
    currentGPS[1] = BAYLANDS_LONG;
    currentGPS[2] = BAYLANDS_ALT;
    break;
  }
}

void SendXplaneLoadAircraft(const uint8_t s, const uint8_t index, const char* path)
{
  vehN_struct packet;
  //if (strlen(path) > sizeof(packet.path_rel))
  //  return;

  memset(&packet,0,sizeof(packet));

  packet.p = index;
  //strcpy(packet.path_rel,path);
  //packet.unknown1 = 0;

  StringToSocket(s, "VEHN"); ByteToSocket(s, 17);
  // NOTE: FOR THIS MESSAGE, YOU MUST ENTER 17 AS THE VALUE RIGHT AFTER THE 4-CHAR LABEL

  ArrayToSocket(s, (BYTE*)&packet, sizeof(packet));
  MyIpData[s].sendPacket = TRUE;

}
//int loadAircraftInXPlane()
//{
//   //put airplane in X-Plane
//   vehN_struct vehN;
//   XPlaneData *xpData = new XPlaneData();
//   char *msgProlouge = "VEHN";
//   char buffer[4096];
//   int ptr = 0;
//   int plane = planeCounter;
//   int val5 = 17;
//
//   if (plane > XP_PLANES_MAX) { return (plane); }
//
//   memset(buffer, 0, sizeof(buffer));
//   strcpy(buffer, msgProlouge);
//   ptr += strlen(buffer);
//   memcpy(&buffer[ptr], &val5, sizeof(int));
//   ptr++;
//
//   vehN.vehIndex = xpData->endian(plane);
//
//   // path to airplane we want to load
//   memset(vehN.path, 0, sizeof(vehN.path));
//   strcpy(vehN.path, xplaneConfig.vehNPath.c_str());
//
//   // weapons we want to load; none
//   memset(vehN.weapons, 0, sizeof(vehN.weapons));
//
//   memcpy(&buffer[5], (char *)&vehN, sizeof(vehN));
//   ptr += sizeof(vehN);
//
//   cout << endl << "loading airplane " << plane << " " << vehN.path << endl;
//   //planeCounter++;
//   int status = xpData->sendMessageToXPlane(buffer, ptr);
//   if (status <= 0)
//       cout << "no message sent to X-Plane: " << status << endl;
//
//   delete xpData;
//}
#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPXPLANE_C_

