// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


#include "../MatrixPilot/defines.h"

#if (USE_MAVLINK == 1)

#include "MAVLink.h"
#include "MAVFTP.h"

//#include <crc32.h>
//#include <unistd.h>
#include <stdio.h>
//#include <fcntl.h>
//#include <sys/types.h>
//#include <dirent.h>

#define DIR int
#define opendir FSchdir

#define O_CREAT   1
#define O_EXCL    2
#define O_APPEND  4
#define O_RDONLY  8

#define DTYPE_FILE       2
#define DTYPE_DIRECTORY  3

#define NAME_MAX 20
struct dirent {
	int d_type;
	char d_name[NAME_MAX];
};

void seekdir(DIR* dirp, long loc)
{
}
int readdir_r(DIR* dirp, struct dirent* entry, struct direct** result)
{
	return 0;
}


#include "MAVFTP.h"

//private:

#define FALSE   0
#define TRUE    !FALSE

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

typedef enum _ErrorCode //: uint8_t
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
} ErrorCode;

typedef struct _RequestHeader
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
typedef struct _Request
{
//	public:
	union {
		//dq_entry_t entry;
		//work_s     work;
		int entry;
		int work;
	};
//	Mavlink* _mavlink;
	mavlink_encapsulated_data_t _message;
} Request;

//MavlinkFTP *MavlinkFTP::_server;
static MavlinkFTP* _server = NULL;

//MavlinkFTP* getServer(void)
//{
	// XXX this really cries out for some locking...
//	if (_server == NULL) {
		//_server = new MavlinkFTP;
//	}
//	return _server;
//}

//static const unsigned kRequestQueueSize = 2;
//static const unsigned kMaxSession = 2;
#define kMaxSession 2
static int _session_fds[kMaxSession];

//static Mavlink* _mavlink;
static mavlink_encapsulated_data_t _message;
// work freelist
#define kRequestQueueSize 2
static Request _workBufs[kRequestQueueSize];
//dq_queue_t _workFree;
//sem_t _lock;

static int _findUnusedSession(void);
static boolean _validSession(unsigned index);
static char* dataAsCString(Request* req);

static const uint8_t kProtocolMagic = 'f';
static const char    kDirentFile = 'F';
static const char    kDirentDir = 'D';
static const char    kDirentUnknown = 'U';
static const uint8_t kMaxDataLength = MAVLINK_MSG_ENCAPSULATED_DATA_FIELD_DATA_LEN - sizeof(RequestHeader);

/// Request worker; runs on the low-priority work queue to service
/// remote requests.
///
static void	_workerTrampoline(void* arg);
static void _worker(Request* req);

/// Reply to a request (XXX should be a Request method)
///
static void _reply(Request* req);

//#define ErrorCode int

static ErrorCode _workList(Request* req);
static ErrorCode _workOpen(Request* req, boolean create);
static ErrorCode _workRead(Request* req);
static ErrorCode _workWrite(Request* req);
static ErrorCode _workRemove(Request* req);
static ErrorCode _workTerminate(Request* req);
static ErrorCode _workReset(void);

//void _qLock() { do {} while (sem_wait(&_lock) != 0); }
//void _qUnlock() { sem_post(&_lock); }
void _qLock(void) { do {} while (0); }
void _qUnlock(void) { }

void _qFree(Request* req) {
//_qLock();
//dq_addlast(&req->entry, &_workFree);
//_qUnlock();
}

Request* _dqFree() {
//_qLock();
//auto req = reinterpret_cast<Request *>(dq_remfirst(&_workFree));
//_qUnlock();
//return req;
	return NULL;
}

uint8_t* rawData(void) { return &_message.data[0]; }
//RequestHeader *header(void) { return reinterpret_cast<RequestHeader *>(&_message.data[0]); }
RequestHeader* header(void) { return (RequestHeader*)&_message.data[0]; }
uint8_t* requestData(RequestHeader* req) { return req->data[0]; }
unsigned dataSize(RequestHeader* req) { return req->size + sizeof(RequestHeader); }
uint16_t sequence(void) { return _message.seqnr; }
//mavlink_channel_t channel(void) { return _mavlink->get_channel(); }



