#include "stdafx.h"
#include "SerialIO.h"


extern "C" {
#include "UDBSocket.h" // MS Visual C++, XCode, linux builds
}


UDBSocket		serialSock = NULL;
UDBSocket		udpSock = NULL;
UDBSocket		activeSock = NULL; // points to one of the other two, only 1 of which remains active


extern LogFile LoggingFile;
extern string	CommPortString;
extern long CommPortSpeed;
extern uint16_t PortNum;


void StartSerial(void);
void StartServer(void);


//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------

void CloseComms(void)
{
	if (serialSock) {
		UDBSocket_close(serialSock);
		serialSock = NULL;
		LoggingFile.mLogFile << "Closed serial port" << endl;
		printf("Closed serial port\n");
	}
	if (udpSock) {
		UDBSocket_close(udpSock);
		udpSock = NULL;
		LoggingFile.mLogFile << "Closed udp port" << endl;
		printf("Closed udp port\n");
	}
	activeSock = NULL;
}

//---------------------------------------------------------------------------


void StartSerial(void)
{
	fprintf(stderr, "--- trying comm port %s\n", CommPortString.c_str());
	serialSock = UDBSocket_init(UDBSocketSerial, 0, NULL, (char *)CommPortString.c_str(), CommPortSpeed);
	if (serialSock) {
		LoggingFile.mLogFile << "Opened serial port " << CommPortString.c_str() << endl;
	}
	else {
		LoggingFile.mLogFile << "Open serial port " << CommPortString.c_str() << " failed." << endl;
		LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
		printf("%s\n", UDBSocketLastErrorMessage());
	}
}


void StartServer(void)
{
	fprintf(stderr, "--- trying server on port %d\n", PortNum);
	udpSock = UDBSocket_init(UDBSocketUDPServer, PortNum, NULL, NULL, 0);
	if (udpSock) {
		LoggingFile.mLogFile << "Opened UDP server on port " << PortNum << endl;
	}
	else {
		LoggingFile.mLogFile << "Open UDP server on port " << PortNum << " failed." << endl;
		LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
		printf("%s\n", UDBSocketLastErrorMessage());
	}
}

//---------------------------------------------------------------------------


void SendToComPort(unsigned long ResponseLength, unsigned char *Buffer)
{
	if (activeSock) {
		int written = UDBSocket_write(activeSock, Buffer, ResponseLength);
		if (written < 0) {
			LoggingFile.mLogFile << "serial write failed" << endl;
			LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
			printf("%s\n", UDBSocketLastErrorMessage());
			CloseComms();
		}
	}
}
//---------------------------------------------------------------------------


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
				LoggingFile.mLogFile << "serial read failed" << endl;
				LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
				printf("%s\n", UDBSocketLastErrorMessage());
				CloseComms();
				break;
			}
			else {
				if (n == 0) {
					break;
				}
				else {
					int i;
					for (i=0; i<n; i++) {
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
		LoggingFile.mLogFile << "Using serial port.  Closed udp port." << endl;
		printf("Using serial port.  Closed udp port.\n");
	}
	if (ReceiveFromSocket(udpSock) && !activeSock) {
		activeSock = udpSock;
		if (serialSock) {
			UDBSocket_close(serialSock);
			serialSock = NULL;
		}
		LoggingFile.mLogFile << "Using udp port.  Closed serial port." << endl;
		printf("Using udp port.  Closed serial port.\n");
	}
}


//---------------------------------------------------------------------------


void ShowMessage(const char *pErrorString)
{
	LoggingFile.mLogFile << "MESSAGE: " << pErrorString << endl;
	cerr << "MESSAGE: " << pErrorString << endl;
}
