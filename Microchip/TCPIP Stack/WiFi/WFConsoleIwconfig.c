/******************************************************************************

 MRF24W Driver iwconfig
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFConsoleIwconfig.c
 Dependencies:  TCP/IP Stack header files
 Processor:     PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 Compiler:      Microchip C32 v1.10b or higher
                Microchip C30 v3.22 or higher
                Microchip C18 v3.34 or higher
 Company:       Microchip Technology, Inc.

 Software License Agreement

 Copyright (C) 2002-2010 Microchip Technology Inc.  All rights reserved.

 Microchip licenses to you the right to use, modify, copy, and distribute:
 (i)  the Software when embedded on a Microchip microcontroller or digital 
      signal controller product ("Device") which is integrated into 
      Licensee's product; or
 (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
      ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device used in 
      conjunction with a Microchip ethernet controller for the sole purpose 
      of interfacing with the ethernet controller.

 You should refer to the license agreement accompanying this Software for 
 additional information regarding your rights and obligations.

 THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 NON-INFRINGEMENT. IN NO EVENT SHALL MICROCHIP BE LIABLE FOR ANY INCIDENTAL,
 SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST
 OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS BY
 THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS
 FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON
 THE BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR
 OTHERWISE.


 Author             Date        Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 KH                 27 Jan 2010 Updated for MRF24W
******************************************************************************/


//============================================================================
//                                  Includes
//============================================================================
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/WFConsole.h"

#if defined( WF_CONSOLE_IFCFGUTIL )

#include "TCPIP Stack/WFConsoleIwconfig.h"
#include "TCPIP Stack/WFConsoleMsgs.h"
#include "TCPIP Stack/WFConsoleMsgHandler.h"

#if defined ( EZ_CONFIG_SCAN )
#include "TCPIP Stack/WFEasyConfig.h"
#endif /* EZ_CONFIG_SCAN */
//============================================================================
//                                  Constants
//============================================================================

//============================================================================
//                                  Globals
//============================================================================

static BOOL      iwconfigCbInitialized = FALSE;
tWFIwconfigCb    iwconfigCb;

//============================================================================
//                                  Local Function Prototypes
//============================================================================
static void iwconfigDisplayStatus(void);
static BOOL iwconfigSetSsid(void);
static BOOL iwconfigSetMode(void);
static BOOL iwconfigSetChannel(void);
static BOOL iwconfigSetPower(void);
static BOOL iwconfigSetDomain(void);
static BOOL iwconfigSetRTS(void);
static BOOL iwconfigSetTxRate(void);
static BOOL iwconfigSetConnect(void);
#ifdef STACK_USE_CERTIFICATE_DEBUG
static BOOL iwconfigGetMacStats(void);
#endif
tWFHibernate WF_hibernate;

BOOL     test_flag;
DWORD    test_sec;
int      test_count;
char     test_buf[80];