void reply(Request* req)
{
	// XXX the proper way would be an IOCTL / uORB call, rather than exploiting the
	// flat memory architecture, as we're operating between threads here.
	mavlink_message_t msg;
	msg.checksum = 0;
	boolean success = FALSE;
	unsigned len;

//	len = mavlink_msg_encapsulated_data_pack_chan(_mavlink->get_system_id(), _mavlink->get_component_id(), _mavlink->get_channel(), &msg, sequence(), rawData());
//	_mavlink->lockMessageBufferMutex();
//	boolean success = _mavlink->message_buffer_write(&msg, len);
//	_mavlink->unlockMessageBufferMutex();

//	mavlink_msg_mission_count_send(MAVLINK_COMM_0, mavlink_waypoint_dest_sysid, mavlink_waypoint_dest_compid, number_of_waypoints);

	if (!success) {
		DPRINT("FTP TX ERR");
	}
	// else {
	// DPRINT("wrote: sys: %d, comp: %d, chan: %d, len: %d, checksum: %d",
	// _mavlink->get_system_id(),
	// _mavlink->get_component_id(),
	// _mavlink->get_channel(),
	// len,
	// msg.checksum);
	// }
}

/* TODO:
MavlinkFTP::MavlinkFTP()
{
	// initialise the request freelist
	dq_init(&_workFree);
	sem_init(&_lock, 0, 1);
    // initialize session list
    for (size_t i=0; i<kMaxSession; i++) {
        _session_fds[i] = -1;
    }
	// drop work entries onto the free list
	for (unsigned i = 0; i < kRequestQueueSize; i++) {
		_qFree(&_workBufs[i]);
	}
}
 */

static void _workerTrampoline(void* arg)
{
//	auto req = reinterpret_cast<Request *>(arg);
//	auto server = MavlinkFTP::getServer();
	// call the server worker with the work item
//	server->_worker(req);
}

static void _worker(Request* req)
{
	RequestHeader* hdr = (RequestHeader*)req;
//	auto hdr = req->header();
	ErrorCode errorCode = kErrNone;
	uint32_t messageCRC;

	// basic sanity checks; must validate length before use
	if ((hdr->magic != kProtocolMagic) || (hdr->size > kMaxDataLength)) {
		errorCode = kErrNoRequest;
		goto out;
	}

	// check request CRC to make sure this is one of ours
	messageCRC = hdr->crc32;
	hdr->crc32 = 0;
	if (crc32(rawData(), dataSize(req)) != messageCRC) {
		errorCode = kErrNoRequest;
		goto out;
		DPRINT("ftp: bad crc");
	}

//printf("ftp: channel %u opc %u size %u offset %u\n", req->channel(), hdr->opcode, hdr->size, hdr->offset);

	switch (hdr->opcode) {
		case kCmdNone:
		break;
		case kCmdTerminate:
		errorCode = _workTerminate(req);
		break;
		case kCmdReset:
		errorCode = _workReset();
		break;
		case kCmdList:
		errorCode = _workList(req);
		break;
		case kCmdOpen:
		errorCode = _workOpen(req, false);
		break;
		case kCmdCreate:
		errorCode = _workOpen(req, true);
		break;
		case kCmdRead:
		errorCode = _workRead(req);
		break;
		case kCmdWrite:
		errorCode = _workWrite(req);
		break;
		case kCmdRemove:
		errorCode = _workRemove(req);
		break;
		default:
		errorCode = kErrNoRequest;
		break;	
	}
out:
	// handle success vs. error
	if (errorCode == kErrNone) {
		hdr->opcode = kRspAck;
		//DPRINT("FTP: ack\n");
	} else {
		DPRINT("FTP: nak %u", errorCode);
		hdr->opcode = kRspNak;
		hdr->size = 1;
		hdr->data[0] = errorCode;
	}
	// respond to the request
	_reply(req);
	// free the request buffer back to the freelist
//	_qFree(req);
}

static void _reply(Request* req)
{
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;

	// message is assumed to be already constructed in the request buffer, so generate the CRC
	hdr->crc32 = 0;
	hdr->crc32 = crc32(rawData(), dataSize(req));
	// then pack and send the reply back to the request source
//	req->reply();
	reply(req);
}

