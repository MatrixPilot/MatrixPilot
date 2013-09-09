/*********************************************************************
 *
 *	Address Resolution Protocol (ARP) Client and Server
 *  Module for Microchip TCP/IP Stack
 *	 -Provides IP address to Ethernet MAC address translation
 *	 -Reference: RFC 826
 *
 *********************************************************************
 * FileName:        ARP.c
 * Dependencies:    string.h
 *                  StackTsk.h
 *                  Helpers.h
 *                  ARP.h
 *                  MAC.h
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
 * Nilesh Rajbharti     5/1/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Howard Schlunder		8/17/06	Combined ARP.c and ARPTsk.c into ARP.c; 
 *								rewrote some of it to look more linear
 ********************************************************************/
#define __ARP_C

#include "TCPIP Stack/TCPIP.h"

/****************************************************************************
  Section:
	Constants and Variables
  ***************************************************************************/
#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
//#define ARP_OPERATION_REQ       0x01u		// Operation code indicating an ARP Request
//#define ARP_OPERATION_RESP      0x02u		// Operation code indicating an ARP Response

#define HW_ETHERNET             (0x0001u)	// ARP Hardware type as defined by IEEE 802.3
#define ARP_IP                  (0x0800u)	// ARP IP packet type as defined by IEEE 802.3
#endif

#ifdef STACK_CLIENT_MODE
static NODE_INFO Cache;						// Cache for one ARP response
#endif

#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
#define MAX_REG_APPS            2           // MAX num allowed registrations of Modules/Apps
static struct arp_app_callbacks reg_apps[MAX_REG_APPS]; // Call-Backs storage for MAX of two Modules/Apps
/*
// ARP packet structure
typedef struct __attribute__((aligned(2), packed))
{
    WORD        HardwareType;
    WORD        Protocol;
    BYTE        MACAddrLen;
    BYTE        ProtocolLen;
    WORD        Operation;
    MAC_ADDR    SenderMACAddr;
    IP_ADDR     SenderIPAddr;
    MAC_ADDR    TargetMACAddr;
    IP_ADDR     TargetIPAddr;
} ARP_PACKET;
*/
#endif


/****************************************************************************
  Section:
	Helper Function Prototypes
  ***************************************************************************/

static BOOL ARPPut(ARP_PACKET* packet);


/****************************************************************************
  Section:
	Function Implementations
  ***************************************************************************/
#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
/************ User Application APIs ****************************************/

/*****************************************************************************
  Function:
	CHAR ARPRegisterCallbacks(struct arp_app_callbacks *app)

  Summary:
	Registering callback with ARP module to get notified about certian events.
	
  Description:
  	This function allows end user application to register with callbacks, which
    will be called by ARP module to give notification to user-application about 
    events occurred at ARP layer. For ex: when a ARP-packet is received, which is
    conflicting with our own pair of addresses (MAC-Address and IP-address).
    This is an extension for zeroconf protocol implementation (ZeroconfLL.c)

  Precondition:
	None

  Parameters:
	app - ARP-Application callbacks structure supplied by user-application 
    
  Returns:
    id > 0 - Returns non-negative value that represents the id of registration
             The same id needs to be used in de-registration
    -1     - When registered applications exceed MAX_REG_APPS and there is no
             free slot for registration
 
  ***************************************************************************/
CHAR ARPRegisterCallbacks(struct arp_app_callbacks *app)
{
    BYTE i;
    for(i=0; i<MAX_REG_APPS; i++)
    {
        if(!reg_apps[i].used)
        {
            reg_apps[i].ARPPkt_notify = app->ARPPkt_notify;
            reg_apps[i].used = 1;
            return (i+1); // Return Code. Should be used in deregister.
        }
    }
    return -1; // No space for registration
}

/*****************************************************************************
  Function:
	BOOL ARPDeRegisterCallbacks(CHAR reg_id)

  Summary:
	De-Registering callbacks with ARP module that are registered previously.
	
  Description:
  	This function allows end user-application to de-register with callbacks, 
    which were registered previously.
    This is called by user-application, when its no longer interested in 
    notifications from ARP-Module. This allows the other application to get 
    registered with ARP-module.   

  Precondition:
	None

  Parameters:
	reg_id - Registration-id returned in ARPRegisterCallbacks call
    
  Returns:
    TRUE  - On success
    FALSE - Failure to indicate invalid reg_id  
  ***************************************************************************/ 
