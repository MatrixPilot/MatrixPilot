/*********************************************************************
 *
 *  SSLv3 Protocol Client and Server Implementation
 *  Module for Microchip TCP/IP Stack
 *    - Implements an SSL layer supporting both client and server
 *		operation for any given TCP socket.
 *
 **********************************************************************
 * FileName:        SSL.c
 * Dependencies:    TCPIP.h
 *					SSL.h
 *                  TCP.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
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
 ********************************************************************/
#define __SSL_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)

#include "TCPIP Stack/TCPIP.h"
	
/****************************************************************************
  Section:
	SSL Connection State Global Variables
  ***************************************************************************/
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata SSL_STATE_RAM
	#endif
	static SSL_STUB sslStub;		// The current SSL stub
	static BYTE sslStubID;			// Which SSL_STUB is loaded
	static SSL_KEYS sslKeys;		// The current SSL session
	static BYTE sslKeysID;			// Which SSL_KEYS are loaded
	static BYTE sslBufferID;		// Which buffer is loaded
	static BYTE sslHashID;			// Which hash is loaded
	static BYTE sslSessionID;		// Which session is loaded
	static BOOL sslSessionUpdated;	// Whether or not it has been updated
	static BYTE sslRSAStubID;		// Which stub is using RSA, if any
	
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata SSL_LARGE_RAM
	#endif
	SSL_BUFFER sslBuffer;			// SBox and RSA storage

	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata SSL_HASH_RAM
	#endif
	HASH_SUM sslHash;				// Hash storage

	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata SSL_SESSION_RAM
	#endif
	SSL_SESSION sslSession;			// Current session data
	
	// 8 byte session stubs
	SSL_SESSION_STUB sslSessionStubs[MAX_SSL_SESSIONS];
	
	BYTE *ptrHS;					// Used in buffering handshake results

	extern ROM WORD SSL_CERT_LEN;	// RSA public certificate length		?
	extern ROM BYTE SSL_CERT[];		// RSA public certificate data			?

	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata						
	#endif
	
/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/
	// Section: Cryptographic Calculation Functions
	static RSA_STATUS SSLRSAOperation(void);
	static void GenerateHashRounds(BYTE num, BYTE* rand1, BYTE* rand2);
	static void CalculateFinishedHash(BYTE hashID, BOOL fromClient, BYTE *result);
	static void GenerateSessionKeys(void);

	// Section: Ethernet Buffer RAM Management
	static void SSLStubSync(BYTE id);
	static BOOL SSLStubAlloc(void);
	static void SSLStubFree(BYTE id);
	static void SSLKeysSync(BYTE id);
	static void SSLHashSync(BYTE id);
	static void SSLHashAlloc(BYTE *id);
	static void SSLHashFree(BYTE *id);
	static void SSLBufferSync(BYTE id);
	static void SSLBufferAlloc(BYTE *id);
	static void SSLBufferFree(BYTE *id);
	static BYTE SSLSessionNew(void);
	static void SSLSessionSync(BYTE id);
	#define SSLSessionUpdated()		sslSessionUpdated = TRUE;
	static void SaveOffChip(BYTE *ramAddr, PTR_BASE ethAddr, WORD len);
	static void LoadOffChip(BYTE *ramAddr, PTR_BASE ethAddr, WORD len);
	
	// Section: Handshake Hash and I/O Functions
	static void HSStart(void);
	static void HSEnd(void);
	static WORD HSGet(TCP_SOCKET skt, BYTE *b);
	static WORD HSGetWord(TCP_SOCKET skt, WORD *w);
	static WORD HSGetArray(TCP_SOCKET skt, BYTE *data, WORD len);
	static WORD HSPut(TCP_SOCKET skt, BYTE b);
	static WORD HSPutWord(TCP_SOCKET skt, WORD w);
	static WORD HSPutArray(TCP_SOCKET skt, BYTE *data, WORD len);
	#if defined(STACK_USE_SSL_SERVER)
	static WORD HSPutROMArray(TCP_SOCKET skt, ROM BYTE *data, WORD len);
	#endif
	
	// Section: Client messages
	#if defined(STACK_USE_SSL_CLIENT)
		static BYTE SSLSessionMatchIP(IP_ADDR ip);
		static void SSLTxClientHello(TCP_SOCKET hTCP);
		static void SSLRxServerHello(TCP_SOCKET hTCP);
		static void SSLRxServerCertificate(TCP_SOCKET hTCP);
		static void SSLTxClientKeyExchange(TCP_SOCKET hTCP);
	#endif
	
	// Section: Server messages
	#if defined(STACK_USE_SSL_SERVER)
		static BYTE SSLSessionMatchID(BYTE* SessionID);
		static void SSLRxAntiqueClientHello(TCP_SOCKET hTCP);
		static void SSLRxClientHello(TCP_SOCKET hTCP);
		static void SSLTxServerHello(TCP_SOCKET hTCP);
		static void SSLTxServerCertificate(TCP_SOCKET hTCP);
		static void SSLTxServerHelloDone(TCP_SOCKET hTCP);
		static void SSLRxClientKeyExchange(TCP_SOCKET hTCP);
	#endif
	
	// Section: Client and server messages
	static void SSLTxCCSFin(TCP_SOCKET hTCP);
	static void SSLRxCCS(TCP_SOCKET hTCP);
	static void SSLRxFinished(TCP_SOCKET hTCP);
	static void SSLRxAlert(TCP_SOCKET hTCP);

/****************************************************************************
  Section:
	Macros and Definitions
  ***************************************************************************/
	#define mMIN(a, b)	((a<b)?a:b)

	#define SSL_RSA_EXPORT_WITH_ARCFOUR_40_MD5	0x0003u
	#define SSL_RSA_WITH_ARCFOUR_128_MD5		0x0004u

/****************************************************************************
  Section:
	Resource Management Variables
  ***************************************************************************/
WORD isStubUsed;			// Indicates which stubs are in use
WORD isHashUsed;			// Indicates which hashes are in use
WORD isBufferUsed;			// Indicates which buffers are in use

// Masks for each bit in the is*Used variables
static ROM WORD masks[16] = { 0x0001, 0x0002, 0x0004, 0x0008,
							  0x0010, 0x0020, 0x0040, 0x0080, 
							  0x0100, 0x0200, 0x0400, 0x0800,
							  0x1000, 0x2000, 0x4000, 0x8000 };

// Base address for SSL stubs
#define SSL_BASE_STUB_ADDR			(BASE_SSLB_ADDR)

// Base address for SSL keys
#define SSL_BASE_KEYS_ADDR			(BASE_SSLB_ADDR + SSL_STUB_SPACE)

// Base address for SSL hashes
#define SSL_BASE_HASH_ADDR			(BASE_SSLB_ADDR + SSL_STUB_SPACE + SSL_KEYS_SPACE)

// Base address for SSL buffers
#define SSL_BASE_BUFFER_ADDR		(BASE_SSLB_ADDR + SSL_STUB_SPACE + SSL_KEYS_SPACE + SSL_HASH_SPACE)

// Base address for SSL sessions
#define SSL_BASE_SESSION_ADDR		(BASE_SSLB_ADDR + SSL_STUB_SPACE + SSL_KEYS_SPACE + SSL_HASH_SPACE + SSL_BUFFER_SPACE)


/****************************************************************************
  ===========================================================================
  Section:
	SSL Management Functions
  ===========================================================================
  ***************************************************************************/

/*****************************************************************************
  Function:
	void SSLInit(void)

  Description:
	Initializes the SSL engine.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function is called only one during lifetime of the application.
  ***************************************************************************/
void SSLInit(void)
{
	// Set all resources to unused
	isStubUsed = 0;
	isHashUsed = 0;
	isBufferUsed = 0;
	for(sslSessionID = 0; sslSessionID < MAX_SSL_SESSIONS; sslSessionID++)
		sslSessionStubs[sslSessionID].tag.Val = 0;
		
	// Indicate that nothing is loaded
	sslHashID = SSL_INVALID_ID;
	sslStubID = SSL_INVALID_ID;
	sslSessionID = SSL_INVALID_ID;
	sslKeysID = SSL_INVALID_ID;
	sslBufferID = SSL_INVALID_ID;
	sslSessionUpdated = FALSE;
	sslRSAStubID = SSL_INVALID_ID;
}	

/*****************************************************************************
  Function:
	void SSLPeriodic(TCP_SOCKET hTCP, BYTE id)

  Summary:
	Performs any periodic tasks for the SSL module.

  Description:
	This function performs periodic tasks for the SSL module.  This includes
	processing for RSA operations.

  Precondition:
	SSL has already been initialized.

  Parameters:
	hTCP - the socket for which to perform periodic functions
	id - the SSL stub to use
	
  Returns:
  	None
  	
  ***************************************************************************/
void SSLPeriodic(TCP_SOCKET hTCP, BYTE id)
{
	// Sync the SSL Stub
	SSLStubSync(id);
	
	// For new sessions, try to claim a session
	if(sslStub.Flags.bNewSession && sslStub.idSession == SSL_INVALID_ID)
	{
		sslStub.idSession = SSLSessionNew();
	}
	
	// If RSA is in progress, do some RSA work
	if(sslStub.Flags.bRSAInProgress)
	{			
		if(SSLRSAOperation() == RSA_DONE)
		{// Move on with the connection
			sslStub.Flags.bRSAInProgress = 0;

			// For clients, request the CKE message
			#if defined(STACK_USE_SSL_CLIENT)
			if(!sslStub.Flags.bIsServer)
				TCPRequestSSLMessage(hTCP, SSL_CLIENT_KEY_EXCHANGE);
			#endif
			
			// For servers, copy the decoded message to the session data
			#if defined(STACK_USE_SSL_SERVER)
			if(sslStub.Flags.bIsServer)
			{
				// Copy over the pre-master secret
				SSLSessionSync(sslStub.idSession);
				memcpy((void*)sslSession.masterSecret, (void*)&sslBuffer.full[(SSL_RSA_KEY_SIZE/8)-48], 48);
												
				// Generate the Master Secret
				SSLKeysSync(sslStubID);
				SSLBufferSync(SSL_INVALID_ID);
				GenerateHashRounds(3, sslKeys.Remote.random, sslKeys.Local.random);
				memcpy(sslSession.masterSecret, (void*)sslBuffer.hashRounds.temp, 48);
				
				// Note the new session data and release RSA engine
				SSLSessionUpdated();
				RSAEndUsage();
				sslRSAStubID = SSL_INVALID_ID;
			}
			
			// Continue receiving the CCS and Finished messages
			TCPSSLHandleIncoming(hTCP);
			#endif
		}
	}
}

/*****************************************************************************
  Function:
	BYTE SSLStartSession(TCP_SOCKET hTCP, BYTE * buffer, BYTE supDataType)

  Description:
	Begins a new SSL session for the given TCP connection.

  Precondition:
	SSL has been initialized and hTCP is connected.

  Parameters:
	hTCP - the socket to begin the SSL connection on
	buffer - pointer to a supplementary data buffer
	supDataType - type of supplementary data to store
	
  Return Values:
  	SSL_INVALID_ID - insufficient SSL resources to start a new connection
  	others - the allocated SSL stub ID
  ***************************************************************************/
BYTE SSLStartSession(TCP_SOCKET hTCP, void * buffer, BYTE supDataType)
{
	BYTE i;
	
	// Allocate a stub for use, or fail
	if(!SSLStubAlloc())
		return SSL_INVALID_ID;
	
	// Clear stub state
	sslStub.wRxBytesRem = 0;
	sslStub.wRxHsBytesRem = 0;
	*(WORD*)&sslStub.Flags = 0x0000;
	
	// Clear any allocations
	sslStub.idSession = SSL_INVALID_ID;
	sslStub.idRxHash = SSL_INVALID_ID;
	sslStub.idMD5 = SSL_INVALID_ID;
	sslStub.idSHA1 = SSL_INVALID_ID;
	sslStub.idRxBuffer = SSL_INVALID_ID;
	sslStub.idTxBuffer = SSL_INVALID_ID;
	sslStub.requestedMessage = SSL_NO_MESSAGE;
	sslStub.dwTemp.Val = 0;
	sslStub.supplementaryBuffer = buffer;
    sslStub.supplementaryDataType = supDataType;

	// Allocate handshake hashes for use, or fail
	SSLHashAlloc(&sslStub.idMD5);
	SSLHashAlloc(&sslStub.idSHA1);
	if(sslStub.idMD5 == SSL_INVALID_ID || sslStub.idSHA1 == SSL_INVALID_ID)
	{
		SSLHashFree(&sslStub.idMD5);
		SSLHashFree(&sslStub.idSHA1);
		SSLStubFree(sslStubID);
		return SSL_INVALID_ID;
	}
	
	// Initialize the handshake hashes
	SSLHashSync(sslStub.idSHA1);
	SHA1Initialize(&sslHash);
	SSLHashSync(sslStub.idMD5);
	MD5Initialize(&sslHash);
	
	// Set up Local.random (4 byte UTC time, 28 bytes random)
	SSLKeysSync(sslStubID);
	#if defined(STACK_USE_SNTP_CLIENT)
	{
		DWORD_VAL temp;

		temp.Val = SNTPGetUTCSeconds();
		sslKeys.Local.random[0] = temp.v[3];
		sslKeys.Local.random[1] = temp.v[2];
		sslKeys.Local.random[2] = temp.v[1];
		sslKeys.Local.random[3] = temp.v[0];
		i = 4;
	}
	#else
		i = 0;
	#endif
	while(i < 32u)
		sslKeys.Local.random[i++] = RandomGet();
		
	// Return the ID
	return sslStubID;
}

