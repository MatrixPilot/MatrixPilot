#include "stdafx.h"
#include "Setup.h"

/*************************************************************************************
 HILSIM setup loading from HILSIMSetup.txt in the XPLane executable directory

 Matthew Coleman - 10-05-05
 
 Using the following format

 Comm,COMx (or on a mac, something like: Comm,/dev/cu.usbserial-A600akaF)
 Overide,***Overide path description****
 Control,Servo Channel,Xplane control surface path, Servo zero, Servo Hi Travel, Servo Lo Travel, Surface Max travel, Surface Min Travel
 Engine,Servo Channel,Engine Mask,Servo Zero,Servo Hi Travel,Servo Low Travel, Engine Max, Engine Min

To comment out any line in the setup, change the first word before the comma.  

The surface and overide paths are defined in the xsquarkbox data references file
http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html

If you need simple control choose the joystick overide.
If you need to translate servos to flying surfaces choose full surface overide.

Surface travel is normally in degrees.  Read the XPlane documentation
Servo Hi and Low travel is defined by the relative count travelled from zero, 
	not the absolute value.

The engine mask is a bitfield Bit0 = engine 1, Bit 7 = engine 8.

If you need to control all engines together on one channel, choose mask 255

Use your transmitter and/or autopilot to do the mixing.

THIS SETUP WILL NOT DO INTER-CHANNEL MIXING.  This feature is to be added if needed. 
This might be a problem for some heli setups.

LOGFILE
The plugin will create a logfile in the XPlane executable directory.  
It will give you details of what has been loaded from the setup file.

**************************************************************************************/

// Default constructor for setups
ChannelSetup::ChannelSetup()
{
	mServoChannel      = 0;
	mChannelOffset     = 3000;
	mChannelLoTravel   = 1000;
	mChannelHiTravel   = 1000;
	mControlSurfaceRef = 0;
	mMaxDeflection     = 20;
	mMinDeflection     = -20;
	mControlType       = CONTROL_TYPE_NULL;
	mEngineMask        = 0;
}

// Copy constructor for setups
ChannelSetup::ChannelSetup(const ChannelSetup* pCopyChannel)
{
	mServoChannel      = pCopyChannel->mServoChannel;
	mChannelOffset     = pCopyChannel->mChannelOffset;
	mChannelHiTravel   = pCopyChannel->mChannelHiTravel;
	mChannelLoTravel   = pCopyChannel->mChannelLoTravel;
	mControlSurfaceRef = pCopyChannel->mControlSurfaceRef;
	mMaxDeflection     = pCopyChannel->mMaxDeflection;
	mMinDeflection     = pCopyChannel->mMinDeflection;
	mControlType       = pCopyChannel->mControlType;
	mEngineMask        = pCopyChannel->mEngineMask;
}

// Calculate control deflection from the setup
float ChannelSetup::GetControlDeflection(int ServoPosition)
{
	int ServoDiff = ServoPosition - mChannelOffset;
	float Deflection = 0;

	if (ServoDiff > 0)
	{
		if (mChannelHiTravel)
		{
			Deflection = float(ServoDiff) * mMaxDeflection / float(mChannelHiTravel);
		}
	}
	else
	{
		if (mChannelLoTravel)
		{
			Deflection = float(ServoDiff) * mMinDeflection / float(mChannelLoTravel);
		}
	}
	return Deflection;
}

/*
ChannelSetup::ChannelSetup(int mChannelOffset, int mChannelMax, int mChannelMin, SurfaceType mControlSurface, float mMaxDeflection, float mMinDeflection)
{
}
*/

// Reload the setup file
void SetupFile::LoadSetupFile(Channels &ChannelInfo, string& CommStr, long& CommSpeed, uint16_t& PortNum, string& OverideStr)
{
	string FileLine;

	ChannelInfo.clear();
	ifstream ChannelFile("HILSIMSetup.txt");
	if (ChannelFile.is_open())
	{
		string FoundLine = string("Found File Line: ");
		while (!ChannelFile.eof())
		{
			getline(ChannelFile, FileLine);
			size_t endpos = FileLine.find_last_not_of(" \t\r\n");
			if (string::npos != endpos)
				FileLine = FileLine.substr(0, endpos + 1);
			LoggingFile.AppendString(FoundLine);
			LoggingFile.AppendString(FileLine);
			LoggingFile.mLogFile << endl;
			ParseLine(FileLine, ChannelInfo, CommStr, CommSpeed, PortNum, OverideStr);
		}
		ChannelFile.close();
	}
}

