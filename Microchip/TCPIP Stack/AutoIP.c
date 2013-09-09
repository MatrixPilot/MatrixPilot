/*********************************************************************
 *
 *	AutoIP Client 
 *  Module for Microchip TCP/IP Stack
 *	 -Provides AutoIP Code for automatically allocating a link-layer
 *      address
 *	
 *	-Reference: RFC 3927   	
 *********************************************************************
 * FileName:        AutoIP.c
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
 * Copyright (C) 2009 Microchip Technology Inc.  All rights
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
 * Peter Reen           7/10/09  	Original        (Rev 1.0)
 ********************************************************************/

#define __AUTOIP_C

#include "TCPIPConfig.h"

#if defined (STACK_USE_AUTO_IP)

#include "TCPIP Stack/TCPIP.h"

// Set of variables for each network interface
typedef struct
{
    // Global state-tracker variable
    SM_AUTOIP smAUTOIPState;
    // Global backoff-time counter
    DWORD gAutoIPConflictTimer;
    // Records the last time at which an AutoIP event occured
    DWORD eventTime;
    // An ARP packet
    ARP_PACKET packet;
    // Holds the number of ticks needed for a random delay
    DWORD randomDelay;
    // The seed value for the RNG
    DWORD wRandSeed;

	union
	{
	    struct
	    {
	        unsigned char gDisableAutoIP : 1;   // Prevents AutoIP from initializing if the user wants a static address
	        unsigned char bConfigureAutoIP : 1;
	        unsigned char bLastLinkState : 1;
			unsigned char checkAddress : 1;
			unsigned char bCurrentLinkState : 1;
            #if defined (STACK_USE_DHCP_CLIENT)
                unsigned char bLastDHCPState : 1;
            #endif
	    } bits;
	    BYTE val;
	} flags;


    BYTE conflicts;
} AUTOIP_CLIENT_VARS;

// Prototypes

void AutoIPRandSeed (DWORD seed, BYTE vInterface);
DWORD AutoIPRand (BYTE vInterface);


/*****************************************************************************
  Function:
	static void LoadState(BYTE vInterface)

  Summary:
	Saves the AutoIP client state information structure to the appropriate 
	location and loads AutoIP client with the state information for the specified 
	interface.

  Description:
	Saves the AutoIP client state information structure to the appropriate 
	location and loads AutoIP client with the state information for the specified 
	interface.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
  	This function does nothing when you only have one physical interface.
***************************************************************************/
#if NETWORK_INTERFACES > 1

static AUTOIP_CLIENT_VARS	AutoIPClients[NETWORK_INTERFACES];
static AUTOIP_CLIENT_VARS	*SelectedAutoIPClient;
#define AutoIPClient		(*SelectedAutoIPClient)
#define LoadState(v)	do(SelectedAutoIPClient = &AutoIPClients[v])while(0)

#else

static AUTOIP_CLIENT_VARS AutoIPClient;
#define LoadState(v)

#endif


/*****************************************************************************
  Function:
	void AutoIPInit(BYTE vInterface)

  Summary:
	Resets the AutoIP client module for the specified interface.

  Description:
	Resets the AutoIP client module

  Precondition:
	None

  Parameters:
	vInterface - Interface number to initialize AutoIP client state variables 
		for.   If you only have one interface, specify 0x00.

  Returns:
	None

  Remarks:
	This function may be called multiple times throughout the life of the 
	application, if desired.  
***************************************************************************/

void AutoIPInit (BYTE vInterface)
{
    LoadState (vInterface);

    AutoIPClient.flags.bits.bConfigureAutoIP = FALSE;
    AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
    AutoIPClient.gAutoIPConflictTimer = 0;
    AutoIPClient.flags.bits.bLastLinkState = FALSE;
    AutoIPClient.flags.bits.checkAddress = FALSE;
    AutoIPClient.conflicts = 0;
    #if defined (STACK_USE_DHCP_CLIENT)
        AutoIPClient.flags.bits.bLastDHCPState = FALSE;
    #endif
}


