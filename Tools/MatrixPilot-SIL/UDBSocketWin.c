//
//  UDBSocketWin.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#if (WIN == 1 || WIN32 == 1) // WIN defined in GGC builds, WIN32 defined in VC++

#include "UDBSocket.h"

#ifdef WIN32
#define snprintf _snprintf
#else
#include <Windows.h> // don't include if building with Visual Studio
#define _strdup strdup
#endif

#include <conio.h>
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define true 1
#define false 0

#define LAST_ERR_BUF_SIZE 256
char UDBSocketLastError[LAST_ERR_BUF_SIZE];

static WSADATA wsa;
static uint8_t hasInitializedWSA = 0;

struct UDBSocket_t {
	HANDLE             hComms;
	int                fd;
	struct sockaddr_in si_other;
	UDBSocketType      type;
	uint16_t           UDP_port;
	char*              UDP_host;
	char*              serial_port;
	long               serial_baud;
} UDBSocket_t;

void SetTermIOs(void)
{
	// Disable buffering.
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);
}

UDBSocket UDBSocket_init(UDBSocketType type, uint16_t UDP_port, char* UDP_host, char* serial_port, long serial_baud)
{
	UDBSocket newSocket = (UDBSocket)malloc(sizeof(UDBSocket_t));
	if (!newSocket)
	{
		return NULL;
	}
	memset((char*)newSocket, 0, sizeof(UDBSocket_t));
	newSocket->type = type;
	newSocket->UDP_port = UDP_port;
	newSocket->UDP_host = (UDP_host) ? _strdup(UDP_host) : NULL;
	newSocket->serial_port = (serial_port) ? _strdup(serial_port) : NULL;
	newSocket->serial_baud = serial_baud;

	switch (newSocket->type)
	{
		case UDBSocketStandardInOut:
		{
			SetTermIOs();
			break;
		}
		case UDBSocketUDPClient:
		{
			u_long on = 1;
			if (!hasInitializedWSA)
			{
				if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
				{
					snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "WSAStartup Failed. Error Code : %d", WSAGetLastError());
					return NULL;
				}
				hasInitializedWSA = 1;
			}
			if ((newSocket->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
			{
				snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "socket() failed.  Error Code : %d", WSAGetLastError());
				free(newSocket);
				return NULL;
			}
			if (ioctlsocket(newSocket->fd, FIONBIO, &on) < 0)
			{
				snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "ioctl() failed.  Error Code : %d", WSAGetLastError());
				UDBSocket_close(newSocket);
				return NULL;
			}
			memset((char*) &newSocket->si_other, 0, sizeof(newSocket->si_other));
			newSocket->si_other.sin_family = AF_INET;
			newSocket->si_other.sin_port = htons(newSocket->UDP_port);
			newSocket->si_other.sin_addr.S_un.S_addr = inet_addr(UDP_host);

			UDBSocket_write(newSocket, (unsigned char*)"", 0); // Initiate connection
			break;
		}
		case UDBSocketUDPServer:
		{
			struct sockaddr_in si_me;
			u_long on = 1;

			if (!hasInitializedWSA)
			{
				if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
				{
					snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "WSAStartup Failed. Error Code : %d", WSAGetLastError());
					return NULL;
				}
				hasInitializedWSA = 1;
			}
			if ((newSocket->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
			{
				snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "socket() Error Code : %d", WSAGetLastError());
				free(newSocket);
				return NULL;
			}
			if (ioctlsocket(newSocket->fd, FIONBIO, &on) < 0)
			{
				snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "ioctl() Error Code : %d", WSAGetLastError());
				UDBSocket_close(newSocket);
				return NULL;
			}
			newSocket->si_other.sin_family = AF_INET;
			memset((char*) &si_me, 0, sizeof(si_me));
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(newSocket->UDP_port);
			si_me.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(newSocket->fd, (const struct sockaddr*)&si_me, sizeof(si_me)) == -1)
			{
				snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "bind() Error Code : %d", WSAGetLastError());
				UDBSocket_close(newSocket);
				return NULL;
			}
			break;
		}
		case UDBSocketSerial:
		{
			DCB Dcb;
			COMMTIMEOUTS CommTimeouts;
			DWORD dwRetFlag;

			if (newSocket->hComms == 0)
			{
				char port[512]; // sketchy, but should be plenty long
				strcpy(port, "\\\\.\\");
				strncpy(port+strlen(port), newSocket->serial_port, 512-6);
				newSocket->hComms = CreateFile(port,
				    GENERIC_READ | GENERIC_WRITE,
				    0,
				    NULL,
				    OPEN_EXISTING,
				    FILE_ATTRIBUTE_NORMAL,// | FILE_FLAG_RANDOM_ACCESS,
				    NULL);
				
				if (newSocket->hComms == INVALID_HANDLE_VALUE)
				{
					//sprintf(ErrorString, "Could not open comm port");
					//ShowMessage(ErrorString);
					//LoggingFile.mLogFile << "Could not open Com Port";
					//LoggingFile.mLogFile << endl;
					snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "Could not open comm port");
					UDBSocket_close(newSocket);
					return NULL;
				}
				else
				{
					dwRetFlag = GetCommState(newSocket->hComms, &Dcb);
					if (!dwRetFlag)
					{
						//sprintf(ErrorString, "GetCommState Error = %d", GetLastError());
						//ShowMessage(ErrorString);
						snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "GetCommState Error = %lu", GetLastError());
						UDBSocket_close(newSocket);
						return NULL;
					}
					Dcb.DCBlength = sizeof(Dcb);
					Dcb.BaudRate = newSocket->serial_baud;
					Dcb.ByteSize = 8;
					Dcb.Parity = NOPARITY;
					Dcb.StopBits = ONESTOPBIT;
					Dcb.fTXContinueOnXoff = true;
					Dcb.fOutxCtsFlow = false;                 // disable CTS output flow control
					Dcb.fOutxDsrFlow = false;                 // disable DSR output flow control
					Dcb.fDtrControl = DTR_CONTROL_DISABLE;    // DTR_CONTROL_HANDSHAKE  /*DTR_CONTROL_DISABLE DTR_CONTROL_ENABLE*/;
					Dcb.fDsrSensitivity = false;              // enable DSR sensitivity
					Dcb.fOutX = false;                        // disable XON/XOFF out flow control
					Dcb.fInX = false;                         // disable XON/XOFF in flow control
					Dcb.fErrorChar = false;                   // disable error replacement
					Dcb.fNull = false;                        // disable null stripping
					Dcb.fRtsControl = RTS_CONTROL_DISABLE;    // RTS_CONTROL_HANDSHAKE	  /* RTS_CONTROL_ENABLE  RTS_CONTROL_DISABLE*/;   //  enable RTS line
					Dcb.fAbortOnError = true;                 // don't abort reads/writes on error

					dwRetFlag = SetCommState(newSocket->hComms, &Dcb);
					if (!dwRetFlag)
					{
						//sprintf(ErrorString, "SetCommState Error = %d", GetLastError());
						//ShowMessage(ErrorString);
						snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "SetCommState Error = %lu", GetLastError());
						UDBSocket_close(newSocket);
						return NULL;
					}
					dwRetFlag = GetCommTimeouts(newSocket->hComms, &CommTimeouts);
					if (!dwRetFlag)
					{
						//sprintf(ErrorString, "GetCommTimeouts Error = %d", GetLastError());
						//ShowMessage(ErrorString);
						snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "GetCommTimeouts Error = %lu", GetLastError());
						UDBSocket_close(newSocket);
						return NULL;
					}
					CommTimeouts.ReadIntervalTimeout         = MAXDWORD;    //Don't use interval timeouts
					CommTimeouts.ReadTotalTimeoutMultiplier  = 0;           //Don't use multipliers
					CommTimeouts.ReadTotalTimeoutConstant    = 0;           //150ms total read timeout
					CommTimeouts.WriteTotalTimeoutMultiplier = 0;           //Don't use multipliers
					CommTimeouts.WriteTotalTimeoutConstant   = 0;           //2200ms total write timeout
					
					dwRetFlag = SetCommTimeouts(newSocket->hComms, &CommTimeouts);
					if (!dwRetFlag)
					{
						//sprintf(ErrorString, "SetCommTimeouts Error = %d", GetLastError());
						//ShowMessage(ErrorString);
						snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "SetCommTimeouts Error = %lu", GetLastError());
						UDBSocket_close(newSocket);
						return NULL;
					}
				}
			}
			else
			{
				//ShowMessage("Comm port already open");
				snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "Com Port already open");
				UDBSocket_close(newSocket);
				return NULL;
			}
			break;
		}
		default:
			break;
	}
	return newSocket;
}

