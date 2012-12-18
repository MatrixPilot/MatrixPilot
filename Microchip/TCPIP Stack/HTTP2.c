/*********************************************************************
 *
 *  HyperText Transfer Protocol (HTTP) Server
 *  Module for Microchip TCP/IP Stack
 *   -Serves dynamic pages to web browsers such as Microsoft Internet 
 *    Explorer, Mozilla Firefox, etc.
 *	 -Reference: RFC 2616
 *
 **********************************************************************
 * FileName:        HTTP2.c
 * Dependencies:    TCP, MPFS2, Tick, CustomHTTPApp.c callbacks
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
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     8/14/01     Original
 * Elliott Wood			6/4/07		Complete rewrite, known as HTTP2
 ********************************************************************/

#define __HTTP2_C

#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_HTTP2_SERVER)

#include "HTTPPrint.h"

/****************************************************************************
  Section:
	String Constants
  ***************************************************************************/
	static ROM BYTE HTTP_CRLF[] = "\r\n";	// New line sequence
	#define HTTP_CRLF_LEN	2				// Length of above string
		
/****************************************************************************
  Section:
	File and Content Type Settings
  ***************************************************************************/
	// File type extensions corresponding to HTTP_FILE_TYPE
	static ROM char * ROM httpFileExtensions[HTTP_UNKNOWN+1] =
	{
	    "txt",          // HTTP_TXT
	    "htm",          // HTTP_HTM
	    "html",         // HTTP_HTML
	    "cgi",          // HTTP_CGI
	    "xml",          // HTTP_XML
	    "css",          // HTTP_CSS
	    "gif",          // HTTP_GIF
	    "png",          // HTTP_PNG
	    "jpg",          // HTTP_JPG
	    "cla",          // HTTP_JAVA
	    "wav",          // HTTP_WAV
		"\0\0\0"		// HTTP_UNKNOWN
	};
	
	// Content-type strings corresponding to HTTP_FILE_TYPE
	static ROM char * ROM httpContentTypes[HTTP_UNKNOWN+1] =
	{
	    "text/plain",            // HTTP_TXT
	    "text/html",             // HTTP_HTM
	    "text/html",             // HTTP_HTML
	    "text/html",             // HTTP_CGI
	    "text/xml",              // HTTP_XML
	    "text/css",              // HTTP_CSS
	    "image/gif",             // HTTP_GIF
	    "image/png",             // HTTP_PNG
	    "image/jpeg",            // HTTP_JPG
	    "application/java-vm",   // HTTP_JAVA
	    "audio/x-wave",          // HTTP_WAV
		""						 // HTTP_UNKNOWN
	};
		
/****************************************************************************
  Section:
	Commands and Server Responses
  ***************************************************************************/

	// Initial response strings (Corresponding to HTTP_STATUS)
	static ROM char * ROM HTTPResponseHeaders[] =
	{
		"HTTP/1.1 200 OK\r\nConnection: close\r\n",
		"HTTP/1.1 200 OK\r\nConnection: close\r\n",
		"HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n400 Bad Request: can't handle Content-Length\r\n",
		"HTTP/1.1 401 Unauthorized\r\nWWW-Authenticate: Basic realm=\"Protected\"\r\nConnection: close\r\n\r\n401 Unauthorized: Password required\r\n",
		#if defined(HTTP_MPFS_UPLOAD)
		"HTTP/1.1 404 Not found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n404: File not found<br>Use <a href=\"/" HTTP_MPFS_UPLOAD "\">MPFS Upload</a> to program web pages\r\n",
		#else		
		"HTTP/1.1 404 Not found\r\nConnection: close\r\n\r\n404: File not found\r\n",
		#endif
		"HTTP/1.1 414 Request-URI Too Long\r\nConnection: close\r\n\r\n414 Request-URI Too Long: Buffer overflow detected\r\n",
		"HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\n500 Internal Server Error: Expected data not present\r\n",
		"HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n\r\n501 Not Implemented: Only GET and POST supported\r\n",
		#if defined(HTTP_MPFS_UPLOAD)
		"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><form method=post action=\"/" HTTP_MPFS_UPLOAD "\" enctype=\"multipart/form-data\"><b>MPFS Image Upload</b><p><input type=file name=i size=40> &nbsp; <input type=submit value=\"Upload\"></form></body></html>",
		"",
		"HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><b>MPFS Update Successful</b><p><a href=\"/\">Site main page</a></body></html>",
		"HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><body style=\"margin:100px\"><b>MPFS Image Corrupt or Wrong Version</b><p><a href=\"/" HTTP_MPFS_UPLOAD "\">Try again?</a></body></html>",
		#endif
		"HTTP/1.1 302 Found\r\nConnection: close\r\nLocation: ",
		"HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n403 Forbidden: SSL Required - use HTTPS\r\n"
	};
	
/****************************************************************************
  Section:
	Header Parsing Configuration
  ***************************************************************************/
	
	// Header strings for which we'd like to parse
	static ROM char * ROM HTTPRequestHeaders[] =
	{
		"Cookie:",
		"Authorization:",
		"Content-Length:"
	};
	
	// Set to length of longest string above
	#define HTTP_MAX_HEADER_LEN		(15u)

/****************************************************************************
  Section:
	HTTP Connection State Global Variables
  ***************************************************************************/
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata HTTP_CONNECTION_STATES
	#endif
	#if defined(HTTP_SAVE_CONTEXT_IN_PIC_RAM)
		HTTP_CONN					HTTPControlBlocks[MAX_HTTP_CONNECTIONS];
		#define HTTPLoadConn(a)		do{curHTTPID = (a);}while(0)
	#else
		HTTP_CONN curHTTP;							// Current HTTP connection state
		static void HTTPLoadConn(BYTE hHTTP);
	#endif
	HTTP_STUB httpStubs[MAX_HTTP_CONNECTIONS];	// HTTP stubs with state machine and socket
	BYTE curHTTPID;								// ID of the currently loaded HTTP_CONN
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata
	#endif

/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/
	static void HTTPHeaderParseLookup(BYTE i);
	#if defined(HTTP_USE_COOKIES)
	static void HTTPHeaderParseCookie(void);
	#endif
	#if defined(HTTP_USE_AUTHENTICATION)
	static void HTTPHeaderParseAuthorization(void);
	#endif
	#if defined(HTTP_USE_POST)
	static void HTTPHeaderParseContentLength(void);
	static HTTP_READ_STATUS HTTPReadTo(BYTE delim, BYTE* buf, WORD len);
	#endif
	
	static void HTTPProcess(void);
	static BOOL HTTPSendFile(void);

	#if defined(HTTP_MPFS_UPLOAD)
	static HTTP_IO_RESULT HTTPMPFSUpload(void);
	#endif

	#define mMIN(a, b)	((a<b)?a:b)
	#define smHTTP		httpStubs[curHTTPID].sm			// Access the current state machine

/*****************************************************************************
  Function:
	void HTTPInit(void)

  Summary:
	Initializes the HTTP server module.

  Description:
	Sets all HTTP sockets to the listening state, and initializes the
	state machine and file handles for each connection.  If SSL is
	enabled, opens a socket on that port as well.

  Precondition:
	TCP must already be initialized.

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function is called only one during lifetime of the application.
  ***************************************************************************/
