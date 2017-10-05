#include "stdafx.h"

#define MAX_ITEMS 30
#define MAG_FIELD 1000.0

bool CommsEnabled;
float fTextColour[3];
char szString[100];

XPLMHotKeyID gHotKey = NULL;

void MyHotKeyCallback(void* inRefcon);
int MyOrbitPlaneFunc(XPLMCameraPosition_t* outCameraPosition,
                     int                   inIsLosingControl,
                     void*                 inRefcon);

int	MyDrawCallback(XPLMDrawingPhase inPhase,
                   int              inIsBefore,
                   void*            inRefcon);
int MyKeySniffer(char         inChar,
                 XPLMKeyFlags inFlags,
                 char         inVirtualKey,
                 void*        inRefcon);

// Here are the variables for implementing the file based control of the setup
Channels ControlSurfaces;               // The list of control surfaces
string   CommPortString = "";           // Place to put the port string to open
long     CommPortSpeed = 38400;
uint16_t PortNum = 14551;
string   OverString = "sim/operation/override/override_flightcontrol";
                                                    // Defaults to standard joystick control
float    ThrottleSettings[8] = {0,0,0,0,0,0,0,0};   // The throttle settings with default values

#define PARKBRAKE_ON  1.0
#define PARKBRAKE_OFF 0.0
#define PARKBRAKE_THROTTLE_THRESHOLD 0.05
float    BrakeSetting = PARKBRAKE_ON;   // Initialise with park brake enabled

float GetBodyRates(float elapsedMe, float elapsedSim, int counter, void* refcon);
void SerialPortAccessCallback(XPLMWindowID inWindowID, void* inRefcon);
int DrawStrings(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon);
void CalculateChecksum(unsigned char msg[]);
void Store4LE(unsigned char* store, union longbbbb data);
void Store2LE(unsigned char* store, union intbb data);

int msgDefault(unsigned char rxChar);
int msgSync1(unsigned char rxChar);
int msgServos(unsigned char rxChar);
int msgCheckSum(unsigned char rxChar);
int msgVarSize(unsigned char rxChar);   // Parser for variable count data byte
int msgVarServos(unsigned char rxChar); // Receive channel data of variable length
int msgSync2(unsigned char rxChar);     // Parser for second byte of variable channel count message

unsigned char var_channel_count;        // Number of channels to be recieved
unsigned char ck_in_a, ck_in_b, ck_calc_a, ck_calc_b;

void SetupDefaultServoZeros(void);
void ServosToControls(void);

int GPSCount = 0;
int store_index = 0;
int ConnectionCount = 0;
float pendingElapsedTime = 0.0;

int (*msg_parse)(unsigned char rxChar) = &msgDefault;

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
            0xB5, 0x62,     // Header
            0x01, 0xAB,     // ID
            0x0C, 0x00,     // Payload Length
            0x00, 0x00,     // P
            0x00, 0x00,     // Q
            0x00, 0x00,     // R
            0x00, 0x00,     // gravity minus acceleration, UDBx
            0x00, 0x00,     // gravity minus acceleration, UDBy
            0x00, 0x00,     // gravity minus acceleration, UDBz
            0x00, 0x00      // Checksum
            };

unsigned char NAV_KEYSTROKE[] = {
            0xB5, 0x62,     // Header
            0x01, 0xAC,     // ID
            0x02, 0x00,     // Payload Length
            0x00, 0x00,     // KeySniffer keystroke data, ckey, vkey
            0x00, 0x00      // Checksum
            };

// At 4hz the normal messages are ~6000 bps

// 60 bytes
unsigned char NAV_SOL[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x06,             // ID               - DONE
            0x34, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x00, 0x00, 0x00, 0x00, // fTOW             - DONE
            0x00, 0x00,             // week             - DONE
            0x03,                   // gpsFix           - DONE
            0x0D,                   // flags            - DONE
            0x00, 0x00, 0x00, 0x00, // ecefX            - DONE
            0x00, 0x00, 0x00, 0x00, // ecefY            - DONE
            0x00, 0x00, 0x00, 0x00, // ecefZ            - DONE
            0x64, 0x00, 0x00, 0x00, // pAcc             - DONE
            0x00, 0x00, 0x00, 0x00, // ecefVX           - DONE
            0x00, 0x00, 0x00, 0x00, // ecefVY           - DONE
            0x00, 0x00, 0x00, 0x00, // ecefVZ           - DONE
            0x64, 0x00, 0x00, 0x00, // sACC             - DONE
            0x64, 0x00,             // pDOP             - DONE
            0x00,                   // res1             - DONE
            0x04,                   // numSV            - DONE
            0x00, 0x00, 0x00, 0x00, // res2             - DONE
            0x00, 0x00              // Checksum         - DONE
            };