/*******************************************************************************
  Function:    
    void do_iwconfig_cmd(void)
    
  Summary:
    Responds to the user invoking iwconfig command 
    
  Description:
    Responds to the user invoking iwconfig command

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void do_iwconfig_cmd(void)
{
    if (!WF_hibernate.state && !iwconfigSetCb() )
        return;

    // if user only typed in iwconfig with no other parameters
    if (ARGC == 1u)
    {
        if (!WF_hibernate.state)
            iwconfigDisplayStatus();
        else
            #if defined(STACK_USE_UART)
                WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", TRUE);
            #endif
        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "wakeup") == 0) )
    {
        if (!WF_hibernate.wakeup_notice)
        {
            WF_hibernate.wakeup_notice = TRUE;
        }          
        
        #if defined(STACK_USE_UART)
            WFConsolePrintRomStr("The Wi-Fi module is awake.", TRUE);    
        #endif
        
        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "hibernate") == 0) )
    {
        if (!WF_hibernate.state)
        {
            WF_hibernate.state = WF_HB_ENTER_SLEEP;
            WF_hibernate.wakeup_notice = FALSE;
            WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode.", TRUE);
        }
        else
            WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode.", TRUE);
        return;
    }
    
    if (WF_hibernate.state)
    {
        WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", TRUE);
        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "ssid") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetSsid())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "mode") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetMode())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "channel") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetChannel())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "power") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetPower())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "domain") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetDomain())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "rts") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetRTS())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "txrate") == 0) )
    {
        // txrate is NOT available. Will always return FALSE 
        if (!WF_hibernate.state && !iwconfigSetTxRate())
            return;
    }

    #if defined ( EZ_CONFIG_SCAN ) && !defined(__18CXX)
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "scan") == 0))
    {
        if (!WF_hibernate.state) 
        {
            WFInitScan();
            WFConsolePrintRomStr("Scanning...", TRUE);
            if (WFStartScan() == WF_SUCCESS)
            {
                WFConsolePrintRomStr("Scan completed.", TRUE);
            }
            else 
            {
                WFConsolePrintRomStr("Scan failed. Already in progress or not allowed", TRUE);
            }
        }
        else 
        {
            WFConsolePrintRomStr("In hibernate mode - scan is not allowed.", TRUE);    
        }
        return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "scanresults") == 0) )
    {
        if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
            WFConsolePrintRomStr("Scann in process...please wait or try again later", TRUE);
        else if (SCANCXT.numScanResults > 0) 
        {
            SCAN_SET_DISPLAY(SCANCXT.scanState);
            SCANCXT.displayIdx = 0;
            while (IS_SCAN_STATE_DISPLAY(SCANCXT.scanState))
            {
                WFDisplayScanMgr();
            }
        }
        else
            WFConsolePrintRomStr("No scan results to display.", TRUE);

        return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "starttest") == 0))
    {
        test_flag = TRUE;
        test_count = 0;
        return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "stoptest") == 0))
    {
        test_flag = FALSE;
        return;
    }
    
    #endif /* EZ_CONFIG_SCAN */

	else if( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "connect") == 0))
	{
		iwconfigSetConnect();
		return;
	}
#ifdef	STACK_USE_CERTIFICATE_DEBUG
	else if( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "macstats") == 0))
	{
		iwconfigGetMacStats();
		return;
	}
#endif
    else
    {
        WFConsolePrintRomStr("Unknown parameter", TRUE);
        return;
    }
}

/*******************************************************************************
  Function:      
      BOOL iwconfigSetCb(void)
       
  Summary:
	Set the iwconfigCb structure
    
  Description:
	Set the iwconfigCb structure

  Parameters:
      None.

  Returns:
      TRUE or FALSE
      
  Remarks:
       None.
 *****************************************************************************/
BOOL iwconfigSetCb(void)
{
    UINT8 cpId;

    if ( !iwconfigCbInitialized ) // first time call of iwconfigSetCb
    {
        memset(&iwconfigCb, 0, sizeof(iwconfigCb));
        iwconfigCbInitialized = TRUE;
    }

    #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_GetPowerSaveState(&iwconfigCb.powerSaveState);
    #endif
    if (iwconfigCb.powerSaveState == WF_PS_HIBERNATE)
    {
        WFConsolePrintRomStr("WF device hibernated", TRUE);
        return FALSE;
    }

    WF_CMGetConnectionState(&iwconfigCb.connState, &cpId);

    if ( iwconfigCb.cpId == WF_CURRENT_CPID_NONE )
    {
        if ( cpId == WF_CURRENT_CPID_NONE )
        {
            iwconfigCb.cpId = 1;     // console demo only supports 1 CPID; don't create a new one here
        }
        else if ( cpId == WF_CURRENT_CPID_LIST )
        {
            WFConsolePrintRomStr("Connection profile list not supported", TRUE);
            return FALSE;
        }
        else
        {
            iwconfigCb.cpId = cpId; // use the application-created profile
        }
    }
    else // WF_MIN_NUM_CPID <= iwconfigCb.cpId && iwconfigCb.cpId <= WF_MAX_NUM_CPID
    {
        if ( cpId == WF_CURRENT_CPID_NONE )
        {
            // continue to use iwconfigCb.cpId
        }
        else if ( cpId == WF_CURRENT_CPID_LIST )
        {
            WFConsolePrintRomStr("Conection profile list not supported", TRUE);

            WF_CPDelete(iwconfigCb.cpId);
            iwconfigCb.cpId = WF_CURRENT_CPID_NONE;

            return FALSE;
        }
        else if ( cpId != iwconfigCb.cpId )
        {
            WF_CPDelete(iwconfigCb.cpId);
            iwconfigCb.cpId = cpId; // use the application-created profile
        }
        else // cpId == iwconfigCb.cpId
        {
            // contine to use iwconfigCb.cpId
        }
    }

    if ((iwconfigCb.connState == WF_CSTATE_NOT_CONNECTED) || (iwconfigCb.connState == WF_CSTATE_CONNECTION_PERMANENTLY_LOST))
    {
        iwconfigCb.isIdle = TRUE;
    }    
    else
    {
        iwconfigCb.isIdle = FALSE;
    }    

    return TRUE;
}