/*****************************************************************************
  Function:
	void SSLTerminate(BYTE id)

  Description:
	Terminates an SSL connection and releases allocated resources.

  Precondition:
	None

  Parameters:
	id - the SSL stub ID to terminate
	
  Returns:
  	None
  ***************************************************************************/
void SSLTerminate(BYTE id)
{
	// Sync in the right stub
	SSLStubSync(id);
	
	// If no CloseNotify, then invalidate the session so it cannot resume
	// ( This restriction is not presently enforced.  IE incorrectly
	//   completes the handshake, then disconnects without a CloseNotify
	//   when it decides to prompt the user whether or not to accept a 
	//   unverifiable certificate. )
	//if(!sslStub.Flags.bCloseNotify)
	//{
	//	sslSessionStubs[sslStub.idSession].tag.Val = 0;
	//}	
	
	// Free up resources
	SSLBufferFree(&sslStub.idRxBuffer);
	SSLBufferFree(&sslStub.idTxBuffer);
	SSLHashFree(&sslStub.idMD5);
	SSLHashFree(&sslStub.idSHA1);
	SSLHashFree(&sslStub.idRxHash);
	SSLStubFree(id);
	if(sslRSAStubID == id)
	{
		sslRSAStubID = SSL_INVALID_ID;
		RSAEndUsage();
	}
	
}

/****************************************************************************
  ===========================================================================
  Section:
	SSL Record Processing Functions
  ===========================================================================
  ***************************************************************************/

/*****************************************************************************
  Function:
	WORD SSLRxRecord(TCP_SOCKET hTCP, BYTE id)

  Summary:
	Receives an SSL record.

  Description:
	Reads at most one SSL Record header from the TCP stream and determines what
	to do with the rest of the data.  If not all of the data is available for 
	the record, then the function returns and future call(s) to SSLRxRecord() 
	will process the remaining data until the end of the record is reached.  
	If this call process data from a past record, the next record will not be 
	started until the next call.

  Precondition:
	The specified SSL stub is initialized and the TCP socket is connected.

  Parameters:
	hTCP - The TCP socket from which to read
	id - The active SSL stub ID
	
  Returns:
  	WORD indicating the number of data bytes there were decrypted but left in 
  	the stream.

  Remarks:
  	SSL record headers, MAC footers, and symetric cipher block padding (if any) 
  	will be extracted from the TCP stream by this function.  Data will be 
  	decrypted but left in the stream.
  ***************************************************************************/
WORD SSLRxRecord(TCP_SOCKET hTCP, BYTE id)
{	
	BYTE temp[32];
	WORD wLen;
	
	SSLStubSync(id);
	
	// Don't do anything for terminated connections
	if(sslStub.Flags.bDone)
		return 0;

	// If this is a new record, then read the header
	// When bytes remain, a message is not yet fully read, so
	// the switch statement will continue handling the data
	if(sslStub.wRxBytesRem == 0u)
	{
		// See if we expect a MAC
		if(sslStub.Flags.bExpectingMAC)
		{// Receive and verify the MAC
			if(TCPIsGetReady(hTCP) < 16u)
				return 0;
				
			// Read the MAC
			TCPGetArray(hTCP, temp, 16);
			
			// Calculate the expected MAC
			SSLBufferSync(sslStub.idRxBuffer);
			SSLKeysSync(id);
			SSLHashSync(sslStub.idRxHash);
			
			ARCFOURCrypt(&sslKeys.Remote.app.cryptCtx, temp, 16);
			SSLMACCalc(sslKeys.Remote.app.MACSecret, &temp[16]);
			
			// MAC no longer expected
			sslStub.Flags.bExpectingMAC = 0;
			
			// Verify the MAC
			if(memcmp((void*)temp, (void*)&temp[16], 16) != 0)
			{// MAC fails
				TCPRequestSSLMessage(hTCP, SSL_ALERT_BAD_RECORD_MAC);
				return 0;
			}
		}	
		
		// Check if a new header is available
		// Also ignore data if SSL is terminated
		if(TCPIsGetReady(hTCP) < 5u)
			return 0;
		
		// Read the record type (BYTE)
		TCPGet(hTCP, &sslStub.rxProtocol);
		
		#if defined(STACK_USE_SSL_SERVER)
		// Check if we've received an SSLv2 ClientHello message
		// Client-only implementations don't need to deal with this
		if((sslStub.rxProtocol & 0x80) == 0x80)
		{
			// After MSB, next 15 bits are the length
			((BYTE*)&sslStub.wRxBytesRem)[1] = sslStub.rxProtocol & 0x7F;
			TCPGet(hTCP, ((BYTE*)&sslStub.wRxBytesRem));
			
			// Tell the handshaker what message to expect
			sslStub.wRxHsBytesRem = sslStub.wRxBytesRem;
			sslStub.rxProtocol = SSL_HANDSHAKE;
			sslStub.rxHSType = SSL_ANTIQUE_CLIENT_HELLO;
		}
		
		// Otherwise, this is a normal SSLv3 message
		// Read the rest of the record header and proceed normally
		else
		#endif
		{
			// Read version (WORD, currently ignored)
			TCPGet(hTCP, NULL);
			TCPGet(hTCP, NULL);
	
			// Read length (WORD)
			TCPGet(hTCP, ((BYTE*)&sslStub.wRxBytesRem)+1);
			TCPGet(hTCP, ((BYTE*)&sslStub.wRxBytesRem));
			
			// Determine if a MAC is expected
			if(sslStub.Flags.bRemoteChangeCipherSpec)
			{
				sslStub.Flags.bExpectingMAC = 1;
				sslStub.wRxBytesRem -= 16;
							
				// Set up the MAC
				SSLKeysSync(sslStubID);
				SSLHashSync(sslStub.idRxHash);
				SSLMACBegin(sslKeys.Remote.app.MACSecret, 
					sslKeys.Remote.app.sequence++, 
					sslStub.rxProtocol, sslStub.wRxBytesRem);
			}
		}
		
	}
	
	// See if data is ready that needs decryption
	wLen = TCPIsGetReady(hTCP);

	// Decrypt and MAC if necessary
	if(sslStub.Flags.bRemoteChangeCipherSpec && wLen)
	{// Need to decrypt the data
		
		// Only decrypt up to end of record
		if(wLen > sslStub.wRxBytesRem)
			wLen = sslStub.wRxBytesRem;
						
		// Prepare for decryption
		SSLKeysSync(id);
		SSLBufferSync(sslStub.idRxBuffer);
		SSLHashSync(sslStub.idRxHash);

		// Decrypt application data to proper location, non-app in place
		TCPSSLDecryptMAC(hTCP, &sslKeys.Remote.app.cryptCtx, wLen);
	}
	
	// Determine what to do with the rest of the data
	switch(sslStub.rxProtocol)
	{
		case SSL_HANDSHAKE:
			SSLRxHandshake(hTCP);
			break;
			
		case SSL_CHANGE_CIPHER_SPEC:
			SSLRxCCS(hTCP);
			break;
			
		case SSL_APPLICATION:
			// Data was handled above
			// Just note that it's all been read
			sslStub.wRxBytesRem -= wLen;
			return wLen;
		
		case SSL_ALERT:
			SSLRxAlert(hTCP);
			break;
	}
	
	return 0;
}

/*****************************************************************************
  Function:
	void SSLTxRecord(TCP_SOCKET hTCP, BYTE id, BYTE txProtocol)

  Summary:
	Transmits an SSL record.

  Description:
	Transmits all pending data in the TCP TX buffer as an SSL record using
	the specified protocol.  This function transparently encrypts and MACs
	the data if there is an active cipher spec.

  Precondition:
	The specified SSL stub is initialized and the TCP socket is connected.

  Parameters:
	hTCP - The TCP socket with data waiting to be transmitted
	id - The active SSL stub ID
	txPortocol - The SSL protocol number to attach to this record
	
  Returns:
  	None
  ***************************************************************************/
void SSLTxRecord(TCP_SOCKET hTCP, BYTE id, BYTE txProtocol)
{
	WORD_VAL wLen;
	BYTE hdr[5];
	
	SSLStubSync(id);
	
	// If stub is done, prevent writing data
	if(sslStub.Flags.bDone)
		return;
	
	// Determine how many bytes are ready to write
	wLen.Val = TCPSSLGetPendingTxSize(hTCP);
	if(wLen.Val == 0u)
		return;
	
	// Determine if a MAC is required
	if(sslStub.Flags.bLocalChangeCipherSpec)
	{// Perform the encryption and MAC
		// Sync needed data
		SSLKeysSync(sslStubID);
		SSLHashSync(SSL_INVALID_ID);
		SSLBufferSync(sslStub.idTxBuffer);
		
		// Start the MAC calculation
		SSLMACBegin(sslKeys.Local.app.MACSecret, 
			sslKeys.Local.app.sequence, txProtocol, wLen.Val);
		sslKeys.Local.app.sequence++;
		
		// Get ready to send
		TCPSSLInPlaceMACEncrypt(hTCP, &sslKeys.Local.app.cryptCtx,
				sslKeys.Local.app.MACSecret, wLen.Val);
		
		// Add MAC length to the data length
		wLen.Val += 16;
	}
	
	// Prepare the header
	hdr[0] = txProtocol;
	hdr[1] = SSL_VERSION_HI;
	hdr[2] = SSL_VERSION_LO;
	hdr[3] = wLen.v[1];
	hdr[4] = wLen.v[0];
	
	// Put the record header and send the data
	TCPSSLPutRecordHeader(hTCP, hdr, TRUE);
	
}

/*****************************************************************************
  Function:
	void SSLStartPartialRecord(TCP_SOCKET hTCP, BYTE id, BYTE txProtocol,
								 WORD wLen)

  Summary:
	Begins a long SSL record.

  Description:
	This function allows messages longer than the TCP buffer to be sent,
	which is frequently the case for the Certificate handshake message.  The
	final message length is required to be known in order to transmit the
	header.  Once called, SSLFlushPartialRecord and SSLFinishPartialRecord
	must be called to write remaining data, finalize, and prepare for a new
	record.

  Precondition:
	The specified SSL stub is initialized and the TCP socket is connected.

  Parameters:
	hTCP - The TCP socket with data waiting to be transmitted
	id - The active SSL stub ID
	txPortocol - The SSL protocol number to attach to this record
	wLen - The length of all the data to be sent
	
  Returns:
  	None
  
  Remarks:
	Partial messages do not support the current cipher spec, so this can
	only be used during the handshake procedure.
  ***************************************************************************/
#if defined(STACK_USE_SSL_SERVER)
void SSLStartPartialRecord(TCP_SOCKET hTCP, BYTE id, BYTE txProtocol, WORD wLen)
{
	BYTE hdr[5];
	
	SSLStubSync(id);
	
	// Prepare the header
	hdr[0] = txProtocol;
	hdr[1] = SSL_VERSION_HI;
	hdr[2] = SSL_VERSION_LO;
	hdr[3] = wLen >> 8;
	hdr[4] = wLen;
	
	// Put the record header and send the data
	TCPSSLPutRecordHeader(hTCP, hdr, FALSE);
	
}
#endif

/*****************************************************************************
  Function:
	void SSLTxMessage(TCP_SOCKET hTCP, BYTE id, BYTE msg)

  Summary:
	Transmits an SSL message.

  Description:
	This function transmits a specific SSL message for handshakes and alert
	messages.  Supported messages are listed in SSL_MESSAGES.

  Precondition:
	The specified SSL stub is initialized and the TCP socket is connected.

  Parameters:
	hTCP - The TCP socket with data waiting to be transmitted
	id - The active SSL stub ID
	msg - One of the SSL_MESSAGES types to send
	
  Returns:
  	None
  ***************************************************************************/
void SSLTxMessage(TCP_SOCKET hTCP, BYTE id, BYTE msg)
{
	SSLStubSync(id);
	
	// Don't do anything for terminated connections
	if(sslStub.Flags.bDone)
		return;
	
	// Transmit the requested message
	switch(msg)
	{
		#if defined(STACK_USE_SSL_CLIENT)
		case SSL_CLIENT_HELLO:
			SSLTxClientHello(hTCP);
			break;
		case SSL_CLIENT_KEY_EXCHANGE:
			SSLTxClientKeyExchange(hTCP);
			break;
		#endif
		
		#if defined(STACK_USE_SSL_SERVER)
		case SSL_SERVER_HELLO:
			SSLTxServerHello(hTCP);
			break;
		case SSL_CERTIFICATE:
			SSLTxServerCertificate(hTCP);
			break;
		case SSL_SERVER_HELLO_DONE:
			SSLTxServerHelloDone(hTCP);
			break;
		#endif
		
		case SSL_CHANGE_CIPHER_SPEC:
			SSLTxCCSFin(hTCP);
			break;
			
		// Handle all alert messages
		default:
			if((msg & 0x80) != 0x80)
				break;
			
			// Make sure we can write the message
			if(TCPIsPutReady(hTCP) < 2u)
				break;
			
			// Select FATAL or WARNING
			if(msg == SSL_ALERT_CLOSE_NOTIFY)
			{
				TCPPut(hTCP, SSL_ALERT_WARNING);
				sslStub.Flags.bCloseNotify = 1;
			}
			else
				TCPPut(hTCP, SSL_ALERT_FATAL);
			
			// Put the message byte
			TCPPut(hTCP, msg - 0x80);
			
			// Flush the message
			SSLTxRecord(hTCP, sslStubID, SSL_ALERT);
			TCPRequestSSLMessage(hTCP, SSL_NO_MESSAGE);
			
			// Mark session as terminated
			sslStub.Flags.bDone = 1;
	}
	
}

