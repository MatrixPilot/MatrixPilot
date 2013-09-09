/*********************************************************************
 *
 *  NetBIOS Name Service (NBNS) Server
 *  Module for Microchip TCP/IP Stack
 *	 -Responds to NBNS name requests to allow human name assignment 
 *	  to the board.  i.e. allows nodes on the same IP subnet to use a 
 *    hostname to access the board instead of an IP address.
 *	 -Reference: RFC 1002
 *
 *********************************************************************
 * FileName:        NBNS.c
 * Dependencies:    UDP
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
 * Howard Schlunder     8/01/06	Original
 ********************************************************************/
#define __NBNS_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_NBNS)

#include "TCPIP Stack/TCPIP.h"

// NetBIOS Name Service port
#define NBNS_PORT		(137u)

// NBNS Header structure
typedef struct _NBNS_HEADER
{
	WORD_VAL TransactionID;
	WORD_VAL Flags;
	WORD_VAL Questions;
	WORD_VAL Answers;
	WORD_VAL AuthoritativeRecords;
	WORD_VAL AdditionalRecords;
} NBNS_HEADER;

static void NBNSPutName(BYTE *String);
static void NBNSGetName(BYTE *String);

extern NODE_INFO remoteNode;


/*********************************************************************
 * Function:        void NBNSTask(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Sends responses to NetBIOS name requests
 *
 * Note:            None
 ********************************************************************/
void NBNSTask(void)
{
	static UDP_SOCKET	MySocket;
	BYTE 				i;
	WORD_VAL			Type, Class;
	NBNS_HEADER			NBNSHeader;
	BYTE				NameString[16];
	static enum
	{
		NBNS_HOME = 0,
		NBNS_OPEN_SOCKET,
		NBNS_LISTEN
	} smNBNS = NBNS_HOME;

	switch(smNBNS)
	{
		case NBNS_HOME:
			smNBNS++;
			break;

		case NBNS_OPEN_SOCKET:
			//MySocket = UDPOpen(NBNS_PORT, NULL, NBNS_PORT);
			
			MySocket = UDPOpenEx(0,UDP_OPEN_SERVER,NBNS_PORT,NBNS_PORT);
			if(MySocket == INVALID_UDP_SOCKET)
				break;

			smNBNS++;

		case NBNS_LISTEN:
			if(!UDPIsGetReady(MySocket))
				break;

			// Respond only to name requests sent to us from nodes on the same subnet
			// This prevents us from sending out the wrong IP address information if 
			// we haven't gotten a DHCP lease yet.
        	if((remoteNode.IPAddr.Val & AppConfig.MyMask.Val) != (AppConfig.MyIPAddr.Val & AppConfig.MyMask.Val))
			{
				UDPDiscard();
				break;
			}

			// Retrieve the NBNS header and de-big-endian it
			UDPGet(&NBNSHeader.TransactionID.v[1]);
			UDPGet(&NBNSHeader.TransactionID.v[0]);
			UDPGet(&NBNSHeader.Flags.v[1]);
			UDPGet(&NBNSHeader.Flags.v[0]);
			UDPGet(&NBNSHeader.Questions.v[1]);
			UDPGet(&NBNSHeader.Questions.v[0]);
			UDPGet(&NBNSHeader.Answers.v[1]);
			UDPGet(&NBNSHeader.Answers.v[0]);
			UDPGet(&NBNSHeader.AuthoritativeRecords.v[1]);
			UDPGet(&NBNSHeader.AuthoritativeRecords.v[0]);
			UDPGet(&NBNSHeader.AdditionalRecords.v[1]);
			UDPGet(&NBNSHeader.AdditionalRecords.v[0]);

			// Remove all questions
			while(NBNSHeader.Questions.Val--)
			{
				NBNSGetName(NameString);
				UDPGet(&i);				// <??> Trailing character on string
				UDPGet(&Type.v[1]);		// Question type
				UDPGet(&Type.v[0]);
				UDPGet(&Class.v[1]);	// Question class
				UDPGet(&Class.v[0]);
				
				if(Type.Val == 0x0020u && Class.Val == 0x0001u && memcmp((void*)NameString, (void*)AppConfig.NetBIOSName, sizeof(AppConfig.NetBIOSName)) == 0)
				{
					while(!UDPIsPutReady(MySocket));

					NBNSHeader.Flags.Val = 0x8400;

					UDPPut(NBNSHeader.TransactionID.v[1]);
					UDPPut(NBNSHeader.TransactionID.v[0]);
					UDPPut(NBNSHeader.Flags.v[1]);
					UDPPut(NBNSHeader.Flags.v[0]);
					UDPPut(0x00);	// 0x0000 Questions
					UDPPut(0x00);
					UDPPut(0x00);	// 0x0001 Answers
					UDPPut(0x01);
					UDPPut(0x00);	// 0x0000 Athoritative records
					UDPPut(0x00);
					UDPPut(0x00);	// 0x0000 Additional records
					UDPPut(0x00);

					NBNSPutName(AppConfig.NetBIOSName);
					UDPPut(0x00);	// 0x0020 Type: NetBIOS
					UDPPut(0x20);
					UDPPut(0x00);	// 0x0001 Class: Internet
					UDPPut(0x01);
					UDPPut(0x00);	// 0x00000000 Time To Live
					UDPPut(0x00);
					UDPPut(0x00);
					UDPPut(0x00);

					UDPPut(0x00);	// 0x0006 Data length
					UDPPut(0x06);	
					UDPPut(0x60);	// 0x6000 Flags: H-node, Unique
					UDPPut(0x00);
					UDPPut(AppConfig.MyIPAddr.v[0]);	// Put out IP address
					UDPPut(AppConfig.MyIPAddr.v[1]);
					UDPPut(AppConfig.MyIPAddr.v[2]);
					UDPPut(AppConfig.MyIPAddr.v[3]);

					// Change the destination address to the unicast address of the last received packet
		        	memcpy((void*)&UDPSocketInfo[MySocket].remote.remoteNode, (const void*)&remoteNode, sizeof(remoteNode));
					UDPFlush();				
				}

			}
			
			UDPDiscard();

			break;
	}
}