#if defined(MRF24WG)
static void OutputMacAddress(void)
{
    UINT8 mac[6];
    int i;
    char buf[4];
    
    WF_GetMacAddress(mac);
    for (i = 0; i < 6; ++i)
    {
        sprintf(buf, "%02X ", mac[i]);
        putsUART(buf);
    }    
    putrsUART("\r\n");
}    
#endif

extern void WF_OutputConnectionContext(void);

/*******************************************************************************
  Function:      
	  void iwconfigDisplayStatus(void)
       
  Summary:
	Responds to the user invoking iwconfig with no parameters
    
  Description:
	Responds to the user invoking iwconfig with no parameters

  Parameters:
      None.

  Returns:
	None
      
  Remarks:
       None.
 *****************************************************************************/
static void iwconfigDisplayStatus(void)
{
    UINT8 *p;
    UINT8 tmp;
    UINT8 connectionState;
    UINT8 cpId;
    #if defined(MRF24WG)
    char buf[6];
    #endif

    union
    {
        struct
        {
            UINT8 List[WF_CHANNEL_LIST_LENGTH];
            UINT8 Num;
        } Channel;

        UINT8 Domain;

        struct
        {
            UINT8 String[WF_MAX_SSID_LENGTH+1];
            UINT8 Len;
        } Ssid;

        struct
        {
            UINT8 NetworkType;
        } Mode;

        struct
        {
            UINT16 Threshold;
        } Rts;
    } ws; // workspace

    // cpId
    {
        WFConsolePrintRomStr("\tcpid:     ", FALSE);
        WFConsolePrintInteger(iwconfigCb.cpId, 'd');
        WFConsolePrintRomStr("", TRUE);
    }

    // channel
    {
        WF_CAGetChannelList(ws.Channel.List, &ws.Channel.Num);
        WFConsolePrintRomStr("\tchannel:  ", FALSE);

        p = ws.Channel.List;
        tmp = ws.Channel.Num;

        while ( --tmp > 0u )
        {
            WFConsolePrintInteger(*p, 'd');
            WFConsolePrintRomStr(",", FALSE);
            p++;
        }

        WFConsolePrintInteger(*p, 'd');
        WFConsolePrintRomStr("", TRUE);
    }

#if defined(MRF24WG)
    // domain
    {
        WF_GetRegionalDomain(&ws.Domain);

        WFConsolePrintRomStr("\tdomain:   ", FALSE);

        if ( ws.Domain == WF_DOMAIN_FCC )
        {
            WFConsolePrintRomStr("fcc", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_ETSI )
        {
            WFConsolePrintRomStr("etsi", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_JAPAN )
        {
            WFConsolePrintRomStr("japan", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_OTHER )
        {
            WFConsolePrintRomStr("other", TRUE);
        }
        else
        {
            WFConsolePrintRomStr("unknown", TRUE);
        }
    }
#else
    // domain
    {
        WF_GetRegionalDomain(&ws.Domain);

        WFConsolePrintRomStr("\tdomain:   ", FALSE);

        if ( ws.Domain == WF_DOMAIN_FCC )
        {
            WFConsolePrintRomStr("fcc", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_IC )
        {
            WFConsolePrintRomStr("ic", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_ETSI )
        {
            WFConsolePrintRomStr("etsi", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_SPAIN )
        {
            WFConsolePrintRomStr("spain", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_FRANCE )
        {
            WFConsolePrintRomStr("france", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_JAPAN_A )
        {
            WFConsolePrintRomStr("japana", TRUE);
        }
        else if ( ws.Domain == WF_DOMAIN_JAPAN_B )
        {
            WFConsolePrintRomStr("japanb", TRUE);
        }
        else
        {
            WFConsolePrintRomStr("unknown", TRUE);
        }
    }

#endif    

    // rts
    {
        WF_GetRtsThreshold(&ws.Rts.Threshold);

        WFConsolePrintRomStr("\trts:      ", FALSE);
        WFConsolePrintInteger(ws.Rts.Threshold, 'd');
        WFConsolePrintRomStr("", TRUE);
    }

    // mode
    {

        WF_CMGetConnectionState(&connectionState, &cpId);
        WF_CPGetNetworkType(iwconfigCb.cpId, &ws.Mode.NetworkType);
        
        WFConsolePrintRomStr("\tmode:     ", FALSE);

        if (iwconfigCb.isIdle)
        {
            if (iwconfigCb.connState == WF_CSTATE_NOT_CONNECTED)
            {
                WFConsolePrintRomStr("idle", TRUE);
            }
            else if (iwconfigCb.connState == WF_CSTATE_CONNECTION_PERMANENTLY_LOST)
            {
                WFConsolePrintRomStr("idle (connection permanently lost)", TRUE);                
            }   
            else
            {
                WFConsolePrintRomStr("idle (?)", TRUE);                
            }               
        }
        else
        {
            WF_CPGetNetworkType(iwconfigCb.cpId, &ws.Mode.NetworkType);
            if (ws.Mode.NetworkType == WF_INFRASTRUCTURE)
            {
                if (iwconfigCb.connState == WF_CSTATE_CONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("managed (connection in progress)", TRUE);
                }
                else if (iwconfigCb.connState == WF_CSTATE_CONNECTED_INFRASTRUCTURE)
                {
                    WFConsolePrintRomStr("managed", TRUE);                    
                }    
                else if (iwconfigCb.connState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("managed (reconnection in progress)", TRUE);                    
                }          
                else 
                {
                    WFConsolePrintRomStr("managed (?)", TRUE);                                        
                }    
            }
            else if (ws.Mode.NetworkType == WF_ADHOC)
            {
                if (iwconfigCb.connState == WF_CSTATE_CONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("adhoc (connection in progress)", TRUE);
                }
                else if (iwconfigCb.connState == WF_CSTATE_CONNECTED_ADHOC)
                {
                    WFConsolePrintRomStr("adhoc", TRUE);                    
                }    
                else if (iwconfigCb.connState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("adhoc (reconnection in progress)", TRUE);                    
                }          
                else 
                {
                    WFConsolePrintRomStr("adhoc (?)", TRUE);                                        
                }    
            }
            else
            {
                WFConsolePrintRomStr("unknown", TRUE);
            }
        }
    }

    // ssid
    {
        WF_CPGetSsid(iwconfigCb.cpId, ws.Ssid.String, &ws.Ssid.Len);
        ws.Ssid.String[ws.Ssid.Len] = '\0';

        WFConsolePrintRomStr("\tssid:     ", FALSE);
        WFConsolePrintRamStr(ws.Ssid.String, TRUE);
    }

    // power
    {
        switch (iwconfigCb.powerSaveState)
        {
        case WF_PS_PS_POLL_DTIM_ENABLED:
            WFConsolePrintRomStr("\tpwrsave:  enabled", TRUE);
            WFConsolePrintRomStr("\tdtim rx:  enabled", TRUE);
            break;
        case WF_PS_PS_POLL_DTIM_DISABLED:
            WFConsolePrintRomStr("\tpwrsave:  enabled", TRUE);
            WFConsolePrintRomStr("\tdtim rx:  disabled", TRUE);
            break;
        case WF_PS_OFF:
            WFConsolePrintRomStr("\tpwrsave:  disabled", TRUE);
            break;
        default:
            WFConsolePrintRomStr("\tpwrsave:  unknown(", FALSE);
            WFConsolePrintInteger(iwconfigCb.powerSaveState, 'd');
            WFConsolePrintRomStr(")", TRUE);
            break;
        }
    }
    
    #if defined(MRF24WG)
    // context
    WF_OutputConnectionContext();
    
    // Network Type
    putrsUART("\tNetwork:  ");
    #if defined(EZ_CONFIG_STORE) && !defined(WF_CONSOLE_DEMO)   /* if EZConfig demo */

        if (AppConfig.networkType == WF_ADHOC) 
        {
            putrsUART("AdHoc\r\n");
        }
        else 
        {
            putrsUART("Infrastructure\r\n");
        }
    #else
        #if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
            putrsUART("AdHoc\r\n");
         #elif (MY_DEFAULT_NETWORK_TYPE == WF_P2P)
            putrsUART("P2P\r\n");
        #elif (MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE)
            #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON)
                putrsUART("Infrastructure (using WPS Push Button)\r\n");
            #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)
                putrsUART("Infrastructure (using WPS Pin)\r\n"); 
            #else
                putrsUART("Infrastructure\r\n");
            #endif
        #endif
    #endif /* EZ_CONFIG_STORE  */
    
    // Retry Count
    putrsUART("\tRetries   ");
    #if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
        sprintf(buf, "%d\r\n", ADHOC_RETRY_COUNT);
        putsUART(buf);
    #elif (MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE)
        #if (INFRASTRUCTURE_RETRY_COUNT == WF_RETRY_FOREVER)    
            sprintf(buf, "Retry Forever\r\n");
            putsUART(buf);
        #else
            sprintf(buf, "%d\r\n", INFRASTRUCTURE_RETRY_COUNT);
            putsUART(buf);
        #endif
    #endif    /* (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC) */

    // Security
    putrsUART("\tSecurity: ");  
    #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
        putrsUART("WF_SECURITY_OPEN");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40)
        putrsUART("WF_SECURITY_WEP_40");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104)
        putrsUART("WF_SECURITY_WEP_104");    
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_KEY)
        putrsUART("WF_SECURITY_WPA_WITH_KEY");    
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_WITH_PASS_PHRASE)
        putrsUART("WF_SECURITY_WPA_WITH_PASS_PHRASE");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_KEY)
        putrsUART("WF_SECURITY_WPA2_WITH_KEY");    
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA2_WITH_PASS_PHRASE)
        putrsUART("WF_SECURITY_WPA2_WITH_PASS_PHRASE");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY)    
        putrsUART("WF_SECURITY_WPA_AUTO_WITH_KEY");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)    
        putrsUART("WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE");    
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON)    
        putrsUART("WF_SECURITY_WPS_PUSH_BUTTON");    
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)    
        putrsUART("WF_SECURITY_WPS_PIN");    
    #else
        putrsUART("Unknown");
    #endif
    putrsUART("\r\n");
        
    // scan type
    putrsUART("\tScan:     ");
    #if (MY_DEFAULT_SCAN_TYPE == WF_ACTIVE_SCAN)
        putrsUART("Active Scan\r\n");
    #else
        putrsUART("Passive Scan\r\n");
    #endif
    
    // MAC address
    putrsUART("\tMAC:      ");
    OutputMacAddress();

    #endif  /* MRF24WG */  
}