// 26 bytes
unsigned char NAV_DOP[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x04,             // ID               - DONE
            0x12, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x64, 0x00,             // gDOP             - DONE
            0x64, 0x00,             // pDOP             - DONE
            0x64, 0x00,             // tDOP             - DONE
            0x64, 0x00,             // vDOP             - DONE
            0x64, 0x00,             // hDOP             - DONE
            0x64, 0x00,             // nDOP             - DONE
            0x64, 0x00,             // eDOP             - DONE
            0x00, 0x00              // Checksum         - DONE
            };

// 36 bytes
unsigned char NAV_POSLLH[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x02,             // ID               - DONE
            0x1C, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x00, 0x00, 0x00, 0x00, // lon              - DONE
            0x00, 0x00, 0x00, 0x00, // lat              - DONE
            0x00, 0x00, 0x00, 0x00, // height           - DONE
            0x00, 0x00, 0x00, 0x00, // hMSL             - DONE
            0xE8, 0x03, 0x00, 0x00, // hAcc             - DONE
            0xE8, 0x03, 0x00, 0x00, // vAcc             - DONE
            0x00, 0x00              // Checksum         - DONE
            };

// 44 bytes
unsigned char NAV_VELNED[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x12,             // ID               - DONE
            0x24, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x00, 0x00, 0x00, 0x00, // velN             - DONE
            0x00, 0x00, 0x00, 0x00, // velE             - DONE
            0x00, 0x00, 0x00, 0x00, // velD             - DONE
            0x00, 0x00, 0x00, 0x00, // speed            - DONE
            0x00, 0x00, 0x00, 0x00, // gSpeed           - DONE
            0x00, 0x00, 0x00, 0x00, // heading          - DONE
            0x64, 0x00, 0x00, 0x00, // sAcc             - DONE
            0xA0, 0x86, 0x01, 0x00, // cAcc             - DONE
            0x00, 0x00              // Checksum         - DONE
            };

XPLMDataRef drP, drQ, drR,
            drLat, drLon, drElev,
            drLocal_ax, drLocal_ay, drLocal_az,
            drLocal_vx, drLocal_vy, drLocal_vz,
            drLocal_x,  drLocal_y,  drLocal_z,
            drAirSpeedTrue,
            drLocalDays, drLocalSecs,
            drPhi, drTheta, drPsi,
            drAlpha, drBeta,
            drOverRide, drThrOverRide,
            drPitchAxis, drRollAxis, drYawAxis, drThro,
            drg_nrml, drg_axil, drg_side,
            drBrakeOverRide, drBrake;

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

