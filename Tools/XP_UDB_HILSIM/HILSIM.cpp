#include "stdafx.h"

#define MAX_ITEMS 30;

bool CommsEnabled;
HANDLE hComms;
DWORD dwRetFlag;
float fTextColour[3];
char szString[100];

float   SerialPortAccessCB(float elapsedMe, float elapsedSim, int counter, void *refcon);
float	GetBodyRates(float elapsedMe, float elapsedSim, int counter, void * refcon);
void	SerialPortAccessCallback(XPLMWindowID inWindowID, void *inRefcon);
int		DrawStrings(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon);
void	CalculateChecksum(unsigned char msg[]);
void	Store4LE(unsigned char *store, union longbbbb data);
void	Store2LE(unsigned char *store, union intbb data);

void	msgDefault(unsigned char rxChar);
void	msgSync1(unsigned char rxChar);
void	msgServos(unsigned char rxChar);
void	msgCheckSum(unsigned char rxChar);
unsigned char ck_in_a, ck_in_b, ck_calc_a, ck_calc_b;

int store_index = 0;
int gotPacket = 0;

void	(* msg_parse) (unsigned char rxChar) = &msgDefault;

#define SERVO_MSG_LENGTH 12

unsigned char SERVO_IN[] = {
			0x0B, 0xB8,
			0x0B, 0xB8,
			0x0f, 0xff,
			0x0B, 0xB8,
			0x0B, 0xB8,
			0x0B, 0xB8
			};

unsigned char SERVO_IN_[] = {
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x00,
			0x00, 0x00
			};

int rxCount = 0;
int sync = 0;


// At 50hz, this message will be ~9000bps

// 20 bytes
unsigned char NAV_BODYRATES[] = {
			0xB5, 0x62,		// Header
			0x01, 0xAB,		// ID
			0x0C, 0x00,		// Payload Length
			0x00, 0x00,		// P
			0x00, 0x00,		// Q
			0x00, 0x00,		// R
			0x00, 0x00,		// U_dot
			0x00, 0x00,		// V_dot
			0x00, 0x00,		// W_dot
			0x00, 0x00		// Checksum
			};

// At 4hz the normal messages are ~6000 bps

// 60 bytes
unsigned char NAV_SOL[] = {
			0xB5, 0x62,					// Header			- DONE
			0x01, 0x06,					// ID				- DONE
			0x34, 0x00,					// Payload Length	- DONE
            0x00, 0x00, 0x00, 0x00,		// iTOW				- DONE
			0x00, 0x00, 0x00, 0x00,		// fTOW				- DONE
			0x00, 0x00,					// week				- DONE
			0x03,						// gpsFix			- DONE
			0x0D,						// flags			- DONE
			0x00, 0x00, 0x00, 0x00,		// ecefX			- DONE
			0x00, 0x00, 0x00, 0x00,		// ecefY			- DONE
			0x00, 0x00, 0x00, 0x00,		// ecefZ			- DONE
			0x64, 0x00, 0x00, 0x00,		// pAcc				- DONE
			0x00, 0x00, 0x00, 0x00,		// ecefVX			- DONE		
			0x00, 0x00, 0x00, 0x00,		// ecefVY			- DONE		
			0x00, 0x00, 0x00, 0x00,		// ecefVZ			- DONE		
			0x64, 0x00, 0x00, 0x00,		// sACC				- DONE
			0x64, 0x00,					// pDOP				- DONE
			0x00, 						// res1				- DONE
			0x04,						// numSV			- DONE
			0x00, 0x00, 0x00, 0x00,		// res2				- DONE
			0x00, 0x00					// Checksum			- DONE
			};

// 26 bytes
unsigned char NAV_DOP[] = {
			0xB5, 0x62,					// Header			- DONE
			0x01, 0x04,					// ID				- DONE
			0x12, 0x00,					// Payload Length	- DONE
			0x00, 0x00, 0x00, 0x00,		// iTOW				- DONE
			0x64, 0x00,					// gDOP				- DONE
			0x64, 0x00,					// pDOP				- DONE
			0x64, 0x00,					// tDOP				- DONE
			0x64, 0x00,					// vDOP				- DONE
			0x64, 0x00,					// hDOP				- DONE
			0x64, 0x00, 				// nDOP				- DONE
			0x64, 0x00,					// eDOP				- DONE
			0x00, 0x00					// Checksum			- DONE
			};

