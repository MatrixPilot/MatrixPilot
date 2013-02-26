
#ifndef _MYIPNETWORK_C_
#define _MYIPNETWORK_C_


#include "defines.h"
#if (USE_NETWORK == 1)
#include "HardwareProfile.h"
//#include "../libUDB/libUDB_internal.h" // for indicate_loading_inter and pwmIn
#include "MyIpData.h"
#include "TCPIP_Stack/TCPIP.h"
APP_CONFIG AppConfig;



//////////////////////////
// Defines



//////////////////////////
// Enums


//////////////////////////
// Variables

#if (USE_WIFI_MRF24WG == 1)
    UINT8 ConnectionProfileID;
    #if !defined(MRF24WG)
        extern BOOL gRFModuleVer1209orLater;
    #endif // USE_WIFI_NETWORK_LINK
    #define WF_MODULE_NUMBER   WF_MODULE_MAIN_DEMO
#endif // USE_WIFI_MRF24WG


//////////////////////////////////////
// Local Functions
static void InitAppConfig(void);
static void InitializeBoard(void);
void WF_Connect(void);
void DisplayIPValue(const IP_ADDR IPVal);

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

    #if defined(WF_CS_TRIS)
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

    #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
    ZeroconfLLInitialize();
    #endif

    #if defined(STACK_USE_ZEROCONF_MDNS_SD)
    mDNSInitialize(MY_DEFAULT_HOST_NAME);
    mDNSServiceRegister(
        (const int8_t *) "DemoWebServer",    // base name of the service
        "_http._tcp.local",                // type of the service
        80,                                // TCP or UDP port, at which this service is available
        ((const uint8_t *)"path=/index.htm"), // TXT info
        1,                                 // auto rename the service when if needed
        NULL,                              // no callback function
        NULL                               // no application context
        );

    mDNSMulticastFilterRegister();
    #endif

    InitMyIpData();
}	