void HTTPInit(void)
{
    for(curHTTPID = 0; curHTTPID < MAX_HTTP_CONNECTIONS; curHTTPID++)
    {
		smHTTP = SM_HTTP_IDLE;
		sktHTTP = TCPOpen(0, TCP_OPEN_SERVER, HTTP_PORT, TCP_PURPOSE_HTTP_SERVER);
		#if defined(STACK_USE_SSL_SERVER)
		TCPAddSSLListener(sktHTTP, HTTPS_PORT);
		#endif
		
	    // Save the default record (just invalid file handles)
		curHTTP.file = MPFS_INVALID_HANDLE;
		curHTTP.offsets = MPFS_INVALID_HANDLE;
		#if !defined(HTTP_SAVE_CONTEXT_IN_PIC_RAM)
		{
			PTR_BASE oldPtr;

			oldPtr = MACSetWritePtr(BASE_HTTPB_ADDR + curHTTPID*sizeof(HTTP_CONN));
			MACPutArray((BYTE*)&curHTTP, sizeof(HTTP_CONN));
			MACSetWritePtr(oldPtr);
		}
		#endif
    }

	// Set curHTTPID to zero so that first call to HTTPLoadConn() doesn't write 
	// dummy data outside reserved HTTP memory.
    curHTTPID = 0;	
}


/*****************************************************************************
  Function:
	void HTTPServer(void)

  Summary:
	Performs periodic tasks for the HTTP2 module.

  Description:
	Browses through each open connection and attempts to process any
	pending operations.

  Precondition:
	HTTPInit() must already be called.

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function acts as a task (similar to one in an RTOS).  It
	performs its task in a co-operative manner, and the main application
	must call this function repeatedly to ensure that all open or new
	connections are served in a timely fashion.
  ***************************************************************************/
void HTTPServer(void)
{
	BYTE conn;

	for(conn = 0; conn < MAX_HTTP_CONNECTIONS; conn++)
	{
		if(httpStubs[conn].socket == INVALID_SOCKET)
			continue;
		
		// If a socket is disconnected at any time 
		// forget about it and return to idle state.
		// Must do this here, otherwise we will wait until a new
		// connection arrives, which causes problems with Linux and with SSL
		if(TCPWasReset(httpStubs[conn].socket))
		{
			HTTPLoadConn(conn);
			smHTTP = SM_HTTP_IDLE;

			// Make sure any opened files are closed
			if(curHTTP.file != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.file);
				curHTTP.file = MPFS_INVALID_HANDLE;
			}
			if(curHTTP.offsets != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.offsets);
				curHTTP.offsets = MPFS_INVALID_HANDLE;
			}

			// Adjust FIFO sizes to half and half.  Default state must remain
			// here so that SSL handshakes, if required, can proceed
			TCPAdjustFIFOSize(sktHTTP, 1, 0, TCP_ADJUST_PRESERVE_RX);
		}
		
		// Determine if this connection is eligible for processing
		if(httpStubs[conn].sm != SM_HTTP_IDLE || TCPIsGetReady(httpStubs[conn].socket))
		{
			HTTPLoadConn(conn);
			HTTPProcess();
		}
	}
}

/*****************************************************************************
  Function:
	static void HTTPLoadConn(BYTE hHTTP)

  Summary:
	Switches the currently loaded connection for the HTTP2 module.

  Description:
	Saves the currently loaded HTTP connection back to Ethernet buffer
	RAM, then loads the selected connection into curHTTP in local RAM
	for processing.

  Precondition:
	None

  Parameters:
	hHTTP - the connection ID to load

  Returns:
  	None
  ***************************************************************************/
#if !defined(HTTP_SAVE_CONTEXT_IN_PIC_RAM)
static void HTTPLoadConn(BYTE hHTTP)
{
    WORD oldPtr;
    
    // Return if already loaded
    if(hHTTP == curHTTPID)
    	return;
    
    // Save the old one
    oldPtr = MACSetWritePtr(BASE_HTTPB_ADDR + curHTTPID*sizeof(HTTP_CONN));
	MACPutArray((BYTE*)&curHTTP, sizeof(HTTP_CONN));
	MACSetWritePtr(oldPtr);
	
	// Load the new one
    oldPtr = MACSetReadPtr(BASE_HTTPB_ADDR + hHTTP*sizeof(HTTP_CONN));
	MACGetArray((BYTE*)&curHTTP, sizeof(HTTP_CONN));
	MACSetReadPtr(oldPtr);
	
	// Remember which one is loaded
	curHTTPID = hHTTP;
			
}
#endif

/*****************************************************************************
  Function:
	static void HTTPProcess(void)

  Description:
	Performs any pending operations for the currently loaded HTTP connection.

  Precondition:
	HTTPInit() and HTTPLoadConn() have been called.

  Parameters:
	None

  Returns:
  	None
  ***************************************************************************/
