/******************************************************************************

 MRF24W Driver Console Messages
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFConsoleMsgs.h
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

#ifndef __WFCONSOLEMSGS_H
#define __WFCONSOLEMSGS_H

#ifdef __cplusplus
 extern "C" {
#endif

//----------------------------------------------------------------------------
//                                 Defines
//----------------------------------------------------------------------------

// !!! These defines MUST match the g_consoleCmd structure  !!!
enum validConsoleCmds
{
    HELP_MSG = 0,                   // only used by humans
    GET_WF_VERSION_MSG,
    RESET_HOST,
    CLEAR_SCREEN_MSG,
#if defined(MRF24WG)
    WPS_PIN_MSG,
    WPS_PUSHBUTTON_MSG,
    WPS_GET_CREDENTIALS_MSG,
#endif /* MRF24WG */

    IFCONFIG_MSG,
    IWCONFIG_MSG,
    IWPRIV_MSG,
	PING_MSG,
#if defined(STACK_USE_CERTIFICATE_DEBUG)
	KILLPING_MSG,
#endif
    INVALID_CMD = 0xFF
};


#define kWFMaxTokensPerCmd  (16)  /* max tokens, including cmd and parameters */
#define kConsoleMaxMsgSize  (80)
#define kConsoleCmdMaxLen   (16)  /* max string length of console commands (w/o arguments) */
#define kConsoleCmdMaxNum   (8)   /* max number of registered console commands */


//----------------------------------------------------------------------------
//                                 Typedefs
//----------------------------------------------------------------------------

// structure definition to define all ASCII messages
typedef struct msg_struct
{
    ROM INT8   *p_cmdName;       // pointer to cmd name
    ROM INT8   *p_cmdHelp;       // pointer to cmd help string
    UINT8   maxTokens;            // max tokens for this command
} tWFCmd;

// parsed version numbers put in this structure
typedef struct version_struct
{
    UINT8 major;
    UINT8 minor;
    UINT8 revision;
} tWFVersion;


//----------------------------------------------------------------------------
//                                 External Globals
//----------------------------------------------------------------------------
extern  const tWFCmd    g_consoleCmd[];
extern  const UINT8     g_numCmds;


//----------------------------------------------------------------------------
//                                 Function Prototypes
//----------------------------------------------------------------------------
void   TokenizeCmdLine(INT8 *p_line);
UINT8  GetCmdId(void);
void   Output_Monitor_Hdr(void);
BOOL   ConvertASCIIHexToBinary(INT8 *p_ascii, UINT16 *p_binary);
BOOL   ConvertASCIIUnsignedDecimalToBinary(INT8 *p_ascii, UINT16 *p_binary);
BOOL   ConvertASCIISignedDecimalToBinary(INT8 *p_ascii, INT16 *p_binary);
UINT8  HexToBin(UINT8 hexChar);
void   WFConsoleSetMsgFlag(void);



#ifdef __cplusplus
 }
#endif


#endif /* __WFCONSOLEMSGS_H */