/*****************************************************************************
  Function:
	void SSLRxHandshake(TCP_SOCKET hTCP)

  Summary:
	Receives a handshake message.

  Description:
	This function receives handshake messages, reads the handshake header,
	and passes the data off to the appropriate handshake parser. 

  Precondition:
	The specified SSL stub is initialized and the TCP socket is connected.
	Also requires that rxBytesRem has been populated and the current SSL stub
	has been synced into memory.

  Parameters:
	hTCP - The TCP socket to read a handshake message from
	
  Returns:
  	None
  ***************************************************************************/
void SSLRxHandshake(TCP_SOCKET hTCP)
{
	WORD wLen;
	
	// Start reading handshake data
	HSStart();
	
	// If this is a new handshake message, read the header
	// If the message has already been started, there will
	// still be bytes remaining and the switch statement will 
	// handle the rest.
	if(sslStub.wRxHsBytesRem == 0u)
	{
		// Make sure entire header is in the buffer
		if(TCPIsGetReady(hTCP) < 4u)
			return;
		
		// Read the message type (BYTE)
		HSGet(hTCP, &sslStub.rxHSType);
		
		// Read the length (3 BYTES)
		HSGet(hTCP, NULL);
		HSGetWord(hTCP, &wLen);
		sslStub.wRxHsBytesRem = wLen;
	}
	
	// Determine what to do with the rest of the data
	switch(sslStub.rxHSType)
	{
		#if defined(STACK_USE_SSL_CLIENT)
		case SSL_SERVER_HELLO:
			SSLRxServerHello(hTCP);
			break;

		case SSL_CERTIFICATE:
			SSLRxServerCertificate(hTCP);
			break;
			
		case SSL_SERVER_HELLO_DONE:
			// This message contains no data
			// Record that message was received
			sslStub.Flags.bServerHelloDone = 1;
			break;
		#endif
		
		#if defined(STACK_USE_SSL_SERVER)
		case SSL_ANTIQUE_CLIENT_HELLO:
			SSLRxAntiqueClientHello(hTCP);
			break;

		case SSL_CLIENT_HELLO:
			SSLRxClientHello(hTCP);
			break;
			
		case SSL_CLIENT_KEY_EXCHANGE:
			SSLRxClientKeyExchange(hTCP);
			break;
		#endif
		
		case SSL_FINISHED:
			SSLRxFinished(hTCP);
			break;
	}
	
	// End reading handshake data
	HSEnd();
	
}	


/****************************************************************************
  ===========================================================================
  Section:
	SSL Message Processing Functions
  ===========================================================================
  ***************************************************************************/

/*********************************************************************
 * Function:        BYTE SSLTxClientHello(TCP_SOCKET hTCP)
 *
 * PreCondition:    Enough space is available in hTCP to write the
 *					entire message. 
 *
 * Input:           hTCP - the TCP Socket to send the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Transmits the ClientHello message to initiate a
 *					new SSL session with the server.
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_CLIENT)
static void SSLTxClientHello(TCP_SOCKET hTCP)
{	
	// Restart the handshake hasher
	HSStart();
	
	// Indicate that we're the client
	//sslStub.Flags.bIsServer = 0;  // This is the default already
	
	// Make sure enough space is available to transmit
	if(TCPIsPutReady(hTCP) < 100u)
		return;
	
	// Look for a valid session to reuse
	sslStub.idSession = SSLSessionMatchIP(TCPGetRemoteInfo(hTCP)->remote.IPAddr);
	sslStub.Flags.bNewSession = (sslStub.idSession == SSL_INVALID_ID);
	
	// If none is found, generate a new one
	if(sslStub.Flags.bNewSession)
	{
		sslStub.idSession = SSLSessionNew();
		if(sslStub.idSession == SSL_INVALID_ID)
		{// No free sessions, so abort
			return;
		}

		// Mark session as using this IP
		memcpy((void*)&sslSessionStubs[sslStub.idSession].tag.v[0],
				(void*)&(TCPGetRemoteInfo(hTCP)->remote.IPAddr), 4);
	}

	// Send handshake message header (hashed)
	HSPut(hTCP, SSL_CLIENT_HELLO);
	HSPut(hTCP, 0x00);				
	HSPut(hTCP, 0x00);				// Message length is 40 bytes,
	if(sslStub.Flags.bNewSession)	// plus 32 more if a session
		HSPut(hTCP, 43);			// ID is being included.
	else
		HSPut(hTCP, 43+32);
	
	// Send 
	HSPut(hTCP, SSL_VERSION_HI);
	HSPut(hTCP, SSL_VERSION_LO);
	
	// Put Client.Random
	HSPutArray(hTCP, sslKeys.Local.random, 32);
	
	// Put Session ID
	if(sslStub.Flags.bNewSession)
	{// Send no session ID
		HSPut(hTCP, 0x00);
	}
	else
	{// Send the requested Session ID
		SSLSessionSync(sslStub.idSession);
		HSPut(hTCP, 0x20);
		HSPutArray(hTCP, sslSession.sessionID, 32);
	}
	
	// Put Cipher Suites List
	HSPutWord(hTCP, 0x0004);
	HSPutWord(hTCP, SSL_RSA_WITH_ARCFOUR_128_MD5);
	HSPutWord(hTCP, SSL_RSA_EXPORT_WITH_ARCFOUR_40_MD5);
	
	// Put Compression Methods List (just null)
	HSPut(hTCP, 0x01);
	HSPut(hTCP, 0x00);
	
	// End the handshake and save the hash
	HSEnd();
	
	// Send record
	SSLTxRecord(hTCP, sslStubID, SSL_HANDSHAKE);
	
	// Record that message was sent
	TCPRequestSSLMessage(hTCP, SSL_NO_MESSAGE);
	sslStub.Flags.bClientHello = 1;

}
#endif

/*********************************************************************
 * Function:        BYTE SSLRxClientHello(TCP_SOCKET hTCP)
 *
 * PreCondition:    Handshake hasher is started, and SSL has a stub
 *					assigned. 
 *
 * Input:           hTCP - the TCP Socket to send the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives the ClientHello message, initiating a
 *					new SSL session with a client
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_SERVER)
static void SSLRxClientHello(TCP_SOCKET hTCP)
{
	WORD w;
	BYTE c, *ptrID;
	
	// Make sure entire message is ready
	if(TCPIsGetReady(hTCP) < sslStub.wRxHsBytesRem)
		return;
		
	// Verify handshake message sequence
	if(sslStub.Flags.bClientHello)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Indicate that we're the server
	sslStub.Flags.bIsServer = 1;
	
	// Read the version again
	HSGetWord(hTCP, &w);
	// Ignore the version here.  It must be at least 3.0 to receive this type
	// of message, and Safari 3.1 sends 0x0301 (TLS 1.0) even when the last 
	// connection was only 0x0300 (SSL 3.0)
		
	// Make sure the session keys are synced
	SSLKeysSync(sslStubID);
	
	// Read the Client.Random array
	HSGetArray(hTCP, sslKeys.Remote.random, 32);
	
	// Read the Session ID length
	HSGet(hTCP, &c);
	
	// Read the Session ID if it exists
	sslStub.Flags.bNewSession = TRUE;
	if(c > 0u)
	{
		// Note where it will be stored in RAM
		ptrID = ptrHS;
		HSGetArray(hTCP, NULL, c);
		
		// Try to match it with a known session
		sslStub.idSession = SSLSessionMatchID(ptrID);
		if(sslStub.idSession != SSL_INVALID_ID)
			sslStub.Flags.bNewSession = FALSE;
	}
	
	// If we we're starting a new session, try to obtain a free one
	if(sslStub.Flags.bNewSession)
		sslStub.idSession = SSLSessionNew();
	
	// Read CipherSuites length
	HSGetWord(hTCP, &w);
	
	// Check for an acceptable CipherSuite
	// Right now we just ignore this and assume support for 
	// SSL_RSA_WITH_ARCFOUR_128_MD5.  If we request this suite later 
	// and it isn't supported, the client will kill the connection.
	HSGetArray(hTCP, NULL, w);
	
	// Read the Compression Methods length
	HSGet(hTCP, &c);
	
	// Check for an acceptable Compression Method
	// Right now we just ignore this and assume support for
	// NULL_COMPRESSION.  If we request this later and the client
	// doesn't really support it, they'll kill the connection.
	HSGetArray(hTCP, NULL, c);
	
	// For TLS compatibility, we must ignore further bytes in ClientHello.
	// FF2+ may send "extensions" ad other things we don't support
	HSGetArray(hTCP, NULL, sslStub.wRxBytesRem);
	
	// Mark message as received and request a ServerHello
	sslStub.Flags.bClientHello = 1;
	TCPRequestSSLMessage(hTCP, SSL_SERVER_HELLO);

}
#endif

/*********************************************************************
 * Function:        BYTE SSLRxAntiqueClientHello(TCP_SOCKET hTCP)
 *
 * PreCondition:    Handshake hasher is started, and SSL has a stub
 *					assigned.
 *
 * Input:           hTCP - the TCP Socket to send the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives the SSLv2 ClientHello message, initiating
 *					a new SSL session with a client
 *
 * Note:            This is the only SSLv2 message we support, and
 *					is provided for browsers seeking backwards
 *					compatibility.  Connections must be upgraded to
 *					SSLv3.0 immediately following, otherwise the 
 *					connection will fail.
 ********************************************************************/
#if defined(STACK_USE_SSL_SERVER)
static void SSLRxAntiqueClientHello(TCP_SOCKET hTCP)
{
	WORD suiteLen, idLen, randLen;
	BYTE c;
	
	// Make sure entire message is ready
	if(TCPIsGetReady(hTCP) < sslStub.wRxHsBytesRem)
		return;
		
	// Verify handshake message sequence
	if(sslStub.Flags.bClientHello)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
		
	// Indicate that we're the server
	sslStub.Flags.bIsServer = 1;
	
	// Make sure the session keys are synced
	SSLKeysSync(sslStubID);
	
	// Read and verify the handshake message type
	HSGet(hTCP, &c);
	if(c != 0x01u)
	{// This message is not supported, so handshake fails
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
		return;
	}
	
	// Read and verify the version
	HSGet(hTCP, &c);
	if(c != SSL_VERSION_HI)
	{// Version is too low, so handshake fails
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
		return;
	}
	HSGet(hTCP, &c);	// Ignore low byte of version number
	
	// Read the CipherSuite length
	HSGetWord(hTCP, &suiteLen);
	
	// Read Session ID Length
	HSGetWord(hTCP, &idLen);
	
	// Read Challenge (Client.Random) length
	HSGetWord(hTCP, &randLen);
		
	// Check for an acceptable CipherSuite
	// Right now we just ignore this and assume support for 
	// SSL_RSA_WITH_ARCFOUR_128_MD5.  If we request this suite later 
	// and it isn't supported, the client will kill the connection.
	HSGetArray(hTCP, NULL, suiteLen);
	
	// Read the SessionID
	// SSLv3 clients will send a v3 ClientHello when resuming, so
	// this is always a new session.  Therefore, ignore the ID
	HSGetArray(hTCP, NULL, idLen);
	
	// Obtain a new session
	sslStub.idSession = SSLSessionNew();
	sslStub.Flags.bNewSession = 1;
	
	// Read Client.Random
	// This needs to be 32 bytes, so zero-pad the left side
	for(c = 0; c < 32 - randLen; c++)
		sslKeys.Remote.random[c] = 0;
	HSGetArray(hTCP, &sslKeys.Remote.random[c], randLen);
	
	// Mark message as received and request a ServerHello
	TCPRequestSSLMessage(hTCP, SSL_SERVER_HELLO);
	sslStub.Flags.bClientHello = 1;
}
#endif

