/******************************************************************************

 MRF24W Driver Management Messages
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFMgmtMsg.c
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

/*
*********************************************************************************************************
*                                           INCLUDES                               
*********************************************************************************************************
*/

#include "TCPIP Stack/WFMac.h"
#include "TCPIP Stack/TCPIP.h"
#if defined(WF_CS_TRIS)

/*
*********************************************************************************************************
*                                           DEFINES                               
*********************************************************************************************************
*/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_MGMT_MSG
#endif

/* Macro used to determine is application is trying to send a management when in the driver function  */
/* WF_ProcessEvent().                                                                                 */
#define isInWFProcessEvent()    ((g_FuncFlags & WF_PROCESS_EVENT_FUNC) > 0)

/*
*********************************************************************************************************
*                                           LOCAL GLOBAL VARIABLES                               
*********************************************************************************************************
*/

static volatile BOOL gMgmtConfirmMsgReceived  = FALSE;
static BOOL RestoreRxData = FALSE;
UINT8 g_WaitingForMgmtResponse = FALSE;

#if defined(WF_DEBUG)
   static UINT8 g_FuncFlags = 0x00;  
#endif


/*****************************************************************************
 * FUNCTION: SendMgmtMsg
 *
 * RETURNS:  error code
 *
 * PARAMS:   p_header      -- pointer to mgmt message header data
 *           headerLength  -- number of bytes in the header
 *                              will be written
 *           p_data        -- pointer to mgmt message data
 *           dataLength    -- number of byte of data
 *
 *  NOTES:   Sends a management message
 *****************************************************************************/
void SendMgmtMsg(UINT8 *p_header,
                 UINT8 headerLength,
                 UINT8 *p_data,
                 UINT8 dataLength)
{
    #if defined(__18CXX)
        static UINT32  startTickCount;   
        static UINT32  maxAllowedTicks;  
    #else
        UINT32  startTickCount;   
        UINT32  maxAllowedTicks;  
    #endif


    /* cannot send management messages while in WF_ProcessEvent() */
    WF_ASSERT(!isInWFProcessEvent());
    
    EnsureWFisAwake();
    
    /* if a Rx Data packet is mounted that has not yet been processed */
    if (GetRawWindowState(RAW_RX_ID) == WF_RAW_DATA_MOUNTED)
    {
        /* save it, so after mgmt response received it can be restored */
        PushRawWindow(RAW_RX_ID);
        RestoreRxData = TRUE;     
    }    


    /* mounts a tx mgmt buffer on the MRF24W when data tx is done */
    maxAllowedTicks = TICKS_PER_SECOND / 200;  /* 5 ms timeout */
    startTickCount = (UINT32)TickGet();
    while (!WFisTxMgmtReady() )
    {
        MACProcess();
        
        /* DEBUG -- REMOVE AFTER FIGURE OUT WHY TIMING OUT (RARELY HAPPENS) */
        if (TickGet() - startTickCount >= maxAllowedTicks)
        {
            /* force flags so WFisTxMgmtReady will return TRUE */
            SetRawWindowState(RAW_TX_ID, WF_RAW_UNMOUNTED);
            RawWindowReady[RAW_TX_ID] = FALSE;
        }    
    }    

    /* write out management header */
    RawSetByte(RAW_TX_ID, p_header, headerLength); 
    
    /* write out data (if any) */
    if (dataLength > 0)
    {
        RawSetByte(RAW_TX_ID, p_data, dataLength);         
    }  

    /* send mgmt msg to MRF24W */
    SendRAWManagementFrame(headerLength + dataLength);
}                           

/*****************************************************************************
 * FUNCTION: SignalMgmtConfirmReceivedEvent
 *
 * RETURNS:  None
 *
 * PARAMS:   None
 *
 *  NOTES:   Called by ProcessMgmtRxMsg when a mgmt confirm has been received.  
 *           This function then sets a local flag for this module indicating 
 *           the event.  
 *****************************************************************************/
void SignalMgmtConfirmReceivedEvent(void)
{
    gMgmtConfirmMsgReceived = TRUE;        
}      

/*****************************************************************************
 * FUNCTION: WaitForMgmtResponse
 *
 * RETURNS:  None
 *
 * PARAMS:   expectedSubtype -- The expected subtype of the mgmt response
 *           freeAction      -- FREE_MGMT_BUFFER or DO_NOT_FREE_MGMT_BUFFER
 *
 *  NOTES:   Called after sending a mgmt request.  This function waits for a mgmt
 *           response.  The caller can optionally request the the management 
 *           response be freed immediately (by this function) or not freed.  If not
 *           freed the caller is responsible to free the response buffer.
 *****************************************************************************/