// Parse a line of the setup file
void SetupFile::ParseLine(string& ParseString, Channels& ChannelInfo, string& CommStr, long& CommSpeed, uint16_t& PortNum, string& OverideStr)
{
	size_t iSearchPos = 0;        // The next position found for a delimeter

	string TypeStr;
	TypeStr.resize(60);
	TypeStr.erase();

	iSearchPos = ParseString.find(",", 0);
	if (iSearchPos == ParseString.npos) return;

	TypeStr.append(ParseString, 0, iSearchPos);
	if (TypeStr == ControlString)
	{
		LoggingFile.mLogFile << "Parse Control Line :";
		ParseControlLine(ParseString, ChannelInfo);
	}
	else if (TypeStr == CommString)
	{
		LoggingFile.mLogFile << "Parse Comm :";
		ParseCommLine(ParseString, CommStr, CommSpeed);
	}
	else if (TypeStr == PortString)
	{
		LoggingFile.mLogFile << "Parse Server Port :";
		ParsePortLine(ParseString, PortNum);
	}
	else if (TypeStr == EngineString)
	{
		LoggingFile.mLogFile << "Parse Engine Line :";
		ParseEngineLine(ParseString, ChannelInfo);
	}
	else if (TypeStr == OverideString)
	{
		ParseOverideLine(ParseString, OverideStr);
	}
	else
	{
		LoggingFile.mLogFile << "Did not understand information type";
		LoggingFile.mLogFile << endl;
	}
}

// Parse a Control line in the setup file
void SetupFile::ParseControlLine(string& ParseString, Channels &ChannelInfo)
{
	int iValueIndex = 0;            // Index of the number of values found
	size_t iSearchPos = 0;          // The next position found for a delimeter;
	size_t iLastPos = 0;            // The last position found for a delimeter;
	bool EndFound = false;

	ChannelSetup ParseSetup;
	ChannelSetup* pParseSetup = (ChannelSetup*)&ParseSetup; // new ChannelSetup();

	string FindStr;
	FindStr.resize(60);
	do {
		FindStr.erase();
		iSearchPos = ParseString.find(",", iLastPos);
		if (iSearchPos == ParseString.npos)
		{
			FindStr.append(ParseString, iLastPos, ParseString.length() - iLastPos);
			EndFound = true;
		}
		else
		{
			FindStr.append(ParseString, iLastPos, iSearchPos - iLastPos);
		}
		ParseControlString(FindStr, iValueIndex, pParseSetup);
		iValueIndex++;
		iSearchPos++;               // Increment search position to avoid repeat finding the same character
		iLastPos = iSearchPos;
	} while (!EndFound);

	// Only create new data channel if we have the right number of data points
	if (iValueIndex == DATAPOS_CONTROL_LAST_INDEX)
	{
		ChannelSetup* pNewSetup = new ChannelSetup(pParseSetup);
		ChannelInfo.push_back(pNewSetup);
		LoggingFile.mLogFile << "New channel added: ";
	}
	else
	{
		LoggingFile.mLogFile << "New channel failed: ";
	}
	LoggingFile.mLogFile << endl;
}

// Parse a Control string in the setup file
void SetupFile::ParseControlString(string& ValueString, int Index, ChannelSetup* pSetup)
{
	LoggingFile.mLogFile << Index;
	LoggingFile.mLogFile << ":";
	LoggingFile.mLogFile << ValueString;
	LoggingFile.mLogFile << " ";

	switch (Index)
	{
		case DATAPOS_CONTROL_DATATYPE:
			pSetup->mControlType = CONTROL_TYPE_SURFACE;
			break;
		case DATAPOS_CONTROL_SERVO_CHAN:
			pSetup->mServoChannel = strtol(ValueString.data(), NULL, 10) - 1;
			break;
		case DATAPOS_CONTROL_IDENTITY:
			pSetup->mControlSurfaceRef = XPLMFindDataRef(ValueString.data()); 
			break;
		case DATAPOS_CONTROL_SERVO_ZERO:
			pSetup->mChannelOffset = strtol(ValueString.data(), NULL, 10);
			break;
		case DATAPOS_CONTROL_SERVO_HI_TRAVEL:
			pSetup->mChannelHiTravel = strtol(ValueString.data(), NULL, 10);
			break;
		case DATAPOS_CONTROL_SERVO_LO_TRAVEL:
			pSetup->mChannelLoTravel = strtol(ValueString.data(), NULL, 10);
			break;
		case DATAPOS_CONTROL_SURFACE_MAX:
			pSetup->mMaxDeflection = (float)strtod(ValueString.data(), NULL);
			break;
		case DATAPOS_CONTROL_SURFACE_MIN:
			pSetup->mMinDeflection = (float)strtod(ValueString.data(), NULL);
			break;
		default:
			break;
	}
}

