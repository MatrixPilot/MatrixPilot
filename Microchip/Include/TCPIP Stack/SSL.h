/*********************************************************************
 *
 *                  SSLv3 Module Headers
 *
 *********************************************************************
 * FileName:        SSL.h
 * Dependencies:    None
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
 *
 * Author               Date     Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Elliott Wood			6/20/07	 Original
 * Elliott Wood			12/17/07 Rewritten to integrate with TCP
 *								  and support both client & server
 ********************************************************************/

#ifndef __SSL_H
#define __SSL_H

#include "TCPIP Stack/SSLClientSize.h"

/****************************************************************************
  Section:
	Configuration Settings
  ***************************************************************************/
	#define SSL_VERSION				(0x0300u)	// SSL version number
	#define SSL_VERSION_HI			(0x03u)		// SSL version number (high byte)
	#define SSL_VERSION_LO			(0x00u)		// SSL version number (low byte)
	
	#define SSL_INVALID_ID			(0xFFu)		// Identifier for invalid SSL allocations
	
	// Minimum lifetime for SSL Sessions
	// Sessions cannot be reallocated until this much time has elapsed
	#define SSL_MIN_SESSION_LIFETIME	(1*TICK_SECOND)
	
	// Lifetime extension for RSA operations
	// Sessions lifetime is extended by this amount when an RSA calculation is made
	#define SSL_RSA_LIFETIME_EXTENSION	(8*TICK_SECOND)
	
	
/****************************************************************************
  Section:
	SSL Commands and Responses
  ***************************************************************************/

	#define SSL_CHANGE_CIPHER_SPEC	20u		// Protocol code for Change Cipher Spec records
	#define SSL_ALERT				21u		// Protocol code for Alert records
	#define SSL_HANDSHAKE			22u		// Protocol code for Handshake records
	#define SSL_APPLICATION			23u		// Protocol code for Application data records
	
	// Describes the types of SSL messages (handshake and alerts)
	typedef enum
	{
		SSL_HELLO_REQUEST				= 0u,	// HelloRequest handshake message (not currently supported)
		SSL_CLIENT_HELLO				= 1u,	// ClientHello handshake message
		SSL_ANTIQUE_CLIENT_HELLO		= 18u,	// SSLv2 ClientHello handshake message (Supported for backwards compatibility.  This is an internally defined value.)
		SSL_SERVER_HELLO				= 2u,	// ServerHello handshake message
		SSL_CERTIFICATE					= 11u,	// ServerCertifiate handshake message
		SSL_SERVER_HELLO_DONE			= 14u,	// ServerHelloDone handshake message
		SSL_CLIENT_KEY_EXCHANGE			= 16u,	// ClientKeyExchange handshake message
		SSL_FINISHED					= 20u,	// Finished handshake message
		
		// Alert Messages
		SSL_ALERT_CLOSE_NOTIFY			= 0u  + 0x80,	// CloseNotify alert message (dummy value used internally)
		SSL_ALERT_UNEXPECTED_MESSAGE	= 10u + 0x80,	// UnexpectedMessage alert message (dummy value used internally)
		SSL_ALERT_BAD_RECORD_MAC		= 20u + 0x80,	// BadRecordMAC alert message (dummy value used internally)
		SSL_ALERT_HANDSHAKE_FAILURE		= 40u + 0x80,	// HandshakeFailure alert message (dummy value used internally)
		
		// No Message
		SSL_NO_MESSAGE					= 0xff			// No message is currently requested (internally used value)
		
	} SSL_MESSAGES;
	
	// Describes the two types of Alert records
	typedef enum
	{
		SSL_ALERT_WARNING 	= 1u,	// Alert message is a warning (session can be resumed)
		SSL_ALERT_FATAL		= 2u	// Alert message is fatal (session is non-resumable)
	} SSL_ALERT_LEVEL;

	// SSL Session Type Enumeration
	typedef enum
	{
		SSL_CLIENT,			// Local device is the SSL client
		SSL_SERVER			// Local device is the SSL host
	} SSL_SESSION_TYPE;	


/****************************************************************************
  Section:
	State Machines
  ***************************************************************************/
	
	// State machine for SSLRxServerHello
	typedef enum
	{
		RX_SERVER_CERT_START = 0u,
		RX_SERVER_CERT_FIND_KEY,
		RX_SERVER_CERT_FIND_N,
		RX_SERVER_CERT_READ_N,
		RX_SERVER_CERT_READ_E,
		RX_SERVER_CERT_CLEAR
	} SM_SSL_RX_SERVER_HELLO;


