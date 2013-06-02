#ifndef _MYIPXPLANE_C_
#define _MYIPXPLANE_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_XPLANE == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpXPlane.h"
#include "MyIpHelpers.h"
#include "euler_angles.h"

// For info about the X Plane UDP protocol, see this file in your xplane folder
// C:\X-Plane 10\Instructions\Sending Data to X-Plane.html
typedef int8_t xchr;
typedef int32_t xint;
typedef float xflt;
typedef long double xdob;
#define strDIM      500
#define vehDIM      20
#define path_rel_SIZE 150
#define att_file_QTY 24
#define att_file_SIZE 40
#define XPLANE_PACKET_HEADER_SIZE    5

void CalculateGPS_Orientation();
double currentOrientation[3];
double currentGPS[3];

#define BAYLANDS_LAT    (374124664)
#define BAYLANDS_LONG   (-1219950467)
#define BAYLANDS_ALT    (2.0)
#define SFO_LAT         (37.622118)
#define SFO_LONG        (-122.381172)
#define SFO_ALT         (10.0)


typedef struct __attribute__((aligned(4), packed)) {
    xint index; // data index, the index into the list of variables you can output from the Data Output screen in X-Plane.
    xflt data[8]; // the up to 8 numbers you see in the data output screen associated with that selection.. many outputs do not use all 8, though.
}data_struct;

typedef struct __attribute__((aligned(4), packed)) { // play any sound
    xflt freq,vol;
    xchr path[strDIM];
} soun_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xflt var;
    xchr dref_path[strDIM];
} dref_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xchr apt_ID[5] ; // ID to go to, null-termed
    xint type_start ; // 601 for takeoff, 602 for final approach, 603 for far-away final approach
    xint lcl_rwy_ram; // which runway or ramp-start location to use, starting at 0 for each airport
    xint backwards ; // 0 or 1, depending on if you want to appraoch from the other direction
} papt_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xint p;
    xchr path_rel[path_rel_SIZE];
    xchr att_file[att_file_QTY][att_file_SIZE];
}vehN_struct;

typedef struct __attribute__((aligned(4), packed)) { // byte-align 4
    xint unknown;
    xint p; // this is the plane you wish to control.. 0 for your plane, but you can enter up to 9 here
    xdob lat_lon_ele[3];
    xflt psi_the_phi[3];
    xflt gear_flap_vect[3];
}VEH1_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xint num_p;

    xint unknown1;
    xdob lat_lon_ele[vehDIM][3];
    xflt psi_the_phi[vehDIM][3];
    xflt gear_flap_vect[vehDIM][3];
    xint unknown2;
    xdob lat_view,lon_view,ele_view;
    xflt psi_view,the_view,phi_view;
}vehA_struct;

typedef struct __attribute__((aligned(4), packed)) {	 // object name: draw any object in the world in the sim
    xint index;
    xchr path[strDIM];
}objN_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xint index; // (0->20 in the lsit below)
    xchr str_ipad_them[16]; // IP's we are sending to, in english
    xchr str_port_them[6]; // ports are easier to work with in STRINGS!
    xint use_ip ; // to use this option, 0 not to.
}iset_struct;

void SendXplanePacketSingle(uint8_t s, xint planeNumber);
void SendXplanePacketMulti(uint8_t s);
void SendXplaneAircraft(uint8_t);

//////////////////////////
// Module Variables
uint32_t taskTimer_XPlane[MAX_NUM_INSTANCES_OF_MODULES];

void MyIpOnConnect_XPlane(const BYTE s)
{
  //SendXplaneAircraft(s);
}
//void SendXplaneAircraft(uint8_t)
//{
//  int i;
//  vehN_struct packet; // 150 + 24*40
//  memset(0, &packet, sizeof(packet));
//
//  packet.p = 0;
//  #define testSTR "test"
//
//  memcpy(&packet.path_rel,testSTR, sizeof(testSTR));
//  for (i=0;i<att_file_QTY;i++)
//  {
//    memcpy(&packet.att_file,testSTR, sizeof(testSTR));
//  }
//}


void MyIpInit_XPlane(const BYTE s)
{
    // This gets called once for every socket we're configured to use for this module.
    BYTE i = MyIpData[s].instance;
    taskTimer_XPlane[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_XPlane(const BYTE s)
{
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;

    uint8_t i = MyIpData[s].instance;
    uint32_t tick = TickGet();
    
    if ((tick - taskTimer_XPlane[i]) > ((TICK_SECOND)/2)) // 10Hz
    {
      taskTimer_XPlane[i] = tick;
      CalculateGPS_Orientation();


      SendXplanePacketSingle(s, 1);
      //SendXplanePacketMulti(s);
    }
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
      // No Rx data parsing implemented
    }
  } while (successfulRead);
}

