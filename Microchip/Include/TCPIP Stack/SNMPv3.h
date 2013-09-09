/*********************************************************************
 *
 *  Simple Network Management Protocol (SNMP) Version 3 Agent 
 *  
 *  Module for Microchip TCP/IP Stack
 *	 -Provides SNMPv3 API for doing stuff
 *	
 *	-Reference: RFCs 3410, 3411, 3412, 3413, 3414 
 *********************************************************************
 * FileName:        SNMPv3.h
 * Dependencies: TCP/IP stack
 * Processor:       PIC32
 * Compiler:        Microchip C32 
 *
 * Software License Agreement
 *
 * Copyright (C) 2012 Microchip Technology Inc.  All rights
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
 ********************************************************************/

#ifndef SNMPV3_H
#define SNMPV3_H


#include "GenericTypeDefs.h"

#include "TCPIP Stack/TCPIP.h"
/*=======================================================================================*/

/* Abstarct Service Interfaces and Set of primitives for various sub systems of the SNMP engine */

/*=======================================================================================*/

/* ============== */
/* Dispatcher Primitives  */
/* ============== */

extern UINT8 snmpEngineID[]; //Reserving 32 bytes for the snmpEngineID as the octet string length can vary form 5 to 32 
extern	UINT32 snmpEngineBoots;//The number of times that the SNMP engine has (re-)initialized itself since snmpEngineID was last configured.
extern DWORD_VAL snmpEngineTime;//The number of seconds since the value of the snmpEngineBoots object last changed
extern	DWORD_VAL snmpEngineMaxMessageSize;
extern UINT8 snmpEngnIDLength;
#define SNMPV3_USM_MAX_USER	3 //User Security Model should have atleast 1 user. Default is 3. User should change as per the requirement.  

//SNMPv3 User Security Name length
#define USER_SECURITY_NAME_LEN (16)
//User security name length for memory validation
#define SNMPV3_USER_SECURITY_NAME_LEN_MEM_USE (USER_SECURITY_NAME_LEN+1)

//SNMPv3 Authentication Localized passwed key lenegth size
#define AUTH_LOCALIZED_PASSWORD_KEY_LEN	(20)
//SNMPv3 authentication localized Key length for memory validation
#define SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (AUTH_LOCALIZED_PASSWORD_KEY_LEN+1)

//#define PRIV_LOCALIZED_PASSWORD_KEY_LEN	16
// SNMPv3 Privacy Pasword key length size
#define PRIV_LOCALIZED_PASSWORD_KEY_LEN	(20)
//SNMPv3 privacy key length size for memory validation
#define SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE (PRIV_LOCALIZED_PASSWORD_KEY_LEN+1)


/* 
Registering Responsibility for Handling SNMP PDUs 

Applications can register/unregister responsibility for a specific
contextEngineID, for specific pduTypes, with the PDU Dispatcher
according to the following primitives. The list of particular
pduTypes that an application can register for is determined by the
Message Processing Model(s) supported by the SNMP entity that
contains the PDU Dispatcher.

Note that realizations of the registerContextEngineID and
unregisterContextEngineID abstract service interfaces may provide
implementation-specific ways for applications to register/deregister
responsibility for all possible values of the contextEngineID or
pduType parameters.
*/

typedef struct  registerContextEngineID
{
	BYTE* contextEngineID; //take responsibility for this one
	BYTE pduType;			//the pduType(s) to be registered
}statusInformation; //success or errorIndication

struct unregisterContextEngineID 
{
	BYTE* contextEngineID; //give up responsibility for this one
	BYTE pduType;			//the pduType(s) to be unregistered
};

#define SNMPV3MSG_AUTHENTICATION_FAIL	0
#define SNMPV3MSG_AUTHENTICATION_SUCCESS 1

typedef enum{

	SNMPV3_MSG_AUTH_FAIL=0x00,
	SNMPV3_MSG_AUTH_PASS=0x01
}SNMPV3_MSG_AUTH_SEC_PARAM_RESULT;

typedef enum{

SNMPV3_MSG_PRIV_FAIL=0x00,
SNMPV3_MSG_PRIV_PASS=0x01

}SNMPV3_MSG_PRIV_SEC_PARAM_RESULT;




extern UINT8  snmpSecurityLevel;