/****************************************************************************
  Section:
	Connection Struct Definitions
  ***************************************************************************/

	// Memory holder for general information associated with
	// an SSL connections.
	typedef struct
	{
		WORD wRxBytesRem;					// Bytes left to read in current record
		WORD wRxHsBytesRem;					// Bytes left to read in current Handshake submessage
		
		BYTE rxProtocol;					// Protocol for message being read
		BYTE rxHSType;						// Handshake message being received
		
		BYTE idSession;						// ID for associated session
		BYTE idMD5, idSHA1;					// ID for current hashes
		BYTE idRxHash;						// ID for MAC hash (TX needs no persistence)
		BYTE idRxBuffer, idTxBuffer;		// ID for current buffers (Sboxes)
		
		DWORD_VAL dwTemp;					// Used for state machine in RxCertificate
		
		struct
		{
			unsigned char bIsServer					: 1;	// We are the server
			unsigned char bClientHello				: 1;	// ClientHello has been sent/received
			unsigned char bServerHello				: 1;	// ServerHello has been sent/received
			unsigned char bServerCertificate		: 1;	// ServerCertificate has been sent/received
			unsigned char bServerHelloDone			: 1;	// ServerHelloDone has been sent/received
			unsigned char bClientKeyExchange		: 1;	// ClientKeyExchange has been sent/received
			unsigned char bRemoteChangeCipherSpec	: 1;	// Remote node has sent a ChangeCipherSpec message
			unsigned char bRemoteFinished			: 1;	// Remote node has sent a Finished message
			unsigned char bLocalChangeCipherSpec	: 1;	// We have sent a ChangeCipherSpec message
			unsigned char bLocalFinished			: 1;	// We have sent a Finished message
			unsigned char bExpectingMAC				: 1;	// We expect a MAC at end of message
			unsigned char bNewSession				: 1;	// TRUE if a new session, FALSE if resuming
			unsigned char bCloseNotify				: 1;	// Whether or not a CloseNotify has been sent/received
			unsigned char bDone						: 1;	// TRUE if the connection is closed
			unsigned char bRSAInProgress			: 1;	// TRUE when RSA op is in progress
			unsigned char bKeysValid				: 1;	// TRUE if the session keys have been generated
		} Flags;
		
		BYTE requestedMessage;				// Currently requested message to send, or 0xff
        void * supplementaryBuffer;
        BYTE supplementaryDataType;
	} SSL_STUB;

    typedef enum
    {
        SSL_SUPPLEMENTARY_DATA_NONE = 0,
        SSL_SUPPLEMENTARY_DATA_CERT_PUBLIC_KEY
    } SSL_SUPPLEMENTARY_DATA_TYPES;

    // To hash the public key information, we need to actually get
    // the public key information...
    // 1024 bit key at 8 bits/byte = 128 bytes needed for the public key.
    typedef struct
    {
      WORD pub_size_bytes;
      BYTE pub_key[SSL_RSA_CLIENT_SIZE/8];
      BYTE pub_e[3];
      BYTE pub_guid;    // This is used as a TCP_SOCKET which is a BYTE
    } SSL_PKEY_INFO;

	// Memory definition for SSL keys.  This area is split into Local and
	// Remote areas.  During the handshake, Local.random and Remote.random
	// hold the ServerRandom and ClientRandom values.  Once the session keys
	// are calculated, the Local.app and Remote.app contain the MAC
	// secret, record sequence number, and encryption context for the
	// ARCFOUR module.
	typedef struct
	{
		union
		{
			struct
			{
				BYTE MACSecret[16];			// Server's MAC write secret
				DWORD sequence;				// Server's write sequence number
				ARCFOUR_CTX cryptCtx;		// Server's write encryption context
				BYTE reserved[6];			// Future expansion
			}app;
			BYTE random[32];				// Server.random value
		} Local;
		
		union
		{
			struct
			{
				BYTE MACSecret[16];			// Client's MAC write secret
				DWORD sequence;				// Client's write sequence number
				ARCFOUR_CTX cryptCtx;		// Client's write encryption context
				BYTE reserved[6];			// Future expansion
			}app;
			BYTE random[32];				// Client.random value
		} Remote;		
	} SSL_KEYS;

	// Generic buffer space for SSL.  The hashRounds element is used
	// when this buffer is needed for handshake hash calculations, and
	// the full element is used as the Sbox for ARCFOUR calculations.
	typedef union
	{
		struct
		{
			HASH_SUM hash;
			BYTE md5_hash[16];
			BYTE sha_hash[20];
			#if SSL_RSA_CLIENT_SIZE > 1024
			    BYTE temp[(SSL_RSA_CLIENT_SIZE/4)-sizeof(HASH_SUM)-16-20];
			#else
			    BYTE temp[256-sizeof(HASH_SUM)-16-20];
			#endif
		} hashRounds;
		#if SSL_RSA_CLIENT_SIZE > 1024
    		BYTE full[(SSL_RSA_CLIENT_SIZE/4)];
    	#else
    		BYTE full[256];
    	#endif
	} SSL_BUFFER;
	
	// Storage space for SSL Session identifiers.  (The SessionID and MasterSecret)
	typedef struct
	{
		BYTE sessionID[32];					// The SSL Session ID for this session
		BYTE masterSecret[48];				// Associated Master Secret for this session
	} SSL_SESSION;

	// Stub value for an SSL_SESSION.  The tag associates this session with a 
	// remote node, either by matching to a remote IP address when we are
	// the client or the first 3 bytes of the session ID when we are the host.
	// When a session is free/expired, the tag is 0x00000000.  The lastUsed
	// value is the Tick count when the session was last used so that 
	// older sessions may be overwritten first.
	typedef struct
	{
		DWORD_VAL tag;						// Identifying tag for connection
											// When we're a client, this is the remote IP
											// When we're a host, this is 0x00 followed by first 3 bytes of session ID
											// When this stub is free/expired, this is 0x00
		DWORD lastUsed;						// Tick count when session was last used
	} SSL_SESSION_STUB;

	
	#define SSL_STUB_SIZE		((DWORD)sizeof(SSL_STUB))				// Amount of space needed by a single SSL stub
	#define SSL_STUB_SPACE		(SSL_STUB_SIZE*MAX_SSL_CONNECTIONS)		// Amount of space needed by all SSL stubs
	#define SSL_KEYS_SIZE		((DWORD)sizeof(SSL_KEYS))				// Amount of space needed by a single SSL key
	#define SSL_KEYS_SPACE		(SSL_KEYS_SIZE*MAX_SSL_CONNECTIONS)		// Amount of space needed by all SSL key
	#define SSL_HASH_SIZE		((DWORD)sizeof(HASH_SUM))				// Amount of space needed by a single SSL hash
	#define SSL_HASH_SPACE		((DWORD)(SSL_HASH_SIZE*MAX_SSL_HASHES))	// Amount of space needed by all SSL hash
	#define SSL_BUFFER_SIZE		((DWORD)sizeof(SSL_BUFFER))				// Amount of space needed by a single SSL buffer
	#define SSL_BUFFER_SPACE	(SSL_BUFFER_SIZE*MAX_SSL_BUFFERS)		// Amount of space needed by all SSL buffer
	#define SSL_SESSION_SIZE	((DWORD)sizeof(SSL_SESSION))			// Amount of space needed by a single SSL session
	#define SSL_SESSION_SPACE	(SSL_SESSION_SIZE*MAX_SSL_SESSIONS)		// Amount of space needed by all SSL session
	
	// Total space needed by all SSL storage requirements
	#define RESERVED_SSL_MEMORY ((DWORD)(SSL_STUB_SPACE + SSL_KEYS_SPACE + SSL_HASH_SPACE + SSL_BUFFER_SPACE + SSL_SESSION_SPACE))


