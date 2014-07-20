// This file is part of MatrixPilot.
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


#ifndef MAVFTP_H
#define MAVFTP_H

/*
*
* MAVLink remote file server.
*
* Messages are wrapped in ENCAPSULATED_DATA messages. Every message includes
* a session ID and sequence number.
*
* A limited number of requests (currently 2) may be outstanding at a time.
* Additional messages will be discarded.
*
* Messages consist of a fixed header, followed by a data area.
*
*/
//#include <dirent.h>
//#include <queue.h>

//#include <nuttx/wqueue.h>
//#include <systemlib/err.h>

//#include "mavlink_messages.h"

//class MavlinkFTP
//{
//public:
//MavlinkFTP();
typedef struct _MavlinkFTP
{
	int foo;
} MavlinkFTP;

MavlinkFTP* getServer(void);

// static interface
//void handle_message(Mavlink* mavlink, mavlink_message_t *msg);
//bool decode(Mavlink* mavlink, mavlink_message_t* fromMessage);

boolean MAVFTPHandleMessage(mavlink_message_t* handle_msg);
void MAVFTPOutput_40hz(void);


#endif // MAVFTP_H
