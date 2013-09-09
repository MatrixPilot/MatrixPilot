/******************************************************************************

 MRF24W10C Driver iwpriv
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W10C WiFi controller
  -Reference: MRF24W10C Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFConsoleIwpriv.c
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

#include "TCPIP Stack/WFConsoleMsgs.h"
#include "TCPIP Stack/WFConsoleMsgHandler.h"
#include "TCPIP Stack/WFConsoleIwconfig.h" // for using iwconfigCb
#include "TCPIP Stack/WFConsoleIwpriv.h"

//============================================================================
//                                  Constants
//============================================================================

#define IWPRIV_WEB_KEY_NUM          (4u)
#define IWPRIV_WEB_LONG_KEY_LEN     (13u) // in byte
#define IWPRIV_WEB_SHORT_KEY_LEN    (5u)  // in byte

//============================================================================
//                                  Globals
//============================================================================

static BOOL iwprivCbInitialized = FALSE;
static struct
{
    // NOTE: cpId, ssid, and wepDefaultKeyId
    // are refreshed at each iwpriv

    UINT8 cpId; // conn. profile ID
    UINT8 ssid[WF_MAX_SSID_LENGTH+1];
    UINT8 wepDefaultKeyId;

    // NOTE: securityType, securityKey and securityKeyLength are
    // not refreshed at each iwpriv

    UINT8 securityType;
    UINT8 securityKey[WF_MAX_SECURITY_KEY_LENGTH+1];
    UINT8 securityKeyLength;
} iwprivCb;

extern tWFHibernate WF_hibernate;
//============================================================================
//                                  Local Function Prototypes
//============================================================================

/*****************************************************************************
 * FUNCTION: iwprivSetCb
 *
 * RETURNS: TRUE or FALSE
 *
 * PARAMS:  None
 *
 * NOTES:   Set the iwprivCb structure
 *****************************************************************************/
static BOOL iwprivSetCb(void)
{
    tWFCPElements cprof;
    BOOL cpIdChanged = FALSE;

    if ( !iwprivCbInitialized ) // first time call of iwprivSetCb
    {
        memset(&iwprivCb, 0, sizeof(iwprivCb));
        iwprivCbInitialized = TRUE;
    }

    if (!WF_hibernate.state && !iwconfigSetCb() ) // first set iwconfigCb
        return FALSE;

    if ( iwprivCb.cpId != iwconfigCb.cpId)
    {
        iwprivCb.cpId = iwconfigCb.cpId;
        cpIdChanged = TRUE;
    }    
        
    WF_CPGetElements(iwprivCb.cpId, &cprof);
        
    // set refreshable part of iwprivCb
    {
        memcpy((void*)iwprivCb.ssid, (const void*)cprof.ssid, cprof.ssidLength);
        iwprivCb.ssid[cprof.ssidLength] = '\0';

        iwprivCb.wepDefaultKeyId = cprof.wepDefaultKeyId;
    }
    
    // set non-refreshable part of iwprivCb only when cpId has changed
    if (cpIdChanged)
    {
        iwprivCb.securityType = cprof.securityType;
        if (iwprivCb.securityType == WF_SECURITY_WPA_WITH_KEY || iwprivCb.securityType == WF_SECURITY_WPA2_WITH_KEY)
        {
            iwprivCb.securityType = WF_SECURITY_WPA_AUTO_WITH_KEY;
        }
        else if (iwprivCb.securityType == WF_SECURITY_WPA_WITH_PASS_PHRASE || iwprivCb.securityType == WF_SECURITY_WPA2_WITH_PASS_PHRASE)
        {
            iwprivCb.securityType = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
        }
                      
        iwprivCb.securityKeyLength = 0;
    }       

    return TRUE;
}