/*****************************************************************************
  Function:
	BOOL AutoIPIsConfigured (BYTE vInterface)

  Summary:
	Determines if the AutoIP interface has successfully claimed an IP address

  Description:
	Determines if the AutoIP interface has successfully claimed an IP address

  Precondition:
	None

  Parameters:
	vInterface - Interface number to check AutoIP configuration for. If you 
        only have one interface, specify 0x00.

  Returns:
	TRUE - The AutoIP client has successfully claimed an IP address
    FALSE - The AutoIP client has not claimed an IP address

  Remarks:
    None
***************************************************************************/
BOOL AutoIPIsConfigured (BYTE vInterface)
{
    LoadState(vInterface);
    return (AutoIPClient.smAUTOIPState == SM_AUTOIP_CONFIGURED) ? TRUE : FALSE;
}

/*****************************************************************************
  Function:
	BOOL AutoIPConfigIsInProgress (BYTE vInterface)

  Summary:
	Determines if the AutoIP address is being configured

  Description:
	Determines if the AutoIP address is being configured

  Precondition:
	None

  Parameters:
	vInterface - Interface number to check AutoIP configuration for. If you 
        only have one interface, specify 0x00.

  Returns:
	TRUE - The AutoIP client is being configured
    FALSE - The AutoIP client is not in configuration mode

  Remarks:
    None
***************************************************************************/
BOOL AutoIPConfigIsInProgress (BYTE vInterface)
{
    LoadState(vInterface);
    return AutoIPClient.flags.bits.bConfigureAutoIP;
}

/*****************************************************************************
  Function:
	void AutoIPTasks(void)

  Summary:
	Performs periodic AutoIP tasks.

  Description:
	This function performs any periodic tasks requied by the AutoIP module, 
	such as re/transmitting gratuitous ARP packets and defending addresses

  Precondition:
	None

  Parameters:
	None

  Returns:
	None
***************************************************************************/