/*********************************************************************
 * Function:        void SSLRxServerHello(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized and HSStart() has been
 *					called.
 *
 * Input:           hTCP - the TCP Socket to read from
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives the ServerHello from the remote server
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_CLIENT)
static void SSLRxServerHello(TCP_SOCKET hTCP)
{
	BYTE b, sessionID[32];
	WORD w;
		
	// Make sure entire message is ready
	if(TCPIsGetReady(hTCP) < sslStub.wRxHsBytesRem)
		return;
		
	// Verify handshake message sequence
	if(!sslStub.Flags.bClientHello || sslStub.Flags.bServerHello)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Make sure correct session and key set are loaded
	SSLKeysSync(sslStubID);
	
	// Read Version (2)
	HSGetWord(hTCP, NULL);
	
	// Read Server.Random (32)
	HSGetArray(hTCP, sslKeys.Remote.random, 32);
	
	// Read Session ID Length (byte)
	HSGet(hTCP, &b);
	
	// Read Session ID (if any)
	SSLSessionSync(sslStub.idSession);
	if(b != 0u)
	{
		HSGetArray(hTCP, sessionID, b);

		// If reusing a session, check if our session ID was accepted
		if(!sslStub.Flags.bNewSession &&
			memcmp((void*)sslSession.sessionID, (void*)sessionID, 32) == 0)
		{// Session restart was accepted
			// Nothing to do here...move along
		}
		else
		{// This is a new session
			memcpy((void*)sslSession.sessionID, (void*)sessionID, 32);

			// Reset the RxServerCertificate state machine
			sslStub.dwTemp.v[0] = RX_SERVER_CERT_START;
		}
	}
	else
	{
		// Session is non-resumable, so invalidate its tag
		sslSessionStubs[sslStub.idSession].tag.Val = 0;
	}
	
	// Read and verify Cipher Suite (WORD)
	HSGetWord(hTCP, &w);
	if(w != SSL_RSA_WITH_ARCFOUR_128_MD5)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Read and verify Compression Method (BYTE)
	HSGet(hTCP, &b);
	if(b != 0x00u)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Note that message was received
	sslStub.Flags.bServerHello = 1;
	
	// Note that we updated session data
	SSLSessionUpdated();
}
#endif

/*********************************************************************
 * Function:        BYTE SSLTxServerHello(TCP_SOCKET hTCP)
 *
 * PreCondition:    None
 *
 * Input:           hTCP - the TCP Socket to send the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Transmits the ServerHello message.
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_SERVER)
static void SSLTxServerHello(TCP_SOCKET hTCP)
{
	BYTE i;
	
	// Only continue if the session has been obtained
	if(sslStub.idSession == SSL_INVALID_ID)
		return;
	
	// Make sure enough space is available to transmit
	if(TCPIsPutReady(hTCP) < 78u)
		return;
	
	// Restart the handshake hasher
	HSStart();
	
	// Sync the session and keys
	SSLKeysSync(sslStubID);
	SSLSessionSync(sslStub.idSession);
	
	// If this session is new, generate an ID
	if(sslStub.Flags.bNewSession)
	{
		for(i = 0; i < 32u; i++)
			sslSession.sessionID[i] = RandomGet();
		SSLSessionUpdated();
		
		// Tag this session identifier
		memcpy((void*)&sslSessionStubs[sslStub.idSession].tag.v[1],
			(void*)(sslSession.sessionID), 3);
	}

	// Send handshake message header (hashed)
	HSPut(hTCP, SSL_SERVER_HELLO);
	HSPut(hTCP, 0x00);				
	HSPut(hTCP, 0x00);				
	HSPut(hTCP, 70);
	
	// Send the version number
	HSPut(hTCP, SSL_VERSION_HI);
	HSPut(hTCP, SSL_VERSION_LO);
	
	// Put Server.Random
	#if defined(STACK_USE_SNTP_CLIENT)
	{
		DWORD_VAL temp;
		
		temp.Val = SNTPGetUTCSeconds();
		sslKeys.Local.random[0] = temp.v[3];
		sslKeys.Local.random[1] = temp.v[2];
		sslKeys.Local.random[2] = temp.v[1];
		sslKeys.Local.random[3] = temp.v[0];
		i = 4;
	}
	#else
		i = 0;
	#endif
	while(i < 32u)
		sslKeys.Local.random[i++] = RandomGet();
	HSPutArray(hTCP, sslKeys.Local.random, 32);
	
	// Put Session ID
	HSPut(hTCP, 0x20);
	HSPutArray(hTCP, sslSession.sessionID, 32);
	
	// Put Cipher Suites
	HSPutWord(hTCP, SSL_RSA_WITH_ARCFOUR_128_MD5);
	
	// Put Compression Method (just null)
	HSPut(hTCP, 0x00);
	
	// End the handshake and save the hash
	HSEnd();
	
	// Send record
	SSLTxRecord(hTCP, sslStubID, SSL_HANDSHAKE);
	
	// Record that message was sent and request the next message
	sslStub.Flags.bServerHello = 1;
	TCPRequestSSLMessage(hTCP, SSL_NO_MESSAGE);
	if(sslStub.Flags.bNewSession)
		TCPRequestSSLMessage(hTCP, SSL_CERTIFICATE);
	else
		TCPRequestSSLMessage(hTCP, SSL_CHANGE_CIPHER_SPEC);
	
	// Set up to transmit certificate
	sslStub.dwTemp.Val = SSL_CERT_LEN;
}
#endif

/*********************************************************************
 * Function:        void SSLRxServerCertificate(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized and HSStart() has been
 *					called.
 *
 * Input:           hTCP - the TCP Socket to read from
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives ServerCertificate from the remote server,
 *					locates the public key information, and executes
 *					RSA operation.
 *
 * Note:            This shortcuts full parsing of the certificate by
 *					just finding the Public Key Algorithm identifier
 *					for RSA.  From there, the following ASN.1 struct
 *					is the public key.  That struct consists of the
 *					value for N, followed by the value for E.
 ********************************************************************/
#if defined(STACK_USE_SSL_CLIENT)
static void SSLRxServerCertificate(TCP_SOCKET hTCP)
{
	WORD len;
	BYTE i, e[3];
	WORD data_length;   // number of key bytes read from certificate
	BYTE length_bytes;  // decoded length value
	BYTE index;         // temp index
	
	// Verify handshake message sequence
	if(!sslStub.Flags.bServerHello || sslStub.Flags.bServerCertificate)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Check state machine variable
	switch(sslStub.dwTemp.v[0]) {

		case RX_SERVER_CERT_START:
			// Find RSA Public Key Algorithm identifier
			len = TCPFindROMArray(hTCP, (ROM BYTE*)"\x2A\x86\x48\x86\xF7\x0D\x01\x01\x01", 9, 0, FALSE);
			
			if(len == 0xFFFFu)
			{// If not found, clear most of buffer and return to wait
				HSGetArray(hTCP, NULL, TCPIsGetReady(hTCP) - 10);
				return;
			}
			
			// Otherwise, read it and move on
			HSGetArray(hTCP, NULL, len + 9);
			sslStub.dwTemp.v[0]++;
					
		case RX_SERVER_CERT_FIND_KEY:
			// Search for beginning of struct
			len = TCPFind(hTCP, 0x30, 0, FALSE);
			
			if(len == 0xFFFFu)
			{// Not found, so clear and return
				HSGetArray(hTCP, NULL, TCPIsGetReady(hTCP));
			}
			
			// Clear up through the 0x30
			HSGetArray(hTCP, NULL, len + 1);
			
			// Make sure 2 more bytes remain
			if(TCPIsGetReady(hTCP) < 3u)
				return;
			
			// Read 1 or 2 length bytes (ignore)
			HSGet(hTCP, &i);
			if(i > 0x80)
				HSGet(hTCP, &i);
				
			// Increment and continue
			sslStub.dwTemp.v[0]++;
		
		case RX_SERVER_CERT_FIND_N:
			// Make sure tag and length bytes are ready, plus one more
			if(TCPIsGetReady(hTCP) < 8u)
				return;
            // Read until 0x02
            i = 0;
            while(i != 2)
    			HSGet(hTCP, &i);
			// Read 1 or 2 length bytes to sslStub.dwTemp.v[1]
			// The next byte tells us how many bytes are in the length structure if it's MSB is set
			HSGet(hTCP, &i);
			data_length = 0;
			if(i & 0x80)
			{
    			length_bytes = i & 0x7F;
    			for(index=0;index<length_bytes;index++)
    			{
    				HSGet(hTCP, &i);
    				data_length = (data_length<<8)+i;
    			}
            }
            else
   				data_length = i;
            
			sslStub.dwTemp.w[1] = data_length;
			// If there's one odd byte, it's a leading zero that we don't need
			if(sslStub.dwTemp.w[1] & 0x01)
			{
				HSGet(hTCP, NULL);
				sslStub.dwTemp.w[1]--;
			}
			// The max modulus we support is 2048 bits
			if(sslStub.dwTemp.w[1] > SSL_RSA_CLIENT_SIZE/8)
			{
				TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
				sslStub.dwTemp.w[1] = SSL_RSA_CLIENT_SIZE/8;
				// Need to determine a graceful way to abort
			}

			// Increment and continue
			sslStub.dwTemp.v[0]++;
		
		case RX_SERVER_CERT_READ_N:
			// Make sure sslStub.dwTemp.v[1] bytes are ready
			if(TCPIsGetReady(hTCP) < sslStub.dwTemp.w[1])
				return;
			
			// N will be stored in sslBuffer, which is currently in use
			// for handshaking.  We can stop the handshake hashing, read 
			// and hash this data, then resume more handshake hashing
			HSEnd();
			
			// Claim an SSL Buffer for RSA operations
			SSLBufferAlloc(&sslStub.idRxBuffer);
			if(sslStub.idRxBuffer == SSL_INVALID_ID)
				return;
				
			// Make sure we can claim RSA Engine
			if(!RSABeginEncrypt(sslStub.dwTemp.w[1]))
				return;
			sslRSAStubID = sslStubID;
			
			// Read N to proper location
//			for(i = 0; i < 128u - sslStub.dwTemp.v[1]; i++)
//				sslBuffer.full[i] = 0x00;
			HSGetArray(hTCP, sslBuffer.full, sslStub.dwTemp.w[1]);
			
            if (sslStub.supplementaryDataType == SSL_SUPPLEMENTARY_DATA_CERT_PUBLIC_KEY)
            {
                SSL_PKEY_INFO * tmpPKeyPtr = ((SSL_PKEY_INFO *)sslStub.supplementaryBuffer);
                tmpPKeyPtr->pub_size_bytes = sslStub.dwTemp.w[1];
                if (tmpPKeyPtr->pub_size_bytes <= sizeof (tmpPKeyPtr->pub_key))
                    memcpy (&tmpPKeyPtr->pub_key[0], sslBuffer.full, tmpPKeyPtr->pub_size_bytes);
            }


			// Hash what we just read
			SSLHashSync(sslStub.idSHA1);
			HashAddData(&sslHash, sslBuffer.full, sslStub.dwTemp.w[1]);
			SSLHashSync(sslStub.idMD5);
			HashAddData(&sslHash, sslBuffer.full, sslStub.dwTemp.w[1]);
			
			// Generate { SSL_VERSION rand[46] } as pre-master secret & save
			SSLSessionSync(sslStub.idSession);
			sslSession.masterSecret[0] = SSL_VERSION_HI;
			sslSession.masterSecret[1] = SSL_VERSION_LO;
			for(i = 2; i < 48u; i++)
				sslSession.masterSecret[i] = RandomGet();
			SSLSessionUpdated();
			
			// Set RSA engine to use this data and key
			RSASetData(sslSession.masterSecret, 48, RSA_BIG_ENDIAN);
			RSASetN(sslBuffer.full, RSA_BIG_ENDIAN);
			RSASetResult(sslBuffer.full+sslStub.dwTemp.w[1], RSA_BIG_ENDIAN);
			
			// Start a new hash
			HSStart();
			
			// Increment and continue
			sslStub.dwTemp.v[0]++;
			
		case RX_SERVER_CERT_READ_E:
			// Make sure 5 bytes are ready
			if(TCPIsGetReady(hTCP) < 5u)
				return;

			// Read 0x02
			HSGet(hTCP, NULL);
			
			// Read 1 length byte to temp
			HSGet(hTCP, &i);
			if(i > 3u)
				i = 3;
			
			// Read E to temp
			HSGetArray(hTCP, e, i);

            if (sslStub.supplementaryDataType == SSL_SUPPLEMENTARY_DATA_CERT_PUBLIC_KEY)
            {
                SSL_PKEY_INFO * tmpPKeyPtr = ((SSL_PKEY_INFO *)sslStub.supplementaryBuffer);
                if (i <= sizeof (tmpPKeyPtr->pub_e))
                    memcpy (&tmpPKeyPtr->pub_e[0], e, i);
            }

			// Set RSA engine to encrypt with E
			RSASetE(e, i, RSA_BIG_ENDIAN);
			
			// Increment and continue
			sslStub.dwTemp.v[0]++;
		
		case RX_SERVER_CERT_CLEAR:
			// Clear up to sslStub.wRxHsBytesRem from hTCP
			len = TCPIsGetReady(hTCP);
			if(len > sslStub.wRxHsBytesRem)
				len = sslStub.wRxHsBytesRem;
			HSGetArray(hTCP, NULL, len);
			
			// If we're done, kick off the RSA encryption next
			if(sslStub.wRxHsBytesRem == 0u)
			{
				// Set periodic function to do RSA operation
				sslStub.Flags.bRSAInProgress = 1;
				sslSessionStubs[sslStub.idSession].lastUsed += SSL_RSA_LIFETIME_EXTENSION;
				
				// Note that we've received this message
				sslStub.Flags.bServerCertificate = 1;	
			}
			
			break;
	}
}
#endif

/*********************************************************************
 * Function:        BYTE SSLTxServerCertificate(TCP_SOCKET hTCP)
 *
 * PreCondition:    None
 *
 * Input:           hTCP - the TCP Socket to send the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Transmits the Certificate message with the 
 *					server's specified public key certificate.
 *
 * Note:            Certificate is defined in CustomSSLCert.c.
 *					This function requires special handling for
 *					partial records because the certificate will 
 *					likely be larger than the TCP buffer, and SSL
 *					handshake messages are constrained to fit in a
 *					single SSL handshake record
 ********************************************************************/