static void iwprivDisplayStatus(void)
{
    UINT8 i, j;
    UINT8* p;

    // security type
    {
        WFConsolePrintRomStr("Encryption: ", FALSE);

        switch (iwprivCb.securityType)
        {
        case WF_SECURITY_OPEN:
            WFConsolePrintRomStr("none", TRUE);
            break;
        case WF_SECURITY_WEP_40:
        case WF_SECURITY_WEP_104:
            WFConsolePrintRomStr("wep", TRUE);
            break;
        case WF_SECURITY_WPA_WITH_KEY:
        case WF_SECURITY_WPA2_WITH_KEY:
        case WF_SECURITY_WPA_AUTO_WITH_KEY:
            WFConsolePrintRomStr("wpa-psk", TRUE);
            break;
        case WF_SECURITY_WPA_WITH_PASS_PHRASE:
        case WF_SECURITY_WPA2_WITH_PASS_PHRASE:
        case WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
            WFConsolePrintRomStr("wpa-phrase", TRUE);
            break;
        default:
            WFConsolePrintRomStr("unknown", TRUE);
            return;
        }
    }

    if ( iwprivCb.securityType == WF_SECURITY_WEP_40 || iwprivCb.securityType == WF_SECURITY_WEP_104 )
    {
        UINT8 webKeyLen;

        if ( iwprivCb.securityKeyLength == (IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_LONG_KEY_LEN) )
        {
            webKeyLen = IWPRIV_WEB_LONG_KEY_LEN;
        }
        else if ( iwprivCb.securityKeyLength == (IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_SHORT_KEY_LEN) )
        {
            webKeyLen = IWPRIV_WEB_SHORT_KEY_LEN;
        }
        else
        {
            WFConsolePrintRomStr("  Wep key: not yet set or unknown", TRUE);
            return;
        }

        p = iwprivCb.securityKey;
        for( j=0; j < IWPRIV_WEB_KEY_NUM ; j++ )
        {
            if ( j == iwprivCb.wepDefaultKeyId )
                WFConsolePrintRomStr(" *", FALSE);
            else
                WFConsolePrintRomStr("  ", FALSE);

            WFConsolePrintRomStr("Wep key[", FALSE);
            WFConsolePrintInteger(j+1, FALSE);
            WFConsolePrintRomStr("]:  0x", FALSE);

            for ( i=0; i < webKeyLen ; i++ )
            {
                sprintf( (char *) g_ConsoleContext.txBuf,
                    "%.2x", *p++);
                WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf, FALSE);
            }

            WFConsolePrintRomStr("", TRUE);
        }
    }
    else if ( iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_KEY )
    {
        if ( iwprivCb.securityKeyLength != WF_WPA_KEY_LENGTH )
        {
            WFConsolePrintRomStr("  PSK: not yet set or unknown", TRUE);
            return;
        }

        putrsUART("  PSK: \"");

        p = iwprivCb.securityKey;
        for( j=0; j < WF_WPA_KEY_LENGTH ; j++ )
        {
            sprintf( (char *) g_ConsoleContext.txBuf, "%.2x", *p++);
            WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf , FALSE );
        }

        WFConsolePrintRomStr("", TRUE);
    }
    else if ( iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE )
    {
        if ( iwprivCb.securityKeyLength == 0 )
        {
            WFConsolePrintRomStr("  Phrase: not yet set or unknown", TRUE);
            return;
        }

        WFConsolePrintRomStr("  Phrase: \"", FALSE);

        p = iwprivCb.securityKey;
        for( j=0; j < iwprivCb.securityKeyLength ; j++ )
        {
            sprintf( (char *) g_ConsoleContext.txBuf, "%c", *p++);
            WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf, FALSE );
        }

        WFConsolePrintRomStr("\"", TRUE);

        WFConsolePrintRomStr("  SSID: ", FALSE);
        WFConsolePrintRamStr(iwprivCb.ssid, TRUE);
    }
}

static BOOL iwprivSetEnc(void)
{
    UINT8 securityType;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "none") == 0) )
    {
        securityType = WF_SECURITY_OPEN;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "wep") == 0) )
    {
        securityType = WF_SECURITY_WEP_40; // by default
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "wpa-psk") == 0) )
    {
        securityType = WF_SECURITY_WPA_AUTO_WITH_KEY;

    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "wpa-phrase") == 0) )
    {
        securityType = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", TRUE);
        return FALSE;
    }

    if ( iwprivCb.securityType != securityType ) // security type changed
    {    // reset the security context
        memset(iwprivCb.securityKey, 0, sizeof(iwprivCb.securityKey));
        iwprivCb.securityKeyLength = 0;
    }
    
    iwprivCb.securityType = securityType; // save the security type
    
    if (iwprivCb.securityType == WF_SECURITY_OPEN)
    {
        WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, 0, NULL, 0);
    }       

    return TRUE;
}

static BOOL iwprivSetKey(void)
{
    UINT8 webKey;

    if (iwprivCb.securityType != WF_SECURITY_WEP_40 && iwprivCb.securityType != WF_SECURITY_WEP_104)
    {
        WFConsolePrintRomStr("WEP encryption mode is not selected", TRUE);
        return FALSE;
    }

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "[1]") == 0) )
    {
        webKey = 0u;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "[2]") == 0) )
    {
        webKey = 1u;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "[3]") == 0) )
    {
        webKey = 2u;

    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "[4]") == 0) )
    {
        webKey = 3u;
    }
    else
    {
        WFConsolePrintRomStr("Invalid WEP key index", TRUE);
        return FALSE;
    }

    if (4u <= ARGC)
    {
        if ( convertAsciiToHexInPlace(ARGV[3], IWPRIV_WEB_LONG_KEY_LEN) ) // for long web key
        {
            iwprivCb.securityType = WF_SECURITY_WEP_104;
            
            memcpy((void*)(iwprivCb.securityKey + webKey * IWPRIV_WEB_LONG_KEY_LEN), (const void*)ARGV[3], IWPRIV_WEB_LONG_KEY_LEN);
            iwprivCb.securityKeyLength = IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_LONG_KEY_LEN;
            
            WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, webKey,
                iwprivCb.securityKey + webKey * IWPRIV_WEB_LONG_KEY_LEN, IWPRIV_WEB_LONG_KEY_LEN);
        }
        else if ( convertAsciiToHexInPlace(ARGV[3], IWPRIV_WEB_SHORT_KEY_LEN) ) // for short web key
        {
            iwprivCb.securityType = WF_SECURITY_WEP_40;
            
            memcpy((void*)(iwprivCb.securityKey + webKey * IWPRIV_WEB_SHORT_KEY_LEN), (const void*)ARGV[3], IWPRIV_WEB_SHORT_KEY_LEN);
            iwprivCb.securityKeyLength = IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_SHORT_KEY_LEN;
            
            WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, webKey,
                iwprivCb.securityKey + webKey * IWPRIV_WEB_SHORT_KEY_LEN, IWPRIV_WEB_SHORT_KEY_LEN);
        }
        else
        {
            WFConsolePrintRomStr("64/128bit WEP key format not valid", TRUE);
            return FALSE;
        }
    }
    else // ARGC == 3u
    {
        WF_CPSetDefaultWepKeyIndex(iwprivCb.cpId, webKey);
    }

    return TRUE;
}