typedef enum
{
	SNMPV3_DES_PRIV=0x0,
	SNMPV3_AES_PRIV,
	SNMPV3_NO_PRIV
}SNMPV3_PRIV_PROT_TYPE;


// Type of hash being calculated
typedef enum
{
	SNMPV3_HAMC_MD5	= 0u,		// MD5 is being calculated
	SNMPV3_HMAC_SHA1,				// SHA-1 is being calculated
	SNMPV3_NO_HMAC_AUTH
} SNMPV3_HMAC_HASH_TYPE;

/*
Generate Outgoing Request or Notification 

statusInformation =  -- sendPduHandle if success
				   -- errorIndication if failure
*/

struct dispatcherStatusInfo
{
	BYTE transportDomain;		//transport domain to be used
	UINT32 transportAddress;	//transport address to be used
	BYTE messageProcessingModel;//typically, SNMP version
	BYTE securityModel; 		//Security Model to use
	BYTE* securityName; 		//on behalf of this principal
	BYTE securityLevel;			//Level of Security requested
	BYTE* contextEngineID; 	//data from/at this entity
	BYTE* contextName;			//data from/in this context
	BYTE pduVersion; 			//the version of the PDU
	BYTE* PDU; 					//SNMP Protocol Data Unit
	BOOL expectResponse; 		//TRUE or FALSE

};



/* 
Process Incoming Request or Notification PDU 

Dispatcher provides the following primitive to pass an incoming snmp pdu to an application. 
*/

struct dispatcherProcessPdu  //process Request/Notification PDU
{
	BYTE messageProcessingModel;	//typically, SNMP version
	BYTE securityModel;				//Security Model in use
	BYTE* securityName;			//on behalf of this principal
	BYTE securityLevel;				//Level of Security
	BYTE* contextEngineID;			//data from/at this SNMP entity
	BYTE* contextName;				//data from/in this context
	BYTE pduVersion;				//the version of the PDU
	BYTE* PDU;						//SNMP Protocol Data Unit
	DWORD_VAL maxSizeResponseScopedPDU;// maximum size of the Response PDU
	UINT32 stateReference;			//reference to state information needed when sending a response

};

typedef struct
{
	
	UINT8 userName[SNMPV3_USER_SECURITY_NAME_LEN_MEM_USE];
	UINT8 userAuthPswd[SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE]; //RFC specifies not to save password with the managed nodes instead store pswd ipad and opad values.
	UINT8 userPrivPswd[SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE];
	UINT8 userAuthPswdLoclizdKey[SNMPV3_AUTH_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE];
	UINT8 userPrivPswdLoclizdKey[SNMPV3_PRIV_LOCALIZED_PASSWORD_KEY_LEN_MEM_USE];
	//UINT8 userPrivPswdLoclizdKey[20];
	UINT8 userAuthLocalKeyHmacIpad[64];
	UINT8 userAuthLocalKeyHmacOpad[64];
	UINT8 userDBIndex;
	UINT8 userHashType;
	UINT8 userNameLength;
	UINT8 userAuthPswdLen;
	UINT8 userPrivPswdLen;
	UINT8 userPrivType;
}snmpV3EngnUserDataBase; 

extern snmpV3EngnUserDataBase snmpV3UserDataBase[];

typedef struct  
{
	
	UINT32 maxMessageSize; 			//of the sending SNMP entity
//	IN securityParameters; 			//for the received message
	UINT32 wholeMsgLength; 			//length as received on the wire
	BYTE* wholeMsg; 				//as received on the wire
	BYTE* securityEngineID;	 		//authoritative SNMP entity
	BYTE* securityName; 			//identification of the principal
	BYTE* scopedPDU; 				//message (plaintext) payload
//	OUT securityStateReference; 	//reference to security state
	UINT32 maxSizeResponseScopedPDU;//maximum size sender can handle
	BYTE messageProcessingModel; 	//typically, SNMP version
	BYTE securityModel; 			//for the received message
	BYTE securityLevel; 			//Level of Security
	UINT8 securityEngineIDLen;	 		//authoritative SNMP entity
	BYTE securityNameLength;
}SecuritySysProcessIncomingMsg; 

extern SecuritySysProcessIncomingMsg securityPrimitivesOfIncomingPdu;