// Writes an IP address to the UART directly
#if defined(STACK_USE_UART)
void DisplayIPValue(const IP_ADDR IPVal)
{
    //printf("%u.%u.%u.%u", IPVal.v[0], IPVal.v[1], IPVal.v[2], IPVal.v[3]);
    uint8_t IPDigit[4];
    uint8_t i;

    for(i = 0; i < sizeof(IP_ADDR); i++)
    {
        uitoa((uint16_t)IPVal.v[i], IPDigit);
        putsUART((int8_t *) IPDigit);
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
#if (USE_WIFI_MRF24WG == 1)
    AD1PCFGHbits.PCFG17 = 1;	// Make AN17/RC2 a digital pin for MRF24WG0M Hibernate
    AD1PCFGHbits.PCFG18 = 1;	// Make AN18/RC3 a digital pin for MRF24WG0M Reset
    AD1PCFGHbits.PCFG20 = 1;	// Make AN20/RA12/INT1 a digital for MRF24WG0M interrupt

    WF_CS_IO = 1;
    WF_CS_TRIS = 0;

#elif (USE_ETHERNET_ENC28J60 == 1)
    AD1PCFGHbits.PCFG20 = 1;	// Make AN20/RA12/INT1 a digital for MRF24WG0M interrupt

    ENC_CS_IO = 1;
    ENC_CS_TRIS = 0;

#elif (USE_ETHERNET_ENC624J600 == 1)
    AD1PCFGHbits.PCFG20 = 1;	// Make AN20/RA12/INT1 a digital for MRF24WG0M interrupt

    ENC100_CS_IO = 1;
    ENC100_CS_TRIS = 0;
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
static ROM uint8_t SerializedMACAddress[6] = {MY_DEFAULT_MAC_BYTE1, MY_DEFAULT_MAC_BYTE2, MY_DEFAULT_MAC_BYTE3, MY_DEFAULT_MAC_BYTE4, MY_DEFAULT_MAC_BYTE5, MY_DEFAULT_MAC_BYTE6};
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
        //{
        //    _prog_addressT MACAddressAddress;
        //    MACAddressAddress.next = 0x157F8;
        //    _memcpy_p2d24((int8_t*)&AppConfig.MyMACAddr, MACAddressAddress, sizeof(AppConfig.MyMACAddr));
        //}
        AppConfig.MyIPAddr.Val = MY_DEFAULT_IP_ADDR_BYTE1 | MY_DEFAULT_IP_ADDR_BYTE2<<8ul | MY_DEFAULT_IP_ADDR_BYTE3<<16ul | MY_DEFAULT_IP_ADDR_BYTE4<<24ul;
        AppConfig.DefaultIPAddr.Val = AppConfig.MyIPAddr.Val;
        AppConfig.MyMask.Val = MY_DEFAULT_MASK_BYTE1 | MY_DEFAULT_MASK_BYTE2<<8ul | MY_DEFAULT_MASK_BYTE3<<16ul | MY_DEFAULT_MASK_BYTE4<<24ul;
        AppConfig.DefaultMask.Val = AppConfig.MyMask.Val;
        AppConfig.MyGateway.Val = MY_DEFAULT_GATE_BYTE1 | MY_DEFAULT_GATE_BYTE2<<8ul | MY_DEFAULT_GATE_BYTE3<<16ul | MY_DEFAULT_GATE_BYTE4<<24ul;
        AppConfig.PrimaryDNSServer.Val = MY_DEFAULT_PRIMARY_DNS_BYTE1 | MY_DEFAULT_PRIMARY_DNS_BYTE2<<8ul  | MY_DEFAULT_PRIMARY_DNS_BYTE3<<16ul  | MY_DEFAULT_PRIMARY_DNS_BYTE4<<24ul;
        AppConfig.SecondaryDNSServer.Val = MY_DEFAULT_SECONDARY_DNS_BYTE1 | MY_DEFAULT_SECONDARY_DNS_BYTE2<<8ul  | MY_DEFAULT_SECONDARY_DNS_BYTE3<<16ul  | MY_DEFAULT_SECONDARY_DNS_BYTE4<<24ul;



        // SNMP Community String configuration
        #if defined(STACK_USE_SNMP_SERVER)
        {
            uint8_t i;
            static ROM int8_t * ROM cReadCommunities[] = SNMP_READ_COMMUNITIES;
            static ROM int8_t * ROM cWriteCommunities[] = SNMP_WRITE_COMMUNITIES;
            ROM int8_t * strCommunity;

            for(i = 0; i < SNMP_MAX_COMMUNITY_SUPPORT; i++)
            {
                // Get a pointer to the next community string
                strCommunity = cReadCommunities[i];
                if(i >= sizeof(cReadCommunities)/sizeof(cReadCommunities[0]))
                    strCommunity = "";

                // Ensure we don't buffer overflow.  If your code gets stuck here,
                // it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h
                // is either too small or one of your community string lengths
                // (SNMP_READ_COMMUNITIES) are too large.  Fix either.
                if(strlenpgm(strCommunity) >= sizeof(AppConfig.readCommunity[0]))
                    while(1);

                // Copy string into AppConfig
                strcpypgm2ram((int8_t*)AppConfig.readCommunity[i], strCommunity);

                // Get a pointer to the next community string
                strCommunity = cWriteCommunities[i];
                if(i >= sizeof(cWriteCommunities)/sizeof(cWriteCommunities[0]))
                strCommunity = "";

                // Ensure we don't buffer overflow.  If your code gets stuck here,
                // it means your SNMP_COMMUNITY_MAX_LEN definition in TCPIPConfig.h
                // is either too small or one of your community string lengths
                // (SNMP_WRITE_COMMUNITIES) are too large.  Fix either.
                if(strlenpgm(strCommunity) >= sizeof(AppConfig.writeCommunity[0]))
                    while(1);

                // Copy string into AppConfig
                strcpypgm2ram((int8_t*)AppConfig.writeCommunity[i], strCommunity);
            }
        }
        #endif

        // Load the default NetBIOS Host Name
        memcpypgm2ram(AppConfig.NetBIOSName, (ROM void*)MY_DEFAULT_HOST_NAME, 16);
        FormatNetBIOSName(AppConfig.NetBIOSName);


        #if defined(WF_CS_TRIS)
        // Load the default SSID Name
        WF_ASSERT(sizeof(MY_DEFAULT_SSID_NAME) <= sizeof(AppConfig.MySSID));
        memcpypgm2ram(AppConfig.MySSID, (ROM void*)MY_DEFAULT_SSID_NAME, sizeof(MY_DEFAULT_SSID_NAME));
        AppConfig.SsidLength = sizeof(MY_DEFAULT_SSID_NAME) - 1;

        AppConfig.SecurityMode = MY_DEFAULT_WIFI_SECURITY_MODE;

        #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
            memset(AppConfig.SecurityKey, 0x00, sizeof(AppConfig.SecurityKey));
            AppConfig.SecurityKeyLength = 0;

        #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40
            AppConfig.WepKeyIndex  = MY_DEFAULT_WEP_KEY_INDEX;
            memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_40, sizeof(MY_DEFAULT_WEP_KEYS_40) - 1);
            AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_40) - 1;

        #elif MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104
            AppConfig.WepKeyIndex  = MY_DEFAULT_WEP_KEY_INDEX;
            memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WEP_KEYS_104, sizeof(MY_DEFAULT_WEP_KEYS_104) - 1);
            AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WEP_KEYS_104) - 1;

        #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY)       || \
            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY)      || \
            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY)
            memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK, sizeof(MY_DEFAULT_PSK) - 1);
            AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK) - 1;

        #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE)     || \
            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE)    || \
            (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)
            memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_PSK_PHRASE, sizeof(MY_DEFAULT_PSK_PHRASE) - 1);
            AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_PSK_PHRASE) - 1;
        #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON)
            memset(AppConfig.SecurityKey, 0x00, sizeof(AppConfig.SecurityKey));
            AppConfig.SecurityKeyLength = 0;
        #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)
            memcpypgm2ram(AppConfig.SecurityKey, (ROM void*)MY_DEFAULT_WPS_PIN, sizeof(MY_DEFAULT_WPS_PIN) - 1);
            AppConfig.SecurityKeyLength = sizeof(MY_DEFAULT_WPS_PIN) - 1;
        #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_EAP)
            memset(AppConfig.SecurityKey, 0x00, sizeof(AppConfig.SecurityKey));
            AppConfig.SecurityKeyLength = 0;
        #else
            #error "No security defined"
        #endif /* MY_DEFAULT_WIFI_SECURITY_MODE */

        #endif
        break;
    }
}

