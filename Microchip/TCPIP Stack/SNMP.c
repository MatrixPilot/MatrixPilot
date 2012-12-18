/*********************************************************************
 *
 *  Simple Network Management Protocol (SNMP) Version 1 Agent 
 *  Simple Network Management Protocol (SNMP) Version 2 community based Agent
 *  Module for Microchip TCP/IP Stack
 *	 -Provides SNMP API for doing stuff
 *	
 *	-Reference: RFC 1157 (for SNMP V1)
 *			     RFC 3416 (for SNMPv2C)   	
 *********************************************************************
 * FileName:        SNMP.c
 * Dependencies:    UDP, ARP
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
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     1/9/03  	Original        (Rev 1.0)
 * Dan Cohen            12/11/03	Removed trap support by #define if not
 *                              	required to lower code space requirements
 * Amit Shirbhate 		09/24/08	SNMPv2c Support, comments and function
 *									headers added.	
 * Hrisikesh Sahu		04/15/10	SNMPv2 Trap Format Support.
 ********************************************************************/

#define __SNMP_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_SNMP_SERVER)

#include "TCPIP Stack/TCPIP.h"
#ifdef STACK_USE_SNMPV3_SERVER
#include "TCPIP Stack/SNMPv3.h"
#endif
#if defined(STACK_USE_MPFS2)



static BYTE FindOIDsInRequest(WORD pdulen);
static SNMP_ACTION ProcessHeader(PDU_INFO* pduDbPtr, char* community, BYTE* len);
static BOOL ProcessGetSetHeader(PDU_INFO* pduDbPtr);
static BOOL ProcessVariables(PDU_INFO* pduDbPtr,char* community, BYTE len);
static BOOL IsValidOID(BYTE* oid, BYTE* len);
static BOOL IsValidCommunity(char* community, BYTE* len);
static BOOL IsValidPDU(SNMP_ACTION* pdu);
static BOOL IsASNNull(void);
static BOOL SNMPCheckIfPvtMibObjRequested(BYTE* OIDValuePtr);
static void ReadMIBRecord(DWORD h, OID_INFO* rec);



// This function is used only when TRAP is enabled.
#if !defined(SNMP_TRAP_DISABLED)
//extern void Snmpv3InitializeUserDataBase(void);
#endif



/****************************************************************************
  Section:
	Global Variables
  ***************************************************************************/
WORD SNMPTxOffset=0;	//Snmp udp buffer tx offset
static WORD SNMPRxOffset=0;	//Snmp udp buffer rx offset		
static SNMP_STATUS SNMPStatus;	//MIB file access status
static UDP_SOCKET SNMPAgentSocket = INVALID_UDP_SOCKET;	//Snmp udp socket
MPFS_HANDLE hMPFS;	//MPFS file handler
extern TRAP_INFO trapInfo;	//trap information
WORD msgSecrtyParamLenOffset;
/* This variable is used for gext next request for zero instance  */
BOOL getZeroInstance=FALSE;

BYTE appendZeroToOID=FALSE;//global flag to modify OID by appending zero 	
reqVarErrStatus snmpReqVarErrStatus; //vars	from req list processing err status	
WORD msgSecrtyParamLenOffset=0;




// SNMPNotifyInfo is not required if TRAP is disabled
#if !defined(SNMP_TRAP_DISABLED)
SNMP_NOTIFY_INFO SNMPNotifyInfo; //notify info for trap
#endif

//ASN format datatype for snmp v1 and v2c
static ROM DATA_TYPE_INFO dataTypeTable[] =
{
    { ASN_INT,           1       }, //INT8_VAL          
    { ASN_INT,           2       }, //INT16_VAL         
    { ASN_INT,           4       }, //INT32_VAL         
    { OCTET_STRING,      0xff    }, //BYTE_ARRAY        
    { OCTET_STRING,      0xff    }, //ASCII_ARRAY       
    { SNMP_IP_ADDR,      4       }, //IP_ADDRESS        
    { SNMP_COUNTER32,    4       }, //COUNTER32         
    { SNMP_TIME_TICKS,   4       }, //TIME_TICKS_VAL    
    { SNMP_GAUGE32,      4       }, //GAUTE32           
    { ASN_OID,           0xff    }  //OID_VAL           
};

/****************************************************************************
Function:
	BYTE _SNMPGet(void)

Summary:
	Read byte from snmp udp socket rx buffer.

Description:
	This function reads a single byte from the currently active UDP socket, 
	receive buffer, while incrementing the buffer offset from where the next 
	byte will be read.

Precondition:
	SNMPTask() is called.
	A active udp socket is available to read from.

Parameters:
	None

Returns:
	None.

Remarks:
	None.
***************************************************************************/
BYTE _SNMPGet(void)
{
	BYTE v;

	UDPSetRxBuffer(SNMPRxOffset++);
	UDPGet(&v);
	return v;
}

/****************************************************************************
  Function:
	BYTE IsValidLength(WORD* len)
	
  Summary:
	Retrieves the packet length and actual pdu length.

  Description:
	Checks current packet and returns total length value as well as
	actual length bytes.We do not support any length byte count of more
	than 2 i.e. total length value must not be more than 16-bit.
						
  Precondition:
	None 
	
  Parameters:
	len - Pointer to memory where actual length is stored.
 
  Return Values:
	lengthBytes - Total length bytes are 0x80 itself plus tempData.
		
  Remarks:
	None.
***************************************************************************/

BYTE IsValidLength(WORD *len)
{
	BYTE tempData;
	WORD_VAL tempLen;
	BYTE lengthBytes;

	// Initialize length value.
	tempLen.Val = 0;
	lengthBytes = 0;

	tempData = _SNMPGet();
	tempLen.v[0] = tempData;
	if ( tempData & 0x80 )
	{
		tempData &= 0x7F;

		// We do not support any length byte count of more than 2
		// i.e. total length value must not be more than 16-bit.
		if ( tempData > 2u )
			return FALSE;

		// Total length bytes are 0x80 itself plus tempData.
		lengthBytes = tempData + 1;

		// Get upto 2 bytes of length value.
		while( tempData-- )
			tempLen.v[tempData] = _SNMPGet();
	}
	else
		lengthBytes = 1;

	*len = tempLen.Val;

	return lengthBytes;
}


/****************************************************************************
  Function:
	static BOOL IsValidCommunity(char* community, BYTE* len)
	
  Summary:
	Verifies for the community string datatype and the max 
	community name and length, this agent can process.

  Description:
	This routine populates and validates the community datatype, community
	name and length from the received snmp request pdu. Community name is
	used for accessing public and private memebrs of the mib.
						
  Precondition:
	ProcessHeader() is called. 
	
  Parameters:
	community - Pointer to memory where community string will be stored.
	len 	  - Pointer to memory where comunity length gets stored.

  Return Values:
	TRUE	- If valid community received.
	FALSE	- If community is not valid.
	
  Remarks:
	None.
***************************************************************************/
static BOOL IsValidCommunity(char* community, BYTE* len)
{
	BYTE tempData;
	BYTE tempLen;

	tempData = _SNMPGet();
	if ( !IS_OCTET_STRING(tempData) )
		return FALSE;

	tempLen = _SNMPGet();
	*len	= tempLen;
	if ( tempLen > SNMP_COMMUNITY_MAX_LEN )
		return FALSE;

	while( tempLen-- )
	{
		tempData = _SNMPGet();
		*community++ = tempData;
	}
	*community = '\0';
	 return TRUE;
}


/****************************************************************************
  Function:
	BOOL IsValidInt(DWORD* val)
	
  Summary:
	Verifies variable datatype as INT and retrieves its value.

  Description:
	This routine populates and validates the received variable for the
	data type as "ASN_INT" and the data length for max 4 bytes.
						
  Precondition:
	ProcessHeader() or ProcessGetSetHeader() is called. 
	
  Parameters:
	val - Pointer to memory where int var value will be stored.
 
  ReturnValues:
	TRUE	- If valid integer type and value is received.
	FALSE	- Other than integer data type and value received .
	
  Remarks:
	None.
***************************************************************************/
BOOL IsValidInt(DWORD* val)
{
	DWORD_VAL tempData;
	DWORD_VAL tempLen;

	tempLen.Val = 0;

	// Get variable type
	if ( !IS_ASN_INT(_SNMPGet()) )
		return FALSE;

	if ( !IsValidLength(&tempLen.w[0]) )
		return FALSE;

	// Integer length of more than 32-bit is not supported.
	if ( tempLen.Val > 4u )
		return FALSE;

	tempData.Val = 0;
	while( tempLen.v[0]-- )
		tempData.v[tempLen.v[0]] = _SNMPGet();

	*val = tempData.Val;

	return TRUE;
}


/****************************************************************************
  Function:
	BOOL IsValidPDU(SNMP_ACTION* pdu)
	
  Summary:
	Verifies for the snmp request type.

  Description:
	This routine populates and verifies for the received snmp request 
	pdu type. 
						
  Precondition:
	ProcessHeader() is called. 
	
  Parameters:
	val - Pointer to memory where received snmp request type is stored.
 
  Return Values:
	TRUE	- If this snmp request can be processed by the agent. 
	FALSE	- If the request can not be processed.
	
  Remarks:
	None.
***************************************************************************/
BOOL IsValidPDU(SNMP_ACTION* pdu)
{
	BYTE tempData;
	WORD tempLen;


	// Fetch pdu data type
	tempData = _SNMPGet();
	if ( !IS_AGENT_PDU(tempData) )
		return FALSE;

	*pdu = tempData;


	/* Now fetch pdu length.  We don't need to remember pdu length.
	   Do this to proceed to next pdu element of interest*/ 
	return IsValidLength(&tempLen);
}



/****************************************************************************
  Function:
	BOOL IsASNNull(void)

	
  Summary:
	Verifies the value type as ASN_NULL.

  Description:
	For Get,Get_Next,Get_Bulk snmp reuest, the var bind the value data type 
	should be ASN_NULL and value field must be NULL and . This routine
	verifies the data type and value fields in the received requests.
	The SET request, the value data type can not be ASN_NULL,
	otherwise the snmp request is not processed.

  Precondition:
	None
	
  Parameters:
	None
 
  Returns Values
	TRUE	- If value type is ASN_NULL and value is NULL. 
	FALSE	- If data type and value is other than ASN_NULL and NULL resp.
	
  Remarks:
	None.
***************************************************************************/
static BOOL IsASNNull(void)
{
	BYTE a;

		
	// Fetch and verify that this is NULL data type.
	/*if ( !IS_ASN_NULL(_SNMPGet()) )
		return FALSE;*/

	a=_SNMPGet();

	if (!IS_ASN_NULL(a))
			return FALSE;

	// Fetch and verify that length value is zero.
	return (_SNMPGet() == 0u );
}


/****************************************************************************
  Function:
	BOOL IsValidOID(BYTE* oid, BYTE* len)
	
  Summary:
	Populates OID type, length and oid string from the received pdu.

  Description:
	In this routine, OID data type "ASN_OID" is verified in the received pdu.
	If the data type is matched, then only var bind is processed. OID length
	and OID is populated. The max OID length can be 15. 
	
  Precondition:
	ProcessVariabels() is called.
	
  Parameters:
	oid - Pointer to memory to store the received OID string
	len - Pointer to memory to store OID length
 
  Return Values:
	TRUE	- If value type is ASN_OID and oid length not more than 15. 
	FALSE	- Otherwise.
	
  Remarks:
	None.
***************************************************************************/
static BOOL IsValidOID(BYTE* oid, BYTE* len)
{
	DWORD_VAL tempLen;

	// Fetch and verify that this is OID.
	if ( !IS_OID(_SNMPGet()) )
		return FALSE;

	// Retrieve OID length
	if ( !IsValidLength(&tempLen.w[0]) )
		return FALSE;

	// Make sure that OID length is within our capability.
	if ( tempLen.w[0] > (WORD)SNMP_MAX_OID_LEN_MEM_USE)
		return FALSE;

	*len = tempLen.v[0];

	while( tempLen.v[0]-- )
	{
	   *oid++ = _SNMPGet();
	}
	*oid=0xff;
	return TRUE;
}

/****************************************************************************
  Function:
	BYTE IsValidStructure(WORD* dataLen)
	
  Summary:
	Decode variable length structure.

  Description:
	This routine is used  to verify whether the received varbind is of type
	STRUCTURE and to find out the variable binding structure length.
	
  Precondition:
	ProcessHeader() is called.	
	
  Parameters:
	datalen - Pointer to memory to store OID structure length.
 
  Return Values:
	headrbytes	- Variable binding length.
	FALSE		- If variable data structure is not type STRUCTURE. 	

  Remarks:
	None.
***************************************************************************/
BYTE IsValidStructure(WORD* dataLen)
{
	DWORD_VAL tempLen;
	BYTE headerBytes;

	if ( !IS_STRUCTURE(_SNMPGet()) )
		return FALSE;

	// Retrieve structure length
	headerBytes = IsValidLength(&tempLen.w[0]);
	if ( !headerBytes )
		return FALSE;

	headerBytes++;

	// Since we are using UDP as our transport and UDP are not fragmented,
	// this structure length cannot be more than 1500 bytes.
	// As a result, we will only use lower WORD of length value.
	*dataLen = tempLen.w[0];

	return headerBytes;
}

/****************************************************************************
  Function:
	void _SNMPDuplexInit(UDP_SOCKET socket)
	
  Summary:
	Prepare for full duplex transfer.

  Description:
	As we process SNMP variables, we will prepare response on-the-fly
	creating full duplex transfer. Current MAC layer does not support 
	full duplex transfer, so SNMP needs to manage its own full duplex
	connection. Prepare for full duplex transfer. Set the Tx and Rx 
	offset to start of the buffer.
	
  Precondition:
	SNMPTask() is called.	
	
  Parameters:
	socket - An active udp socket for which tx and rx offset to be set.
 
  Returns:
	None.

  Remarks:
	 This routine should be called for every new snmp packet received.
***************************************************************************/
void _SNMPDuplexInit(UDP_SOCKET socket)
{
	// In full duplex transfer, transport protocol must be ready to
	// accept new transmit packet.
	while( !UDPIsPutReady(socket) ) ;

	// Initialize buffer offsets.
	SNMPRxOffset = 0;
	SNMPTxOffset = 0;
}


/****************************************************************************
  Function:
	void _SNMPPut(BYTE v)
	
  Summary:
	Copy byte to tx buffer.

  Description:
	This function writes a single byte to the currently active UDP socket, 
	transmit buffer, while incrementing the buffer offset for the next write 
	operation.

  Precondition:
	SNMPTask() is called.	
	A active udp socket is availabe to tx from.
	
  Parameters:
	None.
 
  Returns:
	None.

  Remarks:
	None.
***************************************************************************/
void _SNMPPut(BYTE v)
{
	UDPSetTxBuffer(SNMPTxOffset);

	UDPPut(v);

	SNMPTxOffset++;
}



/****************************************************************************
  Function:
    void SNMPInit(void)

  Summary:
    Initialize SNMP module internals.

  Description:
  	This function initializes the Snmp agent. One udp socket is intialized 
  	and opened at port 161. Agent will receive and transmit all the snmp 
  	pdus on this udp socket. 
  	
  Precondition:
	At least one UDP socket must be available. UDPInit() is already called.

  Parameters:
	None

  Returns:
	None
    
  Remarks:
	This function is called only once during lifetime of the application.
	One UDP socket will be used.
 ***************************************************************************/
void SNMPInit(void)
{
    // Start with no error or flag set.
    SNMPStatus.Val = 0;

    //SNMPAgentSocket = UDPOpen(SNMP_AGENT_PORT, 0, INVALID_UDP_SOCKET);
    
	SNMPAgentSocket = UDPOpenEx(0,UDP_OPEN_SERVER,SNMP_AGENT_PORT,INVALID_UDP_PORT);
    // SNMPAgentSocket must not be INVALID_UDP_SOCKET.
    // If it is, compile time value of UDP Socket numbers must be increased.
#ifdef STACK_USE_SNMPV3_SERVER
	Snmpv3Init();
#endif
	
    return;
}

/****************************************************************************
  Function:
	BOOL GetDataTypeInfo(DATA_TYPE dataType, DATA_TYPE_INFO *info )
  Summary:
  	Get ASN data type info. 
  	
  Description:
  	This routine validates the SNMP GEt dtat type and collects ASN data type value to the
  	info.
  	
  Precondition:
	ProcessHeader() is called. 	
	
  Parameters:
    	dataType - SNMP pdu data type info.
    	info  - matching data type info w.r.t dataTypeTable
    
  Returns:
	TRUE -  success
	FALSE -  failure
	
  Remarks:
  	None.
***************************************************************************/
BOOL GetDataTypeInfo(DATA_TYPE dataType, DATA_TYPE_INFO *info )
{
    if ( dataType >= DATA_TYPE_UNKNOWN )
    {
		info->asnType   = 0x00;
	    info->asnLen    = 0x00;
		return FALSE;
	}

    info->asnType   = dataTypeTable[dataType].asnType;
    info->asnLen    = dataTypeTable[dataType].asnLen;

    return TRUE;
}



/****************************************************************************
  Function:
	BOOL SNMPTask(void)

  Summary:
	Polls for every snmp pdu received.

  Description:
	Handle incoming SNMP requests as well as any outgoing SNMP 
	responses and timeout conditions.
	
  Precondition:
	SNMPInit() is already called.

  Parameters:
	None

  Return Values:
	TRUE	-	If SNMP module has finished with a state
	FALSE	-	If a state has not been finished.
	
  Remarks:
	None
 ***************************************************************************/
BOOL SNMPTask(void)
{
    char community[SNMP_COMMUNITY_MAX_LEN+1];
    BYTE communityLen;
  	PDU_INFO pduInfoDB; //received pdu information database
    BOOL lbReturn=TRUE;

	if(SNMPAgentSocket == INVALID_UDP_SOCKET)
		return TRUE;

    // Check to see if there is any packet on SNMP Agent socket.
    if ( !UDPIsGetReady(SNMPAgentSocket) )
        return TRUE;

    // As we process SNMP variables, we will prepare response on-the-fly
    // creating full duplex transfer.
    // Prepare for full duplex transfer.
    _SNMPDuplexInit(SNMPAgentSocket);

	communityLen = 0;	// Suppress C30 warning: 'communityLen' may be used uninitialized in this function
    pduInfoDB.pduType = ProcessHeader(&pduInfoDB,community, &communityLen);

	if(pduInfoDB.snmpVersion != SNMP_V3)
	{
		if ( pduInfoDB.pduType == SNMP_ACTION_UNKNOWN )
		    goto _SNMPDiscard;

		if ( !ProcessGetSetHeader(&pduInfoDB))	
		    goto _SNMPDiscard;
	}

    // Open MIB file.
    SNMPStatus.Flags.bIsFileOpen = FALSE;

		
	
    hMPFS = MPFSOpenROM((ROM BYTE*)SNMP_BIB_FILE_NAME);

	
	if(hMPFS != MPFS_INVALID_HANDLE)
    {
       SNMPStatus.Flags.bIsFileOpen = TRUE;
    }

	if(pduInfoDB.snmpVersion != SNMP_V3) // if(SNMP_V1, SNMP_V2C)
	{
    	lbReturn = ProcessVariables(&pduInfoDB,community, communityLen);
	}

	#ifdef STACK_USE_SNMPV3_SERVER
	else
	{
		
		lbReturn = ProcessSnmpv3MsgData(&pduInfoDB);
	}
	#endif 
	
    if ( SNMPStatus.Flags.bIsFileOpen )
    {
       MPFSClose(hMPFS);
    }

    if ( lbReturn == FALSE )
        goto _SNMPDiscard;

#if !defined(SNMP_TRAP_DISABLED)
	if(gSendTrapFlag==(BYTE)FALSE)	
#endif		
   		UDPFlush();

	#ifdef STACK_USE_SNMPV3_SERVER
	Snmpv3FreeDynAllocMem();
	#endif 

    return TRUE;

_SNMPDiscard:
    UDPDiscard();

	#ifdef STACK_USE_SNMPV3_SERVER
	Snmpv3FreeDynAllocMem();
	#endif 
	
    return TRUE;
}