static BOOL iwprivSetPsk(void)
{
    if ( iwprivCb.securityType != WF_SECURITY_WPA_AUTO_WITH_KEY )
    {
        WFConsolePrintRomStr("WPA-PSK encryption mode is not selected", TRUE);
        return FALSE;
    }

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    if ( convertAsciiToHexInPlace(ARGV[2], WF_WPA_KEY_LENGTH) )
    {
        memcpy((void*)iwprivCb.securityKey, (const void*)ARGV[2], WF_WPA_KEY_LENGTH);
        iwprivCb.securityKeyLength = WF_WPA_KEY_LENGTH;
    }
    else
    {
        WFConsolePrintRomStr("WPA PSK must be exactly 32 bytes", TRUE);
        return FALSE;
    }

    WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, 0, iwprivCb.securityKey, iwprivCb.securityKeyLength);

    return TRUE;
}

static BOOL iwprivSetPhrase(void)
{
    UINT8 j;
    UINT8 securityType;
    UINT8* phraseStart;
    UINT8* phraseEnd;
    UINT8 phraseLen;

    if ( iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_KEY || iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE )
    {
        securityType = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
    }
    else
    {
        WFConsolePrintRomStr("WPA-PSK or WPA-PHRASE encryption mode is not selected", TRUE);
        return FALSE;
    }

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", TRUE);
        return FALSE;
    }

    phraseStart = (UINT8*) ARGV[2];
    if (*phraseStart == '\"') // cancatenate remaining args into one string
    {
        for (j = 2; j < (ARGC-1); j++)
        {
            UINT8 argLen = strlen((char*)ARGV[j]);
            ARGV[j][argLen] = ' '; // replace '\0' with ' '
        }

        // searching for an ending quote
        phraseEnd = phraseStart + strlen((char *)phraseStart) - 1;
        while (*phraseEnd != '\"')
            phraseEnd--;

        // remove the double quotes
        phraseStart++;
        phraseEnd--;
    }
    else // a single word
    {
        phraseEnd = phraseStart + strlen((char *)phraseStart) - 1;
    }

    phraseLen = phraseEnd - phraseStart + 1;
    if (phraseLen < WF_MIN_WPA_PASS_PHRASE_LENGTH || WF_MAX_WPA_PASS_PHRASE_LENGTH < phraseLen)
    {
        WFConsolePrintRomStr("Phrase string must be at least 8 chars and no greater than 64", TRUE);
        return FALSE;
    }
    
    iwprivCb.securityType = securityType;

    memcpy((void*)iwprivCb.securityKey, (const void*)phraseStart, phraseLen);
    iwprivCb.securityKey[phraseLen] = '\0'; // just for easy printing on the console
    iwprivCb.securityKeyLength = phraseLen;

    WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, 0,
        iwprivCb.securityKey, iwprivCb.securityKeyLength);

    return TRUE;
}

/*****************************************************************************
* FUNCTION: do_iwpriv_cmd
*
* RETURNS: None
*
* PARAMS:    None
*
* NOTES:   Responds to the user invoking ifconfig
*****************************************************************************/
void do_iwpriv_cmd(void)
{
    if (WF_hibernate.state)
    {
        WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", TRUE);
        return;
    }

    if ( !iwprivSetCb() )
            return;

    // if user only typed in iwpriv with no other parameters
    if (ARGC == 1u)
    {
        iwprivDisplayStatus();
        return;
    }

    if ( !iwconfigCb.isIdle )
    {
        WFConsolePrintRomStr("Security context modification can be only done in the idle state", TRUE);
        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "enc") == 0) )
    {
        if ( !iwprivSetEnc() )
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "key") == 0) )
    {
        if ( !iwprivSetKey() )
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "psk") == 0) )
    {
        if ( !iwprivSetPsk() )
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "phrase") == 0) )
    {
        if ( !iwprivSetPhrase() )
            return;
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", TRUE);
        return;
    }
}

#endif /* WF_CONSOLE_IFCFGUTIL */


