/*********************************************************************
 *
 *	Reboot Module
 *  Module for Microchip TCP/IP Stack
 *	 -Remotely resets the PIC
 *	 -Reference: Internet Bootloader documentation
 *
 *********************************************************************
 * FileName:        Reboot.c
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
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     02/22/07	Original
 ********************************************************************/
#define __REBOOT_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_REBOOT_SERVER)

#include "TCPIP Stack/TCPIP.h"


#define REBOOT_PORT		69	// UDP TFTP port


// For improved security, you might want to limit reboot capabilities 
// to only users on the same IP subnet.  Define REBOOT_SAME_SUBNET_ONLY 
// to enable this access restriction.
#define REBOOT_SAME_SUBNET_ONLY


extern NODE_INFO remoteNode;

/*********************************************************************
 * Function:        void RebootTask(void)
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Checks for incomming traffic on port 69.  
 *					Resets the PIC if a 'R' is received.
 *
 * Note:            This module is primarily for use with the 
 *					Ethernet bootloader.  By resetting, the Ethernet 
 *					bootloader can take control for a second and let
 *					a firmware upgrade take place.
 ********************************************************************/
void RebootTask(void)
{
	static UDP_SOCKET	MySocket = INVALID_UDP_SOCKET;
	struct
	{
		BYTE vMACAddress[6];
		DWORD dwIPAddress;
		WORD wChecksum;
	} BootloaderAddress;
	
	if(MySocket == INVALID_UDP_SOCKET)
		MySocket = UDPOpenEx(0,UDP_OPEN_SERVER,REBOOT_PORT,INVALID_UDP_PORT);
//		MySocket = UDPOpen(REBOOT_PORT, NULL, INVALID_UDP_PORT);

	if(MySocket == INVALID_UDP_SOCKET)
		return;

	// Do nothing if no data is waiting
	if(!UDPIsGetReady(MySocket))
		return;

	#if defined(REBOOT_SAME_SUBNET_ONLY)
		// Respond only to name requests sent to us from nodes on the same subnet
     	if((remoteNode.IPAddr.Val & AppConfig.MyMask.Val) != (AppConfig.MyIPAddr.Val & AppConfig.MyMask.Val))
		{
			UDPDiscard();
			return;
		}
	#endif

	// Get our MAC address, IP address, and compute a checksum of them 
	memcpy((void*)&BootloaderAddress.vMACAddress[0], (void*)&AppConfig.MyMACAddr.v[0], sizeof(AppConfig.MyMACAddr));
	BootloaderAddress.dwIPAddress = AppConfig.MyIPAddr.Val;
	BootloaderAddress.wChecksum = CalcIPChecksum((BYTE*)&BootloaderAddress, sizeof(BootloaderAddress) - sizeof(BootloaderAddress.wChecksum));
	
	// To enter the bootloader, we need to clear the /POR bit in RCON.
	// Otherwise, the bootloader will immediately hand off execution 
	// to us.
	#if defined(USE_LCD)
		strcpypgm2ram((char*)LCDText, "Bootloader Reset");
		LCDUpdate();
	#endif
	RCONbits.POR = 0;
	#if defined(__18CXX)
	{
		WORD_VAL wvPROD;

		wvPROD.Val = ((WORD)&BootloaderAddress);
		PRODH = wvPROD.v[1];
		PRODL = wvPROD.v[0];
	}
	#endif
	Reset();
}

#endif //#if defined(STACK_USE_REBOOT_SERVER)
