extern "C" {
#include "UDBSocket.h"
}

#include "SerialIO.h"
#include "stdafx.h"

UDBSocket		sock;


extern LogFile LoggingFile;
extern string	CommPortString;
extern long CommPortSpeed;


//---------------------------------------------------------------------------

void OpenComms(void)
{
	sock = UDBSocket_init(UDBSocketSerial, 0, (char *)CommPortString.c_str(), CommPortSpeed);
	if (sock) {
		LoggingFile.mLogFile << "Opened serial port " << CommPortString.c_str() << endl;
	}
	else {
		LoggingFile.mLogFile << "Open serial port " << CommPortString.c_str() << " failed." << endl;
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