static void HTTPProcess(void)
{
    WORD lenA, lenB;
	BYTE c, i;
    BOOL isDone;
	BYTE *ext;
	BYTE buffer[HTTP_MAX_HEADER_LEN+1];

    do
    {
        isDone = TRUE;

        switch(smHTTP)
        {

        case SM_HTTP_IDLE:

			// Check how much data is waiting
			lenA = TCPIsGetReady(sktHTTP);

			// If a connection has been made, then process the request
            if(lenA)
            {// Clear out state info and move to next state
				curHTTP.ptrData = curHTTP.data;
				smHTTP = SM_HTTP_PARSE_REQUEST;
				curHTTP.isAuthorized = 0xff;
				curHTTP.hasArgs = FALSE;
				curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;
				curHTTP.callbackPos = 0xffffffff;
				curHTTP.byteCount = 0;
				#if defined(HTTP_USE_POST)
				curHTTP.smPost = 0x00;
				#endif
				
				// Adjust the TCP FIFOs for optimal reception of 
				// the next HTTP request from the browser
				TCPAdjustFIFOSize(sktHTTP, 1, 0, TCP_ADJUST_PRESERVE_RX | TCP_ADJUST_GIVE_REST_TO_RX);
 			}
 			else
 				// Don't break for new connections.  There may be 
 				// an entire request in the buffer already.
 				break;

		case SM_HTTP_PARSE_REQUEST:

			// Verify the entire first line is in the FIFO
			if(TCPFind(sktHTTP, '\n', 0, FALSE) == 0xffff)
			{// First line isn't here yet
				if(TCPGetRxFIFOFree(sktHTTP) == 0u)
				{// If the FIFO is full, we overflowed
					curHTTP.httpStatus = HTTP_OVERFLOW;
					smHTTP = SM_HTTP_SERVE_HEADERS;
					isDone = FALSE;
				}
				if((LONG)(TickGet() - curHTTP.callbackID) > (LONG)0)
				{// A timeout has occurred
					TCPDisconnect(sktHTTP);
					smHTTP = SM_HTTP_DISCONNECT;
					isDone = FALSE;
				}
				break;
			}

			// Reset the watchdog timer
			curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;

			// Determine the request method
			lenA = TCPFind(sktHTTP, ' ', 0, FALSE);
			if(lenA > 5u)
				lenA = 5;
			TCPGetArray(sktHTTP, curHTTP.data, lenA+1);

		    if ( memcmppgm2ram(curHTTP.data, (ROM void*)"GET", 3) == 0)
			    curHTTP.httpStatus = HTTP_GET;
			#if defined(HTTP_USE_POST)
		    else if ( memcmppgm2ram(curHTTP.data, (ROM void*)"POST", 4) == 0)
			    curHTTP.httpStatus = HTTP_POST;
			#endif
		    else
			{// Unrecognized method, so return not implemented
		        curHTTP.httpStatus = HTTP_NOT_IMPLEMENTED;
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
				break;
			}

			// Find end of filename
			lenA = TCPFind(sktHTTP, ' ', 0, FALSE);
			lenB = TCPFindEx(sktHTTP, '?', 0, lenA, FALSE);
			lenA = mMIN(lenA, lenB);
			
			// If the file name is too long, then reject the request
			if(lenA > HTTP_MAX_DATA_LEN - HTTP_DEFAULT_LEN - 1)
			{
				curHTTP.httpStatus = HTTP_OVERFLOW;
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
				break;
			}

			// Read in the filename and decode
			lenB = TCPGetArray(sktHTTP, curHTTP.data, lenA);
			curHTTP.data[lenB] = '\0';
			HTTPURLDecode(curHTTP.data);

			// Decode may have changed the string length - update it here
			lenB = strlen((char*)curHTTP.data);

			// Check if this is an MPFS Upload
			#if defined(HTTP_MPFS_UPLOAD)
			if(memcmppgm2ram(&curHTTP.data[1], HTTP_MPFS_UPLOAD, sizeof(HTTP_MPFS_UPLOAD)) == 0)
			{// Read remainder of line, and bypass all file opening, etc.
				#if defined(HTTP_USE_AUTHENTICATION)
				curHTTP.isAuthorized = HTTPNeedsAuth(&curHTTP.data[1]);
				#endif
				if(curHTTP.httpStatus == HTTP_GET)
					curHTTP.httpStatus = HTTP_MPFS_FORM;
				else
					curHTTP.httpStatus = HTTP_MPFS_UP;

				smHTTP = SM_HTTP_PARSE_HEADERS;
				isDone = FALSE;
				break;
			}
			#endif
			
			// If the last character is a not a directory delimiter, then try to open the file
			// String starts at 2nd character, because the first is always a '/'
			if(curHTTP.data[lenB-1] != '/')
				curHTTP.file = MPFSOpen(&curHTTP.data[1]);
				
			// If the open fails, then add our default name and try again
			if(curHTTP.file == MPFS_INVALID_HANDLE)
			{
				// Add the directory delimiter if needed
				if(curHTTP.data[lenB-1] != '/')
					curHTTP.data[lenB++] = '/';
				
				// Add our default file name			
				#if defined(STACK_USE_SSL_SERVER)
				if(TCPIsSSL(sktHTTP))
				{
					strcpypgm2ram((void*)&curHTTP.data[lenB], HTTPS_DEFAULT_FILE);
					lenB += strlenpgm(HTTPS_DEFAULT_FILE);
				}
				else
				#endif
				{
					strcpypgm2ram((void*)&curHTTP.data[lenB], HTTP_DEFAULT_FILE);
					lenB += strlenpgm(HTTP_DEFAULT_FILE);
				}
					
				// Try to open again
				curHTTP.file = MPFSOpen(&curHTTP.data[1]);
			}
			
			// Find the extension in the filename
			for(ext = curHTTP.data + lenB-1; ext != curHTTP.data; ext--)
				if(*ext == '.')
					break;
					
			// Compare to known extensions to determine Content-Type
			ext++;
			for(curHTTP.fileType = HTTP_TXT; curHTTP.fileType < HTTP_UNKNOWN; curHTTP.fileType++)
				if(!stricmppgm2ram(ext, (ROM void*)httpFileExtensions[curHTTP.fileType]))
					break;
			
			// Perform first round authentication (pass file name only)
			#if defined(HTTP_USE_AUTHENTICATION)
			curHTTP.isAuthorized = HTTPNeedsAuth(&curHTTP.data[1]);
			#endif
			
			// If the file was found, see if it has an index
			if(curHTTP.file != MPFS_INVALID_HANDLE &&
				(MPFSGetFlags(curHTTP.file) & MPFS2_FLAG_HASINDEX) )
			{
				curHTTP.offsets = MPFSOpenID(MPFSGetID(curHTTP.file) + 1);
			}

			// Read GET args, up to buffer size - 1
			lenA = TCPFind(sktHTTP, ' ', 0, FALSE);
			if(lenA != 0u)
			{
				curHTTP.hasArgs = TRUE;
				
				// Trash the '?'
				TCPGet(sktHTTP, &c);

				// Verify there's enough space
				lenA--;
				if(lenA >= HTTP_MAX_DATA_LEN - 2)
				{
			        curHTTP.httpStatus = HTTP_OVERFLOW;
					smHTTP = SM_HTTP_SERVE_HEADERS;
					isDone = FALSE;
					break;
				}

				// Read in the arguments and '&'-terminate in anticipation of cookies
				curHTTP.ptrData += TCPGetArray(sktHTTP, curHTTP.data, lenA);
				*(curHTTP.ptrData++) = '&';

			}

			// Clear the rest of the line
			lenA = TCPFind(sktHTTP, '\n', 0, FALSE);
			TCPGetArray(sktHTTP, NULL, lenA + 1);

			// Move to parsing the headers
			smHTTP = SM_HTTP_PARSE_HEADERS;
			
			// No break, continue to parsing headers

		case SM_HTTP_PARSE_HEADERS:

			// Loop over all the headers
			while(1)
			{
				// Make sure entire line is in the FIFO
				lenA = TCPFind(sktHTTP, '\n', 0, FALSE);
				if(lenA == 0xffff)
				{// If not, make sure we can receive more data
					if(TCPGetRxFIFOFree(sktHTTP) == 0u)
					{// Overflow
						curHTTP.httpStatus = HTTP_OVERFLOW;
						smHTTP = SM_HTTP_SERVE_HEADERS;
						isDone = FALSE;
					}
					if((LONG)(TickGet() - curHTTP.callbackID) > (LONG)0)
					{// A timeout has occured
						TCPDisconnect(sktHTTP);
						smHTTP = SM_HTTP_DISCONNECT;
						isDone = FALSE;
					}
					break;
				}
				
				// Reset the watchdog timer
				curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;
				
				// If a CRLF is immediate, then headers are done
				if(lenA == 1u)
				{// Remove the CRLF and move to next state
					TCPGetArray(sktHTTP, NULL, 2);
					smHTTP = SM_HTTP_AUTHENTICATE;
					isDone = FALSE;
					break;
				}
	
				// Find the header name, and use isDone as a flag to indicate a match
				lenB = TCPFindEx(sktHTTP, ':', 0, lenA, FALSE) + 2;
				isDone = FALSE;
	
				// If name is too long or this line isn't a header, ignore it
				if(lenB > sizeof(buffer))
				{
					TCPGetArray(sktHTTP, NULL, lenA+1);
					continue;
				}
				
				// Read in the header name
				TCPGetArray(sktHTTP, buffer, lenB);
				buffer[lenB-1] = '\0';
				lenA -= lenB;
		
				// Compare header read to ones we're interested in
				for(i = 0; i < sizeof(HTTPRequestHeaders)/sizeof(HTTPRequestHeaders[0]); i++)
				{
					if(strcmppgm2ram((char*)buffer, (ROM char *)HTTPRequestHeaders[i]) == 0)
					{// Parse the header and stop the loop
						HTTPHeaderParseLookup(i);
						isDone = TRUE;
						break;
					}
				}
				
				// Clear the rest of the line, and call the loop again
				if(isDone)
				{// We already know how much to remove unless a header was found
					lenA = TCPFind(sktHTTP, '\n', 0, FALSE);
				}
				TCPGetArray(sktHTTP, NULL, lenA+1);
			}
			
			break;

		case SM_HTTP_AUTHENTICATE:
		
			#if defined(HTTP_USE_AUTHENTICATION)
			// Check current authorization state
			if(curHTTP.isAuthorized < 0x80)
			{// 401 error
				curHTTP.httpStatus = HTTP_UNAUTHORIZED;
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
				
				#if defined(HTTP_NO_AUTH_WITHOUT_SSL)
				if(!TCPIsSSL(sktHTTP))
					curHTTP.httpStatus = HTTP_SSL_REQUIRED;
				#endif

				break;
			}
			#endif

			// Parse the args string
			*curHTTP.ptrData = '\0';
			curHTTP.ptrData = HTTPURLDecode(curHTTP.data);

			// If this is an MPFS upload form request, bypass to headers
			#if defined(HTTP_MPFS_UPLOAD)
			if(curHTTP.httpStatus == HTTP_MPFS_FORM)
			{
				smHTTP = SM_HTTP_SERVE_HEADERS;
				isDone = FALSE;
				break;
			}
			#endif
			
			// Move on to GET args, unless there are none
			smHTTP = SM_HTTP_PROCESS_GET;
			if(!curHTTP.hasArgs)
				smHTTP = SM_HTTP_PROCESS_POST;
			isDone = FALSE;
			curHTTP.hasArgs = FALSE;
			break;

		case SM_HTTP_PROCESS_GET:

			// Run the application callback HTTPExecuteGet()
			if(HTTPExecuteGet() == HTTP_IO_WAITING)
			{// If waiting for asynchronous process, return to main app
				break;
			}

			// Move on to POST data
			smHTTP = SM_HTTP_PROCESS_POST;

		case SM_HTTP_PROCESS_POST:

			#if defined(HTTP_USE_POST)
			
			// See if we have any new data
			if(TCPIsGetReady(sktHTTP) == curHTTP.callbackPos)
			{
				if((LONG)(TickGet() - curHTTP.callbackID) > (LONG)0)
				{// If a timeout has occured, disconnect
					TCPDisconnect(sktHTTP);
					smHTTP = SM_HTTP_DISCONNECT;
					isDone = FALSE;
					break;
				}
			}
			
			if(curHTTP.httpStatus == HTTP_POST 
				#if defined(HTTP_MPFS_UPLOAD)
				|| (curHTTP.httpStatus >= HTTP_MPFS_UP && curHTTP.httpStatus <= HTTP_MPFS_ERROR)
				#endif
				 )
			{
				// Run the application callback HTTPExecutePost()
				#if defined(HTTP_MPFS_UPLOAD)
				if(curHTTP.httpStatus >= HTTP_MPFS_UP && curHTTP.httpStatus <= HTTP_MPFS_ERROR)
				{
					c = HTTPMPFSUpload();
					if(c == (BYTE)HTTP_IO_DONE)
					{
						smHTTP = SM_HTTP_SERVE_HEADERS;
						isDone = FALSE;
						break;
					}
				}
				else
				#endif
				c = HTTPExecutePost();
				
				// If waiting for asynchronous process, return to main app
				if(c == (BYTE)HTTP_IO_WAITING)
				{// return to main app and make sure we don't get stuck by the watchdog
					curHTTP.callbackPos = TCPIsGetReady(sktHTTP) - 1;
					break;
				}
				else if(c == (BYTE)HTTP_IO_NEED_DATA)
				{// If waiting for more data
					curHTTP.callbackPos = TCPIsGetReady(sktHTTP);
					curHTTP.callbackID = TickGet() + HTTP_TIMEOUT*TICK_SECOND;
					
					// If more is expected and space is available, return to main app
					if(curHTTP.byteCount > curHTTP.callbackPos && TCPGetRxFIFOFree(sktHTTP) != 0u)
						break;
					
					// Handle cases where application ran out of data or buffer space
					curHTTP.httpStatus = HTTP_INTERNAL_SERVER_ERROR;
					smHTTP = SM_HTTP_SERVE_HEADERS;
					isDone = FALSE;
					break;	
				}
			}
			#endif

			// We're done with POST
			smHTTP = SM_HTTP_PROCESS_REQUEST;
			// No break, continue to sending request

		case SM_HTTP_PROCESS_REQUEST:

			// Check for 404
            if(curHTTP.file == MPFS_INVALID_HANDLE)
            {
                curHTTP.httpStatus = HTTP_NOT_FOUND;
                smHTTP = SM_HTTP_SERVE_HEADERS;
                isDone = FALSE;
                break;
            }

			// Set up the dynamic substitutions
			curHTTP.byteCount = 0;
			if(curHTTP.offsets == MPFS_INVALID_HANDLE)
            {// If no index file, then set next offset to huge
	            curHTTP.nextCallback = 0xffffffff;
            }
            else
            {// Read in the next callback index
	            MPFSGetLong(curHTTP.offsets, &(curHTTP.nextCallback));
			}
			
			// Move to next state
			smHTTP = SM_HTTP_SERVE_HEADERS;

		case SM_HTTP_SERVE_HEADERS:

			// We're in write mode now:
			// Adjust the TCP FIFOs for optimal transmission of 
			// the HTTP response to the browser
			TCPAdjustFIFOSize(sktHTTP, 1, 0, TCP_ADJUST_GIVE_REST_TO_TX);
				
			// Send headers
			TCPPutROMString(sktHTTP, (ROM BYTE*)HTTPResponseHeaders[curHTTP.httpStatus]);
			
			// If this is a redirect, print the rest of the Location: header			   
			if(curHTTP.httpStatus == HTTP_REDIRECT)
			{
				TCPPutString(sktHTTP, curHTTP.data);
				TCPPutROMString(sktHTTP, (ROM BYTE*)"\r\n\r\n304 Redirect: ");
				TCPPutString(sktHTTP, curHTTP.data);
				TCPPutROMString(sktHTTP, (ROM BYTE*)HTTP_CRLF);
			}

			// If not GET or POST, we're done
			if(curHTTP.httpStatus != HTTP_GET && curHTTP.httpStatus != HTTP_POST)
			{// Disconnect
				smHTTP = SM_HTTP_DISCONNECT;
				break;
			}

			// Output the content type, if known
			if(curHTTP.fileType != HTTP_UNKNOWN)
			{
				TCPPutROMString(sktHTTP, (ROM BYTE*)"Content-Type: ");
				TCPPutROMString(sktHTTP, (ROM BYTE*)httpContentTypes[curHTTP.fileType]);
				TCPPutROMString(sktHTTP, HTTP_CRLF);
			}
			
			// Output the gzip encoding header if needed
			if(MPFSGetFlags(curHTTP.file) & MPFS2_FLAG_ISZIPPED)
			{
				TCPPutROMString(sktHTTP, (ROM BYTE*)"Content-Encoding: gzip\r\n");
			}
						
			// Output the cache-control
			TCPPutROMString(sktHTTP, (ROM BYTE*)"Cache-Control: ");
			if(curHTTP.httpStatus == HTTP_POST || curHTTP.nextCallback != 0xffffffff)
			{// This is a dynamic page or a POST request, so no cache
				TCPPutROMString(sktHTTP, (ROM BYTE*)"no-cache");
			}
			else
			{// This is a static page, so save it for the specified amount of time
				TCPPutROMString(sktHTTP, (ROM BYTE*)"max-age=");
				TCPPutROMString(sktHTTP, (ROM BYTE*)HTTP_CACHE_LEN);
			}
			TCPPutROMString(sktHTTP, HTTP_CRLF);
			
			// Check if we should output cookies
			if(curHTTP.hasArgs)
				smHTTP = SM_HTTP_SERVE_COOKIES;
			else
			{// Terminate the headers
				TCPPutROMString(sktHTTP, HTTP_CRLF);
				smHTTP = SM_HTTP_SERVE_BODY;
			}
	
			// Move to next stage
			isDone = FALSE;
			break;

		case SM_HTTP_SERVE_COOKIES:

			#if defined(HTTP_USE_COOKIES)
			// If the TX FIFO runs out of space, the client will never get CRLFCRLF
			// Avoid writing huge cookies - keep it under a hundred bytes max

			// Write cookies one at a time as space permits
			for(curHTTP.ptrRead = curHTTP.data; curHTTP.hasArgs != 0u; curHTTP.hasArgs--)
			{
				// Write the header
				TCPPutROMString(sktHTTP, (ROM BYTE*)"Set-Cookie: ");

				// Write the name, URL encoded, one character at a time
				while((c = *(curHTTP.ptrRead++)))
				{
					if(c == ' ')
						TCPPut(sktHTTP, '+');
					else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
					{
						TCPPut(sktHTTP, '%');
						TCPPut(sktHTTP, btohexa_high(c));
						TCPPut(sktHTTP, btohexa_low(c));
					}
					else
						TCPPut(sktHTTP, c);
				}
				
				TCPPut(sktHTTP, '=');
				
				// Write the value, URL encoded, one character at a time
				while((c = *(curHTTP.ptrRead++)))
				{
					if(c == ' ')
						TCPPut(sktHTTP, '+');
					else if(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a') || c > 'z')
					{
						TCPPut(sktHTTP, '%');
						TCPPut(sktHTTP, btohexa_high(c));
						TCPPut(sktHTTP, btohexa_low(c));
					}
					else
						TCPPut(sktHTTP, c);
				}
				
				// Finish the line
				TCPPutROMString(sktHTTP, HTTP_CRLF);

			}
			#endif

			// We're done, move to next state
			TCPPutROMString(sktHTTP, HTTP_CRLF);
			smHTTP = SM_HTTP_SERVE_BODY;

		case SM_HTTP_SERVE_BODY:

			isDone = FALSE;

			// Try to send next packet
			if(HTTPSendFile())
			{// If EOF, then we're done so close and disconnect
				MPFSClose(curHTTP.file);
				curHTTP.file = MPFS_INVALID_HANDLE;
				smHTTP = SM_HTTP_DISCONNECT;
				isDone = TRUE;
			}
			
			// If the TX FIFO is full, then return to main app loop
			if(TCPIsPutReady(sktHTTP) == 0u)
				isDone = TRUE;
            break;

		case SM_HTTP_SEND_FROM_CALLBACK:

			isDone = TRUE;

			// Check that at least the minimum bytes are free
			if(TCPIsPutReady(sktHTTP) < HTTP_MIN_CALLBACK_FREE)
				break;

			// Fill TX FIFO from callback
			HTTPPrint(curHTTP.callbackID);
			
			if(curHTTP.callbackPos == 0u)
			{// Callback finished its output, so move on
				isDone = FALSE;
				smHTTP = SM_HTTP_SERVE_BODY;
			}// Otherwise, callback needs more buffer space, so return and wait
			
			break;

		case SM_HTTP_DISCONNECT:
			// Make sure any opened files are closed
			if(curHTTP.file != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.file);
				curHTTP.file = MPFS_INVALID_HANDLE;
			}
			if(curHTTP.offsets != MPFS_INVALID_HANDLE)
			{
				MPFSClose(curHTTP.offsets);
				curHTTP.offsets = MPFS_INVALID_HANDLE;
			}

			TCPDisconnect(sktHTTP);
            smHTTP = SM_HTTP_IDLE;
            break;
		}
	} while(!isDone);

}