/*
Generate Outgoing Response

The PDU Dispatcher provides the following primitive for an
application to return an SNMP Response PDU to the PDU Dispatcher:

result = SUCCESS or FAILURE
*/

struct dispathcerReturnResponsePdu
{
	BYTE messageProcessingModel;	//typically, SNMP version
	BYTE securityModel;				//Security Model in use
	BYTE* securityName;			//on behalf of this principal
	BYTE securityLevel;				//same as on incoming request
	BYTE* contextEngineID;			//data from/at this SNMP entity
	BYTE* contextName;				//data from/in this context
	BYTE pduVersion;				//the version of the PDU
	BYTE* PDU;						//SNMP Protocol Data Unit
	UINT32 maxSizeResponseScopedPDU;//maximum size sender can accept
	UINT32 stateReference;			//reference to state information as presented with the request
	statusInformation statInfo;		//success or errorIndication, error counter OID/value if error

};


/*
Process Incoming Response PDU

The PDU Dispatcher provides the following primitive to pass an
incoming SNMP Response PDU to an application:

*/

struct processResponsePdu //process Response PDU
{ 
	BYTE messageProcessingModel;	//typically, SNMP version
	BYTE securityModel;				//Security Model in use
	BYTE* securityName;			//on behalf of this principal
	BYTE securityLevel;				//Level of Security
	BYTE* contextEngineID;			//data from/at this SNMP entity
	BYTE* contextName;				//data from/in this context
	BYTE pduVersion;				//the version of the PDU
	BYTE* PDU;						//SNMP Protocol Data Unit
	statusInformation statInfo;		//success or errorIndication
//	IN sendPduHandle;				//handle from sendPdu
};			






/*=======================================================================================*/

/* =========================== */
/* Message Processing Subsystem Primitives  */
/* =========================== */

/*
The Dispatcher interacts with a Message Processing Model to process a
specific version of an SNMP Message. Below are the
primitives provided by the Message Processing Subsystem.
*/

/* 
Prepare Outgoing SNMP Request or Notification Message

The Message Processing Subsystem provides this service primitive for
preparing an outgoing SNMP Request or Notification Message
*/

struct MsgProcModPrepareOutgoingMessage
{
	BYTE transportDomain;		//transport domain to be used
	UINT32 transportAddress;	//transport address to be used
	BYTE messageProcessingModel;//typically, SNMP version
	BYTE securityModel;			//Security Model to use
	BYTE* securityName;		//on behalf of this principal
	BYTE securityLevel;			//Level of Security requested
	BYTE* contextEngineID;		//data from/at this entity
	BYTE* contextName;			//data from/in this context
	BYTE pduVersion;			//the version of the PDU
	BYTE* PDU;					//SNMP Protocol Data Unit
	BOOL expectResponse;		//TRUE or FALSE
	//IN sendPduHandle;			//the handle for matching incoming responses
	BYTE destTransportDomain;	//destination transport domain
	UINT32 destTransportAddress;//destination transport address
	BYTE* outgoingMessage;		//the message to send
	UINT32 outgoingMessageLength; //its length
};



/*
Prepare an Outgoing SNMP Response Message

The Message Processing Subsystem provides this service primitive for
preparing an outgoing SNMP Response Message:
result = -- SUCCESS or FAILURE

*/
struct MsgProcModPrepareResponseMessage
{
	BYTE messageProcessingModel;	//typically, SNMP version
	BYTE securityModel;  			//same as on incoming request
	BYTE* securityName;  			//same as on incoming request
	BYTE securityLevel;  			//same as on incoming request
	BYTE* contextEngineID;  		//data from/at this SNMP entity
	BYTE* contextName;  			//data from/in this context
	BYTE pduVersion;  				//the version of the PDU
	BYTE* PDU;  					//SNMP Protocol Data Unit
	UINT32 maxSizeResponseScopedPDU;//maximum size able to accept
	UINT32 stateReference;  		//reference to state information as presented with the request
	statusInformation statInfo;//success or errorIndication, error counter OID/value if error
	BYTE destTransportDomain;  		//destination transport domain
	UINT32 destTransportAddress;  	//destination transport address
	BYTE* outgoingMessage;  		//the message to send
	UINT32 outgoingMessageLength;  	//its length
};



