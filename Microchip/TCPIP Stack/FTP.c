/*********************************************************************
 *
 *	File Transfer Protocol (FTP) Client
 *  	Module for Microchip TCP/IP Stack
 *	 -Provides ability to remotely upload MPFS2 image (web pages) 
 *	  to external EEPROM or external Flash memory
 *	 -Reference: RFC 959
 *
 *********************************************************************
 * FileName:        FTP.c
 * Dependencies:    TCP, Tick, MPFS2, FTPVerify() callback
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
 ********************************************************************
 File Description:

 Change History:
  Rev   Description
  ----  -----------------------------------------
  5.36  FTP with MPFS2 and HTTP2
 
  ********************************************************************/
#define __FTP_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_FTP_SERVER)

#include "TCPIP Stack/TCPIP.h"


#define FTP_COMMAND_PORT                (21u)
#define FTP_DATA_PORT                   (20u)
#define FTP_TIMEOUT                     (180ul * TICK_SECOND)
#define MAX_FTP_ARGS                    (7u)
#define MAX_FTP_CMD_STRING_LEN          (31u)

typedef enum _SM_FTP
{
    SM_FTP_NOT_CONNECTED,
    SM_FTP_CONNECTED,
    SM_FTP_USER_NAME,
    SM_FTP_USER_PASS,
    SM_FTP_RESPOND
} SM_FTP;

typedef enum _SM_FTP_CMD
{
    SM_FTP_CMD_IDLE,
    SM_FTP_CMD_WAIT,
    SM_FTP_CMD_RECEIVE,
    SM_FTP_CMD_WAIT_FOR_DISCONNECT
} SM_FTP_CMD;

typedef enum _FTP_COMMAND
{
    FTP_CMD_USER,
    FTP_CMD_PASS,
    FTP_CMD_QUIT,
    FTP_CMD_STOR,
    FTP_CMD_PORT,
    FTP_CMD_ABORT,
    FTP_CMD_PWD,
    FTP_CMD_CWD,
    FTP_CMD_TYPE,
    FTP_CMD_UNKNOWN,
    FTP_CMD_NONE,
} FTP_COMMAND;

// Each entry in following table must match with that of FTP_COMMAND enum.
static ROM char * ROM FTPCommandString[] =
{
    "USER",                         // FTP_CMD_USER
    "PASS",                         // FTP_CMD_PASS
    "QUIT",                         // FTP_CMD_QUIT
    "STOR",                         // FTP_CMD_STOR
    "PORT",                         // FTP_CMD_PORT
    "ABOR",                         // FTP_CMD_ABORT
    "PWD ",                         // FTP_CMD_PWD
    "CWD ",                         // FTP_CMD_CWD
    "TYPE"                          // FTP_CMD_TYPE
};
#define FTP_COMMAND_TABLE_SIZE  ( sizeof(FTPCommandString)/sizeof(FTPCommandString[0]) )


typedef enum _FTP_RESPONSE
{
    FTP_RESP_BANNER,
    FTP_RESP_USER_OK,
    FTP_RESP_PASS_OK,
    FTP_RESP_QUIT_OK,
    FTP_RESP_STOR_OK,
    FTP_RESP_UNKNOWN,
    FTP_RESP_LOGIN,
    FTP_RESP_DATA_OPEN,
    FTP_RESP_DATA_READY,
    FTP_RESP_DATA_CLOSE,
	FTP_RESP_DATA_NO_SOCKET,
	FTP_RESP_PWD,
    FTP_RESP_OK,

    FTP_RESP_NONE                       // This must always be the last
                                        // There is no corresponding string.
} FTP_RESPONSE;

// Each entry in following table must match with FTP_RESPONE enum
static ROM char * ROM FTPResponseString[] =
{
    "220 Ready\r\n",                    // FTP_RESP_BANNER
    "331 Password required\r\n",        // FTP_RESP_USER_OK
    "230 Logged in\r\n",                // FTP_RESP_PASS_OK
    "221 Bye\r\n",                      // FTP_RESP_QUIT_OK
    "500 \r\n",                         // FTP_RESP_STOR_OK
    "502 Not implemented\r\n",          // FTP_RESP_UNKNOWN
    "530 Login required\r\n",           // FTP_RESP_LOGIN
    "150 Transferring data...\r\n",     // FTP_RESP_DATA_OPEN
    "125 Done\r\n",                    	// FTP_RESP_DATA_READY
    "226 Transfer Complete\r\n",        // FTP_RESP_DATA_CLOSE
	"425 Can't create data socket.\r\n",// FTP_RESP_DATA_NO_SOCKET
	"257 \"/\" is current\r\n",         // FTP_RESP_PWD
    "200 Ok\r\n"                        // FTP_RESP_OK
};


