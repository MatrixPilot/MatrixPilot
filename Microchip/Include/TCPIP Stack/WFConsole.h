/******************************************************************************

 MRF24W Driver Console
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFConsole.h
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

#ifndef _WFCONSOLE_H_
#define _WFCONSOLE_H_

#include "TCPIP Stack/TCPIP.h"
#include "TCPIP Stack/WFConsoleMsgHandler.h"

#if defined (WF_CONSOLE)
  #if defined(__18CXX)
  #pragma varlocate 4 g_ConsoleContext
  #endif

  extern tConsoleContext g_ConsoleContext;
#endif

#define ARGC           g_ConsoleContext.argc
#define ARGV           g_ConsoleContext.argv

#if !defined(WF_CS_TRIS) && defined(WF_CONSOLE)
  #undef WF_CONSOLE
#endif

#if !defined(WF_CONSOLE) && defined(WF_CONSOLE_IFCFGUTIL)
  #undef WF_CONSOLE_IFCFGUTIL
#endif

extern void WFConsoleInit(void);
extern void WFConsoleProcess(void);
extern void WFConsoleProcessEpilogue(void);
extern void WFConsoleReqClear(void);
extern BOOL WFConsoleIsConsoleMsgReceived(void);
extern void WFConsoleReleaseConsoleMsg(void);
extern INT8 ** WFConsoleGetCmdLineArgv(void);
extern UINT8 WFConsoleGetCmdLineArgc(void);
extern void WFConsoleSetMsgFlag(void);
extern BOOL WFConsoleIsIperfAppKillRequested(void);

extern void WFConsolePrintInteger(UINT32 val, char mode);
extern void WFConsolePrintHex(UINT32 val, UINT8 width);

#define WFConsolePrintRamStr(str, newline) do { \
        putsUART((char*)(str)); \
        if (newline) putrsUART("\n\r"); \
    } while (FALSE)

#define WFConsolePrintRomStr(str, newline) do { \
        putrsUART(str); \
        if (newline) putrsUART("\n\r"); \
    } while (FALSE)

#endif /* _WFCONSOLE_H_ */