PLUGIN_API int XPluginStart(char* outName,
                            char* outSig,
                            char* outDesc)
{
	szString[0] = '\0';
	strcpy(outName, "UDB HILSIM");
	strcpy(outSig, "UDB.HardwareInLoop");
	strcpy(outDesc, "UDB Hardware-In-Loop Simulator");

	// P, Q, and R are roll, pitch, and yaw rates, degrees per second,
	// in the NED coordinate system, in "flight" (not body) frame of reference
	drP = XPLMFindDataRef("sim/flightmodel/position/P");
	drQ = XPLMFindDataRef("sim/flightmodel/position/Q");
	drR = XPLMFindDataRef("sim/flightmodel/position/R");

	// standard GPS lat, long, and elevation
	drLat  = XPLMFindDataRef("sim/flightmodel/position/latitude");
	drLon  = XPLMFindDataRef("sim/flightmodel/position/longitude");
	drElev = XPLMFindDataRef("sim/flightmodel/position/elevation");

	// acceleration, meters/sec/sec, in OGL frame of reference
	// note: this is NOT the same as what the accelerometers read
	drLocal_ax = XPLMFindDataRef("sim/flightmodel/position/local_ax");
	drLocal_ay = XPLMFindDataRef("sim/flightmodel/position/local_ay"); 
	drLocal_az = XPLMFindDataRef("sim/flightmodel/position/local_az");

	// velocity, meters/sec, in OGL frame of reference
	drLocal_vx = XPLMFindDataRef("sim/flightmodel/position/local_vx");
	drLocal_vy = XPLMFindDataRef("sim/flightmodel/position/local_vy"); 
	drLocal_vz = XPLMFindDataRef("sim/flightmodel/position/local_vz");

	drLocalDays = XPLMFindDataRef("sim/time/local_date_days");
	drLocalSecs = XPLMFindDataRef("sim/time/local_time_sec");

	// 3D true airspeed, meters/second
	drAirSpeedTrue = XPLMFindDataRef("sim/flightmodel/position/true_airspeed");

	// location in OGL frame
	drLocal_x = XPLMFindDataRef("sim/flightmodel/position/local_x");
	drLocal_y = XPLMFindDataRef("sim/flightmodel/position/local_y");
	drLocal_z = XPLMFindDataRef("sim/flightmodel/position/local_z");

	// Euler roll, pitch, and yaw angles, degrees, using NED
	// + phi is roll to the right (CCW around NED_x)
	// + theta is pitch up (CCW around NED_y)
	// + psi is yaw to the right (CCW around NED_z)
	drPhi   = XPLMFindDataRef("sim/flightmodel/position/phi");
	drTheta = XPLMFindDataRef("sim/flightmodel/position/theta");
	drPsi   = XPLMFindDataRef("sim/flightmodel/position/psi");

	// Euler angles of attack and sideslip, degrees, using NED
	// These are angles of body frame with respect to fight path
	// + alpha is pitch up (CCW around NED_y)
	// + beta is yaw right (CCW around NED_z)
	drAlpha = XPLMFindDataRef("sim/flightmodel/position/alpha");
	drBeta  = XPLMFindDataRef("sim/flightmodel/position/beta");

	drThrOverRide = XPLMFindDataRef("sim/operation/override/override_throttles");
	drPitchAxis   = XPLMFindDataRef("sim/joystick/FC_ptch");
	drRollAxis    = XPLMFindDataRef("sim/joystick/FC_roll");
	drYawAxis     = XPLMFindDataRef("sim/joystick/FC_hdng");
	drThro        = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_use");

// sim/operation/override/override_gearbrake	int	y	boolean	Override gear and brake staus
// sim/flightmodel/controls/parkbrake	float	y	???	Parking Brake

	drBrakeOverRide = XPLMFindDataRef("sim/operation/override/override_gearbrake");
	drBrake         = XPLMFindDataRef("sim/flightmodel/controls/parkbrake");

	// These are the values that the accelerometers read, in multiples of gravity
	// g_nrml is UDB_z
	// g_axil is UDB_y
	// g_side is UDB_x
	drg_nrml = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");
	drg_axil = XPLMFindDataRef("sim/flightmodel/forces/g_axil");
	drg_side = XPLMFindDataRef("sim/flightmodel/forces/g_side");

	fTextColour[0] = 1.0;
	fTextColour[1] = 1.0;
	fTextColour[2] = 1.0;

	XPLMRegisterFlightLoopCallback(GetBodyRates, 1.0, NULL);
	XPLMRegisterDrawCallback(
	    DrawStrings,
	    xplm_Phase_Window,      // Draw when sim is doing objects
	    0,                      // After objects
	    NULL);                  // No refcon needed

//	XPLMSetDatai(drOverRide, 1);// Now overide surfaces, not yaw, roll, pitch

	// Register our hot key for the new view
	gHotKey = XPLMRegisterHotKey(
	    XPLM_VK_F8,
	    xplm_DownFlag, 
	    "Circling External View",
	    MyHotKeyCallback,
	    NULL);

	XPLMRegisterDrawCallback(
	    MyDrawCallback,
	    xplm_Phase_Objects,     // Draw when sim is doing objects
	    0,                      // After objects
	    NULL);                  // No refcon needed

	XPLMRegisterKeySniffer(
		MyKeySniffer,           // Our callback
		1,                      // Receive input before plugin windows
		0);                     // Refcon - not used

	memset(&CamPath, 0, sizeof(float) * 3 * CamPathLength);
	return 1;
}

/*
 * MyKeySniffer
 *
 * This routnine receives keystrokes from the simulator as they are pressed.
 * A separate message is received for each key press and release as well as
 * keys being held down.
 *
 */
int MyKeySniffer(char         inChar,
                 XPLMKeyFlags inFlags,
                 char         inVirtualKey,
                 void*        inRefcon)
{
#if 1
	if ((inVirtualKey >= 33 && inVirtualKey <= 40) || (inVirtualKey >= 96 && inVirtualKey <= 111))
	{
		NAV_KEYSTROKE[6] = (unsigned char)inFlags;
		NAV_KEYSTROKE[7] = (unsigned char)inVirtualKey;
		CalculateChecksum(NAV_KEYSTROKE);
		SendToComPort(sizeof(NAV_KEYSTROKE), NAV_KEYSTROKE);
		return 0;   // Returning 0 consumes the keystroke
	}

//	LoggingFile.mLogFile << "KeySniffer " << inFlags << ":" << inVirtualKey << endl;

#else
	NAV_KEYSTROKE[6] = (unsigned char)inFlags;
	NAV_KEYSTROKE[7] = (unsigned char)inVirtualKey;
	CalculateChecksum(NAV_KEYSTROKE);
	SendToComPort(sizeof(NAV_KEYSTROKE), NAV_KEYSTROKE);
	if ((inVirtualKey >= 33 && inVirtualKey <= 40) || (inVirtualKey >= 96 && inVirtualKey <= 111))
	{
		return 0;   // Returning 0 consumes the keystroke
	}
#endif
	return 1;       // Return 1 to pass the keystroke to plugin windows and X-Plane
}
/*
Key | inChar | inVirtualKey | MatrixPilot function
Page Up    0 | 33
Page Down  0 | 34
End        0 | 35     Mode manual
Home       0 | 36     Mode failsafe
Left      28 | 37
Up        30 | 38
Right     29 | 39
Down      31 | 40

x        120 | 88
z        122 | 90

Numpad-0  48 | 96
Numpad-1  49 | 97     Rudder left
Numpad-2  50 | 98     Elevator down
Numpad-3  51 | 99     Rudder right
Numpad-4  52 | 100    Aileron left
Numpad-5  53 | 101    Centre the stick
Numpad-6  54 | 102    Aileron right
Numpad-7  55 | 103
Numpad-8  56 | 104    Elevator up
Numpad-9  57 | 105
Numpad-*  42 | 106    Mode guided / waypoint
Numpad-+  43 | 107    Throttle up
Numpad--  45 | 109    Throttle down
Numpad-/  47 | 111    Mode stablised

,         44 | 183
.         46 | 185
 */

