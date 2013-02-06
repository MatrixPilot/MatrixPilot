//
//  SIL-socket.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include "SIL-socket.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>


#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define LOCALHOST_IP "127.0.0.1"


struct SILSocket_t {
	int					fd;
	struct sockaddr_in	si_other;
	SILSocketType		type;
	long				UDP_port;
	char*				serial_port;
	long				serial_baud;
} SILSocket_t;


SILSocket SILSocket_init(SILSocketType type, long UDP_port, char *serial_port, long serial_baud)
{
	SILSocket newSocket = malloc(sizeof(SILSocket_t));
	if (!newSocket) {
		return NULL;
	}
	
	memset((char *)newSocket, 0, sizeof(SILSocket_t));
	newSocket->type = type;
	newSocket->UDP_port = UDP_port;
	newSocket->serial_port = (serial_port) ? strdup(serial_port) : NULL;
	newSocket->serial_baud = serial_baud;
	
	switch (newSocket->type) {
		case SILSocketUDPClient:
		{
			if ((newSocket->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
				perror("socket() failed");
				free(newSocket);
				return NULL;
			}
			
			int on = 1;
			if (ioctl(newSocket->fd, FIONBIO, (char *)&on) < 0)
			{
				perror("ioctl() failed");
				SILSocket_close(newSocket);
				return NULL;
			}
			
			memset((char *) &newSocket->si_other, 0, sizeof(newSocket->si_other));
			newSocket->si_other.sin_family = AF_INET;
			newSocket->si_other.sin_port = htons(newSocket->UDP_port);
			if (inet_aton(LOCALHOST_IP, &newSocket->si_other.sin_addr) == 0) {
				fprintf(stderr, "inet_aton() failed\n");
				SILSocket_close(newSocket);
				return NULL;
			}
			
			SILSocket_write(newSocket, (unsigned char*)"", 0); // Initiate connection
			
			break;
		}
		
		case SILSocketUDPServer:
		{
			struct sockaddr_in si_me;
			
			if ((newSocket->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
				perror("socket");
				free(newSocket);
				return NULL;
			}
			
			int on = 1;
			if (ioctl(newSocket->fd, FIONBIO, (char *)&on) < 0)
			{
				perror("ioctl() failed");
				SILSocket_close(newSocket);
				return NULL;
			}
			
			newSocket->si_other.sin_family = AF_INET;
			
			memset((char *) &si_me, 0, sizeof(si_me));
			si_me.sin_family = AF_INET;
			si_me.sin_port = htons(newSocket->UDP_port);
			si_me.sin_addr.s_addr = htonl(INADDR_ANY);
			if (bind(newSocket->fd, (const struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
				perror("bind");
				SILSocket_close(newSocket);
				return NULL;
			}
			break;
		}
		case SILSocketSerial:
		{
			struct termios  config;
			
			newSocket->fd = open(newSocket->serial_port, O_RDWR | O_NOCTTY | O_NDELAY);
			if (newSocket->fd == -1) {
				perror("serial open() failed");
				return NULL;
			}
			
			if (!isatty(newSocket->fd)) {
				SILSocket_close(newSocket);
				return NULL;
			}
			
			if (tcgetattr(newSocket->fd, &config) < 0) {
				SILSocket_close(newSocket);
				return NULL;
			}
			
			//
			// Input flags - Turn off input processing
			// convert break to null byte, no CR to NL translation,
			// no NL to CR translation, don't mark parity errors or breaks
			// no input parity check, don't strip high bit off,
			// no XON/XOFF software flow control
			//
			config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
								INLCR | PARMRK | INPCK | ISTRIP | IXON);
			//
			// Output flags - Turn off output processing
			// no CR to NL translation, no NL to CR-NL translation,
			// no NL to CR translation, no column 0 CR suppression,
			// no Ctrl-D suppression, no fill characters, no case mapping,
			// no local output processing
			//
			// config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
			//                     ONOCR | ONOEOT| OFILL | OLCUC | OPOST);
			config.c_oflag = 0;
			
			//
			// No line processing:
			// echo off, echo newline off, canonical mode off,
			// extended input processing off, signal chars off
			//
			config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
			
			//
			// Turn off character processing
			// clear current char size mask, no parity checking,
			// no output processing, force 8 bit input
			//
			config.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
			config.c_cflag |= CS8;
			
			//
			// One input byte is enough to return from read()
			// Inter-character timer off
			//
			config.c_cc[VMIN]  = 1;
			config.c_cc[VTIME] = 0;
			
			//
			// Communication speed (simple version, using the predefined
			// constants)
			//
#if ( LIN == 1 )
			switch (newSocket->serial_baud)
			{
				case 1152000:
					BAUD = 1152000;
					break;
				case 1000000:
					BAUD = 1000000;
					break;
				case 921600:
					BAUD = 921600;
					break;
				case 576000:
					BAUD = 576000;
					break;
				case 500000:
					BAUD = 500000;
					break;
				case 460800:
					BAUD = 460800;
					break;
				case 230400:
					BAUD = B230400;
					break;
				case 115200:
					BAUD = B115200;
					break;
				case 57600:
					BAUD = B57600;
					break;
				case 38400:
					BAUD = B38400;
					break;
				case 19200:
					BAUD  = B19200;
					break;
				case 9600:
					BAUD  = B9600;
					break;
				case 4800:
					BAUD  = B4800;
					break;
				case 2400:
					BAUD  = B2400;
					break;
				case 1800:
					BAUD  = B1800;
					break;
				case 1200:
					BAUD  = B1200;
					break;
				case 600:
					BAUD  = B600;
					break;
				case 300:
					BAUD  = B300;
					break;
				case 200:
					BAUD  = B200;
					break;
				case 150:
					BAUD  = B150;
					break;
				case 134:
					BAUD  = B134;
					break;
				case 110:
					BAUD  = B110;
					break;
				case 75:
					BAUD  = B75;
					break;
				case 50:
					BAUD  = B50;
					break;
				default:
					BAUD = B19200;
					break;
					
			}  //end of switch CommPortSpeed
			if (cfsetispeed(&config, BAUD) < 0 || cfsetospeed(&config, BAUD) < 0)
#else
				if (cfsetispeed(&config, newSocket->serial_baud) < 0 || cfsetospeed(&config, newSocket->serial_baud) < 0)
#endif
				{
					SILSocket_close(newSocket);
					return NULL;
				}
			
			//
			// Finally, apply the configuration
			//
			if (tcsetattr(newSocket->fd, TCSAFLUSH, &config) < 0) {
				SILSocket_close(newSocket);
				return NULL;
			}
			
			break;
		}
		default:
			break;
	}
	
	return newSocket;
}

void SILSocket_close(SILSocket socket)
{
	switch (socket->type) {
		case SILSocketUDPClient:
		case SILSocketUDPServer:
		case SILSocketSerial:
		{
			close(socket->fd);
			if (socket->serial_port) free(socket->serial_port);
			free(socket);
			break;
		}
		default:
			break;
	}
}

int SILSocket_read(SILSocket socket, unsigned char *buffer, int bufferLength)
{
	switch (socket->type) {
		case SILSocketUDPClient:
		case SILSocketUDPServer:
		{
			struct sockaddr_in from;
			socklen_t fromLength = sizeof(from);
			
			int received_bytes = (int)recvfrom(socket->fd, (char*)buffer, bufferLength, 0,
											   (struct sockaddr*)&from, &fromLength);
			
			if ( received_bytes < 0 ) {
				if (errno != EWOULDBLOCK) {
					SILSocket_close(socket);
					return -1;
				}
			}
			
			if (socket->type == SILSocketUDPServer) {
				socket->si_other.sin_port = from.sin_port;
				socket->si_other.sin_addr = from.sin_addr;
			}
			
			if (received_bytes < 0) return 0;
			
			return (int)received_bytes;
		}
		case SILSocketSerial:
		{
			int received_bytes = (int)read(socket->fd, (char*)buffer, bufferLength);
			
			if ( received_bytes < 0 ) {
				if (errno != EWOULDBLOCK) {
					SILSocket_close(socket);
					return -1;
				}
			}
			
			if (received_bytes < 0) return 0;
			
			return received_bytes;
		}
		default:
			break;
	}
	return -1;
}


int SILSocket_write(SILSocket socket, unsigned char *data, int dataLength)
{
	switch (socket->type) {
		case SILSocketUDPClient:
		case SILSocketUDPServer:
		{
			if (socket->type == SILSocketUDPServer && socket->si_other.sin_port == 0) {
				SILSocket_read(socket, NULL, 0);
				if (socket->si_other.sin_port == 0) {
					return 0;
				}
			}
			
			int bytesWritten = (int)sendto(socket->fd, data, dataLength, 0, (const struct sockaddr*)&socket->si_other, sizeof(socket->si_other));
			if (bytesWritten < 0) {
				perror("sendto()");
				SILSocket_close(socket);
				return -1;
			}
			return bytesWritten;
		}
		case SILSocketSerial:
		{
			int bytesWritten = (int)write(socket->fd, data, dataLength);
			if (bytesWritten < 0) {
				perror("sendto()");
				SILSocket_close(socket);
				return -1;
			}
			return bytesWritten;
		}
		default:
			break;
	}
	return -1;
}
