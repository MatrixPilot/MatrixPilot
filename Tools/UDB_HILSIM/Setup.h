#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iterator>

using namespace std;

/*************************************************************************************
 HILSIM setup loading from HILSIMSetup.txt in the XPLane executable directory

 Matthew Coleman - 10-05-05
 
 Using the following format

 Comm,COMx
 Overide,***Overide path description****
 Control,Servo Channel,Xplane control surface path, Servo zero, Servo Hi Travel, Servo Lo Travel, Surafe Max travel, Serface Min Travel
 Engine,Servo Channel,Engine Mask,Servo Zero,Servo Hi Travel,Servo Low Travel, Engine Max, Engine Min

To comment out any line in the setup, change the first word before the comma.  

The surface and overide paths are defined in the xsquarkbox data references file
http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html

If you need simple control choose the joystick overide.
If you need to translate servos to flying surfaces choose full surface overide.

Surface travel is normally in degrees for advanced mode.  Read the XPlane documentation
Surface travel is normally +-1 for simple joystick overide mode

Servo Hi and Low travel is defined by the relative count travelled from zero, 
	not the absolute value.

The engine mask is a bitfield Bit0 = engine 1, Bit 7 = engine 8.

If you need to control all engines together on one channel, choose mask 256

Use your transmitter and/or autopilot to do the mixing.

THIS SETUP WILL NOT DO INTER-CHANNEL MIXING.  This feature is to be added if needed. 
This might be a problem for some heli setups.

LOGFILE
The plugin will create a logfile in the XPlane executable directory.  
It will give you details of what has been loaded from the setup file.

**************************************************************************************/

#define	SERVO_CHANNELS	8

#define EngineString	"Engine"
#define ControlString	"Control"
#define CommString		"Comm"
#define OverideString	"Overide"

typedef enum
{	
	DATAPOS_CONTROL_DATATYPE = 0,
	DATAPOS_CONTROL_SERVO_CHAN,
	DATAPOS_CONTROL_IDENTITY,
	DATAPOS_CONTROL_SERVO_ZERO,
	DATAPOS_CONTROL_SERVO_HI_TRAVEL,
	DATAPOS_CONTROL_SERVO_LO_TRAVEL,
	DATAPOS_CONTROL_SURFACE_MAX,
	DATAPOS_CONTROL_SURFACE_MIN,
	DATAPOS_CONTROL_LAST_INDEX,
} ControlDataPos;


typedef enum
{	
	DATAPOS_ENGINE_DATATYPE = 0,
	DATAPOS_ENGINE_SERVO_CHAN,
	DATAPOS_ENGINE_MASK,				// An 8 bit mask of which engines this servo drives
	DATAPOS_ENGINE_SERVO_ZERO,
	DATAPOS_ENGINE_SERVO_HI_TRAVEL,
	DATAPOS_ENGINE_SERVO_LO_TRAVEL,
	DATAPOS_ENGINE_MAX,					// Keep MAX-MIN for engine reversing
	DATAPOS_ENGINE_MIN,
	DATAPOS_ENGINE_LAST_INDEX,
} EngineDataPos;


typedef enum
{	
	DATAPOS_OVERIDE_DATATYPE = 0,
	DATAPOS_OVERIDE_IDENTITY,
	DATAPOS_OVERIDE_LAST_INDEX,
} OverideDataPos;


typedef enum
{	
	DATAPOS_COMM_DATATYPE = 0,
	DATAPOS_COMM_IDENTITY,
	DATAPOS_COMM_LAST_INDEX
} CommDataPos;


typedef enum
{
	CONTROL_TYPE_NULL = 0,
	CONTROL_TYPE_SURFACE,
	CONTROL_TYPE_ENGINE
} ControlType;


class ChannelSetup
{
public:
	ChannelSetup();									// Default constructor
	ChannelSetup(const ChannelSetup* pCopyChannel);	// Copy constructor

	float GetControlDeflection(int ServoPosition);	// Translate from servo position to control deflection

	int mServoChannel;							// Servo channel variables
	int	mChannelOffset;
	int	mChannelHiTravel;
	int	mChannelLoTravel;

	XPLMDataRef	mControlSurfaceRef;

	float mMaxDeflection;						// Deflection at channel max.  THIS IS NOT A LIMITER
	float mMinDeflection;						// Deflection at channel min.  THIS IS NOT A LIMITER

	ControlType	mControlType;
	unsigned int mEngineMask;
};


class Channels : public vector<ChannelSetup>
{
};


class SetupFile
{
public:
	void LoadSetupFile(Channels& ChannelInfo, string& CommStr, string& OverideStr);

	void ParseLine(string& ParseString, Channels &ChannelInfo, string& CommStr, string& OverideStr);

	void ParseControlLine(string& ParseString, Channels &ChannelInfo);
	void ParseControlString(string& ValueString, int Index, ChannelSetup* pSetup);

	void ParseEngineLine(string& ParseString, Channels &ChannelInfo);
	void ParseEngineString(string& ValueString, int Index, ChannelSetup* pSetup);

	void ParseCommLine(string& ParseString, string& CommStr);

	void ParseOverideLine(string& ParseString, string& OverideStr);
};


class LogFile
{
public:
	LogFile();
	~LogFile();
	void AppendString(string& AddString);

	ofstream mLogFile;
};

static LogFile LoggingFile;