#if defined(STACK_USE_SSL_SERVER)
static void SSLTxServerCertificate(TCP_SOCKET hTCP)
{
	WORD len;
	ROM BYTE* loc;
	
	// Restart the handshake hasher
	HSStart();
	
	// See how much we can write
	len = TCPIsPutReady(hTCP);
	
	// If full certificate remains, write the headers
	if(sslStub.dwTemp.Val == SSL_CERT_LEN)
	{
		// Make sure we can send all headers plus one byte
		if(len < 11u)
			return;
		
		// Transmit the handshake headers
		HSPut(hTCP, SSL_CERTIFICATE);
		HSPut(hTCP, 0x00);
		HSPutWord(hTCP, SSL_CERT_LEN + 3 + 3);
		
		// Send length of all certificates
		HSPut(hTCP, 0x00);
		HSPutWord(hTCP, SSL_CERT_LEN + 3);
		
		// Send length of this (only) certificate
		HSPut(hTCP, 0x00);
		HSPutWord(hTCP, SSL_CERT_LEN);
		
		// Put in the record header and begin the partial record sending
		SSLStartPartialRecord(hTCP, sslStubID, SSL_HANDSHAKE, SSL_CERT_LEN + 3 + 3 + 4);
		
		// Update free space
		len -= 10;
	}
	
	// Figure out where to start, and how much to send
	loc = SSL_CERT + (SSL_CERT_LEN - sslStub.dwTemp.Val);
	if(sslStub.dwTemp.Val < len)
		len = sslStub.dwTemp.Val;
		
	// Write the bytes
	HSPutROMArray(hTCP, loc, len);
	sslStub.dwTemp.Val -= len;
	
	// End the handshake and save the hash
	HSEnd();
	
	// Send record
	SSLFlushPartialRecord(hTCP);
		
	// Check if entire certificate was sent
	if(sslStub.dwTemp.Val == 0u)
	{
		// Finish the partial record
		SSLFinishPartialRecord(hTCP);
		
		// Record that message was sent and request a Certificate
		TCPRequestSSLMessage(hTCP, SSL_NO_MESSAGE);
		TCPRequestSSLMessage(hTCP, SSL_SERVER_HELLO_DONE);
		sslStub.Flags.bServerCertificate = 1;
	}
}
#endif

/*********************************************************************
 * Function:        BYTE SSLTxServerHelloDone(TCP_SOCKET hTCP)
 *
 * PreCondition:    None
 *
 * Input:           hTCP - the TCP Socket to send the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Transmits the ServerHelloDone message.
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_SERVER)
static void SSLTxServerHelloDone(TCP_SOCKET hTCP)
{
	// Make sure enough space is available to transmit
	if(TCPIsPutReady(hTCP) < 4u)
		return;
	
	// Restart the handshake hasher
	HSStart();
	
	// Send handshake message header (hashed)
	HSPut(hTCP, SSL_SERVER_HELLO_DONE);
	HSPut(hTCP, 0x00);				
	HSPut(hTCP, 0x00);				
	HSPut(hTCP, 0x00);

	// Message has no content, so we're done
	
	// End the handshake and save the hash
	HSEnd();
	
	// Send record
	SSLTxRecord(hTCP, sslStubID, SSL_HANDSHAKE);
	
	// Record that message was sent
	TCPRequestSSLMessage(hTCP, SSL_NO_MESSAGE);
	sslStub.Flags.bServerHelloDone = 1;
}
#endif

/*********************************************************************
 * Function:        void SSLTxClientKeyExchange(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized, sslStub.dwTemp.v[1]
 *					contains the length of the public key, and 
 *					the RxBuffer contains the encrypted pre-master
 *					secret at address 0x80.
 *
 * Input:           hTCP - the TCP Socket to write the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Transmits the encrypted pre-master secret to the
 *					server and requests the Change Cipher Spec.  Also
 *					generates the Master Secret from the pre-master
 *					secret that was used.
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_CLIENT)
static void SSLTxClientKeyExchange(TCP_SOCKET hTCP)
{
	WORD len;
	
	// Load length of modulus from RxServerCertificate
	len = sslStub.dwTemp.w[1];
	
	// Make sure there's len+9 bytes free
	if(TCPIsPutReady(hTCP) < len + 9)
		return;
	
	// Start the handshake processor
	HSStart();

	// Send handshake message header (hashed)
	HSPut(hTCP, SSL_CLIENT_KEY_EXCHANGE);
	HSPut(hTCP, 0x00);				
	HSPut(hTCP, (len>>8)&0xFF);				// Message length is (length of key) bytes
	HSPut(hTCP, len&0xFF);
	
	// Suspend the handshake hasher and load the buffer
	HSEnd();
	SSLBufferSync(sslStub.idRxBuffer);	

	// Send encrypted pre-master secret
	TCPPutArray(hTCP, (BYTE*) sslBuffer.full + len, len);
	
	// Free the RSA Engine
	RSAEndUsage();
	sslRSAStubID = SSL_INVALID_ID;

	// Hash what we just sent
	SSLHashSync(sslStub.idSHA1);
	HashAddData(&sslHash, sslBuffer.full + len, len);
	SSLHashSync(sslStub.idMD5);
	HashAddData(&sslHash, sslBuffer.full + len, len);
	
	// Generate the Master Secret
	SSLKeysSync(sslStubID);
	SSLSessionSync(sslStub.idSession);
	GenerateHashRounds(3, sslKeys.Local.random, sslKeys.Remote.random);
	memcpy(sslSession.masterSecret, (void*)sslBuffer.hashRounds.temp, 48);
	SSLSessionUpdated();
	
	// Free the buffer with the encrypted pre-master secret
	SSLBufferFree(&sslStub.idRxBuffer);
	
	// Restart the handshaker
	HSStart();
	
	// Send the record
	SSLTxRecord(hTCP, sslStubID, SSL_HANDSHAKE);
	
	// Request a Change Cipher Spec and Finished message
	TCPRequestSSLMessage(hTCP, SSL_NO_MESSAGE);
	TCPRequestSSLMessage(hTCP, SSL_CHANGE_CIPHER_SPEC);
	
	// Note that this message was sent
	sslStub.Flags.bClientKeyExchange = 1;
}
#endif

/*********************************************************************
 * Function:        void SSLRxClientKeyExchange(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized and HSStart() has been
 *					called.
 *
 * Input:           hTCP - the TCP Socket to read from
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives the ClientKeyExchange message and begins
 *					the decryption process.
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_SERVER)
static void SSLRxClientKeyExchange(TCP_SOCKET hTCP)
{
	WORD wKeyLength;
	
	// Make sure entire message is ready
	if(TCPIsGetReady(hTCP) < sslStub.wRxHsBytesRem)
		return;
	
	// Verify handshake message sequence
	if(!sslStub.Flags.bServerHello || sslStub.Flags.bClientKeyExchange)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Obtain a buffer to use
	SSLBufferAlloc(&sslStub.idRxBuffer);
	if(sslStub.idRxBuffer == SSL_INVALID_ID)
		return;
	
	// Claim the RSA engine
	if(!RSABeginDecrypt())
		return;
	sslRSAStubID = sslStubID;
	
	// Read the data
	wKeyLength = sslStub.wRxHsBytesRem;
	HSEnd();
	HSStart();
	HSGetArray(hTCP, NULL, wKeyLength);
	HSEnd();
	RSASetData(sslBuffer.full, wKeyLength, RSA_BIG_ENDIAN);
	sslBufferID = sslStub.idRxBuffer;
	
	// Note that message was received
	sslStub.Flags.bClientKeyExchange = 1;
	
	// Kick off the RSA decryptor
	sslStub.Flags.bRSAInProgress = 1;
	sslSessionStubs[sslStub.idSession].lastUsed += SSL_RSA_LIFETIME_EXTENSION;
	
}
#endif

/*********************************************************************
 * Function:        void SSLTxCCSFin(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized, and the current session
 *					has a valid pre-master secret to use.
 *
 * Input:           hTCP - the TCP Socket to write the message to
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Generates the session keys from the master secret,
 *					then allocates and generates the encryption 
 *					context.  Once processing is complete, transmits
 *					the Change Cipher Spec message and the Finished
 *					handshake message to the server.
 *
 * Note:            None
 ********************************************************************/
static void SSLTxCCSFin(TCP_SOCKET hTCP)
{
	BYTE data[20];
	
	// Make sure enough space is available for both
	if(TCPIsPutReady(hTCP) < 68u)
		return;

	// Sync up the session
	SSLSessionSync(sslStub.idSession);
	SSLKeysSync(sslStubID);
	SSLBufferSync(SSL_INVALID_ID);
			
	// Send the CCS (not a handshake message)
	TCPPut(hTCP, 1);
	SSLTxRecord(hTCP, sslStubID, SSL_CHANGE_CIPHER_SPEC);
	sslStub.Flags.bLocalChangeCipherSpec = 1;
	
	// If keys are not ready, generate them
	if(!sslStub.Flags.bKeysValid)
	{
		// Obtain two full buffers for the Sboxes
		SSLBufferAlloc(&sslStub.idTxBuffer);
		SSLBufferAlloc(&sslStub.idRxBuffer);
		if(sslStub.idTxBuffer == SSL_INVALID_ID || sslStub.idRxBuffer == SSL_INVALID_ID)
			return;

		// Generate the keys
		SSLHashSync(SSL_INVALID_ID);
		GenerateSessionKeys();
		sslStub.Flags.bKeysValid = 1;
	}
	
	// Reset the sequence counters
	sslKeys.Local.app.sequence = 0;	
	
	// Start the handshake data processor
	HSStart();

	// First, write the handshake header
	HSPut(hTCP, SSL_FINISHED);
	HSPut(hTCP, 0x00);
	HSPut(hTCP, 0x00);
	HSPut(hTCP, 0x24);

	// Calculate the Finished hashes
	CalculateFinishedHash(sslStub.idMD5, !sslStub.Flags.bIsServer, data);
	HSPutArray(hTCP, data, 16);
	CalculateFinishedHash(sslStub.idSHA1, !sslStub.Flags.bIsServer, data);
	HSPutArray(hTCP, data, 20);	

	// Hash this message to the handshake hash
	HSEnd();

	// Send the record
	SSLTxRecord(hTCP, sslStubID, SSL_HANDSHAKE);
	
	// Update the connection state
	TCPRequestSSLMessage(hTCP, SSL_NO_MESSAGE);
	sslStub.Flags.bLocalFinished = 1;
	
	// If complete, note that
	if(sslStub.Flags.bRemoteFinished)
	{
		TCPSSLHandshakeComplete(hTCP);
		SSLHashFree(&sslStub.idMD5);
		SSLHashFree(&sslStub.idSHA1);
	}

}

/*********************************************************************
 * Function:        void SSLRxCCS(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized.
 *
 * Input:           hTCP - the TCP Socket to read from
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives a ChangeCipherSpec from the remote server
 *
 * Note:            None
 ********************************************************************/
static void SSLRxCCS(TCP_SOCKET hTCP)
{
	// Only proceed if RSA is done
	if(sslStub.Flags.bRSAInProgress)
		return;
	
	// Verify handshake message sequence
	if(!sslStub.Flags.bClientHello || !sslStub.Flags.bServerHello)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Allocate a hash for MACing data
	SSLHashAlloc(&sslStub.idRxHash);

	// Make sure entire message is ready and an RX hash is allocated
	if(TCPIsGetReady(hTCP) < sslStub.wRxBytesRem 
		|| sslStub.idRxHash == SSL_INVALID_ID)
		return;
	
	// If keys are not ready, generate them
	if(!sslStub.Flags.bKeysValid)
	{
		// Sync up the session
		SSLSessionSync(sslStub.idSession);
		SSLKeysSync(sslStubID);
		SSLBufferSync(SSL_INVALID_ID);

		// Obtain two full buffers for the Sboxes
		SSLBufferAlloc(&sslStub.idTxBuffer);
		SSLBufferAlloc(&sslStub.idRxBuffer);
		if(sslStub.idTxBuffer == SSL_INVALID_ID || sslStub.idRxBuffer == SSL_INVALID_ID)
			return;

		// Generate the keys
		SSLHashSync(SSL_INVALID_ID);
		GenerateSessionKeys();
		sslStub.Flags.bKeysValid = 1;
	}
	
	// Read the CCS message (ignoring its contents)
	sslStub.wRxBytesRem -= TCPGetArray(hTCP, NULL, sslStub.wRxBytesRem);
	
	// Note that message was received
	SSLKeysSync(sslStubID);
	sslKeys.Remote.app.sequence = 0;
	sslStub.Flags.bRemoteChangeCipherSpec = 1;
}

/*********************************************************************
 * Function:        void SSLRxFinished(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized and HSStart() has been
 *					called.
 *
 * Input:           hTCP - the TCP Socket to read from
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives the Finished message from remote node
 *
 * Note:            None
 ********************************************************************/
static void SSLRxFinished(TCP_SOCKET hTCP)
{
	BYTE rxHash[20], expectedHash[20];
	
	// Make sure entire message is ready
	if(TCPIsGetReady(hTCP) < sslStub.wRxHsBytesRem)
		return;
	
	// Verify handshake message sequence
	if(!sslStub.Flags.bRemoteChangeCipherSpec)
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	
	// Make sure correct session and key set are loaded
	SSLSessionSync(sslStub.idSession);
	SSLKeysSync(sslStubID);
	
	// Read md5_sum to temporary location
	HSGetArray(hTCP, rxHash, 16);
	
	// Calculate expected MD5 hash
	CalculateFinishedHash(sslStub.idMD5, sslStub.Flags.bIsServer, expectedHash);	
	if(memcmp((void*)rxHash, (void*)expectedHash, 16) != 0)
	{// Handshake hash fails
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	}
	
	// Read sha_sum to temporary location
	HSGetArray(hTCP, rxHash, 20);
	
	// Calculate expected SHA-1 hash	
	CalculateFinishedHash(sslStub.idSHA1, sslStub.Flags.bIsServer, expectedHash);
	if(memcmp((void*)rxHash, (void*)expectedHash, 20) != 0)
	{// Handshake hash fails
		TCPRequestSSLMessage(hTCP, SSL_ALERT_HANDSHAKE_FAILURE);
	}
	
	// Note that message was received
	sslStub.Flags.bRemoteFinished = 1;
	
	// If complete, note that, otherwise, request our own CCS message
	if(sslStub.Flags.bLocalFinished)
	{
		TCPSSLHandshakeComplete(hTCP);
		SSLHashFree(&sslStub.idMD5);
		SSLHashFree(&sslStub.idSHA1);
	}
	else
		TCPRequestSSLMessage(hTCP, SSL_CHANGE_CIPHER_SPEC);

}