/*****************************************************************************
  Function:
	static BOOL HTTPSendFile(void)

  Description:
	Serves up the next chunk of curHTTP's file, up to a) available TX FIFO
	space or b) the next callback index, whichever comes first.

  Precondition:
	curHTTP.file and curHTTP.offsets have both been opened for reading.

  Parameters:
	None

  Return Values:
	TRUE - the end of the file was reached and reading is done
	FALSE - more data remains to be read
  ***************************************************************************/
static BOOL HTTPSendFile(void)
{
	WORD numBytes, len;
	BYTE c, data[64];
	
	// Determine how many bytes we can read right now
	len = TCPIsPutReady(sktHTTP);
	numBytes = mMIN(len, curHTTP.nextCallback - curHTTP.byteCount);
	
	// Get/put as many bytes as possible
	curHTTP.byteCount += numBytes;
	while(numBytes > 0u)
	{
		len = MPFSGetArray(curHTTP.file, data, mMIN(numBytes, sizeof(data)));
		if(len == 0u)
			return TRUE;
		else
			TCPPutArray(sktHTTP, data, len);
		numBytes -= len;
	}
	
	// Check if a callback index was reached
	if(curHTTP.byteCount == curHTTP.nextCallback)
	{
		// Update the state machine
		smHTTP = SM_HTTP_SEND_FROM_CALLBACK;
		curHTTP.callbackPos = 0;

		// Read past the variable name and close the MPFS
		MPFSGet(curHTTP.file, NULL);
		do
		{
			if(!MPFSGet(curHTTP.file, &c))
				break;
			curHTTP.byteCount++;
		} while(c != '~');
		curHTTP.byteCount++;
		
		// Read in the callback address and next offset
		MPFSGetLong(curHTTP.offsets, &(curHTTP.callbackID));
		if(!MPFSGetLong(curHTTP.offsets, &(curHTTP.nextCallback)))
		{
			curHTTP.nextCallback = 0xffffffff;
			MPFSClose(curHTTP.offsets);
			curHTTP.offsets = MPFS_INVALID_HANDLE;
		}
	}

    // We are not done sending a file yet...
    return FALSE;
}