void AutoIPTasks(void)
{
    BYTE i;

    for (i = 0; i < NETWORK_INTERFACES; i++)
    {
        LoadState (i);
        AutoIPClient.flags.bits.bCurrentLinkState = MACIsLinked();
    	if(AutoIPClient.flags.bits.bCurrentLinkState != AutoIPClient.flags.bits.bLastLinkState)
    	{
    		AutoIPClient.flags.bits.bLastLinkState = AutoIPClient.flags.bits.bCurrentLinkState;
    		if(!AutoIPClient.flags.bits.bCurrentLinkState)
    		{
                AutoIPClient.flags.bits.bConfigureAutoIP = FALSE;
                AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
    			AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
    			AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
    		}
            else
            {
                AutoIPClient.smAUTOIPState = SM_AUTOIP_INIT_RNG;
            }
    	}
    
        #if defined (STACK_USE_DHCP_CLIENT)
        if (DHCPIsBound(i))
        {
            AutoIPClient.flags.bits.bConfigureAutoIP = FALSE;
            AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
            AutoIPClient.flags.bits.bLastDHCPState = TRUE;
        }
        else
        {
            if (AutoIPClient.flags.bits.bLastDHCPState == TRUE)
            {
                if (AutoIPClient.flags.bits.bCurrentLinkState)
                    AutoIPClient.smAUTOIPState = SM_AUTOIP_INIT_RNG;
            }
            AutoIPClient.flags.bits.bLastDHCPState = FALSE;
        }
        #endif
    
    
        if (AutoIPClient.flags.bits.gDisableAutoIP == TRUE)
        {
            AutoIPClient.flags.bits.bConfigureAutoIP = FALSE;
            AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
        }
    
    
        switch (AutoIPClient.smAUTOIPState)
        {
            // Default no-AutoIP case
        	case SM_AUTOIP_DISABLED:

                break;
    
            // Initializes the random number generator with a seed based on the MAC address
            case SM_AUTOIP_INIT_RNG:
                AutoIPRandSeed (((DWORD)AppConfig.MyMACAddr.v[0] + ((DWORD)AppConfig.MyMACAddr.v[1] << 8) + \
                        ((DWORD)AppConfig.MyMACAddr.v[2] << 16) + ((DWORD)AppConfig.MyMACAddr.v[3] << 24) + \
                        ((DWORD)AppConfig.MyMACAddr.v[4]) + ((DWORD)AppConfig.MyMACAddr.v[5] << 8)), i);
    
                AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;
    
            // Check the address to see if it's in use before we write it into AppConfig
            case SM_AUTOIP_CHECK_ADDRESS:
    
                if (AutoIPClient.flags.bits.checkAddress == FALSE)
                {
                    AutoIPClient.flags.bits.checkAddress = TRUE;
    
                    AppConfig.MyMask.Val = 0x00000000;
    
                    // Generate a random IP address (based on the MAC address) to try and claim.
                    // Dynamic link-local addresses can fall within the range:
                    // 169.254.1.0 - 169.254.254.255
                    AutoIPClient.packet.TargetIPAddr.byte.MB = AutoIPRand(i) % 256;
                    AutoIPClient.packet.TargetIPAddr.byte.UB = (AutoIPRand(i) % 254) + 1;
                    AutoIPClient.packet.TargetIPAddr.word.LW = 0xFEA9;
    
                    ARPResolve (&AutoIPClient.packet.TargetIPAddr);
    
                    AutoIPClient.eventTime = TickGet();
                }
                
                if (!ARPIsResolved (&AutoIPClient.packet.TargetIPAddr, &AutoIPClient.packet.TargetMACAddr))
                {
                    if (TickGet() - AutoIPClient.eventTime > TICK_SECOND)
                    {
                        AutoIPClient.smAUTOIPState = SM_AUTOIP_SETUP_MESSAGE;
                    }
                }
                else
                {
                    AutoIPClient.flags.bits.checkAddress = FALSE;
                }
    
                break;
    
            // Set up an ARP packet
            case SM_AUTOIP_SETUP_MESSAGE:
    
                AutoIPClient.flags.bits.checkAddress = FALSE;
    
                // Set the bConfigureAutoIP flag- This flag will cause an AutoIP conflict
                // if a response packet is received from the address we're trying to claim.
                AutoIPClient.flags.bits.bConfigureAutoIP = TRUE;
    
                // Configure the fields for a gratuitous ARP packet
            	AutoIPClient.packet.Operation            = ARP_OPERATION_REQ;
            
            	AutoIPClient.packet.TargetMACAddr.v[0]   = 0xff;
            	AutoIPClient.packet.TargetMACAddr.v[1]   = 0xff;
            	AutoIPClient.packet.TargetMACAddr.v[2]   = 0xff;
            	AutoIPClient.packet.TargetMACAddr.v[3]   = 0xff;
            	AutoIPClient.packet.TargetMACAddr.v[4]   = 0xff;
            	AutoIPClient.packet.TargetMACAddr.v[5]   = 0xff;
    
                AppConfig.MyIPAddr = AutoIPClient.packet.TargetIPAddr;
                AppConfig.MyMask.Val = 0x0000FFFF;
            	memcpy(&AutoIPClient.packet.SenderMACAddr, (void*)&AppConfig.MyMACAddr, sizeof(AutoIPClient.packet.SenderMACAddr));
                AutoIPClient.packet.HardwareType  = HW_ETHERNET;
                AutoIPClient.packet.Protocol      = ARP_IP;
                AutoIPClient.packet.MACAddrLen    = sizeof(MAC_ADDR);
                AutoIPClient.packet.ProtocolLen   = sizeof(IP_ADDR);
                AutoIPClient.packet.SenderIPAddr.Val  = AutoIPClient.packet.TargetIPAddr.Val;
    
                SwapARPPacket(&AutoIPClient.packet);
    
                // Generate a random delay between 0 and 1 second
                AutoIPClient.randomDelay = ((LFSRRand() % 20) * TICK_SECOND) / 20;
                // Store the current time
                AutoIPClient.eventTime = TickGet();
    
                // Set the state to send the ARP packet
                AutoIPClient.smAUTOIPState = SM_AUTOIP_GRATUITOUS_ARP1;
    
                break;
    
            // Send a gratuitous ARP packet to try and claim our address
            case SM_AUTOIP_GRATUITOUS_ARP1:
            case SM_AUTOIP_GRATUITOUS_ARP2:
            case SM_AUTOIP_GRATUITOUS_ARP3:
                // Check to ensure we've passed the delay time
                if (TickGet() - AutoIPClient.eventTime > AutoIPClient.randomDelay)
                {
                    // Store the new event time
                    AutoIPClient.eventTime = TickGet();
                    // Generate a new random delay between 1 and 2 seconds
                    AutoIPClient.randomDelay = TICK_SECOND + (((LFSRRand() % 20) * TICK_SECOND) / 20);
    
                    // Transmit the packet
                	while(!MACIsTxReady());
                	MACSetWritePtr(BASE_TX_ADDR);
    
                    MACPutHeader(&AutoIPClient.packet.TargetMACAddr, MAC_ARP, sizeof(AutoIPClient.packet));
                    MACPutArray((BYTE*)&AutoIPClient.packet, sizeof(AutoIPClient.packet));
                    MACFlush();
    
                    // Increment the probe iteration or increment to the delay state
                    AutoIPClient.smAUTOIPState++;
                }
                break;
    
            // Delay for 1-2 seconds after sending the third ARP request before
            // entering the configured state
            case SM_AUTOIP_DELAY:
                if (TickGet() - AutoIPClient.eventTime > AutoIPClient.randomDelay)
                    AutoIPClient.smAUTOIPState = SM_AUTOIP_CONFIGURED;
                break;
    
            // Configure the module to limit the rate at which packets are sent
            case SM_AUTOIP_RATE_LIMIT_SET:
                AutoIPClient.eventTime = TickGet();
                AppConfig.MyIPAddr.v[0] = MY_DEFAULT_IP_ADDR_BYTE1;
                AppConfig.MyIPAddr.v[1] = MY_DEFAULT_IP_ADDR_BYTE2;
                AppConfig.MyIPAddr.v[2] = MY_DEFAULT_IP_ADDR_BYTE3;
                AppConfig.MyIPAddr.v[3] = MY_DEFAULT_IP_ADDR_BYTE4;
                AutoIPClient.smAUTOIPState = SM_AUTOIP_RATE_LIMIT_WAIT;
                break;
    
            // Ensure that we don't try more than one address every 60 seconds
            case SM_AUTOIP_RATE_LIMIT_WAIT:
                if (TickGet() - AutoIPClient.eventTime > TICK_SECOND * 60)
                    AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;
                break;
    
            // Configured state
            case SM_AUTOIP_CONFIGURED:
                AutoIPClient.flags.bits.bConfigureAutoIP = FALSE;
                break;
    
            // Address defense state
            case SM_AUTOIP_DEFEND:
                // Prepare and send an ARP response
                AutoIPClient.packet.Operation     = ARP_OPERATION_RESP;
                AutoIPClient.packet.HardwareType  = HW_ETHERNET;
                AutoIPClient.packet.Protocol      = ARP_IP;
    
                SwapARPPacket(&AutoIPClient.packet);
    
            	while(!MACIsTxReady());
            	MACSetWritePtr(BASE_TX_ADDR);
    
                MACPutHeader(&AutoIPClient.packet.TargetMACAddr, MAC_ARP, sizeof(AutoIPClient.packet));
                MACPutArray((BYTE*)&AutoIPClient.packet, sizeof(AutoIPClient.packet));
                MACFlush();
    
                AutoIPClient.smAUTOIPState = SM_AUTOIP_CONFIGURED;
                break;
        }
    }
}

