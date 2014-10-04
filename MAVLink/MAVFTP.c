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

#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)

#include "MAVLink.h"
#include "MAVFTP.h"

//#include <crc32.h>
//#include <unistd.h>
#include <stdio.h>
//#include <fcntl.h>

#if 0

//#include "mavlink_ftp.h"

//MavlinkFTP *MavlinkFTP::_server;
static MavlinkFTP* _server = NULL;

MavlinkFTP* getServer(void)
{
	// XXX this really cries out for some locking...
	if (_server == NULL) {
		//_server = new MavlinkFTP;
	}
	return _server;
}

static const unsigned kRequestQueueSize = 2;
#define kMaxSession 2
static int _session_fds[kMaxSession];

static Mavlink* _mavlink;
static mavlink_encapsulated_data_t _message;
// work freelist
//static Request _workBufs[kRequestQueueSize];
//dq_queue_t _workFree;
//sem_t _lock;

static int _findUnusedSession(void);
static boolean _validSession(unsigned index);
char* dataAsCString();

static const uint8_t kProtocolMagic = 'f';
static const char    kDirentFile = 'F';
static const char    kDirentDir = 'D';
static const char    kDirentUnknown = 'U';
//static const uint8_t kMaxDataLength = MAVLINK_MSG_ENCAPSULATED_DATA_FIELD_DATA_LEN - sizeof(RequestHeader);
#define kMaxDataLength MAVLINK_MSG_ENCAPSULATED_DATA_FIELD_DATA_LEN

/// Request worker; runs on the low-priority work queue to service
/// remote requests.
///
static void	_workerTrampoline(void* arg);
static void _worker(Request* req);

/// Reply to a request (XXX should be a Request method)
///
static void _reply(Request* req);

static ErrorCode _workList(Request* req);
static ErrorCode _workOpen(Request* req, boolean create);
static ErrorCode _workRead(Request* req);
static ErrorCode _workWrite(Request* req);
static ErrorCode _workRemove(Request* req);
static ErrorCode _workTerminate(Request* req);
static ErrorCode _workReset();

//void _qLock() { do {} while (sem_wait(&_lock) != 0); }
//void _qUnlock() { sem_post(&_lock); }
void _qLock() { do {} while (0); }
void _qUnlock() { }

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
	return 0;
}

void reply(Request* req);

uint8_t* rawData() { return &_message.data[0]; }
//RequestHeader *header() { return reinterpret_cast<RequestHeader *>(&_message.data[0]); }
RequestHeader* header() { return (RequestHeader*)&_message.data[0]; }
uint8_t* requestData() { return &(header()->data[0]); }
unsigned dataSize() { return header()->size + sizeof(RequestHeader); }
uint16_t sequence() { return _message.seqnr; }
//mavlink_channel_t channel() { return _mavlink->get_channel(); }


///////////////////////////////////////////////////////////////////////////////
//int crc32(int a, int b) { return 0; }
DIR* opendir(char* path) { return 0; }
void seekdir(DIR* dp, int offset) {}
int readdir_r(DIR* dp, struct dirent* entry, struct dirent** result) { return 0; }
void closedir(DIR* dp) {}

int open(char* path, int mode) { return 0; }
int lseek(int session, int offset, int mode) { return 0; }
int read(int session, char* buf, int buflen) { return 0; }
int close(int fd) { return 0; }

