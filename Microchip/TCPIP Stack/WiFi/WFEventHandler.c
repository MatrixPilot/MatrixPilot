/******************************************************************************

 MRF24W Driver Event Handler
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFEventHandler.c
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

/*==========================================================================*/
/*                                  INCLUDES                                */
/*==========================================================================*/
#include "TCPIP Stack/WFMac.h"
#if defined(WF_CS_TRIS)

#include "TCPIP Stack/TCPIP.h"

/*==========================================================================*/
/*                                  DEFINES                                 */
/*==========================================================================*/
/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_EVENT_HANDLER
#endif 

/*-------------------------------------------*/
/* Connection Manager Event Message Subtypes */
/* (Used in Mgmt Indicate messages)          */
/*-------------------------------------------*/
#define WF_EVENT_CONNECTION_ATTEMPT_STATUS_SUBTYPE   (6)  
#define WF_EVENT_CONNECTION_LOST_SUBTYPE             (7)
#define WF_EVENT_CONNECTION_REESTABLISHED_SUBTYPE    (8)
#define WF_EVENT_KEY_CALCULATION_REQUEST_SUBTYPE    (9)
#define WF_EVENT_SCAN_RESULTS_READY_SUBTYPE          (11)
#define WF_EVENT_SCAN_IE_RESULTS_READY_SUBTYPE       (12)

/* event values for index 2 of WF_CONNECTION_ATTEMPT_STATUS_EVENT_SUBTYPE */
#define CONNECTION_ATTEMPT_SUCCESSFUL    ((UINT8)1)   /* if not 1 then failed to connect and info field is error code */
#define CONNECTION_ATTEMPT_FAILED        ((UINT8)2)

/* event values for index 2 of WF_EVENT_CONNECTION_LOST_SUBTYPE */
#define CONNECTION_TEMPORARILY_LOST      ((UINT8)1)
#define CONNECTION_PERMANENTLY_LOST      ((UINT8)2)
#define CONNECTION_REESTABLISHED         ((UINT8)3)   



/*==========================================================================*/
/*                                  LOCAL FUNCTIONS                         */
/*==========================================================================*/
static BOOL isNotifyApp(UINT8 event);
static BOOL isEventNotifyBitSet(UINT8 notifyMask, UINT8 notifyBit);

/* if a MRF24WB device */
#if !defined(MRF24WG)
    #define RAW_MGMT_RX_ID   RAW_RX_ID
#endif

extern void SignalWiFiConnectionChanged(BOOL state);
extern void RenewDhcp(void);

/*****************************************************************************
 * FUNCTION: WFProcessMgmtIndicateMsg
 *
 * RETURNS:  error code
 *
 * PARAMS:   None
 *
 *  NOTES:   Processes a management indicate message
 *****************************************************************************/
void WFProcessMgmtIndicateMsg()
{
    tMgmtIndicateHdr  hdr;
    UINT8 buf[6];
    UINT8 event = 0xff;
    UINT16 eventInfo;
    tMgmtIndicatePassphraseReady passphraseReady;

    /* read 2-byte header of management message */
    RawRead(RAW_MGMT_RX_ID, 0, sizeof(tMgmtIndicateHdr), (UINT8 *)&hdr); 
        
    /* Determine which event occurred and handle it */
    switch (hdr.subType)
    {
        /*-----------------------------------------------------------------*/        
        case WF_EVENT_CONNECTION_ATTEMPT_STATUS_SUBTYPE:
        /*-----------------------------------------------------------------*/
#if defined(MRF24WG)
/* There is one data byte with this message */
            RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr),2, buf); /* read first 2 bytes after header */
            /* if connection attempt successful */
            if (buf[0] == CONNECTION_ATTEMPT_SUCCESSFUL)
            {
                event = WF_EVENT_CONNECTION_SUCCESSFUL;
                eventInfo = WF_NO_ADDITIONAL_INFO;
                SignalWiFiConnectionChanged(TRUE);
                #if defined (STACK_USE_DHCP_CLIENT)
                    RenewDhcp();
                #endif
                SetLogicalConnectionState(TRUE);
            }
            /* else connection attempt failed */
            else
            {
                event = WF_EVENT_CONNECTION_FAILED;
                eventInfo = (UINT16)(buf[0] << 8 | buf[1]); /* contains connection failure code */
                SetLogicalConnectionState(FALSE);
            }

#else    /* !defined(MRF24WG) */
        /* There is one data byte with this message */
        RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 1, buf); /* read first byte after header */
        /* if connection attempt successful */
        if (buf[0] == CONNECTION_ATTEMPT_SUCCESSFUL)
        {
            event = WF_EVENT_CONNECTION_SUCCESSFUL;
            eventInfo = WF_NO_ADDITIONAL_INFO;
            SignalWiFiConnectionChanged(TRUE);
            #if defined (STACK_USE_DHCP_CLIENT)
                RenewDhcp();
            #endif
            SetLogicalConnectionState(TRUE);
        }
        /* else connection attempt failed */
        else
        {
            event = WF_EVENT_CONNECTION_FAILED;
            eventInfo = (UINT16)buf[0];             /* contains connection failure code */
            SetLogicalConnectionState(FALSE);
        }