// 36 bytes
unsigned char NAV_POSLLH[] = {
			0xB5, 0x62,					// Header			- DONE
			0x01, 0x02,					// ID				- DONE
			0x1C, 0x00,					// Payload Length	- DONE
			0x00, 0x00, 0x00, 0x00,		// iTOW				- DONE
			0x00, 0x00, 0x00, 0x00,		// lon				- DONE		
			0x00, 0x00, 0x00, 0x00,		// lat				- DONE
			0x00, 0x00, 0x00, 0x00,		// height			- DONE
			0x00, 0x00, 0x00, 0x00,		// hMSL				- DONE
			0xE8, 0x03, 0x00, 0x00,		// hAcc				- DONE
			0xE8, 0x03, 0x00, 0x00,		// vAcc				- DONE
			0x00, 0x00					// Checksum			- DONE
			};

// 44 bytes
unsigned char NAV_VELNED[] = {
			0xB5, 0x62,					// Header			- DONE
			0x01, 0x12,					// ID				- DONE
			0x24, 0x00,					// Payload Length	- DONE
			0x00, 0x00, 0x00, 0x00, 	// iTOW				- DONE
			0x00, 0x00, 0x00, 0x00,		// velN				- DONE
			0x00, 0x00, 0x00, 0x00, 	// velE				- DONE
			0x00, 0x00, 0x00, 0x00, 	// velD				- DONE
			0x00, 0x00, 0x00, 0x00, 	// speed			- DONE
			0x00, 0x00, 0x00, 0x00, 	// gSpeed			- DONE
			0x00, 0x00, 0x00, 0x00,		// heading			- DONE
			0x64, 0x00, 0x00, 0x00,		// sAcc				- DONE
			0xA0, 0x86, 0x01, 0x00,		// cAcc				- DONE
			0x00, 0x00					// Checksum			- DONE
			};

    XPLMDataRef drP, drQ, drR, 
				drLat, drLon, drElev, 
				drLocal_ax, drLocal_ay, drLocal_az, 
				drLocal_vx, drLocal_vy, drLocal_vz, 
				drLocal_x, drLocal_y, drLocal_z,
				drGroundSpeed, drAirSpeedTrue, drHeading,
				drLocalDays, drLocalSecs,
				drPhi, drTheta, drPsi,
				drAlpha, drBeta,
				drOverRide, drThrOverRide,
				drPitchAxis, drRollAxis, drYawAxis, drThro;


void GetGPSData(void);

void OpenComms(void);
void CloseComms(void);
void SendToComPort(DWORD ResponseLength, unsigned char *Buffer);
void ReceiveFromComPort(void);
void ShowMessage(char *pErrorString);

