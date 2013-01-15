#include "stdafx.h"
#include "SerialIO.h"

#define MAX_ITEMS 30;

bool CommsEnabled;
float fTextColour[3];
char szString[100];

XPLMHotKeyID	gHotKey = NULL;

void	MyHotKeyCallback(void *               inRefcon);    
int 	MyOrbitPlaneFunc(
                                   XPLMCameraPosition_t * outCameraPosition,  
                                   int                  inIsLosingControl,    
                                   void *               inRefcon);  

int	MyDrawCallback(
                                   XPLMDrawingPhase     inPhase,    
                                   int                  inIsBefore,    
                                   void *               inRefcon);

// Here are the variables for implementing the file based control of the setup
Channels ControlSurfaces;							// The list of control surfaces
string	CommPortString = "\\\\.\\COM4";				// Pnace to put the port string to open, defaults to COM4
long	CommPortSpeed = 19200;
string  OverString = "sim/operation/override/override_flightcontrol";
													// Defaults to standard joystick control
float	ThrottleSettings[8] = {0,0,0,0,0,0,0,0};	// The throttle settings with default values

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


void msgVarSize ( unsigned char rxChar );	// Parser for variable count data byte
void msgVarServos(unsigned char rxChar);	// Receive channel data of variable length
void msgSync2 ( unsigned char rxChar );		// Parser for second byte of variable channel count message
unsigned char var_channel_count;			// Number of channels to be recieved

unsigned char ck_in_a, ck_in_b, ck_calc_a, ck_calc_b;

void	SetupDefaultServoZeros(void);
void	ServosToControls();

int store_index = 0;

void	(* msg_parse) (unsigned char rxChar) = &msgDefault;

#define SERVO_MSG_LENGTH (2*FIXED_SERVO_CHANNELS)

// Servo offsets are a variable so that they can be actively zeroed from the real received offsets
// This offset zero will need to be a menu add on to the plugin.
intbb  ServoOffsets[MAX_VARIABLE_CHANNELS];

unsigned char SERVO_IN[MAX_VARIABLE_CHANNELS*2];

unsigned char SERVO_IN_[MAX_VARIABLE_CHANNELS*2];

int rxCount = 0;


// At 40hz, this message will be ~6500bps

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

	float CamYaw, CamPitch, CamRoll;
	float CamX, CamY, CamZ;
	float P_plane, Q_plane, R_plane;
	float ax_NED, ay_NED, az_NED, mag_NED;
	float ax_plane, ay_plane, az_plane;

	#define CamPathLength 50000
	float CamPath[CamPathLength][3];
	int CamPathIterator = 0;
	int CamPathCount = 0;

void GetGPSData(void);

PLUGIN_API int XPluginStart(
                        char *        outName,
                        char *        outSig,
                        char *        outDesc)
{
    strcpy(outName, "UDB HILSIM");
    strcpy(outSig, "UDB.HardwareInLoop");
    strcpy(outDesc, "UDB Hardware-In-Loop Simulator");
          
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
	drThrOverRide = XPLMFindDataRef("sim/operation/override/override_throttles");
	drPitchAxis = XPLMFindDataRef("sim/joystick/FC_ptch");
	drRollAxis = XPLMFindDataRef("sim/joystick/FC_roll");
	drYawAxis = XPLMFindDataRef("sim/joystick/FC_hdng");
	drThro = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");

	fTextColour[0] = 1.0;
	fTextColour[1] = 1.0;
	fTextColour[2] = 1.0;

	XPLMRegisterFlightLoopCallback(GetBodyRates, 1.0, NULL);
	XPLMRegisterDrawCallback(
					DrawStrings,
					xplm_Phase_Window, 	/* Draw when sim is doing objects */
					0,						/* After objects */
					NULL);					/* No refcon needed */
	
//	XPLMSetDatai(drOverRide, 1);		// Now overide surfaces, not yaw,roll,pitch

	/* Register our hot key for the new view. */
	gHotKey = XPLMRegisterHotKey(XPLM_VK_F8, xplm_DownFlag, 
				"Circling External View",
				MyHotKeyCallback,
				NULL);

	XPLMRegisterDrawCallback(
				MyDrawCallback,	
				xplm_Phase_Objects, 	/* Draw when sim is doing objects */
				0,						/* After objects */
				NULL);					/* No refcon needed */

	memset(&CamPath, 0, sizeof(float) * 3 * CamPathLength);

	return 1;
}

