#include "stdafx.h"
#include "Setup.h"
#include "Servos.h"
#include "SendUBX.h"
#include "Display.h"
#include "SerialIO.h"
#include "utility.h"


#define MAX_ITEMS 30
#define MAG_FIELD 1000.0

int MyKeySniffer(char inChar, XPLMKeyFlags inFlags, char inVirtualKey, void* inRefcon);
float MyFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void* refcon);

string   OverString = "sim/operation/override/override_flightcontrol";
                                                    // Defaults to standard joystick control
int GPSCount = 0;
int ConnectionCount = 0;
float pendingElapsedTime = 0.0;

XPLMDataRef drP, drQ, drR,
            drLat, drLon, drElev,
            drLocal_ax, drLocal_ay, drLocal_az,
            drLocal_vx, drLocal_vy, drLocal_vz,
            drLocal_x,  drLocal_y,  drLocal_z,
            drAirSpeedTrue,
            drLocalDays, drLocalSecs,
            drPhi, drTheta, drPsi,
            drOverRide, drThrOverRide, drBrakeOverRide,
            drThro, drBrake,
            drg_nrml, drg_axil, drg_side;

float P_plane, Q_plane, R_plane;
float ax_NED, ay_NED, az_NED, mag_NED;
//float ax_plane, ay_plane, az_plane;


PLUGIN_API int XPluginStart(char* outName,
                            char* outSig,
                            char* outDesc)
{
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
//	drAlpha = XPLMFindDataRef("sim/flightmodel/position/alpha");
//	drBeta  = XPLMFindDataRef("sim/flightmodel/position/beta");

	drThrOverRide = XPLMFindDataRef("sim/operation/override/override_throttles");
//	drPitchAxis   = XPLMFindDataRef("sim/joystick/FC_ptch");
//	drRollAxis    = XPLMFindDataRef("sim/joystick/FC_roll");
//	drYawAxis     = XPLMFindDataRef("sim/joystick/FC_hdng");
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

	XPLMRegisterFlightLoopCallback(MyFlightLoopCallback, 1.0, NULL);
	XPLMRegisterKeySniffer(MyKeySniffer, 1, 0); // Receive input before plugin windows
//	XPLMSetDatai(drOverRide, 1);// Now overide surfaces, not yaw, roll, pitch
	DisplayStart();
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
		Send_NAV_KEYSTROKE(inFlags, inVirtualKey);
		return 0;   // Returning 0 consumes the keystroke
	}
//	Log.Append("KeySniffer inFlags ", inFlags);
//	Log.Append("KeySniffer inVirtualKey ", inVirtualKey);
#else
	Send_NAV_KEYSTROKE(inFlags, inVirtualKey);
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

PLUGIN_API void XPluginStop(void)
{
	DisplayStop();
}

void AttemptConnection(void)
{
	OpenComms();
}

PLUGIN_API void XPluginDisable(void)
{
	Log.Append("XPluginDisable");
	CloseComms();
	XPLMSetDatai(drOverRide, 0);        // Clear the overides
	XPLMSetDatai(drThrOverRide, 0);
	XPLMSetDatai(drBrakeOverRide, 0);
}