PLUGIN_API int XPluginStart(
                        char *        outName,
                        char *        outSig,
                        char *        outDesc)
{
    strcpy(outName, "UDB HILSIM");
    strcpy(outSig, "UDB.HardwareInLoop");
    strcpy(outDesc, "The PC half of the UDB Hardware-In-Loop Simulator");
          
    drP = XPLMFindDataRef("sim/flightmodel/position/P");
	drQ = XPLMFindDataRef("sim/flightmodel/position/Q");
	drR = XPLMFindDataRef("sim/flightmodel/position/R");
    drLat = XPLMFindDataRef("sim/flightmodel/position/latitude");
	drLon = XPLMFindDataRef("sim/flightmodel/position/longitude");
	drElev = XPLMFindDataRef("sim/flightmodel/position/elevation");
	drLocal_ax = XPLMFindDataRef("sim/flightmodel/position/local_ax");
	drLocal_ay = XPLMFindDataRef("sim/flightmodel/position/local_ay"); 
	drLocal_az = XPLMFindDataRef("sim/flightmodel/position/local_az");
	drLocal_vx = XPLMFindDataRef("sim/flightmodel/position/local_vx");
	drLocal_vy = XPLMFindDataRef("sim/flightmodel/position/local_vy"); 
	drLocal_vz = XPLMFindDataRef("sim/flightmodel/position/local_vz");
	drLocalDays = XPLMFindDataRef("sim/time/local_date_days");
	drLocalSecs = XPLMFindDataRef("sim/time/local_time_sec");
	drGroundSpeed = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
	drAirSpeedTrue = XPLMFindDataRef("sim/flightmodel/position/true_airspeed");
	drHeading = XPLMFindDataRef("sim/flightmodel/position/psi");
	drLocal_x = XPLMFindDataRef("sim/flightmodel/position/local_x");
	drLocal_y = XPLMFindDataRef("sim/flightmodel/position/local_y");
	drLocal_z = XPLMFindDataRef("sim/flightmodel/position/local_z");
	drPhi = XPLMFindDataRef("sim/flightmodel/position/phi");
	drTheta = XPLMFindDataRef("sim/flightmodel/position/theta");
    drPsi = XPLMFindDataRef("sim/flightmodel/position/psi");
	drAlpha = XPLMFindDataRef("sim/flightmodel/position/alpha");
	drBeta = XPLMFindDataRef("sim/flightmodel/position/beta");
	drOverRide = XPLMFindDataRef("sim/operation/override/override_flightcontrol");
	drThrOverRide = XPLMFindDataRef("sim/operation/override/override_throttles");
	drPitchAxis = XPLMFindDataRef("sim/joystick/FC_ptch");
	drRollAxis = XPLMFindDataRef("sim/joystick/FC_roll");
	drYawAxis = XPLMFindDataRef("sim/joystick/FC_hdng");
	drThro = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");

	fTextColour[0] = 1.0;
	fTextColour[1] = 1.0;
	fTextColour[2] = 1.0;

    XPLMRegisterFlightLoopCallback(SerialPortAccessCB, 1.0, NULL);
	XPLMRegisterFlightLoopCallback(GetBodyRates, 1.0, NULL);
	XPLMRegisterDrawCallback(
					DrawStrings,
					xplm_Phase_Window, 	/* Draw when sim is doing objects */
					0,						/* After objects */
					NULL);					/* No refcon needed */
	
	XPLMSetDatai(drOverRide, 1);

	return 1;
}

int DrawStrings(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
	XPLMDrawString(fTextColour,300,740,szString,NULL,xplmFont_Basic);
	return 1;
}

PLUGIN_API void		XPluginStop(void)
{
}

PLUGIN_API void		XPluginDisable(void)
{
    CloseComms();
	XPLMSetDatai(drOverRide, 0);
	XPLMSetDatai(drThrOverRide, 0);
}

PLUGIN_API int		XPluginEnable(void)
{
    OpenComms();
	XPLMSetDatai(drOverRide, 1);
	XPLMSetDatai(drThrOverRide, 1);
    return 1;
}

PLUGIN_API void		XPluginReceiveMessage(
                    XPLMPluginID    inFromWho,
                    long            inMessage,
                    void *            inParam)
{
}

