/*********************************************************************
 *
 *	Trivial File Transfer Protocol (TFTP) Client
 *  Module for Microchip TCP/IP Stack
 *	 -Provides unreliable file upload and download services to other 
 *    applications which wish to connect to a remote UDP based TFTP 
 *    server.
 *	 -Reference: RFC 1350
 *
 *********************************************************************
 * FileName:        TFTPc.c
 * Dependencies:    UDP, ARP, Tick
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/5/03  	Original        (Rev 1.0)
 * Howard Schlunder		01/05/2010	Added TFTPUploadRAMFileToHost(), 
 									TFTPUploadFragmentedRAMFileToHost(), 
 									and TFTPGetUploadStatus() APIs.
 ********************************************************************/
#define __TFTPC_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_TFTP_CLIENT)

#include "TCPIP Stack/TCPIP.h"

// The TFTP Client port - a unique port on this device
#define TFTP_CLIENT_PORT        65352L       
// The TFTP Server Port
#define TFTP_SERVER_PORT        (69L)
// The size of a TFTP block - 512 bytes
#define TFTP_BLOCK_SIZE         (0x200L)
// The MSB of the TFTP_BLOCK_SIZE
#define TFTP_BLOCK_SIZE_MSB     (0x02u)

// The TFTP state machine
typedef enum
{
    SM_TFTP_WAIT = 0,
    SM_TFTP_READY,
    SM_TFTP_WAIT_FOR_DATA,
    SM_TFTP_WAIT_FOR_ACK,
    SM_TFTP_DUPLICATE_ACK,
    SM_TFTP_SEND_ACK,
    SM_TFTP_SEND_LAST_ACK
} TFTP_STATE;

// Enumeration of TFTP opcodes
typedef enum
{
    TFTP_OPCODE_RRQ = 1,        // Get
    TFTP_OPCODE_WRQ,            // Put
    TFTP_OPCODE_DATA,           // Actual data
    TFTP_OPCODE_ACK,            // Ack for Get/Put
    TFTP_OPCODE_ERROR           // Error
} TFTP_OPCODE;


UDP_SOCKET _tftpSocket = TFTP_UPLOAD_COMPLETE;         // TFTP Socket for TFTP server link
WORD _tftpError;                // Variable to preserve error condition causes for later transmission

static union
{
    struct
    {
        NODE_INFO _hostInfo;
    } group1;

    struct
    {
        WORD_VAL _tftpBlockNumber;
        WORD_VAL _tftpDuplicateBlock;
        WORD_VAL _tftpBlockLength;
    } group2;
} MutExVar;     // Mutually Exclusive variable groups to conserve RAM.

// TFTP state machine tracker variable
static TFTP_STATE _tftpState;
// Tracker variable for the number of TFTP retries
static BYTE _tftpRetries;
// Timing variable used to detect timeout conditions
static DWORD _tftpStartTick;

// TFTP status flags
static union
{
    struct
    {
        unsigned int bIsFlushed : 1;
        unsigned int bIsAcked : 1;
        unsigned int bIsClosed : 1;
        unsigned int bIsClosing : 1;
        unsigned int bIsReading : 1;
    } bits;
    BYTE Val;
} _tftpFlags;


// Private helper function
static void _TFTPSendFileName(TFTP_OPCODE command, BYTE *fileName);
// Private helper function
static void _TFTPSendAck(WORD_VAL blockNumber);

#if defined(__18CXX)
    // PIC18 ROM variable argument implementation of _TFTPSendFileName
	static void _TFTPSendROMFileName(TFTP_OPCODE opcode, ROM BYTE *fileName);
#endif

// Blank out DEBUG statements if not enabled.
#if defined(TFTP_DEBUG)
    #define DEBUG(a)        a
#else
    #define DEBUG(a)
#endif

// TFTPUploadRAMFileToHost(), TFTPUploadFragmentedRAMFileToHost() and 
// TFTPGetUploadStatus() functions require the DNS client module to be enabled 
// for them to work.  The RAM and ROM resources for these functions can be 
// preserved if the DNS client module isn't enabled.
#if defined(STACK_USE_DNS)

static ROM BYTE *vUploadRemoteHost;
static ROM BYTE *vUploadFilename;
static TFTP_CHUNK_DESCRIPTOR *uploadChunkDescriptor;
static WORD wUploadChunkOffset;
static CHAR smUpload = TFTP_UPLOAD_COMPLETE;
static TFTP_CHUNK_DESCRIPTOR *uploadChunkDescriptorForRetransmit;
static WORD wUploadChunkOffsetForRetransmit;

/*****************************************************************************
  Function:
	void TFTPUploadRAMFileToHost(ROM BYTE *vRemoteHost, 
								 ROM BYTE *vFilename, 
								 BYTE *vData, 
								 WORD wDataLength)

  Summary:
	Uploads a contiguous array of RAM bytes as a file to a remote TFTP server.
	
  Description:
	Uploads a contiguous array of RAM bytes as a file to a remote TFTP server.
	
  Precondition:
	None

  Parameters:
	vRemoteHost: ROM string of the remote TFTP server to upload to (ex: 
		"www.myserver.com").  For device architectures that make no distinction 
		between RAM and ROM pointers (PIC24, dsPIC and PIC32), this string must 
		remain allocated and unmodified in RAM until the TFTP upload process 
		completes (as indicated by TFTPGetUploadStatus()).  
	vFilename: ROM string of the remote file to create/overwrite (ex: 
		"status.txt").  For device architectures that make no distinction 
		between RAM and ROM pointers (PIC24, dsPIC and PIC32), this string must 
		remain allocated and unmodified in RAM until the TFTP upload process 
		completes (as indicated by TFTPGetUploadStatus()).
	vData: Pointer to a RAM array of data to write to the file.
	wDataLength: Number of bytes pointed to by vData.  This will be the final 
		file size of the uploaded file.  Note that since this is defined as a 
		WORD type, the maximum possible file size is 65535 bytes.  For longer 
		files, call the TFTPUploadFragmentedRAMFileToHost() function instead.

  Returns:
  	None
  
  Remarks:
	The DNS client module must be enabled to use this function.  i.e. 
	STACK_USE_DNS must be defined in TCPIPConfig.h.

  	Call the TFTPGetUploadStatus() function to determine the status of the file 
  	upload.
  	
  	It is only possible to have one TFTP operation active at any given time.  
  	After starting a TFTP operation by calling TFTPUploadRAMFileToHost() or 
  	TFTPUploadFragmentedRAMFileToHost(), you must wait until 
  	TFTPGetUploadStatus() returns a completion status code (<=0) before calling 
  	any other TFTP API functions.
  ***************************************************************************/
