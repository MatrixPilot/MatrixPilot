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
typedef struct tag_Mavlink
{
//	len = mavlink_msg_encapsulated_data_pack_chan(_mavlink->get_system_id(), _mavlink->get_component_id(), _mavlink->get_channel(), &msg, sequence(), rawData());
	int get_system_id;
	int get_component_id;
	int get_channel;
} Mavlink;

typedef struct tag_MavlinkFTP
{
	int bar;
} MavlinkFTP;

MavlinkFTP* getServer(void);

// static interface
void handle_message(Mavlink* mavlink, mavlink_message_t *msg);

//private:

enum Opcode //: uint8_t
{
	kCmdNone,	// ignored, always acked
	kCmdTerminate,	// releases sessionID, closes file
	kCmdReset,	// terminates all sessions
	kCmdList,	// list files in <path> from <offset>
	kCmdOpen,	// opens <path> for reading, returns <session>
	kCmdRead,	// reads <size> bytes from <offset> in <session>
	kCmdCreate,	// creates <path> for writing, returns <session>
	kCmdWrite,	// appends <size> bytes at <offset> in <session>
	kCmdRemove,	// remove file (only if created by server?)

	kRspAck,
	kRspNak
};

enum ErrorCode //: uint8_t
{
	kErrNone,
	kErrNoRequest,
	kErrNoSession,
	kErrSequence,
	kErrNotDir,
	kErrNotFile,
	kErrEOF,
	kErrNotAppend,
	kErrTooBig,
	kErrIO,
	kErrPerm
};

typedef struct tag_RequestHeader
{
	uint8_t	magic;
	uint8_t	session;
	uint8_t	opcode;
	uint8_t	size;
	uint32_t crc32;
	uint32_t offset;
	uint8_t	data[];
} RequestHeader;

//class Request
typedef struct tag_Request
{
	int rawData;
	int dataSize;
//	public:
	union {
		//dq_entry_t entry;
		//work_s     work;
		int entry;
		int work;
	};
	Mavlink* _mavlink;
	mavlink_encapsulated_data_t _message;
} Request;

struct dirent {
	int d_type;
	char d_name[13];
};

#define ErrorCode int

#define DIR int
#define DTYPE_FILE      1
#define DTYPE_DIRECTORY 2


//boolean decode(Mavlink* mavlink, mavlink_message_t* fromMessage);

boolean MAVFTPHandleMessage(mavlink_message_t* handle_msg);
void MAVFTPOutput_40hz(void);


#endif // MAVFTP_H