static union
{
    struct
    {
        unsigned char bUserSupplied : 1;
        unsigned char bLoggedIn: 1;
    } Bits;
    BYTE Val;
} FTPFlags;


static TCP_SOCKET       FTPSocket;      // Main ftp command socket.
static TCP_SOCKET       FTPDataSocket;  // ftp data socket.
static WORD_VAL         FTPDataPort;    // ftp data port number as supplied by client

static SM_FTP           smFTP;          // ftp server FSM state
static SM_FTP_CMD       smFTPCommand;   // ftp command FSM state

static FTP_COMMAND      FTPCommand;
static FTP_RESPONSE     FTPResponse;

static BYTE             FTPUser[FTP_USER_NAME_LEN];
static BYTE             FTPString[MAX_FTP_CMD_STRING_LEN+2];
static BYTE             FTPStringLen;
static BYTE             *FTP_argv[MAX_FTP_ARGS];    // Parameters for a ftp command
static BYTE             FTP_argc;       // Total number of params for a ftp command
static DWORD            lastActivity;   // Timeout keeper.


static MPFS_HANDLE      FTPFileHandle;


// Private helper functions.
static void ParseFTPString(void);
static FTP_COMMAND ParseFTPCommand(BYTE *cmd);
static void ParseFTPString(void);
static BOOL ExecuteFTPCommand(FTP_COMMAND cmd);
static BOOL PutFile(void);
static BOOL Quit(void);


#define FTP_PUT_ENABLED


/*********************************************************************
 * Function:        void FTPInit(void)
 *
 * PreCondition:    TCP module is already initialized.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes internal variables of FTP
 *
 * Note:
 ********************************************************************/
void FTPInit(void)
{
    FTPSocket       = TCPOpen(0, TCP_OPEN_SERVER, FTP_COMMAND_PORT, TCP_PURPOSE_FTP_COMMAND);

	// If this trap ever gets entered, it means your TCPIPConfig.h file 
	// needs to be edited to make a TCP_PURPOSE_FTP_COMMAND socket 
	// available, as defined by the TCPSocketInitializer[] array.  Make 
	// sure that a TCP_PURPOSE_FTP_DATA socket is also available to 
	// use the FTP Server.
    if(FTPSocket == INVALID_SOCKET)
    	while(1);

	FTPDataSocket	= INVALID_SOCKET;
    smFTP           = SM_FTP_NOT_CONNECTED;
    FTPStringLen    = 0;
    FTPFlags.Val    = 0;
    FTPDataPort.Val = FTP_DATA_PORT;
}


/*********************************************************************
 * Function:        void FTPServer(void)
 *
 * PreCondition:    FTPInit() must already be called.
 *
 * Input:           None
 *
 * Output:          Opened FTP connections are served.
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            This function acts as a task (similar to one in
 *                  RTOS).  This function performs its task in
 *                  co-operative manner.  Main application must call
 *                  this function repeatdly to ensure all open
 *                  or new connections are served on time.
 ********************************************************************/
