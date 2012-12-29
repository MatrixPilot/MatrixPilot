
#ifndef _MYIPNETWORK_C_
#define _MYIPNETWORK_C_

#include "defines.h"
#include "GenericTypeDefs.h"


#include "TCPIP Stack/TCPIP.h"
APP_CONFIG AppConfig;

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#include "HardwareProfile.h"
#include "MyIpNetwork.h"

//////////////////////////
// Enums
enum
{
	SM_HOME = 0,
	SM_SOCKET_OBTAINED,
	SM_CONNECTED,
} TelemetryState;


//////////////////////////
// Defines
#define ASYNC_DATA_BUFFER_SIZE (256)


//////////////////////////
// Variables
TCP_SOCKET MyTelemetrySocket = INVALID_SOCKET;
BYTE MyTelemetryState = SM_HOME;
int AsyncData_index_head = 0;
int AsyncData_index_tail = 0;
BYTE AsyncData_buffer[ASYNC_DATA_BUFFER_SIZE];
BOOL telemetryIsEOL = FALSE;

#if (USE_WIFI_NETWORK_LINK == 1)
	UINT8 ConnectionProfileID;
	#if !defined(MRF24WG)
		extern BOOL gRFModuleVer1209orLater;
	#endif // USE_WIFI_NETWORK_LINK
#endif // MRF24WG


//////////////////////////////////////
// Local Functions
static void InitAppConfig(void);
static void InitializeBoard(void);
void WF_Connect(void);
void InitTelemetry(void);
void ServiceTelemetry(void);
int ThreadSafeHeadIndexRead(void);
BOOL ThreadSafe_EOLcheck(BOOL doClearFlag);
void DisplayIPValue(IP_ADDR IPVal);

// initialize all network related parameters
void init_MyIpNetwork(void)
{
	// Initialize application specific hardware
	InitializeBoard();
	TickInit();
	#if defined(STACK_USE_MPFS2)
	MPFSInit();
	#endif

	// Initialize Stack and application related NV variables into AppConfig.
	InitAppConfig();
	// Initialize core stack layers (MAC, ARP, TCP, UDP) and
	// application modules (HTTP, SNMP, etc.)
    StackInit();

    #if (USE_WIFI_NETWORK_LINK == 1)
    #if defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST)
        g_WpsPassphrase.valid = FALSE;
    #endif    /* defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST) */
    WF_Connect();
    #endif

	// Initialize any application-specific modules or functions/
	// For this demo application, this only includes the
	// UART 2 TCP Bridge
	#if defined(STACK_USE_UART2TCP_BRIDGE)
	UART2TCPBridgeInit();
	#endif
	
	InitTelemetry();
}	

// Writes an IP address to the UART directly
#if defined(STACK_USE_UART)
void DisplayIPValue(IP_ADDR IPVal)
{
//	printf("%u.%u.%u.%u", IPVal.v[0], IPVal.v[1], IPVal.v[2], IPVal.v[3]);
    BYTE IPDigit[4];
	BYTE i;

	for(i = 0; i < sizeof(IP_ADDR); i++)
	{
	    uitoa((WORD)IPVal.v[i], IPDigit);
		putsUART((char *) IPDigit);
		if(i == sizeof(IP_ADDR)-1)
			break;
		while(BusyUART());
		WriteUART('.');
	}
}
#endif


