#include "SerialIO.h"
#include "stdafx.h"

extern "C" {
#include "UDBSocket.h"
}

SILSocket		sock;


extern LogFile LoggingFile;
extern string	CommPortString;
extern long CommPortSpeed;


//---------------------------------------------------------------------------

void OpenComms(void)
{
	sock = SILSocket_init(SILSocketSerial, 0, (char *)CommPortString.c_str(), CommPortSpeed);
	LoggingFile.mLogFile << "Opened serial port " << CommPortString.c_str() << endl;
}
//---------------------------------------------------------------------------

void CloseComms(void)
{
	if (sock) {
		SILSocket_close(sock);
		sock = NULL;
		LoggingFile.mLogFile << "Closed port" << endl;
	}
}

//---------------------------------------------------------------------------


void StartServer(long PortNum)
{
	sock = SILSocket_init(SILSocketUDPServer, PortNum, NULL, 0);
	if (sock) {
		LoggingFile.mLogFile << "Opened serial port " << PortNum << endl;
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
		int written = SILSocket_write(sock, Buffer, ResponseLength);
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
			long n = SILSocket_read(sock, Buffer, BUFLEN);
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