BOOL FTPServer(void)
{
    BYTE v;
    DWORD currentTick;

    if ( !TCPIsConnected(FTPSocket) )
    {
        FTPStringLen    = 0;
        FTPCommand      = FTP_CMD_NONE;
        smFTP           = SM_FTP_NOT_CONNECTED;
        FTPFlags.Val    = 0;
        smFTPCommand    = SM_FTP_CMD_IDLE;
		if(FTPDataSocket != INVALID_SOCKET)
		{
			TCPDisconnect(FTPDataSocket);
			FTPDataSocket = INVALID_SOCKET;
		}

        return TRUE;
    }

    if ( TCPIsGetReady(FTPSocket) )
    {
        lastActivity    = TickGet();

        while( TCPGet(FTPSocket, &v ) )
        {
            FTPString[FTPStringLen++]   = v;
            if ( FTPStringLen == MAX_FTP_CMD_STRING_LEN )
                FTPStringLen            = 0;
        }


        if ( v == '\n' )
        {
            FTPString[FTPStringLen]     = '\0';
            FTPStringLen                = 0;
            ParseFTPString();
            FTPCommand                  = ParseFTPCommand(FTP_argv[0]);
        }
    }
    else if ( smFTP != SM_FTP_NOT_CONNECTED )
    {
        currentTick = TickGet();
        currentTick = currentTick - lastActivity;
        if ( currentTick >= FTP_TIMEOUT )
        {
            lastActivity                = TickGet();
            FTPCommand                  = FTP_CMD_QUIT;
            smFTP                       = SM_FTP_CONNECTED;
        }
    }

    switch(smFTP)
    {
    case SM_FTP_NOT_CONNECTED:
        FTPResponse = FTP_RESP_BANNER;
        lastActivity = TickGet();
        // No break - Continue...

    case SM_FTP_RESPOND:
SM_FTP_RESPOND_Label:
		// Make sure there is enough TCP TX FIFO space to put our response
        if(TCPIsPutReady(FTPSocket) < strlenpgm(FTPResponseString[FTPResponse]))
			return TRUE;

		TCPPutROMString(FTPSocket, (ROM BYTE*)FTPResponseString[FTPResponse]);
		TCPFlush(FTPSocket);
		FTPResponse = FTP_RESP_NONE;
		smFTP = SM_FTP_CONNECTED;
        // No break - this will speed up little bit

    case SM_FTP_CONNECTED:
        if ( FTPCommand != FTP_CMD_NONE )
        {
            if ( ExecuteFTPCommand(FTPCommand) )
            {
                if ( FTPResponse != FTP_RESP_NONE )
                    smFTP = SM_FTP_RESPOND;
                else if ( FTPCommand == FTP_CMD_QUIT )
                    smFTP = SM_FTP_NOT_CONNECTED;

                FTPCommand = FTP_CMD_NONE;
                smFTPCommand = SM_FTP_CMD_IDLE;
            }
            else if ( FTPResponse != FTP_RESP_NONE )
            {
                smFTP = SM_FTP_RESPOND;
                goto SM_FTP_RESPOND_Label;
            }
        }
        break;
	case SM_FTP_USER_NAME:
	case SM_FTP_USER_PASS:
		break;

    }

    return TRUE;
}

#ifdef WIFI_NET_TEST
static BOOL FTPVerify(BYTE *login, BYTE *password)
{
	return TRUE;
}
#endif

static BOOL ExecuteFTPCommand(FTP_COMMAND cmd)
{
    switch(cmd)
    {
    case FTP_CMD_USER:
        FTPFlags.Bits.bUserSupplied = TRUE;
        FTPFlags.Bits.bLoggedIn = FALSE;
        FTPResponse = FTP_RESP_USER_OK;
        strncpy((char*)FTPUser, (char*)FTP_argv[1], sizeof(FTPUser));
        break;

    case FTP_CMD_PASS:
        if ( !FTPFlags.Bits.bUserSupplied )
            FTPResponse = FTP_RESP_LOGIN;
        else
        {
            if ( FTPVerify(FTPUser, FTP_argv[1]) )
            {
                FTPFlags.Bits.bLoggedIn = TRUE;
                FTPResponse = FTP_RESP_PASS_OK;
            }
            else
                FTPResponse = FTP_RESP_LOGIN;
        }
        break;

    case FTP_CMD_QUIT:
        return Quit();

    case FTP_CMD_PORT:
        FTPDataPort.v[1] = (BYTE)atoi((char*)FTP_argv[5]);
        FTPDataPort.v[0] = (BYTE)atoi((char*)FTP_argv[6]);
        FTPResponse = FTP_RESP_OK;
        break;

    case FTP_CMD_STOR:
        return PutFile();
        
    case FTP_CMD_PWD:
    	FTPResponse = FTP_RESP_PWD;
    	break;

    case FTP_CMD_CWD:
    case FTP_CMD_TYPE:
       	FTPResponse = FTP_RESP_OK;
    	break;

    case FTP_CMD_ABORT:
        FTPResponse = FTP_RESP_OK;
        if ( FTPDataSocket != INVALID_SOCKET )
		{
			TCPDisconnect(FTPDataSocket);
			FTPDataSocket = INVALID_SOCKET;
		}
        break;

    default:
        FTPResponse = FTP_RESP_UNKNOWN;
        break;
    }
    return TRUE;
}