int DrawStrings(XPLMDrawingPhase inPhase, int inIsBefore, void *inRefcon)
{
	XPLMDrawString(fTextColour,300,740,szString,NULL,xplmFont_Basic);
	return 1;
}

PLUGIN_API void		XPluginStop(void)
{
		XPLMUnregisterHotKey(gHotKey);
		XPLMUnregisterDrawCallback(
				MyDrawCallback,
				xplm_Phase_LastCockpit, 
				0,
				NULL);	
}

PLUGIN_API void		XPluginDisable(void)
{
    CloseComms();

	XPLMSetDatai(drOverRide, 0);				// Clear the overides
	XPLMSetDatai(drThrOverRide, 0);
}

PLUGIN_API int		XPluginEnable(void)
{
	// Load the setup file on enable.  This allows the user to modify the file without exit of XPlane
	SetupFile Setup;
	Setup.LoadSetupFile(ControlSurfaces, CommPortString, CommPortSpeed, OverString);	// Open the setup file and parse it into the control surface list

	OpenComms();

	SetupDefaultServoZeros();											// Setup the servo defaults.

	drOverRide = XPLMFindDataRef(OverString.data());					// Get the latest overide reference
	XPLMSetDatai(drOverRide, 1);										// Overide from the setup file
	XPLMSetDatai(drThrOverRide, 1);
	memset(&CamPath, 0, sizeof(float) * 3 * CamPathLength);
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
	float P_flight, Q_flight, R_flight;
	float ax, ay, az;

	
	// Angular rates in X-Plane are specified relative to the flight path, not to the aircraft,
	// for reasons unknown. So that means we need to rotate by alpha and beta to get angular rates
	// in the aircraft body frame, which is what the UDB measures.
	
 // Retrieve rates and slip angles, and convert to radians
    P_flight = XPLMGetDataf(drP) / 180 * PI ;
    Q_flight = XPLMGetDataf(drQ) / 180 * PI ;
    R_flight = XPLMGetDataf(drR) / 180 * PI ;
    alpha = XPLMGetDataf(drAlpha) / 180 * PI;
    beta = XPLMGetDataf(drBeta) / 180 * PI;

	FLIGHTtoBCBF(P_flight, Q_flight, R_flight, alpha, beta);
    
	P_plane = P_flight;
	Q_plane = Q_flight;
	R_plane = R_flight;
	
	// Angular rate
	// multiply by 5632 (constant from UDB code)
	// Divide by SCALEGYRO(3.0 for red board)
	// 1 * 5632 / 3.0 = 1877.33
	Temp2.BB = (int)(P_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[6], Temp2);
	Temp2.BB = (int)(Q_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[8], Temp2);
	Temp2.BB = (int)(R_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[10], Temp2);

	// Our euler angles:
	// X-Plane angles are in degrees.
	// Phi is roll, roll to right is positive
	// Theta is pitch, pitch up is positive
	// Psi is yaw, relative to north. North is 0/360.
	
	// Convert these angles to radians first.

	phi =(XPLMGetDataf(drPhi)) / 180 * PI * -1.0;
    theta = (XPLMGetDataf(drTheta)) / 180 * PI;
    psi = (XPLMGetDataf(drPsi)) / 180 * PI * -1.0;
	
	// Get accelerations in OpenGL coordinate frame
	//ax = XPLMGetDataf(drLocal_ax);
    //ay = XPLMGetDataf(drLocal_ay);
    //az = XPLMGetDataf(drLocal_ay); 

	ax = 0;
	ay = 0;
	az = 0;

	// Gravity is not included in ay, we need to add it. OGL y axis is +ve up,
	// so g is -9.8.
	ay -= (float)9.8;

	// Convert from OGL frame to Aircraft body fixed frame
	OGLtoBCBF(ax, ay, az, phi, theta, psi);

	ax_plane = ax;
	ay_plane = ay;
	az_plane = az;

	// Lastly we need to convert from X-Plane units (m/s^2) to the arbitrary units used by the UDB
	
	// Accelerations are in m/s^2
	// Divide by 9.8 to get g's
	// Multiply by 5280 (constant from UDB code)
	// Divide by SCALEACCEL (2.64 for red board)
	// 1 / 9.8 * 5280 / 2.64 = 204.8
		
	Temp2.BB = (int)(ax * 204.8);
	Store2LE(&NAV_BODYRATES[12], Temp2);
	Temp2.BB = (int)(ay * 204.8);
	Store2LE(&NAV_BODYRATES[14], Temp2);
	Temp2.BB = (int)(az * 204.8);
	Store2LE(&NAV_BODYRATES[16], Temp2);
	
	CalculateChecksum(NAV_BODYRATES);
	SendToComPort(sizeof(NAV_BODYRATES),NAV_BODYRATES);

	ReceiveFromComPort();

	ServosToControls();

//	float ThrottleSetting = 0;	//SurfaceDeflections[CHANNEL_THROTTLE];
//	float throttle[8] = {ThrottleSetting, ThrottleSetting, ThrottleSetting, ThrottleSetting, 
//											ThrottleSetting, ThrottleSetting, ThrottleSetting, ThrottleSetting};

	XPLMSetDatavf(drThro, ThrottleSettings,0,8);
	
	return -1;
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


void HandleMsgByte(char b)
{
	(* msg_parse) ( b ) ;
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
	switch(rxChar)
	{
	case 0xFF:
		msg_parse = &msgSync1 ;		// Fixed size channel count message
		break;
	case 0xFE:
		msg_parse = &msgSync2 ;		// Variable size channel count message
		break;
	}
	return ;
}

// Parser for second byte of variable channel count message
void msgSync2 ( unsigned char rxChar )
{
	switch(rxChar)
	{
	case 0xEF:
		msg_parse = &msgVarSize;	// Next char is variable size
		break;
	default:
		msg_parse = &msgDefault;	// Faulty start
		break;
	};
}

// Parser for variable count data byte
void msgVarSize ( unsigned char rxChar )
{
	switch( rxChar )
	{
	case 0xFF:
	case 0xFE:
	case 0xEF:
		msg_parse = &msgDefault;	// Faulty value
		break;
	default:
		if(var_channel_count > MAX_VARIABLE_CHANNELS)
		{
			msg_parse = &msgDefault;	// Faulty start
			return;
		}
		var_channel_count = rxChar;
		msg_parse = &msgVarServos;	// Next char is variable size
		store_index = 0 ;
		ck_calc_a = ck_calc_b = 0;
		break;
	}
}



void msgVarServos(unsigned char rxChar)
{
	if(store_index < (var_channel_count*2))
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



void msgSync1 ( unsigned char rxChar )
{
	
	switch(rxChar)
	{
	case 0xEE:
		{
		store_index = 0 ;
		ck_calc_a = ck_calc_b = 0;
		msg_parse = &msgServos ;
		} break;
	case 0xFF:
		break;						// do nothing
	default: 
		msg_parse = &msgDefault;	// error condition
		break;
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

static int GPSCount = 0;

void	msgCheckSum(unsigned char rxChar)
{
	ck_in_b = rxChar;
	if((ck_in_a == ck_calc_a) && (ck_in_b == ck_calc_b))
	{
		memcpy(SERVO_IN,SERVO_IN_,sizeof(SERVO_IN_));
		
		GPSCount++;
		if (GPSCount % 10 == 0)
		{
			GetGPSData();
			GPSCount = 0;
		}
	}

	msg_parse = &msgDefault;
}

/****************************************************************************************/
// Step through list of controls reading the servo channel, calculating new position 
// and setting either a surface or engine(s) to the value
void ServosToControls()
{
	int		iIndex = 0;
	int		iSize = ControlSurfaces.size();
	
	int		iServoChannel;
	intbb	ServoValue;

	int		Value;

	float	ControlSetting;

	ChannelSetup* pScanSetup;

	for(iIndex = 0; iIndex < iSize; iIndex++)
	{
		pScanSetup = &ControlSurfaces[iIndex];

		iServoChannel = pScanSetup->mServoChannel;
		ServoValue._.B1 = SERVO_IN[2*iServoChannel];
		ServoValue._.B0 = SERVO_IN[(2*iServoChannel)+1];

		ControlSetting = pScanSetup->GetControlDeflection(ServoValue.BB);
		
		if(pScanSetup->mControlType == CONTROL_TYPE_SURFACE)
		{
			XPLMSetDataf(pScanSetup->mControlSurfaceRef, ControlSetting);
		}
		else if(pScanSetup->mControlType == CONTROL_TYPE_ENGINE)
		{
			unsigned int Mask = pScanSetup->mEngineMask;

			for (int Engine = 0; Engine < 8; Engine++)
			{
				if( (Mask & 0x01) != 0 )
				{
					ThrottleSettings[Engine] = ControlSetting;
				}
				Mask >>= 1;
			}
		}
	}
}


// Set all control channels in the list to the default servo position
// Note, if there is more than one control surface per channel, it will reset it
// to the last surface defined in the list
void	SetupDefaultServoZeros(void)
{
	int		iIndex = 0;
	int		iSize = ControlSurfaces.size();
	
	int		iServoChannel;
	intbb	ServoValue;

	ChannelSetup* pScanSetup;

	for(iIndex = 0; iIndex < iSize; iIndex++)
	{
		pScanSetup = &ControlSurfaces[iIndex];

		ServoValue.BB = pScanSetup->mChannelOffset;
		iServoChannel = pScanSetup->mServoChannel;

		SERVO_IN[2*iServoChannel] = ServoValue._.B1;
		SERVO_IN[(2*iServoChannel)+1] = ServoValue._.B0;
	};
}

void	MyHotKeyCallback(void *               inRefcon)
{
	/* This is the hotkey callback.  First we simulate a joystick press and
	 * release to put us in 'free view 1'.  This guarantees that no panels
	 * are showing and we are an external view. */
	XPLMCommandButtonPress(xplm_joy_v_fr1);
	XPLMCommandButtonRelease(xplm_joy_v_fr1);
	
	/* Now we control the camera until the view changes. */
	XPLMControlCamera(xplm_ControlCameraUntilViewChanges, MyOrbitPlaneFunc, NULL);
}

/*
 * MyOrbitPlaneFunc
 * 
 * This is the actual camera control function, the real worker of the plugin.  It is 
 * called each time X-Plane needs to draw a frame.
 * 
 */
int 	MyOrbitPlaneFunc(
                                   XPLMCameraPosition_t * outCameraPosition,   
                                   int                  inIsLosingControl,    
                                   void *               inRefcon)
{
	if (outCameraPosition && !inIsLosingControl)
	{
			int	w, h, x, y;
			float dx, dz, dy, heading, pitch;
		
		/* First get the screen size and mouse location.  We will use this to decide
		 * what part of the orbit we are in.  The mouse will move us up-down and around. */
		XPLMGetScreenSize(&w, &h);
		XPLMGetMouseLocation(&x, &y);
		heading = 2 * PI * (((float) x / (float) w)-0.5);
		pitch = PI * (((float) y / (float) h) - 0.5);
		
		double local_x	= XPLMGetDataf(drLocal_x);
		double local_y	= XPLMGetDataf(drLocal_y);
		double local_z	= XPLMGetDataf(drLocal_z);
		float phi		= XPLMGetDataf(drPhi);
		float theta		= XPLMGetDataf(drTheta);
		float psi		= XPLMGetDataf(drPsi);
		
		// Convert this vector back into angles.
		float CamPitchOGL = (atan2( CamY , sqrt( (CamX * CamX) + (CamZ * CamZ) ))) / PI * 180;
		float CamYawOGL = (atan2( CamX , CamZ * (float)-1.0 )) / PI * 180;
		
		//sprintf(szString,"CamPitch: %f,\tCamYaw: %f,\tCamX: %f,\tCamY: %f,\tCamZ: %f,\0", (CamPitch), (CamYaw), CamX, CamY, CamZ);

		// Camera Position On Aircraft
		dx = -1.0;
		dy = 0.0;
		dz = 1.0;

		BCBFtoOGL(dx,dy,dz,phi,theta,psi);

		/* Fill out the camera position info. */
		outCameraPosition->x = local_x + dx;
		outCameraPosition->y = local_y + dy;
		outCameraPosition->z = local_z + dz;
		outCameraPosition->pitch = CamPitchOGL;
		outCameraPosition->heading = CamYawOGL;
		outCameraPosition->roll = 0;		

	}
	
	/* Return 1 to indicate we want to keep controlling the camera. */
	return 1;
}        

int	MyDrawCallback(
                                   XPLMDrawingPhase     inPhase,    
                                   int                  inIsBefore,    
                                   void *               inRefcon)
{
	/* If any data refs are missing, do not draw. */
	if (!drLocal_x || !drLocal_y || !drLocal_z)
		return 1;
		
	/* Fetch the plane's location at this instant in OGL coordinates. */	
	float planeX = XPLMGetDataf(drLocal_x);
	float planeY = XPLMGetDataf(drLocal_y);
	float planeZ = XPLMGetDataf(drLocal_z);

	float phi	= XPLMGetDataf(drPhi) / 180 * PI;
	float theta	= XPLMGetDataf(drTheta) / 180 * PI;
	float psi	= XPLMGetDataf(drPsi) / 180 * PI;

	float alpha = XPLMGetDataf(drAlpha) / 180 * PI;
    float beta	= XPLMGetDataf(drBeta) / 180 * PI;
	
	/* Reset the graphics state.  This turns off fog, texturing, lighting,
	 * alpha blending or testing and depth reading and writing, which
	 * guarantees that our axes will be seen no matter what. */
	XPLMSetGraphicsState(0, 0, 0, 0, 0, 0, 0);

	/* Do the actual drawing.  use GL_LINES to draw sets of discrete lines.
	 * Each one will go 100 meters in any direction from the plane. */
	
	float pointX1 = 0;
	float pointY1 = 0;
	float pointZ1 = 0;
	float pointX2 = 10 * ax_plane;
	float pointY2 = 0;
	float pointZ2 = 0;
	
	BCBFtoOGL(pointX1, pointY1, pointZ1, phi, theta, psi);
	BCBFtoOGL(pointX2, pointY2, pointZ2, phi, theta, psi);
	
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(pointX1 + planeX, pointY1 + planeY, pointZ1 + planeZ);
	glVertex3f(pointX2 + planeX, pointY2 + planeY, pointZ2 + planeZ);
	glEnd();
		
	pointX1 = 0;
	pointY1 = 0;
	pointZ1 = 0;
	pointX2 = 0;
	pointY2 = 10 * ay_plane;
	pointZ2 = 0;
	
	BCBFtoOGL(pointX1, pointY1, pointZ1, phi, theta, psi);
	BCBFtoOGL(pointX2, pointY2, pointZ2, phi, theta, psi);
	
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(pointX1 + planeX, pointY1 + planeY, pointZ1 + planeZ);
	glVertex3f(pointX2 + planeX, pointY2 + planeY, pointZ2 + planeZ);
	glEnd();

	pointX1 = 0;
	pointY1 = 0;
	pointZ1 = 0;
	pointX2 = 0;
	pointY2 = 0;
	pointZ2 = 10 * az_plane;
	
	BCBFtoOGL(pointX1, pointY1, pointZ1, phi, theta, psi);
	BCBFtoOGL(pointX2, pointY2, pointZ2, phi, theta, psi);
	
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(pointX1 + planeX, pointY1 + planeY, pointZ1 + planeZ);
	glVertex3f(pointX2 + planeX, pointY2 + planeY, pointZ2 + planeZ);
	glEnd();

	// Calculate a direction vector for the camera in the aircraft reference frame.
		
	CamX = cos(CamPitch) * cos(CamYaw);
	CamY = cos(CamPitch) * sin(CamYaw);
	CamZ = sin(CamPitch);
		
	// Convert this vector to the OGL frame
	BCBFtoOGL(CamX, CamY, CamZ, phi, theta, psi);

	float scalar = 1;
	if( CamY != 0 ) scalar = (planeY - 624) / CamY * -1.0;
	
	// If the camera view intersects the ground, add that point to the
	// camera ground path.
	if(CamY < 0)
	{
		CamPath[CamPathIterator][0] = (CamX * scalar) + planeX;
		CamPath[CamPathIterator][1] = (CamY * scalar) + planeY;
		CamPath[CamPathIterator][2] = (CamZ * scalar) + planeZ;
	}

	// A vector pointing in the direction of the camera
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(planeX, planeY, planeZ);
	glVertex3f((CamX * scalar) + planeX,(CamY * scalar) + planeY,(CamZ * scalar) + planeZ);
	glEnd();

	// Display the OpenGL axis'
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(planeX, planeY, planeZ);
	glVertex3f(planeX+100, planeY, planeZ);
	glVertex3f(planeX, planeY, planeZ);
	glVertex3f(planeX, planeY+100, planeZ);
	glVertex3f(planeX, planeY, planeZ);
	glVertex3f(planeX, planeY, planeZ+100);
	glEnd();

	// Display the camera ground track
	int i = 0;
	int j = CamPathIterator;
	/*glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for(i = 0; i < CamPathCount; i++)
	{
		glVertex3f(CamPath[j][0],CamPath[j][1],CamPath[j][2]);
		j--;
		if(j < 0) j = (CamPathLength - 1);
	}
	glEnd();*/
	if(CamPathCount < CamPathLength) CamPathCount++;
	if(CamY < 0) CamPathIterator++;
	if(CamPathIterator >= CamPathLength) CamPathIterator = 0;

	// Display a vector in the flight frame
	pointX1 = 100;
	pointY1 = 0;
	pointZ1 = 0;
	
	FLIGHTtoBCBF(pointX1, pointY1, pointZ1, alpha, beta);
	BCBFtoOGL(pointX1, pointY1, pointZ1, phi, theta, psi);
		
	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(pointX1 + planeX, pointY1 + planeY, pointZ1 + planeZ);
	glVertex3f(planeX, planeY, planeZ);
	glEnd();

	// Test our rotation from OGL to BCBF frame
	/*pointX1 = 0;
	pointY1 = 0;
	pointZ1 = 20;
		
	OGLtoBCBF(pointX1, pointY1, pointZ1, phi, theta, psi);
	BCBFtoOGL(pointX1, pointY1, pointZ1, phi, theta, psi);
		
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(pointX1 + planeX, pointY1 + planeY, pointZ1 + planeZ);
	glVertex3f(planeX, planeY, planeZ);
	glEnd();*/



	return 1;
}                                   