int DrawStrings(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon)
{
	// unused
	(void)inPhase;
	(void)inIsBefore;
	(void)inRefcon;

	XPLMDrawString(fTextColour, 300, 740, szString, NULL, xplmFont_Basic);
	return 1;
}

PLUGIN_API void XPluginStop(void)
{
	XPLMUnregisterHotKey(gHotKey);
	XPLMUnregisterDrawCallback(
	    MyDrawCallback,
	    xplm_Phase_LastCockpit, 
	    0,
	    NULL);
}

void AttemptConnection(void)
{
	OpenComms();
}

PLUGIN_API void XPluginDisable(void)
{
	LoggingFile.mLogFile << "XPluginDisable\n";

	CloseComms();

	XPLMSetDatai(drOverRide, 0);        // Clear the overides
	XPLMSetDatai(drThrOverRide, 0);
	XPLMSetDatai(drBrakeOverRide, 0);
}

PLUGIN_API int XPluginEnable(void)
{
	PortNum = 0;
	pendingElapsedTime = 0;

	LoggingFile.mLogFile << "XPluginEnable\n";

	// Load the setup file on enable.  This allows the user to modify the file without exit of XPlane
	SetupFile Setup;
	// Open the setup file and parse it into the control surface list
	Setup.LoadSetupFile(ControlSurfaces, CommPortString, CommPortSpeed, PortNum, OverString);

	// Don't attempt a conection until we're done starting up
	// AttemptConnection();

	SetupDefaultServoZeros();                           // Setup the servo defaults.

	drOverRide = XPLMFindDataRef(OverString.data());    // Get the latest overide reference
	XPLMSetDatai(drOverRide, 1);                        // Overide from the setup file
	XPLMSetDatai(drThrOverRide, 1);
	XPLMSetDatai(drBrakeOverRide, 1);
	memset(&CamPath, 0, sizeof(float) * 3 * CamPathLength);
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho,
                                      int          inMessage,
                                      void*        inParam)
{
	(void)inFromWho;
	(void)inMessage;
	(void)inParam;
}