void ResetTermIOs(void)
{
	// nothing needed
}

void UDBSocket_close(UDBSocket socket)
{
	switch (socket->type)
	{
		case UDBSocketStandardInOut:
		{
			ResetTermIOs();
			break;
		}
		case UDBSocketUDPClient:
		case UDBSocketUDPServer:
		{
			closesocket(socket->fd);
			if (socket->UDP_host) free(socket->UDP_host);
			if (socket->serial_port) free(socket->serial_port);
			free(socket);
			break;
		}
		case UDBSocketSerial:
		{
			if (socket->hComms != 0)
			{
				CloseHandle(socket->hComms);
				socket->hComms = 0;
			}
			else
			{
				//ShowMessage("Comm port already closed");
			}
			break;
		}
		default:
			break;
	}
}

int UDBSocket_read(UDBSocket socket, unsigned char* buffer, int bufferLength)
{
	switch (socket->type)
	{
		case UDBSocketStandardInOut:
		{
			// Check for input on stdin
			int pos = 0;
			while (_kbhit())
			{
				buffer[pos++] = _getch();
			}
			return pos;
		}
		case UDBSocketUDPClient:
		case UDBSocketUDPServer:
		{
			struct sockaddr_in from;
			int fromLength = sizeof(from);
			int received_bytes = (int)recvfrom(socket->fd, (char*)buffer, bufferLength, 0,
			                                   (struct sockaddr*)&from, &fromLength);
			if (received_bytes < 0)
			{
				if (WSAGetLastError() != WSAEWOULDBLOCK && WSAGetLastError() != WSAECONNRESET)
				{
					snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "recvfrom() failed: %d", WSAGetLastError());
					return -1;
				}
				return 0;
			}
			if (socket->type == UDBSocketUDPServer)
			{
				socket->si_other.sin_port = from.sin_port;
				socket->si_other.sin_addr = from.sin_addr;
			}
			if (received_bytes < 0) return 0;
			return (int)received_bytes;
		}
		case UDBSocketSerial:
		{
			unsigned long bytesTransferred = 0;
			//DWORD dwRetFlag;
			//char ErrorString[80];

			if (socket->hComms != INVALID_HANDLE_VALUE)
			{
				// Read the data from the serial port.
				if (ReadFile(socket->hComms, buffer, bufferLength, &bytesTransferred, 0) == 0)
				{
					COMSTAT comStat;
					unsigned long dwErrors;

					//sprintf(ErrorString, "ReadFile Error = %d", GetLastError());
					//ShowMessage(ErrorString);
					snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "ReadFile() failed");
					ClearCommError(socket->hComms, &dwErrors, &comStat);
					return -1;
				}
			}
			return bytesTransferred;
		}
		default:
			break;
	}
	return -1;
}