/*
Prepare Data Elements from an Incoming SNMP Message

The Message Processing Subsystem provides this service primitive for
preparing the abstract data elements from an incoming SNMP message:
result = -- SUCCESS or errorIndication

*/
struct MsgProcModPrepareDataElements
{
	BYTE transportDomain;		//origin transport domain
	UINT32 transportAddress; 	//origin transport address
	BYTE* wholeMsg; 			//as received from the network
	UINT32 wholeMsgLength; 		//as received from the network
	BYTE messageProcessingModel;//typically, SNMP version
	BYTE securityModel; 		//Security Model to use
	BYTE* securityName; 		//on behalf of this principal
	BYTE securityLevel; 		//Level of Security requested
	BYTE* contextEngineID; 	//data from/at this entity
	BYTE* contextName; 		//data from/in this context
	BYTE pduVersion; 			//the version of the PDU
	BYTE* PDU ; 				//SNMP Protocol Data Unit
	BYTE pduType ; 				//SNMP PDU type
//	OUT sendPduHandle; 			// handle for matched request
	UINT32 maxSizeResponseScopedPDU; //maximum size sender can accept
	statusInformation statInfo; 	//success or errorIndication error counter OID/value if error
	UINT32 stateReference; 		//reference to state information to be used for possible Response
};



/*=======================================================================================*/

/* ======================== */
/* Access Control Subsystem Primitives  */
/* ======================== */

/*
Applications are the typical clients of the service(s) of the Access
Control Subsystem. The following primitive is provided by the Access 
Control Subsystem to check if access is allowed:

statusInformation = -- success or errorIndication
*/

struct AccessCtrlSubSysIsAccessAllowed
{
	BYTE securityModel; 	//Security Model in use
	BYTE* securityName;	//principal who wants to access
	BYTE securityLevel;	 	//Level of Security
	BYTE viewType; 			//read, write, or notify view
	BYTE* contextName; 	//context containing variableName
	BYTE* variableName; 	//OID for the managed object
};


/*=======================================================================================*/

/* ==================== */
/* Security Subsystem Primitives  */
/* ==================== */

/*
The Message Processing Subsystem is the typical client of the services of the Security Subsystem.
*/


/* 
Generate a Request or Notification Message

The Security Subsystem provides the following primitive to generate a
Request or Notification message:

*/

typedef enum
{

	RESERVED=0x0,
	IPV4_ADDR_ENGN_ID=0x01,//4octets 
	IPV6_ADDR_ENGN_ID=0x02,//16 octets
	MAC_ADDR_ENGN_ID=0x03,//6 octets
	ADMIN_ASSIGNED_TEXT=0x04,
	ADMIN_ASSIGNED_OCTETS=0x05,
	RESERVED_UNUSED=0x06, //6 to 127 are reserved and unused
	ENTERPRISE_DEFINED=128 //128 to 255 as defiend by the enterprise maximum remaining length

}SNMP_ENGNID_OCTET_IDENTIFIER_VAL;	//The fifth octet indicates how the rest (6th and following octets) are formatted. Refer to RFC3411 section5 Page# 41	


typedef enum
{
	/*Octet's Least significant three bits: Reportable, PrivFlag, AuthFlag */	
	NO_REPORT_NO_PRIVACY_NO_AUTH 				=0x00, /* 00000000b */
	NO_REPORT_NO_PRIVACY_BUT_AUTH_PROVIDED		=0x01, /* 00000001b */
	NO_REPORT_PRIVACY_PROVIDED_BUT_NO_AUTH		=0x02, /* 00000010b Priv without Auth is not allowed*/
	NO_REPORT_PRIVACY_AND_AUTH_PROVIDED			=0x03, /* 00000011b */
	
	REPORT2REQ_NO_PRIVACY_NO_AUTH				=0x04, /* 00000100b */
	REPORT2REQ_NO_PRIVACY_BUT_AUTH_PROVIDED		=0x05, /* 00000101b */
	REPORT2REQ_PRIVACY_PROVIDED_BUT_NO_AUTH		=0x06, /* 00000110b Priv without Auth is not allowed*/
	REPORT2REQ_PRIVACY_AND_AUTH_PROVIDED		=0x07, /* 00000111b */
	INVALID_MSG									=0xFF
	
}REPORT_FLAG_AND_SECURITY_LEVEL_FLAGS;