/*****************************************************************************
  Function:
	static void HTTPHeaderParseLookup(BYTE i)

  Description:
	Calls the appropriate header parser based on the index of the header
	that was read from the request.

  Precondition:
	None

  Parameters:
	i - the index of the string found in HTTPRequestHeaders

  Return Values:
	TRUE - the end of the file was reached and reading is done
	FALSE - more data remains to be read
  ***************************************************************************/
static void HTTPHeaderParseLookup(BYTE i)
{
	// i corresponds to an index in HTTPRequestHeaders
	
	#if defined(HTTP_USE_COOKIES)
	if(i == 0u)
	{
		HTTPHeaderParseCookie();
		return;
	}
	#endif
	
	#if defined(HTTP_USE_AUTHENTICATION)	
	if(i == 1u)
	{
		HTTPHeaderParseAuthorization();
		return;
	}
	#endif
	
	#if defined(HTTP_USE_POST)
	if(i == 2u)
	{
		HTTPHeaderParseContentLength();
		return;
	}
	#endif
}

/*****************************************************************************
  Function:
	static void HTTPHeaderParseAuthorization(void)

  Summary:
	Parses the "Authorization:" header for a request and verifies the
	credentials.

  Description:
	Parses the "Authorization:" header for a request.  For example, 
	"BASIC YWRtaW46cGFzc3dvcmQ=" is decoded to a user name of "admin" and
	a password of "password".  Once read, HTTPCheckAuth is called from
	CustomHTTPApp.c to determine if the credentials are acceptable.

	The return value of HTTPCheckAuth is saved in curHTTP.isAuthorized for
	later use by the application.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
	This function is ony available when HTTP_USE_AUTHENTICATION is defined.
  ***************************************************************************/