/*****************************************************************************
  Function:
	void AutoIPConflict(BYTE vInterface)

  Summary:
	Handles AutoIP address conflicts.

  Description:
	This function will change the state machine to handle AutoIP address
    conflicts.

  Precondition:
	None

  Parameters:
	vInterface - Interface number to cause an AutoIP conflict for.   If 
        you only have one interface, specify 0x00.

  Returns:
	None
***************************************************************************/
void AutoIPConflict (BYTE vInterface)
{
    LoadState (vInterface);

    AutoIPClient.conflicts++;

    // State handler
    switch (AutoIPClient.smAUTOIPState)
    {
        // During configuration, if there is a conflict, immediately give
        // up the current address and select a new one.
        // If more than 10 conflicts have occured, limit the rate of
        // address retrys to 1 every 60 seconds.
        case SM_AUTOIP_INIT_RNG:
        case SM_AUTOIP_CHECK_ADDRESS:
        case SM_AUTOIP_SETUP_MESSAGE:
        case SM_AUTOIP_GRATUITOUS_ARP1:
        case SM_AUTOIP_GRATUITOUS_ARP2:
        case SM_AUTOIP_GRATUITOUS_ARP3:
        case SM_AUTOIP_DELAY:
            if (AutoIPClient.conflicts >= 10u)
                AutoIPClient.smAUTOIPState = SM_AUTOIP_RATE_LIMIT_SET;
            else
                AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;
            break;
        case SM_AUTOIP_RATE_LIMIT_SET:
        case SM_AUTOIP_RATE_LIMIT_WAIT:
        case SM_AUTOIP_DISABLED:
            AutoIPClient.conflicts--;
            break;
        // If there is a conflict while we have an address configured,
        // send a defense packet.  If more than one conflict occurs within
        // 10 seconds, claim a new address.
        case SM_AUTOIP_CONFIGURED:
        case SM_AUTOIP_DEFEND:
            if (AutoIPClient.gAutoIPConflictTimer != 0u)
            {
                if (TickGet() - AutoIPClient.gAutoIPConflictTimer < TICK_SECOND * 10)
                {
                    AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;
                    return;;
                }
            }
            AutoIPClient.gAutoIPConflictTimer = TickGet();
            AutoIPClient.smAUTOIPState = SM_AUTOIP_DEFEND;
            break;

    }
}