#if !defined(SNMP_TRAP_DISABLED)
/****************************************************************************
  Function:
	void SNMPNotifyPrepare(IP_ADDR* remoteHost,
                           char* community,
                           BYTE communityLen,
                           SNMP_ID agentIDVar,
                           BYTE notificationCode,
                           DWORD timestamp)

  Summary:
	Collects trap notification info and send ARP to remote host.

  Description:
	This function prepares SNMP module to send SNMP trap notification
	to remote host. It sends ARP request to remote host to learn remote
	host MAC address.
	
  Precondition:
	SNMPInit() is already called.

  Parameters:
	remoteHost  - pointer to remote Host IP address
	community   - Community string to use to notify
	communityLen- Community string length
	agentIDVar  - System ID to use identify this agent
	notificaitonCode - Notification Code to use
	timestamp   - Notification timestamp in 100th of second.

  Returns:
	None
		
  Remarks:
	This is first of series of functions to complete SNMP notification.
 ***************************************************************************/
void SNMPNotifyPrepare(IP_ADDR* remoteHost,
                        char* community,
                        BYTE communityLen,
                        SNMP_ID agentIDVar,
                        BYTE notificationCode,
                        DWORD timestamp )
{
	static IP_ADDR* remHostIpAddrPtr;
	remHostIpAddrPtr = remoteHost;
							
    strcpy(SNMPNotifyInfo.community, community);
    SNMPNotifyInfo.communityLen = communityLen;

    SNMPNotifyInfo.agentIDVar = agentIDVar;
    SNMPNotifyInfo.notificationCode = notificationCode;

    SNMPNotifyInfo.timestamp.Val = timestamp;
	SNMPNotifyInfo.socket = INVALID_UDP_SOCKET;

//    ARPResolve(remHostIpAddrPtr);
}


/****************************************************************************
  Function:
	BOOL SNMPIsNotifyReady(IP_ADDR* remoteHost)

  Summary:
	Resolves given remoteHost IP address into MAC address.
	
  Description:
  	This function resolves given remoteHost IP address into MAC address using 
  	ARP module. If remoteHost is not aviailable, this function would never 
  	return TRUE. Application must implement timeout logic to handle 
  	"remoteHost not avialable" situation.
	
  Precondition:
	SNMPNotifyPrepare() is already called.

  Parameters:
	remoteHost  - Pointer to remote Host IP address
      
  Return Values:
	TRUE	-	If remoteHost IP address is resolved and 
				SNMPNotify may be called.
    FALSE	-	If remoteHost IP address is not resolved.
    
  Remarks:
	This would fail if there were not UDP socket to open.
 ***************************************************************************/
BOOL SNMPIsNotifyReady(IP_ADDR* remoteHost)
{
    NODE_INFO remoteNode;

	IP_ADDR * remHostIpAddrPtr;
	remHostIpAddrPtr = remoteHost;
	remoteNode.IPAddr.Val = remHostIpAddrPtr->Val;
	
	if(SNMPNotifyInfo.socket == INVALID_UDP_SOCKET)
	{
		SNMPNotifyInfo.socket = UDPOpenEx(remoteNode.IPAddr.Val,UDP_OPEN_IP_ADDRESS,AGENT_NOTIFY_PORT,SNMP_NMS_PORT);
        //return (SNMPNotifyInfo.socket != INVALID_UDP_SOCKET);
	}

	if(UDPIsOpened(SNMPNotifyInfo.socket)!= TRUE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

#if 0
    if ( ARPIsResolved(remHostIpAddrPtr, &remoteNode.MACAddr) )
    {
        remoteNode.IPAddr.Val = remHostIpAddrPtr->Val;

        //SNMPNotifyInfo.socket = UDPOpen(AGENT_NOTIFY_PORT, &remoteNode, SNMP_NMS_PORT);
        
		SNMPNotifyInfo.socket = UDPOpenEx(remoteNode.IPAddr.Val,UDP_OPEN_IP_ADDRESS,AGENT_NOTIFY_PORT,SNMP_NMS_PORT);

        return (SNMPNotifyInfo.socket != INVALID_UDP_SOCKET);
    }
#endif

    return FALSE;
}

/****************************************************************************
  Function:
	BYTE *getSnmpV2GenTrapOid(BYTE generic_trap_code,BYTE *len)
  Summary:
	Resolves generic trap code to generic trap OID.
	
  Description:
  	This function resolves given generic trap code  to generic trap OID.
	
  Precondition:
	SNMPNotifyPrepare() is already called.

  Parameters:
	generic_trap_code  - GENERIC_TRAP_NOTIFICATION_TYPE
	len 	- generic trap OID length
      
  Return Values:
	BYTE *- TRAP OID
    
  Remarks:
	This would fail if generic_trap_code is not coming under 
	GENERIC_TRAP_NOTIFICATION_TYPE 
 ***************************************************************************/

BYTE *getSnmpV2GenTrapOid(BYTE generic_trap_code,BYTE *len)
{
    static  BYTE gen_trap_oid[] = {0x2b,6,1,6,3,1,1,5,1};
/*	
	static  BYTE cold_trap_oid[] = {0x2b,6,1,6,3,1,1,5,1};
    static  BYTE warm_start_oid = {0x2b,6,1,6,3,1,1,5,2};
    static  BYTE auth_fail_oid  = {0x2b,6,1,6,3,1,1,5,5};
    static  BYTE linkdown_oid   = {0x2b,6,1,6,3,1,1,5,3};
    static  BYTE linkup_oid     = {0x2b,6,1,6,3,1,1,5,4};
*/
	static BYTE snmptrap_oids[]  = {0x2b,6,1,6,3,1,1,4,1 }; 

	*len = sizeof(gen_trap_oid);
    switch (generic_trap_code) 
	{
      case COLD_START:
	  	  gen_trap_oid[*len-1] = 1;
        break;

      case WARM_START:
        gen_trap_oid[*len-1] = 2;
        break;
      case LINK_UP:
           gen_trap_oid[*len-1] = 4;
        break;
      case LINK_DOWN:
          gen_trap_oid[*len-1] = 3;
        break;
      case AUTH_FAILURE:
          gen_trap_oid[*len-1] = 5;
        break;
	case ENTERPRISE_SPECIFIC:
		*len = sizeof(snmptrap_oids);
		return snmptrap_oids;
      default:
          return NULL;

    } /* switch (generic_trap_code) */

    return gen_trap_oid;

} /* end getSnmpV2TrapOid() */



/****************************************************************************
  Function:
	BOOL SNMPNotify(SNMP_ID var,SNMP_VAL val,SNMP_INDEX index)

  Summary:
  	Creates and Sends TRAP pdu.
	
  Description:
	This function creates SNMP V2 Trap PDU and sends it to previously specified
	remoteHost.
	
	snmpv1 trap pdu:
       | PDU-type | enterprise | agent-addr | generic-trap | specific-trap |
       | time-stamp | varbind-list |

       The v1 enterprise is mapped directly to SNMPv2TrapOID.0
	SNMP v2 trap pdu:
       version (0 or 1) | community | SNMP-PDU |pdu-type | request-id | error-status 
       |err-index |varbinds

       The first two variables (in varbind-list) of snmpv2 are: sysUpTime.0 and
        SNMPv2TrapOID.0

        Generic Trap OID is used as the varbind for authentication failure.

        For ASCII STR trap VAL(argument) contains the pointer address of the string variable.

  Precondition:
 	SNMPIsNotifyReady() is already called and returned TRUE.

  Parameters:
	var     - SNMP var ID that is to be used in notification
	val     - Value of var. Only value of BYTE, WORD or DWORD can be sent.
	index   - Index of var. If this var is a single,index would be 0, or else 
			  if this var Is a sequence, index could be any value 
			  from 0 to 127
	  
  Return Values:
	TRUE	-	if SNMP notification was successful sent.
	   			This does not guarantee that remoteHost recieved it.
	FALSE	-	Notification sent failed.
  			    This would fail under following contions:
  			    1) Given SNMP_BIB_FILE does not exist in MPFS
  			    2) Given var does not exist.
  			    3) Previously given agentID does not exist
  			    4) Data type of given var is unknown - only
  			       possible if MPFS itself was corrupted.

  Remarks:
	This would fail if there were not UDP socket to open.
 ***************************************************************************/
#if defined(SNMP_STACK_USE_V2_TRAP)

BOOL SNMPNotify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index)
{
	char* pCommunity;
	BYTE len;
	BYTE OIDValue[OID_MAX_LEN];
	BYTE OIDLen;
	static DWORD varbindlen = 0;
	BYTE agentIDLen;
	BYTE* pOIDValue;
	static WORD packetStructLenOffset = 0;
	static WORD pduStructLenOffset = 0;
	static WORD varBindStructLenOffset = 0;
	static WORD varPairStructLenOffset = 0;
	static WORD prevOffset = 0;
	WORD tempOffset = 0;
	OID_INFO rec;
	DATA_TYPE_INFO dataTypeInfo;
	BYTE	snmptrap_oids[]  = {0x2b,6,1,6,3,1,1,4,1 }; /* len=10 */
	BYTE	sysUpTime_oids[] = {0x2b,6,1,2,1,1,3}; /* len = 8 */
	WORD_VAL trapVarBindLen={0};
	int i=0;
	hMPFS = MPFSOpenROM((ROM BYTE*)SNMP_BIB_FILE_NAME);
	if ( hMPFS == MPFS_INVALID_HANDLE )
	{
		UDPClose(SNMPNotifyInfo.socket);
		return FALSE;
	}
	
	if((packetStructLenOffset == 0)&&(pduStructLenOffset==0))
	{
		_SNMPDuplexInit(SNMPNotifyInfo.socket);
		prevOffset = _SNMPGetTxOffset();
		
		len = SNMPNotifyInfo.communityLen;
		pCommunity = SNMPNotifyInfo.community;

		_SNMPPut(STRUCTURE);			// First item is packet structure
		_SNMPPut(0x82);
		packetStructLenOffset = SNMPTxOffset;
		_SNMPPut(0);
		_SNMPPut(0);

		// Put SNMP version info.
		_SNMPPut(ASN_INT);				// Int type.
		_SNMPPut(1);					// One byte long value.
		_SNMPPut(SNMP_V2C); 		  // v2

		//len = strlen(community);	// Save community length for later use.
		_SNMPPut(OCTET_STRING); 		// Octet string type.
		_SNMPPut(len);					// community string length
		while( len-- )					// Copy entire string.
			_SNMPPut(*(pCommunity++));

		//TRAP Version type.  
		_SNMPPut(SNMP_V2_TRAP);
		_SNMPPut(0x82);
		pduStructLenOffset = SNMPTxOffset;
		_SNMPPut(0);
		_SNMPPut(0);

		//put Request ID for the trapv2 as 1 
		_SNMPPut(ASN_INT);	// Int type.
		_SNMPPut(4);		// To simplify logic, always use 4 byte long requestID
		_SNMPPut(0); _SNMPPut(0); _SNMPPut(0); _SNMPPut(1); 

		// Put error status.
		_SNMPPut(ASN_INT);				// Int type
		_SNMPPut(1);					// One byte long.
		_SNMPPut(0);					// Placeholder.

		// Similarly put error index.
		_SNMPPut(ASN_INT);				// Int type
		_SNMPPut(1);					// One byte long
		_SNMPPut(0);					// Placeholder.

		// Variable binding structure header
		_SNMPPut(0x30);
		_SNMPPut(0x82);
		varBindStructLenOffset = SNMPTxOffset;
		_SNMPPut(0);
		_SNMPPut(0);

		// Create variable name-pair structure
		_SNMPPut(0x30);
		varPairStructLenOffset = SNMPTxOffset;
		_SNMPPut(0);

		// Set 1st varbind object i,e sysUpTime.0 time stamp for the snmpv2 trap
		// Get complete notification variable OID string.

		_SNMPPut(ASN_OID);
		OIDLen = (BYTE)sizeof(sysUpTime_oids);
		_SNMPPut((BYTE)(OIDLen)+1);
		pOIDValue = sysUpTime_oids;
		while( OIDLen-- )
			_SNMPPut(*pOIDValue++);

		//1st varbind	 and this is a scalar object so index = 0
		_SNMPPut(0);

		// Time stamp
		_SNMPPut(SNMP_TIME_TICKS);
		_SNMPPut(4);
		_SNMPPut(SNMPNotifyInfo.timestamp.v[3]);
		_SNMPPut(SNMPNotifyInfo.timestamp.v[2]);
		_SNMPPut(SNMPNotifyInfo.timestamp.v[1]);
		_SNMPPut(SNMPNotifyInfo.timestamp.v[0]);

		tempOffset = _SNMPGetTxOffset();
		//set the snmp time varbind trap offset 
		_SNMPSetTxOffset(varPairStructLenOffset);

		// SNMP time stamp varbind length
		OIDLen = 2							// 1st varbind header 
		   + (BYTE)sizeof(sysUpTime_oids)
		   + 1						   // index byte
		   + 6 ;						// time stamp
		   
		_SNMPPut(OIDLen);
		//set the previous TX offset
		_SNMPSetTxOffset(tempOffset);
		varbindlen += OIDLen // varbind length
					+ 2;  // varbind type(30) and length of individual varbind pdu
					
		// Set 2nd varbind object i,e snmpTrapOID.0 for the snmpv2 trap
		// Get complete notification variable OID string.

		// Create variable name-pair structure
		_SNMPPut(0x30);
		varPairStructLenOffset = SNMPTxOffset;
		_SNMPPut(0);

		// Copy OID string into PDU.
		_SNMPPut(ASN_OID);
		OIDLen = (BYTE)sizeof(snmptrap_oids);
		_SNMPPut((BYTE)(OIDLen)+1);

		pOIDValue = snmptrap_oids;
		while( OIDLen-- )
		_SNMPPut(*pOIDValue++);

		//2nd varbind  and this is a scalar object so index = 0
		_SNMPPut(0);
		if ( !GetOIDStringByID(SNMPNotifyInfo.trapIDVar, &rec, OIDValue, &OIDLen) )
		{
			MPFSClose(hMPFS);
			UDPClose(SNMPNotifyInfo.socket);
			return FALSE;
		}
		_SNMPPut(ASN_OID);
		agentIDLen = OIDLen;
		len  = OIDLen;
		_SNMPPut(agentIDLen);
		for(i=0;i<len;i++)
		{
			_SNMPPut(OIDValue[i]);
		}
		tempOffset = _SNMPGetTxOffset();
		//set the snmp varbind trap offset
		_SNMPSetTxOffset(varPairStructLenOffset);
		// Snmp trap varbind length 
		OIDLen = 2					 // Agent ID header bytes
			+ (BYTE)sizeof(snmptrap_oids)
			+ 1 					   // index byte
			+ 2 					 // header
			+ agentIDLen;				 // Agent ID bytes				  
		_SNMPPut(OIDLen);

		//set the previous TX offset
		_SNMPSetTxOffset(tempOffset);
		varbindlen += OIDLen // varbind length
					+ 2;	 // varbind type(30) and length of individual varbind pdu
	
	}
	else
	{ // collect the last varbind offset value.
		_SNMPSetTxOffset(varPairStructLenOffset);
	}
	
	// Create variable name-pair structure
	_SNMPPut(0x30);
	_SNMPPut(0x82);
	varPairStructLenOffset = SNMPTxOffset;
	_SNMPPut(0);
	_SNMPPut(0);
	/* to send generic trap trap */
	if(gGenericTrapNotification != ENTERPRISE_SPECIFIC)
	{
		pOIDValue = getSnmpV2GenTrapOid(gGenericTrapNotification,&OIDLen);			
		if(pOIDValue == NULL)
		{
			MPFSClose(hMPFS);
			UDPClose(SNMPNotifyInfo.socket);
			return FALSE;
		}
		// Copy OID string into PDU.
		_SNMPPut(ASN_OID);
		_SNMPPut((BYTE)(OIDLen)+1);
		while( OIDLen-- )
		_SNMPPut(*pOIDValue++);

		//2nd varbind  and this is a scalar object so index = 0
		_SNMPPut(0);
		// for microchip , SNMPNotifyInfo.agentIDVar == MICROCHIP
		if ( !GetOIDStringByID(SNMPNotifyInfo.agentIDVar, &rec, OIDValue, &OIDLen) )
		{
			MPFSClose(hMPFS);
			UDPClose(SNMPNotifyInfo.socket);
			return FALSE;
		}
		if ( !rec.nodeInfo.Flags.bIsAgentID )
		{
			MPFSClose(hMPFS);
			UDPClose(SNMPNotifyInfo.socket);
			return FALSE;
		}

		MPFSSeek(hMPFS, rec.hData, MPFS_SEEK_START);

		_SNMPPut(ASN_OID);
		MPFSGet(hMPFS, &len);
		agentIDLen = len;
		_SNMPPut(agentIDLen);
		while( len-- )
		{
			BYTE c;
			MPFSGet(hMPFS, &c);
			_SNMPPut(c);
		}
		tempOffset = _SNMPGetTxOffset();
		//set the snmp varbind trap offset
		_SNMPSetTxOffset(varPairStructLenOffset);
		// Snmp trap varbind length 
		trapVarBindLen.Val = 2					 // Agent ID header bytes
			+ (BYTE)sizeof(snmptrap_oids)
			+ 1 					   // index byte
			+ 2 					 // header
			+ agentIDLen;				 // Agent ID bytes				  
		_SNMPPut(trapVarBindLen.v[1]);
		_SNMPPut(trapVarBindLen.v[0]);
		len = trapVarBindLen.Val;
	}
	else
	{
		// Get complete notification variable OID string.
		if ( !GetOIDStringByID(var, &rec, OIDValue, &OIDLen) )
		{
			MPFSClose(hMPFS);
			UDPClose(SNMPNotifyInfo.socket);
			return FALSE;
		}		
		pOIDValue = OIDValue;
	
		// Copy OID string into packet.
		_SNMPPut(ASN_OID);
		_SNMPPut((BYTE)(OIDLen+1));
		len = OIDLen;
		while( len-- )
			_SNMPPut(*pOIDValue++);
		_SNMPPut(index);

		// Encode and Copy actual data bytes
		if ( !GetDataTypeInfo(rec.dataType, &dataTypeInfo) )
		{
			MPFSClose(hMPFS);
			UDPClose(SNMPNotifyInfo.socket);
			return FALSE;
		}
		_SNMPPut(dataTypeInfo.asnType);
		 //Modified to Send trap even for  dataTypeInfo.asnType= ASCII_STRING, 
		//where dataTypeInfo.asnLen=0xff
		if ( dataTypeInfo.asnLen == 0xff )
		{
			UINT8 *asciiStr= (UINT8 *)(PTR_BASE) val.dword;
			int k=0;
			dataTypeInfo.asnLen=strlen((char *)asciiStr);
			len = dataTypeInfo.asnLen;
			_SNMPPut(len);
			for(k=0;k<len;k++)
				_SNMPPut(asciiStr[k]);
		}
		else
		{
			len = dataTypeInfo.asnLen;
	
			_SNMPPut(len);
			while( len-- )
				_SNMPPut(val.v[len]);
		}
	
		trapVarBindLen.Val = dataTypeInfo.asnLen	// data bytes count
			 + 1					// Length byte
			 + 1					// Data type byte
			 + OIDLen				// OID bytes
			 + 2					// OID header bytes
			 + 1;					// index byte
		tempOffset = _SNMPGetTxOffset();
		_SNMPSetTxOffset(varPairStructLenOffset);
		_SNMPPut(trapVarBindLen.v[1]);
		_SNMPPut(trapVarBindLen.v[0]);						 
	} 
	//set the previous TX offset
	_SNMPSetTxOffset(tempOffset);
	varPairStructLenOffset = tempOffset;
	
	varbindlen += trapVarBindLen.Val // length of varbind
				+4; // varbind type(30) and 0x82 , lenght1 and length2 of individual varbind pdu
	if(gSetTrapSendFlag == TRUE)
	{
		MPFSClose(hMPFS);
		return TRUE;
	}
	trapVarBindLen.Val = varbindlen;
	_SNMPSetTxOffset(varBindStructLenOffset);
	_SNMPPut(trapVarBindLen.v[1]);
	_SNMPPut(trapVarBindLen.v[0]);
	trapVarBindLen.Val = varbindlen
	+ 4 				   //  Variable Binding structure header(0x30,0x82,length1,length2)
	+ 12;					// req , error and error status for SNMPv2

	_SNMPSetTxOffset(pduStructLenOffset);
	_SNMPPut(trapVarBindLen.v[1]);
	_SNMPPut(trapVarBindLen.v[0]);


	trapVarBindLen.Val = trapVarBindLen.Val 						  // PDU struct length
	+ 4 							// PDU trap header
	+ SNMPNotifyInfo.communityLen			 // Community string bytes
	+ 2 							// Community header bytes
	+ 3;							// SNMP version bytes


	_SNMPSetTxOffset(packetStructLenOffset);
	_SNMPPut(trapVarBindLen.v[1]);
	_SNMPPut(trapVarBindLen.v[0]);

	_SNMPSetTxOffset(prevOffset);

// after setting all the offset values, initialize all static variables to 0.
	packetStructLenOffset = 0;
	pduStructLenOffset = 0;
	varBindStructLenOffset = 0;
	varPairStructLenOffset = 0;
	prevOffset = 0;
	varbindlen = 0;
	
	MPFSClose(hMPFS);
	UDPFlush();
	UDPClose(SNMPNotifyInfo.socket);

	return TRUE;
}


