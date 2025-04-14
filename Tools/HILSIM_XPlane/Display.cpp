#include "stdafx.h"
#include "Display.h"
#include "utility.h"


#define DEBUG_DISPLAY_AXIS 1

static XPLMDataRef drLocal_x;
static XPLMDataRef drLocal_y;
static XPLMDataRef drLocal_z;
static XPLMDataRef drPhi;
static XPLMDataRef drTheta;
static XPLMDataRef drPsi;
static XPLMDataRef drAlpha;
static XPLMDataRef drBeta;

static float fTextColour[3];
static char szString[100];
static XPLMHotKeyID gHotKey = NULL;

static float CamX;
static float CamY;
static float CamZ;

#define CamPathLength 50000
static float CamPath[CamPathLength][3];
#if (DEBUG_DISPLAY_AXIS == 1)
static float CamYaw;
static float CamPitch;
//static float CamRoll;
static int CamPathIterator = 0;
static int CamPathCount = 0;
#endif // (DEBUG_DISPLAY_AXIS == 1)


int MyDrawCallback_Window(XPLMDrawingPhase inPhase, int inIsBefore, void* inRefcon)
{
	// unused
	(void)inPhase;
	(void)inIsBefore;
	(void)inRefcon;

	XPLMDrawString(fTextColour, 300, 740, szString, NULL, xplmFont_Basic);
	return 1;
}

/*
 * MyOrbitPlaneFunc
 * 
 * This is the actual camera control function, the real worker of the plugin.
 * It is called each time X-Plane needs to draw a frame.
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

		BCBFtoOGL(dx, dy, dz, phi, theta, psi);

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

int MyDrawCallback_Objects(XPLMDrawingPhase inPhase,
                           int              inIsBefore,
                           void*            inRefcon)
{
#if (DEBUG_DISPLAY_AXIS == 1)
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

	// Our euler angles:
	// X-Plane angles are in degrees.
	// Phi is roll, roll to right is positive
	// Theta is pitch, pitch up is positive
	// Psi is yaw, relative to north. North is 0/360.

	// Convert these angles to radians first.
	float phi   = XPLMGetDataf(drPhi)   / 180 * PI;
	float theta = XPLMGetDataf(drTheta) / 180 * PI;
	float psi   = XPLMGetDataf(drPsi)   / 180 * PI;
	float alpha = XPLMGetDataf(drAlpha) / 180 * PI;
	float beta  = XPLMGetDataf(drBeta)  / 180 * PI;

	// set up a vertical reference for the plotting computations
	// vertical in earth frame:
	float ax = 0;
	float ay = -(float)9.8;
	float az = 0;

	// Convert from OGL frame to Aircraft body fixed frame
	// This produces a vertical reference in body frame
	OGLtoBCBF(ax, ay, az, phi, theta, psi);

	// Reset the graphics state.  This turns off fog, texturing, lighting,
	// alpha blending or testing and depth reading and writing, which
	// guarantees that our axes will be seen no matter what.
	XPLMSetGraphicsState(0, 0, 0, 0, 0, 0, 0);

	// Do the actual drawing.  use GL_LINES to draw sets of discrete lines.
	// Each one will go 100 meters in any direction from the plane.
	float pointX1 = 0;
	float pointY1 = 0;
	float pointZ1 = 0;
	float pointX2 = 10 * ax;
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
	pointY2 = 10 * -ay;     // convert from NED to UDB
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
	pointZ2 = 10 * az;

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
		glVertex3f(CamPath[j][0], CamPath[j][1], CamPath[j][2]);
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
#endif   // (DEBUG_DISPLAY_AXIS == 1)
	return 1;
}

void DisplayStart(void)
{
	szString[0] = '\0';
	fTextColour[0] = 1.0;
	fTextColour[1] = 1.0;
	fTextColour[2] = 1.0;

	memset(&CamPath, 0, sizeof(float) * 3 * CamPathLength);

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

	// Register our hot key for the new view
	gHotKey = XPLMRegisterHotKey(
	    XPLM_VK_F8,
	    xplm_DownFlag, 
	    "Circling External View",
	    MyHotKeyCallback,
	    NULL);

	XPLMRegisterDrawCallback(
	    MyDrawCallback_Window,
	    xplm_Phase_Window,      // Draw when sim is doing objects
	    0,                      // After objects
	    NULL);                  // No refcon needed

	XPLMRegisterDrawCallback(
	    MyDrawCallback_Objects,
	    xplm_Phase_Objects,     // Draw when sim is doing objects
	    0,                      // After objects
	    NULL);                  // No refcon needed
}

void DisplayStop(void)
{
	XPLMUnregisterHotKey(gHotKey);
	XPLMUnregisterDrawCallback(
	    MyDrawCallback_Objects,
	    xplm_Phase_LastCockpit, 
	    0,
	    NULL);
}

void DisplayEnable(void)
{
	memset(&CamPath, 0, sizeof(float) * 3 * CamPathLength);
}

void DisplayDisable(void)
{
}

