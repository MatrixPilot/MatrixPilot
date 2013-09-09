/*********************************************************************
 *
 *  Zero Confiruation (Zeroconf) IPV4 Link Local Addressing
 *  Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ZeroconfLinkLocal.h
 * Dependencies:    IP, ARP
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
#ifndef __ZEROCONF_LINK_LOCAL_H
#define __ZEROCONF_LINK_LOCAL_H

#define ZEROCONF_LINK_LOCAL_DHCP_TIMEOUT 5  // Time for which ZCLL allows DHCP to get IP

// define the debugging levels
#include "TCPIP Stack/ZeroconfHelper.h"

// For verbose debugging
#ifdef DEBUG_ZCLL
#define DEBUG_ZCLL_PRINT(x) debug_zcll_print(x)
#define DEBUG_ZCLL_MESG     sprintf
#else
#define DEBUG_ZCLL_PRINT(x)
#define DEBUG_ZCLL_MESG(x,y,...) 
#endif

// For less verbose debugging
#ifdef INFO_ZCLL
#define INFO_ZCLL_PRINT(x) info_zcll_print(x)
#define INFO_ZCLL_MESG     sprintf
#else
#define INFO_ZCLL_PRINT(x)
#define INFO_ZCLL_MESG(x,y,...) 
#endif

// For warnings, errors, and other exceptions
#ifdef WARN_ZCLL
#define WARN_ZCLL_PRINT(x) putsUART(x)
#define WARN_ZCLL_MESG     sprintf
#else
#define WARN_ZCLL_PRINT(x)
#define WARN_ZCLL_MESG(x,y,...) 
#endif

// Always print
#define ZCLL_PRINT(x) putsUART(x)
#define ZCLL_MESG     sprintf

#define DEBUG0_ZCLL_PRINT  DEBUG_ZCLL_PRINT
#define DEBUG0_ZCLL_MESG   DEBUG_ZCLL_MESG

void ZeroconfLLInitialize(void);
void ZeroconfLLProcess(void);

#endif //#ifndef __ZEROCONF_LINK_LOCAL_H