#else /* SNMP_STACK_USE_V2_TRAP */

/****************************************************************************
  Function:
	BOOL SNMPNotify(SNMP_ID var,SNMP_VAL val,SNMP_INDEX index)

  Summary:
  	Creates and Sends TRAP pdu.
	
  Description:
	This function creates SNMP trap PDU and sends it to previously specified
	remoteHost.
	snmpv1 trap pdu:
       | PDU-type | enterprise | agent-addr | generic-trap | specific-trap |
       | time-stamp | varbind-list |

       The v1 enterprise is mapped directly to SNMPv2TrapOID.0

       For ASCII STR trap VAL(argument) contains the pointer address of the string variable.
  Precondition:
	SNMPIsNotifyReady() is already called and returned TRUE.

  Parameters:
	var     - SNMP var ID that is to be used in notification
	val     - Value of var. Only value of BYTE, WORD or DWORD can be sent.
	index   - Index of var. If this var is a single,index would be 0, or else 
			  if this var Is a sequence, index could be any value 
			  from 0 to 127
	  
  Return Values:
	TRUE	-	if SNMP notification was successful sent.
	   			This does not guarantee that remoteHost recieved it.
	FALSE	-	Notification sent failed.
  			    This would fail under following contions:
  			    1) Given SNMP_BIB_FILE does not exist in MPFS
  			    2) Given var does not exist.
  			    3) Previously given agentID does not exist
  			    4) Data type of given var is unknown - only
  			       possible if MPFS itself was corrupted.

  Remarks:
	This would fail if there were not UDP socket to open.
 ***************************************************************************/
BOOL SNMPNotify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index)
{
	char* pCommunity;
    BYTE len;
    BYTE OIDValue[OID_MAX_LEN];
    BYTE OIDLen;
    BYTE agentIDLen;
    BYTE* pOIDValue;
    WORD packetStructLenOffset;
    WORD pduStructLenOffset;
    WORD varBindStructLenOffset;
    WORD varPairStructLenOffset;
    WORD prevOffset;
    OID_INFO rec;
	DATA_TYPE_INFO dataTypeInfo;

    hMPFS = MPFSOpenROM((ROM BYTE*)SNMP_BIB_FILE_NAME);
    if ( hMPFS == MPFS_INVALID_HANDLE )
    {
        UDPClose(SNMPNotifyInfo.socket);
        return FALSE;
    }

    _SNMPDuplexInit(SNMPNotifyInfo.socket);

    len = SNMPNotifyInfo.communityLen;
    pCommunity = SNMPNotifyInfo.community;

    _SNMPPut(STRUCTURE);            // First item is packet structure
    packetStructLenOffset = SNMPTxOffset;
    _SNMPPut(0);

    // Put SNMP version info.
    _SNMPPut(ASN_INT);              // Int type.
    _SNMPPut(1);                    // One byte long value.

	//Application has to decide which snmp version has to be 
	//updated to the notification pdu.
    _SNMPPut(SNMP_V1);              // v1.
    

    //len = strlen(community);  // Save community length for later use.
    _SNMPPut(OCTET_STRING);         // Octet string type.
    _SNMPPut(len);                  // community string length
    while( len-- )                  // Copy entire string.
        _SNMPPut(*(pCommunity++));

    // Put PDU type.  SNMP agent's response is always GET RESPONSE
    _SNMPPut(TRAP);
    pduStructLenOffset = SNMPTxOffset;
    _SNMPPut(0);

    // Get complete OID string from MPFS.
    if ( !GetOIDStringByID(SNMPNotifyInfo.agentIDVar,
                           &rec, OIDValue, &agentIDLen) )
    {
        MPFSClose(hMPFS);
        UDPClose(SNMPNotifyInfo.socket);
        return FALSE;
    }

    if ( !rec.nodeInfo.Flags.bIsAgentID )
    {
        MPFSClose(hMPFS);
        UDPClose(SNMPNotifyInfo.socket);
        return FALSE;
    }

    MPFSSeek(hMPFS, rec.hData, MPFS_SEEK_START);

    _SNMPPut(ASN_OID);
    MPFSGet(hMPFS, &len);
    agentIDLen = len;
    _SNMPPut(len);
    while( len-- )
    {
	    BYTE c;
	    MPFSGet(hMPFS, &c);
        _SNMPPut(c);
    }

    // This agent's IP address.
    _SNMPPut(SNMP_IP_ADDR);
    _SNMPPut(4);
    _SNMPPut(AppConfig.MyIPAddr.v[0]);
    _SNMPPut(AppConfig.MyIPAddr.v[1]);
    _SNMPPut(AppConfig.MyIPAddr.v[2]);
    _SNMPPut(AppConfig.MyIPAddr.v[3]);

	// Geberic/Enterprise Trap code
	 _SNMPPut(ASN_INT);
	 _SNMPPut(1);
	 _SNMPPut(gGenericTrapNotification); 

	// Specific Trap code
    _SNMPPut(ASN_INT);
    _SNMPPut(1);
    _SNMPPut(SNMPNotifyInfo.notificationCode);

    // Time stamp
    _SNMPPut(SNMP_TIME_TICKS);
    _SNMPPut(4);
    _SNMPPut(SNMPNotifyInfo.timestamp.v[3]);
    _SNMPPut(SNMPNotifyInfo.timestamp.v[2]);
    _SNMPPut(SNMPNotifyInfo.timestamp.v[1]);
    _SNMPPut(SNMPNotifyInfo.timestamp.v[0]);

    // Variable binding structure header
    _SNMPPut(0x30);
    varBindStructLenOffset = SNMPTxOffset;
    _SNMPPut(0);

    // Create variable name-pair structure
    _SNMPPut(0x30);
    varPairStructLenOffset = SNMPTxOffset;
	 _SNMPPut(0);
	 
    // Get complete notification variable OID string.
    if ( !GetOIDStringByID(var, &rec, OIDValue, &OIDLen) )
    {
        MPFSClose(hMPFS);
        UDPClose(SNMPNotifyInfo.socket);
        return FALSE;
    }

    // Copy OID string into packet.
    _SNMPPut(ASN_OID);
    _SNMPPut((BYTE)(OIDLen+1));
    len = OIDLen;
    pOIDValue = OIDValue;
    while( len-- )
        _SNMPPut(*pOIDValue++);
    _SNMPPut(index);

    // Encode and Copy actual data bytes
    if ( !GetDataTypeInfo(rec.dataType, &dataTypeInfo) )
    {
        MPFSClose(hMPFS);
        UDPClose(SNMPNotifyInfo.socket);
        return FALSE;
    }

    _SNMPPut(dataTypeInfo.asnType);


	//Modified to Send trap even for  dataTypeInfo.asnType= ASCII_STRING, 
	//where dataTypeInfo.asnLen=0xff
	if ( dataTypeInfo.asnLen == 0xff )
	{
		UINT8 *asciiStr= (UINT8 *)(PTR_BASE) val.dword;
		int k=0;
		dataTypeInfo.asnLen=strlen((char *)asciiStr);
		len = dataTypeInfo.asnLen;
		_SNMPPut(len);
		for(k=0;k<len;k++)
			_SNMPPut(asciiStr[k]);
	}
	else
	{
		len = dataTypeInfo.asnLen;
	
		_SNMPPut(len);
		while( len-- )
			_SNMPPut(val.v[len]);
	}

    len = dataTypeInfo.asnLen           // data bytes count
         + 1                            // Length byte
         + 1                            // Data type byte
         + OIDLen                       // OID bytes
         + 2                            // OID header bytes
         + 1;                           // index byte

    prevOffset = _SNMPGetTxOffset();
    _SNMPSetTxOffset(varPairStructLenOffset);
    _SNMPPut(len);

    len += 2;                           // Variable Binding structure header
    _SNMPSetTxOffset(varBindStructLenOffset);
    _SNMPPut(len);

    len = len
        + 2                             // Var bind struct header
        + 6                             // 6 bytes of timestamp
        + 3                             // 3 bytes of trap code
        + 3                             // 3 bytes of notification code
        + 6                             // 6 bytes of agnent IP address
        + agentIDLen                    // Agent ID bytes
        + 2;                                // Agent ID header bytes
    _SNMPSetTxOffset(pduStructLenOffset);
    _SNMPPut(len);

    len = len                           // PDU struct length
        + 2                             // PDU header
        + SNMPNotifyInfo.communityLen            // Community string bytes
        + 2                             // Community header bytes
        + 3;                            // SNMP version bytes
    _SNMPSetTxOffset(packetStructLenOffset);
    _SNMPPut(len);

    _SNMPSetTxOffset(prevOffset);

    MPFSClose(hMPFS);
    UDPFlush();
    UDPClose(SNMPNotifyInfo.socket);

    return TRUE;
}

#endif 

#endif // Code removed when SNMP_TRAP_DISABLED



/****************************************************************************
  Function:
	BOOL GetOIDStringByID(SNMP_ID id, OID_INFO* info, 
						  BYTE* oidString, BYTE* len)
	
  Summary:
  	Get complete notification variable OID string from MPFS using var id.
  	
  Description:
  	This routine is called when a OID string is required to be searched
  	from MPFS using agent id. The string is saved with agent.  
  	TRAP pdu is send with this OID corresponding to the SNMP_ID used 
  	by the agent application to send the pdu.
	
  Precondition:
		
  Parameters:
	id			-	System ID to use identify this agent.
	info		-	Pointer to SNMP MIB variable object information
	oidString	-	Pointer to store the string of OID serached
	len			-	Oid length
 
  Return Values:
	TRUE	-	If oid string is found for the variable id in MPFS.
	FLASE	-	Otherwise.

  Remarks:
  	This function is used only when TRAP is enabled.
***************************************************************************/
BOOL GetOIDStringByID(SNMP_ID id, OID_INFO* info, BYTE* oidString, BYTE* len)
{
    DWORD hCurrent;

    hCurrent = 0;

    while (1)
    {
    	//Read in the Mib record for the oid info
        ReadMIBRecord(hCurrent, info);

        if ( !info->nodeInfo.Flags.bIsParent )
        {
            if ( info->nodeInfo.Flags.bIsIDPresent )
            {
                if ( info->id == id )
                    return GetOIDStringByAddr(info, oidString, len);
            }

            if ( info->nodeInfo.Flags.bIsSibling ||
                 info->nodeInfo.Flags.bIsDistantSibling )
                MPFSSeek(hMPFS, info->hSibling, MPFS_SEEK_START);

            else
                break;

        }
        hCurrent = MPFSTell(hMPFS);
    }
    return FALSE;
}



/****************************************************************************
  Function:
	BOOL GetOIDStringByAddr(OID_INFO* rec, BYTE* oidString, BYTE* len)
	
  Summary:
  	Get OID string from MPFS using the node address.
  	
  Description:
  	This routine is called when a OID string is required to be searched
  	from MPFS using node address.
  	
  Precondition:
	None.
	
  Parameters:
	rec			-	Pointer to SNMP MIB variable object information
	oidString	-	Pointer to store the string of OID searched
	len			-	Oid length
 
  Return Values:
	TRUE	-	If oid string is found.
	FLASE	-	Otherwise.

  Remarks:
  	None.
***************************************************************************/
BOOL GetOIDStringByAddr(OID_INFO* rec, BYTE* oidString, BYTE* len)
{
    DWORD hTarget;
    DWORD hCurrent;
    DWORD hNext;
    OID_INFO currentMIB;
    BYTE index;
    enum { SM_PROBE_SIBLING, SM_PROBE_CHILD } state;

    hCurrent = 0;


    hTarget = rec->hNode;//node address
    state = SM_PROBE_SIBLING;
    index = 0;

    while( 1 )
    {
        ReadMIBRecord(hCurrent, &currentMIB);

        oidString[index] = currentMIB.oid;

        if ( hTarget == hCurrent )
        {
            *len = ++index;
            return TRUE;
        }

        switch(state)
        {
        case SM_PROBE_SIBLING:
            if ( !currentMIB.nodeInfo.Flags.bIsSibling )
                state = SM_PROBE_CHILD;

            else
            {
                hNext = currentMIB.hSibling;
                MPFSSeek(hMPFS, hNext, MPFS_SEEK_START);
                hNext = MPFSTell(hMPFS);
                if ( hTarget >= hNext )
                {
                    hCurrent = hNext;
                    break;
                }
                else
                    state = SM_PROBE_CHILD;
            }

        case SM_PROBE_CHILD:
            if ( !currentMIB.nodeInfo.Flags.bIsParent )
                return FALSE;

            index++;

            hCurrent = currentMIB.hChild;
            state = SM_PROBE_SIBLING;
            break;
        }
    }
    return FALSE;
}


/****************************************************************************
  Function:
	SNMP_ACTION ProcessHeader(PDU_INFO* pduDbPtr,
							  char* community, BYTE* len)

  Summary:
  	Validates the received udp packet Snmp header. 
	
  Description:
  	Collects PDU_INFO (SNMP pdu information database),community name,
  	community length and length of data payload. 
	This function validates the received udp packet for these different 
	variables of snmp pdu. The sequence in which these elements
	are received is important. The validation is done for the agent
	processing capabilities and the max UDP packet length as UDP packets
	can not be fragmented.
		
  Precondition:
	UDPIsGetReady(SNMPAgentSocket) is called in SNMPTask(),
	it check if there is any packet on SNMP Agent socket,
    should return TRUE.
    
  Parameters:
  	 pduDbPtr  - Pointer to received pdu information database
	 community - Pointer to var storing, community string in rxed pdu
	 len	   - Pointer to var storing, community string length rxed in pdu	
	 
  Return Values:
	SNMP_ACTION - Snmp request pdu type.
	
  Remarks:
	The received pdu will be processed only if this routine returns the
	pdu type else the pdu is discarded as not Snmp pdu.
 ***************************************************************************/

static SNMP_ACTION ProcessHeader(PDU_INFO* pduDbPtr, char* community, BYTE* len)
{
    DWORD_VAL tempLen; 
	SNMP_ACTION pdu=SNMP_ACTION_UNKNOWN;   
	UINT8 snmpMsgBuf[7];/* 0x30,0x81/0x82/length,0xlen,0xlen,0x02,0x01,0x03(Snmp Version)*/
	WORD_VAL snmpMsgLen;
	WORD retlen,tempCntr=0;
	UINT8* tempPtr;
#ifdef STACK_USE_SNMPV3_SERVER
	UINT8* inDataPtr;
	BYTE extraMemReqdFor16BytesBlocks;
#endif
	tempPtr=snmpMsgBuf;

	//Get complete StructureOF var binds info
	*snmpMsgBuf = _SNMPGet();
	tempCntr+=1;
	
	if(!IS_STRUCTURE(*snmpMsgBuf))
		 return SNMP_ACTION_UNKNOWN;

	 // Retrieve structure length
    retlen= IsValidLength(&tempLen.w[0]);
    if ( !retlen )
        return FALSE;

	if(retlen == 1)
	{
		tempCntr+=1;
		*(snmpMsgBuf+1)=tempLen.w[0];	
	}
	else if( retlen== 2)
	{	
		tempCntr+=2;
		*(snmpMsgBuf+1)=0x81; //BER encoding
		*(snmpMsgBuf+2)=tempLen.w[0];
		
	} 
	else if(retlen== 3)
	{
		tempCntr+=3;
		*(snmpMsgBuf+1)=0x82;//BER encoding
		*(snmpMsgBuf+2)=tempLen.v[1];	
		*(snmpMsgBuf+3)=tempLen.v[0];	
	}
	else
	{
		return SNMP_ACTION_UNKNOWN;
	}
	snmpMsgLen.Val=tempLen.w[0];
	

	//Get snmp version info ASN_INT (1 Byte) + Length (1 Byte)+ snmp Version 1 Byte

	 if ( !IsValidInt(&tempLen.Val) )
        return SNMP_ACTION_UNKNOWN;

	snmpMsgBuf[tempCntr]=ASN_INT;
		
	pduDbPtr->snmpVersion= tempLen.v[0];
    if ( (tempLen.v[0] != (BYTE)SNMP_V1) && ( tempLen.v[0] != (BYTE)SNMP_V2C )&&( tempLen.v[0] != (BYTE)SNMP_V3 ))
        return SNMP_ACTION_UNKNOWN;

	snmpMsgBuf[tempCntr+1]=0x01;
	snmpMsgBuf[tempCntr+2]=tempLen.v[0];

#ifdef STACK_USE_SNMPV3_SERVER


	//Valid snmp packet in the SNMP UDP Socket received 
	if(pduDbPtr->snmpVersion == (BYTE)SNMP_V3)
	{
		
		//Allocate dynamic memory to store the received SNMPv3 Whole Message.
		gSnmpV3InPduWholeMsgBuf.wholeMsgLen.Val=(snmpMsgLen.Val+tempCntr);
		extraMemReqdFor16BytesBlocks=16-(gSnmpV3InPduWholeMsgBuf.wholeMsgLen.Val%16);

		gSnmpV3InPduWholeMsgBuf.wholeMsgHead=(UINT8*)(malloc((size_t)gSnmpV3InPduWholeMsgBuf.wholeMsgLen.Val+extraMemReqdFor16BytesBlocks+16));
		if(gSnmpV3InPduWholeMsgBuf.wholeMsgHead == NULL)
			return SNMP_ACTION_UNKNOWN;
		gSnmpV3InPduWholeMsgBuf.snmpMsgHead=gSnmpV3InPduWholeMsgBuf.wholeMsgHead+tempCntr+3/*snmp Version info 0x02,0x01,0x03*/;
		gSnmpV3InPduWholeMsgBuf.snmpMsgLen.Val=snmpMsgLen.Val-3/*snmp Version info 0x02,0x01,0x03*/;


		//copy the WholeMsg structure info and snmp version info to dynamic mem from the UDP buffer. 
		//(WholeMsg is required to authenticate the received snmp pdu )//RFC 3414.
		tempCntr=tempCntr+3/*snmp Version info 0x02,0x01,0x03*/;
		inDataPtr=gSnmpV3InPduWholeMsgBuf.wholeMsgHead;
		while((tempCntr--)!=0)
		{
			*inDataPtr++=*tempPtr++;
		}

		inDataPtr=gSnmpV3InPduWholeMsgBuf.snmpMsgHead;
		tempCntr=gSnmpV3InPduWholeMsgBuf.snmpMsgLen.Val;

		while(tempCntr--)
		{
			*inDataPtr++=_SNMPGet();
	
		}
		
		if(Snmpv3MsgProcessingModelProcessPDU(SNMP_REQUEST_PDU) != SNMP_NO_ERR)
			pdu = SNMP_ACTION_UNKNOWN;
		if(Snmpv3UserSecurityModelProcessPDU(SNMP_REQUEST_PDU) != SNMP_NO_ERR)
			pdu = SNMP_ACTION_UNKNOWN;
		if(Snmpv3ScopedPduProcessing(SNMP_REQUEST_PDU)!= SNMP_NO_ERR)
			pdu = SNMP_ACTION_UNKNOWN;


		//Complete SNMPv3 data payload (Encrypted or as plain text) is received

		 //Check if received SNMPv3 message is Authenticated
		if((snmpSecurityLevel & 0x01)==0x01)
		{		
				//Message is authenticated
				if(Snmpv3AuthenticateRxedPduForDataIntegrity(&gSnmpV3InPduWholeMsgBuf)
					!= SNMPV3_MSG_AUTH_PASS)
				return SNMPV3_MSG_AUTH_FAIL;
		}	

		//Check if received SNMPv3 message is Encrypted.
		if((snmpSecurityLevel & 0x02)==0x02) 
		{
			 //Message is encrypted. Decrypt the message for processing
			 //user privacy protocol is AES

			if(Snmpv3AESDecryptRxedScopedPdu() != SNMPV3_MSG_PRIV_PASS)
				return SNMPV3_MSG_PRIV_FAIL;
		}	
	}

	else 
#endif	

	if((tempLen.v[0] == (BYTE)SNMP_V1)||(tempLen.v[0] == (BYTE)SNMP_V2C))
	{

	    // This function populates response as it processes community string.
	    if ( !IsValidCommunity(community, len) )
	        return SNMP_ACTION_UNKNOWN;

    	// Fetch and validate pdu type.  
    	if ( !IsValidPDU(&pdu) )
        return SNMP_ACTION_UNKNOWN;

		pduDbPtr->pduType = pdu;

		//Get_Bulk_Request is not defined in SNMP V1, hence discard udp request packet	
		if(pduDbPtr->snmpVersion==(BYTE)SNMP_V1 && pduDbPtr->pduType == GET_BULK_REQUEST)
			return SNMP_ACTION_UNKNOWN;
	
	    // Ask main application to verify community name against requested pdu type.
	    if(SNMPValidateCommunity((BYTE *)community)==(BYTE)INVALID_COMMUNITY)
	        return SNMP_ACTION_UNKNOWN;
	}
		
    return pdu;
}