void WaitForMgmtResponse(UINT8 expectedSubtype, UINT8 freeAction)
{
    #if defined(__18CXX)
        static tMgmtMsgRxHdr  hdr; 
    #else
        tMgmtMsgRxHdr  hdr; 
    #endif
    
    g_WaitingForMgmtResponse = TRUE;
        
    /* Wait until mgmt response is received */
    while (gMgmtConfirmMsgReceived == FALSE)
    {
        WFProcess();
        
        /* if received a data packet while waiting for mgmt packet */
        if (g_HostRAWDataPacketReceived)
        {
            // We can't let the StackTask processs data messages that come in while waiting for mgmt 
            // response because the application might send another mgmt message, which is illegal until the response
            // is received for the first mgmt msg.  And, we can't prevent the race condition where a data message 
            // comes in before a mgmt response is received.  Thus, the only solution is to throw away a data message
            // that comes in while waiting for a mgmt response.  This should happen very infrequently.  If using TCP then the 
            // stack takes care of retries.  If using UDP, the application has to deal with occasional data messages not being
            // received.  Also, applications typically do not send a lot of management messages after connected.

            // throw away the data rx 
            RawMountRxBuffer();
            DeallocateDataRxBuffer();
            g_HostRAWDataPacketReceived = FALSE;

            /* ensure interrupts enabled */
            WF_EintEnable();
        }    
    } 
    
    /* set this back to FALSE so the next mgmt send won't think he has a response before one is received */
    gMgmtConfirmMsgReceived = FALSE;
    
    
    /* if the caller wants to delete the response immediately (doesn't need any data from it */
    if (freeAction == FREE_MGMT_BUFFER)
    {
        /* read and verify result before freeing up buffer to ensure our message send was successful */
        RawRead(RAW_RX_ID, 0, (UINT16)(sizeof(tMgmtMsgRxHdr)), (UINT8 *)&hdr);

        /* mgmt response subtype had better match subtype we were expecting */
        WF_ASSERT(hdr.subtype == expectedSubtype);

        if (hdr.result == WF_ERROR_DISCONNECT_FAILED 
            || hdr.result == WF_ERROR_NOT_CONNECTED) {
            #if defined(STACK_USE_UART)
                putrsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
            #endif
        } else if (hdr.result == WF_ERROR_NO_STORED_BSS_DESCRIPTOR) {
            #if defined(STACK_USE_UART)
                putrsUART("No stored scan results\r\n");
            #endif
        } else {
            WF_ASSERT(hdr.result == WF_SUCCESS); 
        }

        /* free mgmt buffer */
        DeallocateMgmtRxBuffer();  
        
        /* if there was a mounted data packet prior to the mgmt tx/rx transaction, then restore it */
        if (RestoreRxData == TRUE)
        {
            RestoreRxData = FALSE;
            PopRawWindow(RAW_RX_ID);
            SetRawWindowState(RAW_RX_ID, WF_RAW_DATA_MOUNTED); 
        }          
    }   
}  


/*****************************************************************************
 * FUNCTION: WaitForMgmtRespAndReadData
 *
 * RETURNS:  None
 *
 * PARAMS:   expectedSubtype -- management message subtype that we are expecting
 *           p_data          -- pointer where any desired management data bytes 
 *                              will be written
 
 *           numDataBytes    -- Number of data bytes from mgmt response to write to
 *                              p_data.  Data always starts at index 4 of mgmt response.
 *           skipDataRead    -- if TRUE, then no data will be read and the mgmt buffer will not
 *                              be freed.  If FALSE, the data will be read and the mgmt buffer
 *                              will be freed.
 *
 *  NOTES:   Waits for the mgmt response message and validates it by:
 *             1) checking the result field
 *             2) verifying that the received subtype matches the execpted subtype
 *
 *            In addition, this function reads the desired number of data bytes from 
 *            the mgmt response, copies them to p_data, and then frees the mgmt buffer. 
 *****************************************************************************/
void WaitForMgmtResponseAndReadData(UINT8 expectedSubtype, 
                                    UINT8 numDataBytes,  
                                    UINT8 startIndex, 
                                    UINT8 *p_data)

{
    tMgmtMsgRxHdr  hdr;  /* management msg header struct */
    
    WaitForMgmtResponse(expectedSubtype, DO_NOT_FREE_MGMT_BUFFER);
        
    /* if made it here then received a management message */
    RawRead(RAW_RX_ID, 0, (UINT16)(sizeof(tMgmtMsgRxHdr)), (UINT8 *)&hdr);

    /* check header result and subtype fields */
    WF_ASSERT(hdr.result  == WF_SUCCESS || hdr.result == WF_ERROR_NO_STORED_BSS_DESCRIPTOR);
    WF_ASSERT(hdr.subtype == expectedSubtype);

    /* if caller wants to read data from this mgmt response */
    if (numDataBytes > 0) 
    {
        RawRead(RAW_RX_ID, startIndex, numDataBytes, p_data);  
    }    
    
    /* free the mgmt buffer */    
    DeallocateMgmtRxBuffer();
    
     /* if there was a mounted data packet prior to the mgmt tx/rx transaction, then restore it */    
    if (RestoreRxData == TRUE)
    {
        RestoreRxData = FALSE;
        PopRawWindow(RAW_RX_ID);
        SetRawWindowState(RAW_RX_ID, WF_RAW_DATA_MOUNTED); 
    }          
}


#if defined(WF_DEBUG)
/*****************************************************************************
 * FUNCTION: WFSetFuncState
 *
 * RETURNS:  None
 *
 * PARAMS:   funcMask -- bit mask indicating the calling function
 *           state    -- WF_ENTERING_FUNCTION or WF_LEAVING_FUNCTION
 *
 *  NOTES:   Called by WF_ProcessEvent() to be able to detect if there is an attempt 
 *           to send a management message while processing the event (not allowed).
 *****************************************************************************/
void WFSetFuncState(UINT8 funcMask, UINT8 state)
{
    if (state == WF_ENTERING_FUNCTION)
    {
        g_FuncFlags |= funcMask;
    }    
    else
    {
        g_FuncFlags &= ~funcMask;
    }        
    
}    
#endif /* WF_DEBUG */


#endif /* WF_CS_TRIS */
