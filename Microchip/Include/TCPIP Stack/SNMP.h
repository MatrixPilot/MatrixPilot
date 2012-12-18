/*********************************************************************
 *
 *                  SNMP Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        SNMP.h
 * Dependencies:    StackTsk.h
 *                  UDP.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
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
 * V5.36 ---- STACK_USE_MPFS has been removed	
 ********************************************************************/
 
#ifndef SNMP_H
#define SNMP_H

#include "TCPIP Stack/TCPIP.h"
/****************************************************************************
  Section:
	Macros and Definitions 
  ***************************************************************************/

#define DATA_TYPE_TABLE_SIZE    (sizeof(dataTypeTable)/sizeof(dataTypeTable[0]))


//This is the file that contains SNMP bib file.
//File name must contain all upper case letter and must match
//with what was included in MPFS2 image.

#define SNMP_BIB_FILE_NAME		"snmp.bib"


// Section:  SNMP Tx pdu offset settings
#define _SNMPSetTxOffset(o)     (SNMPTxOffset = o)
#define _SNMPGetTxOffset()      SNMPTxOffset

//This macro will be used to avoid SNMP OID memory buffer corruption 
#define SNMP_MAX_OID_LEN_MEM_USE   (18)

//Change this to match your OID string length.
#define OID_MAX_LEN             (SNMP_MAX_OID_LEN_MEM_USE+1)


#define SNMP_START_OF_VAR       (0)
#define SNMP_END_OF_VAR         (0xff)
#define SNMP_INDEX_INVALID      (0xff)


//Trap information.
//This table maintains list of intereseted receivers
//who should receive notifications when some interesting
//event occurs.

//This macro will be used to avoid SNMP OID memory buffer corruption 
#define SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE   (8)

#define TRAP_TABLE_SIZE         (2)
#define TRAP_COMMUNITY_MAX_LEN       (SNMP_TRAP_COMMUNITY_MAX_LEN_MEM_USE+1)


// Section:  SNMP specific data validation
#define IS_STRUCTURE(a)         (a==STRUCTURE)
#define IS_ASN_INT(a)           (a==ASN_INT)
#define IS_OCTET_STRING(a)      (a==OCTET_STRING)
#define IS_OID(a)               (a==ASN_OID)
#define IS_ASN_NULL(a)          (a==ASN_NULL)
#define IS_GET_REQUEST(a)       (a==GET_REQUEST)
#define IS_GET_NEXT_REQUEST(a)  (a==GET_NEXT_REQUEST)
#define IS_GET_RESPONSE(a)      (a==GET_RESPONSE)
#define IS_SET_REQUEST(a)       (a==SET_REQUEST)
#define IS_TRAP(a)              (a==TRAP)
#define IS_AGENT_PDU(a)         (a==GET_REQUEST || \
                                 a==GET_NEXT_REQUEST || \
                                 a==SET_REQUEST || \
                                 a==SNMP_V2C_GET_BULK)
#define IS_SNMPV3_AUTH_STRUCTURE(a) (a==SNMPV3_ENCRYPTION)


// Section:  SNMP specific variables
#define STRUCTURE               (0x30u)
#define ASN_INT                 (0x02u)
#define OCTET_STRING            (0x04u)
#define ASN_NULL                (0x05u)
#define ASN_OID                 (0x06u)

#define SNMP_IP_ADDR            (0x40)
#define SNMP_COUNTER32          (0x41)
#define SNMP_GAUGE32            (0x42)
#define SNMP_TIME_TICKS         (0x43)
#define SNMP_OPAQUE             (0x44)
#define SNMP_NSAP_ADDR          (0x45)


//This is the SNMP OID variable id.
//This id is assigned via MIB file.  Only dynamic and AgentID
//variables can contian ID.  MIB2BIB utility enforces this
//rules when BIB was generated.
typedef int SNMP_ID;
typedef BYTE SNMP_INDEX;