/****************************************************************************
  Function:
	BOOL ProcessGetSetHeader(PDU_INFO* pduDbPtr)
	
  Summary:
  	Validates the received udp packet Get/Set request header. 
	
  Description:
	All the variables of snmp pdu request header are validated for their
	data types. Collects request_id for the snmp request pdu. Fetch,validates
	error status,error index and discard as they are need not to be processed
	as received in request pdu. Collects non repeaters and max repeaters
	values in case of Get_Bulk request.  
  	
  Precondition:
	ProcessHeader() is called and returns pdu type and do not returns 
	SNMP_ACTION_UNKNOWN
    
  Parameters:
  	pduDbPtr  - Pointer to received pdu information database.
  	
  Return Values:
	TRUE  - If the received request header is validated and passed.
	FALSE - If rxed request header is not valid.
	
  Remarks:
	The request pdu will be processed only if this routine returns TRUE
 ***************************************************************************/
static BOOL ProcessGetSetHeader(PDU_INFO* pduDbPtr)
{
    DWORD_VAL tempData;

    // Fetch and save request ID.
    if ( IsValidInt(&tempData.Val) )
         pduDbPtr->requestID.Val = tempData.Val;
    else
        return FALSE;

	if((pduDbPtr->snmpVersion == (BYTE)SNMP_V1 || pduDbPtr->snmpVersion == (BYTE)SNMP_V2C /*|| pduDbPtr->snmpVersion == (BYTE)SNMP_V3*/) &&(pduDbPtr->pduType != GET_BULK_REQUEST))
	{
	    // Fetch and discard error status
	    if ( !IsValidInt(&tempData.Val) )
	        return FALSE;

	    // Fetch and disacard error index
	    return IsValidInt(&tempData.Val);
	}
	else if((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C /*|| pduDbPtr->snmpVersion == (BYTE)SNMP_V3*/ )&& pduDbPtr->pduType == GET_BULK_REQUEST )
	{
		// Fetch non-repeaters value
		if ( IsValidInt(&tempData.Val) )
			 pduDbPtr->nonRepeators=tempData.v[0];
		else
			return FALSE;
			
		// Fetch Max-repetitions value
		if(IsValidInt(&tempData.Val))
			 pduDbPtr->maxRepetitions=(BYTE)tempData.v[0];
		else
			return FALSE;
	}
	else 
		return FALSE;
	
	return TRUE;
}

/****************************************************************************
  Function:
	BYTE ProcessSetVar(PDU_INFO* pduDbPtr,OID_INFO* rec,
					   SNMP_ERR_STATUS* errorStatus)
	
  Summary:
  	Processes snmp Set request pdu.
  	
  Description:
  	This routine processes the received snmp set request pdu for the 
  	variable binding in the request and also creates the response pdu.
  	
  Precondition:
	ProcessVariables() is called.
	
  Parameters:
    pduDbPtr	-   Received pdu information database pointer
    rec		  	-   Pointer to SNMP MIB variable object information
    errorStatus -   Pointer to update error status info

  Return Values:
	copiedBytes	- Number of bytes copied by this routine to the 
				  snmp pdu tx buffer.

  Remarks:
  	None.
***************************************************************************/
BYTE ProcessSetVar(PDU_INFO* pduDbPtr,OID_INFO* rec, SNMP_ERR_STATUS* errorStatus)
{
	BYTE ref;
	BYTE temp=0;
	BYTE dataType=0;
	BYTE dataLen=0;
	BYTE copiedBytes=0;
    SNMP_ERR_STATUS errorCode;
    DATA_TYPE_INFO actualDataTypeInfo;
    SNMP_VAL dataValue;
	
	#ifdef STACK_USE_SNMPV3_SERVER	
	SNMPV3MSGDATA	*dynPduBuf=NULL;
	dynPduBuf = &gSNMPv3ScopedPduResponseBuf;
    #endif
    // Start with no error.
    errorCode = SNMP_NO_ERR;
    copiedBytes = 0;

	

    // Non-leaf, Constant and ReadOnly node cannot be modified
    if(rec->nodeInfo.Flags.bIsParent   ||
       rec->nodeInfo.Flags.bIsConstant ||
       !rec->nodeInfo.Flags.bIsEditable )
    {	
        if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
       		errorCode = SNMP_NO_SUCH_NAME;
		else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C) ||
						(pduDbPtr->snmpVersion == (BYTE)SNMP_V3))
			errorCode = SNMP_NOT_WRITABLE;
	}

	if(pduDbPtr->snmpVersion != (BYTE)SNMP_V3)
	{
	    dataType = _SNMPGet();
	    _SNMPPut(dataType);
	}
	#ifdef STACK_USE_SNMPV3_SERVER	
	else
	{
	    dataType = Snmpv3GetBufferData(gSNMPv3ScopedPduRequestBuf,++gSNMPv3ScopedPduDataPos);	    
		Snmpv3BufferPut(dataType,dynPduBuf);
	}
	#endif
	copiedBytes++;

    // Get data type for this node.
    //actualDataType = MPFSGet();
    if ( !GetDataTypeInfo(rec->dataType, &actualDataTypeInfo) )
	{
		if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
        	errorCode = SNMP_BAD_VALUE;
		else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C) ||
					(pduDbPtr->snmpVersion == (BYTE) SNMP_V3))
			errorCode = SNMP_WRONG_TYPE;
	}

    // Make sure that received data type is same as what is declared
    // for this node.
    if ( dataType != actualDataTypeInfo.asnType )
	{	
        if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
        	errorCode = SNMP_BAD_VALUE;
		else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C) ||
					(pduDbPtr->snmpVersion == (BYTE) SNMP_V3))
			errorCode = SNMP_WRONG_TYPE;
	}

    // Make sure that received data length is within our capability.
	if(pduDbPtr->snmpVersion != (BYTE) SNMP_V3)
	{
	    dataLen = _SNMPGet();
	    _SNMPPut(dataLen);
	}
	#ifdef STACK_USE_SNMPV3_SERVER	
	else
	{
	    dataLen = Snmpv3GetBufferData(gSNMPv3ScopedPduRequestBuf,++gSNMPv3ScopedPduDataPos);	    
		Snmpv3BufferPut(dataLen,dynPduBuf);
	}
	#endif
    copiedBytes++;

    // Only max data length of 127 is supported.
    if ( dataLen > 0x7fu )
	{
		if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
        	errorCode = SNMP_BAD_VALUE;
		else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C)||
				(pduDbPtr->snmpVersion == (BYTE) SNMP_V3))
			errorCode = SNMP_WRONG_LENGTH;
	}

	
    // If this is a Simple variable and given index is other than '0',
    // it is considered bad value
    if ( !rec->nodeInfo.Flags.bIsSequence && rec->index != 0x00u ){
        errorCode = SNMP_NO_SUCH_NAME;}

    dataValue.dword = 0;
    ref = 0;

    // If data length is within 4 bytes, fetch all at once and pass it
    // to application.
    if ( actualDataTypeInfo.asnLen != 0xff )
    {
        // According to mib def., this data length for this data type/
        // must be less or equal to 4, if not, we don't know what this
        // is.
        if ( dataLen <= 4u )
        {
            // Now that we have verified data length, fetch them all
            // at once and save it in correct place.
            //dataLen--;

            while( dataLen-- )
            {
				if(pduDbPtr->snmpVersion != (BYTE) SNMP_V3)
				{
	                temp = _SNMPGet();
	                // Copy same byte back to create response...
	                _SNMPPut(temp);
				}
				#ifdef STACK_USE_SNMPV3_SERVER	
				else
				{
					temp = Snmpv3GetBufferData(gSNMPv3ScopedPduRequestBuf,++gSNMPv3ScopedPduDataPos);		
					Snmpv3BufferPut(temp,dynPduBuf);
				}
				#endif
				dataValue.v[dataLen] = temp;
                copiedBytes++;
            }


            // Pass it to application.
            if ( errorCode == SNMP_NO_ERR )
            {
                if(!SNMPSetVar(rec->id, rec->index, ref, dataValue))
				{
                   	if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
        				errorCode = SNMP_BAD_VALUE;
					else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C) ||
							(pduDbPtr->snmpVersion == (BYTE) SNMP_V3))
						errorCode = SNMP_WRONG_VALUE;
				}
            }
        }
        else
		{
            if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
        		errorCode = SNMP_BAD_VALUE;
			else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C)||
				(pduDbPtr->snmpVersion == (BYTE) SNMP_V3))
			{	
				if( rec->nodeInfo.Flags.bIsConstant)
					errorCode = SNMP_NOT_WRITABLE;
				else
					errorCode = SNMP_WRONG_LENGTH;
			}
		}	
    }
    else
    {
        // This is a multi-byte Set operation.
        // Check with application to see if this many bytes can be
        // written to current variable.
        if ( !SNMPIsValidSetLen(rec->id, dataLen,rec->index) )
		{
            if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
        		errorCode = SNMP_BAD_VALUE;
			else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C)  ||
				(pduDbPtr->snmpVersion == (BYTE) SNMP_V3))
			{
				if( rec->nodeInfo.Flags.bIsConstant)
					errorCode = SNMP_NOT_WRITABLE;
				else
					errorCode = SNMP_WRONG_LENGTH;
			}
		}
        // Even though there may have been error processing this
        // variable, we still need to reply with original data
        // so at least copy those bytes.
        while( dataLen-- )
        {
			if(pduDbPtr->snmpVersion != (BYTE) SNMP_V3)
			{
	            dataValue.byte = _SNMPGet();
	            _SNMPPut(dataValue.byte);
			}
			#ifdef STACK_USE_SNMPV3_SERVER	
			else
			{
				dataValue.byte = Snmpv3GetBufferData(gSNMPv3ScopedPduRequestBuf,++gSNMPv3ScopedPduDataPos);		
				Snmpv3BufferPut(dataValue.byte,dynPduBuf);
			}
			#endif
            copiedBytes++;

            // Ask applicaton to set this variable only if there was
            // no previous error.
            if ( errorCode == SNMP_NO_ERR )
            {
                if ( !SNMPSetVar(rec->id, rec->index, ref++, dataValue) )
				{
                    errorCode = SNMP_BAD_VALUE;
				}
            }
        }
        // Let application know about end of data transfer
        if ( errorCode == SNMP_NO_ERR )
		{
            if(!SNMPSetVar(rec->id, rec->index, (WORD)SNMP_END_OF_VAR, dataValue))
			{
                errorCode = SNMP_BAD_VALUE;
			}
		}
    }
    *errorStatus = errorCode;

    return copiedBytes;
}



/****************************************************************************
  Function:
	BYTE ProcessGetVar(OID_INFO* rec, BOOL bAsOID,PDU_INFO* pduDbPtr)
	
  Summary:
  	Processes snmp Get request pdu.
  	
  Description:
  	This routine processes the received snmp Get request pdu for the 
  	variable binding in the request and also creates the response pdu.
  	
  Precondition:
	ProcessVariables() is called.
	
  Parameters:
    rec		 -   Pointer to SNMP MIB variable object information
    bAsOID	 -   Oid flag.

  Return Values:
	varLen	- Number of bytes put in response tx pdu		
	FALSE	- If any of the elements of the request pdu validation fails.
	
  Remarks:
  	None.
***************************************************************************/
BYTE ProcessGetVar(OID_INFO* rec, BOOL bAsOID,PDU_INFO* pduDbPtr)
{
    BYTE ref;
    BYTE temp;
    BYTE varLen;
    BYTE dataType;
    WORD offset;
    WORD prevOffset;
    SNMP_VAL v;
    DATA_TYPE_INFO dataTypeInfo;
	#ifdef STACK_USE_SNMPV3_SERVER	
	SNMPV3MSGDATA	*dynPduBuf=NULL;
	dynPduBuf = &gSNMPv3ScopedPduResponseBuf;
	#endif
	
	offset = 0;	// Suppress C30 warning: 'offset' may be used uninitialized in this function
    v.dword   = 0;

    // Non-leaf node does not contain any data.
    if ( rec->nodeInfo.Flags.bIsParent )
        return FALSE;

    // If current OID is Simple variable and index is other than .0
    // we don't Get this variable.
    if ( !rec->nodeInfo.Flags.bIsSequence )
    {
        // index of other than '0' is not invalid.
        if ( rec->index > 0u )
            return FALSE;
    }

    dataType = rec->dataType;
    if ( !GetDataTypeInfo(dataType, &dataTypeInfo) )
        return FALSE;

    if ( !bAsOID )
    {
		if(pduDbPtr->snmpVersion != (BYTE) SNMP_V3)
		{
			_SNMPPut(dataTypeInfo.asnType);

	        offset = SNMPTxOffset;
	        _SNMPPut(dataTypeInfo.asnLen);
		}
		#ifdef STACK_USE_SNMPV3_SERVER	
		else
		{
			if(Snmpv3BufferPut(dataTypeInfo.asnType,dynPduBuf) != TRUE)
				return FALSE;
	        offset = dynPduBuf->length;
			if(Snmpv3BufferPut(dataTypeInfo.asnLen,dynPduBuf)!= TRUE)
				return FALSE;
		}
		#endif
    }

    if ( rec->nodeInfo.Flags.bIsConstant )
    {
        BYTE c;
        MPFSSeek(hMPFS, rec->hData, MPFS_SEEK_START);

        MPFSGet(hMPFS, &varLen);
        temp = varLen;
        while( temp-- )
	    {
	        MPFSGet(hMPFS, &c);
			if(pduDbPtr->snmpVersion != (BYTE) SNMP_V3)
            	_SNMPPut(c);
			#ifdef STACK_USE_SNMPV3_SERVER	
			else
				if(Snmpv3BufferPut(c,dynPduBuf) != TRUE)
					return FALSE;
			#endif	
        }
    }
    else
    {
        ref = SNMP_START_OF_VAR;
        v.dword = 0;
        varLen = 0;

        do
        {
            if ( SNMPGetVar(rec->id, rec->index, &ref, &v) )
            {
                if ( dataTypeInfo.asnLen != 0xff )
                {
                    varLen = dataTypeInfo.asnLen;

                    while( dataTypeInfo.asnLen )
                    {
						if(pduDbPtr->snmpVersion != (BYTE) SNMP_V3)
                      	 	_SNMPPut(v.v[--dataTypeInfo.asnLen]);
						#ifdef STACK_USE_SNMPV3_SERVER
						else
							if(Snmpv3BufferPut(v.v[--dataTypeInfo.asnLen],dynPduBuf)!=TRUE)
								return FALSE;
						#endif
                    }

                    break;
                }
                else
                {
                    varLen++;
					if(pduDbPtr->snmpVersion != (BYTE) SNMP_V3)
						_SNMPPut(v.v[0]);
					#ifdef STACK_USE_SNMPV3_SERVER
					else
						if(Snmpv3BufferPut(v.v[0],dynPduBuf) != TRUE)
							return FALSE;
					#endif
                }
            }
            else
            {
            	return FALSE;
            }

        } while( ref != SNMP_END_OF_VAR );
    }

    if ( !bAsOID )
    {

		 if(pduDbPtr->snmpVersion != SNMP_V3)
		 {
	         prevOffset = _SNMPGetTxOffset();

	        _SNMPSetTxOffset(offset);
	        _SNMPPut(varLen);

	        _SNMPSetTxOffset(prevOffset);
		 }
		 #ifdef STACK_USE_SNMPV3_SERVER	
		 else
	 	{
	 		prevOffset = dynPduBuf->length;
			dynPduBuf->length = offset;
			Snmpv3BufferPut(varLen,dynPduBuf);	
			dynPduBuf->length = prevOffset;
	 	}
		#endif
        varLen++;
        varLen++;
    }

    return varLen;
}


/****************************************************************************
  Function:
	BYTE ProcessGetNextVar(OID_INFO* rec,PDU_INFO* pduDbPtr)
	
  Summary:
  	Retrieves next node from the MIB database.  
  	
  Description:
  	This routine reads into the MIB stored with the agent in MPFS2 format.
  	It will search for the first lexicographic successor of the variable 
  	binding's name in the incoming GetNextRequest-PDU. If found, the 
  	corresponding variable binding's name and value fields in the Response 
  	pdu are set to the name and value of the located variable. If the 
  	lexicographic succesor is not found, the vlaue filed is set to
  	"endofMibView" and name field is retained as in request.
	  	
  Precondition:
	ProcessVariables is called.
	
  Parameters:
  	rec - Pointer to SNMP MIB object information for which next node 
  		  to be found
  	pduDbPtr - Pointer to received pdu information database
  Return Values:
	Value - Total number of bytes copied to response packet if succesful.	
	FALSE - If End of MIB is reached or processing is failure.
	
  Remarks:
	None.
 ***************************************************************************/