static ErrorCode _workList(Request* req)
{
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;
    
    char dirPath[kMaxDataLength];
    strncpy(dirPath, dataAsCString(req), kMaxDataLength);
    
	DIR* dp = opendir(dirPath);
	if (dp == NULL) {
		DPRINT("FTP: can't open path '%s'", dirPath);
		return kErrNotDir;
	}
    
	//DPRINT("FTP: list %s offset %d", dirPath, hdr->offset);

	ErrorCode errorCode = kErrNone;
	struct dirent entry;
	struct dirent *result = NULL;
	unsigned offset = 0;

	// move to the requested offset
	seekdir(dp, hdr->offset);

	for (;;) {
		// read the directory entry
		if (readdir_r(dp, &entry, &result)) {
			DPRINT("FTP: list %s readdir_r failure\n", dirPath);
			errorCode = kErrIO;
			break;
		}
		// no more entries?
		if (result == NULL) {
			if (hdr->offset != 0 && offset == 0) {
				// User is requesting subsequent dir entries but there were none. This means the user asked
				// to seek past EOF.
				errorCode = kErrEOF;
			}
			// Otherwise we are just at the last directory entry, so we leave the errorCode at kErrorNone to signal that
			break;
		}
		// name too big to fit?
		if ((strlen(entry.d_name) + offset + 2) > kMaxDataLength) {
			break;
		}
		// store the type marker
		switch (entry.d_type) {
			case DTYPE_FILE:
			hdr->data[offset++] = kDirentFile;
			break;
			case DTYPE_DIRECTORY:
			hdr->data[offset++] = kDirentDir;
			break;
			default:
			hdr->data[offset++] = kDirentUnknown;
			break;
		}
		// copy the name, which we know will fit
		strcpy((char*)&hdr->data[offset], entry.d_name);
		//printf("FTP: list %s %s\n", dirPath, (char *)&hdr->data[offset-1]);
		offset += strlen(entry.d_name) + 1;
	}
//	closedir(dp);
	hdr->size = offset;
	return errorCode;
}

static ErrorCode _workOpen(Request* req, boolean create)
{
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;
	int oflag = create ? (O_CREAT | O_EXCL | O_APPEND) : O_RDONLY;
	int fd;
		
	int session_index = _findUnusedSession();
	if (session_index < 0) {
		return kErrNoSession;
	}
	fd = open(dataAsCString(req), oflag);
	if (fd < 0) {
		return create ? kErrPerm : kErrNotFile;
	}
	_session_fds[session_index] = fd;
	hdr->session = session_index;
	hdr->size = 0;
	return kErrNone;
}

static ErrorCode _workRead(Request* req)
{
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;
	int bytes_read;

	int session_index = hdr->session;

	if (!_validSession(session_index)) {
		return kErrNoSession;
	}
	// Seek to the specified position
	//DPRINT("seek %d", hdr->offset);
	if (lseek(_session_fds[session_index], hdr->offset, SEEK_SET) < 0) {
		// Unable to see to the specified location
		DPRINT("seek fail");
		return kErrEOF;
	}
	bytes_read = read(_session_fds[session_index], &hdr->data[0], kMaxDataLength);
	if (bytes_read < 0) {
		// Negative return indicates error other than eof
		DPRINT("read fail %d", bytes_read);
		return kErrIO;
	}
	hdr->size = bytes_read;
	return kErrNone;
}

static ErrorCode _workWrite(Request* req)
{
#if 0
	// NYI: Coming soon
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;
	int result;

	// look up session
	auto session = getSession(hdr->session);
	if (session == NULL) {
		return kErrNoSession;
	}
	// append to file
	result = session->append(hdr->offset, &hdr->data[0], hdr->size);
	if (result < 0) {
		// XXX might also be no space, I/O, etc.
		return kErrNotAppend;
	}
	hdr->size = result;
	return kErrNone;
#else
	return kErrPerm;
#endif
}

static ErrorCode _workRemove(Request* req)
{
	//auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;

	// for now, send error reply
	return kErrPerm;
}

static ErrorCode _workTerminate(Request* req)
{
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;
		
	if (!_validSession(hdr->session)) {
		return kErrNoSession;
	}	
	close(_session_fds[hdr->session]);
	return kErrNone;
}

static ErrorCode _workReset(void)
{
	size_t i;

	for (i = 0; i < kMaxSession; i++) {
		if (_session_fds[i] != -1) {
			close(_session_fds[i]);
			_session_fds[i] = -1;
		}
	}
	return kErrNone;
}