/*SNMP MIN and MAX message 484 bytes in size */
/*As per RFC 3411 snmpEngineMaxMessageSize and 
RFC 1157 ( section 4- protocol specification )
and implementation  supports more than 484 whenever
feasible.*/
#define SNMP_MAX_MSG_SIZE  484

// Section:  SNMP agent version types
#define SNMP_V1                 (0)
#define SNMP_V2C				(1)
#define SNMP_V3					(3)


// Section:  SNMP v1 and v2c pdu types
#define GET_REQUEST             (0xa0)
#define GET_NEXT_REQUEST        (0xa1)
#define GET_RESPONSE            (0xa2)
#define SET_REQUEST             (0xa3)
#define TRAP                    (0xa4)
#define GET_BULK_REQUEST        (0xa5)
#define REPORT_RESPONSE			(0xa8)

// Section:  SNMP Udp ports
#define SNMP_AGENT_PORT     (161)
#define SNMP_NMS_PORT       (162)
#define AGENT_NOTIFY_PORT   (0xfffe)




/****************************************************************************
  Section:
	Data Structures and Enumerations
  ***************************************************************************/
// Section:  SNMP specific data tyes
typedef enum 
{
	INT8_VAL		= 0x00, 	//8 bit integer value
	INT16_VAL		= 0x01, 	//16 bit integer value
	INT32_VAL		= 0x02, 	//32 bit integer value
	BYTE_ARRAY		= 0x03, 	//Aray of bytes 
	ASCII_STRING	= 0x04, 	//Ascii string type
	IP_ADDRESS		= 0x05, 	//IP address variable
	COUNTER32		= 0x06, 	//32 bit counter variable
	TIME_TICKS_VAL	= 0x07, 	//Timer vakue counter variable	
	GAUGE32 		= 0x08, 	//32 bit guage variable
	OID_VAL 		= 0x09, 	//Object id value var
	DATA_TYPE_UNKNOWN			//Unknown data type
} DATA_TYPE;


// Section:  SNMP specific mib file access information
typedef union
{
	struct
	{
		unsigned int bIsFileOpen : 1;  //MIB file access int flag
	} Flags;						
	BYTE Val;						   //MIB file access byte flag
} SNMP_STATUS;


// Section:  SNMP OID index information
typedef union 
{
	struct
	{
		unsigned int bIsOID:1;	//value is OID/index int flag
	} Flags;
	BYTE Val;					//value is OID/index byte flag
} INDEX_INFO;


// Section:  SNMP object information
typedef union 
{
	struct
	{
		unsigned int bIsDistantSibling : 1; //Object have distant sibling node
		unsigned int bIsConstant : 1;		//Object is constant
		unsigned int bIsSequence : 1;		//Object is sequence
		unsigned int bIsSibling : 1;		//Sibling node flag

		unsigned int bIsParent : 1; 		//Node is parent flag	
		unsigned int bIsEditable : 1;		//Node is editable flag
		unsigned int bIsAgentID : 1;		//Node have agent id flag
		unsigned int bIsIDPresent : 1;		//Id present flag
	} Flags;
	BYTE Val;								//MIB Obj info as byte value
} MIB_INFO;


// Section:  SNMP reuested variable list error status information.
//Max variable in a request supported 15
typedef struct 
{
WORD noSuchObjectErr;		//Var list no such obj errors flags
WORD noSuchNameErr; 		//Var list no such name error 
WORD noSuchInstanceErr; 	//Var list no such instance error
WORD endOfMibViewErr;		//Var list end of mib view error
}reqVarErrStatus;  


// Section:  ASN data type info
typedef struct 
{
	BYTE asnType;	//ASN data type
	BYTE asnLen;	//ASN data length
} DATA_TYPE_INFO;


// Section:  SNMP trap notification information for agent
typedef struct 
{
	char community[NOTIFY_COMMUNITY_LEN];	//Community name array
	BYTE communityLen;						//Community name length
	SNMP_ID agentIDVar; 					//Agent id for trap identification
	BYTE notificationCode;					//Trap notification code
	UDP_SOCKET socket;						//Udp socket number 
	DWORD_VAL timestamp;					//Time stamp for trap
#if defined(SNMP_STACK_USE_V2_TRAP) || defined(SNMP_V1_V2_TRAP_WITH_SNMPV3) && !defined(SNMP_TRAP_DISABLED)
	SNMP_ID trapIDVar;						// SNMPV2 specific trap
#endif	
} SNMP_NOTIFY_INFO;