//	fd = open(dataAsCString(req), oflag);
//	if (lseek(_session_fds[session_index], hdr->offset, SEEK_SET) < 0) {
//	bytes_read = read(_session_fds[session_index], &hdr->data[0], kMaxDataLength);
///////////////////////////////////////////////////////////////////////////////

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
/*
void handle_message(Mavlink* mavlink, mavlink_message_t* msg)
{
	auto req = _dqFree();    // get a free request
	Request* req = 0;

	// if we couldn't get a request slot, just drop it
	if (req != NULL) {
		// decode the request
//		if (req->decode(mavlink, msg)) {
//			work_queue(LPWORK, &req->work, &_workerTrampoline, req, 0); // and queue it for the worker
//		} else {
//			_qFree(req);
//		}
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
	if (crc32(req->rawData, req->dataSize) != messageCRC) {
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
	hdr->crc32 = crc32(req->rawData, req->dataSize);
	// then pack and send the reply back to the request source
//	req->reply();
	reply(req);
}

static ErrorCode _workList(Request* req)
{
    char dirPath[kMaxDataLength];
	DIR* dp;
	ErrorCode errorCode = kErrNone;
	struct dirent entry, *result = NULL;
	unsigned offset = 0;
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;
    
    strncpy(dirPath, dataAsCString(req), kMaxDataLength);
    
	dp = opendir(dirPath);

	if (dp == NULL) {
		DPRINT("FTP: can't open path '%s'", dirPath);
		return kErrNotDir;
	}
    
	//DPRINT("FTP: list %s offset %d", dirPath, hdr->offset);

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
	closedir(dp);
	hdr->size = offset;
	return errorCode;
}

static ErrorCode _workOpen(Request* req, boolean create)
{
//	auto hdr = req->header();
	RequestHeader* hdr = (RequestHeader*)req;
//	int oflag = create ? (O_CREAT | O_EXCL | O_APPEND) : O_RDONLY;
	int oflag = 0;
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

	for (i=0; i<kMaxSession; i++) {
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

	for (i=0; i<kMaxSession; i++) {
		if (_session_fds[i] == -1) {
			return i;
		}
	}
	return -1;
}

char* dataAsCString(Request* req)
{
	// guarantee nul termination
	if (header()->size < kMaxDataLength) {
		requestData()[header()->size] = '\0';
	} else {
		requestData()[kMaxDataLength - 1] = '\0';
	}
	// and return data
	return (char *)&(header()->data[0]);
}


boolean decode(Mavlink *mavlink, mavlink_message_t *fromMessage)
{
	if (fromMessage->msgid == MAVLINK_MSG_ID_ENCAPSULATED_DATA) {
		_mavlink = mavlink;
		mavlink_msg_encapsulated_data_decode(fromMessage, &_message);
		return true;
	}
	return false;
}

#endif // 0

boolean MAVFTPHandleMessage(mavlink_message_t* handle_msg)
{
	if (handle_msg->msgid == MAVLINK_MSG_ID_ENCAPSULATED_DATA)
	{
/*
typedef struct __mavlink_encapsulated_data_t
	{
 uint16_t seqnr; ///< sequence number (starting with 0 on every transmission)
 uint8_t data[253]; ///< image data bytes
} mavlink_encapsulated_data_t;

 */
		mavlink_encapsulated_data_t packet;
		mavlink_msg_encapsulated_data_decode(handle_msg, &packet);


		return true;
	}
			return false;
	}

void reply(Request* req)
{
	// XXX the proper way would be an IOCTL / uORB call, rather than exploiting the
	// flat memory architecture, as we're operating between threads here.
	boolean success;
	unsigned len;
	mavlink_message_t msg;
	msg.checksum = 0;
//	len = mavlink_msg_encapsulated_data_pack_chan(_mavlink->get_system_id, _mavlink->get_component_id, _mavlink->get_channel, &msg, sequence(), rawData());

//	_mavlink->lockMessageBufferMutex();
//	success = _mavlink->message_buffer_write(&msg, len);
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


uint16_t mavlink_ftp_timeout = 0;

void MAVFTPOutput_40hz(void)
{
	if (mavlink_flags.mavlink_ftp == 1)
	{
		mavlink_flags.mavlink_ftp = 0;
//		mavlink_msg_encapsulated_data_send(MAVLINK_COMM_0, uint16_t seqnr, const uint8_t *data)
	}

	// CHECK WHETHER PROTOCOL HAS TIMED OUT WAITING ON A RESPONSE
	if (mavlink_ftp_timeout  <= 0)
	{
		if (mavlink_flags.mavlink_ftp_sending ||  mavlink_flags.mavlink_ftp_receiving)
		{
			//send_text((uint8_t *)"Timeout on FTP protocol.\r\n");
			DPRINT("Timeout on FTP protocol.\r\n");
		}
		mavlink_flags.mavlink_ftp_sending   = false;
		mavlink_flags.mavlink_ftp_receiving = false;
	}


	if (mavlink_ftp_timeout  > 0) mavlink_ftp_timeout--;
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
