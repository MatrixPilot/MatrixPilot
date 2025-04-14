#include "stdafx.h"
#include "Setup.h"
#include "SerialIO.h"

extern "C" {
#include "UDBSocket.h" // MS Visual C++, XCode, linux builds
}

static UDBSocket serialSock = NULL;
static UDBSocket udpSock = NULL;
static UDBSocket activeSock = NULL; // points to one of the other two, only 1 of which remains active

extern string CommPortString;
extern long CommPortSpeed;
extern uint16_t PortNum;

void StartSerial(void);
void StartServer(void);

int IsConnected(void)
{
	return (activeSock != NULL);
}

void OpenComms(void)
{
	if (!serialSock && CommPortString.length() > 0) {
		StartSerial();
	}
	if (!udpSock && PortNum > 0) {
		StartServer();
	}
}

void CloseComms(void)
{
	if (serialSock) {
		UDBSocket_close(serialSock);
		serialSock = NULL;
		Log.Append("Closed serial port");
	}
	if (udpSock) {
		UDBSocket_close(udpSock);
		udpSock = NULL;
		Log.Append("Closed UDP port");
	}
	activeSock = NULL;
}

void StartSerial(void)
{
	serialSock = UDBSocket_init(UDBSocketSerial, 0, NULL, (char*)CommPortString.c_str(), CommPortSpeed);
	if (serialSock) {
		Log.Append("Opened COM port ", CommPortString.c_str());
	} else {
		Log.Append("Failed to open COM port ", CommPortString.c_str());
		Log.Append(UDBSocketLastErrorMessage());
	}
}

void StartServer(void)
{
	udpSock = UDBSocket_init(UDBSocketUDPServer, PortNum, NULL, NULL, 0);
	if (udpSock) {
		Log.Append("Opened UDP server on port ", PortNum);
	} else {
		Log.Append("Failed to open UDP server on port ", PortNum);
		Log.Append(UDBSocketLastErrorMessage());
	}
}

void SendToComPort(unsigned long ResponseLength, unsigned char* Buffer)
{
	if (activeSock) {
		int written = UDBSocket_write(activeSock, Buffer, ResponseLength);
		if (written < 0) {
			Log.Append("serial write failed");
			Log.Append(UDBSocketLastErrorMessage());
			CloseComms();
		}
	}
}

#define BUFLEN 512

int ReceiveFromSocket(UDBSocket sock)
{
	int didReceive = 0;
	
	if (sock) {
		unsigned char Buffer[BUFLEN];
		int loops = 10;
		while (loops--) {
			long n = UDBSocket_read(sock, Buffer, BUFLEN);
			if (n < 0) {
				Log.Append("socket read failed");
				Log.Append(UDBSocketLastErrorMessage());
				CloseComms();
				break;
			} else {
				if (n == 0) {
					break;
				} else {
					int i;
					for (i = 0; i < n; i++) {
						if (HandleMsgByte(Buffer[i]) == 1) {
							didReceive = 1;
						}
					}
				}
			}
		}
	}
	return didReceive;
}

void ReceiveFromComPort(void)
{
	if (ReceiveFromSocket(serialSock) && !activeSock) {
		activeSock = serialSock;
		if (udpSock) {
			UDBSocket_close(udpSock);
			udpSock = NULL;
		}
		Log.Append("Using COM port.  Closed UDP port.");
	}
	if (ReceiveFromSocket(udpSock) && !activeSock) {
		activeSock = udpSock;
		if (serialSock) {
			UDBSocket_close(serialSock);
			serialSock = NULL;
		}
		Log.Append("Using UDP port.  Closed COM port.");
	}
}

void ShowMessage(const char* pErrorString)
{
	Log.Append("MESSAGE: ", pErrorString);
}