// Section:  SNMP MIB variable object information
typedef struct 
{
	DWORD			hNode;		//Node location in the mib		
	BYTE			oid;		//Object Id
	MIB_INFO		nodeInfo;	//Node info
	DATA_TYPE		dataType;	//Data type 
	SNMP_ID 		id; 		//Snmp Id	
	WORD_VAL		dataLen;	//Data length	
	DWORD			hData;		//Data
	DWORD			hSibling;	//Sibling info
	DWORD			hChild; 	//Child info
	BYTE			index;		//Index of object
	BYTE			indexLen;	//Index length
} OID_INFO;


// Section:  SNMP pdu information database 
typedef struct 
{
	DWORD_VAL		requestID;		//Snmp request id
	BYTE			nonRepeators;	//# non repeaters in the request	
	BYTE			maxRepetitions; //# max repeaters in the request 
	BYTE			pduType;		//Snmp pdu type
	BYTE			errorStatus;	//Pdu error status
	BYTE			erroIndex;		//Pdu error Index
	BYTE			snmpVersion;	//Snmp version
	WORD			pduLength;		//Pdu length
} PDU_INFO;


// Section:  SNMP specific errors
typedef enum 
{
    SNMP_NO_ERR = 0,			//Snmp no error
    SNMP_TOO_BIG,				//Value too big error
    SNMP_NO_SUCH_NAME,			//No such name in MIB error
    SNMP_BAD_VALUE,				//Not assignable value for the var error	
    SNMP_READ_ONLY,				//Read only variable, write not allowed err
    SNMP_GEN_ERR,				//Snmp gen error
    SNMP_NO_ACCESS,				//Access to modify or read not granted err 
    SNMP_WRONG_TYPE,			//Variable data type wrong error	
    SNMP_WRONG_LENGTH,			//Wrong data length error
    SNMP_WRONG_ENCODING,		//Wrong encoding error
    SNMP_WRONG_VALUE,			//Wrong value for the var type
    SNMP_NO_CREATION,			//No creationg error
    SNMP_INCONSISTENT_VAL,		//Inconsistent value error
    SNMP_RESOURCE_UNAVAILABE,	//Resource unavailbe error	
    SNMP_COMMIT_FAILED,			//Modification update failed error
    SNMP_UNDO_FAILED,			//Modification undo failed	
    SNMP_AUTH_ERROR,			//Authorization failed error
    SNMP_NOT_WRITABLE,			//Variable read only
    SNMP_INCONSISTENT_NAME,		//Inconsistent name
    SNMP_NO_SUCH_OBJ=128,		//No such object error	
    SNMP_NO_SUCH_INSTANCE=129,	//No such instance error
    SNMP_END_OF_MIB_VIEW=130	//Reached to end of mib error
} SNMP_ERR_STATUS;


typedef struct 
{
   BYTE Size;
   struct
   {
       BYTE communityLen;					//Community name length
       char community[TRAP_COMMUNITY_MAX_LEN];	//Community name array
       IP_ADDR IPAddress;					//IP address to which trap to be sent
       struct
       {
           unsigned int bEnabled : 1;		//Trap enabled flag	
       } Flags;
   } table[TRAP_TABLE_SIZE];				
} TRAP_INFO;

typedef union
{
    DWORD dword;				//double word value
    WORD  word;					//word value
    BYTE  byte;					//byte value
    BYTE  v[sizeof(DWORD)];		//byte array
} SNMP_VAL;

typedef enum
{
	READ_COMMUNITY=1,		//Read only community	
	WRITE_COMMUNITY=2,		//Read write community
	INVALID_COMMUNITY=3			//Community invalid
}COMMUNITY_TYPE;


