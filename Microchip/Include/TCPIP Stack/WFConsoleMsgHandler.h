/******************************************************************************

 MRF24W Driver Console messages
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFConsoleMsgHandler.h
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
 KH                 27 Jan 2010 Created for MRF24W
******************************************************************************/

#ifndef __WFCONSOLEVT100_H
#define __WFCONSOLEVT100_H


#include "TCPIP Stack/WFConsoleMsgs.h"

#if defined (WF_CONSOLE)
typedef struct
{
    INT8    rxBuf[kConsoleMaxMsgSize];  // buf for received characters
    INT8    txBuf[kConsoleMaxMsgSize];  // buf for transmitted characters
    UINT8   rxState;                    // current state of Rx state machine
    UINT8   cursorIndex;                // cursor index
    BOOL    firstChar;                  // false if waiting for very first character from user or PC
    BOOL    echoOn;                     // true if human input at console, false if PC communicating

    INT8    p_cmdStrings[kConsoleCmdMaxNum][kConsoleCmdMaxLen];  // cmd string array
    UINT8   numCmdStrings;              // number of cmd strings in p_cmdStrings

    UINT8   appConsoleMsgRx;            // true if app received a console msg, else false

#if defined( __18CXX )
    FAR INT8*   argv[kWFMaxTokensPerCmd];   // pointer to each token in the rxBuf
#else
    INT8*       argv[kWFMaxTokensPerCmd];   // pointer to each token in the rxBuf
#endif

    UINT8   argc;                       // number of tokens in rxBuf
    UINT8   subState;
    BOOL    bStateMachineLoop;
    UINT8   req;
} tConsoleContext;
#endif

#define SET_ECHO_ON()       g_ConsoleContext.echoOn = TRUE
#define SET_ECHO_OFF()      g_ConsoleContext.echoOn = FALSE
#define IS_ECHO_ON()        g_ConsoleContext.echoOn


/*---------------------*/
/* Function Prototypes */
/*---------------------*/
BOOL convertAsciiToHexInPlace(INT8 *p_string, UINT8 expectedHexBinSize);

void process_cmd(void);

#endif /* __WFCONSOLEVT100_H */
