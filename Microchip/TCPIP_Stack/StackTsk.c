/*********************************************************************
 *
 *	TCP/IP Stack Manager
 *  Module for Microchip TCP/IP Stack
 *	 -Handles internal RX packet pre-processing prior to dispatching 
 *    to upper application layers.
 *	 -Reference: AN833
 *
 *********************************************************************
 * FileName:        StackTsk.c
 * Dependencies:    ARP, IP, Network layer interface (ENC28J60.c, 
 *					ETH97J60.c, ENCX24J600.c, or WFMac.c)
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
 * V5.36 ---- STACK_USE_MPFS has been removed.
********************************************************************/
#define __STACKTSK_C

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))

#include "TCPIP_Stack/TCPIP.h"

#if defined( WF_CS_TRIS )
    #if defined( WF_CONFIG_CONSOLE )
        #include "TCPIP_Stack/WFConsole.h"
    #endif
    #if defined( STACK_USE_EZ_CONFIG ) || defined( EZ_CONFIG_SCAN )
        #include "TCPIP_Stack/WFEasyConfig.h"
    #endif
	#include "TCPIP_Stack/WFApi.h"
#endif

// Stack FSM states.
typedef enum
{
    SM_STACK_IDLE,
    SM_STACK_MAC,
    SM_STACK_IP,
    SM_STACK_ARP,
    SM_STACK_TCP,
    SM_STACK_UDP
} SM_STACK;
static SM_STACK smStack;

NODE_INFO remoteNode;

#if defined (WF_CS_TRIS) && defined (STACK_USE_DHCP_CLIENT)
BOOL g_DhcpRenew = FALSE;
extern void SetDhcpProgressState(void);
UINT32 g_DhcpRetryTimer = 0;
#endif


/*********************************************************************
 * Function:        void StackInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Stack and its componets are initialized
 *
 * Side Effects:    None
 *
 * Note:            This function must be called before any of the
 *                  stack or its component routines are used.
 *
 ********************************************************************/
void StackInit(void)
{
	static BOOL once = FALSE;
    smStack                     = SM_STACK_IDLE;

#if defined(STACK_USE_IP_GLEANING) || defined(STACK_USE_DHCP_CLIENT)
    /*
     * If DHCP or IP Gleaning is enabled,
     * startup in Config Mode.
     */
    AppConfig.Flags.bInConfigMode = TRUE;

#endif

#if defined (WF_CS_TRIS) && defined (STACK_USE_DHCP_CLIENT)
	g_DhcpRenew = FALSE;
	g_DhcpRetryTimer = 0;
#endif

	if (!once) {
		// Seed the LFSRRand() function
		LFSRSeedRand(GenerateRandomDWORD());
		once = TRUE;
	}

    MACInit();

#if defined (WF_AGGRESSIVE_PS) && defined (WF_CS_TRIS)
	WFEnableAggressivePowerSave();
#endif

#if defined(WF_CS_TRIS) && defined(STACK_USE_EZ_CONFIG) && !defined(__18CXX)
    WFEasyConfigInit();
#endif    

    ARPInit();

#if defined(STACK_USE_UDP)
    UDPInit();
#endif

#if defined(STACK_USE_TCP)
    TCPInit();
#endif

#if defined(STACK_USE_BERKELEY_API)
	BerkeleySocketInit();
#endif

#if defined(STACK_USE_HTTP2_SERVER)
    HTTPInit();
#endif

#if defined(STACK_USE_RSA)
	RSAInit();
#endif

#if defined(STACK_USE_SSL)
    SSLInit();
#endif

#if defined(STACK_USE_FTP_SERVER) && defined(STACK_USE_MPFS2)
    FTPInit();
#endif

#if defined(STACK_USE_SNMP_SERVER)
	SNMPInit();
#endif

#if defined(STACK_USE_DHCP_CLIENT)
	DHCPInit(0);
    if(!AppConfig.Flags.bIsDHCPEnabled)
    {
        DHCPDisable(0);
    }
#endif

#if defined(STACK_USE_AUTO_IP)
    AutoIPInit(0);
#endif

#if defined(STACK_USE_DYNAMICDNS_CLIENT)
	DDNSInit();
#endif

#if defined(STACK_USE_RANDOM)
	RandomInit();
#endif
}


/*********************************************************************
 * Function:        void StackTask(void)
 *
 * PreCondition:    StackInit() is already called.
 *
 * Input:           None
 *
 * Output:          Stack FSM is executed.
 *
 * Side Effects:    None
 *
 * Note:            This FSM checks for new incoming packets,
 *                  and routes it to appropriate stack components.
 *                  It also performs timed operations.
 *
 *                  This function must be called periodically to
 *                  ensure timely responses.
 *
 ********************************************************************/
