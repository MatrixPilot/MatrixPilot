/*********************************************************************
 *
 *  Zero Confiruation (Zeroconf) Helper
 *  Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ZeroconfHelper.h
 * Dependencies:    ZeroconfLinkLocal, ZeroconfMulticastDNS
 * Processor:       PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2009-2010 Microchip Technology Inc.  All rights
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
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Pradeep Reddy        01 Mar 2009 Initial
 * Brad Rex             05 Apr 2010 Updated for MRF24W.
 ********************************************************************/
#ifndef __ZEROCONF_HELPER_H
#define __ZEROCONF_HELPER_H

#include "TCPIP Stack/TCPIP.h"

// Debugging levels
//#define DEBUG_ZCLL  y					/* Debug Enable for Zeroconf Link-Local Module */
//#define INFO_ZCLL   y
//#define WARN_ZCLL   y

// Debugging levels
//#define DEBUG_MDNS y
//#define INFO_MDNS  y
//#define WARN_MDNS  y

extern BYTE zeroconf_dbg_level;

#if defined(DEBUG_ZCLL) || defined(INFO_ZCLL) || defined(DEBUG_MDNS) || defined(INFO_MDNS)
	/* defined(WARN_ZCLL) || defined(WARN_MDNS */

	#define NEED_TO_DEFINE_zeroconf_dbg_msg 	y
	#define ZEROCONF_DBG_MSG_SIZE  256
	
	extern char zeroconf_dbg_msg[ZEROCONF_DBG_MSG_SIZE];

#if defined(INFO_ZCLL)
	extern void info_zcll_print(char * msg);
#endif
#if defined(DEBUG_ZCLL)
	extern void debug_zcll_print(char * msg);
#endif

#if defined(INFO_MDNS)
	extern void info_mdns_print(char * msg);
#endif

#if defined(DEBUG_MDNS)
	extern void debug_mdns_print(char * msg);
#endif

#endif

#define ZGZC_STARTED_WAITING  (1u)
#define ZGZC_KEEP_WAITING     (3u)
#define ZGZC_DONE_WAITING     (5u)

BYTE zgzc_wait_for(DWORD *pTicksToWait, DWORD *pStartingTickTime, BYTE *pIsStarted);

#endif