static BOOL iwconfigSetSsid(void)
{
    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if (ARGC > 3u)
    {
        WFConsolePrintRomStr("SSID may not contain space for this demo", TRUE);
        return FALSE;
    }

    WF_CPSetSsid(iwconfigCb.cpId, (UINT8 *)ARGV[2], strlen((char*)ARGV[2]));

    return TRUE;
}

/*******************************************************************************
  Function:      
	  BOOL iwconfigSetMode(void)
       
  Summary:
	 Set the mode to idle, managed or adhoc. 
    
  Description:
	 Idle mode        - Force MRF24W module to disconnect from any currently connected network
        Managed mode - MRF24W module will connect to SSID in infrastructure mode. Ensure all network
                                  parameters are correct before this command is invoked.
        Adhoc mode     - MRF24W module will connect to SSID in adhoc mode. Ensure all network
                                  parameters are correct before this command is invoked.                         

  Parameters:
      Mode - idle / managed /adhoc

  Returns:
	TRUE or FALSE
      
  Remarks:
       None.
 *****************************************************************************/
static BOOL iwconfigSetMode(void)
{
    UINT8 networkType;

    WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "idle") == 0) )
    {
        if ( iwconfigCb.isIdle )
        {
            WFConsolePrintRomStr("Already in the idle mode", TRUE);
        }
        else
        {
            if (WF_CMDisconnect() != WF_SUCCESS)
            {
                #if defined(STACK_USE_UART)
                    putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
                #endif
            }
            WF_PsPollDisable();
        }
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "managed") == 0) )
    {
        if ( iwconfigCb.isIdle )
        {
            WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
            WF_CMConnect(iwconfigCb.cpId);
        }
        else
        {
            WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);
            if (networkType == WF_INFRASTRUCTURE)
            {
                WFConsolePrintRomStr("Already in the managed mode", TRUE);
            }
            else
            {
                if (WF_CMDisconnect() != WF_SUCCESS)
                {
                    #if defined(STACK_USE_UART)
                        putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
                    #endif
                }

                WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
                WF_CMConnect(iwconfigCb.cpId);
            }
        }
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "adhoc") == 0) )
    {
        if ( iwconfigCb.isIdle )
        {
            WF_CASetListRetryCount(ADHOC_RETRY_COUNT);
            WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
            WF_CPSetAdHocBehavior(iwconfigCb.cpId, WF_ADHOC_CONNECT_THEN_START);
            WF_CMConnect(iwconfigCb.cpId);
        }
        else
        {
            WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);
            if (networkType == WF_ADHOC)
            {
                WFConsolePrintRomStr("Already in the adhoc mode", TRUE);
            }
            else
            {
                if (WF_CMDisconnect() != WF_SUCCESS)
                {
                    #if defined(STACK_USE_UART)
                        putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
                    #endif
                }

                WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
                WF_CMConnect(iwconfigCb.cpId);
            }
        }
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", TRUE);
        return FALSE;
    }

    return TRUE;
}