float GetBodyRates(float elapsedMe, float elapsedSim, int counter, void * refcon)
{
   	union intbb Temp2;
	float phi, theta, psi;
	float alpha, beta;
	float Cr, Cp, Cy, Ca, Cb;
	float Sr, Sp, Sy, Sa, Sb;
	float ax_NED, ay_NED, az_NED, mag_NED;
	float plane_ax, plane_ay, plane_az, plane_mag;
	float P_flight, Q_flight, R_flight;
	float P_plane, Q_plane, R_plane;
	
	// Angular rates in X-Plane are specified relative to the flight path, not to the aircraft,
	// for reasons unknown. So that means we need to rotate by alpha and beta to get angular rates
	// in the aircraft body frame, which is what the UDB measures.
	
	// Retrieve rates and slip angles, and convert to radians
	//P_flight = XPLMGetDataf(drP) / 180 * PI;
	//Q_flight = XPLMGetDataf(drQ) / 180 * PI;
	//R_flight = XPLMGetDataf(drR) / 180 * PI;
	alpha = XPLMGetDataf(drAlpha) / 180 * PI;
	beta = XPLMGetDataf(drBeta) / 180 * PI;
	
	Ca = cos(alpha);
	Cb = cos(beta);
	Sa = sin(alpha);
	Sb = sin(beta);
	
	//	Create test vector along plane longitudinal axis, but in "flight" frame, so we can convert it back and see if
	//	the transformation works
	P_flight = 1;
	Q_flight = 0;
	R_flight = 0;
	
	P_plane = (P_flight * Ca * Cb) + (R_flight * Sa * Sb) - (Q_flight * Sb);
	Q_plane = (P_flight * Ca * Sb) - (R_flight * Sa * Cb) + (Q_flight * Cb);
	R_plane = (P_flight * Sa) + (Q_flight * Ca);
	
	sprintf(szString,"P_plane: %09.4f,\tQ_plane: %09.4f,\tR_Plane: %09.4f\0", P_plane, Q_plane, R_plane);

	// Angular rate -> convert to rad/s
	// multiply by 5632 (constant from UDB code)
	// Divide by SCALEGYRO(3.0 for red board)
	// 1 * 5632 / 3.0 = 1877.33
	Temp2.BB = (int)(P_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[6], Temp2);
	Temp2.BB = (int)(Q_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[8], Temp2);
	Temp2.BB = (int)(R_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[10], Temp2);
	
	//Accelerations in X-Plane are expressed in the local OpenGL reference frame, for whatever reason. 
	//This coordinate system is defined as follows (taken from the X-Plane SDK Wiki):
	
	//	The origin 0,0,0 is on the surface of the earth at sea level at some "reference point".
	//	The +X axis points east from the reference point.
	//	The +Z axis points south from the reference point.
	//	The +Y axis points straight up away from the center of the earth at the reference point.
	
	// First we shall convert from this East Up South frame, to a more conventional NED (North East Down) frame.

	ax_NED = (-1 * XPLMGetDataf(drLocal_az));
	ay_NED = (XPLMGetDataf(drLocal_ax));
	az_NED = (-1 * XPLMGetDataf(drLocal_ay)) + (float)9.8; 

	// Our euler angles

	// X-Plane angles are in degrees.
	// Phi is roll, roll to right is positive
	// Theta is pitch, pitch up is positive
	// Psi is yaw, relative to north. North is 0/360.
	
	// Convert these angles to radians first.

	phi = (XPLMGetDataf(drPhi)) / 180 * PI;
	theta = (XPLMGetDataf(drTheta)) / 180 * PI;
	psi = (XPLMGetDataf(drPsi)) / 180 * PI;

	// Next calculate cos & sin of each for use in the transformation matrix.
	// r, p & y subscripts stand for roll pitch and yaw.

	Cr = cos(phi);
	Cp = cos(theta);
	Cy = cos(psi);
	Sr = sin(phi);
	Sp = sin(theta);
	Sy = sin(psi);

	// Next we need to rotate our accelerations from the NED reference frame, into the body fixed reference frame
	// Somewhere here i think i need to add centripetal acceleration, as the aircraft body frame is rotating with 
	// respect to the local OpenGL frame we started in.

	//*************************************************
	// !!!	TO DO: Centripetal acceleration addition
	//*************************************************
	
	// THANKS TO GEORGE M SIOURIS WHOSE "MISSILE GUIDANCE AND CONTROL SYSTEMS" BOOK SEEMS TO BE THE ONLY EASY TO FIND REFERENCE THAT
	// ACTUALLY GETS THE NED TO BODY FRAME ROTATION MATRIX CORRECT!!
	
	// CpCy, CpSy, -Sp					| local_ax
	// SrSpCy-CrSy, SrSpSy+CrCy, SrCp	| local_ay
	// CrSpCy+SrSy, CrSpSy-SrCy, CrCp	| local_az
	
	plane_ax = (ax_NED * Cp * Cy) + (ay_NED * Cp * Sy) - (az_NED * Sp);
	plane_ay = (ax_NED * ((Sr * Sp * Cy)-(Cr * Sy))) + (ay_NED * ((Sr * Sp * Sy)+(Cr * Cy))) + (az_NED * Sr * Cp);
	plane_az = (ax_NED * ((Cr * Sp * Cy)+(Sr * Sy))) + (ay_NED * ((Cr * Sp * Sy)-(Sr * Cy))) + (az_NED * Cr * Cp);
	
	mag_NED = (ax_NED * ax_NED) + (ay_NED * ay_NED) + (az_NED * az_NED);
	plane_mag = (plane_ax * plane_ax) + (plane_ay * plane_ay) + (plane_az * plane_az);
	
	//sprintf(szString,"plane_ax: %09.4f,\tplane_ay: %09.4f,\tplane_az: %09.4f\tlocal_mag: %09.4f\tplane_mag: %09.4f\0", plane_ax * 204.8,plane_ay* 204.8,plane_az* 204.8, mag_NED, plane_mag);
	//sprintf(szString,"phi: %09.4f,\ttheta: %09.4f,\tpsi: %09.4f\0", phi, theta, psi);

	// Lastly we need to convert from X-Plane units (m/s^2) to the arbitrary units used by the UDB
	
	// Accelerations are in m/s^2
	// Divide by 9.8 to get g's
	// Multiply by 5280 (constant from UDB code)
	// Divide by SCALEACCEL (2.64 for red board)
	// 1 / 9.8 * 5280 / 2.64 = 204.8
		
	Temp2.BB = (int)(plane_ax * 204.8);
	Store2LE(&NAV_BODYRATES[12], Temp2);
	Temp2.BB = (int)(plane_ay * 204.8);
	Store2LE(&NAV_BODYRATES[14], Temp2);
	Temp2.BB = (int)(plane_az * 204.8);
	Store2LE(&NAV_BODYRATES[16], Temp2);
	
	CalculateChecksum(NAV_BODYRATES);
	SendToComPort(sizeof(NAV_BODYRATES),NAV_BODYRATES);

	ReceiveFromComPort();
	
	Temp2._.B1 = SERVO_IN[0];
	Temp2._.B0 = SERVO_IN[1];
	float servoCh1 = (float)(Temp2.BB - 3000);
	servoCh1 /= 1000;
	
	Temp2._.B1 = SERVO_IN[2];
	Temp2._.B0 = SERVO_IN[3];
	float servoCh2 = (float)(Temp2.BB - 3000);
	servoCh2 /= 1000;
	
	Temp2._.B1 = SERVO_IN[4];
	Temp2._.B0 = SERVO_IN[5];
	float servoCh3 = (float)(Temp2.BB - 3000);
	servoCh3 /= 1000;
	
	Temp2._.B1 = SERVO_IN[6];
	Temp2._.B0 = SERVO_IN[7];
	float servoCh4 = (float)(Temp2.BB - 3000);
	servoCh4 /= 1000;
	
	Temp2._.B1 = SERVO_IN[8];
	Temp2._.B0 = SERVO_IN[9];
	float servoCh5 = (float)(Temp2.BB - 3000);
	servoCh5 /= 1000;
	
	//sprintf(szString,"ch1: %f,\tch2: %f,\tch3: %f,\tch4: %f,\tch5: %f,\t\0", servoCh1, servoCh2, ((servoCh3+0.4)/1.2), servoCh4, servoCh5);
	XPLMSetDataf(drPitchAxis, servoCh4);
	XPLMSetDataf(drRollAxis, servoCh2);
	XPLMSetDataf(drYawAxis, (-1 * servoCh1));
	servoCh3 = (servoCh3 + (float)0.4) / (float)1.2;
	float throttle[8] = {servoCh3, servoCh3, servoCh3, servoCh3, servoCh3, servoCh3, servoCh3, servoCh3};
	XPLMSetDatavf(drThro, throttle,0,8);
	
	return -1;
}