void TFTPUploadRAMFileToHost(ROM BYTE *vRemoteHost, ROM BYTE *vFilename, BYTE *vData, WORD wDataLength)
{
	static TFTP_CHUNK_DESCRIPTOR chunk[2];
	chunk[0].vDataPointer = vData;
	chunk[0].wDataLength = wDataLength;
	chunk[1].vDataPointer = NULL;
	TFTPUploadFragmentedRAMFileToHost(vRemoteHost, vFilename, chunk);
}

/*****************************************************************************
  Function:
	void TFTPUploadFragmentedRAMFileToHost(ROM BYTE *vRemoteHost, 
										   ROM BYTE *vFilename, 
										   TFTP_CHUNK_DESCRIPTOR *vFirstChunkDescriptor)
  Summary:
	Uploads an random, potentially non-contiguous, array of RAM bytes as a file 
	to a remote TFTP server.
	
  Description:
	Uploads an random, potentially non-contiguous, array of RAM bytes as a file 
	to a remote TFTP server.
	
  Precondition:
	None

  Parameters:
	vRemoteHost: ROM string of the remote TFTP server to upload to (ex: 
		"www.myserver.com").  For device architectures that make no distinction 
		between RAM and ROM pointers (PIC24, dsPIC and PIC32), this string must 
		remain allocated and unmodified in RAM until the TFTP upload process 
		completes (as indicated by TFTPGetUploadStatus()).  
	vFilename: ROM string of the remote file to create/overwrite (ex: 
		"status.txt").  For device architectures that make no distinction 
		between RAM and ROM pointers (PIC24, dsPIC and PIC32), this string must 
		remain allocated and unmodified in RAM until the TFTP upload process 
		completes (as indicated by TFTPGetUploadStatus()).
	vFirstChunkDescriptor: Pointer to a static or global (persistent) array of 
		TFTP_CHUNK_DESCRIPTOR structures describing what RAM memory addresses 
		the file contents should be obtained from.  The 
		TFTP_CHUNK_DESCRIPTOR.vDataPointer field should be set to the memory 
		address of the data to transmit, and the 
		TFTP_CHUNK_DESCRIPTOR.wDataLength field should be set to the number of 
		bytes to transmit from the given pointer.  The TFTP_CHUNK_DESCRIPTOR 
		array must be terminated by a dummy descriptor whos 
		TFTP_CHUNK_DESCRIPTOR.vDataPointer pointer is set to NULL.  Refer to the 
		TFTPUploadRAMFileToHost() API for an example calling sequence since it 
		merely a wrapper to this TFTPUploadFragmentedRAMFileToHost() function.

  Returns:
  	None
  
  Remarks:
	The DNS client module must be enabled to use this function.  i.e. 
	STACK_USE_DNS must be defined in TCPIPConfig.h.

  	Call the TFTPGetUploadStatus() function to determine the status of the file 
  	upload.
  	
  	It is only possible to have one TFTP operation active at any given time.  
  	After starting a TFTP operation by calling TFTPUploadRAMFileToHost() or 
  	TFTPUploadFragmentedRAMFileToHost(), you must wait until 
  	TFTPGetUploadStatus() returns a completion status code (<=0) before calling 
  	any other TFTP API functions.
  ***************************************************************************/
void TFTPUploadFragmentedRAMFileToHost(ROM BYTE *vRemoteHost, ROM BYTE *vFilename, TFTP_CHUNK_DESCRIPTOR *vFirstChunkDescriptor)
{
	vUploadRemoteHost = vRemoteHost;
	vUploadFilename = vFilename;
	uploadChunkDescriptor = vFirstChunkDescriptor;
	uploadChunkDescriptorForRetransmit = vFirstChunkDescriptor;
	wUploadChunkOffset = 0;
	wUploadChunkOffsetForRetransmit = 0;
	if(smUpload == TFTP_UPLOAD_RESOLVE_HOST)
		DNSEndUsage();
	smUpload = TFTP_UPLOAD_GET_DNS;
}