/****************************************************************************
  Function:
    static void InitializeBoard(void)

  Description:
    This routine initializes the hardware.  It is a generic initialization
    routine for many of the Microchip development boards, using definitions
    in HardwareProfile.h to determine specific initialization.

  Precondition:
    None

  Parameters:
    None - None

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
static void InitializeBoard(void)
{	

#if defined(ENC_CS_TRIS)
	ENC_CS_IO = 1;
	ENC_CS_TRIS = 0;
	
	#ifdef WF_CS_TRIS
		DISABLE_WF_CS_IO = 1;
		DISABLE_WF_CS_TRIS = 0;	
	#endif
#endif
#if defined(WF_CS_TRIS)
	AD1PCFGHbits.PCFG16 = 1;	// Make RA12 (INT1) a digital input for MRF24WB0M interrupt
    WF_CS_IO = 1;
    WF_CS_TRIS = 0;

	#ifdef ENC_CS_TRIS
		DISABLE_ENC_CS_IO = 1;
		DISABLE_ENC_CS_TRIS = 0;
	#endif
#endif
}

/*********************************************************************
 * Function:        void InitAppConfig(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
// MAC Address Serialization using a MPLAB PM3 Programmer and 
// Serialized Quick Turn Programming (SQTP). 
// The advantage of using SQTP for programming the MAC Address is it
// allows you to auto-increment the MAC address without recompiling 
// the code for each unit.  To use SQTP, the MAC address must be fixed
// at a specific location in program memory.  Uncomment these two pragmas
// that locate the MAC address at 0x1FFF0.  Syntax below is for MPLAB C 
// Compiler for PIC18 MCUs. Syntax will vary for other compilers.
//#pragma romdata MACROM=0x1FFF0
static ROM BYTE SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
//#pragma romdata

static void InitAppConfig(void)
{
	
	while(1)
	{
		// Start out zeroing all AppConfig bytes to ensure all fields are 
		// deterministic for checksum generation
		memset((void*)&AppConfig, 0x00, sizeof(AppConfig));
		
		AppConfig.Flags.bIsDHCPEnabled = TRUE;
		AppConfig.Flags.bInConfigMode = TRUE;
		memcpypgm2ram((void*)&AppConfig.MyMACAddr, (ROM void*)SerializedMACAddress, sizeof(AppConfig.MyMACAddr));
//		{
//			_prog_addressT MACAddressAddress;
//			MACAddressAddress.next = 0x157F8;
//			_memcpy_p2d24((char*)&AppConfig.MyMACAddr, MACAddressAddress, sizeof(AppConfig.MyMACAddr));
//		}
		AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
		AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
		AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
		AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
		AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
		AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
		AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;
	
		
		// Load the default NetBIOS Host Name
		memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
		FormatNetBIOSName(AppConfig.NetBIOSName);
	
		break;
	}
}


void ServiceMyIpNetwork(void)
{
	static DWORD dwLastIP = 0;

	// This task performs normal stack task including checking
	// for incoming packet, type of packet and calling
	// appropriate stack entity to process it.
	StackTask();
       
	#if (USE_WIFI_NETWORK_LINK == 1)
	#if !defined(MRF24WG)
	if (gRFModuleVer1209orLater)
	#endif
		WiFiTask();
	#endif
	
	
	// This tasks invokes each of the core stack application tasks
	StackApplications();
	ServiceTelemetry();
	
	// If the local IP address has changed (ex: due to DHCP lease change)
	// write the new IP address to the UART and Announce service
	if(dwLastIP != AppConfig.MyIPAddr.Val)
	{
		dwLastIP = AppConfig.MyIPAddr.Val;
		
		#if defined(STACK_USE_UART)
		putrsUART((ROM char*)"\r\nNew IP Address: ");
		DisplayIPValue(AppConfig.MyIPAddr);
		putrsUART((ROM char*)"\r\n");
		#endif

		#if defined(STACK_USE_ANNOUNCE)
			AnnounceIP();
		#endif
	}
}

// This is called from within the _U2TXInterrupt when sending UART2 data.
// It takes a copy of the outgoing byte and loads it into a circular buffer
// which can later be asynchonously read by the TCPtelemetry in the idle thread
void LoadAsyncData(BYTE data)
{
	AsyncData_index_head++;
	if (AsyncData_index_head >= ASYNC_DATA_BUFFER_SIZE)
	{
		AsyncData_index_head = 0;
	}
	AsyncData_buffer[AsyncData_index_head] = data;
	
	if ('\n' == data)
	{
		telemetryIsEOL = TRUE;
	}

}
		

// init telemetry variables and states
void InitTelemetry(void)
{
	int i;
	MyTelemetryState = 0;
	MyTelemetrySocket = INVALID_SOCKET;
	telemetryIsEOL = FALSE;
	
	AsyncData_index_head = 0;
	AsyncData_index_tail = 0;
	for (i=0;i<ASYNC_DATA_BUFFER_SIZE;i++)
	{
		AsyncData_buffer[i] = 0;
	}	
}

// Read the circular buffer head index (written to from _U2TXInterrupt) from
// the "idle thread" in a thread-safe manner.
int ThreadSafeHeadIndexRead(void)
{
	BYTE isrState = _U2TXIE;
	_U2TXIE = 0; // inhibit the UART2 ISR from loading more (protect _head reads)
	int head = AsyncData_index_head;
	_U2TXIE = isrState; // resume ISR
	return head;
}

// Read the End-Of-Line flag (set in _U2TXInterrupt) and clear it from
// the "idle thread" in a thread-safe manner
BOOL ThreadSafe_EOLcheck(BOOL doClearFlag)
{
	BYTE isrState = _U2TXIE;
	_U2TXIE = 0; // inhibit the UART2 ISR from changing this on us during a read
	BYTE eolFound = telemetryIsEOL;
	if (doClearFlag)
	{
		telemetryIsEOL = FALSE;
	}
	_U2TXIE = isrState; // resume ISR
	
	return eolFound;
}

// Service the Telemetry system by checking for a TCP connection
// and then sending/recieveing data from the network accordingly

void ServiceTelemetry(void)
{
	int head, index;
	BYTE rxData;
	
	#if (USE_TELEMTETRY_BULK_WRITES == 1)
	int tx_buffer_available, wrote, len;
	#else
	BYTE txData;
	#endif
	
	#if defined(REMOTE_TELEMETRY_SERVER)
	if (MyTelemetrySocket != INVALID_SOCKET)
	{
		if (TCPWasReset(MyTelemetrySocket))
		{
			// If we were a client socket, close the socket and attempt to reconnect
			TCPDisconnect(MyTelemetrySocket);
			MyTelemetrySocket = INVALID_SOCKET;
			MyTelemetryState = SM_HOME;
		}
	}
	#endif

	
	// Handle session state
	switch(MyTelemetryState)
	{
		case SM_HOME:
			#if defined(REMOTE_TELEMETRY_SERVER)
				// Connect a socket to the remote TCP server
				MyTelemetrySocket = TCPOpen((DWORD)REMOTE_TELEMETRY_SERVER, TCP_OPEN_ROM_HOST, TELEMETRY_PORT, TCP_PURPOSE_TELEMETRY);
			#else
				// We are the server, start listening
				MyTelemetrySocket = TCPOpen(0, TCP_OPEN_SERVER, TELEMETRY_PORT, TCP_PURPOSE_TELEMETRY);
			#endif


			// Abort operation if no TCP socket of type TCP_PURPOSE_TELNET is available
			// If this ever happens, you need to go add one to TCPIPConfig.h
			if (INVALID_SOCKET == MyTelemetrySocket)
				break;
	
	
			// Eat the first TCPWasReset() response so we don't 
			// infinitely create and reset/destroy client mode sockets
			TCPWasReset(MyTelemetrySocket);

			MyTelemetryState++;
			break;

		case SM_SOCKET_OBTAINED:
			if (TCPIsPutReady(MyTelemetrySocket) >= 140)
			{
				// Print any one-time connection annoucement text
				TCPPutROMString(MyTelemetrySocket, "\r\nYou've connected to "); // 22 chars
				TCPPutROMString(MyTelemetrySocket, ID_LEAD_PILOT); // 15ish chars
				TCPPutROMString(MyTelemetrySocket, "'s aircraft. More info at "); // 26 chars
				TCPPutROMString(MyTelemetrySocket, ID_DIY_DRONES_URL); // 45ish chars
				TCPPutROMString(MyTelemetrySocket, "\r\n"); // 2 chars
				TCPFlush(MyTelemetrySocket); // send right away
				
				MyTelemetryState++;
			}
			break;

		case SM_CONNECTED:
		
		#if (USE_TELEMTETRY_BULK_WRITES == 1)
			head = ThreadSafeHeadIndexRead();
			if (head == AsyncData_index_tail)
			{
				// nothing to send, skip the while loop
				tx_buffer_available = 0;
			}
			else
			{
				tx_buffer_available = TCPIsPutReady(MyTelemetrySocket);
			}	
	
			// if we're not connected then IsPutReady will return 0. Send data if we can
			while ((AsyncData_index_tail != head) && (tx_buffer_available > 0))
			{
				index = AsyncData_index_tail + 1;

				if (AsyncData_index_tail < head) // usual (easy) case
				{
					len = head - AsyncData_index_tail;
					wrote = TCPPutArray(MyTelemetrySocket, &AsyncData_buffer[index], len);
				}
				else // circular buffer is wrapping around
				{
					if (AsyncData_index_tail >= (ASYNC_DATA_BUFFER_SIZE-1))
					{
						// only one byte before the end, just put it in by itself (single byte Put)
						wrote = TCPPut(MyTelemetrySocket, AsyncData_buffer[0]);
					}
					else
					{
						// fill from tail to end-of-buffer, let the next pass get the lower 0-to-head portion
						len = ASYNC_DATA_BUFFER_SIZE - AsyncData_index_tail - 1;
						wrote = TCPPutArray(MyTelemetrySocket, &AsyncData_buffer[index], len);
					}
				}
				
				if (wrote)
				{
					tx_buffer_available -= wrote;
					AsyncData_index_tail += wrote;
					if (AsyncData_index_tail >= ASYNC_DATA_BUFFER_SIZE)
					{
						AsyncData_index_tail = 0;
					}	
				}
				else
				{
					// something is wrong, we tried to write and failed so lets stop trying
					tx_buffer_available = 0;
				}
				head = ThreadSafeHeadIndexRead(); // refresh index_head
			} // while
			

		#else // single byte writes
			head = ThreadSafeHeadIndexRead();
			while (AsyncData_index_tail != head) // if there's data to write
			{
				index = AsyncData_index_tail + 1;
				if (index >= ASYNC_DATA_BUFFER_SIZE)
				{
					index = 0;
				}

				txData = AsyncData_buffer[index];
				// write if we can
				if (FALSE == TCPPut(MyTelemetrySocket, txData))
				{
					break;
				}

				// write was successful, increment indexes
				AsyncData_index_tail = index;
				// refresh the head index in case new data was just added
				head = ThreadSafeHeadIndexRead();
			} // while
		#endif
		
			if (ThreadSafe_EOLcheck(TRUE))
			{
				TCPFlush(MyTelemetrySocket);
			}

			
			if (TCPGet(MyTelemetrySocket, &rxData))
			{
				switch(rxData)
				{
				case '\r':
				case 'q':
				case 'Q':
					if (TCPIsPutReady(MyTelemetrySocket) > 15)
					{
						TCPPutROMString(MyTelemetrySocket, "\r\nGOODBYE!!!\r\n"); // 14 chars
					}
					#if !defined(REMOTE_TELEMETRY_SERVER)
						TCPDisconnect(MyTelemetrySocket);
						MyTelemetryState = SM_SOCKET_OBTAINED;	
					#endif
						break;
				} //switch rxData
			} // if get
			
		break;
	} // switch
}	

#if defined(WF_CS_TRIS)
/*****************************************************************************
 * FUNCTION: WF_Connect
 *
 * RETURNS:  None
 *
 * PARAMS:   None
 *
 *  NOTES:   Connects to an 802.11 network.  Customize this function as needed 
 *           for your application.
 *****************************************************************************/