float SerialPortAccessCB(float elapsedMe, float elapsedSim, int counter, void * refcon)
{
    GetGPSData();
    return 0.25;
}

void GetGPSData(void)
{
	union longbbbb Temp4;
	union intbb Temp2;
	
	int LocalDays = XPLMGetDatai(drLocalDays);
	float LocalSecsFloat = XPLMGetDataf(drLocalSecs) * 1000;
	
	LocalDays += 5;
	int Week = (int)(LocalDays / 7) + 1564;
	LocalDays = (LocalDays % 7);
	Week = (Week * 10) + LocalDays;
	int LocalSecsInt = (int)LocalSecsFloat + (LocalDays * 86400000);
	LocalSecsFloat = (LocalSecsFloat - (int)LocalSecsFloat) * 1000000;
	
	Temp2.BB = Week;
	Store2LE(&NAV_SOL[14], Temp2);

	Temp4.WW = LocalSecsInt;
	Store4LE(&NAV_SOL[6], Temp4);
	Store4LE(&NAV_DOP[6], Temp4);
	Store4LE(&NAV_POSLLH[6], Temp4);
	Store4LE(&NAV_VELNED[6], Temp4);

	Temp4.WW = (int)LocalSecsFloat;
	Store4LE(&NAV_SOL[10], Temp4);

	double latitude = XPLMGetDataf(drLat);
	double longitude = XPLMGetDataf(drLon);
	double elevation = XPLMGetDataf(drElev);
	double local_x = XPLMGetDataf(drLocal_x);
	double local_y = XPLMGetDataf(drLocal_y);
	double local_z = XPLMGetDataf(drLocal_z);
	double local_vx = XPLMGetDataf(drLocal_vx);
	double local_vy = XPLMGetDataf(drLocal_vy);
	double local_vz = XPLMGetDataf(drLocal_vz);

	Temp4.WW = (int)(local_vx * 100);
	Store4LE(&NAV_VELNED[14], Temp4);
	
	Temp4.WW = (int)(local_vy * -100);
	Store4LE(&NAV_VELNED[18], Temp4);
	
	Temp4.WW = (int)(local_vz * -100);
	Store4LE(&NAV_VELNED[10], Temp4);

	Temp4.WW = (int)(XPLMGetDataf(drAirSpeedTrue) * 100);
	Store4LE(&NAV_VELNED[22], Temp4);

	Temp4.WW = (int)(XPLMGetDataf(drGroundSpeed) * 100);
	Store4LE(&NAV_VELNED[26], Temp4);

	Temp4.WW = (int)(XPLMGetDataf(drHeading) * 100000);
	Store4LE(&NAV_VELNED[30], Temp4);

	Temp4.WW = (int)(latitude * 10000000);
	Store4LE(&NAV_POSLLH[14], Temp4);

	Temp4.WW = (int)(longitude * 10000000);
	Store4LE(&NAV_POSLLH[10], Temp4);
	
	Temp4.WW = (int)(elevation * 1000);
	Store4LE(&NAV_POSLLH[18], Temp4);
	Store4LE(&NAV_POSLLH[22], Temp4);
	
	double ac_pos_lat, ac_pos_lon, ac_pos_elev;
	double ac_vel_lat, ac_vel_lon, ac_vel_elev;
	
	// Get AC pos in LLA
	XPLMLocalToWorld(	local_x, 
						local_y, 
						local_z, 
						&ac_pos_lat, 
						&ac_pos_lon, 
						&ac_pos_elev	);
	
	// Get AC pos + velocity vector in LLA
	XPLMLocalToWorld(	local_x + local_vx, 
						local_y + local_vy, 
						local_z + local_vz,
						&ac_vel_lat,
						&ac_vel_lon,
						&ac_vel_elev	);
	
	// convert to ECEF
	LLAtoECEF(ac_pos_lat, ac_pos_lon, ac_pos_elev, local_x, local_y, local_z);
	LLAtoECEF(ac_vel_lat, ac_vel_lon, ac_vel_elev, local_vx, local_vy, local_vz);
	
	// AC pos stays as is
	// subtract to get velocity vector in ECEF
	local_vy -= local_y;
	local_vx -= local_x;
	local_vz -= local_z;

	Temp4.WW = (int)(local_x * 100);
	Store4LE(&NAV_SOL[18], Temp4);
	
	Temp4.WW = (int)(local_y * 100);
	Store4LE(&NAV_SOL[22], Temp4);
	
	Temp4.WW = (int)(local_z * 100);
	Store4LE(&NAV_SOL[26], Temp4);

	Temp4.WW = (int)(local_vx * 100);
	Store4LE(&NAV_SOL[34], Temp4);
	
	Temp4.WW = (int)(local_vy * 100);
	Store4LE(&NAV_SOL[38], Temp4);
	
	Temp4.WW = (int)(local_vz * 100);
	Store4LE(&NAV_SOL[42], Temp4);

	CalculateChecksum(NAV_SOL);
	SendToComPort(sizeof(NAV_SOL),NAV_SOL);
	CalculateChecksum(NAV_DOP);
	SendToComPort(sizeof(NAV_DOP),NAV_DOP);
	CalculateChecksum(NAV_POSLLH);
	SendToComPort(sizeof(NAV_POSLLH),NAV_POSLLH);
	CalculateChecksum(NAV_VELNED);
	SendToComPort(sizeof(NAV_VELNED),NAV_VELNED);

}