/*****************************************************************************
  Function:
	void AutoIPDisable (BYTE vInterface)

  Summary:
	Disables the AutoIP module.

  Description:
	Disables the AutoIP module.  If DHCP is enabled, this function will
    reset the IP address to the default address if no DHCP address has been
    bound.  If DHCP is disabled, this function will reset the IP address to
    the default address.

  Precondition:
	None

  Parameters:
	vInterface - Interface number to disable AutoIP for.   If you only 
        have one interface, specify 0x00.

  Returns:
	None

  Remarks:
    None
***************************************************************************/

void AutoIPDisable (BYTE vInterface)
{
    LoadState (vInterface);

    AutoIPClient.flags.bits.gDisableAutoIP = TRUE;

    #if defined (STACK_USE_DHCP_CLIENT)
    if (!DHCPIsBound(vInterface))
    {
		AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
		AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
    }
    #else
	AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
	AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
    #endif
}

/*****************************************************************************
  Function:
	void AutoIPEnable (BYTE vInterface)

  Summary:
	Enables the AutoIP module.

  Description:
	Enables the AutoIP module.  This function will end the manual-disable
    condition for AutoIP, and reset the state machine to the beginning.
    If a DHCP address is bound or the link is broken, the state will be
    automatically set back to disabled.

  Precondition:
	None

  Parameters:
	vInterface - Interface number to enable AutoIP for. If you only have 
        one interface, specify 0x00.

  Returns:
	None

  Remarks:
    None
***************************************************************************/

void AutoIPEnable (BYTE vInterface)
{
    LoadState (vInterface);

    AutoIPClient.flags.bits.gDisableAutoIP = FALSE;
    AutoIPClient.smAUTOIPState = SM_AUTOIP_INIT_RNG;
}


/*****************************************************************************
  Function:
	BOOL AutoIPIsDisabled (BYTE vInterface)

  Summary:
	Determines if the AutoIP state machine is in a disabled state.

  Description:
	Determines if the AutoIP state machine is in a disabled state.  This
    could occur because a DHCP address is bound, the link is broken, or
    the user has manually disabled the AutoIP module.

  Precondition:
	None

  Parameters:
	vInterface - Interface number to check the AutoIP disable status for.   
        If you only have one interface, specify 0x00.

  Returns:
	TRUE - The AutoIP client is disabled
    FALSE - The AutoIP client in active

  Remarks:
    None
***************************************************************************/

BOOL AutoIPIsDisabled (BYTE vInterface)
{
    LoadState (vInterface);

    return (AutoIPClient.smAUTOIPState == SM_AUTOIP_DISABLED)?TRUE:FALSE;
}

/*****************************************************************************
  Function:
	void AutoIPRandSeed (DWORD seed, BYTE vInterface)

  Summary:
	Seeds a random number generator used to generate a MAC

  Description:
	Seeds a random number generator used to generate a MAC

  Precondition:
	None

  Parameters:
	seed - The seed value for the RNG
    vInterface - The interface to seed the generator for

  Returns:
	None

  Remarks:
    None
***************************************************************************/

void AutoIPRandSeed (DWORD seed, BYTE vInterface)
{
    LoadState (vInterface);
    AutoIPClient.wRandSeed = seed;
}

/*****************************************************************************
  Function:
	DWORD AutoIPRand (BYTE vInterface)

  Summary:
	Generates a random number

  Description:
	Generates a random number using a LFSR

  Precondition:
	None

  Parameters:
	vInterface - The interface to generate the RNG for

  Returns:
	A random number from 0 to 2^32-1

  Remarks:
    None
***************************************************************************/

DWORD AutoIPRand (BYTE vInterface)
{
    LoadState (vInterface);
    LFSRSeedRand(AutoIPClient.wRandSeed);
    AutoIPClient.wRandSeed = LFSRRand();
    return AutoIPClient.wRandSeed;
}

#endif