void WF_Connect(void)
{
    UINT8 channelList[] = MY_DEFAULT_CHANNEL_LIST;
 
    /* create a Connection Profile */
    WF_CPCreate(&ConnectionProfileID);
    
    WF_SetRegionalDomain(MY_DEFAULT_DOMAIN);  

    WF_CPSetSsid(ConnectionProfileID, 
                 AppConfig.MySSID, 
                 AppConfig.SsidLength);
    
    WF_CPSetNetworkType(ConnectionProfileID, MY_DEFAULT_NETWORK_TYPE);
    
    WF_CASetScanType(MY_DEFAULT_SCAN_TYPE);
    
    
    WF_CASetChannelList(channelList, sizeof(channelList));
    
    // The Retry Count parameter tells the WiFi Connection manager how many attempts to make when trying
    // to connect to an existing network.  In the Infrastructure case, the default is to retry forever so that
    // if the AP is turned off or out of range, the radio will continue to attempt a connection until the
    // AP is eventually back on or in range.  In the Adhoc case, the default is to retry 3 times since the 
    // purpose of attempting to establish a network in the Adhoc case is only to verify that one does not
    // initially exist.  If the retry count was set to WF_RETRY_FOREVER in the AdHoc mode, an AdHoc network
    // would never be established. 
    WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT);

    WF_CASetEventNotificationAction(MY_DEFAULT_EVENT_NOTIFICATION_LIST);
    
    WF_CASetBeaconTimeout(MY_DEFAULT_BEACON_TIMEOUT);
    
    #if !defined(MRF24WG)
        if (gRFModuleVer1209orLater)
    #else
        {
            // If WEP security is used, set WEP Key Type.  The default WEP Key Type is Shared Key.
            if (AppConfig.SecurityMode == WF_SECURITY_WEP_40 || AppConfig.SecurityMode == WF_SECURITY_WEP_104)
            {
                WF_CPSetWepKeyType(ConnectionProfileID, MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE);
            }
        }    
    #endif
            
    #if defined(MRF24WG)
        // Error check items specific to WPS Push Button mode 
        #if (MY_DEFAULT_WIFI_SECURITY_MODE==WF_SECURITY_WPS_PUSH_BUTTON)
            #if !defined(WF_P2P)
                WF_ASSERT(strlen(AppConfig.MySSID) == 0);  // SSID must be empty when using WPS
                WF_ASSERT(sizeof(channelList)==11);        // must scan all channels for WPS       
            #endif

             #if (MY_DEFAULT_NETWORK_TYPE == WF_P2P)
                WF_ASSERT(strcmp((char *)AppConfig.MySSID, "DIRECT-") == 0);
                WF_ASSERT(sizeof(channelList) == 3);
                WF_ASSERT(channelList[0] == 1);
                WF_ASSERT(channelList[1] == 6);
                WF_ASSERT(channelList[2] == 11);           
            #endif
        #endif    

    #endif /* MRF24WG */

    #if defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST)
        if (AppConfig.SecurityMode == WF_SECURITY_WPA_WITH_PASS_PHRASE
            || AppConfig.SecurityMode == WF_SECURITY_WPA2_WITH_PASS_PHRASE
            || AppConfig.SecurityMode == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE) {
            WF_ConvPassphrase2Key(AppConfig.SecurityKeyLength, AppConfig.SecurityKey,
                AppConfig.SsidLength, AppConfig.MySSID);
            AppConfig.SecurityMode--;
            AppConfig.SecurityKeyLength = 32;
        }
    #if defined (MRF24WG)
        else if (AppConfig.SecurityMode == WF_SECURITY_WPS_PUSH_BUTTON
                    || AppConfig.SecurityMode == WF_SECURITY_WPS_PIN) {
            WF_YieldPassphrase2Host();    
        }
    #endif    /* defined (MRF24WG) */
    #endif    /* defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST) */

    WF_CPSetSecurity(ConnectionProfileID,
                     AppConfig.SecurityMode,
                     AppConfig.WepKeyIndex,   /* only used if WEP enabled */
                     AppConfig.SecurityKey,
                     AppConfig.SecurityKeyLength);

    #if MY_DEFAULT_PS_POLL == WF_ENABLED
        WF_PsPollEnable(TRUE);
    #if !defined(MRF24WG) 
        if (gRFModuleVer1209orLater)
            WFEnableDeferredPowerSave();
    #endif    /* !defined(MRF24WG) */
    #else     /* MY_DEFAULT_PS_POLL != WF_ENABLED */
        WF_PsPollDisable();
    #endif    /* MY_DEFAULT_PS_POLL == WF_ENABLED */

    #ifdef WF_AGGRESSIVE_PS
    #if !defined(MRF24WG)
        if (gRFModuleVer1209orLater)
            WFEnableAggressivePowerSave();
    #endif
    #endif
    
    #if defined(STACK_USE_UART)  
        WF_OutputConnectionInfo(&AppConfig);
    #endif
    
    #if defined(DISABLE_MODULE_FW_CONNECT_MANAGER_IN_INFRASTRUCTURE)
        WF_DisableModuleConnectionManager();
    #endif
    
    #if defined(MRF24WG)
        WFEnableDebugPrint(ENABLE_WPS_PRINTS | ENABLE_P2P_PRINTS);
    #endif
    WF_CMConnect(ConnectionProfileID);
}   
#endif /* (USE_WIFI_NETWORK_LINK == 1) */

#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPNETWORK_C_