BOOL ARPDeRegisterCallbacks(CHAR reg_id)
{
    if(reg_id <= 0 || reg_id > MAX_REG_APPS)
        return FALSE;

    reg_apps[reg_id-1].used = 0; // To indicate free slot for registration
	return TRUE;
}

/*****************************************************************************
  Function:
	void ARPProcessRxPkt(ARP_PACKET* packet)

  Summary:
	Processes Received-ARP packet (ARP request/Reply).
	
  Description:
  	This function is to pass-on the ARP-packet to registered application,
    with the notification of Rx-ARP packet. 

  Precondition:
	ARP packet is received completely from MAC

  Parameters:
	packet - Rx packet to be processed     

  Returns:
    None   
  ***************************************************************************/
void ARPProcessRxPkt(ARP_PACKET* packet)
{
    BYTE pass_on = 0; // Flag to indicate whether need to be forwarded
    BYTE i;

    // Probing Stage
    if(AppConfig.MyIPAddr.Val == 0x00)
    {
        pass_on = 1; // Pass to Registered-Application for further processing        
		// putsUART("ARPProcessRxPkt: MyIPAddr=0  -> pass_on = 1 \r\n"); 
	}
    else if ((AppConfig.MyIPAddr.Val != 0x00) && (AppConfig.networkType == WF_SOFT_AP)) // SOFTAP_ZEROCONF_SUPPORT
    {
		//putsUART("ARPProcessRxPkt: MyIPAddr!=0 & SoftAP  -> pass_on = 1 \r\n"); 
        pass_on = 1; // Pass to Registered-Application for further processing        
    }
    else if(AppConfig.MyIPAddr.Val)
    {
        /* Late-conflict */
        if(packet->SenderIPAddr.Val == AppConfig.MyIPAddr.Val)
        {
            pass_on = 1;
			// putsUART("ARPProcessRxPkt: SenderIPAddr = MyIPAddr \r\n");
        }
    }
	
    if(pass_on)
    {    
        for(i =0; i< MAX_REG_APPS; i++)
        {
            if(reg_apps[i].used)
            {
                 //putsUART("ARPProcessRxPkt: pass_on \r\n");
                reg_apps[i].ARPPkt_notify(packet->SenderIPAddr.Val,
                                      packet->TargetIPAddr.Val,
                                      &packet->SenderMACAddr,
                                      &packet->TargetMACAddr,
                                      packet->Operation);                
            }
        }
    }
}
#endif

/*****************************************************************************
  Function:
	void ARPSendPkt(IP_ADDR* SrcIPAddr, IP_ADDR* DestIPAddr, int op_req )

  Summary:
	Transmits an ARP request/Reply initated by Application or external module.
	
  Description:
  	This function transmits and ARP request/reply to determine the hardware
  	address of a given IP address (or) Announce self-address to all nodes in
    network. Extended for zeroconf protocol. 

  Precondition:
	ARP packet is ready in the MAC buffer.

  Parameters:
	SrcIPAddr - The Source IP-address 
    DestIPAddr - The Destination IP-Address
    op_req     - Operation Request (ARP_REQ/ARP_RESP)

  Returns:
    TRUE - The ARP packet was generated properly
  	FALSE - Not possible return value

  Remarks:
  	This API is to give control over AR-packet to external modules. 
  ***************************************************************************/
BOOL ARPSendPkt(DWORD SrcIPAddr, DWORD DestIPAddr, BYTE op_req )
{
    ARP_PACKET packet;

#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
#define KS_ARP_IP_MULTICAST_HACK y
#ifdef KS_ARP_IP_MULTICAST_HACK
	DWORD_VAL *DestAddr = (DWORD_VAL *)&DestIPAddr;
	if ((DestAddr->v[0] >= 224) &&(DestAddr->v[0] <= 239)) {
		// "Resolve" the IP to MAC address mapping for
		// IP multicast address range from 224.0.0.0 to 239.255.255.255
	
		Cache.MACAddr.v[0] = 0x01;
		Cache.MACAddr.v[1] = 0x00;
		Cache.MACAddr.v[2] = 0x5E;
		Cache.MACAddr.v[3] = 0x7f & DestAddr->v[1];
		Cache.MACAddr.v[4] = DestAddr->v[2];
		Cache.MACAddr.v[5] = DestAddr->v[3];
	
		Cache.IPAddr.Val = DestAddr->Val;
	
		return TRUE;
	}
#endif
#endif

    packet.Operation = op_req;
	packet.TargetMACAddr.v[0]   = 0xff;
	packet.TargetMACAddr.v[1]   = 0xff;
	packet.TargetMACAddr.v[2]   = 0xff;
	packet.TargetMACAddr.v[3]   = 0xff;
	packet.TargetMACAddr.v[4]   = 0xff;
	packet.TargetMACAddr.v[5]   = 0xff;

    packet.TargetIPAddr.Val	= DestIPAddr;
    packet.SenderIPAddr.Val = SrcIPAddr;

    return ( ARPPut(&packet) );
}