/*****************************************************************************
  Function:
	CHAR TFTPGetUploadStatus(void)
	
  Summary:
	Returns the TFTP file upload status started by calling the 
	TFTPUploadRAMFileToHost() or TFTPUploadFragmentedRAMFileToHost() functions.
	
  Description:
	Returns the TFTP file upload status started by calling the 
	TFTPUploadRAMFileToHost() or TFTPUploadFragmentedRAMFileToHost() functions.
	
  Precondition:
	None

  Parameters:
	None

  Returns:
  	A status code.  Negative results are fatal errors.  Positive results 
  	indicate the TFTP upload operation is still being processed.  A zero result 
  	indicates successful file upload completion (TFTP API is now idle and 
  	available for further calls).  Specific return values are as follows:
  	0 (TFTP_UPLOAD_COMPLETE): Upload completed successfully
  	1 (TFTP_UPLOAD_GET_DNS): Attempting to obtain DNS client module
  	2 (TFTP_UPLOAD_RESOLVE_HOST): Attempting to resolve TFTP hostname
  	3 (TFTP_UPLOAD_CONNECT): Attempting to ARP and contact the TFTP server
  	4 (TFTP_UPLOAD_SEND_FILENAME): Attempting to send the filename and receive 
  		acknowledgement.
  	5 (TFTP_UPLOAD_SEND_DATA): Attempting to send the file contents and receive 
  		acknowledgement.
  	6 (TFTP_UPLOAD_WAIT_FOR_CLOSURE): Attempting to send the final packet of 
  		file contents and receive acknowledgement.
  	-1 (TFTP_UPLOAD_HOST_RESOLVE_TIMEOUT): Couldn't resolve hostname
  	-2 (TFTP_UPLOAD_CONNECT_TIMEOUT): Couldn't finish ARP and reach server
  	-3 (TFTP_UPLOAD_SERVER_ERROR): TFTP server returned an error (ex: access 
  		denial) or file upload failed due to a timeout (partial file may have 
  		been uploaded).
  
  Remarks:
	The DNS client module must be enabled to use this function.  i.e. 
	STACK_USE_DNS must be defined in TCPIPConfig.h.
  ***************************************************************************/
