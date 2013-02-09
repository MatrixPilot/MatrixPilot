//
//  SIL-udb.h
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "UDBSocket.h"
#include "options.h"

SILSocket stdioSocket;
SILSocket udpSocket;

uint8_t readSockets(void);


void printHelp(void)
{
	printf("usage: silcat [-h | -s | -c | -l] [port] [baud]\n");
	printf("	-h    print Help\n");
	printf("	-s    udp Server\n");
	printf("	-c    udp Client\n");
	printf("	-l    seriaL\n");
	printf("	port: udp port number, or serial port\n");
	printf("	baud: serial port speed\n");
	printf("	(With no arguments, will connect to MatrixPilot-SIL's telemetry port.)\n");
}


int main(int argc, char** argv)
{
	SILSocketType socketType = (!SILSIM_TELEMETRY_SERVER) ? SILSocketUDPServer : SILSocketUDPClient;
	uint32_t udpPort = SILSIM_TELEMETRY_PORT;
	char *serialPort = NULL;
	uint32_t serialBaud = 0;
	
	uint8_t argPos = 0;
	
	int i;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'h') {
				printHelp();
				return 0;
			}
			else if (argv[i][1] == 's') {
				socketType = SILSocketUDPServer;
			}
			else if (argv[i][1] == 'c') {
				socketType = SILSocketUDPClient;
			}
			else if (argv[i][1] == 'l') {
				socketType = SILSocketSerial;
			}
		}
		else {
			if (argPos == 0) {
				if (socketType == SILSocketSerial) {
					serialPort = argv[i];
				}
				else {
					udpPort = atoi(argv[i]);
				}
			}
			else if (argPos == 1) {
				serialBaud = atoi(argv[i]);
			}
			else {
				printHelp();
				return 0;
			}
			argPos++;
		}
	}
	
	stdioSocket = SILSocket_init(SILSocketStandardInOut, 0, NULL, 0);
	udpSocket = SILSocket_init(socketType, udpPort, serialPort, serialBaud);
	
	while (1) {
		if (!readSockets()) {
			usleep(1000);
		}
	}
}


#define BUFLEN 512

uint8_t readSockets(void)
{
	unsigned char buffer[BUFLEN];
	int32_t bytesRead;
	uint8_t didRead = 0;
	
	if (udpSocket) {
		bytesRead = SILSocket_read(udpSocket, buffer, BUFLEN);
		if (bytesRead < 0) {
			SILSocket_close(udpSocket);
			udpSocket = NULL;
			printf("ERROR: read failed\n");
			exit(1);
		}
		else if (bytesRead > 0) {
			bytesRead = SILSocket_write(stdioSocket, buffer, bytesRead);
			didRead = 1;
		}
	}
	
	if (stdioSocket) {
		bytesRead = SILSocket_read(stdioSocket, buffer, BUFLEN);
		if (bytesRead > 0) {
			bytesRead = SILSocket_write(udpSocket, buffer, bytesRead);
			if (bytesRead < 0) {
				SILSocket_close(udpSocket);
				udpSocket = NULL;
				printf("ERROR: write failed\n");
				exit(1);
			}
			didRead = 1;
		}
	}
		
	return didRead;
}
