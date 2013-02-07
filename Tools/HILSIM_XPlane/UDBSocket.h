//
//  UDBSocket.h
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#ifndef MatrixPilot_SIL_SIL_sockets_h
#define MatrixPilot_SIL_SIL_sockets_h

#include <stdint.h>


typedef enum {
	SILSocketUndefined = 0,
	SILSocketUDPClient,
	SILSocketUDPServer,
	SILSocketSerial
} SILSocketType;


typedef struct SILSocket_t *SILSocket;


// Leave serial_port NULL and serial_baud 0 for UDP sockets
// Leave UDP_port 0 for serial sockets
SILSocket SILSocket_init(SILSocketType type, long UDP_port, char *serial_port, long serial_baud);

void SILSocket_close(SILSocket socket);

int SILSocket_read(SILSocket socket, unsigned char *buffer, int bufferLength);
int SILSocket_write(SILSocket socket, unsigned char *data, int dataLength);

#endif
