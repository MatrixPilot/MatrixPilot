/*********************************************************************
 *
 *                  Simple Mail Transfer Protocol (SMTP) Client
 *					Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        SMTP.c
 * Dependencies:    TCP.h
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     3/03/06	Original
 ********************************************************************/
#ifndef __SMTP_H
#define __SMTP_H

/****************************************************************************
  Section:
	Data Type Definitions
  ***************************************************************************/
	#define SMTP_SUCCESS		(0x0000u)	// Message was successfully sent
	#define SMTP_RESOLVE_ERROR	(0x8000u)	// DNS lookup for SMTP server failed
	#define SMTP_CONNECT_ERROR	(0x8001u)	// Connection to SMTP server failed
	
/****************************************************************************
  Function:
      typedef struct SMTP_POINTERS
    
  Summary:
    Configures the SMTP client to send a message
    
  Description:
    This structure of pointers configures the SMTP Client to send an e-mail
    message. Initially, all pointers will be null. Set <c>SMTPClient.[field
    name].szRAM</c> to use a string stored in RAM, or <c>SMTPClient.[field
    name].szROM</c> to use a string stored in ROM. (Where <c>[field name]</c>
    is one of the parameters below.)
    
    If a ROM string is specified, <c>SMTPClient.ROMPointers.[field name]</c>
    must also be set to 1 to indicate that this field should be retrieved
    from ROM instead of RAM.
    
  Parameters:
    Server -        the SMTP server to relay the message through
    Username -      the user name to use when logging into the SMTP server,
                    if any is required
    Password -      the password to supply when logging in, if any is required
    To -            the destination address for this message. May be a
                    comma\-separated list of addresss, and/or formatted.
    CC -            The CC addresses for this message, if any. May be a
                    comma\-separated list of addresss, and/or formatted.
    BCC -           The BCC addresses for this message, if any. May be a
                    comma\-separated list of addresss, and/or formatted.
    From -          The From address for this message. May be formatted.
    Subject -       The Subject header for this message.
    OtherHeaders -  Any additional headers for this message. Each additional 
                    header, including the last one, must be terminated with
                    a CRLF pair.
    Body -          When sending a message from memory, the location of the
    				body of this message in memory. Leave as NULL to build 
    				a message on\-the\-fly.
    ROMPointers -   Indicates which parameters to read from ROM instead of RAM.
    UseSSL -        When STACK_USE_SSL_CLIENT is enabled, this flag causes
                    the SMTP client to make an SSL connection to the server.
    ServerPort -    (WORD value) Indicates the port on which to connect to the
                    remote SMTP server.

  Remarks:
    When formatting an e-mail address, the SMTP standard format for associating a
    printable name may be used. This format places the printable name in quotation
    marks, with the address following in pointed brackets, such as "John Smith"
    \<john.smith@domain.com\>                                                       
  ***************************************************************************/
typedef struct
{
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} Server;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} Username;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} Password;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} To;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} CC;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} BCC;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} From;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} Subject;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} OtherHeaders;
	union
	{
		BYTE *szRAM;
		ROM BYTE *szROM;
	} Body;

	struct
	{
		unsigned char Server:1;
		unsigned char Username:1;
		unsigned char Password:1;
		unsigned char To:1;
		unsigned char CC:1;
		unsigned char BCC:1;
		unsigned char From:1;
		unsigned char Subject:1;
		unsigned char OtherHeaders:1;
		unsigned char Body:1;
	} ROMPointers;
	
	#if defined(STACK_USE_SSL_CLIENT)
	BOOL UseSSL;
	#endif
	
	WORD ServerPort;
	
} SMTP_POINTERS;

/****************************************************************************
  Section:
	Global SMTP Variables
  ***************************************************************************/
	extern SMTP_POINTERS SMTPClient;
	
/****************************************************************************
  Section:
	SMTP Function Prototypes
  ***************************************************************************/

BOOL SMTPBeginUsage(void);
WORD SMTPEndUsage(void);
void SMTPTask(void);
void SMTPSendMail(void);
BOOL SMTPIsBusy(void);
WORD SMTPIsPutReady(void);
BOOL SMTPPut(BYTE c);
WORD SMTPPutArray(BYTE* Data, WORD Len);
WORD SMTPPutString(BYTE* Data);
void SMTPFlush(void);
void SMTPPutDone(void);

#if defined(__18CXX)
	WORD SMTPPutROMArray(ROM BYTE* Data, WORD Len);
	WORD SMTPPutROMString(ROM BYTE* Data);
#else
	// Non-ROM variant for C30 / C32
	#define SMTPPutROMArray(a,b)	SMTPPutArray((BYTE*)a,b)
	// Non-ROM variant for C30 / C32
	#define SMTPPutROMString(a)		SMTPPutString((BYTE*)a)
#endif


#endif
