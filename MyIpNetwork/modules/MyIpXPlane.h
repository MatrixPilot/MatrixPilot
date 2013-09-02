
#ifndef _MYIPXPLANE_H_
#define _MYIPXPLANE_H_

enum LOCATION_SELECT{
  LOCATION_SELECT_ACTUAL = 0,
  LOCATION_SELECT_SFO,
  LOCATION_SELECT_BAYLANDS,
};
#define BAYLANDS_LAT    (37.4124664)
#define BAYLANDS_LONG   (-121.9950467)
#define BAYLANDS_ALT    (2.0)
#define SFO_LAT         (37.622118)
#define SFO_LONG        (-122.381172)
#define SFO_ALT         (10.0)

void MyIpInit_XPlane(const uint8_t s);
void MyIpService_XPlane(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_XPlane(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_XPlane(const uint8_t s);
void MyIpProcessRxData_XPlane(const uint8_t s);
void MyIpOnConnect_XPlane(const uint8_t s);

void MyIpXplane_ResetSM();

// For info about the X Plane UDP protocol, see this file in your xplane folder
// C:\X-Plane 10\Instructions\Sending Data to X-Plane.html
typedef char xchr;
typedef int32_t xint;
typedef float xflt;
typedef long double xdob;
#define strDIM      500
#define vehDIM      20
#define path_rel_SIZE 150
#define att_file_QTY 24
#define att_file_SIZE 40
#define XPLANE_PACKET_HEADER_SIZE    5



typedef struct __attribute__((aligned(4), packed))
{
  xint index; // data index, the index into the list of variables you can output from the Data Output screen in X-Plane.
  xflt data[8]; // the up to 8 numbers you see in the data output screen associated with that selection.. many outputs do not use all 8, though.
}
data_struct;

typedef struct __attribute__((aligned(4), packed))
{ // play any sound
  xflt freq, vol;
  xchr path[strDIM];
}
soun_struct;

typedef struct __attribute__((aligned(4), packed))
{
  xflt var;
  xchr dref_path[strDIM];
}
dref_struct;

typedef struct __attribute__((aligned(4), packed))
{
  xchr apt_ID[5]; // ID to go to, null-termed
  xint type_start; // 601 for takeoff, 602 for final approach, 603 for far-away final approach
  xint lcl_rwy_ram; // which runway or ramp-start location to use, starting at 0 for each airport
  xint backwards; // 0 or 1, depending on if you want to appraoch from the other direction
}
papt_struct;

typedef struct __attribute__((aligned(4), packed))
{
  // NOTE: FOR THIS MESSAGE, YOU MUST ENTER 17 AS THE VALUE RIGHT AFTER THE 4-CHAR LABEL
  xint p;
  xchr path_rel[path_rel_SIZE];
  xchr att_file[att_file_QTY][att_file_SIZE];
}
vehN_struct;

typedef struct __attribute__((aligned(4), packed))
{
  xint unknown;
  xint p; // this is the plane you wish to control.. 0 for your plane, but you can enter up to 9 here
  xdob lat_lon_ele[3];
  xflt psi_the_phi[3];
  xflt gear_flap_vect[3];
}
VEH1_struct;

typedef struct __attribute__((aligned(4), packed))
{
  xint num_p;

  xint unknown1;
  xdob lat_lon_ele[vehDIM][3];
  xflt psi_the_phi[vehDIM][3];
  xflt gear_flap_vect[vehDIM][3];
  xint unknown2;
  xdob lat_view, lon_view, ele_view;
  xflt psi_view, the_view, phi_view;
}
vehA_struct;

typedef struct __attribute__((aligned(4), packed))
{ // object name: draw any object in the world in the sim
  xint index;
  xchr path[strDIM];
}
objN_struct;

#endif // _MYIPXPLANE_H_