int UDBSocket_write(UDBSocket socket, const unsigned char* data, int dataLength)
{
	switch (socket->type)
	{
		case UDBSocketStandardInOut:
		{
			int i;
			for (i = 0; i < dataLength; i++)
			{
				_putch(data[i]);
			}
			fflush(stdout);
			return i;
		}
		case UDBSocketUDPClient:
		case UDBSocketUDPServer:
		{
			int bytesWritten;
			if (socket->type == UDBSocketUDPServer && socket->si_other.sin_port == 0)
			{
				UDBSocket_read(socket, NULL, 0);
				if (socket->si_other.sin_port == 0)
				{
					return 0;
				}
			}
			bytesWritten = (int)sendto(socket->fd, (char*)data, dataLength, 0, (const struct sockaddr*)&socket->si_other, sizeof(socket->si_other));
			if (bytesWritten < 0)
			{
				snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "sendto() Error Code : %d", WSAGetLastError());
				return -1;
			}
			return bytesWritten;
		}
		case UDBSocketSerial:
		{
			unsigned long bytesWritten;
			DWORD dwRetFlag;
			if (socket->hComms != 0)
			{
				dwRetFlag = WriteFile(socket->hComms, data, dataLength, &bytesWritten, NULL);
				if (!dwRetFlag)
				{
					snprintf(UDBSocketLastError, LAST_ERR_BUF_SIZE, "WriteFile Error = %lu", GetLastError());
					//ShowMessage(ErrorString);
					return -1;
				}
			}
			else
			{
				//ShowMessage("Comm port not open");
			}
			return bytesWritten;
		}
		default:
			break;
	}
	return -1;
}

char* UDBSocketLastErrorMessage(void)
{
	return UDBSocketLastError;
}

#endif // (WIN == 1)