static BOOL iwconfigSetChannel(void)
{
    UINT8 *p1, *p2;
    UINT8 *p_channelList;
    UINT8 index = 0;
    UINT16 temp;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if ( !iwconfigCb.isIdle )
    {
        WFConsolePrintRomStr("Channel can only be set in idle mode", TRUE);
        return FALSE;
    }

    p_channelList = (UINT8*) ARGV[2];
    p1 = p2 = p_channelList;

    if ( strlen( (char*) p_channelList) == 0u )
        return FALSE;

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "all") == 0) )
    {
        WF_CASetChannelList(p_channelList, 0); // reset to domain default channel list
        return TRUE;
    }

    do
    {
       if ( (p2 = (UINT8*) strchr( (const char *) p1, (int) ',')) != NULL )
       {
          *p2='\0';
          p2++;
       }

       if( !ConvertASCIIUnsignedDecimalToBinary((INT8 *)p1, &temp) )
          return  FALSE;

       p1 = p2;
       p_channelList[index] = (UINT8) temp;
       index++;

    } while (  p2 != NULL );

    WF_CASetChannelList(p_channelList, index);

    return TRUE;
}

/*******************************************************************************
  Function:      
	  BOOL iwconfigSetPower(void)
       
  Summary:
	 Enables or disables PS Poll mode.
    
  Description:
	 Enables or disables PS Poll mode.
	 reenable / all - Enables all power saving features (PS_POLL) of the MRF24W. MRF24W
	                        will wake up to check for all types of traffic (unicast, multicast, and broadcast)
        disable          - Disables any power savings features. 
        unicast          - MRF24W will be in its deepest sleep state, only waking up at periodic intervals 
                               to check for unicast data. MRF24W will not wake up on the DTIM period for 
                               broadcast or multicast traffic.

  Parameters:
       reenable / disable / unicast /all

  Returns:
	TRUE or FALSE
      
  Remarks:
       WF_USE_POWER_SAVE_FUNCTIONS must be defined to use PS Poll mode.
 *****************************************************************************/