void SetupFile::ParseEngineLine(string& ParseString, Channels &ChannelInfo)
{
	int iValueIndex = 0;            // Index of the number of values found
	size_t iSearchPos = 0;          // The next position found for a delimeter;
	size_t iLastPos = 0;            // The last position found for a delimeter;
	bool EndFound = false;

	ChannelSetup ParseSetup;
	ChannelSetup* pParseSetup = (ChannelSetup*)&ParseSetup; // new ChannelSetup();

	string FindStr;
	string TypeStr;
	FindStr.resize(60);
	do {
		FindStr.erase();
		iSearchPos = ParseString.find(",", iLastPos);
		if (iSearchPos == ParseString.npos)
		{
			FindStr.append(ParseString, iLastPos, ParseString.length() - iLastPos);
			EndFound = true;
		}
		else
		{
			FindStr.append(ParseString, iLastPos, iSearchPos - iLastPos);
		}
		ParseEngineString(FindStr, iValueIndex, pParseSetup);
		iValueIndex++;
		iSearchPos++;               // Increment search position to avoid repeat finding the same character
		iLastPos = iSearchPos;
	} while (!EndFound);

	// Only create new data channel if we have the right number of data points
	if (iValueIndex == DATAPOS_ENGINE_LAST_INDEX)
	{
		ChannelSetup* pNewSetup = new ChannelSetup(pParseSetup);
		ChannelInfo.push_back(pNewSetup);
		LoggingFile.mLogFile << "New engine added: ";
	}
	else
	{
		LoggingFile.mLogFile << "New engine failed: ";
	}
}

void SetupFile::ParseEngineString(string& ValueString, int Index, ChannelSetup* pSetup)
{
	LoggingFile.mLogFile << Index;
	LoggingFile.mLogFile << ":";
	LoggingFile.mLogFile << ValueString;
	LoggingFile.mLogFile << " ";

	switch (Index)
	{
		case DATAPOS_ENGINE_DATATYPE:
			pSetup->mControlType = CONTROL_TYPE_ENGINE;
			break;
		case DATAPOS_ENGINE_SERVO_CHAN:
			pSetup->mServoChannel = strtol(ValueString.data(), NULL, 10) - 1;
			break;
		case DATAPOS_ENGINE_MASK:
			pSetup->mEngineMask = strtol(ValueString.data(), NULL, 10);
			break;
		case DATAPOS_ENGINE_SERVO_ZERO:
			pSetup->mChannelOffset = strtol(ValueString.data(), NULL, 10);
			break;
		case DATAPOS_ENGINE_SERVO_HI_TRAVEL:
			pSetup->mChannelHiTravel = strtol(ValueString.data(), NULL, 10);
			break;
		case DATAPOS_ENGINE_SERVO_LO_TRAVEL:
			pSetup->mChannelLoTravel = strtol(ValueString.data(), NULL, 10);
			break;
		case DATAPOS_ENGINE_MAX:
			pSetup->mMaxDeflection = (float)strtod(ValueString.data(), NULL);
			break;
		case DATAPOS_ENGINE_MIN:
			pSetup->mMinDeflection = (float)strtod(ValueString.data(), NULL);
			break;
		default:
			break;
	}
}

void SetupFile::ParseCommLine(string& ParseString, string& CommStr, long& CommSpeed)
{
	size_t iSearchPos = 0;    // The next position found for a delimeter;

	iSearchPos = ParseString.find(",", 0);
	if (iSearchPos == ParseString.npos) return;

	iSearchPos++;
	size_t sSearchPos = ParseString.find(",", iSearchPos);
	if (sSearchPos == ParseString.npos)
	{
		CommStr.clear();
		CommStr.append(ParseString, iSearchPos, ParseString.length() - iSearchPos);
		CommSpeed = 19200;
	}
	else
	{
		sSearchPos++;
		CommStr.clear();
		CommStr.append(ParseString,iSearchPos, sSearchPos-iSearchPos-1);
		CommSpeed = strtol(ParseString.substr(sSearchPos, ParseString.length() - sSearchPos).data(), NULL, 10);
	}
	LoggingFile.mLogFile << "Comm port set for :";
	LoggingFile.mLogFile << CommStr;
	LoggingFile.mLogFile << " at ";
	LoggingFile.mLogFile << CommSpeed;
	LoggingFile.mLogFile << endl;
}

void SetupFile::ParsePortLine(string& ParseString, uint16_t& PortNum)
{
	size_t iSearchPos = 0;    // The next position found for a delimeter;

	iSearchPos = ParseString.find(",", 0);
	if (iSearchPos == ParseString.npos) return;

	iSearchPos++;
	PortNum = (uint16_t)strtol(ParseString.substr(iSearchPos, ParseString.length() - iSearchPos).data(), NULL, 10);
	LoggingFile.mLogFile << "Server port set for: ";
	LoggingFile.mLogFile << PortNum;
	LoggingFile.mLogFile << endl;
}

void SetupFile::ParseOverideLine(string& ParseString, string& OverideStr)
{
	size_t iSearchPos = 0;    // The next position found for a delimeter;

	iSearchPos = ParseString.find(",", 0);
	if (iSearchPos == ParseString.npos) return;

	iSearchPos++;
	OverideStr.clear();
	OverideStr.append(ParseString, iSearchPos, ParseString.length() - iSearchPos);
	LoggingFile.mLogFile << "Overide set for :";
	LoggingFile.mLogFile << OverideStr;
	LoggingFile.mLogFile << endl;
}

LogFile::LogFile()
{
	mLogFile.open("HILSIMLogFile.txt");
}

LogFile::~LogFile()
{
	if (mLogFile.is_open()) mLogFile.close();
}

void LogFile::AppendString(string& AddString)
{
	mLogFile << AddString << endl;
}

LogFile LoggingFile;