typedef enum
{
	noAuthProtocol = 0x1,
	hmacMD5Auth,
	hmacSHAAuth,
	noPrivProtocol,
	desPrivProtocol = 0x5,
	aesPrivProtocol =0x6
}USM_SECURITY_LEVEL;

struct SecuritySysGenerateRequestMsg
{
	BYTE messageProcessingModel; 	//typically, SNMP version
	BYTE* globalData; 				//message header, admin data
	UINT32 maxMessageSize; 			//of the sending SNMP entity
	BYTE securityModel; 			//for the outgoing message
	BYTE* securityEngineID ; 		//authoritative SNMP entity
	BYTE* securityName;			//on behalf of this principal
	BYTE securityLevel; 			//Level of Security requested
	BYTE* scopedPDU; 				//message (plaintext) payload
	//OUT securityParameters; 		//filled in by Security Module
	BYTE* wholeMsg ; 				//complete generated message
	UINT32 wholeMsgLength; 			//length of the generated message
};



/*
Process Incoming Message

The Security Subsystem provides the following primitive to process an
incoming message:
statusInformation = -- errorIndication or success error counter OID/value if error

*/ 




/*
Generate a Response Message

The Security Subsystem provides the following primitive to generate a
Response message:

*/ 

struct SecuritySysGenerateResponseMsg
{
	BYTE messageProcessingModel; 	//typically, SNMP version
	BYTE* globalData; 				//message header, admin data
	UINT32 maxMessageSize; 			//of the sending SNMP entity
	BYTE securityModel; 			//for the outgoing message
	BYTE* securityEngineID; 		//authoritative SNMP entity
	BYTE* securityName;			//on behalf of this principal
	BYTE securityLevel; 			//for the outgoing message
	BYTE* scopedPDU; 				//message (plaintext) payload
//	IN securityStateReference; 	//reference to security state information from original request
//	OUT securityParameters; 	//filled in by Security Module
	BYTE* wholeMsg;	 			//complete generated message
	UINT32 wholeMsgLength; 		//length of the generated message
};

/* Snmp Message Processing Model */

typedef enum
{
	ANY_SECUTIRY_MODEL=0x00,
	SNMPV1_SECURITY_MODEL=0X01,
	SNMPV2C_SECURITY_MODEL=0X02,
	SNMPV3_USM_SECURITY_MODEL=0X03
	/* Values between 1 to 255, inclusive, are resereved for standards-track
	     Security Models  and are managed by IANA.*/
}STD_BASED_SNMP_SECURITY_MODEL;

typedef enum
{
	SNMPV1_MSG_PROCESSING_MODEL=0X00,
	SNMPV2C_MSG_PROCESSING_MODEL=0X01,
	SNMPV2U_V2_MSG_PROCESSING_MODEL=0X02,
	SNMPV3_MSG_PROCESSING_MODEL=0X03
	/* Values between 0 to 255, inclusive, are resereved for standards-track
	     Message processing Models and are managed by IANA.*/

}STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL;

typedef enum
{
	NO_AUTH_NO_PRIV=1,
	AUTH_NO_PRIV,
	AUTH_PRIV
}STD_BASED_SNMPV3_SECURITY_LEVEL;


/* 
 snmpv3 target configuration with respect to trap.
*/
typedef struct
{	
	UINT8 userSecurityName[USER_SECURITY_NAME_LEN];
	STD_BASED_SNMP_MESSAGE_PROCESSING_MODEL messageProcessingModelType;
	STD_BASED_SNMP_SECURITY_MODEL securityModelType;
	STD_BASED_SNMPV3_SECURITY_LEVEL securityLevelType;
}snmpV3TrapConfigDataBase; 




typedef struct 
{	
	UINT8* wholeMsgHead;
	UINT8* snmpMsgHead;
	WORD_VAL wholeMsgLen;
	WORD_VAL snmpMsgLen;
	WORD msgAuthParamOffsetInWholeMsg;
	WORD scopedPduOffset;
	BYTE scopedPduAuthStructVal;
	WORD scopedPduStructLen;
}SNMPV3_REQUEST_WHOLEMSG;