static BOOL iwconfigSetPower(void)
{
    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "reenable") == 0) )
    {    // reenable power saving
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_PsPollEnable(TRUE);
        #endif
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "disable") == 0) )
    {    // disable power saving
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)        
        WF_PsPollDisable();
        #endif
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "unicast") == 0) )
    {   // enable power saving but don't poll for DTIM
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_PsPollEnable(FALSE);
        #endif
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "all") == 0) )
    {    // enable power saving and poll for DTIM
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_PsPollEnable(TRUE);
        #endif
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", TRUE);
        return FALSE;
    }

    return TRUE;
}

/*******************************************************************************
  Function:      
	  BOOL iwconfigSetDomain(void)
       
  Summary:
	 Set the domain. 
    
  Description:
	 Set the MRF24W Regional Domain.
	 For MRF24WG with RF module FW version 0x3107 and future releases, this function 
	 is NOT supported due to changes in FCC requirements, which does not allow programming 
	 of the regional domain. 

  Parameters:
       Domain - fcc / etsi /japan / other

  Returns:
	TRUE or FALSE
      
  Remarks:
       None.
 *****************************************************************************/
static BOOL iwconfigSetDomain(void)
{
    UINT8 domain;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if ( !iwconfigCb.isIdle )
    {
        WFConsolePrintRomStr("Domain can only be set in idle mode", TRUE);
        return FALSE;
    }

#if defined(MRF24WG)
    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "fcc") == 0) )
    {
        domain = WF_DOMAIN_FCC;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "etsi") == 0) )
    {
        domain = WF_DOMAIN_ETSI;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japan") == 0) )
    {
        domain = WF_DOMAIN_JAPAN;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "other") == 0) )
    {
        domain = WF_DOMAIN_OTHER;
    }
    else
    {
        WFConsolePrintRomStr("Unknown domain", TRUE);
        return FALSE;
    }