float GetBodyRates(float elapsedMe, float elapsedSim, int counter, void* refcon)
{
	(void)elapsedSim;
	(void)counter;
	(void)refcon;

	pendingElapsedTime += elapsedMe;

	ReceiveFromComPort();

	if (pendingElapsedTime < 0.025) // Don't run faster than 40Hz
	{
		return -1;
	}

	union intbb Temp2;
	float phi, theta, psi;
	float P_flight, Q_flight, R_flight;
	float ax, ay, az;
	float gravity_acceleration_x, gravity_acceleration_y, gravity_acceleration_z;

	// Angular rates in X-Plane are specified relative to the flight path, not to the aircraft,
	// for reasons unknown. So that means we need to rotate by alpha and beta to get angular rates
	// in the aircraft body frame, which is what the UDB measures.

	// Retrieve rates and slip angles, and convert to radians
	P_flight = XPLMGetDataf(drP)  / 180 * PI;
	Q_flight = XPLMGetDataf(drQ)  / 180 * PI;
	R_flight = XPLMGetDataf(drR)  / 180 * PI;
	
    // On 25th Jan 2015, Bill Premerlani confirmed with Austin Meyer, author of X-Plane
    // that P, Q and R are rotations in the body frame. So they do not need to be rotated into
    // any other frame of reference, other than a small sign correction for the UDB frame conventions.
    // Austin Meyer said: "now, i CAN say that P is roll, Q is pitch, and R is yaw, all in degrees per second
    //about the aircraft axis,..... (i just looked at the code to confirm this)"

	P_plane = P_flight;
	Q_plane = -Q_flight;   // convert from NED to UDB
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
	phi =   (float)((XPLMGetDataf(drPhi)   / 180) * PI);
	theta = (float)((XPLMGetDataf(drTheta) / 180) * PI);
	psi =   (float)((XPLMGetDataf(drPsi)   / 180) * PI);

	// set up a vertical reference for the plotting computations
	// vertical in earth frame:
	ax = 0;
	ay = -(float)9.8;
	az = 0;

	// get the acceleration loading (gravity-acceleration) in the body frame in "g"s,
	// and convert to meter/sec/sec
	// x, y, and z are "UDB" coordinates, x is left wing, y is forward, and z is down.
	gravity_acceleration_x = (float)((XPLMGetDataf(drg_side)) * 9.8);
	gravity_acceleration_y = (float)((XPLMGetDataf(drg_axil)) * 9.8);
	gravity_acceleration_z = (float)((XPLMGetDataf(drg_nrml)) * 9.8);

	// Convert from OGL frame to Aircraft body fixed frame
	// This produces a vertical reference in body frame
	OGLtoBCBF(ax, ay, az, phi, theta, psi);

	ax_plane =  ax;
	ay_plane = -ay;     // convert from NED to UDB
	az_plane =  az;

	// Lastly we need to convert from X-Plane units (m/s^2) to the arbitrary units used by the UDB

	// Accelerations are in m/s^2
	// Divide by 9.8 to get g's
	// Multiply by 5280 (constant from UDB code)
	// Divide by SCALEACCEL (2.64 for red board)
	// 1 / 9.8 * 5280 / 2.64 = 204.8
	Temp2.BB = (int)(gravity_acceleration_x * 204.8);
	Store2LE(&NAV_BODYRATES[12], Temp2);
	Temp2.BB = (int)(gravity_acceleration_y * 204.8);
	Store2LE(&NAV_BODYRATES[14], Temp2);
	Temp2.BB = (int)(gravity_acceleration_z * 204.8);
	Store2LE(&NAV_BODYRATES[16], Temp2);

	CalculateChecksum(NAV_BODYRATES);
	SendToComPort(sizeof(NAV_BODYRATES), NAV_BODYRATES);

	while (pendingElapsedTime >= 0.025) // Don't run slower than 40Hz
	{
		GPSCount++;
		if (!IsConnected())
		{
			ConnectionCount++;
			if (ConnectionCount % 160 == 0) // attempt reconnection every 4 seconds when disconnected
			{
				AttemptConnection();
				ConnectionCount = 0;
			}
		}
		if (GPSCount % 10 == 0)
		{
			GetGPSData();
			GPSCount = 0;
		}
		pendingElapsedTime -= (float)0.025;
	}
	ServosToControls();

//	float ThrottleSetting = 0;	//SurfaceDeflections[CHANNEL_THROTTLE];
//	float throttle[8] = {ThrottleSetting, ThrottleSetting, ThrottleSetting, ThrottleSetting,
//	                     ThrottleSetting, ThrottleSetting, ThrottleSetting, ThrottleSetting};
	XPLMSetDatavf(drThro, ThrottleSettings, 0, 8);

	static float prevBrakeSetting = PARKBRAKE_ON;
	if (BrakeSetting != prevBrakeSetting)
	{
		prevBrakeSetting = BrakeSetting;
		XPLMSetDataf(drBrake, BrakeSetting);
//		LoggingFile.mLogFile << "Set parkbrake to " << BrakeSetting << endl;
	}
	return -1;  // get called back on every frame
}