BYTE ProcessGetNextVar(OID_INFO* rec, PDU_INFO* pduDbPtr)
{
    WORD_VAL temp;
    BYTE putBytes=0;
    OID_INFO indexRec;
    BYTE *pOIDValue;
    BYTE OIDValue[OID_MAX_LEN];
    BYTE OIDLen;
    INDEX_INFO indexInfo;
    MIB_INFO varNodeInfo;
    SNMP_ID varID;
    WORD OIDValOffset=0;
    WORD prevOffset;
    BOOL lbNextLeaf;
    BYTE ref;
    static SNMP_VAL v;
    static BYTE varDataType;
    static BYTE indexBytes;
	BYTE idLen = 1;
	#ifdef STACK_USE_SNMPV3_SERVER	
	SNMPV3MSGDATA	*dynPduBuf=NULL;
	dynPduBuf = &gSNMPv3ScopedPduResponseBuf;
	#endif
	
	
    lbNextLeaf = FALSE;
    temp.v[0] = 0;

    // Get next leaf only if this OID is a parent or a simple leaf node.
    if ( rec->nodeInfo.Flags.bIsParent ||
       (!rec->nodeInfo.Flags.bIsParent && !rec->nodeInfo.Flags.bIsSequence) )
    {	
		_GetNextLeaf:
		lbNextLeaf = TRUE;
        if ( !GetNextLeaf(rec))
			return FALSE;
    }    
    // Get complete OID string from oid record.
    if ( !GetOIDStringByAddr(rec, OIDValue, &OIDLen))
	{	
        return FALSE;
	}

	//to validate the REC ID is present or not
	while(1)
	{
		if(SNMPIdRecrdValidation(pduDbPtr,rec,OIDValue,OIDLen) != TRUE)
		{
			if(!GetNextLeaf(rec))
				return FALSE;
			else
			{
				// Get complete OID string from oid record.
			    if ( !GetOIDStringByAddr(rec, OIDValue, &OIDLen))
				{	
			        return FALSE;
				}
			}
		}
		else
		{
			break;
		}
	}

    // Copy complete OID string to create response packet.
    pOIDValue = OIDValue;	
	temp.v[0] = OIDLen;
	if(pduDbPtr->snmpVersion != SNMP_V3)
	{
	    OIDValOffset = _SNMPGetTxOffset();
	    //temp.v[0] = OIDLen;
	    _SNMPSetTxOffset(OIDValOffset+1);
		while( temp.v[0]-- )
		_SNMPPut(*pOIDValue++);
	}
	#ifdef STACK_USE_SNMPV3_SERVER	
	else
	{
	    OIDValOffset = dynPduBuf->length;
	    //temp.v[0] = OIDLen;
	    //dynPduBuf.length = OIDValOffset+1;	// offset for the OID length	    
		if(Snmpv3BufferPut(0,dynPduBuf)!= TRUE)
			return FALSE;
		while( temp.v[0]-- )
			if(Snmpv3BufferPut(*pOIDValue++,dynPduBuf) != TRUE)
				return FALSE;
	}
	#endif

	//Put OID

    // Start counting number of bytes put - OIDLen is already counted.
    temp.v[0] = OIDLen;

    varDataType = rec->dataType;
    varID = rec->id;

    // If this is a simple OID, handle it as a GetVar command.
    if(!rec->nodeInfo.Flags.bIsSequence)
    {
    	if(pduDbPtr->snmpVersion != SNMP_V3)
		{
			// This is an addition to previously copied OID string.
		    // This is index value of '0'.
		    _SNMPPut(0);
		    temp.v[0]++;

		    // Since we added one more byte to previously copied OID
		    // string, we need to update OIDLen value.
		    prevOffset = _SNMPGetTxOffset();
		    _SNMPSetTxOffset(OIDValOffset);
		    _SNMPPut(++OIDLen);
		    _SNMPSetTxOffset(prevOffset);

		    // Now do Get on this simple variable.
		    prevOffset = _SNMPGetTxOffset();
		    putBytes = ProcessGetVar(rec, FALSE,pduDbPtr);
		    if ( putBytes == 0u )
		    {
		        _SNMPSetTxOffset(prevOffset);
		        _SNMPPut(ASN_NULL);
		        _SNMPPut(0);
		        putBytes = 2;
		    }
		}
		#ifdef STACK_USE_SNMPV3_SERVER	
		else
		{
		    // This is index value of '0'.
			if(Snmpv3BufferPut(0,dynPduBuf) != TRUE)
				return FALSE;
		    temp.v[0]++;
		    prevOffset = dynPduBuf->length;
		    dynPduBuf->length = OIDValOffset;
		    // Since we added one more byte to previously copied OID
		    // string, we need to update OIDLen value.
			if(Snmpv3BufferPut(++OIDLen,dynPduBuf)!= TRUE)
				return FALSE;
		    dynPduBuf->length = prevOffset;
			
		    // Now do Get on this simple variable.
		    prevOffset = dynPduBuf->length;
		    putBytes = ProcessGetVar(rec, FALSE,pduDbPtr);
			if(dynPduBuf->length >= dynPduBuf->maxlength)
				return FALSE;
			if(( putBytes == 0u ) && (pduDbPtr->snmpVersion == SNMP_V3))
			{
				dynPduBuf->length = prevOffset;
				if(Snmpv3BufferPut(ASN_NULL,dynPduBuf)!= TRUE)
					return FALSE;
				if(Snmpv3BufferPut(0,dynPduBuf)!= TRUE)
					return FALSE;
				putBytes = 2;
			}
		}
		#endif
        temp.v[0] += putBytes; // ProcessGetVar(rec, FALSE,pduDbPtr);

         // Return with total number of bytes copied to response packet.
        return temp.v[0];
    }

    // This is a sequence variable.
    // First of all make sure that there is a next index after this
    // index.  We also need to make sure that we do not do this foerever.
    // So make sure that this is not a repeat test.
    ref = 0;
    if ( lbNextLeaf == TRUE )
    {
         // Let application tell us whether this is a valid index or not.
        if ( !SNMPGetVar(rec->id, rec->index, &ref, &v) )
        {
             // If not, then we need to get next leaf in line.
            // Remember that we have already did this once, so that we do not
            // do this forever.
            //lbNextSequence = TRUE;

            // Reset the response packet pointer to begining of OID.
           // _SNMPSetTxOffset(OIDValOffset);
		   #ifdef STACK_USE_SNMPV3_SERVER	
             if(pduDbPtr->snmpVersion == SNMP_V3)
			 	dynPduBuf->length = OIDValOffset;
			 else
		   #endif
	            _SNMPSetTxOffset(OIDValOffset);

            // Jump to this label within this function - Not a good SW engineering
            // practice, but this will reuse code at much lower expense.
            goto _GetNextLeaf;
        }
    }

    // Need to fetch index information from MIB and prepare complete OID+
    // index response.
     varNodeInfo.Val = rec->nodeInfo.Val;

    // In this version, only 7-bit index is supported.
    MPFSGet(hMPFS, NULL);

    indexBytes = 0;

    MPFSGet(hMPFS, &indexInfo.Val);

    // Fetch index ID.
    MPFSGet(hMPFS,&idLen);
    if(idLen == 1)
    {
        BYTE temp;
        MPFSGet(hMPFS,&temp);
        indexRec.id = temp & 0xFF;
        //MPFSGet(hMPFS,(BYTE *)&indexRec.id);
    }
    else if(idLen == 2)
    {
        BYTE temp[2];
        MPFSGetArray(hMPFS,temp,2);
        indexRec.id = 0;
        indexRec.id = temp[0] & 0xFF;
        indexRec.id <<= 8;
        indexRec.id |= temp[1] & 0xFF;
    }

	// Fetch index data type.
    indexRec.dataType = 0;
    MPFSGet(hMPFS, (BYTE*)&indexRec.dataType);

    indexRec.index = rec->index;

    // Check with application to see if there exists next index
    // for this index id.
    if ( !lbNextLeaf && !SNMPGetNextIndex(indexRec.id, &indexRec.index) )
    {
        //lbNextSeqeuence = TRUE;
         // Reset the response packet pointer to begining of OID.
       // _SNMPSetTxOffset(OIDValOffset);
#ifdef STACK_USE_SNMPV3_SERVER	
		if(pduDbPtr->snmpVersion == SNMP_V3)
		   dynPduBuf->length = OIDValOffset;
		else
#endif
		   _SNMPSetTxOffset(OIDValOffset);
		
        // Jump to this label.  Not a good practice, but once-in-a-while
        // it should be acceptable !
        goto _GetNextLeaf;
    }

    // Index is assumed to be dynamic, and leaf node.
    // mib2bib has already ensured that this was the case.
    indexRec.nodeInfo.Flags.bIsConstant = 0;
    indexRec.nodeInfo.Flags.bIsParent = 0;
    indexRec.nodeInfo.Flags.bIsSequence = 1;

    // Now handle this as simple GetVar.
    // Keep track of number of bytes added to OID.
    indexBytes += ProcessGetVar(&indexRec, TRUE,pduDbPtr);

    rec->index = indexRec.index;

    // These are the total number of bytes put so far as a result of this function.
    temp.v[0] += indexBytes;

    // These are the total number of bytes in OID string including index bytes.
    OIDLen += indexBytes;

	if(pduDbPtr->snmpVersion != SNMP_V3)
	{
	    // Since we added index bytes to previously copied OID
	    // string, we need to update OIDLen value.
	    prevOffset = _SNMPGetTxOffset();
	    _SNMPSetTxOffset(OIDValOffset);
	    _SNMPPut(OIDLen);
	    _SNMPSetTxOffset(prevOffset);
	}
#ifdef STACK_USE_SNMPV3_SERVER	
	else
	{
	    // Since we added index bytes to previously copied OID
	    // string, we need to update OIDLen value.
	    prevOffset = dynPduBuf->length;
	    dynPduBuf->length = OIDValOffset;
	    Snmpv3BufferPut(OIDLen,dynPduBuf);
		dynPduBuf->length = prevOffset;
	}
#endif

    // Fetch actual value itself.
    // Need to restore original OID value.
    rec->nodeInfo.Val = varNodeInfo.Val;
    rec->id = varID;
    rec->dataType = varDataType;

    temp.v[0] += ProcessGetVar(rec, FALSE,pduDbPtr);
     return temp.v[0];
}



/****************************************************************************
  Function:
	BYTE ProcessGetBulkVar(OID_INFO* rec, BYTE* oidValuePtr, 
						   BYTE* oidLenPtr,BYTE* successor)	

  Summary:
  	This routine process the SNMPv2c Get Bulk Request.
  	
  Description:
	ProcessVariables() processes the received snmp request pdu for each of
	the variable binding in the variable binding list to produce a response 
	pdu. Depending on the number of the Max_repetitions for every variable
	in the list for which Bulk information is expected, ProcessGetBulkVar()
	is executed. It searches for the next lexicographically ordered 
	successor for of the OID received in the request. For each of the 
	iterations upto max-repetitions, the next leaf node is searched in the
	MIB to that of the leaf node found in the last iteration, for the 
	corresponding variable binding.
 		  	
  Precondition:
	ProcessVariables() is called.
	
  Parameters:
  	rec 		- Pointer to SNMP MIB variable object information OID 
  	oidValuePtr	- Pointer to new node OID found in MIB 
  	oidLenPtr	- Oid length
  	successor	- 'I'th lexicographic successor to be found value
  	  	
  Return Values:
	FALSE 	  - If no lexicographic successor found
	temp.v[0] - Total number of bytes copied to response packet
	
  Remarks:
	None.
***************************************************************************/
BYTE ProcessGetBulkVar(OID_INFO* rec, BYTE* oidValuePtr, BYTE* oidLenPtr,BYTE* successor,PDU_INFO* pduDbPtr)
{
    
    BYTE ref,putBytes,cntr;
    BYTE OIDLen;
	static BYTE varDataType;
    static BYTE indexBytes;
	BYTE sequenceCnt=0;
	BYTE sequenceRepeatCnt=0;	
	BOOL lbNextLeaf;
	SNMP_ID varID;
	OID_INFO indexRec;	
	INDEX_INFO indexInfo;
    MIB_INFO varNodeInfo;
 	WORD OIDValOffset;
	WORD varBindOffset=0;
    WORD prevOffset;	
	WORD_VAL temp;
	static SNMP_VAL v;
	BYTE idLen=1;
	BYTE *prevOIDVlaue;
	#ifdef STACK_USE_SNMPV3_SERVER	
	SNMPV3MSGDATA	*dynPduBuf=NULL;
	dynPduBuf = &gSNMPv3ScopedPduResponseBuf;
	#endif
	
	
	/* intialize the local variables to 0 */
	OIDLen=0;
	sequenceCnt=0;
	sequenceRepeatCnt=0;
	varID=0;
	OIDValOffset=0;
	varBindOffset=0;
	prevOffset=0;
	temp.Val=0;
	
    lbNextLeaf = FALSE;
    temp.v[0] = 0;
	sequenceRepeatCnt=*successor;

	prevOIDVlaue = oidValuePtr;
	
	//Reach to the node for the expected iteration
	for(cntr=1;cntr<=*successor;cntr++)
	{
    	// Get next leaf only if this OID is a parent or a simple leaf node.
    	if((rec->nodeInfo.Flags.bIsParent)|| 
        (!rec->nodeInfo.Flags.bIsParent && !rec->nodeInfo.Flags.bIsSequence)) 
		{	/* to maintain the number of interations */
			sequenceCnt++;
		   	_GetNextLeaf:
	        lbNextLeaf = TRUE;
	        if(!GetNextLeaf(rec))
				return FALSE;
		}
	}
	
	/* If request OID is a sequence variable, the below for loop retrives the 
	expected instance for the OID. SequenceRepeatCnt starts with "0th instance" and 
	increments to Max repeatations. Find the exact indexed OID in the request at first. 
	If indexed OID is not available, then go for the next index. 
	If the next index is not available , then go to the next leaf.
	*/
	if(rec->nodeInfo.Flags.bIsSequence)
	{
		for(;sequenceCnt<=sequenceRepeatCnt;sequenceCnt++)
		{
			if(!SNMPGetExactIndex(rec->id, rec->index))
			{
				if(!SNMPGetNextIndex(rec->id,&rec->index))
				{
		        	if(!GetNextLeaf(rec))
						return FALSE;
				}
			}
			if(sequenceRepeatCnt>sequenceCnt)
				rec->index+=1;
		}
	}	

    // Get complete OID string from oid record.
    if(!GetOIDStringByAddr(rec, oidValuePtr, &OIDLen))
        return FALSE;

	//to validate the REC ID is present or not
	while(1)
	{
		if(SNMPIdRecrdValidation(pduDbPtr,rec,oidValuePtr,OIDLen) != TRUE)
		{
			if(!GetNextLeaf(rec))
				return FALSE;
			else
			{
				// Get complete OID string from oid record.
			    if(!GetOIDStringByAddr(rec, oidValuePtr, &OIDLen))
			        return FALSE;
			}
		}
		else
		{
			break;
		}
	}
	
	#ifdef STACK_USE_SNMPV3_SERVER	
	if(pduDbPtr->snmpVersion == SNMP_V3)
	{
		varBindOffset = dynPduBuf->length;
		if(Snmpv3BufferPut(ASN_OID,dynPduBuf) != TRUE)
			return FALSE;

		OIDValOffset = dynPduBuf->length;
		temp.v[0] = OIDLen;
	    //_SNMPSetTxOffset(OIDValOffset+1);
		if(Snmpv3BufferPut(0,dynPduBuf) != TRUE)
			return FALSE;
		//Put OID
		while( temp.v[0]-- )
		{
			//_SNMPPut(*oidValuePtr);	
			if(Snmpv3BufferPut(*oidValuePtr,dynPduBuf) != TRUE)
				return FALSE;
			oidValuePtr++;
		}
	}
	else
	#endif
	{
		varBindOffset = _SNMPGetTxOffset();
	    _SNMPPut(ASN_OID);

		OIDValOffset = _SNMPGetTxOffset();
		temp.v[0] = OIDLen;
	    _SNMPSetTxOffset(OIDValOffset+1);

		//Put OID
		while( temp.v[0]-- )
		{
			_SNMPPut(*oidValuePtr);		
			oidValuePtr++;
		}
	}
	// Start counting number of bytes put - OIDLen is already counted.
	temp.v[0] =*oidLenPtr= OIDLen;

    varDataType = rec->dataType;
    varID = rec->id;

    // If this is a simple OID, handle it as a GetVar command.
    if ( !rec->nodeInfo.Flags.bIsSequence )
    {
		#ifdef STACK_USE_SNMPV3_SERVER	
    	if(pduDbPtr->snmpVersion == SNMP_V3)
    	{
	         // This is an addition to previously copied OID string.
	        // This is index value of '0'.
			 if(Snmpv3BufferPut(0,dynPduBuf) != TRUE)
				 return FALSE;
	        temp.v[0]++;

	        // Since we added one more byte to previously copied OID
	        // string, we need to update OIDLen value.
	        prevOffset = dynPduBuf->length;
	        dynPduBuf->length = OIDValOffset;
			if(Snmpv3BufferPut(++OIDLen,dynPduBuf) != TRUE)
				return FALSE;
	        dynPduBuf->length = prevOffset;
	       

	        // Now do Get on this simple variable.
	        prevOffset = dynPduBuf->length;
	        putBytes = ProcessGetVar(rec, FALSE,pduDbPtr);
    	}
		else
		#endif
		{
			 // This is an addition to previously copied OID string.
			// This is index value of '0'.
			_SNMPPut(0);
			temp.v[0]++;
		
			// Since we added one more byte to previously copied OID
			// string, we need to update OIDLen value.
			prevOffset = _SNMPGetTxOffset();
			_SNMPSetTxOffset(OIDValOffset);
			_SNMPPut(++OIDLen);
			_SNMPSetTxOffset(prevOffset);
		
			// Now do Get on this simple variable.
			prevOffset = _SNMPGetTxOffset();
			putBytes = ProcessGetVar(rec, FALSE,pduDbPtr);
		}
        temp.v[0] += putBytes; // ProcessGetVar(rec, FALSE,pduDbPtr);
   
	    // Return with total number of bytes copied to response packet.
        return temp.v[0];
    }
	
    ref = 0;
    if ( lbNextLeaf == TRUE )
    {
    	// Let application tell us whether this is a valid index or not.
        if ( !SNMPGetVar(rec->id, rec->index, &ref, &v) )
        {
             // Reset the response packet pointer to begining of OID.
        #ifdef STACK_USE_SNMPV3_SERVER	
		     if(pduDbPtr->snmpVersion == SNMP_V3)
			 	dynPduBuf->length = varBindOffset;
			 else
		#endif
	            _SNMPSetTxOffset(varBindOffset);

            // Jump to this label within this function - Not a good SW engineering
            // practice, but this will reuse code at much lower expense.
            
			// Get the Exact OIDValuePtr location.
			oidValuePtr = prevOIDVlaue;
            goto _GetNextLeaf;
        }
    }

	// Need to fetch index information from MIB and prepare complete OID+
    // index response.
    varNodeInfo.Val = rec->nodeInfo.Val;

    // In this version, only 7-bit index is supported.
    MPFSGet(hMPFS, NULL);

    indexBytes = 0;
	 // Fetch index ID.
    	

    MPFSGet(hMPFS, &indexInfo.Val);

    // Fetch index ID.
    MPFSGet(hMPFS,&idLen);
    if(idLen == 1)
    {
        BYTE temp;
        MPFSGet(hMPFS,&temp);
        indexRec.id = temp & 0xFF;
        //MPFSGet(hMPFS,(BYTE *)&indexRec.id);
    }
    else if(idLen == 2)
    {
        BYTE temp[2];
        MPFSGetArray(hMPFS, temp,2);
        indexRec.id = 0;
        indexRec.id = temp[0] & 0xFF;
        indexRec.id <<= 8;
        indexRec.id |= temp[1] & 0xFF;
    }
   // MPFSGet(hMPFS, &indexRec.id);
    // Fetch index data type.
    indexRec.dataType = 0;
    MPFSGet(hMPFS, (BYTE*)&indexRec.dataType);

    indexRec.index = rec->index;
#if 0
    // Check with application to see if there exists next index
    // for this index id.
   if ( !lbNextLeaf && !SNMPGetNextIndex(indexRec.id, &indexRec.index) )
   {   
		//lbNextSeqeuence = TRUE;

	   // Reset the response packet pointer to begining of OID.
	   _SNMPSetTxOffset(OIDValOffset);

	   // Jump to this label.  Not a good practice, but once-in-a-while
	   // it should be acceptable !
	   
	   goto _GetNextLeaf;
   }

#endif
    // Index is assumed to be dynamic, and leaf node.
    // mib2bib has already ensured that this was the case.
    indexRec.nodeInfo.Flags.bIsConstant = 0;
    indexRec.nodeInfo.Flags.bIsParent = 0;
    indexRec.nodeInfo.Flags.bIsSequence = 1;

    // Now handle this as simple GetVar.
    // Keep track of number of bytes added to OID.
    indexBytes += ProcessGetVar(&indexRec, TRUE,pduDbPtr);

    rec->index = indexRec.index;

    // These are the total number of bytes put so far as a result of this function.
    temp.v[0] += indexBytes;

    // These are the total number of bytes in OID string including index bytes.
    OIDLen += indexBytes;

	if(pduDbPtr->snmpVersion != SNMP_V3)
	{
		// Since we added index bytes to previously copied OID
		// string, we need to update OIDLen value.
		prevOffset = _SNMPGetTxOffset();
		_SNMPSetTxOffset(OIDValOffset);
		_SNMPPut(OIDLen);
		_SNMPSetTxOffset(prevOffset);
	}
	#ifdef STACK_USE_SNMPV3_SERVER	
	else
	{
		// Since we added index bytes to previously copied OID
		// string, we need to update OIDLen value.
		prevOffset = dynPduBuf->length;
		dynPduBuf->length =OIDValOffset;
		Snmpv3BufferPut(OIDLen,dynPduBuf);		
		dynPduBuf->length = prevOffset;
	}
	#endif
    // Fetch actual value itself.
    // Need to restore original OID value.
    rec->nodeInfo.Val = varNodeInfo.Val;
    rec->id = varID;
    rec->dataType = varDataType;

    temp.v[0] += ProcessGetVar(rec, FALSE,pduDbPtr);
	
	return temp.v[0];
}