#if defined(HTTP_USE_AUTHENTICATION)
static void HTTPHeaderParseAuthorization(void)
{
    WORD len;
    BYTE buf[40];
	BYTE *ptrBuf;
	
	// If auth processing is not required, return
	if(curHTTP.isAuthorized & 0x80)
		return;

	// Clear the auth type ("BASIC ")
	TCPGetArray(sktHTTP, NULL, 6);

	// Find the terminating CRLF and make sure it's a multiple of four
	len = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	len += 3;
	len &= 0xfc;
	len = mMIN(len, sizeof(buf)-4);
	
	// Read in 4 bytes at a time and decode (slower, but saves RAM)
	for(ptrBuf = buf; len > 0u; len-=4, ptrBuf+=3)
	{
		TCPGetArray(sktHTTP, ptrBuf, 4);
		Base64Decode(ptrBuf, 4, ptrBuf, 3);
	}

	// Null terminate both, and make sure there's at least two terminators
	*ptrBuf = '\0';
	for(len = 0, ptrBuf = buf; len < sizeof(buf); len++, ptrBuf++)
		if(*ptrBuf == ':')
			break;
	*(ptrBuf++) = '\0';
	
	// Verify credentials
	curHTTP.isAuthorized = HTTPCheckAuth(buf, ptrBuf);

	return;
}
#endif

/*****************************************************************************
  Function:
	static void HTTPHeaderParseCookie(void)

  Summary:
	Parses the "Cookie:" headers for a request and stores them as GET
	variables.

  Description:
	Parses the "Cookie:" headers for a request.  For example, 
 	"Cookie: name=Wile+E.+Coyote; order=ROCKET_LAUNCHER" is decoded to 
	"name=Wile+E.+Coyote&order=ROCKET_LAUNCHER&" and stored as any other 
	GET variable in curHTTP.data.

	The user application can easily access these values later using the
	HTTPGetArg() and HTTPGetROMArg() functions.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
	This function is ony available when HTTP_USE_COOKIES is defined.
  ***************************************************************************/
#if defined(HTTP_USE_COOKIES)
static void HTTPHeaderParseCookie(void)
{
	WORD lenA, lenB;

	// Verify there's enough space
	lenB = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	if(lenB >= (WORD)(curHTTP.data + HTTP_MAX_DATA_LEN - curHTTP.ptrData - 2))
	{// If not, overflow
		curHTTP.httpStatus = HTTP_OVERFLOW;
		smHTTP = SM_HTTP_SERVE_HEADERS;
		return;
	}

	// While a CRLF is not immediate, grab a cookie value
	while(lenB != 0u)
	{
		// Look for a ';' and use the shorter of that or a CRLF
		lenA = TCPFind(sktHTTP, ';', 0, FALSE);
		
		// Read to the terminator
		curHTTP.ptrData += TCPGetArray(sktHTTP, curHTTP.ptrData, mMIN(lenA, lenB));
		
		// Insert an & to anticipate another cookie
		*(curHTTP.ptrData++) = '&';
		
		// If semicolon, trash it and whitespace
		if(lenA < lenB)
		{
			TCPGet(sktHTTP, NULL);
			while(TCPFind(sktHTTP, ' ', 0, FALSE) == 0u)
				TCPGet(sktHTTP, NULL);
		}
		
		// Find the new distance to the CRLF
		lenB = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	}

	return;

}
#endif

/*****************************************************************************
  Function:
	static void HTTPHeaderParseContentLength(void)

  Summary:
	Parses the "Content-Length:" header for a request.

  Description:
	Parses the "Content-Length:" header to determine how many bytes of
	POST data to expect after the request.  This value is stored in 
	curHTTP.byteCount.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
	This function is ony available when HTTP_USE_POST is defined.
  ***************************************************************************/
#if defined(HTTP_USE_POST)
static void HTTPHeaderParseContentLength(void)
{
	WORD len;
	BYTE buf[10];

	// Read up to the CRLF (max 9 bytes or ~1GB)
	len = TCPFindROMArray(sktHTTP, HTTP_CRLF, HTTP_CRLF_LEN, 0, FALSE);
	if(len >= sizeof(buf))
	{
		curHTTP.httpStatus = HTTP_BAD_REQUEST;
		curHTTP.byteCount = 0;
		return;
	}	
	len = TCPGetArray(sktHTTP, buf, len);
	buf[len] = '\0';
	
	curHTTP.byteCount = atol((char*)buf);
}
#endif

/*****************************************************************************
  Function:
	BYTE* HTTPURLDecode(BYTE* cData)

  Summary:
	Parses a string from URL encoding to plain-text.

  Description:
	Parses a string from URL encoding to plain-text.  The following
	conversions are made: ‘=’ to ‘\0’, ‘&’ to ‘\0’, ‘+’ to ‘ ‘, and
	“%xx” to a single hex byte.
 
	After completion, the data has been decoded and a null terminator
	signifies the end of a name or value.  A second null terminator (or a
	null name parameter) indicates the end of all the data.

  Precondition:
	The data parameter is null terminated and has at least one extra
	byte free.

  Parameters:
	cData - The string which is to be decoded in place.

  Returns:
	A pointer to the last null terminator in data, which is also the
	first free byte for new data.

  Remarks:
	This function is called by the stack to parse GET arguments and 
	cookie data.  User applications can use this function to decode POST
	data, but first need to verify that the string is null-terminated.
  ***************************************************************************/
BYTE* HTTPURLDecode(BYTE* cData)
{
	BYTE *pRead, *pWrite;
	WORD wLen;
	BYTE c;
	WORD hex;
	 
	// Determine length of input
	wLen = strlen((char*)cData);
	 
	// Read all characters in the string
	for(pRead = pWrite = cData; wLen != 0u; )
	{
		c = *pRead++;
		wLen--;
		
		if(c == '=' || c == '&')
			*pWrite++ = '\0';
		else if(c == '+')
			*pWrite++ = ' ';
		else if(c == '%')
		{
			if(wLen < 2u)
				wLen = 0;
			else
			{
				((BYTE*)&hex)[1] = *pRead++;
				((BYTE*)&hex)[0] = *pRead++;
				wLen--;
				wLen--;
				*pWrite++ = hexatob(*((WORD_VAL*)&hex));
			}
		}
		else
			*pWrite++ = c;
	}
	
	// Double null terminate the last value
	*pWrite++ = '\0';
	*pWrite = '\0';
	
	return pWrite;
}

/*****************************************************************************
  Function:
	BYTE* HTTPGetArg(BYTE* cData, BYTE* cArg)

  Summary:
	Locates a form field value in a given data array.

  Description:
	Searches through a data array to find the value associated with a
	given argument.  It can be used to find form field values in data
	received over GET or POST.
	
	The end of data is assumed to be reached when a null name parameter is
	encountered.  This requires the string to have an even number of 
	null-terminated strings, followed by an additional null terminator.

  Precondition:
	The data array has a valid series of null terminated name/value pairs.

  Parameters:
	data - the buffer to search
	arg - the name of the argument to find

  Returns:
	A pointer to the argument value, or NULL if not found.
  ***************************************************************************/
BYTE* HTTPGetArg(BYTE* cData, BYTE* cArg)
{
	// Search through the array while bytes remain
	while(*cData != '\0')
	{ 
		// Look for arg at current position
		if(!strcmp((char*)cArg, (char*)cData))
		{// Found it, so return parameter
			return cData + strlen((char*)cArg) + 1;
		}
		
		// Skip past two strings (NUL bytes)
		cData += strlen((char*)cData) + 1;
		cData += strlen((char*)cData) + 1;
	}
	 	
	// Return NULL if not found
	return NULL;
}