static boolean _validSession(unsigned index)
{
	if ((index >= kMaxSession) || (_session_fds[index] < 0)) {
		return false;
	}
	return true;
}

static int _findUnusedSession(void)
{
	size_t i;

	for (i = 0; i < kMaxSession; i++) {
		if (_session_fds[i] == -1) {
			return i;
		}
	}
	return -1;
}

static char* dataAsCString(Request* req)
{
	// guarantee nul termination
	if (header()->size < kMaxDataLength) {
//		requestData()[header()->size] = '\0';
	} else {
//		requestData()[kMaxDataLength - 1] = '\0';
	}
	// and return data
	return (char*)&(header()->data[0]);
}



//boolean decode(Mavlink* mavlink, mavlink_message_t* fromMessage)
//boolean decode(mavlink_message_t* fromMessage)
//{
//	if (fromMessage->msgid == MAVLINK_MSG_ID_ENCAPSULATED_DATA) {
//		_mavlink = mavlink;
//		mavlink_msg_encapsulated_data_decode(fromMessage, &_message);
//		return true;
//	}
//	return false;
//}

//void handle_message(Mavlink* mavlink, mavlink_message_t* msg)
//{
//	auto req = _dqFree();    // get a free request
//	Request* req = ;

	// if we couldn't get a request slot, just drop it
//	if (req != NULL) {
		// decode the request
//		if (req->decode(mavlink, msg)) {
//			work_queue(LPWORK, &req->work, &_workerTrampoline, req, 0); // and queue it for the worker
//		} else {
//			_qFree(req);
//		}
//	}
//}

static inline void EncapsulatedData(mavlink_message_t* handle_msg)
{
	mavlink_mission_set_current_t packet;

//		mavlink_msg_encapsulated_data_decode(fromMessage, &_message);

	DPRINT("EncapsulatedData\r\n");

	// decode
	mavlink_msg_mission_set_current_decode(handle_msg, &packet);
	if (mavlink_check_target(packet.target_system, packet.target_component)) return;

//	mavlink_msg_mission_current_send(MAVLINK_COMM_0, get(PARAM_WP_INDEX));
}

boolean MAVFTPHandleMessage(mavlink_message_t* handle_msg)
{
	switch (handle_msg->msgid)
	{
		case MAVLINK_MSG_ID_ENCAPSULATED_DATA:
			EncapsulatedData(handle_msg);
			break;
		default:
			DPRINT("MAVFTPHandleMessage(handle_msg->msgid %u) - NOT HANDLED\r\n", handle_msg->msgid);
			return false;
	}
	return true;
}

void MAVFTPOutput_40hz(void)
{
//	vect3_32t wp;

//	if (mavlink_flags.mavlink_send_waypoint_changed == 1)
//	{
//		mavlink_flags.mavlink_send_waypoint_changed = 0;
//		mavlink_msg_mission_current_send(MAVLINK_COMM_0, mav_waypoint_changed);
//	}

//static inline void mavlink_msg_mission_item_reached_send(mavlink_channel_t chan, uint16_t seq)
//static inline void mavlink_msg_mission_current_send(mavlink_channel_t chan, uint16_t seq)

	// CHECK WHETHER WAYPOINT PROTOCOL HAS TIMED OUT WAITING ON A RESPONSE
/*
	if (mavlink_waypoint_timeout  <= 0)
	{
		if (mavlink_flags.mavlink_sending_waypoints ||  mavlink_flags.mavlink_receiving_waypoints)
		{
			//send_text((uint8_t *)"Timeout on waypoint protocol.\r\n");
			DPRINT("Timeout on waypoint protocol.\r\n");
		}
		mavlink_flags.mavlink_sending_waypoints   = false;
		mavlink_flags.mavlink_receiving_waypoints = false;
	}



	if (mavlink_waypoint_timeout  > 0) mavlink_waypoint_timeout--;
 */
/*
	// Acknowledge a command if flaged to do so.
	if (mavlink_send_command_ack == true)
	{
		mavlink_msg_command_ack_send(MAVLINK_COMM_0, mavlink_command_ack_command, mavlink_command_ack_result);
		mavlink_send_command_ack = false;
	}
 */
}

#endif // (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
