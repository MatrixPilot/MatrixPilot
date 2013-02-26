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
#define __Zeroconf_Helper_C

#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL) || defined(STACK_USE_ZEROCONF_MDNS_SD)

#include "TCPIP Stack/ZeroconfHelper.h"

BYTE zeroconf_dbg_level = 3; // All levels of debug info are printed.

#if defined(NEED_TO_DEFINE_zeroconf_dbg_msg)

char zeroconf_dbg_msg[ZEROCONF_DBG_MSG_SIZE];

#endif

#if defined(INFO_ZCLL)
void info_zcll_print(char * msg)
{
	if (zeroconf_dbg_level >= 1)
		putsUART(msg);
}
#endif

#if defined(DEBUG_ZCLL)
void debug_zcll_print(char * msg)
{
	if (zeroconf_dbg_level >= 2)
		putsUART(msg);
}
#endif

#if defined(INFO_MDNS)
void info_mdns_print(char * msg)
{
	if (zeroconf_dbg_level >= 1)
		putsUART(msg);
}
#endif

#if defined(DEBUG_MDNS)
void debug_mdns_print(char * msg)
{
	if (zeroconf_dbg_level >= 2)
		putsUART(msg);
}
#endif

/*
The calling convention is:

	static TICK event_time = 0;
	static TICK random_delay = 0;
	static BYTE time_recorded = 0;

	switch ( zgzc_wait_for(&random_delay, &event_time, &time_recorded) )
	{
	case ZGZC_STARTED_WAITING:

		# Set random_delay value;

		// Intentional fall-through

	case ZGZC_KEEP_WAITING:

		// Not Completed the delay proposed
		return;
	}

	// Completed the delay required

	# Do the scheduled work;
*/

BYTE zgzc_wait_for(DWORD *pTicksToWait, DWORD *pStartingTickTime, BYTE *pWaitingHasStarted)
{
	if ( !(*pWaitingHasStarted) )
	{
		// start a new waiting period
		*pStartingTickTime = TickGet();		// The time we started the waiting.
		*pWaitingHasStarted = 1;			// To indicate that the timer has started.

		return ZGZC_STARTED_WAITING;
	}

	if( (TickGet() - *pStartingTickTime) < (*pTicksToWait) )
	{
		/* Not Completed the delay proposed */
		return ZGZC_KEEP_WAITING;
	}
	
	// We have completed the required waiting.

	*pStartingTickTime	= 0 ; /* Reset starting time. Not really necessary. */
	*pWaitingHasStarted = 0;  /* Reset timer */

	return ZGZC_DONE_WAITING;
}

#endif // (STACK_USE_ZEROCONF_LINK_LOCAL) || defined(STACK_USE_ZEROCONF_MDNS_SD)