#if defined(WF_CS_TRIS)
void WF_Connect(void)
{
    UINT8 channelList[] = MY_DEFAULT_CHANNEL_LIST;
 
    // create a Connection Profile
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
                WF_ASSERT(strcmp((int8_t *)AppConfig.MySSID, "DIRECT-") == 0);
                WF_ASSERT(sizeof(channelList) == 3);
                WF_ASSERT(channelList[0] == 1);
                WF_ASSERT(channelList[1] == 6);
                WF_ASSERT(channelList[2] == 11);           
            #endif
        #endif    

    #endif // MRF24WG

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
    #endif    // defined (MRF24WG)
    #endif    // defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST)

    WF_CPSetSecurity(ConnectionProfileID,
                     AppConfig.SecurityMode,
                     AppConfig.WepKeyIndex,   // only used if WEP enabled
                     AppConfig.SecurityKey,
                     AppConfig.SecurityKeyLength);

    #if MY_DEFAULT_PS_POLL == WF_ENABLED
        WF_PsPollEnable(TRUE);
    #if !defined(MRF24WG) 
        if (gRFModuleVer1209orLater)
            WFEnableDeferredPowerSave();
    #endif    // !defined(MRF24WG)
    #else     // MY_DEFAULT_PS_POLL != WF_ENABLED
        WF_PsPollDisable();
    #endif    // MY_DEFAULT_PS_POLL == WF_ENABLED

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
#endif // (WF_CS_TRIS == 1)


void ServiceMyIpNetwork(void)
{
    static uint32_t dwLastIP = 0;
    uint8_t s;

    // TODO: This is something to experiment with for cpu usage calc
    //indicate_loading_inter ;


    // This task performs normal stack task including checking
    // for incoming packet, type of packet and calling
    // appropriate stack entity to process it.
    StackTask();

    #if (USE_WIFI_MRF24WG == 1)
    #if !defined(MRF24WG)
    if (gRFModuleVer1209orLater)
    #endif
    WiFiTask();
    #endif


    // This tasks invokes each of the core stack application tasks
    StackApplications();

    static uint32_t ledBlinkTimer = 0;
    if(TickGet() - ledBlinkTimer > (TICK_SECOND/4))
    {
        ledBlinkTimer = TickGet();
        LED_ORANGE ^= 1;
    }


    #if defined(STACK_USE_DHCP_CLIENT)
    static uint32_t dwTimer = 0;

    // Wait until DHCP module is finished
    if(DHCPIsEnabled(0) && !DHCPIsBound(0))
    {
        dwTimer = TickGet();
    }

    // Wait an additional half second after DHCP is finished to let the announce module and any other stack state machines to reach normal operation
    else if(TickGet() - dwTimer > (TICK_SECOND/2))
    #endif
    {
        boolean isMacLinked = MACIsLinked();
        boolean tcpIsConnected = FALSE;
        for (s = 0; s < NumSockets(); s++)
        {
            tcpIsConnected |= ServiceMyIpTCP(s,isMacLinked);
            if (isMacLinked)
            {
                ServiceMyIpUDP(s);
            }
            ServiceMyIpData(s);
        } // for

        if (tcpIsConnected)
            LED_TCP_CONNECTED = LED_ON;
        else
            LED_TCP_CONNECTED = LED_OFF;
    } // if DHCP

    // If the local IP address has changed (ex: due to DHCP lease change)
    // write the new IP address to the UART and Announce service
    if(dwLastIP != AppConfig.MyIPAddr.Val)
    {
        dwLastIP = AppConfig.MyIPAddr.Val;

        #if defined(STACK_USE_UART)
        putrsUART((ROM int8_t*)"\r\nNew IP Address: ");
        DisplayIPValue(AppConfig.MyIPAddr);
        putrsUART((ROM int8_t*)"\r\n");
        #endif

        #if defined(STACK_USE_ANNOUNCE)
        AnnounceIP();
        #endif
    }
}


#endif // #if (USE_NETWORK == 1)
#endif // _MYIPNETWORK_C_