//---------------------------------------------------------------------------
void OpenComms(void)
{
    DCB Dcb;
    COMMTIMEOUTS CommTimeouts;
    char ErrorString[80];
    char *PortString = "\\\\.\\COM4";

    if (hComms == 0)
    {
        //PortString[7] = (char) 13 + 48;
        hComms = CreateFile(PortString,
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                            NULL);

        if (hComms == INVALID_HANDLE_VALUE)
        {
            sprintf(ErrorString, "CreateFile Error = %d", GetLastError());
            ShowMessage(ErrorString);
        }
        else
        {

            dwRetFlag = GetCommState(hComms, &Dcb);

            if (!dwRetFlag)
            {
                sprintf(ErrorString, "GetCommState Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }

            Dcb.DCBlength = sizeof(Dcb);

            Dcb.BaudRate = CBR_19200;

            Dcb.ByteSize = 8;
            Dcb.Parity = NOPARITY;
            Dcb.StopBits = ONESTOPBIT;
            Dcb.fTXContinueOnXoff = TRUE;

            Dcb.fOutxCtsFlow = FALSE;//TRUE;                  // disable CTS output flow control
            Dcb.fOutxDsrFlow = FALSE;                  // disable DSR output flow control
            Dcb.fDtrControl = DTR_CONTROL_HANDSHAKE  /*DTR_CONTROL_DISABLE DTR_CONTROL_ENABLE*/;
            Dcb.fDsrSensitivity = FALSE;               // enable DSR sensitivity

            Dcb.fOutX = FALSE;                        // disable XON/XOFF out flow control
            Dcb.fInX = FALSE;                         // disable XON/XOFF in flow control
            Dcb.fErrorChar = FALSE;                   // disable error replacement
            Dcb.fNull = FALSE;                        // disable null stripping
            Dcb.fRtsControl = RTS_CONTROL_HANDSHAKE	  /* RTS_CONTROL_ENABLE  RTS_CONTROL_DISABLE*/;   //  enable RTS line
            Dcb.fAbortOnError = TRUE;                 // don't abort reads/writes on error

            dwRetFlag = SetCommState(hComms, &Dcb);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "SetCommState Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }

            dwRetFlag = GetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "GetCommTimeouts Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }

            CommTimeouts.ReadIntervalTimeout         = MAXDWORD;    //Don't use interval timeouts
            CommTimeouts.ReadTotalTimeoutMultiplier  = 0;    //Don't use multipliers
            CommTimeouts.ReadTotalTimeoutConstant    = 0;    //150ms total read timeout
            CommTimeouts.WriteTotalTimeoutMultiplier = 0; //Don't use multipliers
            CommTimeouts.WriteTotalTimeoutConstant   = 0; //2200ms total write timeout

            dwRetFlag = SetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "SetCommTimeouts Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }
        }
    }
    else
    {
        ShowMessage("Comm port already open");
    }
}
//---------------------------------------------------------------------------

void CloseComms(void)
{
    if (hComms != 0)
    {
        CloseHandle(hComms);
        hComms = 0;
    }
    else
    {
        ShowMessage("Comm port already closed");
    }
}
//---------------------------------------------------------------------------

void SendToComPort(DWORD ResponseLength, unsigned char *Buffer)
{
    DWORD dwBytesWritten;
		
	if (hComms != 0)
        dwRetFlag = WriteFile(hComms, Buffer, ResponseLength, &dwBytesWritten, NULL);
    else
    {
        ShowMessage("Comm port not open");
    }
}
//---------------------------------------------------------------------------
void ReceiveFromComPort(void)
{
	BYTE Byte = 0x00;
	DWORD dwBytesTransferred = 0;
	
	if(hComms != INVALID_HANDLE_VALUE) 
	{
		// Loop for waiting for the data.
		do 
		{
			// Read the data from the serial port.
			if(ReadFile (hComms, &Byte, 1, &dwBytesTransferred, 0))
			{
				(* msg_parse) ( Byte ) ;
			}
			else
			{
				COMSTAT comStat;
				DWORD   dwErrors;
				BOOL    fOOP, fOVERRUN, fPTO, fRXOVER, fRXPARITY, fTXFULL;
				BOOL    fBREAK, fDNS, fFRAME, fIOE, fMODE;

				// Get and clear current errors on the port.
				if (!ClearCommError(hComms, &dwErrors, &comStat))
					// Report error in ClearCommError.
					return;

				// Get error flags.
				fDNS = dwErrors & CE_DNS;
				fIOE = dwErrors & CE_IOE;
				fOOP = dwErrors & CE_OOP;
				fPTO = dwErrors & CE_PTO;
				fMODE = dwErrors & CE_MODE;
				fBREAK = dwErrors & CE_BREAK;
				fFRAME = dwErrors & CE_FRAME;
				fRXOVER = dwErrors & CE_RXOVER;
				fTXFULL = dwErrors & CE_TXFULL;
				fOVERRUN = dwErrors & CE_OVERRUN;
				fRXPARITY = dwErrors & CE_RXPARITY;

				/* The only reason i left these if statements in was so i had
				somewhere to set breakpoints when debugging the serial port stuff.


				// COMSTAT structure contains information regarding
				// communications status.
				if (comStat.fCtsHold);
					// Tx waiting for CTS signal
				if (comStat.fDsrHold);
					// Tx waiting for DSR signal
				if (comStat.fRlsdHold);
					// Tx waiting for RLSD signal
				if (comStat.fXoffHold);
					// Tx waiting, XOFF char rec'd
				if (comStat.fXoffSent);
					// Tx waiting, XOFF char sent
				if (comStat.fEof);
					// EOF character received
				if (comStat.fTxim);
					// Character waiting for Tx; char queued with TransmitCommChar
				if (comStat.cbInQue);
					// comStat.cbInQue bytes have been received, but not read
				if (comStat.cbOutQue);
					// comStat.cbOutQue bytes are awaiting transfer	

					*/
			}
		} while ((dwBytesTransferred == 1) && (gotPacket < 2));
		gotPacket = 0;
	}
	return;
}

//---------------------------------------------------------------------------
void ShowMessage(char *pErrorString)
{
    MessageBox(NULL, pErrorString, "Error", MB_OK);
}

void CalculateChecksum(unsigned char *msg)
{
	// length is technically 2 bytes (in little endian order) but we're 
	// not using any messages over 256 bytes long, so we can get away 
	// with just looking at the first byte.

	int length = msg[4];
	int i;
	int CK_A = 0;
	int CK_B = 0;
	for(i = 2 ; i < length + 6 ; i++)
	{
		CK_A += msg[i];
		CK_B += CK_A;
	}	
	msg[length + 6] = CK_A;
	msg[length + 7] = CK_B;
	return;
}



void Store4LE(unsigned char *store, union longbbbb data)
{
	store[0] = data.__.B0;
	store[1] = data.__.B1;
	store[2] = data.__.B2;
	store[3] = data.__.B3;
}

void Store2LE(unsigned char *store, union intbb data)
{
	store[0] = data._.B0;
	store[1] = data._.B1;
}

void msgDefault ( unsigned char rxChar )
{
	if ( rxChar == 0xFF )
	{
		msg_parse = &msgSync1 ;
	}
	else
	{
		// error condition
	}
	return ;
}


void msgSync1 ( unsigned char rxChar )
{
	
	if ( rxChar == 0xEE )
	{
		store_index = 0 ;
		ck_calc_a = ck_calc_b = 0;
		msg_parse = &msgServos ;
	}
	else if ( rxChar == 0xFF )
	{
		// do nothing
	}
	else
	{
		msg_parse = &msgDefault;	// error condition
	}
	return ;
}

void	msgServos(unsigned char rxChar)
{
		if(store_index < SERVO_MSG_LENGTH)
		{
			SERVO_IN_[store_index++] = rxChar;
			ck_calc_a += rxChar;
			ck_calc_b += ck_calc_a;
		}
		else
		{
			ck_in_a = rxChar;
			msg_parse = &msgCheckSum;
		}
}

void	msgCheckSum(unsigned char rxChar)
{
	ck_in_b = rxChar;
	if((ck_in_a == ck_calc_a) && (ck_in_b == ck_calc_b))
	{
		memcpy(SERVO_IN,SERVO_IN_,sizeof(SERVO_IN_));
		gotPacket++;
	}
	msg_parse = &msgDefault;
}
