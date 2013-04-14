/*********************************************************************
 *
 *                  AutoIP Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        AutoIP.h
 * Dependencies:    
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
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
 * Author               Date      Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Peter Reen           7/10/09   Original        (Rev 1.0)
 ********************************************************************/

#ifndef __AUTOIP_H
#define __AUTOIP_H

// AutoIP State Machine
typedef enum _SM_AUTOIP
{
    SM_AUTOIP_DISABLED = 0,
    SM_AUTOIP_INIT_RNG,
    SM_AUTOIP_CHECK_ADDRESS,
    SM_AUTOIP_SETUP_MESSAGE,
    SM_AUTOIP_GRATUITOUS_ARP1,
    SM_AUTOIP_GRATUITOUS_ARP2,
    SM_AUTOIP_GRATUITOUS_ARP3,
    SM_AUTOIP_DELAY,
    SM_AUTOIP_RATE_LIMIT_SET,
    SM_AUTOIP_RATE_LIMIT_WAIT,
    SM_AUTOIP_CONFIGURED,
    SM_AUTOIP_DEFEND
} SM_AUTOIP;


// Function prototypes
void AutoIPInit(BYTE vInterface);
void AutoIPTasks(void);
void AutoIPConflict(BYTE vInterface);
BOOL AutoIPIsConfigured(BYTE vInterface);
BOOL AutoIPConfigIsInProgress (BYTE vInterface);
BOOL AutoIPIsDisabled(BYTE vInterface);
void AutoIPDisable(BYTE vInterface);
void AutoIPEnable(BYTE vInterface);


#endif