#else
    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "fcc") == 0) )
    {
        domain = WF_DOMAIN_FCC;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "ic") == 0) )
    {
        domain = WF_DOMAIN_IC;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "etsi") == 0) )
    {
        domain = WF_DOMAIN_ETSI;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "spain") == 0) )
    {
        domain = WF_DOMAIN_SPAIN;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "france") == 0) )
    {
        domain = WF_DOMAIN_FRANCE;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japana") == 0) )
    {
        domain = WF_DOMAIN_JAPAN_A;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japanb") == 0) )
    {
        domain = WF_DOMAIN_JAPAN_B;
    }
    else
    {
        WFConsolePrintRomStr("Unknown domain", TRUE);
        return FALSE;
    }
#endif    

    WF_SetRegionalDomain(domain);
    WF_CASetChannelList(NULL, 0); // reset to domain default channel list

    return TRUE;
}

static BOOL iwconfigSetRTS(void)
{
    UINT16 rtsThreshold;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if( !ConvertASCIIUnsignedDecimalToBinary(ARGV[2], &rtsThreshold) )
        return  FALSE;

    WF_SetRtsThreshold(rtsThreshold);

    return TRUE;
}

static BOOL iwconfigSetTxRate(void)
{
    return FALSE;
}
static UINT8 SetMode_idle(void)
{
	UINT8 networkType;

	WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);
	
	if (FALSE == iwconfigCb.isIdle )
	{
		if (WF_CMDisconnect() != WF_SUCCESS)
		{
			putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
		}
		WF_PsPollDisable();
#ifdef STACK_USE_CERTIFICATE_DEBUG		
		DelayMs(100);
#endif
	}
	return networkType;
}
static void SetMode_NotIdle(UINT8 networkType)
{
#ifdef STACK_USE_CERTIFICATE_DEBUG
	DelayMs(100);
#endif
	if(WF_INFRASTRUCTURE == networkType)
	{
		WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
		WF_CMConnect(iwconfigCb.cpId);
	}
	else if(WF_ADHOC == networkType)
	{
		WF_CASetListRetryCount(ADHOC_RETRY_COUNT);
		WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
		WF_CPSetAdHocBehavior(iwconfigCb.cpId, WF_ADHOC_CONNECT_THEN_START);
		WF_CMConnect(iwconfigCb.cpId);
	}
	else
	{
		//To be done
	}

}