/*********************************************************************
 * Function:        void SSLRxAlert(TCP_SOCKET hTCP)
 *
 * PreCondition:    sslStub is synchronized
 *
 * Input:           hTCP - the TCP Socket to read from
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Receives an alert message and decides what to do
 *
 * Note:            None
 ********************************************************************/
static void SSLRxAlert(TCP_SOCKET hTCP)
{
	BYTE bLevel, bDesc;
	
	// Make sure entire message is ready
	if(TCPIsGetReady(hTCP) < sslStub.wRxBytesRem)
		return;
	
	// Read the alert message
	TCPGet(hTCP, &bLevel);
	TCPGet(hTCP, &bDesc);
	sslStub.wRxBytesRem -= 2;
	
	// Determine what to do
	switch(bLevel)
	{
		case SSL_ALERT_WARNING:
			// Check if a close notify was received
			if(bDesc + 0x80 == SSL_ALERT_CLOSE_NOTIFY)
				sslStub.Flags.bCloseNotify = 1;
			
			// We don't support non-fatal warnings besides CloseNotify,
			// so the connection is always done now.  When the TCP 
			// session closes, the resources will be cleaned up.
			
			// No break here:
			// Session is terminated, so still mark Done below
			
		case SSL_ALERT_FATAL:
			// Mark session as terminated
			sslStub.Flags.bDone = 1;			
	}
	
}

/****************************************************************************
  ===========================================================================
  Section:
	SSL Key Processing Functions
  ===========================================================================
  ***************************************************************************/

/*********************************************************************
 * Function:        RSA_STATUS SSLRSAOperation(void)
 *
 * PreCondition:    The RSA Module has been secured, an RSA operation
 *					is pending, sslStub.wRxHsBytesRem is the value of
 *					sslStub.wRxBytesRem after completion, and 
 *					sslStub.wRxBytesRem is the value of 
 *					sslStub.rxProtocol after completion.  Also requires
 *					sslStub to be synchronized.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Pauses connection processing until RSA calculation
 *					is complete.
 *
 * Note:            This function exists outside of the handshaking 
 *					functions so that the system does not incur the
 *					expense of resuming and suspending handshake 
 *					hashes.
 ********************************************************************/
RSA_STATUS SSLRSAOperation(void)
{
	SSLBufferSync(sslStub.idRxBuffer);
		
	// Call RSAStep to perform some RSA processing
	return RSAStep();
}

/*********************************************************************
 * Function:        void GenerateHashRounds(BYTE num, BYTE* rand1,
 *												BYTE* rand2)
 *
 * PreCondition:    The SSL buffer is allocated for temporary usage
 *					and the data to run rounds on is in
 *					sslSession.masterSecret
 *
 * Input:           num   - how many rounds to compute
 *					rand1 - the first random data block to use
 *					rand2 - the second random data block to use
 *
 * Output:          None
 *
 * Side Effects:    Destroys the SSL Buffer space
 *
 * Overview:        Generates hash rounds to find either the
 *					Master Secret or the Key Block.
 *
 * Note:            This function will overflow the buffer after 7
 *					rounds, but in practice num = 3 or num = 4.
 ********************************************************************/
void GenerateHashRounds(BYTE num, BYTE* rand1, BYTE* rand2)
{
	BYTE i, j, c, *res;
	
	c = 'A';
	res = sslBuffer.hashRounds.temp;
	
	for(i = 1; i <= num; i++, c++, res += 16)
	{
		SHA1Initialize(&sslBuffer.hashRounds.hash);
		for(j = 0; j < i; j++)
			HashAddData(&sslBuffer.hashRounds.hash, &c, 1);
		HashAddData(&sslBuffer.hashRounds.hash, sslSession.masterSecret, 48);
		HashAddData(&sslBuffer.hashRounds.hash, rand1, 32);
		HashAddData(&sslBuffer.hashRounds.hash, rand2, 32);
		SHA1Calculate(&sslBuffer.hashRounds.hash, sslBuffer.hashRounds.sha_hash);
		MD5Initialize(&sslBuffer.hashRounds.hash);
		HashAddData(&sslBuffer.hashRounds.hash, sslSession.masterSecret, 48);
		HashAddData(&sslBuffer.hashRounds.hash, sslBuffer.hashRounds.sha_hash, 20);
		MD5Calculate(&sslBuffer.hashRounds.hash, res);
	}	
}

/*********************************************************************
 * Function:        void GenerateSessionKeys(void)
 *
 * PreCondition:    The SSL buffer is allocated for temporary usage,
 *					session keys are synced, and the TX and RX buffers
 *					are allocated for S-boxes.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Destroys the SSL Buffer Space
 *
 * Overview:        Generates the session write keys and MAC secrets
 *
 * Note:            None
 ********************************************************************/
void GenerateSessionKeys(void)
{
	// This functionality differs slightly for client and server operations

	#if defined(STACK_USE_SSL_SERVER)
	if(sslStub.Flags.bIsServer)
	{
		// Generate the key expansion block
		GenerateHashRounds(4, sslKeys.Local.random, sslKeys.Remote.random);
		memcpy(sslKeys.Remote.app.MACSecret, (void*)sslBuffer.hashRounds.temp, 16);
		memcpy(sslKeys.Local.app.MACSecret, (void*)sslBuffer.hashRounds.temp+16, 16);
	
		// Save write keys elsewhere temporarily
		SSLHashSync(SSL_INVALID_ID);
		memcpy(&sslHash, (void*)sslBuffer.hashRounds.temp+32, 32);
	
		// Generate ARCFOUR Sboxes
		SSLBufferSync(sslStub.idRxBuffer);
		sslKeys.Remote.app.cryptCtx.Sbox = sslBuffer.full;
		ARCFOURInitialize(&(sslKeys.Remote.app.cryptCtx), (BYTE*)(&sslHash), 16);
		SSLBufferSync(sslStub.idTxBuffer);
		sslKeys.Local.app.cryptCtx.Sbox = sslBuffer.full;
		ARCFOURInitialize(&(sslKeys.Local.app.cryptCtx), (BYTE*)(&sslHash)+16, 16);
		
		return;
	}
	#endif
	
	#if defined(STACK_USE_SSL_CLIENT)
	// Generate the key expansion block
	GenerateHashRounds(4, sslKeys.Remote.random, sslKeys.Local.random);
	memcpy(sslKeys.Local.app.MACSecret, (void*)sslBuffer.hashRounds.temp, 16);
	memcpy(sslKeys.Remote.app.MACSecret, (void*)sslBuffer.hashRounds.temp+16, 16);

	// Save write keys elsewhere temporarily
	SSLHashSync(SSL_INVALID_ID);
	memcpy(&sslHash, (void*)sslBuffer.hashRounds.temp+32, 32);

	// Generate ARCFOUR Sboxes
	SSLBufferSync(sslStub.idTxBuffer);
	sslKeys.Local.app.cryptCtx.Sbox = sslBuffer.full;
	ARCFOURInitialize(&(sslKeys.Local.app.cryptCtx), (BYTE*)(&sslHash), 16);
	SSLBufferSync(sslStub.idRxBuffer);
	sslKeys.Remote.app.cryptCtx.Sbox = sslBuffer.full;
	ARCFOURInitialize(&(sslKeys.Remote.app.cryptCtx), (BYTE*)(&sslHash)+16, 16);
	#endif
	
}

/*********************************************************************
 * Function:        static void CalculateFinishedHash(BYTE hashID,
 *									BOOL fromClient, BYTE *result)
 *
 * PreCondition:    hashID has all handshake data hashed so far and
 *					the current session is synced in.
 *
 * Input:           hashID     - the hash sum to use
 *					fromClient - TRUE if client is sender
 *					result     - where to store results
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Calculates the handshake hash over the data.
 *					hashID can be either MD5 or SHA-1, and this
 *					function will calculate accordingly.
 *
 * Note:            None
 ********************************************************************/
static void CalculateFinishedHash(BYTE hashID, BOOL fromClient, BYTE *result)
{
	BYTE i;

	// Load the hash, but make sure updates aren't saved
	SSLHashSync(hashID);
	sslHashID = SSL_INVALID_ID;
	
	// Sync the session data so masterSecret is available
	SSLSessionSync(sslStub.idSession);
	
	// Hash in the sender phrase & master secret
	if(fromClient)
		HashAddROMData(&sslHash, (ROM BYTE*)"CLNT", 4);
	else
		HashAddROMData(&sslHash, (ROM BYTE*)"SRVR", 4);
	HashAddData(&sslHash, sslSession.masterSecret, 48);
	
	// Hash in the pad1
	i = 6;
	if(sslHash.hashType == HASH_SHA1)
		i--;
	for(; i > 0u; i--)
		HashAddROMData(&sslHash, (ROM BYTE*)"\x36\x36\x36\x36\x36\x36\x36\x36", 8);
	
	// Calculate the inner hash result and store, start new hash
	if(sslHash.hashType == HASH_MD5)
	{
		MD5Calculate(&sslHash, result);
		MD5Initialize(&sslHash);
	}
	else
	{
		SHA1Calculate(&sslHash, result);
		SHA1Initialize(&sslHash);
	}
	
	// Hash in master secret
	HashAddData(&sslHash, sslSession.masterSecret, 48);
	
	// Hash in pad2
	i = 6;
	if(sslHash.hashType == HASH_SHA1)
		i--;
	for(; i > 0u; i--)
		HashAddROMData(&sslHash, (ROM BYTE*)"\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c", 8);
	
	// Hash in the inner hash result and calculate
	if(sslHash.hashType == HASH_MD5)
	{
		HashAddData(&sslHash, result, 16);
		MD5Calculate(&sslHash, result);
	}
	else
	{
		HashAddData(&sslHash, result, 20);
		SHA1Calculate(&sslHash, result);
	}
	
}


/****************************************************************************
  ===========================================================================
  Section:
	SSL Memory Management Functions
  ===========================================================================
  ***************************************************************************/

#define Debug(a,b)	//do{putrsUART("\r\n" a); while(BusyUART()); WriteUART(b+'0');} while(0)


/*********************************************************************
 * Function:        static void SSLStubAlloc(void)
 *
 * PreCondition:    None
 *
 * Inputs:          None
 *
 * Outputs:         None
 *
 * Returns:			TRUE if stub was allocated, FALSE otherwise
 *
 * Side Effects:    None
 *
 * Overview:        Allocates a stub for use.
 *
 * Note:            None
 ********************************************************************/
static BOOL SSLStubAlloc(void)
{
	BYTE i;
	
	// Search for a free stub
	for(i = 0; i != MAX_SSL_CONNECTIONS; i++)
	{
		if(!(isStubUsed & masks[i]))
		{// Stub is free, so claim it
			isStubUsed |= masks[i];
			
			// Save stub currently in RAM
			if(sslStubID != SSL_INVALID_ID)
				SaveOffChip((BYTE*)&sslStub,
					SSL_BASE_STUB_ADDR+SSL_STUB_SIZE*sslStubID,
					SSL_STUB_SIZE);

			// Switch to new stub and return
			Debug("S",i);
			sslStubID = i;
			return TRUE;
		}
	}
	
	// No stub was found to be free
	return FALSE;
	
}

/*********************************************************************
 * Function:        static void SSLStubFree(BYTE id)
 *
 * PreCondition:    None
 *
 * Inputs:          id - the stub ID to free
 *
 * Outputs:         None
 *
 * Returns:			None
 *
 * Side Effects:    None
 *
 * Overview:        Specified stub is released
 *
 * Note:            None
 ********************************************************************/
static void SSLStubFree(BYTE id)
{
	// If ID is not valid
	if(id >= MAX_SSL_CONNECTIONS)
		return;
	
	// If currently in RAM, mark as unused
	if(sslStubID == id)
		sslStubID = SSL_INVALID_ID;

	// Release the stub
	Debug("s",id);
	isStubUsed &= ~masks[id];
}

/*********************************************************************
 * Function:        static void SSLStubSync(BYTE id)
 *
 * PreCondition:    None
 *
 * Input:           id - the stub ID to sync to RAM
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Specified stub is loaded to RAM.  Only loads if
 *					necessary, and saves any current stub before
 *					switching.
 *
 * Note:            None
 ********************************************************************/
static void SSLStubSync(BYTE id)
{
	// Check if already loaded
	if(sslStubID == id)
		return;
	
	// Save old stub
	if(sslStubID != SSL_INVALID_ID)
		SaveOffChip((BYTE*)&sslStub,
			SSL_BASE_STUB_ADDR+SSL_STUB_SIZE*sslStubID,
			SSL_STUB_SIZE);
	
	// Load new stub
	LoadOffChip((BYTE*)&sslStub,
		SSL_BASE_STUB_ADDR+SSL_STUB_SIZE*id,
		SSL_STUB_SIZE);
	sslStubID = id;
}