/********************************************************************
  This is the list of SNMP action a remote NMS can perform. This
  inforamtion is passed to application via callback
  SNMPValidateCommunity(). Application should validate the action for
  given community string.                                            
  ********************************************************************/
typedef enum
{
    SNMP_GET            = 0xa0,	//Snmp GET identifier
    SNMP_GET_NEXT       = 0xa1, //Snmp GET_NEXT identifier
    SNMP_GET_RESPONSE   = 0xa2,	//Snmp GET_RESPONSE identifier
    SNMP_SET            = 0xa3,	//Snmp SET identifier
    SNMP_TRAP           = 0xa4,	//Snmp TRAP identifier
    SNMP_V2C_GET_BULK	= 0xa5,	//Snmp GET_BULK identifier
    SNMP_V2_TRAP		= 0xa7, //Snmp v2 Trap Identifier
    SNMPV3_ENCRYPTION	= 0x04,
    SNMP_ACTION_UNKNOWN = 0		//Snmp requested action unknown
} SNMP_ACTION;

typedef enum
{
	COLD_START 			=0x0,
	WARM_START			=0x1,
	LINK_DOWN			=0x2,
	LINK_UP				=0x3,
	AUTH_FAILURE		=0x4,	
	EGP_NEBOR_LOSS		=0x5,
	ENTERPRISE_SPECIFIC	=0x6
	
} GENERIC_TRAP_NOTIFICATION_TYPE; 


typedef enum
{
	VENDOR_TRAP_DEFAULT 	=0x0,
	BUTTON_PUSH_EVENT		=0x1,
	POT_READING_MORE_512	=0x2
} VENDOR_SPECIFIC_TRAP_NOTIFICATION_TYPE;



typedef enum
{
	SNMP_RESPONSE_PDU=0x01,
	SNMP_REQUEST_PDU=0x02

}INOUT_SNMP_PDU;

// SNMPv3 
typedef struct 
{	
	UINT8 *head;
	WORD length;
	WORD maxlength;
	WORD msgAuthParamOffset;
}SNMPV3MSGDATA;


typedef struct
{
	UINT8 oidstr[16];
	UINT8 version;
}SNMPNONMIBRECDINFO;



extern UINT16 gSNMPv3ScopedPduDataPos;
extern SNMPV3MSGDATA gSNMPv3ScopedPduRequestBuf;
extern SNMPV3MSGDATA gSNMPv3ScopedPduResponseBuf;
//extern SNMPV3_REQUEST_WHOLEMSG gSnmpV3InPduWholeMsgBuf;


/****************************************************************************
  Section:
	Global Variables
  ***************************************************************************/
#if !defined(SNMP_TRAP_DISABLED)  
extern BYTE gSendTrapFlag;//Global flag to send Trap
extern BYTE gGenericTrapNotification;//Global flag for Generic trap notification
extern BYTE gSpecificTrapNotification;//Global flag for vendor specific trap notification
#endif
extern BYTE gOIDCorrespondingSnmpMibID;//Gloabal var to store SNMP ID of var for OID received in SNMP request.
extern BYTE	gSetTrapSendFlag;
extern BYTE appendZeroToOID;//global flag to modify OID by appending zero	
extern BOOL getZeroInstance;

// SNMPNotifyInfo is not required if TRAP is disabled
#if !defined(SNMP_TRAP_DISABLED)
extern SNMP_NOTIFY_INFO SNMPNotifyInfo; //notify info for trap
#endif
extern MPFS_HANDLE hMPFS;	//MPFS file handler
extern SNMPNONMIBRECDINFO gSnmpNonMibRecInfo[];