static BOOL Quit(void)
{
    switch(smFTPCommand)
    {
    case SM_FTP_CMD_IDLE:
#if defined(FTP_PUT_ENABLED)
        if ( smFTPCommand == SM_FTP_CMD_RECEIVE )
            MPFSClose(FTPFileHandle);
#endif

        if ( FTPDataSocket != INVALID_SOCKET )
        {
#if defined(FTP_PUT_ENABLED)
            MPFSClose(FTPFileHandle);
#endif
            TCPDisconnect(FTPDataSocket);
            smFTPCommand = SM_FTP_CMD_WAIT;
        }
        else
            goto Quit_Done;
        break;

    case SM_FTP_CMD_WAIT:
        if ( !TCPIsConnected(FTPDataSocket) )
        {
Quit_Done:
            FTPResponse = FTP_RESP_QUIT_OK;
            smFTPCommand = SM_FTP_CMD_WAIT_FOR_DISCONNECT;
        }
        break;

    case SM_FTP_CMD_WAIT_FOR_DISCONNECT:
        if ( TCPIsPutReady(FTPSocket) )
        {
            if ( TCPIsConnected(FTPSocket) )
                TCPDisconnect(FTPSocket);
        }
        break;
	case SM_FTP_CMD_RECEIVE:
		break;

    }
    return FALSE;
}


static BOOL PutFile(void)
{
    BYTE v;


    switch(smFTPCommand)
    {
    case SM_FTP_CMD_IDLE:
        if ( !FTPFlags.Bits.bLoggedIn )
        {
            FTPResponse     = FTP_RESP_LOGIN;
            return TRUE;
        }
        else
        {
			FTPResponse     = FTP_RESP_DATA_OPEN;
            FTPDataSocket   = TCPOpen((PTR_BASE)&TCPGetRemoteInfo(FTPSocket)->remote, TCP_OPEN_NODE_INFO, FTPDataPort.Val, TCP_PURPOSE_FTP_DATA);

			// Make sure that a valid socket was available and returned
			// If not, return with an error
			if(FTPDataSocket == INVALID_SOCKET)
			{
	            FTPResponse = FTP_RESP_DATA_NO_SOCKET;
	            return TRUE;
			}
			
			smFTPCommand = SM_FTP_CMD_WAIT;
        }
        break;

    case SM_FTP_CMD_WAIT:
        if ( TCPIsConnected(FTPDataSocket) )
        {
#if defined(FTP_PUT_ENABLED)
			FTPFileHandle   = MPFSFormat();
#endif

			smFTPCommand    = SM_FTP_CMD_RECEIVE;
        }
        break;

    case SM_FTP_CMD_RECEIVE:
        if ( TCPIsGetReady(FTPDataSocket) )
        {
            // Reload timeout timer.
            lastActivity    = TickGet();
            while( TCPGet(FTPDataSocket, &v) )
            {
#if defined(FTP_PUT_ENABLED)
                MPFSPutArray(FTPFileHandle,&v,1);
#endif
            }
            MPFSPutEnd(TRUE);

        }
        else if ( !TCPIsConnected(FTPDataSocket) )
        {
#if defined(FTP_PUT_ENABLED)
            MPFSClose(FTPFileHandle);
#endif
            TCPDisconnect(FTPDataSocket);
            FTPDataSocket   = INVALID_SOCKET;
            FTPResponse     = FTP_RESP_DATA_CLOSE;
            return TRUE;
        }
	case SM_FTP_CMD_WAIT_FOR_DISCONNECT:
		break;
    }
    return FALSE;
}



static FTP_COMMAND ParseFTPCommand(BYTE *cmd)
{
    FTP_COMMAND i;

	for ( i = 0; i < (FTP_COMMAND)FTP_COMMAND_TABLE_SIZE; i++ )
	{
		if ( !memcmppgm2ram((void*)cmd, (ROM void*)FTPCommandString[i], 3) )
			return i;
	}

	return FTP_CMD_UNKNOWN;
}

static void ParseFTPString(void)
{
    BYTE *p;
    BYTE v;
    enum { SM_FTP_PARSE_PARAM, SM_FTP_PARSE_SPACE } smParseFTP;

    smParseFTP  = SM_FTP_PARSE_PARAM;
    p           = (BYTE*)&FTPString[0];

    // Skip white blanks
    while( *p == ' ' )
        p++;

    FTP_argv[0]  = (BYTE*)p;
    FTP_argc     = 1;

    while( (v = *p) )
    {
        switch(smParseFTP)
        {
        case SM_FTP_PARSE_PARAM:
            if ( v == ' ' || v == ',' )
            {
                *p = '\0';
                smParseFTP = SM_FTP_PARSE_SPACE;
            }
            else if ( v == '\r' || v == '\n' )
                *p = '\0';
            break;

        case SM_FTP_PARSE_SPACE:
            if ( v != ' ' )
            {
                FTP_argv[FTP_argc++] = (BYTE*)p;
                smParseFTP = SM_FTP_PARSE_PARAM;
            }
            break;
        }
        p++;
        if(FTP_argc == MAX_FTP_ARGS)
        	break;
    }
}

#endif	// #if defined(STACK_USE_FTP_SERVER)