/*****************************************************************************
  Function:
	static BOOL ARPPut(ARP_PACKET* packet)

  Description:
	Writes an ARP packet to the MAC.

  Precondition:
	None

  Parameters:
	packet - A pointer to an ARP_PACKET structure with correct operation
				and target preconfigured.

  Return Values:
  	TRUE - The ARP packet was generated properly
  	FALSE - Not a possible return value
  ***************************************************************************/
static BOOL ARPPut(ARP_PACKET* packet)
{
	while(!MACIsTxReady());
	MACSetWritePtr(BASE_TX_ADDR);
	

    packet->HardwareType  = HW_ETHERNET;
    packet->Protocol      = ARP_IP;
    packet->MACAddrLen    = sizeof(MAC_ADDR);
    packet->ProtocolLen   = sizeof(IP_ADDR);
//    packet->SenderMACAddr = AppConfig.MyMACAddr;	// HI-TECH PICC-18 compiler can't handle this statement, use memcpy() as a workaround
	memcpy(&packet->SenderMACAddr, (void*)&AppConfig.MyMACAddr, sizeof(packet->SenderMACAddr));
#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
    //packet->SenderIPAddr  = AppConfig.MyIPAddr; /* Removed for ZCLL, SenderIPAddr should be filled in */
#else
    packet->SenderIPAddr  = AppConfig.MyIPAddr;
#endif

    SwapARPPacket(packet);

    MACPutHeader(&packet->TargetMACAddr, MAC_ARP, sizeof(*packet));
    MACPutArray((BYTE*)packet, sizeof(*packet));
    MACFlush();
	
	return TRUE;
}



/*****************************************************************************
  Function:
	void ARPInit(void)

  Summary:
	Initializes the ARP module.
	
  Description:
  	Initializes the ARP module.  Call this function once at boot to 
  	invalidate the cached lookup.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  
  Remarks:
  	This function is only required when the stack is a client, and therefore
  	is only enabled when STACK_CLIENT_MODE is enabled.
  ***************************************************************************/
#ifdef STACK_CLIENT_MODE
void ARPInit(void)
{
    Cache.MACAddr.v[0] = 0xff;
    Cache.MACAddr.v[1] = 0xff;
    Cache.MACAddr.v[2] = 0xff;
    Cache.MACAddr.v[3] = 0xff;
    Cache.MACAddr.v[4] = 0xff;
    Cache.MACAddr.v[5] = 0xff;

	Cache.IPAddr.Val = 0xfffffffful;
}
#endif



/*****************************************************************************
  Function:
	BOOL ARPProcess(void)

  Summary:
	Processes an incoming ARP packet.
	
  Description:
  	Retrieves an ARP packet from the MAC buffer and determines if it is a
  	response to our request (in which case the ARP is resolved) or if it
  	is a request requiring our response (in which case we transmit one.)

  Precondition:
	ARP packet is ready in the MAC buffer.

  Parameters:
	None

  Return Values:
  	TRUE - All processing of this ARP packet is complete.  Do not call 
  			again until a new ARP packet is waiting in the RX buffer.
  	FALSE - This function must be called again.  More time is needed to 
  			send an ARP response.
  ***************************************************************************/