void SendXplanePacketSingle(uint8_t s, xint planeNumber)
{
  VEH1_struct packet;

  StringToSocket(s,"VEH1"); ByteToSocket(s, 0);
  packet.unknown = 0;
  packet.p = planeNumber;

  packet.lat_lon_ele[0] = currentGPS[0];
  packet.lat_lon_ele[1] = currentGPS[1];
  packet.lat_lon_ele[2] = currentGPS[2];

  packet.psi_the_phi[0] = currentOrientation[0];// yaw
  packet.psi_the_phi[1] = currentOrientation[1];// pitch
  packet.psi_the_phi[2] = currentOrientation[2];// roll

  packet.gear_flap_vect[0] = -999;
  packet.gear_flap_vect[1] = -999;
  packet.gear_flap_vect[2] = -999;
    
  ArrayToSocket(s, (BYTE*)&packet,sizeof(packet));
  MyIpData[s].sendPacket = TRUE;
}


void SendXplanePacketMulti(uint8_t s)
{
  vehA_struct packet;
  uint8_t i;

  StringToSocket(s,"VEHA"); ByteToSocket(s, 0);

  packet.num_p = 4;   // # of planes
  packet.unknown1 = 0;
  packet.unknown2 = 0;

  packet.lat_view = 0; // 0 == use Xplane default view
  packet.lon_view = 0;
  packet.ele_view = 0;
  packet.phi_view = 0;
  packet.the_view = 0;
  packet.psi_view = 0;

  for (i=0;i<vehDIM;i++)
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
  if (packet.num_p >= 1) {
  packet.lat_lon_ele[0][0] = currentGPS[0];
  packet.lat_lon_ele[0][1] = currentGPS[1];
  packet.lat_lon_ele[0][2] = currentGPS[2];

  packet.psi_the_phi[0][0] = currentOrientation[0];
  packet.psi_the_phi[0][1] = currentOrientation[1];
  packet.psi_the_phi[0][2] = currentOrientation[2];
  }

 
 // extra plane #1
  if (packet.num_p >= 2) {
  packet.lat_lon_ele[1][0] = currentGPS[0] + 0.0001;
  packet.lat_lon_ele[1][1] = currentGPS[1] + 0.0001;
  packet.lat_lon_ele[1][2] = currentGPS[2] + 5;
  }

 // extra plane #2
  if (packet.num_p >= 3) {
  packet.lat_lon_ele[2][0] = currentGPS[0] - 0.0001;
  packet.lat_lon_ele[2][1] = currentGPS[1] - 0.0001;
  packet.lat_lon_ele[2][2] = currentGPS[2] + 10;
  }

  // extra plane #3
  if (packet.num_p >= 4) {
  packet.lat_lon_ele[3][0] = currentGPS[0] + 0.0001;
  packet.lat_lon_ele[3][1] = currentGPS[1] - 0.0001;
  packet.lat_lon_ele[3][2] = currentGPS[2] + 15;
  }

 // extra plane #4
  if (packet.num_p >= 5) {
  packet.lat_lon_ele[4][0] = currentGPS[0] - 0.0001;
  packet.lat_lon_ele[4][1] = currentGPS[1] + 0.0001;
  packet.lat_lon_ele[4][2] = currentGPS[2] + 20;
  }

  ArrayToSocket(s, (BYTE*)&packet,sizeof(packet));
  MyIpData[s].sendPacket = TRUE;
}


void CalculateGPS_Orientation()
{
  int32_t earth_pitch;		// pitch in binary angles ( 0-255 is 360 degreres)
	int32_t earth_roll;		// roll of the plane with respect to earth frame
	struct relative2D matrix_accum;

    	//  Roll
	//  Earth Frame of Reference
	matrix_accum.x = rmat[8] ;
	matrix_accum.y = rmat[6] ;
	earth_roll = rect_to_polar(&matrix_accum) ;					// binary angle (0 - 256 = 360 degrees)
	earth_roll = (-earth_roll * BYTECIR_TO_DEGREE) >> 16 ;		// switch polarity, convert to -180 - 180 degrees

	//  Pitch
	//  Earth Frame of Reference
	//  Note that we are using the matrix_accum.x
	//  left over from previous rect_to_polar in this calculation.
	//  so this Pitch calculation must follow the Roll calculation
  matrix_accum.y = rmat[7] ;
  earth_pitch = rect_to_polar(&matrix_accum) ;				// binary angle (0 - 256 = 360 degrees)
  earth_pitch = (-earth_pitch * BYTECIR_TO_DEGREE) >> 16 ;	// switch polarity, convert to -180 - 180 degrees

  currentOrientation[0] = get_mag_heading_angle();    // yaw (magnetic)
  currentOrientation[1] = earth_pitch;                // pitch
  currentOrientation[2] = earth_roll;                 // roll



#if 1
  currentGPS[0] = ((double)lat_gps.WW)/10000000;
  currentGPS[1] = ((double)long_gps.WW)/10000000;
  currentGPS[2] = (double)alt_sl_gps.WW/100; // meters
#else
// { -1219950467, 374124664, 2.00 }	// A point in Baylands Park in Sunnyvale, CA
// SFO = 37.622118,-122.381172
// baylands = 37.4124664, -121.9950467

  currentGPS[0] = SFO_LAT;   // SFO
  currentGPS[1] = SFO_LONG;  // SFO
  currentGPS[2] = SFO_ALT;
#endif
}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPXPLANE_C_

