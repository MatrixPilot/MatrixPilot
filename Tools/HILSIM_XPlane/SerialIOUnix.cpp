#include "SerialIO.h"
#include "stdafx.h"

extern "C" {
#include "UDBSocketUnix.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>


SILSocket		sock;


extern LogFile LoggingFile;
extern string	CommPortString;
extern long CommPortSpeed;


//---------------------------------------------------------------------------

void OpenComms(void)
{
	sock = SILSocket_init(SILSocketSerial, 0, (char *)CommPortString.c_str(), CommPortSpeed);
	LoggingFile.mLogFile << "Opened port " << CommPortString.c_str() << endl;
}
//---------------------------------------------------------------------------

void CloseComms(void)
{
	if (sock) {
		SILSocket_close(sock);
		sock = NULL;
	}
}

//---------------------------------------------------------------------------


void StartServer(long PortNum)
{
	sock = SILSocket_init(SILSocketUDPServer, PortNum, NULL, 0);
}

void StopServer(void)
{
	if (sock) {
		SILSocket_close(sock);
		sock = NULL;
	}
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
		
		long n = SILSocket_read(sock, Buffer, BUFLEN);
		if (n < 0) {
			LoggingFile.mLogFile << "serial read failed" << endl;
			StopServer();
		}
		else {
			int i;
			for (i=0; i<n; i++) {
				HandleMsgByte(Buffer[i]);
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
