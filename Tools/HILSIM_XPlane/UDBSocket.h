//
//  UDBSocket.h
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#ifndef MatrixPilot_SIL_SIL_sockets_h
#define MatrixPilot_SIL_SIL_sockets_h

#if (!defined(WIN32) || defined(WIN))
#include <stdint.h>
#else
#include "stdint-win.h"
#endif

typedef enum {
	UDBSocketUndefined = 0,
	UDBSocketStandardInOut,
	UDBSocketUDPClient,
	UDBSocketUDPServer,
	UDBSocketSerial
} UDBSocketType;


typedef struct UDBSocket_t *UDBSocket;


// Configure your socket.
// UDBSocketStandardInOut:	specify type
// UDBSocketUDPClient:		specify type, UDP_port, UDP_host
// UDBSocketUDPServer:		specify type, UDP_port
// UDBSocketSerial:			specify type, serial_port, serial_baud
UDBSocket UDBSocket_init(UDBSocketType type, uint16_t UDP_port, char *UDP_host, char *serial_port, long serial_baud);

void UDBSocket_close(UDBSocket socket);

int UDBSocket_read(UDBSocket socket, unsigned char *buffer, int bufferLength);
int UDBSocket_write(UDBSocket socket, unsigned char *data, int dataLength);

char *UDBSocketLastErrorMessage(void);

#endif
