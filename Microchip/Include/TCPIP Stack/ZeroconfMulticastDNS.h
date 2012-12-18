/*********************************************************************
 *
 *  Zero Configuration (Zeroconf) Multicast DNS and 
 *  Service Discovery Module for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ZeroconfMulticastDNS.h
 * Dependencies:    IP
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
 * Pradeep Reddy        31 Mar 2009 Initial creation
 * Pradeep Reddy        23 Apr 2009 DNS-SD implementation
 * Brad Rex             05 Apr 2010 Updated for MRF24W
 ********************************************************************/
#ifndef __ZEROCONF_MULTICAST_DNS_H
#define __ZEROCONF_MULTICAST_DNS_H

// --------------------------------------------------------------
// Configurations
//

#define HTTPUpdateRecord_INTERVAL (300u) 		// Seconds between HTTPUpdateRecord() calls.

#define MDNS_DEFAULT_SERVICE_NAME "Microchip Demo"

// define the debugging levels
#include "TCPIP Stack/ZeroconfHelper.h"
#if defined(MRF24WG)
#include "WFApi.h"
#endif

// For verbose debugging
#ifdef DEBUG_MDNS
#define DEBUG_MDNS_PRINT(x) debug_mdns_print(x)
#define DEBUG_MDNS_MESG     sprintf
#else
#define DEBUG_MDNS_PRINT(x)
#define DEBUG_MDNS_MESG(x,y,...)      
#endif

// For less verbose debugging
#ifdef INFO_MDNS
#define INFO_MDNS_PRINT(x) info_mdns_print(x)
#define INFO_MDNS_MESG     sprintf
#else
#define INFO_MDNS_PRINT(x)
#define INFO_MDNS_MESG(x,y,...)      
#endif

// For warnings, errors, and other exceptions
#ifdef WARN_MDNS
#define WARN_MDNS_PRINT(x) putsUART(x)
#define WARN_MDNS_MESG     sprintf
#else
#define WARN_MDNS_PRINT(x)
#define WARN_MDNS_MESG(x,y,...)      
#endif

// Always print
#define MDNS_PRINT(x) putsUART(x)
#define MDNS_MESG     sprintf

#define DEBUG0_MDNS_PRINT  INFO_MDNS_PRINT
#define DEBUG0_MDNS_MESG   INFO_MDNS_MESG

// --------------------------------------------------------------
// Function prototypes and data structures
//

void mDNSInitialize(const char *szHostName);
void mDNSProcess(void);
void mDNSFillHostRecord(void);

//void DisplayHostName(BYTE *HostName);

typedef enum {
    MDNSD_SUCCESS =  0,
    MDNSD_ERR_BUSY =     1, /* Already Being used for another Service */
    MDNSD_ERR_CONFLICT = 2, /* Name Conflict */
    MDNSD_ERR_INVAL =    3, /* Invalid Parameter */
} MDNSD_ERR_CODE;

/* DNS-Service Discovery APIs */
MDNSD_ERR_CODE mDNSServiceRegister(
        const char *name,
        const char *regtype,
        WORD port,
        const BYTE *txtRecord,
        BYTE auto_rename,
        void (*call_back)(char *name, MDNSD_ERR_CODE err, void *context),    
        void *context );
MDNSD_ERR_CODE mDNSServiceDeRegister();

MDNSD_ERR_CODE mDNSServiceUpdate(
        WORD port,
        const BYTE *txt_record);

MDNSD_ERR_CODE mDNSHostRegister(const char *host_name);

MDNSD_ERR_CODE mDNSMulticastFilterRegister(void);

//#if defined(DEBUG_MDNS) || defined(INFO_MDNS)
	void mDNSDumpInfo(void);
//#endif

#endif