#endif    /* defined(MRF24WG) */
        break;
            
        /*-----------------------------------------------------------------*/
        case WF_EVENT_CONNECTION_LOST_SUBTYPE:
        /*-----------------------------------------------------------------*/ 
            /* read index 2 and 3 from message and store in buf[0] and buf[1]
               buf[0] -- 1: Connection temporarily lost  2: Connection permanently lost 3: Connection Reestablished 
               buf[1] -- 0: Beacon Timeout  1: Deauth from AP  */                         
            RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 2, buf); 

            if (buf[0] == CONNECTION_TEMPORARILY_LOST)
            {
                event     = WF_EVENT_CONNECTION_TEMPORARILY_LOST;
                eventInfo = (UINT16)buf[1];    /* lost due to beacon timeout or deauth */
                SignalWiFiConnectionChanged(FALSE);
            }
            else if (buf[0] == CONNECTION_PERMANENTLY_LOST)
            {
                event     = WF_EVENT_CONNECTION_PERMANENTLY_LOST;
                eventInfo = (UINT16)buf[1];   /* lost due to beacon timeout or deauth */
                SetLogicalConnectionState(FALSE);   
                SignalWiFiConnectionChanged(FALSE);              
            }
            else if (buf[0] == CONNECTION_REESTABLISHED)
            {
                event     = WF_EVENT_CONNECTION_REESTABLISHED;
                eventInfo = (UINT16)buf[1];    /* originally lost due to beacon timeout or deauth */
                #if defined(STACK_USE_DHCP_CLIENT)
                    RenewDhcp();
                #endif
                SignalWiFiConnectionChanged(TRUE);
                SetLogicalConnectionState(TRUE);
            }    
            else
            {
                /* invalid parameter in message */
                WF_ASSERT(FALSE);
            }        
            break;
        
        /*-----------------------------------------------------------------*/                    
        case WF_EVENT_SCAN_RESULTS_READY_SUBTYPE:        
        /*-----------------------------------------------------------------*/        
            /* read index 2 of mgmt indicate to get the number of scan results */
            RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 1, buf);            
            event = WF_EVENT_SCAN_RESULTS_READY;
            eventInfo = (UINT16)buf[0];          /* number of scan results */
            break;
            
        /*-----------------------------------------------------------------*/
        case WF_EVENT_SCAN_IE_RESULTS_READY_SUBTYPE:
        /*-----------------------------------------------------------------*/
            event = WF_EVENT_IE_RESULTS_READY;
            /* read indexes 2 and 3 containing the 16-bit value of IE bytes */
            RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 2, (UINT8 *)&eventInfo);
            eventInfo = WFSTOHS(eventInfo);     /* fix endianess of 16-bit value */
            break;    
            
#if defined(MRF24WG)
        case WF_EVENT_KEY_CALCULATION_REQUEST_SUBTYPE:
            event = WF_EVENT_KEY_CALCULATION_REQUEST;
            RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr),
            sizeof(tMgmtIndicatePassphraseReady), (UINT8 *)&passphraseReady);
            break;
#endif
            
        /*-----------------------------------------------------------------*/
        default:
        /*-----------------------------------------------------------------*/
            WF_ASSERT(FALSE);
            break;        
    }
    
    /* free mgmt buffer */
    DeallocateMgmtRxBuffer();

    /* if the application wants to be notified of the event */
    if (isNotifyApp(event))
    {
        WF_ProcessEvent(event, eventInfo, (UINT8 *)&passphraseReady);  
    }    
}

 
/*****************************************************************************
 * FUNCTION: isEventNotifyBitSet
 *
 * RETURNS:  TRUE if the notify bit is set in the notify mask.
 *
 * PARAMS:   notifyMask -- the bit mask of events the application wishes to be 
 *                         notified of
 *           notifyBit  -- the specific event that occurred
 *
 *  NOTES:   Determines if the input event it enabled in the notify mask
 *****************************************************************************/
static BOOL isEventNotifyBitSet(UINT8 notifyMask, UINT8 notifyBit)
{
    /* check if the event notify bit is set */    
    return ((notifyMask & notifyBit) > 0);
}

/*****************************************************************************
 * FUNCTION: isNotifyApp
 *
 * RETURNS:  TRUE if application wants to be notified of event, else FALSE
 *
 * PARAMS:   event -- the event that occurred
 *
 *  NOTES:   Determines if the input event is one which the application should be 
 *           notified of.
 *****************************************************************************/
static BOOL isNotifyApp(UINT8 event)
{
    BOOL notify = FALSE;
    UINT8 notifyMask;
    
    notifyMask = GetEventNotificationMask();     
    
    /* determine if user wants to be notified of event */
    switch (event)
    {
        case WF_EVENT_CONNECTION_SUCCESSFUL:
            if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL))      
            {
                notify = TRUE;          
            }    
            break;
            
        case WF_EVENT_CONNECTION_FAILED:
            if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_ATTEMPT_FAILED))      
            {
                notify = TRUE;          
            }    
            break;
            
        case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
            if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_TEMPORARILY_LOST))      
            {
                notify = TRUE;          
            }    
            break;
            
        case WF_EVENT_CONNECTION_PERMANENTLY_LOST:
            if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_PERMANENTLY_LOST))      
            {
                notify = TRUE;          
            }    
            break;
            
        case WF_EVENT_CONNECTION_REESTABLISHED:
            if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_REESTABLISHED))      
            {
                notify = TRUE;          
            }    
            break;
 
        default:
            notify = TRUE;  /* the app gets notified of all other events */
            break;
    }  
    
    return notify;  
    
}  



#endif /* WF_CS_TRIS */