/****************************************************************************
  Function:
	BYTE OIDLookup(PDU_INFO* pduDbPtr,BYTE* oid, BYTE oidLen, OID_INFO* rec)
	
  Summary:
  	To search and validate whether the requested OID is in the MIB database.
  	
  Description:
	The MIB database is stored with the agent in binary mib format.
  	This is the binary mib format:
  	<oid, nodeInfo, [id], [SiblingOffset], [DistantSibling], [dataType],
  	[dataLen], [data], [{IndexCount, <IndexType>, <Index>, ...>]}, ChildNode
	variable bind name is a dotted string of oid. Every oid is a node in the
	MIB tree and have varied information. This routine on reception of the 
	snmp request, will search for every oid in the var name. This routine 
	will return information whether the requested var name is part of the 
	MIB tree data structre of this agent or not.
	 		  	
  Precondition:
	Valid snmp request with valid OID format is received.
	
  Parameters:
  	pduDbPtr	- Pointer to received snmp  pdu elements information 
  	oid			- Pointer to the string of OID to be searched
  	oidLen		- Oid length
  	rec			- Pointer to SNMP MIB variable object information  
  	  	
  Return Values:
	TRUE	-	If the complete OID string is found in the mib
	FALSE	-   If complete OID do not match. 
				Also different erros returned are
				SNMP_END_OF_MIB_VIEW
				SNMP_NO_SUCH_NAME
				SNMP_NO_SUCH_OBJ
				SNMP_NO_SUCH_INSTANCE
  Remarks:
	This routine works for the MPFS2 mib storage format. It uses the MPFS2
	APIs to read,search and collect information from the mib database. 
***************************************************************************/
BYTE OIDLookup(PDU_INFO* pduDbPtr,BYTE* oid, BYTE oidLen, OID_INFO* rec)
{
	BYTE idLen=1;
	BYTE savedOID,tempSavedOID;
	BYTE matchedCount;
	BYTE snmpVer;
	BYTE snmpReqType;
	BYTE tempLen;
	BYTE* reqOidPtr;
	BYTE comapreOidWithSibling=FALSE;

	WORD_VAL tempData;
    DWORD hNode,tempHNode;//    MPFS hNode;

	appendZeroToOID=TRUE;

	snmpVer=pduDbPtr->snmpVersion;
	snmpReqType=pduDbPtr->pduType;

    if(!SNMPStatus.Flags.bIsFileOpen )
	   return FALSE;
	
	hNode = 0;
    matchedCount = oidLen;

	tempLen=oidLen;
	reqOidPtr=oid;

    while( 1 )
    {
    
		MPFSSeek(hMPFS, hNode, MPFS_SEEK_START);

        // Remember offset of this node so that we can find its sibling
        // and child data.
        rec->hNode = MPFSTell(hMPFS); // hNode;

        // Read OID byte.
        MPFSGet(hMPFS, &savedOID);

		if(comapreOidWithSibling==(BYTE)FALSE)
		{
		   	tempSavedOID=savedOID;
			tempHNode=hNode;
		}

        // Read Node Info
        MPFSGet(hMPFS, &rec->nodeInfo.Val);

	    // Next byte will be node id, if this is a leaf node with variable data.
        if(rec->nodeInfo.Flags.bIsIDPresent)
        {
        	MPFSGet(hMPFS,&idLen);
			if(idLen == 1)
			{
                            BYTE temp;
                            MPFSGet(hMPFS,&temp);
                            rec->id = temp & 0xFF;
                           //MPFSGet(hMPFS,(BYTE *)&rec->id);
			}
			else if(idLen == 2)
			{
				BYTE temp[2];
				MPFSGetArray(hMPFS, temp,2);
				rec->id = 0;
				rec->id = temp[0] & 0xFF;
				rec->id <<= 8;
				rec->id |= temp[1] & 0xFF;				
			}				
        }
	
        // Read sibling offset, if there is any.
        if(rec->nodeInfo.Flags.bIsSibling)
        {
            MPFSGet(hMPFS, &tempData.v[0]);
            MPFSGet(hMPFS, &tempData.v[1]);
            rec->hSibling = tempData.Val;
        }

        if ( savedOID != *reqOidPtr )
        {
        	/*if very first OID byte does not match, it may be because it is
              0, 1 or 2.  In that case declare that there is a match.
              The command processor would detect OID type and continue or reject
              this OID as a valid argument.*/
            if(matchedCount == oidLen)
				goto FoundIt;
			
			if(comapreOidWithSibling==(BYTE)TRUE && !rec->nodeInfo.Flags.bIsSibling)
				goto DidNotFindIt;

			if ( rec->nodeInfo.Flags.bIsSibling )
            {
 	            hNode = MPFSSeek(hMPFS, tempData.Val, MPFS_SEEK_START);
	            hNode = MPFSTell(hMPFS);
				comapreOidWithSibling=TRUE;
            }
            else
	            goto DidNotFindIt;
        }
        else
        {
	        // One more oid byte matched.
            matchedCount--;
            reqOidPtr++;

            // A node is said to be matched if last matched node is a leaf node
            // or all but last OID string is matched and last byte of OID is '0'.
            // i.e. single index.
            if ( !rec->nodeInfo.Flags.bIsParent )
            {
            	// Read and discard Distant Sibling info if there is any.
                if ( rec->nodeInfo.Flags.bIsDistantSibling )
                {	
                   	MPFSGet(hMPFS, &tempData.v[0]);
                    MPFSGet(hMPFS, &tempData.v[1]);
                    rec->hSibling = tempData.Val;
                }

		        rec->dataType = 0;
                MPFSGet(hMPFS, (BYTE*)&rec->dataType);
                rec->hData = MPFSTell(hMPFS);

				if(snmpReqType==SNMP_GET && matchedCount == 0u)
				{
					appendZeroToOID=FALSE;
					goto DidNotFindIt;
                 }
				else if(snmpReqType==(BYTE)SNMP_GET 
					&& matchedCount == 1u && *reqOidPtr == 0x00u)
				{
					appendZeroToOID=FALSE;
                 }
				else if(snmpReqType==SNMP_GET_NEXT && matchedCount == 0u)
				{
					appendZeroToOID=TRUE;
					getZeroInstance=TRUE;
					
				}
				else if(snmpReqType==(BYTE)SNMP_V2C_GET_BULK && matchedCount == 1u )
				{
					appendZeroToOID=FALSE;
				}
                goto FoundIt;
            }
            else if(matchedCount == 1u && *reqOidPtr == 0x00u)
	        {
	            appendZeroToOID=FALSE;

				if(rec->nodeInfo.Flags.bIsParent)
				{	
					goto DidNotFindIt;
	            }

			}
            else if(matchedCount == 0u)
            {
	           	if(rec->nodeInfo.Flags.bIsParent || snmpReqType==SNMP_GET)
    			{
					appendZeroToOID=FALSE;
					
					goto DidNotFindIt;
                 }
				 else 
				 	goto FoundIt;
			}
            else
            {	
	            hNode = MPFSTell(hMPFS);
                // Try to match following child node.
                continue;
            }
        }
    }

FoundIt:

	// Convert index info from OID to regular value format.
  	rec->index = savedOID;

	/*To Reach To The Next leaf Node */
    savedOID = *reqOidPtr;
	
	rec->indexLen = 1;

	if(matchedCount ==1u)
	{
		rec->index = *reqOidPtr;
	}
	else if(matchedCount == 0u)
	{
		rec->index = 0;
	}
	else if ( matchedCount > 1u || savedOID & 0x80 /*In this version, we only support 7-bit index*/)
    {	
    	// Current instnace spans across more than 7-bit.
        rec->indexLen = 0xff;

		if(snmpReqType==SNMP_GET && snmpVer==(BYTE)SNMP_V1)
		{
			return SNMP_NO_SUCH_NAME;
		}
		else if(snmpReqType==SNMP_GET && snmpVer==(BYTE)SNMP_V2C)
		{
			if(matchedCount== oidLen) //No OBJECT IDNETIFIER Prefix match
				return SNMP_NO_SUCH_INSTANCE;
			else
				return SNMP_NO_SUCH_OBJ;
		}

		return FALSE;
    }

	if(getZeroInstance)
	{
		rec->index = SNMP_INDEX_INVALID;	
		// Here to get the first available index with initializing rec->index to SNMP_INDEX_INVALID
		if(!SNMPGetNextIndex(rec->id, &rec->index))
		{
			rec->index = 0;
		}
		
	}

	return TRUE;

DidNotFindIt:

	if(snmpReqType==SNMP_GET)
	{
		if(snmpVer==(BYTE)SNMP_V1)
			return SNMP_NO_SUCH_NAME;

		else if(snmpVer==(BYTE)SNMP_V2C)
		{	
			if(matchedCount== oidLen) //No OBJECT IDNETIFIER Prefix match
				return SNMP_NO_SUCH_INSTANCE;
			else
				return SNMP_NO_SUCH_OBJ;
		}
	}
	else if((snmpReqType==SNMP_GET_NEXT||snmpReqType==SNMP_V2C_GET_BULK) && snmpVer==(BYTE)SNMP_V2C)
	{
		return SNMP_END_OF_MIB_VIEW;
	}
	
	return FALSE;
}	


/****************************************************************************
  Function:
	BOOL GetNextLeaf(OID_INFO* rec)
	
  Summary:
  	Searches for the next leaf node in the MIP tree.  	

  Description:
  	This routine searches for the next leaf node from the current node.
  	The input to this function is the node from where next leaf node
  	is to be located. The next leaf node will be a silbing else distant 
  	sibling or leaf node of next branch, if any present. The input parameter
  	var pointer will be updated with the newly found leaf node OID info.
 		 		  	
  Precondition:
	ProcessGetBulkVar() else ProcessGetNextVar() is called. 
	
  Parameters:
  	rec		- Pointer to SNMP MIB variable object information  

  Return Values:
	TRUE	- If next leaf node is found.
	FALSE	- There is no next leaf node.
	
  Remarks:
	None.
***************************************************************************/
BOOL GetNextLeaf(OID_INFO* rec)
{
    WORD_VAL temp;
	BYTE idLen=1; 	 

    // If current node is leaf, its next sibling (near or distant) is the next leaf.
    if ( !rec->nodeInfo.Flags.bIsParent )
    {
        // Since this is a leaf node, it must have at least one distant or near
        // sibling to get next sibling.
        if(rec->nodeInfo.Flags.bIsSibling ||
           rec->nodeInfo.Flags.bIsDistantSibling )
        {
            // Reposition at sibling.
            MPFSSeek(hMPFS, rec->hSibling, MPFS_SEEK_START);

            // Fetch node related information
        }
        // There is no sibling to this leaf.  This must be the very last node on the tree.
        else
        {
			//--MPFSClose();
            return FALSE;
        }
    }

    while( 1 )
    {
        // Remember current MPFS position for this node.
       rec->hNode = MPFSTell(hMPFS);

        // Read OID byte.
        MPFSGet(hMPFS, &rec->oid);

        // Read Node Info
        MPFSGet(hMPFS, &rec->nodeInfo.Val);

        // Next byte will be node id, if this is a leaf node with variable data.
       
        if ( rec->nodeInfo.Flags.bIsIDPresent )
		{
			 // Fetch index ID.
		    MPFSGet(hMPFS,&idLen);
		    if(idLen == 1)  
                    {
                        BYTE temp;
                       	MPFSGet(hMPFS,&temp);
                        rec->id = temp & 0xFF;
                    }
			else if(idLen == 2)
			{
				BYTE temp[2];
				MPFSGetArray(hMPFS, temp,2);
				rec->id = 0;
				rec->id = temp[0] & 0xFF;
				rec->id <<= 8;
				rec->id |= temp[1] & 0xFF;				
			}		
            //MPFSGet(hMPFS,(BYTE *) &rec->id);
		}

        // Fetch sibling offset, if there is any.
        if ( rec->nodeInfo.Flags.bIsSibling ||
             rec->nodeInfo.Flags.bIsDistantSibling )
        {
            MPFSGet(hMPFS, &temp.v[0]);
            MPFSGet(hMPFS, &temp.v[1]);
            rec->hSibling = temp.Val;
        }

        // If we have not reached a leaf yet, continue fetching next child in line.
        if ( rec->nodeInfo.Flags.bIsParent )
        {
            continue;
        }

        // Fetch data type.
        rec->dataType = 0;
        MPFSGet(hMPFS, (BYTE*)&rec->dataType);

        rec->hData = MPFSTell(hMPFS);

        // Since we just found next leaf in line, it will always have zero index
        // to it.
        rec->indexLen = 1;
        rec->index = 0;
		if (rec->nodeInfo.Flags.bIsSequence)
	    {
		   rec->index = SNMP_INDEX_INVALID;		   
		   // Here to get the first available index with initializing rec->index to SNMP_INDEX_INVALID
		   if(!SNMPGetNextIndex(rec->id, &rec->index))
			   rec->index = 0;
	    }
        return TRUE;
    }
    return FALSE;
}


/****************************************************************************
  Function:
	void ReadMIBRecord(DWORD h, OID_INFO* rec)
	
  Summary:
  	Get OID string from MPFS using the node address.
  	
  Description:
  	This routine is called when a OID string is required to be searched
  	from MPFS using node address.
  	
  Precondition:
	GetOIDStringByID() or GetOIDStringByAddr() is called.
	
  Parameters:
	h		-	Node adderess whose oid is to be read.
	rec		-	Pointer to store SNMP MIB variable object information
  
  Returns:
	None.
	
  Remarks:
  	None.
***************************************************************************/
static void ReadMIBRecord(DWORD h, OID_INFO* rec)
{
    MIB_INFO nodeInfo;
    WORD_VAL tempVal;
	BYTE idLen=1;

    MPFSSeek(hMPFS, h, MPFS_SEEK_START);

    // Remember location of this record.
    rec->hNode = h;

    // Read OID
    MPFSGet(hMPFS, &rec->oid);

    // Read nodeInfo
    MPFSGet(hMPFS, &rec->nodeInfo.Val);
    nodeInfo = rec->nodeInfo;

    // Read id, if there is any: Only leaf node with dynamic data will have id.
    if ( nodeInfo.Flags.bIsIDPresent )
    {
    	 // Fetch index ID.
	    MPFSGet(hMPFS,&idLen);
	    if(idLen == 1)
	    {
	    	BYTE temp=0;
	    	MPFSGet(hMPFS,&temp);
			rec->id = temp & 0xFF;			
	    }
		else if(idLen == 2)
		{
			BYTE temp[2];
			MPFSGetArray(hMPFS, temp,2);
			rec->id = 0;
			rec->id = temp[0] & 0xFF;
			rec->id <<= 8;
			rec->id |= temp[1] & 0xFF;				
		}		
        //MPFSGet(hMPFS, &rec->id);
    }
    // Read Sibling offset if there is any - any node may have sibling
    if ( nodeInfo.Flags.bIsSibling )
    {
        MPFSGet(hMPFS, &tempVal.v[0]);
        MPFSGet(hMPFS, &tempVal.v[1]);
        rec->hSibling = tempVal.Val;
    }

    // All rest of the parameters are applicable to leaf node only.
    if ( nodeInfo.Flags.bIsParent )
        rec->hChild = MPFSTell(hMPFS);
    else
    {
        if ( nodeInfo.Flags.bIsDistantSibling )
        {
            // Read Distant Sibling if there is any - only leaf node will have distant sibling
            MPFSGet(hMPFS, &tempVal.v[0]);
            MPFSGet(hMPFS, &tempVal.v[1]);
            rec->hSibling = tempVal.Val;
        }

        // Save data type for this node.
        rec->dataType = 0;
        MPFSGet(hMPFS, (BYTE*)&rec->dataType);

        rec->hData = MPFSTell(hMPFS);

    }

}

/****************************************************************************
  Function:
	void SetErrorStatus(WORD errorStatusOffset,
                           WORD errorIndexOffset,
                           SNMP_ERR_STATUS errorStatus,
                           BYTE errorIndex)
  Summary:
  	Set snmp error status in the response pdu. 
  	
  Description:
  	This routine processes the received snmp Get request pdu for the 
  	variable binding in the request and also creates the response pdu.
  	
  Precondition:
	ProcessVariables() is called.	
	
  Parameters:
    errorStatusOffset - Offset to update error status in Response Tx pdu 	
    errorIndexOffset  - Offset to update error index
    errorStatus		  - Snmp process error to be updated in response.	
    errorIndex		  - Index of the request varbind in the var bind list 
    					for which error status is to be updated. 					
    
  Returns:
	None.
	
  Remarks:
  	None.
***************************************************************************/
void SetErrorStatus(WORD errorStatusOffset,
                           WORD errorIndexOffset,
                           SNMP_ERR_STATUS errorStatus,
                           BYTE errorIndex)
{
    WORD prevOffset;

    prevOffset = _SNMPGetTxOffset();

    _SNMPSetTxOffset(errorStatusOffset);
    _SNMPPut((BYTE)errorStatus);

    _SNMPSetTxOffset(errorIndexOffset);
    _SNMPPut(errorIndex);

    _SNMPSetTxOffset(prevOffset);
}


/****************************************************************************
  Function:
	BYTE FindOIDsInRequest(WORD pdulen)
	
  Summary:
  	Finds number of varbinds in the varbind list received in a pdu.
  	
  Description:
  	This routine is used to find the number of OIDs requested in the received
  	snmp pdu.   	
  	
  Precondition	:
	ProcessVariables() is called.	
		
  Parameters:
	pdulen		-	Length of snmp pdu request received. 
    
  Return Values:
	varCount	-	Number of OIDs found in a pdu request. 
	
  Remarks:
  	None.
***************************************************************************/
static BYTE FindOIDsInRequest(WORD pdulen)
{
BYTE  structureLen;
BYTE varCount=0;
WORD prevUDPRxOffset;
WORD varBindLen;
WORD snmpPduLen;
 	
	snmpPduLen=pdulen;

	prevUDPRxOffset=SNMPRxOffset;

	while(snmpPduLen)
	{
		
		structureLen = IsValidStructure(&varBindLen);
		if(!structureLen)
		  return FALSE;
	//	if(!IsValidStructure(&varBindLen))
		//	return FALSE;

		SNMPRxOffset=SNMPRxOffset+varBindLen;
		varCount++;
		snmpPduLen=snmpPduLen 
			- structureLen // 1 byte for STRUCTURE identifier + 0x82 or ox81+1+1 byte(s) for varbind length 
			- varBindLen;
					//-1 //1 byte for STRUCTURE identifier
					//-1//1 byte for varbind length 
				//	-varBindLen;
	}

	SNMPRxOffset=prevUDPRxOffset;

	return varCount;
}

/****************************************************************************
  Function:
	BOOL SNMPCheckIfPvtMibObjRequested(BYTE* OIDValuePtr)
	
  Summary:
  	To find whether requested OID is only for private access.
  	
  Description:
  	This routine is used to find whether requested object belongs
  	to the private object group of the mib of agent. If yes, then
  	that mib object can be accessed only with private community 
  	(supported in SNMPv2c). 
  	
  Precondition	:
	ProcessVariables() is called.	
		
  Parameters:
	OIDValuePtr	-	Pointer to memory stored with received OID.
    
  Return Values:
	TRUE	-	If the requested object is of private branch of the mib.
	FLASE	-	If the requested object is publically accessible.
	
  Remarks:
  	None.
***************************************************************************/
static BOOL SNMPCheckIfPvtMibObjRequested(BYTE* OIDValuePtr)
{
	BYTE cnt=0;
	BYTE pvtObjIdentifier[4]={0x2b,0x06/*dod*/,0x01/*internet*/,0x04/*private*/};

	while(cnt<4u)
	{
		//check whether requested oid is for pvt obj
		if(pvtObjIdentifier[cnt]== OIDValuePtr[cnt])
		{
			cnt++;
		}
		else
		{
			cnt=0;
			return FALSE;
		}
		if(cnt == 0x04u)
			return TRUE;
	}
	return FALSE;

}