typedef struct 
{	
	UINT8* wholeMsgHead;
	UINT8* snmpMsgHead;
	WORD_VAL wholeMsgLen;
	WORD_VAL snmpMsgLen;
	UINT8* msgAuthParamOffsetOutWholeMsg;
	UINT8* scopedPduOffset;
	WORD scopedPduStructLen;
	BYTE scopedPduAuthStructVal;
}SNMPV3_RESPONSE_WHOLEMSG;


extern snmpV3TrapConfigDataBase gSnmpv3TrapConfigData[];
extern SNMPV3_REQUEST_WHOLEMSG gSnmpV3InPduWholeMsgBuf;

/*=======================================================================================*/

/* ============== */
/*  Common Primitives   */
/* ============== */

/*
These primitive(s) are provided by multiple Subsystems.
*/

/*
Release State Reference Information

All Subsystems which pass stateReference information also provide a
primitive to release the memory that holds the referenced state
information

*/

struct StateRelease
{
	UINT32 stateReference; 	//handle of reference to be released
};


void Snmpv3USMOutMsgPrivParam(void);

extern void Snmpv3Init(void);
extern void Snmpv3FreeDynAllocMem(void);
extern void Snmpv3ComputeHMACIpadOpadForAuthLoclzedKey(UINT8 userDBIndex);
extern UINT8* Snmpv3ComputeHmacMD5Digest(UINT8 * inData, UINT32 dataLen,UINT8* userExtendedLclzdKeyIpad,UINT8* userExtendedLclzdKeyOpad);

extern BOOL Snmpv3CmprTrapSecNameAndSecLvlWithUSMDb(BYTE tragetIndex,BYTE userTrapSecLen,
			UINT8 *userTrapSecurityName,STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel);

extern BYTE Snmpv3GetTrapSecurityLevel(STD_BASED_SNMPV3_SECURITY_LEVEL securityLevel);
extern BYTE Snmpv3GetSecurityLevel(BYTE userIndex);

extern BYTE Snmpv3AESEncryptResponseScopedPdu(SNMPV3_RESPONSE_WHOLEMSG* plain_text/*UINT8 userDBIndex*/);
extern BYTE Snmpv3AuthenticateTxPduForDataIntegrity(SNMPV3_RESPONSE_WHOLEMSG* txDataPtr);
extern BYTE Snmpv3AuthenticateRxedPduForDataIntegrity(SNMPV3_REQUEST_WHOLEMSG* rxDataPtr);
extern void Snmpv3UsmSnmpEngnAuthPrivPswdLocalization(UINT8 userDBIndex);
BOOL _IsSNMPv3ValidStructure(UINT8* wholeMsgPtr,WORD* pos, WORD* dataLen );
extern BOOL _Snmpv3IsValidInt(UINT8 * wholeMsgPtr,WORD* pos, DWORD* val);
void Snmpv3Pswd2LocalizedAuthKeyMD5Hashing(UINT8* pswdToLocalized, UINT8 pswdLen);
void Snmpv3Pswd2LocalizedAuthKeySHAHashing(UINT8* pswdToLocalized, UINT8 pswdLen);
void Snmpv3UsmSnmpEngnAuthPrivPswdLocalization(UINT8 userDBIndex);
void Snmpv3InitializeUserDataBase(void);
void Snmpv3ComputeMd5HmacCode(UINT8 xx_bits,UINT8* digestptr,UINT8 * indata, UINT32 dataLen,
					  UINT8* userExtendedLclzdKeyIpad, UINT8* userExtendedLclzdKeyOpad);
void Snmpv3ComputeShaHmacCode(UINT8 xx_bits,UINT8* digestptr, UINT8 * indata, UINT32 dataLen,
					 UINT8* userExtendedLclzdKeyIpad,UINT8* userExtendedLclzdKeyOpad);
void Snmpv3AuthKeyZeroing2HmacBufLen64(UINT8* authKey, UINT8 authKeyLen,	UINT8 hashType);
UINT8* Snmpv3ComputeHmacMD5Digest(UINT8 * inData, UINT32 dataLen,UINT8* userExtendedLclzdKeyIpad,UINT8* userExtendedLclzdKeyOpad);
UINT8* Snmpv3ComputeHmacShaDigest(UINT8 * inData, UINT32 dataLen,UINT8* userExtendedLclzdKeyIpad,UINT8* userExtendedLclzdKeyOpad);




#endif