/*********************************************************************
 * Function:        static void SSLKeysSync(BYTE id)
 *
 * PreCondition:    None
 *
 * Input:           id - the key set ID to sync to RAM
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Specified key set is loaded to RAM.  Only loads if
 *					necessary, and saves any current key set before
 *					switching.
 *
 * Note:            None
 ********************************************************************/
static void SSLKeysSync(BYTE id)
{
	// Check if already loaded
	if(sslKeysID == id)
		return;
	
	// Save old stub
	if(sslKeysID != SSL_INVALID_ID)
		SaveOffChip((BYTE*)&sslKeys,
			SSL_BASE_KEYS_ADDR+SSL_KEYS_SIZE*sslKeysID,
			SSL_KEYS_SIZE);
	
	// Load new stub
	LoadOffChip((BYTE*)&sslKeys,
		SSL_BASE_KEYS_ADDR+SSL_KEYS_SIZE*id,
		SSL_KEYS_SIZE);
	sslKeysID = id;
}

/*********************************************************************
 * Function:        static void SSLHashAlloc(BYTE *id)
 *
 * PreCondition:    None
 *
 * Inputs:          id - Where to store the allocated ID
 *
 * Outputs:         id - Allocated hash ID, or SSL_INVALID_ID if 
 *							none available
 *
 * Side Effects:    None
 *
 * Overview:        Allocates a hash for use.
 *
 * Note:            None
 ********************************************************************/
static void SSLHashAlloc(BYTE *id)
{
	BYTE i;
	
	// If already allocated, just load it up
	if(*id != SSL_INVALID_ID)
	{
		SSLHashSync(*id);
		return;
	}
		
	// Search for a free hash
	for(i = 0; i != MAX_SSL_HASHES; i++)
	{
		if(!(isHashUsed & masks[i]))
		{// Hash is free, so claim it
			isHashUsed |= masks[i];
			
			// Save hash currently in RAM
			if(sslHashID != SSL_INVALID_ID)
				SaveOffChip((BYTE*)&sslHash,
					SSL_BASE_HASH_ADDR+SSL_HASH_SIZE*sslHashID,
					SSL_HASH_SIZE);

			// Switch to new hash and return
			Debug("H",i);
			sslHashID = i;
			*id = i;
			return;
		}
	}
}

/*********************************************************************
 * Function:        static void SSLHashFree(BYTE *id)
 *
 * PreCondition:    None
 *
 * Inputs:           id - the hash ID to free
 *
 * Outputs:          id - SSL_INVALID_ID
 *
 * Side Effects:    None
 *
 * Overview:        Specified hash is released
 *
 * Note:            None
 ********************************************************************/
static void SSLHashFree(BYTE *id)
{
	// Nothing to do for invalid hashes
	if(*id > MAX_SSL_HASHES)
		return;
	
	// Purge from RAM if not used
	if(sslHashID == *id)
		sslHashID = SSL_INVALID_ID;

	// Release the hash
	Debug("h",*id);
	isHashUsed &= ~masks[*id];
	*id = SSL_INVALID_ID;
}

/*********************************************************************
 * Function:        static void SSLHashSync(BYTE id)
 *
 * PreCondition:    None
 *
 * Input:           id - the hash ID to sync to RAM
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Specified hash is loaded to RAM.  Only loads if
 *					necessary, and saves any current hash before
 *					switching.
 *
 * Note:            None
 ********************************************************************/
static void SSLHashSync(BYTE id)
{
	// Check if already loaded
	if(sslHashID == id)
		return;
	
	// Save old hash
	if(sslHashID != SSL_INVALID_ID)
		SaveOffChip((BYTE*)&sslHash,
			SSL_BASE_HASH_ADDR+SSL_HASH_SIZE*sslHashID,
			SSL_HASH_SIZE);
	
	// Load new hash if not requesting a temporary hash
	if(id != SSL_INVALID_ID)
		LoadOffChip((BYTE*)&sslHash,
			SSL_BASE_HASH_ADDR+SSL_HASH_SIZE*id,
			SSL_HASH_SIZE);
	sslHashID = id;
}

/*********************************************************************
 * Function:        static void SSLBufferAlloc(BYTE *id)
 *
 * PreCondition:    None
 *
 * Input:           id - Where to store the allocated ID
 *
 * Output:          id - Allocated buffer ID, or SSL_INVALID_ID if 
 *							none available
 *
 * Side Effects:    None
 *
 * Overview:        Allocates a buffer for use.
 *
 * Note:            None
 ********************************************************************/
static void SSLBufferAlloc(BYTE *id)
{
	BYTE i;

	// If already allocated, just load it up
	if(*id != SSL_INVALID_ID)
	{
		SSLBufferSync(*id);
		return;
	}
	
	// Search for a free buffer
	for(i = 0; i != MAX_SSL_BUFFERS; i++)
	{
		if(!(isBufferUsed & masks[i]))
		{// Buffer is free, so claim it
			isBufferUsed |= masks[i];
			
			// Save buffer currently in RAM
			if(sslBufferID != SSL_INVALID_ID)
				SaveOffChip((BYTE*)&sslBuffer, 
					SSL_BASE_BUFFER_ADDR+SSL_BUFFER_SIZE*sslBufferID,
					SSL_BUFFER_SIZE);

			// Switch to new buffer and return
			Debug("B",i);
			sslBufferID = i;
			*id = i;
			return;
		}
	}
}

/*********************************************************************
 * Function:        static void SSLBufferFree(BYTE *id)
 *
 * PreCondition:    None
 *
 * Inputs:           id - the buffer ID to free
 *
 * Outputs:          id - SSL_INVALID_ID
 *
 * Side Effects:    None
 *
 * Overview:        Specified buffer is released
 *
 * Note:            None
 ********************************************************************/
static void SSLBufferFree(BYTE *id)
{
	// Nothing to do for invalid hashes
	if(*id > MAX_SSL_BUFFERS)
		return;

	// Purge from RAM if not used
	if(sslBufferID == *id)
		sslBufferID = SSL_INVALID_ID;\
		
	// Release the buffer
	Debug("b",*id);
	isBufferUsed &= ~masks[*id];
	*id = SSL_INVALID_ID;
}

/*********************************************************************
 * Function:        static void SSLBufferSync(BYTE id)
 *
 * PreCondition:    None
 *
 * Input:           id - the buffer ID to sync to RAM
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Specified buffer is loaded to RAM.  Only loads if
 *					necessary, and saves any current buffer before
 *					switching.
 *
 * Note:            None
 ********************************************************************/
static void SSLBufferSync(BYTE id)
{
	// Check if already loaded
	if(sslBufferID == id)
		return;
	
	// Save old buffer
	if(sslBufferID != SSL_INVALID_ID)
		SaveOffChip((BYTE*)&sslBuffer,
			SSL_BASE_BUFFER_ADDR+SSL_BUFFER_SIZE*sslBufferID,
			SSL_BUFFER_SIZE);
	
	// Load new buffer if not requesting temporary space
	if(id != SSL_INVALID_ID)
		LoadOffChip((BYTE*)&sslBuffer,
			SSL_BASE_BUFFER_ADDR+SSL_BUFFER_SIZE*id,
			SSL_BUFFER_SIZE);
	sslBufferID = id;
}

/*********************************************************************
 * Function:        static BYTE SSLSessionNew(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Allocated Session ID, or SSL_INVALID_ID if none available
 *
 * Side Effects:    None
 *
 * Overview:        Finds space for a new SSL session
 *
 * Note:            None
 ********************************************************************/
static BYTE SSLSessionNew(void)
{
	BYTE id, oldestID;
	DWORD now, age, oldest;
	
	// Set up the search
	oldestID = SSL_INVALID_ID;
	oldest = SSL_MIN_SESSION_LIFETIME;
	now = TickGet();
		
	// Search for a free session
	for(id = 0; id != MAX_SSL_SESSIONS; id++)
	{
		if(sslSessionStubs[id].tag.Val == 0u)
		{// Unused session, so claim immediately
			break;
		}
		
		// Check how old this session is
		age = now - sslSessionStubs[id].lastUsed;
		if(age > oldest)
		{// This is now the oldest one
			oldest = age;
			oldestID = id;
		}
	}
	
	// Check if we can claim a session
	if(id == MAX_SSL_SESSIONS && oldestID != SSL_INVALID_ID)
		id = oldestID;
	
	// If a valid ID was found, claim it
	if(id < MAX_SSL_SESSIONS)
	{
		// Save old one if needed
		if(sslSessionUpdated)
			SaveOffChip((BYTE*)&sslSession,
				SSL_BASE_SESSION_ADDR+SSL_SESSION_SIZE*sslSessionID,
				SSL_SESSION_SIZE);
		
		// Set up the new session
		sslSessionID = id;
		sslSessionStubs[id].lastUsed = now;
		SSLSessionUpdated();
		return id;
	}
	
	return SSL_INVALID_ID;
}

/*********************************************************************
 * Function:        static BYTE SSLSessionMatchID(BYTE* SessionID)
 *
 * PreCondition:    None
 *
 * Input:           SessionID - the session identifier to match
 *
 * Output:          The matched session ID, or SSL_INVALID_ID if not found
 *
 * Side Effects:    None
 *
 * Overview:        Locates a cached SSL session for reuse.  Syncs 
 *                  found session into RAM.
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_SERVER)
static BYTE SSLSessionMatchID(BYTE* SessionID)
{
	BYTE i;
	
	for(i = 0; i < MAX_SSL_SESSIONS; i++)
	{
		// Check if tag matches the ID
		if(sslSessionStubs[i].tag.v[0] == 0u &&
			!memcmp((void*)&sslSessionStubs[i].tag.v[1], (void*)SessionID, 3) )
		{
			// Found a partial match, so load it to memory
			SSLSessionSync(i);
			
			// Verify complete match
			if(memcmp((void*)sslSession.sessionID, (void*)SessionID, 32) != 0)
				continue;
			
			// Mark it as being used now
			sslSessionStubs[i].lastUsed = TickGet();
			
			// Return this session for use
			return i;
		}
	}
	
	return SSL_INVALID_ID;

}
#endif

/*********************************************************************
 * Function:        static BYTE SSLSessionMatchHost(IP_ADDR ip)
 *
 * PreCondition:    None
 *
 * Input:           ip - the host session to match
 *
 * Output:          The matched session ID, or SSL_INVALID_ID if not found
 *
 * Side Effects:    None
 *
 * Overview:        Locates a cached SSL session for reuse
 *
 * Note:            None
 ********************************************************************/
#if defined(STACK_USE_SSL_CLIENT)
static BYTE SSLSessionMatchIP(IP_ADDR ip)
{
	BYTE i;
	
	for(i = 0; i < MAX_SSL_SESSIONS; i++)
	{
		// Check if tag matches the IP
		if(!memcmp((void*)&sslSessionStubs[i].tag.v[0], (void*)&ip, 4))
		{
			// Found a match, so load it to memory
			SSLSessionSync(i);
			
			// Mark it as being used now
			sslSessionStubs[i].lastUsed = TickGet();
			
			// Return this session for use
			return i;
		}
	}
	
	// No match so return invalid
	return SSL_INVALID_ID;
}
#endif

/*********************************************************************
 * Function:        static void SSLSessionSync(BYTE id)
 *
 * PreCondition:    None
 *
 * Input:           id - the session ID to sync to RAM
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Specified session is loaded to RAM.  Only loads if
 *					necessary, and saves any current session before
 *					switching if it has been updated.
 *
 * Note:            None
 ********************************************************************/
static void SSLSessionSync(BYTE id)
{
	// Check if already loaded
	if(sslSessionID == id)
		return;
	
	// Save old buffer
	if(sslSessionUpdated && sslSessionID != SSL_INVALID_ID)
		SaveOffChip((BYTE*)&sslSession,
			SSL_BASE_SESSION_ADDR+SSL_SESSION_SIZE*sslSessionID,
			SSL_SESSION_SIZE);
	
	// Load new buffer
	LoadOffChip((BYTE*)&sslSession,
		SSL_BASE_SESSION_ADDR+SSL_SESSION_SIZE*id,
		SSL_SESSION_SIZE);
	sslSessionID = id;
	sslSessionUpdated = FALSE;
}

/*********************************************************************
 * Function:        static void SaveOffChip(BYTE *ramAddr,
 *											WORD ethAddr, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           ramAddr - source address in RAM
 *					ethAddr - destination address in Ethernet RAM
 *					len		- number of bytes to copy
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies data in PIC RAM to the Ethernet RAM
 *
 * Note:            None
 ********************************************************************/
static void SaveOffChip(BYTE *ramAddr, PTR_BASE ethAddr, WORD len)
{
	PTR_BASE oldPtr;
	
	oldPtr = MACSetWritePtr(ethAddr);
	MACPutArray(ramAddr, len);
	MACSetWritePtr(oldPtr);
}

/*********************************************************************
 * Function:        static void LoadOffChip(BYTE *ramAddr,
 *											WORD ethAddr, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           ramAddr - destination address in RAM
 *					ethAddr - source address in Ethernet RAM
 *					len		- number of bytes to copy
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies data from Ethernet RAM to local RAM
 *
 * Note:            None
 ********************************************************************/