void StackTask(void)
{
    WORD dataCount;
    IP_ADDR tempLocalIP;
	BYTE cFrameType;
	BYTE cIPFrameType;

   
    #if defined( WF_CS_TRIS )
        // This task performs low-level MAC processing specific to the MRF24W
        MACProcess();
        #if defined( STACK_USE_EZ_CONFIG ) && !defined(__18CXX)
            WFEasyConfigMgr();
        #endif
        
    	#if defined(STACK_USE_DHCP_CLIENT)
        	// Normally, an application would not include  DHCP module
        	// if it is not enabled. But in case some one wants to disable
        	// DHCP module at run-time, remember to not clear our IP
        	// address if link is removed.
        	if(AppConfig.Flags.bIsDHCPEnabled)
        	{
        		if(g_DhcpRenew == TRUE)
        		{
        			g_DhcpRenew = FALSE;
            		AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
        			AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
        			AppConfig.Flags.bInConfigMode = TRUE;
        			DHCPInit(0);
        			g_DhcpRetryTimer = (UINT32)TickGet();
        		}
            else
            {
        			if (g_DhcpRetryTimer && ((TickGet() - g_DhcpRetryTimer) >= (TICKS_PER_SECOND * 8)))
              {
        				DHCPInit(0);
        				g_DhcpRetryTimer = (UINT32)TickGet();
        			}
        		}
        	
        		// DHCP must be called all the time even after IP configuration is
        		// discovered.
        		// DHCP has to account lease expiration time and renew the configuration
        		// time.
        		DHCPTask();
        		
        		if(DHCPIsBound(0)) {
        			AppConfig.Flags.bInConfigMode = FALSE;
    					g_DhcpRetryTimer = 0;
        		}
        	}
    	#endif // STACK_USE_DHCP_CLIENT
        
    #endif // WF_CS_TRIS


	#if defined(STACK_USE_DHCP_CLIENT) && !defined(WF_CS_TRIS)
	// Normally, an application would not include  DHCP module
	// if it is not enabled. But in case some one wants to disable
	// DHCP module at run-time, remember to not clear our IP
	// address if link is removed.
	if(AppConfig.Flags.bIsDHCPEnabled)
	{
		static BOOL bLastLinkState = FALSE;
		BOOL bCurrentLinkState;
		
		bCurrentLinkState = MACIsLinked();
		if(bCurrentLinkState != bLastLinkState)
		{
			bLastLinkState = bCurrentLinkState;
			if(!bCurrentLinkState)
			{
				AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
				AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
				AppConfig.Flags.bInConfigMode = TRUE;
				DHCPInit(0);
			}
		}
	
		// DHCP must be called all the time even after IP configuration is
		// discovered.
		// DHCP has to account lease expiration time and renew the configuration
		// time.
		DHCPTask();
		
		if(DHCPIsBound(0))
			AppConfig.Flags.bInConfigMode = FALSE;
	}
	#endif
	

    #if defined (STACK_USE_AUTO_IP)
    AutoIPTasks();
    #endif

	#if defined(STACK_USE_TCP)
	// Perform all TCP time related tasks (retransmit, send acknowledge, close connection, etc)
	TCPTick();
	#endif


	#if defined(STACK_USE_UDP)
	UDPTask();
	#endif

	// Process as many incomming packets as we can
	while(1)
	{
		//if using the random module, generate entropy
		#if defined(STACK_USE_RANDOM)
			RandomAdd(remoteNode.MACAddr.v[5]);
		#endif

		// We are about to fetch a new packet, make sure that the 
		// UDP module knows that any old RX data it has laying 
		// around will now be gone.
		#if defined(STACK_USE_UDP)
			UDPDiscard();
		#endif

		// Fetch a packet (throws old one away, if not thrown away 
		// yet)
		if(!MACGetHeader(&remoteNode.MACAddr, &cFrameType))
			break;

		// When using a WiFi module, filter out all incoming packets that have 
		// the same source MAC address as our own MAC address.  This is to 
		// prevent receiving and passing our own broadcast packets up to other 
		// layers and avoid, for example, having our own gratuitous ARPs get 
		// answered by ourself.
		#if defined(WF_CS_TRIS)
			if(memcmp((void*)&remoteNode.MACAddr, (void*)&AppConfig.MyMACAddr, 6) == 0u)
				continue;
		#endif

		// Dispatch the packet to the appropriate handler
		switch(cFrameType)
		{
			case MAC_ARP:
				ARPProcess();
				break;
	
			case MAC_IP:
				if(!IPGetHeader(&tempLocalIP, &remoteNode, &cIPFrameType, &dataCount))
					break;

				#if defined(STACK_USE_ICMP_SERVER) || defined(STACK_USE_ICMP_CLIENT)
				if(cIPFrameType == IP_PROT_ICMP)
				{
					#if defined(STACK_USE_IP_GLEANING)
					if(AppConfig.Flags.bInConfigMode && AppConfig.Flags.bIsDHCPEnabled)
					{
						// According to "IP Gleaning" procedure,
						// when we receive an ICMP packet with a valid
						// IP address while we are still in configuration
						// mode, accept that address as ours and conclude
						// configuration mode.
						if(tempLocalIP.Val != 0xffffffff)
						{
							AppConfig.Flags.bInConfigMode = FALSE;
							AppConfig.MyIPAddr = tempLocalIP;
						}
					}
					#endif

					// Process this ICMP packet if it the destination IP address matches our address or one of the broadcast IP addressees
					if( (tempLocalIP.Val == AppConfig.MyIPAddr.Val) ||
						(tempLocalIP.Val == 0xFFFFFFFF) ||
#if defined(STACK_USE_ZEROCONF_LINK_LOCAL) || defined(STACK_USE_ZEROCONF_MDNS_SD)
                                                (tempLocalIP.Val == 0xFB0000E0) ||
#endif
						(tempLocalIP.Val == ((AppConfig.MyIPAddr.Val & AppConfig.MyMask.Val) | ~AppConfig.MyMask.Val)))
					{
						ICMPProcess(&remoteNode, dataCount);
					}

					break;
				}
				#endif
				
				#if defined(STACK_USE_TCP)
				if(cIPFrameType == IP_PROT_TCP)
				{
					TCPProcess(&remoteNode, &tempLocalIP, dataCount);
					break;
				}
				#endif
				
				#if defined(STACK_USE_UDP)
				if(cIPFrameType == IP_PROT_UDP)
				{
					// Stop processing packets if we came upon a UDP frame with application data in it
					if(UDPProcess(&remoteNode, &tempLocalIP, dataCount))
						return;
				}
				#endif

				break;
		}
	}
}