#ifdef STACK_USE_SNMPV3_SERVER	
/****************************************************************************
  Function:
	void Snmpv3Init(void)
	
  Summary:
  	SNMPv3 initialization.
	
  Description:
  	This routine will intialize SNMPv3 global data base ,engine ID  and USM
	
  Precondition:
	At least one UDP socket must be available. UDPInit() is already called.
	
  Parameters:
  	None
	
  Return Values:
	None
	
  Remarks:
	None
 ***************************************************************************/

void Snmpv3Init(void)
{
UINT8 userDBIndex,fifthOctectIdentifier;

	Snmpv3InitializeUserDataBase();

	fifthOctectIdentifier=MAC_ADDR_ENGN_ID;
	
	Snmpv3FormulateEngineID(fifthOctectIdentifier);


	for(userDBIndex=0;userDBIndex<SNMPV3_USM_MAX_USER;userDBIndex++)
	{	
		Snmpv3UsmSnmpEngnAuthPrivPswdLocalization(userDBIndex);
		Snmpv3ComputeHMACIpadOpadForAuthLoclzedKey(userDBIndex);

	}

}
#endif


/****************************************************************************
  Function:
	static BOOL ProcessVariables(PDU_INFO* pduDbPtr,char* community, BYTE len)
	
  Summary:
  	This routine processes the snmp request and parallely creates the 
  	response pdu.
	
  Description:
  	Once the received pdu is validated as Snmp pdu, it is forwarded for 
  	processing to this routine. This rotuine handles Get, Get_Next, Get_Bulk,
  	Set request and creates appropriate response as Get_Response. 
  	This routine will decide on whether the request pdu should be processed
  	or be discarded.
	
  Precondition:
	The received udp packet is varified as SNMP request.
	ProcessHeader() and ProcessGetSetHeader() returns but FALSE.
	
  Parameters:
  	pduDbPtr  - Pointer to received pdu information database
    community - Pointer to var, storing community string in rxed pdu
	len	   	  - Pointer to var, storing community string length rxed in pdu
	
  Return Values:
	TRUE 	- If the snmp request processing is successful.
	FALSE	- If the processing failed else the processing is not completed.
	
  Remarks:
	None
 ***************************************************************************/
static BOOL ProcessVariables(PDU_INFO* pduDbPtr,char* community, BYTE len)
{	
 	BYTE getbulkOverFlowFlag = FALSE;
    BYTE temp =0;
    BYTE OIDValue[OID_MAX_LEN];
    BYTE OIDLen=0;
   	BYTE varIndex =0;
	BYTE errorIndex;
	BYTE communityLen=0,commRetVal=0;
	BYTE noOfOIDsInReq=0,tempNonRepeators=0,noOfVarToBeInResponse=0;
	BYTE repeatCntr,varBindCntr;
	BYTE Getbulk_N=0,Getbulk_M=0,Getbulk_R=0;/*Refer RFC 3416 Section "4.2.3" GetBulkRequest-PDU*/
	BYTE oidLookUpRet=0;
	BYTE templen=0;
	BYTE successor=0;// 'I'th lexicographic successor 	 
	BYTE *ptemp;
	BYTE *ptroid;
	BYTE *rxedCommunityName;
	WORD varBindStructOffset=0;
   	WORD tempTxOffset=0;
    WORD oidOffset=0;
    WORD prevOffset=0;
	WORD packetStructLenOffset=0;
	//WORD snmpV3ScopedPduOffset=0;
    WORD pduLenOffset=0;
    WORD errorStatusOffset=0;
    WORD errorIndexOffset=0;    
    WORD varStructLenOffset=0;	
	WORD prevSnmpRxOffset=0;	
    WORD_VAL varBindingLen={0};
    WORD_VAL tempLen={0};
    WORD_VAL varPairLen={0};
    static WORD_VAL varBindLen={0};
	OID_INFO OIDInfo;  
    SNMP_ERR_STATUS errorStatus;
#ifdef STACK_USE_SNMPV3_SERVER
    BYTE agentIDLen;
    OID_INFO rec;
//	BYTE OIDValue[OID_MAX_LEN];
	WORD_VAL tempByteCntr;
#endif
WORD_VAL bytesAdded2Pdu;
	
	static enum 
	{
		SM_PKT_STRUCT_LEN_OFFSET=0u,
		SM_RESPONSE_PDU_LEN_OFFSET,
		SM_ERROR_STATUS_OFFSET,
		SM_ERROR_INDEX_OFFSET,
		SM_FIND_NO_OF_REQUESTED_VARBINDS,
		SM_FIND_NO_OF_RESPONSE_VARBINDS,
		SM_VARBIND_STRUCT_OFFSET,
		SM_VARSTRUCT_LEN_OFFSET,
		SM_POPULATE_REQ_OID,
		SM_FIND_OID_IN_MIB,
		SM_NON_REPETITIONS,
		SM_MAX_REPETITIONS
	}smSnmp=SM_PKT_STRUCT_LEN_OFFSET;

	snmpReqVarErrStatus.noSuchInstanceErr=0x0000;
	snmpReqVarErrStatus.noSuchNameErr=0x0000;
	snmpReqVarErrStatus.noSuchObjectErr=0x0000;
	snmpReqVarErrStatus.endOfMibViewErr=0x0000;

	rxedCommunityName=(BYTE *)community;
	/* Locate the start offset of the TX PDU */
	tempTxOffset = _SNMPGetTxOffset();
	OIDLen = 0;
	varBindLen.Val=0x0000;
	getZeroInstance = FALSE;
	bytesAdded2Pdu.Val=0x00;
	while(1)
	{
		switch(smSnmp)
		{

		// Before each variables are processed, prepare necessary header.

		case SM_PKT_STRUCT_LEN_OFFSET:

			varPairLen.Val=0x0000;

			_SNMPPut(STRUCTURE); // First item is packet structure

			// Since we do not know length of structure at this point, use
			// placeholder bytes that will be replaced with actual value.

			if(pduDbPtr->snmpVersion !=SNMP_V3)
				_SNMPPut(0x82);
			
			packetStructLenOffset = SNMPTxOffset;
			if(pduDbPtr->snmpVersion !=SNMP_V3)
			_SNMPPut(0);
			_SNMPPut(0);

			// Put SNMP version info - only v1.0 is supported.
			_SNMPPut(ASN_INT);              // Int type.
			_SNMPPut(1);                    // One byte long value.
			_SNMPPut(pduDbPtr->snmpVersion);              // v1.0.

#if 0
			//If SNMPv3 reuest, prepare the outgoing response PDU, update the 
			//Message Processing Model parameters
			//User Security Model Parameters and 
			//Scoped PDU ContextEngineID and ContextName
			if(pduDbPtr->snmpVersion == SNMP_V3)
			{
				Snmpv3MsgProcessingModelProcessPDU(SNMP_RESPONSE_PDU, &tempByteCntr);
				bytesAdded2Pdu.Val+=tempByteCntr.Val;
				Snmpv3UserSecurityModelProcessPDU(SNMP_RESPONSE_PDU,&tempByteCntr);
				bytesAdded2Pdu.Val+=tempByteCntr.Val;
				//Snmpv3ScopedPduProcessing(SNMP_RESPONSE_PDU,&tempByteCntr);
				//bytesAdded2Pdu.Val+=tempByteCntr.Val;
				Snmpv3ScopedPduProcessing(SNMP_RESPONSE_PDU);

			}
			else
			{
#endif 			
				// Put community string
				communityLen = len;             // Save community length for later use.
				_SNMPPut(OCTET_STRING);         // Octet string type.
				_SNMPPut(len);                  // community string length
				while( len-- )                  // Copy entire string.
				_SNMPPut(*community++);
//			}
			smSnmp++;

			//return FALSE;
		case SM_RESPONSE_PDU_LEN_OFFSET:

			// Put PDU type.  SNMP agent's response is always GET RESPONSE
			_SNMPPut(GET_RESPONSE);
			
			// Since we don't know length of this response, use placeholders until
			// we know for sure...
			if(pduDbPtr->snmpVersion !=SNMP_V3)
				_SNMPPut(0x82);                    

			pduLenOffset = SNMPTxOffset;
			if(pduDbPtr->snmpVersion !=SNMP_V3)
			_SNMPPut(0); // Be prepared for 2 byte-long length
			_SNMPPut(0);

			// Put original request back.
			_SNMPPut(ASN_INT);	// Int type.
			_SNMPPut(4);		// To simplify logic, always use 4 byte long requestID
			_SNMPPut(pduDbPtr->requestID.v[3]); // Start MSB
			_SNMPPut(pduDbPtr->requestID.v[2]);
			_SNMPPut(pduDbPtr->requestID.v[1]);
			_SNMPPut(pduDbPtr->requestID.v[0]);

			smSnmp++;

			//return FALSE;
			
		case SM_ERROR_STATUS_OFFSET:

			// Put error status.
			// Since we do not know error status, put place holder until we know it...
			_SNMPPut(ASN_INT);              // Int type
			_SNMPPut(1);                    // One byte long.
			errorStatusOffset = SNMPTxOffset;
			_SNMPPut(0);                    // Placeholder.
			smSnmp++;

		case SM_ERROR_INDEX_OFFSET:

			// Similarly put error index.
			_SNMPPut(ASN_INT);              // Int type
			_SNMPPut(1);                    // One byte long
			errorIndexOffset = SNMPTxOffset;
			_SNMPPut(0);                    // Placeholder.

			varIndex    = 0;
			errorIndex  = 0;
			errorStatus = SNMP_NO_ERR;

			smSnmp++;

		case SM_FIND_NO_OF_REQUESTED_VARBINDS:

			// Decode variable binding structure
			if ( !IsValidStructure(&varBindingLen.Val) )
			return FALSE;

			//Find number of OIDs/varbinds's data requested in received PDU.
			noOfOIDsInReq=FindOIDsInRequest(varBindingLen.Val);
			
			smSnmp++;	

			//return FALSE;

		case SM_FIND_NO_OF_RESPONSE_VARBINDS:

			//Calulate number of variables to be responded for the received request
			Getbulk_N = noOfOIDsInReq; Getbulk_M=0; Getbulk_R=0;
			if(((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C)||
				((pduDbPtr->snmpVersion == (BYTE)SNMP_V3))) && 
				(pduDbPtr->pduType == GET_BULK_REQUEST))
			{
				if((pduDbPtr->nonRepeators) <= noOfOIDsInReq)
				{
					Getbulk_N = pduDbPtr->nonRepeators;
				}

				Getbulk_M = pduDbPtr->maxRepetitions;

				if((noOfOIDsInReq - Getbulk_N)>=0u)
					Getbulk_R = noOfOIDsInReq-Getbulk_N;
			}

			tempNonRepeators = Getbulk_N;

			noOfVarToBeInResponse = Getbulk_N + (Getbulk_M * Getbulk_R);//Refer RFC 3416 

			smSnmp++;

			//return FALSE;

		case SM_VARBIND_STRUCT_OFFSET:
	
			// Put variable binding response structure
			_SNMPPut(STRUCTURE);
			_SNMPPut(0x82);

			// Since we do not know data payload length, put place holder until we know it...
			varBindStructOffset = SNMPTxOffset;
			_SNMPPut(0);
			_SNMPPut(0);

			varBindLen.Val = 0;

			smSnmp++;

			//return FALSE;
			
		case SM_VARSTRUCT_LEN_OFFSET:

			/*	If the getbulk request is received with zero non-repeaters, process
				variable State Machine jumps to SM_MAX_REPETITIONS. Modify the Rx
				and Tx offset accordigly. */
			if(Getbulk_N==0u)
			{

			#ifdef STACK_USE_SNMPV3_SERVER
				if((pduDbPtr->snmpVersion == (BYTE)SNMP_V3))
				{
					// Get complete OID string from MPFS.
				    GetOIDStringByID(MICROCHIP,&rec, OIDValue, &agentIDLen);
				    
				    if ( rec.nodeInfo.Flags.bIsAgentID )
				    {
				        MPFSSeek(hMPFS, rec.hData, MPFS_SEEK_START);
				    }

				    _SNMPPut(ASN_OID);
				    MPFSGet(hMPFS, &len);
				    agentIDLen = len;
				    _SNMPPut(len);
				    while( len-- )
				    {
					    BYTE c;
					    MPFSGet(hMPFS, &c);
				        _SNMPPut(c);
				    }
							
				}
				else
			#endif			
				{
			
					prevSnmpRxOffset=SNMPRxOffset;
					smSnmp=SM_MAX_REPETITIONS;
					varStructLenOffset = SNMPTxOffset;
					SNMPTxOffset=SNMPTxOffset+4;
					break;
				}
			}
			
			/*
				Need to know what variable we are processing, so that in case
				if there is problem for that variable, we can put it in
				errorIndex location of SNMP packet.
			*/
			varIndex++;

			// Decode variable length structure
			temp = IsValidStructure(&tempLen.Val);
			if ( !temp )
			{
				SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex);
				UDPTxCount = SNMPTxOffset;
				goto GEN_ERROR;
	//			return FALSE;
			}
			
			varBindingLen.Val -= tempLen.Val;
			varBindingLen.Val -= temp;

			varStructLenOffset = SNMPTxOffset;

			if(pduDbPtr->pduType == GET_BULK_REQUEST )
			{
				SNMPTxOffset=SNMPTxOffset+4;
			}
			smSnmp++;

			//return FALSE;

		case SM_POPULATE_REQ_OID:
			
			/* 	Populate received pdu for the requested OIDs and also create the 
				response pdu on the go.*/
			
			// Decode next object
			if ( !IsValidOID(OIDValue, &OIDLen) )
			{
				SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex);
				UDPTxCount = SNMPTxOffset-4;
				goto GEN_ERROR;
	//			return FALSE;
			}

			// For Get & Get-Next, value must be NULL.
			if ( pduDbPtr->pduType != (BYTE)SET_REQUEST )
			{
				if ( !IsASNNull() )
				return FALSE;
			}

			if(pduDbPtr->pduType != GET_BULK_REQUEST )
			{
				// Prepare response - original variable
				_SNMPPut(ASN_OID);
				oidOffset = SNMPTxOffset;
				_SNMPPut(OIDLen);
				ptemp = OIDValue;
				temp = OIDLen;
				while( temp-- )
				_SNMPPut(*ptemp++);
			}


			/* 
			   Match "rxedCommunityName" to "readCommunity" to authorize access
			   to private MIB objects.
			   As we start supporting the secured encrypted community transaction, 
			   rxed community string can be an encrypted string which the agent
			   need to decrypt and validate to autohrize access.
			   The agent should respond with encrypted community name.
			*/

			if((pduDbPtr->snmpVersion != (BYTE)SNMP_V3))
			{	
				commRetVal=SNMPValidateCommunity(rxedCommunityName);
					
				smSnmp=SM_PKT_STRUCT_LEN_OFFSET;	// Start out assuming commRetVal == INVALID_COMMUNITY
				if(pduDbPtr->pduType == (BYTE)SET_REQUEST)
				{	
					if(commRetVal==(BYTE)WRITE_COMMUNITY)//If SET request, then "community==WRITE_COMMUNITY" is must.
						smSnmp=SM_FIND_OID_IN_MIB;
				}
				else 
				{	
					if(commRetVal!=(BYTE)INVALID_COMMUNITY)//If any GET request, then "community!=INVALID_COMMUNITY" is must (community is WRITE_COMMUNITY or READ_COMMUNITY).
						smSnmp=SM_FIND_OID_IN_MIB;
				}
			
			}
			#ifdef STACK_USE_SNMPV3_SERVER
			else
			{
				smSnmp=SM_FIND_OID_IN_MIB;

			}
			#endif

			//Verify if trying to access the private object
			//Application has to decide on what community name should allowed to
			//read write the private mib objects.

			if(SNMPCheckIfPvtMibObjRequested(OIDValue) && (smSnmp==SM_PKT_STRUCT_LEN_OFFSET) )
			{
				//If private mib object is requested and community do not match, 
				//generate authentication failure TRAP
#if !defined(SNMP_TRAP_DISABLED)
				Getbulk_N=0;
				noOfVarToBeInResponse=0;
				smSnmp=SM_PKT_STRUCT_LEN_OFFSET;	

				//Searching the requested OID in the MIB database 
				oidLookUpRet = OIDLookup(pduDbPtr,OIDValue, OIDLen, &OIDInfo);	
				gOIDCorrespondingSnmpMibID=OIDInfo.id;

				_SNMPSetTxOffset(packetStructLenOffset-2);
				gSpecificTrapNotification=VENDOR_TRAP_DEFAULT;
				gGenericTrapNotification=AUTH_FAILURE;
				gSendTrapFlag=TRUE;	
#endif				

			}
			/*else 
				smSnmp++;*/

			if(smSnmp==SM_PKT_STRUCT_LEN_OFFSET || smSnmp==SM_VARSTRUCT_LEN_OFFSET)
				break;	
				
			//return FALSE;
			
		case SM_FIND_OID_IN_MIB:

			/* Search for the requested OID in the MIB database with the agent.*/
			
			if(Getbulk_N!= 0u)
				Getbulk_N--;
			
			if(Getbulk_N==0u)
				prevSnmpRxOffset=SNMPRxOffset;

			noOfVarToBeInResponse--;

			//Searching the requested OID in the MIB database 
			oidLookUpRet = OIDLookup(pduDbPtr,OIDValue, OIDLen, &OIDInfo);	

			if(oidLookUpRet != (BYTE)TRUE && (pduDbPtr->pduType != GET_NEXT_REQUEST) &&
				(pduDbPtr->pduType != GET_BULK_REQUEST))
			{
				_SNMPSetTxOffset(varStructLenOffset);

				// Put corresponding variable response structure
				_SNMPPut(STRUCTURE);
				_SNMPPut(0x82);
				
				varStructLenOffset=_SNMPGetTxOffset();
				_SNMPPut(0x00);//Place holder
				_SNMPPut(0x00);

				// ASN OID data type
				templen=OIDLen;
				ptroid=OIDValue;	
				_SNMPPut(ASN_OID);

				if(appendZeroToOID)
					_SNMPPut(OIDLen+1);//for appending "0"
				else 
					_SNMPPut(OIDLen);//do not append "0"		

				//Put OID
				while( templen-- )
				_SNMPPut(*ptroid++);

				if(appendZeroToOID)
				{
					_SNMPPut(0x00);//Appending '0' to OID in response
					varPairLen.Val += OIDLen+1+2; //Modify the response length
				}
				else 
					varPairLen.Val += OIDLen+2;
			
				//update and send the error status and the error index.
				if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
				{
					errorStatus = SNMP_NO_SUCH_NAME;
					SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_NO_SUCH_NAME,varIndex);

					_SNMPPut(ASN_NULL);
					_SNMPPut(0);	
				}
				else if(((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C)
						||( pduDbPtr->snmpVersion == (BYTE)SNMP_V3))
						&& pduDbPtr->pduType != SET_REQUEST)
				{
					if(pduDbPtr->pduType == SNMP_GET)
					{
						_SNMPPut(oidLookUpRet);
						_SNMPPut(0x00);
						if(oidLookUpRet == SNMP_NO_SUCH_OBJ)
						{
							snmpReqVarErrStatus.noSuchObjectErr|=(0x0001 << varIndex);
						}
						else if(oidLookUpRet == SNMP_NO_SUCH_INSTANCE)
						{
							snmpReqVarErrStatus.noSuchInstanceErr|=(0x0001 << varIndex);
						}
					}
				}

				if(pduDbPtr->snmpVersion !=SNMP_V3)
				varPairLen.Val +=2 ;

				varBindLen.Val += 4	// Variable Pair STRUCTURE byte + 1 length byte.
				+ varPairLen.Val;

				//Now update the place holder for var pair length
				prevOffset = _SNMPGetTxOffset();
				_SNMPSetTxOffset(varStructLenOffset);

				_SNMPPut(varPairLen.v[1]);
				_SNMPPut(varPairLen.v[0]);

				_SNMPSetTxOffset(prevOffset);

				varPairLen.Val=0x00;

				//Reset to state machine to access the next oid in request
				smSnmp=SM_VARSTRUCT_LEN_OFFSET;
				break;
			}
			smSnmp++;

			//return FALSE;

		case SM_NON_REPETITIONS:

			/* 	Variables in get,get_next,set and get_bulk ( non repetition variables)
				of snmp request are processed in this part of the state machine.*/
			
			//Save SnmpTxOffsetfor future uses.
			prevOffset = _SNMPGetTxOffset();
			_SNMPSetTxOffset(varStructLenOffset);

			//Start response variable binding with ASN STRUCTURE type.	
			_SNMPPut(STRUCTURE);
			_SNMPPut(0x82);

			varStructLenOffset=_SNMPGetTxOffset();
			_SNMPPut(0x00); //place holder
			_SNMPPut(0x00);

			_SNMPPut(ASN_OID);

			if(pduDbPtr->pduType == SNMP_SET)
            {
            	templen=OIDLen;
				ptroid=OIDValue;	
				//to validate the REC ID is present or not
	
				if(SNMPIdRecrdValidation(pduDbPtr,&OIDInfo,OIDValue,OIDLen) != TRUE)
				{
					/*if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
			       		*errorStatus = SNMP_NO_SUCH_NAME;
					else if ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C) ||
									(pduDbPtr->snmpVersion == (BYTE)SNMP_V3))*/

					 /*if the variable binding's name specifies a
				     * variable which does not exist and could not ever be
				     * created, then the value of the Response-PDU's error-
				     * status field is set to `noCreation', and the value of its
				     * error-index field is set to the index of the failed
				     * variable binding.
				     */
					errorStatus = SNMP_NO_CREATION;
					smSnmp=SM_PKT_STRUCT_LEN_OFFSET;
					return FALSE;
				}

				if(appendZeroToOID)
					_SNMPPut(OIDLen+1);//for appending "0"
				else 
					_SNMPPut(OIDLen);//do not append "0"		

				//Put OID
				while( templen-- )
				_SNMPPut(*ptroid++);

				if(appendZeroToOID)
					_SNMPPut(0x00);//Appending '0' to OID in response

				//Now process the SET command
                temp = ProcessSetVar(pduDbPtr,&OIDInfo, &errorStatus);

				if ( errorStatus != SNMP_NO_ERR )
                {
	                //SET var command failed. Update the error status.
                    SetErrorStatus(errorStatusOffset,
                                   errorIndexOffset,
                                   errorStatus,
                                   varIndex);

                }

				if(appendZeroToOID)
					varPairLen.Val = OIDLen+1 +2   // OID name + header bytes
                                + temp;            // value bytes as put by SetVar
				else
					varPairLen.Val = OIDLen+2+temp;

			}
			else if((pduDbPtr->pduType == SNMP_GET) || (getZeroInstance) ||
				((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet == TRUE)))
			{	
				//to validate the REC ID is present or not
				if(SNMPIdRecrdValidation(pduDbPtr,&OIDInfo,OIDValue,OIDLen) != TRUE)
				{
					smSnmp=SM_PKT_STRUCT_LEN_OFFSET;
					return FALSE;
				}
				templen=OIDLen;
				ptroid=OIDValue;	
				
				if(appendZeroToOID || getZeroInstance)
					_SNMPPut(OIDLen+1);//for appending index
				else 
					_SNMPPut(OIDLen);//do not append "0"

				//Put OID
				while( templen-- )
					_SNMPPut(*ptroid++);

				if(getZeroInstance)
				{
					_SNMPPut(OIDInfo.index);//Appending '0' to OID in response
					varPairLen.Val = OIDLen + 2+1;
				}
				else if(appendZeroToOID)
				{
					_SNMPPut(0x00);//Appending '0' to OID in response
					varPairLen.Val = OIDLen + 2+1;
				}
				else 
					varPairLen.Val = OIDLen +2;
				
				
				//Now process the GET command
				temp=ProcessGetVar(&OIDInfo,FALSE,pduDbPtr);

			}	
			else if((pduDbPtr->pduType == SNMP_GET_NEXT)||
				((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet != TRUE)))
			{			
				temp=ProcessGetNextVar(&OIDInfo,pduDbPtr);

				//If Get Next command failed
				if(temp==0u)
				{
					templen=OIDLen;
					ptroid=OIDValue;
					
					if(appendZeroToOID)
						_SNMPPut(OIDLen+1);//for appending "0"
					else 
						_SNMPPut(OIDLen);//do not append "0"

					//Put OID
					while( templen-- )
						_SNMPPut(*ptroid++);
					
					if(appendZeroToOID)
						_SNMPPut(0x00);//Appending '0' to OID in response
				}
			}


			/*  If the request command processing is failed, update
				the error status, index accordingly and response pdu.*/ 
			if(temp == 0u &&(pduDbPtr->pduType != SNMP_SET))
			{
				if(pduDbPtr->snmpVersion == (BYTE)SNMP_V1)
				{
					errorStatus = SNMP_NO_SUCH_NAME;
					SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_NO_SUCH_NAME,
								   varIndex);
				}

				_SNMPPut(ASN_NULL);
				_SNMPPut(0);

				if((pduDbPtr->pduType == SNMP_GET_NEXT|| pduDbPtr->pduType == SNMP_V2C_GET_BULK)&&pduDbPtr->snmpVersion == (BYTE)SNMP_V2C)
				{
					SNMPTxOffset=SNMPTxOffset-2;
					_SNMPPut(SNMP_END_OF_MIB_VIEW);
					_SNMPPut(0);
				
				}

				if((pduDbPtr->pduType == SNMP_GET) || 
					((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet == TRUE)))
				{
					temp = 2;
				}
				else if((pduDbPtr->pduType == SNMP_GET_NEXT)||
					((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet != TRUE)))
				{
				     varPairLen.Val = OIDLen+1          // as put by GetNextVar()
                                     + 2                // OID header
                                     + 2;               // END_OF_MIB_VIEW bytes
				}


				/* 	Applications can make use of the below information 
					to find the error status for the given variable and to 
					build the logic arround. */
				snmpReqVarErrStatus.noSuchNameErr	 |=(0x0001 << varIndex);
				snmpReqVarErrStatus.noSuchObjectErr	 |=(0x0001 << varIndex);
				snmpReqVarErrStatus.noSuchInstanceErr|=(0x0001 << varIndex);
				snmpReqVarErrStatus.endOfMibViewErr	 |=(0x0001 << varIndex);
				
			}
			else if((pduDbPtr->pduType == SNMP_GET_NEXT)||
				((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet != TRUE)))
			{
				if(getZeroInstance)
					varPairLen.Val += temp;
				else
					varPairLen.Val = (temp + 2);
			}
	
			if((pduDbPtr->pduType == SNMP_GET) || 
				((pduDbPtr->pduType == SNMP_V2C_GET_BULK) && (oidLookUpRet == TRUE)))
				varPairLen.Val += temp;   

			varBindLen.Val += 4	// Variable Pair STRUCTURE byte + 1 length byte.
			+ varPairLen.Val;

			//Update place holder
			prevOffset = _SNMPGetTxOffset();
			_SNMPSetTxOffset(varStructLenOffset);
			_SNMPPut(varPairLen.v[1]);
			_SNMPPut(varPairLen.v[0]);

			_SNMPSetTxOffset(prevOffset);
			varStructLenOffset = _SNMPGetTxOffset();


			/* 	Decide on the number of Non repetition variables remained to 
				be processed, decide the course of state machine.*/
				
			if((pduDbPtr->pduType==GET_BULK_REQUEST) &&
			   ((pduDbPtr->snmpVersion == (BYTE)SNMP_V2C)||
			    (pduDbPtr->snmpVersion == (BYTE)SNMP_V3))&&( Getbulk_N == 0u))
			{
				if((varStructLenOffset - tempTxOffset) >= SNMP_MAX_MSG_SIZE)
				{
					getbulkOverFlowFlag = TRUE;
					goto GETBULK_MAX_REESPONSE;
				}
				else
				{
					smSnmp=SM_MAX_REPETITIONS;
				}
			}
			else
				smSnmp=SM_VARSTRUCT_LEN_OFFSET;

			
			varPairLen.Val=0x00;
			getZeroInstance = FALSE;

			/* check length*/
			break;	

			//return FALSE;

		case SM_MAX_REPETITIONS:

			/*Process each variable in request as Get_Next for 
			  Getbulk_M (Max_repetition) times */
			for(repeatCntr=0;repeatCntr<Getbulk_M;repeatCntr++)
			{
				SNMPRxOffset=prevSnmpRxOffset;
				
				//Process every veriable in the request.
				for(varBindCntr=0;varBindCntr<Getbulk_R;varBindCntr++)
				{

					if(varBindCntr==0u)
						varIndex=(noOfOIDsInReq-Getbulk_R);

					varIndex++;

					if((snmpReqVarErrStatus.endOfMibViewErr >> (tempNonRepeators+varBindCntr+1))&0x0001)
 					{
						noOfVarToBeInResponse--;
 						temp = IsValidStructure(&tempLen.Val);

						if(varBindCntr!=Getbulk_R)
						{
							SNMPRxOffset=SNMPRxOffset+tempLen.Val;//2+OIDLen+2;
						}
						continue;
					}

					if(noOfVarToBeInResponse != 0)
					{
						noOfVarToBeInResponse--;
					}
					prevOffset = _SNMPGetTxOffset();
					_SNMPSetTxOffset(varStructLenOffset);

					_SNMPPut(STRUCTURE);
					_SNMPPut(0x82);
					varStructLenOffset=_SNMPGetTxOffset();
					_SNMPPut(0x00);
					_SNMPPut(0x00);

					successor=repeatCntr;

					// Decode variable length structure
					temp = IsValidStructure(&tempLen.Val);
					if ( !temp )
						break;

					// Decode next object
					if ( !IsValidOID(OIDValue, &OIDLen) )
					{
						SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_GEN_ERR,varIndex);
						UDPTxCount = SNMPTxOffset -4;
						goto GEN_ERROR;
						//return FALSE;
					}
					templen=OIDLen;
					ptroid=OIDValue;
					
					// For Get & Get-Next, value must be NULL.
					if ( pduDbPtr->pduType != (BYTE)SET_REQUEST )
						if ( !IsASNNull() )
							break;

					oidLookUpRet = OIDLookup(pduDbPtr,OIDValue, OIDLen, &OIDInfo);
					if(oidLookUpRet == SNMP_END_OF_MIB_VIEW)
					{
						temp = GetNextLeaf(&OIDInfo);
					}
					if(oidLookUpRet == FALSE)
					{
						templen=OIDLen;
						ptroid=OIDValue;	
						_SNMPPut(ASN_OID);

						if(appendZeroToOID)
						{
							_SNMPPut(OIDLen+1);//for appending "0"
							OIDLen += 1;
						}
						else 
							_SNMPPut(OIDLen);//for appending "0"

						//Put OID
						while( templen-- )
							_SNMPPut(*ptroid++);
						
						if(appendZeroToOID)
							_SNMPPut(0x00);//Appending '0' to OID in response

						_SNMPPut(SNMP_END_OF_MIB_VIEW);
						_SNMPPut(0x00);

						//Start counting total number of bytes in this structure.
						varPairLen.Val = OIDLen // as put by GetNextVar()
						 +2       // OID header
						 +2;      // endOfMibView bytes

						snmpReqVarErrStatus.endOfMibViewErr	 |=(0x0001 << varIndex);
					}
					else if(temp != 0)//if(oidLookUpRet != SNMP_END_OF_MIB_VIEW)
					{
						temp = ProcessGetBulkVar(&OIDInfo, &OIDValue[0],&OIDLen,&successor,pduDbPtr);
					}
					if ( temp == 0u )
					{
						templen=OIDLen;
						ptroid=OIDValue;	
						_SNMPPut(ASN_OID);
						_SNMPPut(OIDLen);

						//Put OID
						while( templen-- )
						_SNMPPut(*ptroid++);

						/*Do send back the Same OID if get_next is EndOfMibView. Do not
						  append zero to this OID*/
						
						_SNMPPut(SNMP_END_OF_MIB_VIEW);
						_SNMPPut(0x00);

						snmpReqVarErrStatus.endOfMibViewErr	 |=(0x0001 << varIndex);
						                    
						//Start counting total number of bytes in this structure.
						varPairLen.Val = OIDLen  // as put by GetNextVar()
						     + 2     // OID header
						     + 2;    // endOfMibView byte.
					}
					else 
						varPairLen.Val = (temp + 2);        // + OID headerbytes
				
					varBindLen.Val += 4	// Variable Pair STRUCTURE byte + 1 length byte.
					+ varPairLen.Val;

					prevOffset = _SNMPGetTxOffset();
					_SNMPSetTxOffset(varStructLenOffset);

					_SNMPPut(varPairLen.v[1]);
					_SNMPPut(varPairLen.v[0]);

					_SNMPSetTxOffset(prevOffset);
					varStructLenOffset = _SNMPGetTxOffset();
					if((varStructLenOffset - tempTxOffset) >= SNMP_MAX_MSG_SIZE)
					{
						getbulkOverFlowFlag = TRUE;
						goto GETBULK_MAX_REESPONSE;
					}
						 
				}//for(varBindCntr=0;varBindCntr<Getbulk_R;varBindCntr++)
				
			}//for(repeatCntr=0;repeatCntr<Getbulk_M;repeatCntr++)
			
			break;
		}//end of switch(smSnmp)

		/*If all the variables are processed and the repsonse pdu is updated with 
		  the number of variable responses ought to be in the response; you are done
		  with the request pdu processing. Else continue to processing.*/
		if(Getbulk_N==0u && noOfVarToBeInResponse==0u)
		{	
 			smSnmp=SM_PKT_STRUCT_LEN_OFFSET;
			break;
		}

	}//end of while(1)		


	// Update the place holders with respective values.
		
	//calculate the number of bytes are the part of RESPONSE PDU 
	if(((prevOffset - tempTxOffset) > SNMP_MAX_MSG_SIZE) 
		&& (pduDbPtr->pduType!=GET_BULK_REQUEST))
	{
		/* for snmpv2 (or snmpv3) by rfc3416 we return special
		*   tooBig(1) response with empty variable-bindings field. 
		* error status  = toobig(1) and error_index set to 0.
		*/
BIG_ERROR:
		SetErrorStatus(errorStatusOffset,errorIndexOffset,SNMP_TOO_BIG,0);
		varBindLen.Val =  6 						// Request ID bytes (4+2)
						+ 3 						// Error status 	(2+1)
						+ 3;						// Error index		(2+1)
		_SNMPSetTxOffset(pduLenOffset);
		_SNMPPut(varBindLen.v[1]);
		_SNMPPut(varBindLen.v[0]);
	
		// varBindLen is reused as "packetLen".
		varBindLen.Val = 3						// SNMP Version bytes
						+ 2 + communityLen		// community string bytes
						+ 4 					// PDU structure header bytes.
						+ varBindLen.Val;
	
		_SNMPSetTxOffset(packetStructLenOffset);
		_SNMPPut(varBindLen.v[1]);
		_SNMPPut(varBindLen.v[0]);
								
		UDPTxCount = _SNMPSetTxOffset(varBindStructOffset-2);
		
		smSnmp = SM_PKT_STRUCT_LEN_OFFSET;
		return TRUE;
	}
