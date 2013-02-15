#include "stdafx.h"
#include "SerialIO.h"

#if defined(WIN32) && defined(WIN)
#include "UDBSocket.h" // gcc windows builds
#else
extern "C" {
#include "UDBSocket.h" // MS Visual C++, XCode, linux builds
}
#endif

UDBSocket		sock;


extern LogFile LoggingFile;
extern string	CommPortString;
extern long CommPortSpeed;


//---------------------------------------------------------------------------

int IsConnected(void)
{
	return (sock != NULL);
}


void OpenComms(void)
{
	sock = UDBSocket_init(UDBSocketSerial, 0, (char *)CommPortString.c_str(), CommPortSpeed);
	if (sock) {
		LoggingFile.mLogFile << "Opened serial port " << CommPortString.c_str() << endl;
	}
	else {
		LoggingFile.mLogFile << "Open serial port " << CommPortString.c_str() << " failed." << endl;
		LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
		printf("%s\n", UDBSocketLastErrorMessage());
	}
}
//---------------------------------------------------------------------------

void CloseComms(void)
{
	if (sock) {
		UDBSocket_close(sock);
		sock = NULL;
		LoggingFile.mLogFile << "Closed port" << endl;
	}
}

//---------------------------------------------------------------------------


void StartServer(uint16_t PortNum)
{
	sock = UDBSocket_init(UDBSocketUDPServer, PortNum, NULL, 0);
	if (sock) {
		LoggingFile.mLogFile << "Opened UDP server on port " << PortNum << endl;
	}
	else {
		LoggingFile.mLogFile << "Open UDP server on port " << PortNum << " failed." << endl;
		LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
		printf("%s\n", UDBSocketLastErrorMessage());
	}
}

void StopServer(void)
{
	CloseComms();
}

//---------------------------------------------------------------------------


void SendToComPort(unsigned long ResponseLength, unsigned char *Buffer)
{
	if (sock) {
		int written = UDBSocket_write(sock, Buffer, ResponseLength);
		if (written < 0) {
			LoggingFile.mLogFile << "serial write failed" << endl;
			LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
			printf("%s\n", UDBSocketLastErrorMessage());
			StopServer();
		}
	}
}
//---------------------------------------------------------------------------


#define BUFLEN 512

void ReceiveFromComPort(void)
{
	if (sock) {
		unsigned char Buffer[BUFLEN];
		while (1) {
			long n = UDBSocket_read(sock, Buffer, BUFLEN);
			if (n < 0) {
				LoggingFile.mLogFile << "serial read failed" << endl;
				LoggingFile.mLogFile << UDBSocketLastErrorMessage() << endl;
				printf("%s\n", UDBSocketLastErrorMessage());
				StopServer();
				break;
			}
			else {
				if (n == 0) {
					break;
				}
				else {
					int i;
					for (i=0; i<n; i++) {
						HandleMsgByte(Buffer[i]);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------------


void ShowMessage(const char *pErrorString)
{
	LoggingFile.mLogFile << "MESSAGE: " << pErrorString << endl;
	cerr << "MESSAGE: " << pErrorString << endl;
}