/*****************************************************************************
  Function:
	BYTE* HTTPGetROMArg(BYTE* cData, ROM BYTE* cArg)

  Summary:
	Locates a form field value in a given data array.

  Description:
	Searches through a data array to find the value associated with a
	given argument.  It can be used to find form field values in data
	received over GET or POST.
	
	The end of data is assumed to be reached when a null name parameter is
	encountered.  This requires the string to have an even number of 
	null-terminated strings, followed by an additional null terminator.

  Precondition:
	The data array has a valid series of null terminated name/value pairs.

  Parameters:
	data - the buffer to search
	arg - the name of the argument to find

  Returns:
	A pointer to the argument value, or NULL if not found.

  Remarks:
  	This function is aliased to HTTPGetArg on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
BYTE* HTTPGetROMArg(BYTE* cData, ROM BYTE* cArg)
{
	// Search through the array while bytes remain
	while(*cData != '\0')
	{
		// Look for arg at current position
		if(!memcmppgm2ram(cData, (ROM void*)cArg, strlenpgm((ROM char*)cArg) + 1))
		{// Found it, so skip to next string
			return cData + strlenpgm((ROM char*)cArg) + 1;
		}
		
		// Skip past two strings (NUL bytes)
		cData += strlen((char*)cData) + 1;
		cData += strlen((char*)cData) + 1;
	}
	 	
	// Return NULL if not found
	return NULL;
}
#endif

/*****************************************************************************
  Function:
	HTTP_READ_STATUS HTTPReadPostName(BYTE* cData, WORD wLen)

  Summary:
	Reads a name from a URL encoded string in the TCP buffer.

  Description:
	Reads a name from a URL encoded string in the TCP buffer.  This function
	is meant to be called from an HTTPExecutePost callback to facilitate
	easier parsing of incoming data.  This function also prevents buffer
	overflows by forcing the programmer to indicate how many bytes are
	expected.  At least 2 extra bytes are needed in cData over the maximum
	length of data expected to be read.
	
	This function will read until the next '=' character, which indicates the
	end of a name parameter.  It assumes that the front of the buffer is
	the beginning of the name paramter to be read.
	
	This function properly updates curHTTP.byteCount by decrementing it
	by the number of bytes read.  It also removes the delimiting '=' from
	the buffer.

  Precondition:
	Front of TCP buffer is the beginning of a name parameter, and the rest of
	the TCP buffer contains a URL-encoded string with a name parameter 
	terminated by a '=' character.

  Parameters:
	cData - where to store the name once it is read
	wLen - how many bytes can be written to cData

  Return Values:
	HTTP_READ_OK - name was successfully read
	HTTP_READ_TRUNCTATED - entire name could not fit in the buffer, so the
							value was truncated and data has been lost
	HTTP_READ_INCOMPLETE - entire name was not yet in the buffer, so call
							this function again later to retrieve
  ***************************************************************************/
#if defined(HTTP_USE_POST)
HTTP_READ_STATUS HTTPReadPostName(BYTE* cData, WORD wLen)
{
	HTTP_READ_STATUS status;
	
	status = HTTPReadTo('=', cData, wLen);

	// Decode the data (if not reading to null or blank) and return
	if(cData && *cData)
		HTTPURLDecode(cData);
	return status;
}	
#endif

/*****************************************************************************
  Function:
	HTTP_READ_STATUS HTTPReadPostValue(BYTE* cData, WORD wLen)

  Summary:
	Reads a value from a URL encoded string in the TCP buffer.

  Description:
	Reads a value from a URL encoded string in the TCP buffer.  This function
	is meant to be called from an HTTPExecutePost callback to facilitate
	easier parsing of incoming data.  This function also prevents buffer
	overflows by forcing the programmer to indicate how many bytes are
	expected.  At least 2 extra bytes are needed in cData above the maximum
	length of data expected to be read.
	
	This function will read until the next '&' character, which indicates the
	end of a value parameter.  It assumes that the front of the buffer is
	the beginning of the value paramter to be read.  If curHTTP.byteCount
	indicates that all expected bytes are in the buffer, it assumes that 
	all remaining data is the value and acts accordingly.
	
	This function properly updates curHTTP.byteCount by decrementing it
	by the number of bytes read.  The terminating '&' character is also 
	removed from the buffer.
	
  Precondition:
	Front of TCP buffer is the beginning of a name parameter, and the rest of
	the TCP buffer contains a URL-encoded string with a name parameter 
	terminated by a '=' character.

  Parameters:
	cData - where to store the value once it is read
	wLen - how many bytes can be written to cData

  Return Values:
	HTTP_READ_OK - value was successfully read
	HTTP_READ_TRUNCTATED - entire value could not fit in the buffer, so the
							value was truncated and data has been lost
	HTTP_READ_INCOMPLETE - entire value was not yet in the buffer, so call
							this function again later to retrieve
  ***************************************************************************/
#if defined(HTTP_USE_POST)
HTTP_READ_STATUS HTTPReadPostValue(BYTE* cData, WORD wLen)
{
	HTTP_READ_STATUS status;
	
	// Try to read the value
	status = HTTPReadTo('&', cData, wLen);
	
	// If read was incomplete, check if we're at the end
	if(status == HTTP_READ_INCOMPLETE)
	{
		// If all data has arrived, read all remaining data
		if(curHTTP.byteCount == TCPIsGetReady(sktHTTP))
			status = HTTPReadTo('\0', cData, wLen);
	}
		
	// Decode the data (if not reading to null or blank) and return
	if(cData && *cData)
		HTTPURLDecode(cData);
	return status;
}	
#endif

/*****************************************************************************
  Function:
	static HTTP_READ_STATUS HTTPReadTo(BYTE cDelim, BYTE* cData, WORD wLen)

  Summary:
	Reads to a buffer until a specified delimiter character.

  Description:
	Reads from the TCP buffer to cData until either cDelim is reached, or
	until wLen - 2 bytes have been read.  The value read is saved to cData and 
	null terminated.  (wLen - 2 is used so that the value can be passed to
	HTTPURLDecode later, which requires a null terminator plus one extra free
	byte.)
	
	The delimiter character is removed from the buffer, but not saved to 
	cData. If all data cannot fit into cData, it will still be removed from 
	the buffer but will not be saved anywhere.

	This function properly updates curHTTP.byteCount by decrementing it
	by the number of bytes read. 

  Precondition:
	None

  Parameters:
  	cDelim - the character at which to stop reading, or NULL to read to
  			 the end of the buffer
	cData - where to store the data being read
	wLen - how many bytes can be written to cData

  Return Values:
	HTTP_READ_OK - data was successfully read
	HTTP_READ_TRUNCTATED - entire data could not fit in the buffer, so the
							data was truncated and data has been lost
	HTTP_READ_INCOMPLETE - delimiter character was not found
  ***************************************************************************/
#if defined(HTTP_USE_POST)
static HTTP_READ_STATUS HTTPReadTo(BYTE cDelim, BYTE* cData, WORD wLen)
{
	HTTP_READ_STATUS status;
	WORD wPos;
	
	// Either look for delimiter, or read all available data
	if(cDelim)
		wPos = TCPFind(sktHTTP, cDelim, 0, FALSE);
	else
		wPos = TCPIsGetReady(sktHTTP);
	
	// If not found, return incomplete
	if(wPos == 0xffff)
		return HTTP_READ_INCOMPLETE;
	
	// Read the value
	if(wLen < 2u && cData != NULL)
	{// Buffer is too small, so read to NULL instead
		curHTTP.byteCount -= TCPGetArray(sktHTTP, NULL, wPos);
		status = HTTP_READ_TRUNCATED;
	}
	else if(cData == NULL)
	{// Just remove the data
		curHTTP.byteCount -= TCPGetArray(sktHTTP, NULL, wPos);
		status = HTTP_READ_OK;
	}
	else if(wPos > wLen - 2)
	{// Read data, but truncate at max length
		curHTTP.byteCount -= TCPGetArray(sktHTTP, cData, wLen - 2);
		curHTTP.byteCount -= TCPGetArray(sktHTTP, NULL, wPos - (wLen - 2));
		cData[wLen - 2] = '\0';
		status = HTTP_READ_TRUNCATED;
	}
	else
	{// Read the data normally
		curHTTP.byteCount -= TCPGetArray(sktHTTP, cData, wPos);
		cData[wPos] = '\0';
		status = HTTP_READ_OK;
	}
	
	// Remove the delimiter
	if(cDelim)
		curHTTP.byteCount -= TCPGet(sktHTTP, NULL);
	
	return status;
}	
#endif

