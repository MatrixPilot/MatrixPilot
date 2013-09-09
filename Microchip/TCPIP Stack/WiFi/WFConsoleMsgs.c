/******************************************************************************

 MRF24W Driver Console Messages
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFConsoleMsgs.c
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

#include <string.h>
#include <ctype.h>

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/WFConsole.h"


//---------------------
// token parsing states
//---------------------
enum
{
    kWFWaitingForStartOfToken,
    kWFWaitingForEndOfToken
};

//----------------
// Command strings
//----------------
ROM INT8 helpCmd[]      = "help";
ROM INT8 helpHelp[]     = "Lists all commands";

ROM INT8 getwfverCmd[]  = "getwfver";
ROM INT8 getwfverHelp[] = "Gets WiFi device version";

ROM INT8 resetCmd[]     = "reset";
ROM INT8 resetHelp[]    = "Reset host MCU";

ROM INT8 clsCmd[]       = "cls";
ROM INT8 clsHelp[]      = "Clears screen";

#if defined(MRF24WG)
ROM INT8 wpsPinCmd[]    = "wpspin";
ROM INT8 wpsPinHelp[]   = "8-digit WPS Pin. Eg wpspin 12390212. Next connection will use the WPS-Pin method";

ROM INT8 wpsPushButtonCmd[]  = "wpspbc";
ROM INT8 wpsPushButtonHelp[] = "Next connection will use the WPS-Push-Button method";

ROM INT8 wpsCredCmd[]        = "wpscred";
ROM INT8 wpsCredHelp[]       = "Dump WPS credentials";
#endif /* MRF24WG */

#if defined(WF_CONSOLE_IFCFGUTIL)
ROM INT8 ifConfigCmd[]  = "ifconfig";
ROM INT8 iwConfigCmd[]  = "iwconfig";
ROM INT8 iwPrivCmd[]    = "iwpriv";
#endif // WF_CONSOLE_IFCFGUTIL

ROM INT8 seeDocHelp[]      = "see documentation";
ROM INT8 pingCmd[]        = "ping"; 
ROM INT8 pingHelp[]       = "ping 192.168.1.1";

#if defined(STACK_USE_CERTIFICATE_DEBUG)
ROM INT8 KillPingCmd[]        = "killping";
ROM INT8 KillPingHelp[]       = "stop the running ping session";
#endif
//----------------------
// Console Command Table
//-----------------------
const tWFCmd g_consoleCmd[] = {

    {helpCmd,                      // cmd name
     helpHelp,                     // cmd description
     2},                           // max tokens

    {getwfverCmd,                  // [1]
     getwfverHelp,
     1},

    {resetCmd,                     // [2]
     resetHelp,
     1},

    {clsCmd,                       // [3]
     clsHelp,
     1},

#if defined(MRF24WG)
    {wpsPinCmd,
     wpsPinHelp,
     2},
     
     {wpsPushButtonCmd,
      wpsPushButtonHelp,
      1},
      
     {wpsCredCmd,
      wpsCredHelp,
      1},
#endif /* MRF24WG */     

#if defined(WF_CONSOLE_IFCFGUTIL)
    {ifConfigCmd,                  // [4]
     seeDocHelp,
     12},

    {iwConfigCmd,                  // [5]
     seeDocHelp,
     12},

    {iwPrivCmd,                    // [6]
     seeDocHelp,
     12},
#endif // WF_CONSOLE_IFCFGUTIL
	
	{pingCmd,
	pingHelp,
	1}
#if defined(STACK_USE_CERTIFICATE_DEBUG)	
	,
	{KillPingCmd,
	KillPingHelp,
 	1}
#endif
};

const UINT8 g_numCmds   = sizeof(g_consoleCmd) / sizeof(tWFCmd);


/*****************************************************************************
 * FUNCTION: ConvertASCIIHexToBinary
 *
 * RETURNS: TRUE if conversion successful, else FALSE
 *
 * PARAMS:  p_ascii   -- ascii string to be converted
 *          p_binary  -- binary value if conversion successful
 *
 * NOTES:   Converts an input ascii hex string to binary value (up to 32-bit value)
 *****************************************************************************/
BOOL ConvertASCIIHexToBinary(INT8 *p_ascii, UINT16 *p_binary)
{
    INT8  i;
    UINT32 multiplier = 1;

    *p_binary = 0;

    // not allowed to have a string of more than 4 nibbles
    if (strlen((char*)p_ascii) > 8u)
    {
        return FALSE;
    }

    // first, ensure all characters are a hex digit
    for (i = (UINT8)strlen((char *)p_ascii) - 1; i >= 0 ; --i)
    {
        if (!isxdigit(p_ascii[i]))
        {
            return FALSE;
        }
        *p_binary += multiplier * HexToBin(p_ascii[i]);
        multiplier *= 16;
    }

    return TRUE;
}

/*****************************************************************************
 * FUNCTION: HexToBin
 *
 * RETURNS: binary value associated with ASCII hex input value
 *
 * PARAMS:  hexChar -- ASCII hex character
 *
 * NOTES:   Converts an input ascii hex character to its binary value.  Function
 *          does not error check; it assumes only hex characters are passed in.
 *****************************************************************************/
UINT8 HexToBin(UINT8 hexChar)
{
    if ((hexChar >= 'a') && (hexChar <= 'f'))
    {
        return (0x0a + (hexChar - 'a'));
    }
    else if ((hexChar >= 'A') && (hexChar <= 'F'))
    {
        return (0x0a + (hexChar - 'A'));
    }
    else //  ((hexChar >= '0') && (hexChar <= '9'))
    {
        return (0x00 + (hexChar - '0'));
    }

}