static void LoadOffChip(BYTE *ramAddr, PTR_BASE ethAddr, WORD len)
{
    PTR_BASE oldPtr;
	
	oldPtr = MACSetReadPtr(ethAddr);
	MACGetArray(ramAddr, len);
	MACSetReadPtr(oldPtr);
}


/****************************************************************************
  ===========================================================================
  Section:
	SSL Handshake Hashing Functions
  ===========================================================================
  ***************************************************************************/

/*********************************************************************
 * Function:        static void HSStart()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Sets up the buffer to store data for handshake
 *					hash tracking
 *
 * Note:            None
 ********************************************************************/
static void HSStart()
{
	// Allocate temporary storage and set the pointer to it
	SSLBufferSync(SSL_INVALID_ID);
	ptrHS = sslBuffer.full;
}

/*********************************************************************
 * Function:        static void HSEnd()
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Hashes the message contents into the Handshake
 *					hash structures and begins a new handshake hash.
 *
 * Note:            None
 ********************************************************************/
static void HSEnd()
{
	// Hash in the received data and reset the pointer
	if(sslHashID == sslStub.idMD5)
	{
		HashAddData(&sslHash, sslBuffer.full, ptrHS - sslBuffer.full);
		SSLHashSync(sslStub.idSHA1);
		HashAddData(&sslHash, sslBuffer.full, ptrHS - sslBuffer.full);
	}
	else
	{
		SSLHashSync(sslStub.idSHA1);
		HashAddData(&sslHash, sslBuffer.full, ptrHS - sslBuffer.full);
		SSLHashSync(sslStub.idMD5);
		HashAddData(&sslHash, sslBuffer.full, ptrHS - sslBuffer.full);
	}
	ptrHS = sslBuffer.full;
}

/*********************************************************************
 * Function:        static WORD HSGet(TCP_SOCKET skt, BYTE *b)
 *
 * PreCondition:    None
 *
 * Input:           skt - socket to read data from
 *					b	- byte to read into
 *
 * Output:          Number of bytes read
 *
 * Side Effects:    None
 *
 * Overview:        Reads data from socket, transparently hashing it
 *					into the handshake hashes.
 *
 * Note:            None
 ********************************************************************/
static WORD HSGet(TCP_SOCKET skt, BYTE *b)
{
	BYTE c;
	
	// Must read to &c in case b is NULL (we still need to hash it)
	if(!TCPGet(skt, &c))
		return 0;
	
	*ptrHS = c;
	if(b)
		*b = c;
	ptrHS++;
	
	sslStub.wRxBytesRem--;
	sslStub.wRxHsBytesRem--;
	
	if(ptrHS > sslBuffer.full + 128)
	{
		HSEnd();
		HSStart();
	}

	return 1;
}

/*********************************************************************
 * Function:        static WORD HSGetWord(TCP_SOCKET skt, WORD *w)
 *
 * PreCondition:    None
 *
 * Input:           skt - socket to read data from
 *					w	- word to read into
 *
 * Output:          Number of bytes read
 *
 * Side Effects:    None
 *
 * Overview:        Reads data from socket, transparently hashing it
 *					into the handshake hashes.
 *
 * Note:            None
 ********************************************************************/
static WORD HSGetWord(TCP_SOCKET skt, WORD *w)
{
	if(w == NULL)
		return HSGet(skt, (BYTE*)w) + HSGet(skt, (BYTE*)w);
	else
		return HSGet(skt, (BYTE*)w+1) + HSGet(skt, (BYTE*)w);
}

/*********************************************************************
 * Function:        static WORD HSGetArray(TCP_SOCKET skt, 
 											 BYTE *data, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           skt  - socket to read data from
 *					data - array to read into, or NULL
 *					len  - number of bytes to read
 *
 * Output:          Number of bytes read
 *
 * Side Effects:    None
 *
 * Overview:        Reads data from socket, transparently hashing it
 *					into the handshake hashes.
 *
 * Note:            None
 ********************************************************************/
static WORD HSGetArray(TCP_SOCKET skt, BYTE *data, WORD len)
{	
	WORD w;

/*	
	WORD wLenActual;
	WORD wChunkLen;

	wLenActual = 0;
	// Add all of this data to the running hash
	while(len)
	{
		wChunkLen = sizeof(sslBuffer.full) - (ptrHS - sslBuffer.full);
		if(wChunkLen == 0u)
		{
			HSEnd();
			wChunkLen = sizeof(sslBuffer.full);
		}	
		if(len < wChunkLen)
			wChunkLen = len;
		
		// Obtain data from TCP
		w = TCPGetArray(skt, ptrHS, wChunkLen);
		if(w == 0u)
			return wLenActual;
		
		// Copy data from hash area to *data area if a non-NULL pointer was 
		// provided
		if(data)
		{
			memcpy((void*)data, ptrHS, w);
			data += w;
		}
		
		// Update all tracking variables
		ptrHS += w;
		len -= w;
		wLenActual += w;
		sslStub.wRxBytesRem -= w;
		sslStub.wRxHsBytesRem -= w;
	}

	return wLenActual;
*/

	//if reading to NULL, we still have to hash
	if(!data)
	{
		WORD i, rem;
		for(i = 0; i < len; )
		{
			// Determine how much we can read
			rem = (sslBuffer.full + 256) - ptrHS;
			if(rem > len - i)
				rem = len - i;

			// Read that much
			rem = TCPGetArray(skt, ptrHS, rem);
			sslStub.wRxBytesRem -= rem;
			sslStub.wRxHsBytesRem -= rem;
						
			// Hash what we've got
			ptrHS += rem;
			HSEnd();
			HSStart();
			
			i += rem;
			
			// Make sure we aren't in an infinite loop
			if(rem == 0u)
				break;
		}
		
		return i;
	}
	
	len = TCPGetArray(skt, data, len);
	w = len;

	memcpy(ptrHS, (void*)data, len);
	ptrHS += len;

	
//	// Add all of this data to the running hash
//	while(w)
//	{
//		WORD wChunkLen = sizeof(sslBuffer.full) - (ptrHS - sslBuffer.full);
//		if(wChunkLen == 0u)
//		{
//			HSEnd();
//			HSStart();
//			wChunkLen = sizeof(sslBuffer.full);
//		}	
//		if(w < wChunkLen)
//			wChunkLen = w;
//		memcpy(ptrHS, (void*)data, wChunkLen);
//		ptrHS += wChunkLen;
//		w -= wChunkLen;
//	}
	
	sslStub.wRxBytesRem -= len;
	sslStub.wRxHsBytesRem -= len;
	
	if(ptrHS > sslBuffer.full + (sizeof(sslBuffer.full)/2))
	{
		HSEnd();
		HSStart();
	}
	
	return len;
}

/*********************************************************************
 * Function:        static WORD HSPut(TCP_SOCKET skt, BYTE b)
 *
 * PreCondition:    None
 *
 * Input:           skt  - socket to write data to
 *					b    - byte to write
 *
 * Output:          Number of bytes written
 *
 * Side Effects:    None
 *
 * Overview:        Writes data to socket, transparently hashing it
 *					into the handshake hashes.
 *
 * Note:            None
 ********************************************************************/
static WORD HSPut(TCP_SOCKET skt, BYTE b)
{
	if(!TCPPut(skt, b))
		return 0;

	// Ensure we don't overflow the Hash buffer	
	if(sizeof(sslBuffer.full) - (ptrHS - sslBuffer.full) == 0u)
		HSEnd();
	
	// Add this byte of data to the running hash
	*ptrHS = b;
	ptrHS++;

	return 1;
}

/*********************************************************************
 * Function:        static WORD HSPutWord(TCP_SOCKET skt, WORD w)
 *
 * PreCondition:    None
 *
 * Input:           skt  - socket to write data to
 *					w    - word to write
 *
 * Output:          Number of bytes written
 *
 * Side Effects:    None
 *
 * Overview:        Writes data to socket, transparently hashing it
 *					into the handshake hashes.
 *
 * Note:            None
 ********************************************************************/
static WORD HSPutWord(TCP_SOCKET skt, WORD w)
{
	return HSPut(skt, (BYTE)(w>>8)) + HSPut(skt, (BYTE)w);
}

/*********************************************************************
 * Function:        static WORD HSPutArray(TCP_SOCKET skt,
 											BYTE *data, BYTE len)
 *
 * PreCondition:    None
 *
 * Input:           skt  - socket to write data to
 *					data - data to write
 *					len  - number of bytes to write
 *
 * Output:          Number of bytes written
 *
 * Side Effects:    None
 *
 * Overview:        Writes data to socket, transparently hashing it
 *					into the handshake hashes.
 *
 * Note:            None
 ********************************************************************/
static WORD HSPutArray(TCP_SOCKET skt, BYTE *data, WORD len)
{	
	WORD w;
	
	len = TCPPutArray(skt, data, len);
	w = len;

	// Add all of this data to the running hash
	while(w)
	{
		WORD wChunkLen = sizeof(sslBuffer.full) - (ptrHS - sslBuffer.full);
		if(wChunkLen == 0u)
		{
			HSEnd();
			wChunkLen = sizeof(sslBuffer.full);
		}	
		if(w < wChunkLen)
			wChunkLen = w;
		memcpy(ptrHS, (void*)data, wChunkLen);
		data += wChunkLen;
		ptrHS += wChunkLen;
		w -= wChunkLen;
	}
	
	return len;
}

#if defined(STACK_USE_SSL_SERVER)
static WORD HSPutROMArray(TCP_SOCKET skt, ROM BYTE *data, WORD len)
{	
	WORD w;
	
	len = TCPPutROMArray(skt, data, len);
	w = len;

	// Add all of this data to the running hash
	while(w)
	{
		WORD wChunkLen = sizeof(sslBuffer.full) - (ptrHS - sslBuffer.full);
		if(wChunkLen == 0u)
		{
			HSEnd();
			wChunkLen = sizeof(sslBuffer.full);
		}	
		if(w < wChunkLen)
			wChunkLen = w;
		memcpypgm2ram(ptrHS, (ROM void*)data, wChunkLen);
		data += wChunkLen;
		ptrHS += wChunkLen;
		w -= wChunkLen;
	}

	return len;
}
#endif

/****************************************************************************
  ===========================================================================
  Section:
	SSL MAC Hashing Functions
  ===========================================================================
  ***************************************************************************/

/*********************************************************************
 * Function:        static void SSLMACBegin(BYTE *MACSecret, DWORD seq, 
 *											BYTE protocol, WORD len)
 *
 * PreCondition:    sslHash is ready to be written
 *					(any pending data saved, nothing useful stored)
 *
 * Input:           MACSecret - the MAC write secret
 *					seq       - the sequence number for this message
 *					protocol  - the SSL_PROTOCOL for this message
 *					len       - the length of the message being MAC'd
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Begins a MAC calculation in sslHash
 *
 * Note:            None
 ********************************************************************/
void SSLMACBegin(BYTE *MACSecret, DWORD seq, BYTE protocol, WORD len)
{
	BYTE i, temp[7];

	MD5Initialize(&sslHash);

	// Form the temp array
	temp[0] = *((BYTE*)&seq+3);
	temp[1] = *((BYTE*)&seq+2);
	temp[2] = *((BYTE*)&seq+1);
	temp[3] = *((BYTE*)&seq+0);
	temp[4] = protocol;
	temp[5] = *((BYTE*)&len+1);
	temp[6] = *((BYTE*)&len+0);
		
	// Hash the initial data (secret, padding, seq, protcol, len)
	HashAddData(&sslHash, MACSecret, 16);
	
	// Add in the padding
	for(i = 0; i < 6u; i++)
	{
		HashAddROMData(&sslHash, (ROM BYTE*)"\x36\x36\x36\x36\x36\x36\x36\x36", 8);
	}

	// Hash in the data length
	HashAddROMData(&sslHash, (ROM BYTE*)"\0\0\0\0", 4);
	HashAddData(&sslHash, temp, 7);	
}

/*********************************************************************
 * Function:        void SSLMACAdd(BYTE *data, WORD len)
 *
 * PreCondition:    sslHash is ready to be written
 *					(any pending data saved, SSLMACBegin called)
 *
 * Input:           data - the data to add to the MAC
 *					len  - the length of data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Adds more data to a MAC in progress
 *
 * Note:            None
 ********************************************************************/
void SSLMACAdd(BYTE *data, WORD len)
{
	HashAddData(&sslHash, data, len);
}

/*********************************************************************
 * Function:        static void SSLMACCalc(BYTE *result)
 *
 * PreCondition:    sslHash is ready to be written
 *					(any pending data saved, SSLMACBegin called)
 *
 * Input:           MACSecret - the MAC write secret
 *					result    - a 16 byte buffer to store result
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Finishes the MAC calculation, and places the
 *					result in the *result array
 *
 * Note:            None
 ********************************************************************/
void SSLMACCalc(BYTE *MACSecret, BYTE *result)
{
	BYTE i;
	
	// Get inner hash result
	MD5Calculate(&sslHash, result);
	
	// Begin outer hash
	MD5Initialize(&sslHash);
	HashAddData(&sslHash, MACSecret, 16);
	
	// Add in padding
	for(i = 0; i < 6u; i++)
	{
		HashAddROMData(&sslHash, (ROM BYTE*)"\x5c\x5c\x5c\x5c\x5c\x5c\x5c\x5c", 8);
	}
	
	// Hash in the previous result and calculate
	HashAddData(&sslHash, result, 16);
	MD5Calculate(&sslHash, result);	
}

#endif