void GetGPSData(void)
{
	union longbbbb Temp4;
	union intbb Temp2;
	float phi, theta, psi;

	phi =   (float)((XPLMGetDataf(drPhi)   / 180) * PI);
	theta = (float)((XPLMGetDataf(drTheta) / 180) * PI);
	psi =   (float)((XPLMGetDataf(drPsi)   / 180) * PI);

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

	double latitude  = XPLMGetDataf(drLat);
	double longitude = XPLMGetDataf(drLon);
	double elevation = XPLMGetDataf(drElev);
	double local_x   = XPLMGetDataf(drLocal_x);
	double local_y   = XPLMGetDataf(drLocal_y);
	double local_z   = XPLMGetDataf(drLocal_z);
	double local_vx  = XPLMGetDataf(drLocal_vx);
	double local_vy  = XPLMGetDataf(drLocal_vy);
	double local_vz  = XPLMGetDataf(drLocal_vz);

	Temp4.WW = (int)(local_vx * 100);
	Store4LE(&NAV_VELNED[14], Temp4);
	Temp4.WW = (int)(local_vy * -100);
	Store4LE(&NAV_VELNED[18], Temp4);
	Temp4.WW = (int)(local_vz * -100);
	Store4LE(&NAV_VELNED[10], Temp4);
	Temp4.WW = (int)(XPLMGetDataf(drAirSpeedTrue) * 100);
	Store4LE(&NAV_VELNED[22], Temp4);

	// note: xplane ground speed is not GPS speed over ground,
	// it is 3D ground speed. we need horizontal ground speed for GPS,
	// which is computed from the horizontal local velocity components:
	double speed_over_ground = 100 * sqrt(local_vx*local_vx + local_vz*local_vz);
	Temp4.WW = (int)speed_over_ground;
	Store4LE(&NAV_VELNED[26], Temp4);

	// Compute course over ground, in degrees,
	// from horizontal earth frame velocities,
	// which are in OGL frame of reference.
	// local_vx is east, local_vz is south.
	double course_over_ground = (atan2(local_vx, -local_vz) / PI * 180.0);
	// MatrixPilot is expecting an angle between 0 and 360 degrees.
	if (course_over_ground < 0.0) course_over_ground += 360.0;

	Temp4.WW = (int)(100000.0 * course_over_ground);
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
	XPLMLocalToWorld(local_x,
	                 local_y,
	                 local_z,
	                 &ac_pos_lat,
	                 &ac_pos_lon,
	                 &ac_pos_elev);

	// Get AC pos + velocity vector in LLA
	XPLMLocalToWorld(local_x + local_vx,
	                 local_y + local_vy,
	                 local_z + local_vz,
	                 &ac_vel_lat,
	                 &ac_vel_lon,
	                 &ac_vel_elev);

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

	// simulate the magnetometer, and place in slots 30,32 and 46 of NAV_SOL
	// these slots are used by Ublox, but not by HILSIM
	// computation is based on zero declination, and zero inclination
	float mag_field_x = 0.0;            // earth OGL x mag field (east)
	float mag_field_y = 0.0;            // earth OGL y mag field (up)
	float mag_field_z = -MAG_FIELD;     // earth OGL z mag field (south)
	// note, the "north pole" of the earth is really a south magnetic pole

	// convert to NED body frame
	OGLtoBCBF(mag_field_x, mag_field_y, mag_field_z, phi, theta, psi);

	// convert from NED body to UDB body frame
	double mag_field_body_udb_x = -mag_field_y;
	double mag_field_body_udb_y =  mag_field_x;
	double mag_field_body_udb_z =  mag_field_z;

	// store in unused slots in NAV_SOL message
	Temp2.BB = (int)mag_field_body_udb_x;
	Store2LE(&NAV_SOL[30], Temp2);
	Temp2.BB = (int)mag_field_body_udb_y;
	Store2LE(&NAV_SOL[32], Temp2);
	Temp2.BB = (int)mag_field_body_udb_z;
	Store2LE(&NAV_SOL[46], Temp2);

	CalculateChecksum(NAV_SOL);
	SendToComPort(sizeof(NAV_SOL), NAV_SOL);
	CalculateChecksum(NAV_DOP);
	SendToComPort(sizeof(NAV_DOP), NAV_DOP);
	CalculateChecksum(NAV_POSLLH);
	SendToComPort(sizeof(NAV_POSLLH), NAV_POSLLH);
	CalculateChecksum(NAV_VELNED);
	SendToComPort(sizeof(NAV_VELNED), NAV_VELNED);
}

void CalculateChecksum(unsigned char* msg)
{
	// length is technically 2 bytes (in little endian order) but we're 
	// not using any messages over 256 bytes long, so we can get away 
	// with just looking at the first byte.

	int length = msg[4];
	int i;
	int CK_A = 0;
	int CK_B = 0;
	for (i = 2; i < (length + 6); i++) {
		CK_A += msg[i];
		CK_B += CK_A;
	}
	msg[length + 6] = CK_A;
	msg[length + 7] = CK_B;
}

// return 1 on receiving full, successful packet
int HandleMsgByte(char b)
{
	return (*msg_parse)(b);
}

void Store4LE(unsigned char* store, union longbbbb data)
{
	store[0] = data.__.B0;
	store[1] = data.__.B1;
	store[2] = data.__.B2;
	store[3] = data.__.B3;
}

void Store2LE(unsigned char* store, union intbb data)
{
	store[0] = data._.B0;
	store[1] = data._.B1;
}

int msgDefault(unsigned char rxChar)
{
	switch (rxChar)
	{
	case 0xFF:
		msg_parse = &msgSync1;      // Fixed size channel count message
		break;
	case 0xFE:
		msg_parse = &msgSync2;      // Variable size channel count message
		break;
	}
	return 0;
}

// Parser for second byte of variable channel count message
int msgSync2(unsigned char rxChar)
{
	switch (rxChar)
	{
	case 0xEF:
		msg_parse = &msgVarSize;    // Next char is variable size
		break;
	default:
		LoggingFile.mLogFile << "msgSync2 faulty start" << endl;
		msg_parse = &msgDefault;    // Faulty start
		break;
	}
	return 0;
}

// Parser for variable count data byte
int msgVarSize(unsigned char rxChar)
{
	switch (rxChar)
	{
	case 0xFF:
	case 0xFE:
	case 0xEF:
		LoggingFile.mLogFile << "msgVarSize faulty value" << endl;
		msg_parse = &msgDefault;    // Faulty value
		break;
	default:
		if (var_channel_count > MAX_VARIABLE_CHANNELS)
		{
			LoggingFile.mLogFile << "msgVarSize faulty start" << endl;
			msg_parse = &msgDefault;// Faulty start
			return 0;
		}
		var_channel_count = rxChar;
		msg_parse = &msgVarServos;  // Next char is variable size
		store_index = 0;
		ck_calc_a = ck_calc_b = 0;
		break;
	}
	return 0;
}

