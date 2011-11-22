// This file is part of the MatrixPilotQuad firmware.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.

#include "../libDCM/libDCM.h"

// Used for serial debug output
#include "stdio.h"


char debug_buffer[128] ;
int db_index = 0 ;
boolean hasWrittenHeader = 0 ;

extern int theta[3] , roll_feedback , pitch_feedback , yaw_feedback , accelEarth[3] , accel_feedback ;


// Prepare a line of serial output and start it sending
void send_debug_line( void )
{
	db_index = 0 ;
	
	if (!hasWrittenHeader)
	{
		sprintf(debug_buffer, "r6 , r7 ,  w0 , w1 , w2 , rfb , pfb , yfb , acc , accfb\r\n") ;
		hasWrittenHeader = 1 ;
	}
	else
	{
		sprintf(debug_buffer, "%i , %i , %i , %i , %i , %i , %i , %i , %i , %i\r\n" ,
			rmat[6] , rmat[7] , 
			theta[0] , theta[1] , theta[2] , 
			roll_feedback , pitch_feedback, yaw_feedback ,
			accelEarth[2] , accel_feedback ) ;
	}
	
	udb_serial_start_sending_data() ;
	
	return ;
}


// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.
int udb_serial_callback_get_byte_to_send(void)
{
	unsigned char c = debug_buffer[ db_index++ ] ;
	
	if (c == 0) return -1 ;
	
	return c ;
}


// Don't respond to serial input
void udb_serial_callback_received_byte(char rxchar)
{
	// Do nothing
	return ;
}


