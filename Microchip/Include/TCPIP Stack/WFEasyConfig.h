/******************************************************************************

 MRF24W Driver 
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFEasyConfig.h
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
 Steve Collmeyer    24 Sep 2009 Initial
 Brad Rex           09 Feb 2010 Update for MRF24WB
******************************************************************************/

#ifndef __WFEASYCONFIG_H
#define __WFEASYCONFIG_H

#include "HardwareProfile.h"
#include "TCPIP Stack/TCPIP.h"
#include "TCPIPConfig.h"

#if defined(EZ_CONFIG_STORE)
#include "MainDemo.h"
#endif

typedef struct
{
    UINT8 configBits;                    /* used to dictate MAC behavior following the calculation */
    UINT8 phraseLen;                     /* number of valid bytes in passphrase */
    UINT8 ssidLen;                       /* number of valid bytes in ssid */
    UINT8 reserved;                      /* alignment byte */
    INT8 ssid[32];           /* the string of characters representing the ssid */
    INT8 passPhrase[64+1]; /* the string of characters representing the pass phrase */
} tWFPskCalcReq;

typedef enum {
    cfg_stopped = 0,
    cfg_stalled,
    cfg_wait_idle_complete,
    cfg_max
} tWFDynamicConfigStates;

typedef struct {
    BYTE            isWifiNeedToConfigure;
    BYTE            isWifiDoneConfigure;
    BYTE            ssid[33];               // 32 for ssid and one for term character
    BYTE            type;                   // Net type infrastructure vs adhoc
    BYTE            prevSSID[33];           // previous SSID we were connected to
    BYTE            prevWLAN;               // previous WLAN type
    
    BYTE            security;               // security type
    BYTE            defaultWepKey;          // WEP index value
    BYTE            key[65];                // 64-byte key plus term character

    tWFDynamicConfigStates cfg_state;
    #if defined (EZ_CONFIG_STALL)
    DWORD timeStart;
    #endif
} tWFEasyConfigCtx;

extern tWFEasyConfigCtx g_easyConfigCtx;
#define CFGCXT    (g_easyConfigCtx)

typedef struct
{
    INT8       scanState;
    UINT16  numScanResults;
    UINT16  displayIdx;
} tWFScanCtx;

extern tWFScanCtx  g_ScanCtx;
#define SCANCXT       (g_ScanCtx)

/* Easy Config Public Functions */
void WFEasyConfigInit(void);
void WFEasyConfigMgr(void);
void WFInitScan(void);

void WFScanEventHandler(UINT16 scanResults);

void WFDisplayScanMgr(void);

UINT16 WFStartScan(void);

UINT16 WFRetrieveScanResult(UINT8 Idx, tWFScanResult *p_ScanResult);

/* Macros */
#define WF_WEP_KEY_INVALID     0xff

#define WF_START_EASY_CONFIG()   CFGCXT.isWifiNeedToConfigure = 1;

/* Definitions for stall feature of state machine */
#define WF_STALL_TIME       1  /* In seconds */
#define WF_STALL_TIME_MS    (WF_STALL_TIME * TICKS_PER_SECOND)

/* Scan status/control bits */
#define SCAN_STATE_IN_PROGRESS    0x0001
#define SCAN_STATE_VALID_RESULTS  0x0002
#define SCAN_STATE_DISPLAY_RESULTS  0x0004

#define IS_SCAN_IN_PROGRESS(x)        ((x) & SCAN_STATE_IN_PROGRESS)
#define IS_SCAN_STATE_VALID(x)        ((x) & SCAN_STATE_VALID_RESULTS)
#define IS_SCAN_STATE_DISPLAY(x)      ((x) & SCAN_STATE_DISPLAY_RESULTS)
#define SCAN_SET_IN_PROGRESS(x)       ((x) |= SCAN_STATE_IN_PROGRESS)
#define SCAN_SET_VALID(x)             ((x) |= SCAN_STATE_VALID_RESULTS)
#define SCAN_SET_DISPLAY(x)           ((x) |= SCAN_STATE_DISPLAY_RESULTS)
#define SCAN_CLEAR_IN_PROGRESS(x)     ((x) &= ~SCAN_STATE_IN_PROGRESS)
#define SCAN_CLEAR_VALID(x)           ((x) &= ~SCAN_STATE_VALID_RESULTS)
#define SCAN_CLEAR_DISPLAY(x)         ((x) &= ~SCAN_STATE_DISPLAY_RESULTS)
#endif /* _WFEASYCONFIG_H_ */