GETBULK_MAX_REESPONSE:
	
	/* As per RFC 3416 - GET bULK Response - 4.2.3
	If the size of the message encapsulating the Response-PDU containing the 
	requested number of variable bindings would be greater than either a local
	constraint or the maximum message size of the originator, then the response
	is generated with a lesser number of variable bindings. This lesser number is
	the ordered set of variable bindings with some of the variable bindings at the
	end of the set removed, such that the size of the message encapsulating the
	Response-PDU is approximately equal to but no greater than either a local
	constraint or the maximum message size of the originator. Note that the 
	number of variable bindings removed has no relationship to the values of N, M, or R.*/
	if(getbulkOverFlowFlag && (pduDbPtr->pduType==GET_BULK_REQUEST))
	{
		SNMPTxOffset = SNMPTxOffset - (varPairLen.Val+4);
		varBindLen.Val = varBindLen.Val - (varPairLen.Val+4);
		UDPTxCount = SNMPTxOffset;
	}
GEN_ERROR:
	prevOffset = _SNMPGetTxOffset();
	/* GetRequest-PDU (As per RFC 3416 - SECTION - 4.2.1)
	During the process of any OID,variable binding fails due to invalid OID 
	or invalid OID type or invalid OID length etc, i,e other than "noSuchObject"
	or " noSuchInstance", then the Response-PDU is re-formatted with the same 
	values in its request-id and variable-bindings fields as the received 
    GetRequest-PDU , with the value of its error-status field set to "genErr", 
	
	GetNextRequest-PDU (As per RFC 3416 - SECTION - 4.2.2)
	During the process of any OID,variable binding fails due to invalid OID 
	or invalid OID type or invalid OID length etc, other than "endOfMibView" ,
	then the Response-PDU is re-formatted with the same values in its request-id and 
	variable-bindings fields as the received GetNextRequest-PDU,with the value of 
	its error-status field set to "genErr", and the value of its error-index
	field is set to the index of the failed variable binding. 

	The generated Response-PDU is then encapsulated into a message. If the size of the resultant 
	message is less than or equal to maximum message size of the originator, it is transmitted 
	to the originator of the GetNextRequest-PDU. 

	Otherwise, an alternate Response-PDU is generated. This alternate Response-PDU is formatted 
	with the same values in its request-id field as the received GetNextRequest-PDU, with the value 
	of its error-status field set to "tooBig", the value of its error-index field set to zero, and an empty 
	variable-bindings field.
	
	*/
	if(((prevOffset - tempTxOffset) > SNMP_MAX_MSG_SIZE) )
		goto BIG_ERROR;
	
    _SNMPSetTxOffset(varBindStructOffset);
    _SNMPPut(varBindLen.v[1]);
    _SNMPPut(varBindLen.v[0]);

	_SNMPSetTxOffset(prevOffset);

    // varBindLen is reused as "pduLen"
    varBindLen.Val = varBindLen.Val+4       // Variable Binding Strucure length
                + 6                         // Request ID bytes (4+2)
                + 3                         // Error status		(2+1)
                + 3;                        // Error index		(2+1)	
	prevOffset = _SNMPGetTxOffset();

    _SNMPSetTxOffset(pduLenOffset);
	if(pduDbPtr->snmpVersion !=SNMP_V3)
    _SNMPPut(varBindLen.v[1]);
    _SNMPPut(varBindLen.v[0]);

	_SNMPSetTxOffset(prevOffset);

#ifdef STACK_USE_SNMPV3_SERVER
	if(pduDbPtr->snmpVersion == SNMP_V3)
	{
		prevOffset = _SNMPGetTxOffset();
	
		_SNMPSetTxOffset(msgSecrtyParamLenOffset);
	   // _SNMPPut(varBindLen.v[1]);
		_SNMPPut(varBindLen.v[0]+tempByteCntr.v[0]);
	
		_SNMPSetTxOffset(prevOffset);
	
	}
#endif

	// Update the place holders with respective values.
	if(pduDbPtr->snmpVersion != SNMP_V3)
	{
    	// varBindLen is reused as "packetLen".
    	varBindLen.Val = 3                      // SNMP Version bytes
                    + 2 + communityLen      // community string bytes
                    + 4                     // PDU structure header bytes.
                    + varBindLen.Val;
	}
#ifdef STACK_USE_SNMPV3_SERVER
	else
	{
		varBindLen.Val = 3       // SNMP Version bytes
		 + 2                     // PDU structure header bytes.
         + varBindLen.Val
		 +bytesAdded2Pdu.Val;
			
	}
#endif

	prevOffset = _SNMPGetTxOffset();

    _SNMPSetTxOffset(packetStructLenOffset);
	if(pduDbPtr->snmpVersion !=SNMP_V3)
    _SNMPPut(varBindLen.v[1]);
    _SNMPPut(varBindLen.v[0]);

	_SNMPSetTxOffset(prevOffset);
	smSnmp = SM_PKT_STRUCT_LEN_OFFSET;


    return TRUE;
	
}
	





#endif

#endif //#if defined(STACK_USE_SNMP_SERVER)