int msgVarServos(unsigned char rxChar)
{
	if (store_index < (var_channel_count*2)) {
		SERVO_IN_[store_index++] = rxChar;
		ck_calc_a += rxChar;
		ck_calc_b += ck_calc_a;
	} else {
		ck_in_a = rxChar;
		msg_parse = &msgCheckSum;
	}
	return 0;
}

int msgSync1(unsigned char rxChar)
{
	switch (rxChar)
	{
	case 0xEE:
		{
		store_index = 0;
		ck_calc_a = ck_calc_b = 0;
		msg_parse = &msgServos;
		}
		break;
	case 0xFF:
		break;                      // do nothing
	default: 
		LoggingFile.mLogFile << "msgSync1 error condition" << endl;
		msg_parse = &msgDefault;    // error condition
		break;
	}
	return 0;
}

int msgServos(unsigned char rxChar)
{
	if (store_index < SERVO_MSG_LENGTH)
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
	return 0;
}

int msgCheckSum(unsigned char rxChar)
{
	msg_parse = &msgDefault;

	ck_in_b = rxChar;
	if ((ck_in_a == ck_calc_a) && (ck_in_b == ck_calc_b)) {
		memcpy(SERVO_IN, SERVO_IN_, sizeof(SERVO_IN_));
		return 1;   // success!
	}
	LoggingFile.mLogFile << "msgCheckSum failed" << endl;
	return 0;
}

/****************************************************************************************/
// Step through list of controls reading the servo channel, calculating new position 
// and setting either a surface or engine(s) to the value
void ServosToControls(void)
{
	int   iIndex = 0;
	int   iSize = ControlSurfaces.size();
	int   iServoChannel;
	intbb ServoValue;
	float ControlSetting;
	ChannelSetup* pScanSetup;

	for (iIndex = 0; iIndex < iSize; iIndex++)
	{
		pScanSetup = &ControlSurfaces[iIndex];

		iServoChannel = pScanSetup->mServoChannel;
		ServoValue._.B1 = SERVO_IN[2*iServoChannel];
		ServoValue._.B0 = SERVO_IN[(2*iServoChannel)+1];

		ControlSetting = pScanSetup->GetControlDeflection(ServoValue.BB);
		
		if (pScanSetup->mControlType == CONTROL_TYPE_SURFACE)
		{
			XPLMSetDataf(pScanSetup->mControlSurfaceRef, ControlSetting);
		}
		else if (pScanSetup->mControlType == CONTROL_TYPE_ENGINE)
		{
			unsigned int Mask = pScanSetup->mEngineMask;

			for (int Engine = 0; Engine < 8; Engine++)
			{
				if ((Mask & 0x01) != 0)
				{
					ThrottleSettings[Engine] = ControlSetting;
				}
				Mask >>= 1;
			}
			if (ControlSetting > PARKBRAKE_THROTTLE_THRESHOLD)  // if engine is throttled-up above threshold..
			{
				BrakeSetting = PARKBRAKE_OFF;   // remove parkbrake
			}
			else
			{
				BrakeSetting = PARKBRAKE_ON;    // apply parkbrake
			}
		}
	}
}

// Set all control channels in the list to the default servo position
// Note, if there is more than one control surface per channel, it will reset it
// to the last surface defined in the list
void SetupDefaultServoZeros(void)
{
	intbb ServoValue;
	int iSize = ControlSurfaces.size();

	for (int iIndex = 0; iIndex < iSize; iIndex++)
	{
		ChannelSetup* pScanSetup = &ControlSurfaces[iIndex];
		ServoValue.BB = pScanSetup->mChannelOffset;
		int iServoChannel = pScanSetup->mServoChannel;
		SERVO_IN[2*iServoChannel] = ServoValue._.B1;
		SERVO_IN[(2*iServoChannel)+1] = ServoValue._.B0;
	}
}

void MyHotKeyCallback(void* inRefcon)
{
	(void)inRefcon;

	// This is the hotkey callback.  First we simulate a joystick press and
	// release to put us in 'free view 1'.  This guarantees that no panels
	// are showing and we are an external view.
	XPLMCommandButtonPress(xplm_joy_v_fr1);
	XPLMCommandButtonRelease(xplm_joy_v_fr1);

	// Now we control the camera until the view changes.
	XPLMControlCamera(xplm_ControlCameraUntilViewChanges, MyOrbitPlaneFunc, NULL);
}

/*
 * MyOrbitPlaneFunc
 * 
 * This is the actual camera control function, the real worker of the plugin.  It is 
 * called each time X-Plane needs to draw a frame.
 * 
 */