BOOL ARPProcess(void)
{
	ARP_PACKET packet;
	static NODE_INFO Target;
    #if defined(STACK_USE_AUTO_IP)
        BYTE i;
    #endif
	static enum
	{
	    SM_ARP_IDLE = 0,
	    SM_ARP_REPLY
	} smARP = SM_ARP_IDLE;

    switch(smARP)
    {
	    case SM_ARP_IDLE:
			// Obtain the incoming ARP packet
		    MACGetArray((BYTE*)&packet, sizeof(packet));		
		    MACDiscardRx();
		    SwapARPPacket(&packet);
		
			// Validate the ARP packet
		    if ( packet.HardwareType != HW_ETHERNET     ||
		         packet.MACAddrLen != sizeof(MAC_ADDR)  ||
		         packet.ProtocolLen != sizeof(IP_ADDR) )
		    {
		         return TRUE;
		    }
#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
			ARPProcessRxPkt(&packet);
#endif

#ifdef STACK_USE_AUTO_IP
            if (packet.SenderIPAddr.Val == AppConfig.MyIPAddr.Val)
            {
                AutoIPConflict(0);
                return TRUE;                
            }
#endif

			// Handle incoming ARP responses
#ifdef STACK_CLIENT_MODE
			if(packet.Operation == ARP_OPERATION_RESP)
			{
/*                #if defined(STACK_USE_AUTO_IP)
                for (i = 0; i < NETWORK_INTERFACES; i++)
                    if (AutoIPConfigIsInProgress(i))
                        AutoIPConflict(i);
                #endif*/
				Cache.MACAddr = packet.SenderMACAddr;
				Cache.IPAddr = packet.SenderIPAddr;
				
				//putsUART("ARPProcess: SM_ARP_IDLE: ARP_OPERATION_RESP  \r\n"); 
				return TRUE;
			}
#endif

			// Handle incoming ARP requests for our MAC address
			if(packet.Operation == ARP_OPERATION_REQ)
			{
				if(packet.TargetIPAddr.Val != AppConfig.MyIPAddr.Val)
				{
					return TRUE;
				}
#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
                               /* Fix for Loop-Back suppression:
                                * For ZCLL-Claim packets, host should not respond.
                                * Check Sender's MAC-address with own MAC-address and 
                                * if it is matched, response will not be sent back. This
                                * was leading to flooding of ARP-answeres */
                                if(!memcmp (&packet.SenderMACAddr, &AppConfig.MyMACAddr, 6))
                                {
                                     putsUART("Loopback answer suppressed \r\n");
                                     return TRUE;
                                }
#endif
                #if defined(STACK_USE_AUTO_IP)
                for (i = 0; i < NETWORK_INTERFACES; i++)
                    if (AutoIPConfigIsInProgress(i))
                    {
                        AutoIPConflict(i);
                        return TRUE;
                    }             
                #endif
				Target.IPAddr = packet.SenderIPAddr;
				Target.MACAddr = packet.SenderMACAddr;

				//putsUART("ARPProcess: SM_ARP_IDLE: ARP_OPERATION_REQ  \r\n"); 

				smARP = SM_ARP_REPLY;
			}
			// Do not break.  If we get down here, we need to send a reply.	

	    case SM_ARP_REPLY:
	        packet.Operation		= ARP_OPERATION_RESP;
            #if defined(STACK_USE_AUTO_IP)
            if (AutoIPIsConfigured(0))
            {
                packet.TargetMACAddr.v[0] = 0xFF;
                packet.TargetMACAddr.v[1] = 0xFF;
                packet.TargetMACAddr.v[2] = 0xFF;
                packet.TargetMACAddr.v[3] = 0xFF;
                packet.TargetMACAddr.v[4] = 0xFF;
                packet.TargetMACAddr.v[5] = 0xFF;
            }
            else
            #endif
        	    packet.TargetMACAddr	= Target.MACAddr;
    	    packet.TargetIPAddr		= Target.IPAddr;
#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
            packet.SenderIPAddr		= AppConfig.MyIPAddr;
#endif
			//putsUART("ARPProcess: SM_ARP_REPLY  \r\n"); 

			// Send an ARP response to a previously received request
			if(!ARPPut(&packet))
			{
	           return FALSE;
			}

			// Begin listening for ARP requests again
			smARP = SM_ARP_IDLE;
	        break;
	}

    return TRUE;
}

/*****************************************************************************
  Function:
	void ARPResolve(IP_ADDR* IPAddr)

  Summary:
	Transmits an ARP request to resolve an IP address.
	
  Description:
  	This function transmits and ARP request to determine the hardware
  	address of a given IP address.

  Precondition:
	None

  Parameters:
	IPAddr - The IP address to be resolved.  The address must be specified 
			 in network byte order (big endian).

  Returns:
  	None

  Remarks:
  	This function is only required when the stack is a client, and therefore
  	is only enabled when STACK_CLIENT_MODE is enabled.

	To retrieve the ARP query result, call the ARPIsResolved() function.
  ***************************************************************************/