PLUGIN_API int XPluginEnable(void)
{
	Log.Append("XPluginEnable");
	pendingElapsedTime = 0;
	ServosEnable(OverString);
	drOverRide = XPLMFindDataRef(OverString.data());    // Get the latest overide reference
	XPLMSetDatai(drOverRide, 1);                        // Overide from the setup file
	XPLMSetDatai(drThrOverRide, 1);
	XPLMSetDatai(drBrakeOverRide, 1);
	DisplayEnable();
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

void GetBodyRatesData(void)
{
	float P_flight, Q_flight, R_flight;
	float grav_accel_x, grav_accel_y, grav_accel_z;

	// Angular rates in X-Plane are specified relative to the flight path, not to the aircraft,
	// for reasons unknown. So that means we need to rotate by alpha and beta to get angular rates
	// in the aircraft body frame, which is what the UDB measures.

	// Retrieve rates and slip angles, and convert to radians
	P_flight = XPLMGetDataf(drP) / 180 * PI;
	Q_flight = XPLMGetDataf(drQ) / 180 * PI;
	R_flight = XPLMGetDataf(drR) / 180 * PI;
	
    // On 25th Jan 2015, Bill Premerlani confirmed with Austin Meyer, author of X-Plane
    // that P, Q and R are rotations in the body frame. So they do not need to be rotated into
    // any other frame of reference, other than a small sign correction for the UDB frame conventions.
    // Austin Meyer said: "now, i CAN say that P is roll, Q is pitch, and R is yaw, all in degrees per second
    //about the aircraft axis,..... (i just looked at the code to confirm this)"
	P_plane =  P_flight;
	Q_plane = -Q_flight;   // convert from NED to UDB
	R_plane =  R_flight;

	// get the acceleration loading (gravity-acceleration) in the body frame in "g"s,
	// and convert to meter/sec/sec
	// x, y, and z are "UDB" coordinates, x is left wing, y is forward, and z is down.
	grav_accel_x = (float)((XPLMGetDataf(drg_side)) * 9.8);
	grav_accel_y = (float)((XPLMGetDataf(drg_axil)) * 9.8);
	grav_accel_z = (float)((XPLMGetDataf(drg_nrml)) * 9.8);

	Send_NAV_BODYRATES(P_plane, Q_plane, R_plane,
	                   grav_accel_x, grav_accel_y, grav_accel_z);
}

void GetGPSData(void)
{
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

	double local_x   = XPLMGetDataf(drLocal_x);
	double local_y   = XPLMGetDataf(drLocal_y);
	double local_z   = XPLMGetDataf(drLocal_z);
	double local_vx  = XPLMGetDataf(drLocal_vx);
	double local_vy  = XPLMGetDataf(drLocal_vy);
	double local_vz  = XPLMGetDataf(drLocal_vz);
	double air_speed = XPLMGetDataf(drAirSpeedTrue);

	// note: xplane ground speed is not GPS speed over ground,
	// it is 3D ground speed. we need horizontal ground speed for GPS,
	// which is computed from the horizontal local velocity components:
	double speed_over_ground = 100 * sqrt(local_vx*local_vx + local_vz*local_vz);

	// Compute course over ground, in degrees,
	// from horizontal earth frame velocities,
	// which are in OGL frame of reference.
	// local_vx is east, local_vz is south.
	double course_over_ground = (atan2(local_vx, -local_vz) / PI * 180.0);
	// MatrixPilot is expecting an angle between 0 and 360 degrees.
	if (course_over_ground < 0.0) course_over_ground += 360.0;

	Send_NAV_VELNED(LocalSecsInt, local_vx, local_vy, local_vz,
	                air_speed, speed_over_ground, course_over_ground);

////////////////////////////////////////////////////////////////////////////////

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

	// computation is based on zero declination, and zero inclination
	float mag_field_x = 0.0;            // earth OGL x mag field (east)
	float mag_field_y = 0.0;            // earth OGL y mag field (up)
	float mag_field_z = -MAG_FIELD;     // earth OGL z mag field (south)
	// note, the "north pole" of the earth is really a south magnetic pole

	// convert to NED body frame
	OGLtoBCBF(mag_field_x, mag_field_y, mag_field_z, phi, theta, psi);

	// convert from NED body to UDB body frame - now done in Send_NAV_SOL()
//	double mag_field_body_udb_x = -mag_field_y;
//	double mag_field_body_udb_y =  mag_field_x;
//	double mag_field_body_udb_z =  mag_field_z;

	Send_NAV_SOL(LocalSecsInt, LocalSecsFloat, Week, 
	             local_x, local_y, local_z, 
	             local_vx, local_vy, local_vz,
	             mag_field_x, mag_field_y, mag_field_z);

	Send_NAV_POSLLH(LocalSecsInt,
	                XPLMGetDataf(drLat),
	                XPLMGetDataf(drLon),
	                XPLMGetDataf(drElev));
}

float MyFlightLoopCallback(float elapsedMe, float elapsedSim, int counter, void* refcon)
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

	GetBodyRatesData();

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
//	XPLMSetDatavf(drThro, ThrottleSettings, 0, 8);
	XPLMSetDatavf(drThro, ServosThrottleSettings(), 0, 8);

//	static float prevBrakeSetting = PARKBRAKE_ON;
//	if (BrakeSetting != prevBrakeSetting)
//	{
//		prevBrakeSetting = BrakeSetting;
//		XPLMSetDataf(drBrake, BrakeSetting);
////		Log.Append("Set parkbrake to " << BrakeSetting << endl;
//	}
	XPLMSetDataf(drBrake, ServosBrakeSetting());
	return -1;  // get called back on every frame
}