BYTE IsValidLength(WORD *len);
void SNMPInit(void);
BOOL SNMPTask(void);
void SNMPSendTrap(void);
BYTE SNMPValidateCommunity(BYTE * community);
BOOL SNMPNotify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index);
BOOL SNMPSetVar(SNMP_ID var, SNMP_INDEX index,BYTE ref, SNMP_VAL val);
BOOL SNMPGetVar(SNMP_ID var, SNMP_INDEX index,BYTE* ref, SNMP_VAL* val);
BOOL SNMPIsNotifyReady(IP_ADDR* remoteHost);
void SNMPNotifyPrepare(IP_ADDR* remoteHost, char* community, BYTE communityLen, SNMP_ID agentIDVar, BYTE notificationCode, DWORD timestamp);
BOOL SNMPGetNextIndex(SNMP_ID var, SNMP_INDEX* index);
BOOL SNMPGetExactIndex(SNMP_ID var, SNMP_INDEX index);
BOOL SNMPIsValidSetLen(SNMP_ID var, BYTE len,BYTE index);

BYTE ProcessGetVar(OID_INFO* rec, BOOL bAsOID, PDU_INFO* pduDbPtr);
BYTE ProcessGetNextVar(OID_INFO* rec, PDU_INFO* pduDbPtr);
BYTE ProcessSetVar(PDU_INFO* pduDbPtr,OID_INFO* rec, SNMP_ERR_STATUS* errorStatus);
BYTE ProcessGetBulkVar(OID_INFO* rec, BYTE* oidValuePtr, BYTE* oidLenPtr,BYTE* successor,PDU_INFO* pduDbPtr);
BYTE IsValidStructure(WORD* dataLen);

BOOL GetOIDStringByID(SNMP_ID id, OID_INFO* info, BYTE* oidString, BYTE* len);


extern BOOL IsValidInt(DWORD* val);
extern BYTE _SNMPGet(void);
extern void _SNMPPut(BYTE v);


extern 	void SetErrorStatus(WORD errorStatusOffset,WORD errorIndexOffset, SNMP_ERR_STATUS errorStatus,BYTE errorIndex);
extern BYTE OIDLookup(PDU_INFO* pduDbPtr,BYTE* oid, BYTE oidLen, OID_INFO* rec);
extern 	BOOL GetNextLeaf(OID_INFO* rec);

extern BOOL SNMPIdRecrdValidation(PDU_INFO * pduPtr,OID_INFO *var,BYTE * oidValuePtr,BYTE oidLen);
BOOL GetOIDStringByAddr(OID_INFO* rec, BYTE* oidString, BYTE* len);




extern SNMP_ERR_STATUS Snmpv3MsgProcessingModelProcessPDU(BYTE inOutPdu);
extern SNMP_ERR_STATUS Snmpv3UserSecurityModelProcessPDU(BYTE inOutPdu);
extern SNMP_ERR_STATUS Snmpv3ScopedPduProcessing(BYTE inOutPdu);

extern UINT8 Snmpv3TrapScopedpdu(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,UINT8 targetIndex);



extern void _SNMPDuplexInit(UDP_SOCKET socket);
extern void Snmpv3GetAuthEngineTime(void);
extern void Snmpv3UsmAesEncryptDecryptInitVector(BYTE inOutPdu);
extern void Snmpv3UsmOutMsgAuthenticationParam(UINT8 hashType);

extern BYTE Snmpv3AESDecryptRxedScopedPdu(void);
extern BYTE *getSnmpV2GenTrapOid(BYTE generic_trap_code,BYTE *len);
extern BYTE Snmpv3GetBufferData(SNMPV3MSGDATA getbuf,UINT16 pos);
extern void Snmpv3FormulateEngineID(UINT8 fifthOctectIdentifier );


extern BOOL Snmpv3ValidateSecNameAndSecLvl(void);
extern BOOL Snmpv3ValidateSecurityName(void);
extern BOOL Snmpv3ValidateEngineId(void);
extern BOOL GetDataTypeInfo(DATA_TYPE dataType, DATA_TYPE_INFO *info );
extern BOOL Snmpv3Notify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index,UINT8 targetIndex);
extern BOOL ProcessSnmpv3MsgData(PDU_INFO* pduDbPtr);
extern BOOL Snmpv3BufferPut(BYTE val ,SNMPV3MSGDATA *putbuf);
extern void Snmpv3InitializeUserDataBase(void);




#endif