#if defined ( WF_CONSOLE )
/*****************************************************************************
 * FUNCTION: TokenizeCmdLine
 *
 * RETURNS: None
 *
 * PARAMS:  p_line -- pointer to the null terminated command line
 *
 * NOTES: Converts the input string into tokens separated by '\0'.
  *****************************************************************************/
void TokenizeCmdLine(INT8 *p_line)
{
    UINT8 state = kWFWaitingForStartOfToken;
    UINT8 index = 0;

    ARGC = 0;

    //---------------------------
    // while not at end of string
    //---------------------------
    while (p_line[index] != (INT8)'\0')
    {

        //----------------------------------------
        if (state == (UINT8)kWFWaitingForStartOfToken)
        //----------------------------------------
        {
            // if hit non whitespace
            if (!isspace((int)p_line[index]))
            {
               // argument string starts here
               ARGV[ARGC++] = (INT8 *)(&(p_line[index]));
               if (ARGC >= (UINT8)kWFMaxTokensPerCmd)
               {
                   return;  // truncate because too many tokens
               }
               state = kWFWaitingForEndOfToken;
            }
            ++index;

        }
        //----------------------------------------
        else if (state == (UINT8)kWFWaitingForEndOfToken)
        //----------------------------------------
        {
            // if white space, then end of token
            if (isspace((int)p_line[index]))
            {
                // string terminate the token
                p_line[index] = '\0';
                state = kWFWaitingForStartOfToken;
            }
            ++index;
        }
    }
}


/*****************************************************************************
 * FUNCTION: GetCmdId
 *
 * RETURNS: None
 *
 * PARAMS:  void
 *
 * NOTES: Determines index of cmd in CMD struct
  *****************************************************************************/
UINT8 GetCmdId(void)
{
    UINT8 i;
    const tWFCmd  *p_msgList;
    UINT16  msgCount;

    p_msgList = g_consoleCmd;
    msgCount  = g_numCmds;

    for (i = 0; i < msgCount; ++i)
    {
        if ( strcmppgm2ram( (FAR char *)ARGV[0], (FAR ROM char *) p_msgList[i].p_cmdName) == 0)
        {
            return i;
        }
    }

    return INVALID_CMD;
}


/*****************************************************************************
 * FUNCTION: ConvertASCIIUnsignedDecimalToBinary
 *
 * RETURNS: TRUE if conversion successful, else FALSE
 *
 * PARAMS:  p_ascii   -- ascii string to be converted
 *          p_binary  -- binary value if conversion successful
 *
 * NOTES:   Converts an input ascii decimal string to binary value
 *****************************************************************************/
BOOL ConvertASCIIUnsignedDecimalToBinary(INT8 *p_ascii, UINT16 *p_binary)
{
    INT8  i;
    UINT32 multiplier = 1;
    INT8 len;

    *p_binary = 0;
    len = (INT8)strlen((char *)p_ascii);

    // should not be any numbers greater than 6 digits
    if ((len > 5) || (len == 0))
    {
        return FALSE;
    }

    // first, ensure all characters are a decimal digit
    for (i = len - 1; i >= 0 ; --i)
    {
        if (!isdigit(p_ascii[i]))
        {
            return FALSE;
        }
        *p_binary += multiplier * (p_ascii[i] - '0');
        multiplier *= 10;
    }

    return TRUE;
}

/*****************************************************************************
 * FUNCTION: ConvertASCIISignedDecimalToBinary
 *
 * RETURNS: TRUE if conversion successful, else FALSE
 *
 * PARAMS:  p_ascii   -- ascii string to be converted
 *          p_binary  -- binary value if conversion successful
 *
 * NOTES:   Converts an input ascii signed decimal string to binary value
 *****************************************************************************/
BOOL ConvertASCIISignedDecimalToBinary(INT8 *p_ascii, INT16 *p_binary)
{
    INT8   i;
    UINT32  multiplier = 1;
    BOOL negFlag = FALSE;
    INT8   endIndex = 0;
    INT8  len;

    *p_binary = 0;
    len = (INT8)strlen((char *)p_ascii);

    // should not be any numbers greater than 5 digits (with -)
    if (len > 6)
    {
        return FALSE;
    }

    if (p_ascii[0] == (INT8)'-')
    {
        negFlag = TRUE;
        endIndex = 1;
    }


    // first, ensure all characters are a decimal digit

    for (i = len - 1; i >= endIndex ; --i)
    {
        if (!isdigit(p_ascii[i]))
        {
            return FALSE;
        }
        *p_binary += multiplier * (p_ascii[i] - '0');
        multiplier *= 10;
    }

    if (negFlag == TRUE)
    {
        *p_binary *= -1;
    }

    return TRUE;
}

BOOL ExtractandValidateU16Range(INT8 *p_string, UINT16 *pValue, UINT16 minValue, UINT16 maxValue)
{
    /* extract next parameter as an unsigned short integer */
    if (!ConvertASCIIUnsignedDecimalToBinary(p_string, pValue))
    {
        /* WFConsolePrintf("   Unable to parse paramter value"); */
        return FALSE;
    }

    if ((*pValue < minValue) || (*pValue > maxValue))
    {
        /* WFConsolePrintf("   parameter value out of range"); */
        return FALSE;
    }

    return TRUE;
}

#endif /* WF_CONSOLE */






