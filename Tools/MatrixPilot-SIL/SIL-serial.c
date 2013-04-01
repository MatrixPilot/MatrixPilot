//
//  SIL-serial.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/10/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include "SIL-udb.h"
#include "libUDB.h"
#include "UDBSocket.h"

UDBSocket gpsSocket;
UDBSocket telemetrySocket;

int32_t gpsRate = 0;
int32_t serialRate = 0;


#define BUFLEN 512


//////////////////////////////////////////////////////////
// GPS and Telemetry
//////////////////////////////////////////////////////////

void udb_gps_set_rate(int32_t rate)
{
	gpsRate = rate;
}


boolean udb_gps_check_rate(int32_t rate)
{
	return (rate == gpsRate);
}


// Call this function to initiate sending a data to the GPS
void udb_gps_start_sending_data(void)
{
	if (!gpsSocket) return;
	
	uint8_t buffer[BUFLEN];
	int16_t c;
	int16_t pos=0;
	
	while (pos < BUFLEN && (c = udb_gps_callback_get_byte_to_send()) != -1) {
		buffer[pos++] = c;
	};
	
	int16_t bytesWritten = UDBSocket_write(gpsSocket, (uint8_t*)buffer, pos);
	
	if (bytesWritten < 0) {
		UDBSocket_close(gpsSocket);
		gpsSocket = NULL;
	}
}


void udb_serial_set_rate(int32_t rate)
{
	serialRate = rate;
}


boolean udb_serial_check_rate(int32_t rate)
{
	return (serialRate == rate);
}


// Call this function to initiate sending a data to the serial port
void udb_serial_start_sending_data(void)
{
	if (!telemetrySocket) return;
	
	uint8_t buffer[BUFLEN];
	int16_t c;
	int16_t pos=0;
	
	while (pos < BUFLEN && (c = udb_serial_callback_get_byte_to_send()) != -1) {
		buffer[pos++] = c;
	}
	
	int16_t bytesWritten = UDBSocket_write(telemetrySocket, (uint8_t*)buffer, pos);
	
	if (bytesWritten == -1) {
		UDBSocket_close(telemetrySocket);
		telemetrySocket = NULL;
	}
}