/*********************************************************************
 * Function:        void StackApplications(void)
 *
 * PreCondition:    StackInit() is already called.
 *
 * Input:           None
 *
 * Output:          Calls all loaded application modules.
 *
 * Side Effects:    None
 *
 * Note:            This function must be called periodically to
 *                  ensure timely responses.
 *
 ********************************************************************/
void StackApplications(void)
{
	#if defined(STACK_USE_HTTP2_SERVER)
	HTTPServer();
	#endif
	
	#if defined(STACK_USE_FTP_SERVER) && defined(STACK_USE_MPFS2)
	FTPServer();
	#endif
	
	#if defined(STACK_USE_SNMP_SERVER)
	SNMPTask();
	#endif
	
	#if defined(STACK_USE_ANNOUNCE)
	DiscoveryTask();
	#endif
	
	#if defined(STACK_USE_NBNS)
	NBNSTask();
	#endif
	
	#if defined(STACK_USE_DHCP_SERVER)
	DHCPServerTask();
	#endif
	
	#if defined(STACK_USE_DNS_SERVER)
	DNSServerTask();
	#endif
	
	#if defined (STACK_USE_DYNAMICDNS_CLIENT)
	DDNSTask();
	#endif
	
	#if defined(STACK_USE_TELNET_SERVER)
	TelnetTask();
	#endif
	
	#if defined(STACK_USE_REBOOT_SERVER)
	RebootTask();
	#endif
	
	#if defined(STACK_USE_SNTP_CLIENT)
	SNTPClient();
	#endif
	
	#if defined(STACK_USE_UDP_PERFORMANCE_TEST)
	UDPPerformanceTask();
	#endif
	
	#if defined(STACK_USE_TCP_PERFORMANCE_TEST)
	TCPPerformanceTask();
	#endif
	
	#if defined(STACK_USE_SMTP_CLIENT)
	SMTPTask();
	#endif
	
	#if defined(STACK_USE_UART2TCP_BRIDGE)
	UART2TCPBridgeTask();
	#endif
}

#if defined(WF_CS_TRIS) && defined(STACK_USE_DHCP_CLIENT)
void RenewDhcp(void)
{
    g_DhcpRenew = TRUE;
    SetDhcpProgressState();
}    
    
#endif

#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))