static BOOL iwconfigSetConnect(void)
{ // IWCONFIG CONNECT [ssid] [channel] [power-mode]   //[security-mode] [WEP-key/passphrase] [retry-attempt]
	UINT8 networkType;
	if(ARGC < 3u)
	{
		putsUART("Wrong command, correct command is:IWCONFIG CONNECT [ssid] [bssid] [channel] [power-mode]\r\n");
		return FALSE;
	}
	networkType = SetMode_idle();
	if(ARGC >= 3u) // ssid
	{
		WF_CPSetSsid(iwconfigCb.cpId, (UINT8 *)ARGV[2], strlen((char*)ARGV[2]));
	}
	if(ARGC >= 4u) //channel
	{
		int int_channel;
		sscanf((const char *)ARGV[3], (const char *)"%d",&int_channel);
		if((int_channel>=1)&&(int_channel<=14))
		{
			WF_CASetChannelList((UINT8 *)&int_channel, 1);
		}
		else
		{
			WFConsolePrintRomStr("channel err (1~14): Unknown parameter", TRUE);
	        return FALSE;
		}
	}if(ARGC >= 5u) //channel
	{
		int int_channel;
		UINT8 channel;
		sscanf((const char *)ARGV[4], (const char *)"%d",&int_channel);
		if((int_channel>=1)&&(int_channel<=14))
		{
			channel = int_channel;
				//{char buf_t[20];sprintf(buf_t,"channel=%d\r\n",int_channel);putsUART(buf_t);}
			WF_CASetChannelList(&channel, 1);
			DelayMs(100);
		}
		else
		{
			WFConsolePrintRomStr("channel err (1~14): Unknown parameter", TRUE);
	        return FALSE;
		}
	}
	if(ARGC >= 6u) // power-mode
	{
		if (strcmppgm2ram((char*)ARGV[5], "reenable") == 0) 
	    {    // reenable power saving
	        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
	        WF_PsPollEnable(TRUE);
	        #endif
	    }
	    else if  (strcmppgm2ram((char*)ARGV[5], "disable") == 0) 
	    {    // disable power saving
	        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)        
	        WF_PsPollDisable();
	        #endif
	    }
	    else if  (strcmppgm2ram((char*)ARGV[5], "unicast") == 0) 
	    {   // enable power saving but don't poll for DTIM
	        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
	        WF_PsPollEnable(FALSE);
	        #endif
	    }
	    else if  (strcmppgm2ram((char*)ARGV[5], "all") == 0) 
	    {    // enable power saving and poll for DTIM
	        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
	        WF_PsPollEnable(TRUE);
	        #endif
	    }
	    else
	    {
	        WFConsolePrintRomStr("Unknown parameter", TRUE);
	        return FALSE;
	    }
	}
	if(ARGC >= 7u) // [security-mode] 
	{
	
	}
	SetMode_NotIdle(networkType);
	return TRUE;
}
#if defined(STACK_USE_CERTIFICATE_DEBUG)
BOOL iwconfigGetMacStats(void)
{
	tWFMacStats my_WFMacStats;
	WF_GetMacStats(&my_WFMacStats);

	putsUART("MibRxMICFailureCounts = ");
	{char buf_t[16]; sprintf(buf_t,"%u",(unsigned int)(my_WFMacStats.MibRxMICFailureCtr));putsUART(buf_t);}
	//putsUART("\r\n");
	return TRUE;
}
#endif
#endif  /* WF_CONSOLE_IFCFGUTIL */