int MyOrbitPlaneFunc(XPLMCameraPosition_t* outCameraPosition,
                     int                   inIsLosingControl,
                     void*                 inRefcon)
{
	(void)inRefcon;

	if (outCameraPosition && !inIsLosingControl)
	{
		int w, h, x, y;
		float dx, dz, dy; 

		// First get the screen size and mouse location.  We will use this to decide
		// what part of the orbit we are in.  The mouse will move us up-down and around.
		XPLMGetScreenSize(&w, &h);
		XPLMGetMouseLocation(&x, &y);

		double local_x = XPLMGetDataf(drLocal_x);
		double local_y = XPLMGetDataf(drLocal_y);
		double local_z = XPLMGetDataf(drLocal_z);
		float phi      = XPLMGetDataf(drPhi);
		float theta    = XPLMGetDataf(drTheta);
		float psi      = XPLMGetDataf(drPsi);

		// Convert this vector back into angles.
		float CamPitchOGL = (atan2(CamY, sqrt((CamX * CamX) + (CamZ * CamZ)))) / PI * 180;
		float CamYawOGL = (atan2(CamX, CamZ * (float)-1.0 )) / PI * 180;

		//sprintf(szString, "CamPitch: %f,\tCamYaw: %f,\tCamX: %f,\tCamY: %f,\tCamZ: %f,\0", (CamPitch), (CamYaw), CamX, CamY, CamZ);

		// Camera Position On Aircraft
		dx = -1.0;
		dy =  0.0;
		dz =  1.0;

		BCBFtoOGL(dx,dy,dz,phi,theta,psi);

		// Fill out the camera position info.
		outCameraPosition->x = (float)(local_x + dx);
		outCameraPosition->y = (float)(local_y + dy);
		outCameraPosition->z = (float)(local_z + dz);
		outCameraPosition->pitch = CamPitchOGL;
		outCameraPosition->heading = CamYawOGL;
		outCameraPosition->roll = 0;
	}
	// Return 1 to indicate we want to keep controlling the camera.
	return 1;
}

int MyDrawCallback(XPLMDrawingPhase inPhase,
                   int              inIsBefore,
                   void*            inRefcon)
{
	(void)inPhase;
	(void)inIsBefore;
	(void)inRefcon;

	// If any data refs are missing, do not draw.
	if (!drLocal_x || !drLocal_y || !drLocal_z)
		return 1;

	// Fetch the plane's location at this instant in OGL coordinates.
	float planeX = XPLMGetDataf(drLocal_x);
	float planeY = XPLMGetDataf(drLocal_y);
	float planeZ = XPLMGetDataf(drLocal_z);

	float phi   = XPLMGetDataf(drPhi)   / 180 * PI;
	float theta = XPLMGetDataf(drTheta) / 180 * PI;
	float psi   = XPLMGetDataf(drPsi)   / 180 * PI;
	float alpha = XPLMGetDataf(drAlpha) / 180 * PI;
	float beta  = XPLMGetDataf(drBeta)  / 180 * PI;

	/* Reset the graphics state.  This turns off fog, texturing, lighting,
	 * alpha blending or testing and depth reading and writing, which
	 * guarantees that our axes will be seen no matter what. */
	XPLMSetGraphicsState(0, 0, 0, 0, 0, 0, 0);

	// Do the actual drawing.  use GL_LINES to draw sets of discrete lines.
	// Each one will go 100 meters in any direction from the plane.
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
	if (CamY != 0) scalar = (float)((planeY - 624) / CamY * -1.0);
	
	// If the camera view intersects the ground, add that point to the
	// camera ground path.
	if (CamY < 0)
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
/*
	int i = 0;
	int j = CamPathIterator;
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < CamPathCount; i++)
	{
		glVertex3f(CamPath[j][0],CamPath[j][1],CamPath[j][2]);
		j--;
		if (j < 0) j = (CamPathLength - 1);
	}
	glEnd();
*/
	if (CamPathCount < CamPathLength) CamPathCount++;
	if (CamY < 0) CamPathIterator++;
	if (CamPathIterator >= CamPathLength) CamPathIterator = 0;

	// Display a vector in the flight frame
	pointX1 = 100;
	pointY1 = 0;
	pointZ1 = 0;

	FLIGHTtoBCBF(pointX1, pointY1, pointZ1, alpha, beta);
	pointY1 = -pointY1; // convert from NED to UDB frame
	BCBFtoOGL(pointX1, pointY1, pointZ1, phi, theta, psi);

	glColor3f(1.0, 0.0, 1.0);
	glBegin(GL_LINES);
	glVertex3f(pointX1 + planeX, pointY1 + planeY, pointZ1 + planeZ);
	glVertex3f(planeX, planeY, planeZ);
	glEnd();

	// Test our rotation from OGL to BCBF frame
/*
	pointX1 = 0;
	pointY1 = 0;
	pointZ1 = 20;

	OGLtoBCBF(pointX1, pointY1, pointZ1, phi, theta, psi);
	BCBFtoOGL(pointX1, pointY1, pointZ1, phi, theta, psi);

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(pointX1 + planeX, pointY1 + planeY, pointZ1 + planeZ);
	glVertex3f(planeX, planeY, planeZ);
	glEnd();
 */
	return 1;
}