/*****************************************************************************
  Function:
	HTTP_IO_RESULT HTTPMPFSUpload(void)

  Summary:
	Saves a file uploaded via POST as the new MPFS image in EEPROM or 
	external Flash.

  Description:
	Allows the MPFS image in EEPROM or external Flash to be updated via a 
	web page by accepting a file upload and storing it to the external memory.

  Precondition:
	MPFSFormat() has been called.

  Parameters:
	None

  Return Values:
	HTTP_IO_DONE - on success
	HTTP_IO_NEED_DATA - if more data is still expected

  Remarks:
	This function is only available when MPFS uploads are enabled and
	the MPFS image is stored in EEPROM.

  Internal:
	After the headers, the first line from the form will be the MIME
	separator.  Following that is more headers about the file, which
	are discarded.  After another CRLFCRLF pair the file data begins,
	which is read 16 bytes at a time and written to external memory.
  ***************************************************************************/
#if defined(HTTP_MPFS_UPLOAD)
static HTTP_IO_RESULT HTTPMPFSUpload(void)
{
	BYTE c[16];
	WORD lenA, lenB;
	
	switch(curHTTP.httpStatus)
	{
		// New upload, so look for the CRLFCRLF
		case HTTP_MPFS_UP:
		
			lenA = TCPFindROMArray(sktHTTP, (ROM BYTE*)"\r\n\r\n", 4, 0, FALSE);
		
			if(lenA != 0xffff)
			{// Found it, so remove all data up to and including
				lenA = TCPGetArray(sktHTTP, NULL, lenA);
				curHTTP.byteCount -= lenA;
				
				// Make sure first 6 bytes are also in
				if(TCPIsGetReady(sktHTTP) < (4u + 6u) )
				{
					lenA++;
					return HTTP_IO_NEED_DATA;
				}
				
				// Make sure it's an MPFS of the correct version
				lenA = TCPGetArray(sktHTTP, c, 10);
				curHTTP.byteCount -= lenA;
				if(memcmppgm2ram(c, (ROM void*)"\r\n\r\nMPFS\x02\x01", 10) == 0)
				{// Read as Ver 2.1
					curHTTP.httpStatus = HTTP_MPFS_OK;
					
					// Format MPFS storage and put 6 byte tag
					curHTTP.file = MPFSFormat();
					MPFSPutArray(curHTTP.file, &c[4], 6);
				}
				else
				{// Version is wrong
					curHTTP.httpStatus = HTTP_MPFS_ERROR;
				}
				
				return HTTP_IO_WAITING;
			}
			else
			{// Otherwise, remove as much as possible
				lenA = TCPGetArray(sktHTTP, NULL, TCPIsGetReady(sktHTTP) - 4);
				curHTTP.byteCount -= lenA;
			}
			
			break;
		
		// Received file is invalid
		case HTTP_MPFS_ERROR:
			curHTTP.byteCount -= TCPIsGetReady(sktHTTP);
			TCPDiscard(sktHTTP);
			if(curHTTP.byteCount < 100u || curHTTP.byteCount > 0x80000000u)
			{// If almost all data was read, or if we overflowed, then return
				smHTTP = SM_HTTP_SERVE_HEADERS;
				return HTTP_IO_DONE;
			}
			break;
		
		// File is verified, so write the data
		case HTTP_MPFS_OK:
			// Determine how much to read
			lenA = TCPIsGetReady(sktHTTP);
			if(lenA > curHTTP.byteCount)
				lenA = curHTTP.byteCount;
				
			while(lenA > 0u)
			{
				lenB = TCPGetArray(sktHTTP, c, mMIN(lenA,16u));
				curHTTP.byteCount -= lenB;
				lenA -= lenB;
				MPFSPutArray(curHTTP.file, c, lenB);
			}
				
			// If we've read all the data
			if(curHTTP.byteCount == 0u)
			{
				MPFSPutEnd(TRUE);
				smHTTP = SM_HTTP_SERVE_HEADERS;
				return HTTP_IO_DONE;
			}
			
		// Other states are not valid here
		default:
			break;
	}
		
	// Ask for more data
	return HTTP_IO_NEED_DATA;
	
}
#endif

/*****************************************************************************
  Function:
	void HTTPIncFile(ROM BYTE* cFile)

  Summary:
	Writes a file byte-for-byte to the currently loaded TCP socket.

  Description:
	Allows an entire file to be included as a dynamic variable, providing
	a basic templating system for HTML web pages.  This reduces unneeded
	duplication of visual elements such as headers, menus, etc.

	When curHTTP.callbackPos is 0, the file is opened and as many bytes
	as possible are written.  The current position is then saved to 
	curHTTP.callbackPos and the file is closed.  On subsequent calls, 
	reading begins at the saved location and continues.  Once the end of
	the input file is reached, curHTTP.callbackPos is set back to 0 to 
	indicate completion.

  Precondition:
	None

  Parameters:
	cFile - the name of the file to be sent

  Returns:
  	None
  	
  Remarks:
	Users should not call this function directly, but should instead add
	dynamic variables in the form of ~inc:filename.ext~ in their HTML code
	to include (for example) the file "filename.ext" at that specified
	location.  The MPFS2 Generator utility will handle the rest.
  ***************************************************************************/
void HTTPIncFile(ROM BYTE* cFile)
{
	WORD wCount, wLen;
	BYTE data[64];
	MPFS_HANDLE fp;
	
	// Check if this is a first round call
	if(curHTTP.callbackPos == 0x00u)
	{// On initial call, open the file and save its ID
		fp = MPFSOpenROM(cFile);
		if(fp == MPFS_INVALID_HANDLE)
		{// File not found, so abort
			return;
		}
		((DWORD_VAL*)&curHTTP.callbackPos)->w[0] = MPFSGetID(fp);
	}
	else
	{// The file was already opened, so load up its ID and seek
		fp = MPFSOpenID(((DWORD_VAL*)&curHTTP.callbackPos)->w[0]);
		if(fp == MPFS_INVALID_HANDLE)
		{// No file handles available, so wait for now
			return;
		}
		MPFSSeek(fp, ((DWORD_VAL*)&curHTTP.callbackPos)->w[1], MPFS_SEEK_FORWARD);
	}
	
	// Get/put as many bytes as possible
	wCount = TCPIsPutReady(sktHTTP);
	while(wCount > 0u)
	{
		wLen = MPFSGetArray(fp, data, mMIN(wCount, sizeof(data)));
		if(wLen == 0u)
		{// If no bytes were read, an EOF was reached
			MPFSClose(fp);
			curHTTP.callbackPos = 0x00;
			return;
		}
		else
		{// Write the bytes to the socket
			TCPPutArray(sktHTTP, data, wLen);
			wCount -= wLen;
		}
	}
	
	// Save the new address and close the file
	((DWORD_VAL*)&curHTTP.callbackPos)->w[1] = MPFSTell(fp);
	MPFSClose(fp);
	
	return;
}


#endif