CHAR TFTPGetUploadStatus(void)
{
	TFTP_RESULT result;
	IP_ADDR ipRemote;
	WORD w, w2;
	BYTE *vData;

	if(UDPIsOpened(_tftpSocket)== FALSE)
	{

		_tftpSocket = UDPOpenEx((DWORD)(ROM_PTR_BASE)vUploadRemoteHost,
										 UDP_OPEN_ROM_HOST,TFTP_CLIENT_PORT,
										 TFTP_SERVER_PORT);
	}

	switch(smUpload)
	{
	case TFTP_UPLOAD_GET_DNS:
		if(!DNSBeginUsage())
			break;
		DNSResolveROM(vUploadRemoteHost, DNS_TYPE_A);
		smUpload = TFTP_UPLOAD_RESOLVE_HOST;
		break;

	case TFTP_UPLOAD_RESOLVE_HOST:
		if(!DNSIsResolved(&ipRemote))
			break;
		DNSEndUsage();
		if(ipRemote.Val == 0u)
		{
			smUpload = TFTP_UPLOAD_HOST_RESOLVE_TIMEOUT;
			break;
		}
		TFTPOpen(&ipRemote);
		smUpload = TFTP_UPLOAD_CONNECT;
		break;
	case TFTP_UPLOAD_CONNECT:
		switch(TFTPIsOpened())
		{
		case TFTP_OK:
			TFTPOpenROMFile(vUploadFilename, TFTP_FILE_MODE_WRITE);
			smUpload = TFTP_UPLOAD_SEND_FILENAME;
			break;
		case TFTP_TIMEOUT:
			smUpload = TFTP_UPLOAD_CONNECT_TIMEOUT;
			break;
		default:
			break;
		}
		break;

	case TFTP_UPLOAD_SEND_FILENAME:
		result = TFTPIsFileOpened();
		switch(result)
		{
		case TFTP_OK:
			smUpload = TFTP_UPLOAD_SEND_DATA;
			break;
		case TFTP_RETRY:
			TFTPOpenROMFile(vUploadFilename, TFTP_FILE_MODE_WRITE);
			break;
		case TFTP_TIMEOUT:
			smUpload = TFTP_UPLOAD_CONNECT_TIMEOUT;
			break;
		case TFTP_ERROR:
			smUpload = TFTP_UPLOAD_SERVER_ERROR;
			break;
		default:
			break;
		}
		if(result != TFTP_OK)
			break;
		// No break when TFTPIsFileOpened() returns TFTP_OK -- we need to immediately start sending data

	case TFTP_UPLOAD_SEND_DATA:
		switch(TFTPIsPutReady())
		{
		case TFTP_OK:
			// Write blocksize bytes of data
			uploadChunkDescriptorForRetransmit = uploadChunkDescriptor;
			wUploadChunkOffsetForRetransmit = wUploadChunkOffset;
			vData = uploadChunkDescriptor->vDataPointer + wUploadChunkOffset;
			w = TFTP_BLOCK_SIZE;
			while(w)
			{
				w2 = uploadChunkDescriptor->wDataLength - wUploadChunkOffset;
				if(w2 > w)
					w2 = w;
				w -= w2;
				wUploadChunkOffset += w2;
				if(vData == NULL)
				{
					TFTPCloseFile();
					smUpload = TFTP_UPLOAD_WAIT_FOR_CLOSURE;
					break;
				}
				while(w2--)
				{
					TFTPPut(*vData++);
				}
				if(wUploadChunkOffset == uploadChunkDescriptor->wDataLength)
				{
					uploadChunkDescriptor++;
					wUploadChunkOffset = 0;
					vData = uploadChunkDescriptor->vDataPointer;
				}
			}
			break;

		case TFTP_RETRY:
			uploadChunkDescriptor = uploadChunkDescriptorForRetransmit;
			wUploadChunkOffset = wUploadChunkOffsetForRetransmit;
			break;

		case TFTP_TIMEOUT:
		case TFTP_ERROR:
			smUpload = TFTP_UPLOAD_SERVER_ERROR;
			break;

		default:
			break;
		}
		break;

	case TFTP_UPLOAD_WAIT_FOR_CLOSURE:
		switch(TFTPIsFileClosed())
		{
		case TFTP_OK:
			smUpload = TFTP_UPLOAD_COMPLETE;
			UDPClose(_tftpSocket);
			break;
		case TFTP_RETRY:
			uploadChunkDescriptor = uploadChunkDescriptorForRetransmit;
			wUploadChunkOffset = wUploadChunkOffsetForRetransmit;
			smUpload = TFTP_UPLOAD_SEND_DATA;
			break;
		case TFTP_TIMEOUT:
		case TFTP_ERROR:
			smUpload = TFTP_UPLOAD_SERVER_ERROR;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}
	
	return smUpload;
}
#endif


/*********************************************************************
 * Function:        void TFTPOpen(IP_ADDR *host)
 *
 * Summary:         Initializes TFTP module.
 *
 * PreCondition:    UDP module is already initialized
 *                  and at least one UDP socket is available.
 *
 * Input:           host        - IP address of remote TFTP server
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initiates ARP for given host and prepares
 *                  TFTP module for next sequence of function calls.
 *
 * Note:            Use TFTPIsOpened() to check if a connection was
 *                  successfully opened or not.
 *
 ********************************************************************/
void TFTPOpen(IP_ADDR *host)
{
    DEBUG(printf("Opening a connection..."));

    // Remember this address locally.
    MutExVar.group1._hostInfo.IPAddr.Val = host->Val;

    // Initiate ARP resolution.
   // ARPResolve(&MutExVar.group1._hostInfo.IPAddr);

    // Wait for ARP to get resolved.
    _tftpState = SM_TFTP_WAIT;

    // Mark this as start tick to detect timeout condition.
    _tftpStartTick = TickGet();

    // Forget about all previous attempts.
    _tftpRetries = 1;

}



/*********************************************************************
 * Function:        TFTP_RESULT TFTPIsOpened(void)
 *
 * Summary:         Determines if the TFTP connection is open.
 *
 * PreCondition:    TFTPOpen() is already called.
 *
 * Input:           None
 *
 * Output:          TFTP_OK if previous call to TFTPOpen is complete
 *
 *                  TFTP_TIMEOUT if remote host did not respond to
 *                          previous ARP request.
 *
 *                  TFTP_NOT_READY if remote has still not responded
 *                          and timeout has not expired.
 *
 * Side Effects:    None
 *
 * Overview:        Waits for ARP reply and opens a UDP socket
 *                  to perform further TFTP operations.
 *
 * Note:            Once opened, application may keep TFTP socket
 *                  open and future TFTP operations.
 *                  If TFTPClose() is called to close the connection
 *                  TFTPOpen() must be called again before performing
 *                  any other TFTP operations.
 ********************************************************************/
TFTP_RESULT TFTPIsOpened(void)
{
    switch(_tftpState)
    {
    default:
		_tftpState = SM_TFTP_READY;
#if 0
        DEBUG(printf("Resolving remote IP...\n"));

        // Check to see if adddress is resolved.
        if ( ARPIsResolved(&MutExVar.group1._hostInfo.IPAddr,
                           &MutExVar.group1._hostInfo.MACAddr) )
        {
            _tftpSocket = UDPOpenEx(TFTP_CLIENT_PORT,
                                  &MutExVar.group1._hostInfo,
                                  TFTP_SERVER_PORT);
            _tftpState = SM_TFTP_READY;
        }
        else
            break;
#endif		

    case SM_TFTP_READY:
        // Wait for UDP to be ready.  Immediately after this user will
        // may TFTPGetFile or TFTPPutFile and we have to make sure that
        // UDP is read to transmit.  These functions do not check for
        // UDP to get ready.
        if ( UDPIsPutReady(_tftpSocket) )
            return TFTP_OK;
    }

    // Make sure that we do not do this forever.
    if ( TickGet() - _tftpStartTick >= TFTP_ARP_TIMEOUT_VAL )
    {
        _tftpStartTick = TickGet();

        // Forget about all previous attempts.
        _tftpRetries = 1;

        return TFTP_TIMEOUT;
    }

    return TFTP_NOT_READY;
}



/*********************************************************************
 * Function:        void TFTPOpenFile(char *fileName,
 *                                    TFTP_FILE_MODE mode)
 *
 * Summary:         Prepares and sends TFTP file name and mode packet.
 *
 * PreCondition:    TFPTIsFileOpenReady() = TRUE
 *
 * Input:           fileName        - File name that is to be opened.
 *                  mode            - Mode of file access
 *                                    Must be
 *                                      TFTP_FILE_MODE_READ for read
 *                                      TFTP_FILE_MODE_WRITE for write
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Prepares and sends TFTP file name and mode packet.
 *
 * Note:            By default, this funciton uses "octet" or binary
 *                  mode of file transfer.
 *                  Use TFTPIsFileOpened() to check if file is
 *                  ready to be read or written.
 ********************************************************************/
void TFTPOpenFile(BYTE *fileName, TFTP_FILE_MODE mode)
{
    DEBUG(printf("Opening file...\n"));

    // Set TFTP Server port.  If this is the first call, remotePort
    // must have been set by TFTPOpen().  But if caller does not do
    // TFTPOpen for every transfer, we must reset remote port.
    // Most TFTP servers accept connection TFTP port. but once
    // connection is established, they use other temp. port,
    UDPSocketInfo[_tftpSocket].remotePort = TFTP_SERVER_PORT;
	memcpy(&UDPSocketInfo[_tftpSocket].remote.remoteHost, &MutExVar.group1._hostInfo.IPAddr.Val, sizeof(DWORD));

    // Tell remote server about our intention.
    _TFTPSendFileName(mode, fileName);

    // Clear all flags.
    _tftpFlags.Val = 0;

    // Remember start tick for this operation.
    _tftpStartTick = TickGet();

    // Depending on mode of operation, remote server will respond with
    // specific block number.
    if ( mode == TFTP_FILE_MODE_READ )
    {
        // Remember that we are reading a file.
        _tftpFlags.bits.bIsReading = TRUE;


        // For read operation, server would respond with data block of 1.
        MutExVar.group2._tftpBlockNumber.Val = 1;

        // Next packet would be the data packet.
        _tftpState = SM_TFTP_WAIT_FOR_DATA;
    }

    else
    {
        // Remember that we are writing a file.
        _tftpFlags.bits.bIsReading = FALSE;

        // For write operation, server would respond with data block of 0.
        MutExVar.group2._tftpBlockNumber.Val = 0;

        // Next packet would be the ACK packet.
        _tftpState = SM_TFTP_WAIT_FOR_ACK;
    }
}

#if defined(__18CXX)
void TFTPOpenROMFile(ROM BYTE *fileName, TFTP_FILE_MODE mode)
{
    DEBUG(printf("Opening file...\n"));

    // Set TFTP Server port.  If this is the first call, remotePort
    // must have been set by TFTPOpen().  But if caller does not do
    // TFTPOpen for every transfer, we must reset remote port.
    // Most TFTP servers accept connection TFTP port. but once
    // connection is established, they use other temp. port,
    UDPSocketInfo[_tftpSocket].remotePort = TFTP_SERVER_PORT;

    // Tell remote server about our intention.
    _TFTPSendROMFileName(mode, fileName);

    // Clear all flags.
    _tftpFlags.Val = 0;

    // Remember start tick for this operation.
    _tftpStartTick = TickGet();

    // Depending on mode of operation, remote server will respond with
    // specific block number.
    if ( mode == TFTP_FILE_MODE_READ )
    {
        // Remember that we are reading a file.
        _tftpFlags.bits.bIsReading = TRUE;


        // For read operation, server would respond with data block of 1.
        MutExVar.group2._tftpBlockNumber.Val = 1;

        // Next packet would be the data packet.
        _tftpState = SM_TFTP_WAIT_FOR_DATA;
    }

    else
    {
        // Remember that we are writing a file.
        _tftpFlags.bits.bIsReading = FALSE;

        // For write operation, server would respond with data block of 0.
        MutExVar.group2._tftpBlockNumber.Val = 0;

        // Next packet would be the ACK packet.
        _tftpState = SM_TFTP_WAIT_FOR_ACK;
    }
}
#endif



/*********************************************************************
 * Function:        TFTP_RESULT TFTPIsFileOpened(void)
 *
 * Summary:         Determines if file has been opened.
 *
 * PreCondition:    TFTPOpenFile() is called.
 *
 * Input:           None
 *
 * Output:          TFTP_OK if file is ready to be read or written
 *
 *                  TFTP_RETRY if previous attempt was timed out
 *                  needs to be retried.
 *
 *                  TFTP_TIMEOUT if all attempts were exhausted.
 *
 *                  TFTP_ERROR if remote server responded with
 *                  error
 *
 *                  TFTP_NOT_READY if file is not yet opened.
 *
 * Side Effects:    None
 *
 * Overview:        Waits for remote server response regarding
 *                  previous attempt to open file.
 *                  If no response is received within specified
 *                  timeout, fnction returns with TFTP_RETRY
 *                  and application logic must issue another
 *                  TFTPFileOpen().
 *
 * Note:            None
 ********************************************************************/
TFTP_RESULT TFTPIsFileOpened(void)
{
    if ( _tftpFlags.bits.bIsReading )
        return TFTPIsGetReady();

    else
        return TFTPIsPutReady();
}




/*********************************************************************
 * Function:        TFTP_RESULT TFTPIsGetReady(void)
 *
 * Summary:         Determines if a data block is ready to be read.
 *
 * PreCondition:    TFTPOpenFile() is called with TFTP_FILE_MODE_READ
 *                  and TFTPIsFileOpened() returned with TRUE.
 *
 * Input:           None
 *
 * Output:          TFTP_OK if it there is more data byte available
 *                  to read
 *
 *                  TFTP_TIMEOUT if timeout occurred waiting for
 *                  new data.
 *
 *                  TFTP_END_OF_FILE if end of file has reached.
 *
 *                  TFTP_ERROR if remote server returned ERROR.
 *                      Actual error code may be read by calling
 *                      TFTPGetError()
 *
 *                  TFTP_NOT_READY if still waiting for new data.
 *
 * Side Effects:    None
 *
 * Overview:        Waits for data block.  If data block does not
 *                  arrive within specified timeout, it automatically
 *                  sends out ack for previous block to remind
 *                  server to send next data block.
 *                  If all attempts are exhausted, it returns with
 *                  TFTP_TIMEOUT.
 *
 * Note:            By default, this funciton uses "octet" or binary
 *                  mode of file transfer.
 ********************************************************************/
TFTP_RESULT TFTPIsGetReady(void)
{
    WORD_VAL opCode;
    WORD_VAL blockNumber;
    BOOL bTimeOut;


    // Check to see if timeout has occurred.
    bTimeOut = FALSE;
    if ( TickGet() - _tftpStartTick >= TFTP_GET_TIMEOUT_VAL )
    {
        bTimeOut = TRUE;
        _tftpStartTick = TickGet();
    }


    switch(_tftpState)
    {
    case SM_TFTP_WAIT_FOR_DATA:
        // If timeout occurs in this state, it may be because, we have not
        // even received very first data block or some in between block.
        if ( bTimeOut == TRUE )
        {
            bTimeOut = FALSE;

            if ( _tftpRetries++ > (TFTP_MAX_RETRIES-1) )
            {
                DEBUG(printf("TFTPIsGetReady(): Timeout.\n"));

                // Forget about all previous attempts.
                _tftpRetries = 1;

                return TFTP_TIMEOUT;
            }

            // If we have not even received first block, ask application
            // retry.
            if ( MutExVar.group2._tftpBlockNumber.Val == 1u )
            {
                DEBUG(printf("TFTPIsGetReady(): TFTPOpen Retry.\n"));
                return TFTP_RETRY;
            }
            else
            {
                DEBUG(printf("TFTPIsGetReady(): ACK Retry #%d...,\n", _tftpRetries));

                // Block number was already incremented in last ACK attempt,
                // so decrement it.
                MutExVar.group2._tftpBlockNumber.Val--;

                // Do it.
                _tftpState = SM_TFTP_SEND_ACK;
                break;
            }
        }

        // For Read operation, server will respond with data block.
        if ( !UDPIsGetReady(_tftpSocket) )
            break;

        // Get opCode
        UDPGet(&opCode.v[1]);
        UDPGet(&opCode.v[0]);

        // Get block number.
        UDPGet(&blockNumber.v[1]);
        UDPGet(&blockNumber.v[0]);

        // In order to read file, this must be data with block number of 0.
        if ( opCode.Val == (WORD)TFTP_OPCODE_DATA )
        {
            // Make sure that this is not a duplicate block.
            if ( MutExVar.group2._tftpBlockNumber.Val == blockNumber.Val )
            {
                // Mark that we have not acked this block.
                _tftpFlags.bits.bIsAcked = FALSE;

                // Since we have a packet, forget about previous retry count.
                _tftpRetries = 1;

                _tftpState = SM_TFTP_READY;
                return TFTP_OK;
            }

            // If received block has already been received, simply ack it
            // so that Server can "get over" it and send next block.
            else if ( MutExVar.group2._tftpBlockNumber.Val > blockNumber.Val )
            {
                DEBUG(printf("TFTPIsGetReady(): "\
                            "Duplicate block %d received - droping it...\n", \
                            blockNumber.Val));
                MutExVar.group2._tftpDuplicateBlock.Val = blockNumber.Val;
                _tftpState = SM_TFTP_DUPLICATE_ACK;
            }
#if defined(TFTP_DEBUG)
            else
            {
                DEBUG(printf("TFTPIsGetReady(): "\
                             "Unexpected block %d received - droping it...\n", \
                             blockNumber.Val));
            }
#endif
        }
        // Discard all unexpected and error blocks.
        UDPDiscard();

        // If this was an error, remember error code for later delivery.
        if ( opCode.Val == (WORD)TFTP_OPCODE_ERROR )
        {
            _tftpError = blockNumber.Val;
            return TFTP_ERROR;
        }

        break;

    case SM_TFTP_DUPLICATE_ACK:
        if ( UDPIsPutReady(_tftpSocket) )
        {
            _TFTPSendAck(MutExVar.group2._tftpDuplicateBlock);
            _tftpState = SM_TFTP_WAIT_FOR_DATA;
        }
        break;

    case SM_TFTP_READY:
        if ( UDPIsGetReady(_tftpSocket) )
        {
            _tftpStartTick = TickGet();
            return TFTP_OK;
        }

        // End of file is reached when data block is less than 512 bytes long.
        // To reduce code, only MSB compared against 0x02 (of 0x200 = 512) to
        // determine if block is less than 512 bytes long or not.
        else if ( MutExVar.group2._tftpBlockLength.Val == 0u ||
                  MutExVar.group2._tftpBlockLength.v[1] < TFTP_BLOCK_SIZE_MSB )
            _tftpState = SM_TFTP_SEND_LAST_ACK;
        else
            break;


    case SM_TFTP_SEND_LAST_ACK:
    case SM_TFTP_SEND_ACK:
        if ( UDPIsPutReady(_tftpSocket) )
        {
            _TFTPSendAck(MutExVar.group2._tftpBlockNumber);

            // This is the next block we are expecting.
            MutExVar.group2._tftpBlockNumber.Val++;

            // Remember that we have already acked current block.
            _tftpFlags.bits.bIsAcked = TRUE;

            if ( _tftpState == SM_TFTP_SEND_LAST_ACK )
                return TFTP_END_OF_FILE;

            _tftpState = SM_TFTP_WAIT_FOR_DATA;
        }
        break;

	// Suppress compiler warnings on unhandled SM_TFTP_WAIT and 
	// SM_TFTP_WAIT_FOR_ACK states.
    default:	
    	break;
    }



    return TFTP_NOT_READY;
}



/*********************************************************************
 * Function:        BYTE TFTPGet(void)
 *
 * Summary:         Gets a data byte from data that was read.
 *
 * PreCondition:    TFTPOpenFile() is called with TFTP_FILE_MODE_READ
 *                  and TFTPIsGetReady() = TRUE
 *
 * Input:           None
 *
 * Output:          data byte as received from remote server.
 *
 * Side Effects:    None
 *
 * Overview:        Fetches next data byte from TFTP socket.
 *                  If end of data block is reached, it issues
 *                  ack to server so that next data block can be
 *                  received.
 *
 * Note:            Use this function to read file from server.
 ********************************************************************/
BYTE TFTPGet(void)
{
    BYTE v;

    // Read byte from UDP
    UDPGet(&v);

    // Update block length
    MutExVar.group2._tftpBlockLength.Val++;

    // Check to see if entire data block is fetched.
    // To reduce code, MSB is compared for 0x02 (of 0x200 = 512).
    if ( MutExVar.group2._tftpBlockLength.v[1] == TFTP_BLOCK_SIZE_MSB )
    {
        // Entire block was fetched.  Discard everything else.
        UDPDiscard();

        // Remember that we have flushed this block.
        _tftpFlags.bits.bIsFlushed = TRUE;

        // Reset block length.
        MutExVar.group2._tftpBlockLength.Val = 0;

        // Must send ACK to receive next block.
        _tftpState = SM_TFTP_SEND_ACK;
    }

    return v;
}




/*********************************************************************
 * Function:        void TFTPCloseFile(void)
 *
 * Summary:         Sends file closing messages.
 *
 * PreCondition:    TFTPOpenFile() was called and TFTPIsFileOpened()
 *                  had returned with TFTP_OK.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        If file is opened in read mode, it makes sure
 *                  that last ACK is sent to server
 *                  If file is opened in write mode, it makes sure
 *                  that last block is sent out to server and
 *                  waits for server to respond with ACK.
 *
 * Note:            TFTPIsFileClosed() must be called to confirm
 *                  if file was really closed.
 ********************************************************************/
void TFTPCloseFile(void)
{
    // If a file was opened for read, we can close it immediately.
    if ( _tftpFlags.bits.bIsReading )
    {
        // If this was premature close, make sure that we discard
        // current block.
        if ( !_tftpFlags.bits.bIsFlushed )
        {
            _tftpFlags.bits.bIsFlushed = TRUE;
            UDPIsGetReady(_tftpSocket);
            UDPDiscard();
        }

        if ( _tftpFlags.bits.bIsAcked )
        {
            _tftpFlags.bits.bIsClosed = TRUE;
            _tftpFlags.bits.bIsClosing = FALSE;
            return;
        }

        else
        {
            _tftpState = SM_TFTP_SEND_LAST_ACK;
            _tftpFlags.bits.bIsClosing = TRUE;
        }
        return;
    }

    // For write mode, if we have not flushed last block, do it now.
    if ( !_tftpFlags.bits.bIsFlushed )
    {
        _tftpFlags.bits.bIsFlushed = TRUE;
        UDPIsPutReady(_tftpSocket);
        UDPFlush();
    }

    // For write mode, if our last block was ack'ed by remote server,
    // file is said to be closed.
    if ( _tftpFlags.bits.bIsAcked )
    {
        _tftpFlags.bits.bIsClosed = TRUE;
        _tftpFlags.bits.bIsClosing = FALSE;
        return;
    }

    _tftpState = SM_TFTP_WAIT_FOR_ACK;
    _tftpFlags.bits.bIsClosing =  TRUE;

}



/*********************************************************************
 * Function:        TFTP_RESULT TFTPIsFileClosed(void)
 *
 * Summary:         Determines if the file was closed.
 *
 * PreCondition:    TFTPCloseFile() is already called.
 *
 * Input:           None
 *
 * Output:          TFTP_OK if file was successfully closdd
 *
 *                  TFTP_RETRY if file mode was Write and remote
 *                  server did not receive last packet.
 *                  Application must retry with last block.
 *
 *                  TFTP_TIMEOUT if all attempts were exhausted
 *                  in closing file.
 *
 *                  TFTP_ERROR if remote server sent an error
 *                  in response to last block.
 *                  Actual error code may be read by calling
 *                  TFTPGetError()
 *
 *                  TFTP_NOT_READY if file is not closed yet.
 *
 * Side Effects:    None
 *
 * Overview:        If file mode is Read, it simply makes that
 *                  last block is acknowledged.
 *                  If file mode is Write, it waits for server ack.
 *                  If no ack was received within specified timeout
 *                  instructs appliaction to resend last block.
 *                  It keeps track of retries and declares timeout
 *                  all attempts were exhausted.
 *
 * Note:            None
 ********************************************************************/
TFTP_RESULT TFTPIsFileClosed(void)
{
    if ( _tftpFlags.bits.bIsReading )
        return TFTPIsGetReady();

    else
        return TFTPIsPutReady();
}




/*********************************************************************
 * Function:        TFTP_RESULT TFTPIsPutReady(void)
 *
 * Summary:         Determines if data can be written to a file.
 *
 * PreCondition:    TFTPOpenFile() is called with TFTP_FILE_MODE_WRITE
 *                  and TFTPIsFileOpened() returned with TRUE.
 *
 * Input:           None
 *
 * Output:          TFTP_OK if it is okay to write more data byte.
 *
 *                  TFTP_TIMEOUT if timeout occurred waiting for
 *                  ack from server
 *
 *                  TFTP_RETRY if all server did not send ack
 *                  on time and application needs to resend
 *                  last block.
 *
 *                  TFTP_ERROR if remote server returned ERROR.
 *                  Actual error code may be read by calling
 *                  TFTPGetError()
 *
 *                  TFTP_NOT_READY if still waiting...
 *
 * Side Effects:    None
 *
 * Overview:        Waits for ack from server.  If ack does not
 *                  arrive within specified timeout, it it instructs
 *                  application to retry last block by returning
 *                  TFTP_RETRY.
 *
 *                  If all attempts are exhausted, it returns with
 *                  TFTP_TIMEOUT.
 *
 * Note:            None
 ********************************************************************/
TFTP_RESULT TFTPIsPutReady(void)
{
    WORD_VAL opCode;
    WORD_VAL blockNumber;
    BOOL bTimeOut;

    // Check to see if timeout has occurred.
    bTimeOut = FALSE;
    if ( TickGet() - _tftpStartTick >= TFTP_GET_TIMEOUT_VAL )
    {
        bTimeOut = TRUE;
        _tftpStartTick = TickGet();
    }

    switch(_tftpState)
    {
    case SM_TFTP_WAIT_FOR_ACK:
        // When timeout occurs in this state, application must retry.
        if ( bTimeOut )
        {
            if ( _tftpRetries++ > (TFTP_MAX_RETRIES-1) )
            {
                DEBUG(printf("TFTPIsPutReady(): Timeout.\n"));

                // Forget about all previous attempts.
                _tftpRetries = 1;

                return TFTP_TIMEOUT;
            }

            else
            {
                DEBUG(printf("TFTPIsPutReady(): Retry.\n"));
                _tftpState = SM_TFTP_WAIT;
                MutExVar.group2._tftpBlockNumber.Val--;	// Roll back by one so proper block number ID is sent for the next packet
                return TFTP_RETRY;
            }
        }

        // Must wait for ACK from server before we transmit next block.
        if ( !UDPIsGetReady(_tftpSocket) )
            break;

        // Get opCode.
        UDPGet(&opCode.v[1]);
        UDPGet(&opCode.v[0]);

        // Get block number.
        UDPGet(&blockNumber.v[1]);
        UDPGet(&blockNumber.v[0]);

        // Discard everything else.
        UDPDiscard();

        // This must be ACK or else there is a problem.
        if ( opCode.Val == (WORD)TFTP_OPCODE_ACK )
        {
            // Also the block number must match with what we are expecting.
            if ( MutExVar.group2._tftpBlockNumber.Val == blockNumber.Val )
            {
                // Mark that block we sent previously has been ack'ed.
                _tftpFlags.bits.bIsAcked = TRUE;

                // Since we have ack, forget about previous retry count.
                _tftpRetries = 1;

                // If this file is being closed, this must be last ack.
                // Declare it as closed.
                if ( _tftpFlags.bits.bIsClosing )
                {
                    _tftpFlags.bits.bIsClosed = TRUE;
                    return TFTP_OK;
                }

                // Or else, wait for put to become ready so that caller
                // can transfer more data blocks.
                _tftpState = SM_TFTP_WAIT;
            }

            else
            {
                DEBUG(printf("TFTPIsPutReady(): "\
                    "Unexpected block %d received - droping it...\n", \
                    blockNumber.Val));
                return TFTP_NOT_READY;
            }
        }

        else if ( opCode.Val == (WORD)TFTP_OPCODE_ERROR )
        {
            // For error opCode, remember error code so that application
            // can read it later.
            _tftpError = blockNumber.Val;

            // Declare error.
            return TFTP_ERROR;
        }
        else
            break;


    case SM_TFTP_WAIT:
        // Wait for UDP is to be ready to transmit.
        if ( UDPIsPutReady(_tftpSocket) )
        {
            // Put next block of data.
            MutExVar.group2._tftpBlockNumber.Val++;
            UDPPut(0);
            UDPPut(TFTP_OPCODE_DATA);

            UDPPut(MutExVar.group2._tftpBlockNumber.v[1]);
            UDPPut(MutExVar.group2._tftpBlockNumber.v[0]);

            // Remember that this block is not yet flushed.
            _tftpFlags.bits.bIsFlushed = FALSE;

            // Remember that this block is not acknowledged.
            _tftpFlags.bits.bIsAcked = FALSE;

            // Now, TFTP module is ready to put more data.
            _tftpState = SM_TFTP_READY;

            return TFTP_OK;
        }
        break;

    case SM_TFTP_READY:
        // TFTP module is said to be ready only when underlying UDP
        // is ready to transmit.
        if ( UDPIsPutReady(_tftpSocket) )
            return TFTP_OK;

	// Suppress compiler warnings on unhandled SM_TFTP_WAIT_FOR_DATA, 
	// SM_TFTP_DUPLICATE_ACK, SM_TFTP_SEND_ACK, SM_TFTP_SEND_LAST_ACK enum 
	// states.
    default:	
    	break;
    }

    return TFTP_NOT_READY;
}



/*********************************************************************
 * Function:        void TFTPPut(BYTE c)
 *
 * Summary:         Write a byte to a file.
 *
 * PreCondition:    TFTPOpenFile() is called with TFTP_FILE_MODE_WRITE
 *                  and TFTPIsPutReady() = TRUE
 *
 * Input:           c       - Data byte that is to be written
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Puts given data byte into TFTP socket.
 *                  If end of data block is reached, it
 *                  transmits entire block.
 *
 * Note:            Use this function to write file to server.
 ********************************************************************/
void TFTPPut(BYTE c)
{
    // Put given byte directly to UDP
    UDPPut(c);

    // One more byte in data block.
    ++MutExVar.group2._tftpBlockLength.Val;

    // Check to see if data block is full.
    if ( MutExVar.group2._tftpBlockLength.v[1] == TFTP_BLOCK_SIZE_MSB )
    {
        // If it is, then transmit this block.
        UDPFlush();

        // Remember that current block is already flushed.
        _tftpFlags.bits.bIsFlushed = TRUE;

        // Prepare for next block.
        MutExVar.group2._tftpBlockLength.Val = 0;

        // Need to wait for ACK from server before putting
        // next block of data.
        _tftpState = SM_TFTP_WAIT_FOR_ACK;
    }
}




static void _TFTPSendFileName(TFTP_OPCODE opcode, BYTE *fileName)
{
    BYTE c;

	// Select the proper UDP socket and wait until we can write to it
	while(!UDPIsPutReady(_tftpSocket));

    // Write opCode
    UDPPut(0);
    UDPPut(opcode);

    // write file name, including NULL.
    do
    {
        c = *fileName++;
        UDPPut(c);
    } while ( c != '\0' );

    // Write mode - always use octet or binay mode to transmit files.
    UDPPut('o');
    UDPPut('c');
    UDPPut('t');
    UDPPut('e');
    UDPPut('t');
    UDPPut(0);

    // Transmit it.
    UDPFlush();

    // Reset data block length.
    MutExVar.group2._tftpBlockLength.Val = 0;
}

#if defined(__18CXX)
static void _TFTPSendROMFileName(TFTP_OPCODE opcode, ROM BYTE *fileName)
{
    BYTE c;

	// Select the proper UDP socket and wait until we can write to it
	while(!UDPIsPutReady(_tftpSocket));

    // Write opCode
    UDPPut(0);
    UDPPut(opcode);

    // write file name, including NULL.
    do
    {
        c = *fileName++;
        UDPPut(c);
    } while ( c != '\0' );

    // Write mode - always use octet or binay mode to transmit files.
    UDPPut('o');
    UDPPut('c');
    UDPPut('t');
    UDPPut('e');
    UDPPut('t');
    UDPPut(0);

    // Transmit it.
    UDPFlush();

    // Reset data block length.
    MutExVar.group2._tftpBlockLength.Val = 0;
}
#endif

static void _TFTPSendAck(WORD_VAL blockNumber)
{
    // Write opCode.
    UDPPut(0);
    UDPPut(TFTP_OPCODE_ACK);

    // Write block number for this ack.
    UDPPut(blockNumber.v[1]);
    UDPPut(blockNumber.v[0]);

    // Transmit it.
    UDPFlush();
}



#endif //#if defined(STACK_USE_TFTP_CLIENT)