/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/

void SSLInit(void);

BYTE SSLStartSession(TCP_SOCKET hTCP, void * buffer, BYTE supDataType);
void SSLTerminate(BYTE sslStubId);
void SSLPeriodic(TCP_SOCKET hTCP, BYTE sslStubID);
WORD SSLRxRecord(TCP_SOCKET hTCP, BYTE sslStubID);
void SSLRxHandshake(TCP_SOCKET hTCP);
void SSLTxRecord(TCP_SOCKET hTCP, BYTE sslStubID, BYTE txProtocol);
void SSLTxMessage(TCP_SOCKET hTCP, BYTE sslStubID, BYTE msg);

void SSLMACBegin(BYTE* MACSecret, DWORD seq, BYTE protocol, WORD len);
void SSLMACAdd(BYTE* data, WORD len);
void SSLMACCalc(BYTE* MACSecret, BYTE* result);

#if defined(STACK_USE_SSL_SERVER)
	void SSLStartPartialRecord(TCP_SOCKET hTCP, BYTE sslStubID, BYTE txProtocol, WORD wLen);
	#define SSLFlushPartialRecord(a)		TCPSSLPutRecordHeader(a, NULL, FALSE);
	#define SSLFinishPartialRecord(a)		TCPSSLPutRecordHeader(a, NULL, TRUE);
#endif

#endif