/*********************************************************************
 * Function:        static void NBNSPutName (BYTE *String)
 *
 * PreCondition:    None
 *
 * Input:           String: The name to transmit
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Transmits the NetBIOS name across an open UDP
 *                  socket.
 *
 * Note:            None
 ********************************************************************/
static void NBNSPutName(BYTE *String)
{
	BYTE i, j;

	UDPPut(32);	// NetBIOS names are always 32 bytes long (16 decoded bytes)
	for(i = 0; i < 16u; i++)
	{
		j = *String++;
		UDPPut((j>>4) + 'A');
		UDPPut((j & 0x0F) + 'A');
	}
	
	UDPPut(0x00);
}

/*********************************************************************
 * Function:        static void NBNSGetName (BYTE *String)
 *
 * PreCondition:    None
 *
 * Input:           String: Pointer to an array into which
 *                  a received NetBIOS name should be copied.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Reads the NetBIOS name from a UDP socket and
 *                  copies it into a user-specified buffer.
 *
 * Note:            None
 ********************************************************************/
static void NBNSGetName(BYTE *String)
{
	BYTE i, j, k;

	if(String == NULL)
	{
		UDPGet(&i);
		while(i--)
		{
			UDPGet(&j);
		}
	}
	else
	{
		UDPGet(&i);
		if(i != 32u)
		{
			*String = 0;
			return;
		}
		while(i--)
		{
			UDPGet(&j);
			j -= 'A';
			k = j<<4;
			i--;
			UDPGet(&j);
			j -= 'A';
			*String++ = k | j;
		}
	}
}


#endif //#if defined(STACK_USE_NBNS)