#ifdef STACK_CLIENT_MODE
void ARPResolve(IP_ADDR* IPAddr)
{
    ARP_PACKET packet;

#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
#define KS_ARP_IP_MULTICAST_HACK y
#ifdef KS_ARP_IP_MULTICAST_HACK
    if ((IPAddr->v[0] >= 224) &&(IPAddr->v[0] <= 239))
    {
		// "Resolve" the IP to MAC address mapping for
		// IP multicast address range from 224.0.0.0 to 239.255.255.255

		Cache.MACAddr.v[0] = 0x01;
		Cache.MACAddr.v[1] = 0x00;
		Cache.MACAddr.v[2] = 0x5E;
		Cache.MACAddr.v[3] = 0x7f & IPAddr->v[1];
		Cache.MACAddr.v[4] = IPAddr->v[2];
		Cache.MACAddr.v[5] = IPAddr->v[3];

		Cache.IPAddr.Val = IPAddr->Val;

		return;
	}
#endif
#endif

	packet.Operation            = ARP_OPERATION_REQ;
	packet.TargetMACAddr.v[0]   = 0xff;
	packet.TargetMACAddr.v[1]   = 0xff;
	packet.TargetMACAddr.v[2]   = 0xff;
	packet.TargetMACAddr.v[3]   = 0xff;
	packet.TargetMACAddr.v[4]   = 0xff;
	packet.TargetMACAddr.v[5]   = 0xff;

	//putsUART("ARPResolve() \r\n"); 

    // ARP query either the IP address directly (on our subnet), or do an ARP query for our Gateway if off of our subnet
	packet.TargetIPAddr			= ((AppConfig.MyIPAddr.Val ^ IPAddr->Val) & AppConfig.MyMask.Val) ? AppConfig.MyGateway : *IPAddr;
#ifdef STACK_USE_ZEROCONF_LINK_LOCAL
	packet.SenderIPAddr			= AppConfig.MyIPAddr;
#endif

    ARPPut(&packet);
}
#endif




/*****************************************************************************
  Function:
	BOOL ARPIsResolved(IP_ADDR* IPAddr, MAC_ADDR* MACAddr)

  Summary:
	Determines if an ARP request has been resolved yet.
	
  Description:
  	This function checks if an ARP request has been resolved yet, and if
  	so, stores the resolved MAC address in the pointer provided.

  Precondition:
	ARP packet is ready in the MAC buffer.

  Parameters:
	IPAddr - The IP address to be resolved.  This must match the IP address 
			 provided to the ARPResolve() function call.
	MACAddr - A buffer to store the corresponding MAC address retrieved from 
			 the ARP query.

  Return Values:
  	TRUE - The IP address has been resolved and MACAddr MAC address field
		   indicates the response.
  	FALSE -	The IP address is not yet resolved.  Try calling ARPIsResolved() 
		   again at a later time.  If you don't get a response after a 
		   application specific timeout period, you may want to call 
		   ARPResolve() again to transmit another ARP query (in case if the 
		   original query or response was lost on the network).  If you never 
		   receive an ARP response, this may indicate that the IP address 
		   isn't in use.

  Remarks:
  	This function is only required when the stack is a client, and therefore
  	is only enabled when STACK_CLIENT_MODE is enabled.
  ***************************************************************************/
#ifdef STACK_CLIENT_MODE
BOOL ARPIsResolved(IP_ADDR* IPAddr, MAC_ADDR* MACAddr)
{
    if((Cache.IPAddr.Val == IPAddr->Val) || 
	  ((Cache.IPAddr.Val == AppConfig.MyGateway.Val) && ((AppConfig.MyIPAddr.Val ^ IPAddr->Val) & AppConfig.MyMask.Val)))
    {
        *MACAddr = Cache.MACAddr;		
		//putsUART("ARPIsResolved  \r\n"); 
        return TRUE;
    }

	//putsUART("ARPIs  NOT Resolved  \r\n"); 

    return FALSE;
}
#endif



/*****************************************************************************
  Function:
	void SwapARPPacket(ARP_PACKET* p)

  Description:
  	Swaps endian-ness of header information in an ARP packet.

  Precondition:
	None

  Parameters:
	p - The ARP packet to be swapped

  Returns:
  	None
  ***************************************************************************/
void SwapARPPacket(ARP_PACKET* p)
{
    p->HardwareType     = swaps(p->HardwareType);
    p->Protocol         = swaps(p->Protocol);
    p->Operation        = swaps(p->Operation);
}
