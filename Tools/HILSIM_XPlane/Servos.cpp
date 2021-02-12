#include "stdafx.h"
#include "Setup.h"
#include "Servos.h"
#include "utility.h"

#define MAX_ITEMS 30

//void SerialPortAccessCallback(XPLMWindowID inWindowID, void* inRefcon);

string   CommPortString = "";           // Place to put the port string to open
long     CommPortSpeed = 38400;
uint16_t PortNum = 14551;

// Here are the variables for implementing the file based control of the setup
Channels ControlSurfaces;               // The list of control surfaces
int store_index = 0;

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

int (*msg_parse)(unsigned char rxChar) = &msgDefault;

#define SERVO_MSG_LENGTH (2*FIXED_SERVO_CHANNELS)

// Servo offsets are a variable so that they can be actively zeroed from the real received offsets
// This offset zero will need to be a menu add on to the plugin.
intbb  ServoOffsets[MAX_VARIABLE_CHANNELS];
unsigned char SERVO_IN[MAX_VARIABLE_CHANNELS*2];
unsigned char SERVO_IN_[MAX_VARIABLE_CHANNELS*2];
//int rxCount = 0;
                                                    // Defaults to standard joystick control
float    ThrottleSettings[8] = {0,0,0,0,0,0,0,0};   // The throttle settings with default values

#define PARKBRAKE_ON  1.0
#define PARKBRAKE_OFF 0.0
#define PARKBRAKE_THROTTLE_THRESHOLD 0.05
float    BrakeSetting = PARKBRAKE_ON;   // Initialise with park brake enabled


float ServosBrakeSetting(void)
{
	return BrakeSetting;
}

float* ServosThrottleSettings(void)
{
	return ThrottleSettings;
}


// return 1 on receiving full, successful packet
int HandleMsgByte(char b)
{
	return (*msg_parse)(b);
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
		Log.Append("msgSync2 faulty start");
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
		Log.Append("msgVarSize faulty value");
		msg_parse = &msgDefault;    // Faulty value
		break;
	default:
		if (var_channel_count > MAX_VARIABLE_CHANNELS)
		{
			Log.Append("msgVarSize faulty start");
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
	if (store_index < (var_channel_count*2))
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

int msgSync1(unsigned char rxChar)
{
	switch (rxChar)
	{
	case 0xEE:
		store_index = 0;
		ck_calc_a = ck_calc_b = 0;
		msg_parse = &msgServos;
		break;
	case 0xFF:
		break;                      // do nothing
	default: 
		Log.Append("msgSync1 error condition");
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
	if ((ck_in_a == ck_calc_a) && (ck_in_b == ck_calc_b))
	{
		memcpy(SERVO_IN, SERVO_IN_, sizeof(SERVO_IN_));
		return 1;   // success!
	}
	Log.Append("msgCheckSum failed");
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

void ServosEnable(string& OverString)
{
	PortNum = 0;

	// Load the setup file on enable. This allows the user to modify the file without exit of XPlane
	SetupFile Setup;
	// Open the setup file and parse it into the control surface list
	Setup.LoadSetupFile(ControlSurfaces, CommPortString, CommPortSpeed, PortNum, OverString);

	// Don't attempt a conection until we're done starting up
	// AttemptConnection();
	SetupDefaultServoZeros();                           // Setup the servo defaults.
}
