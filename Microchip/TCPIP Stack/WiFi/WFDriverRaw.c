/******************************************************************************

 MRF24W Driver RAW driver
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFDriverRaw.c
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

/*
*********************************************************************************************************
*                                           INCLUDES                               
*********************************************************************************************************
*/

#include "TCPIP Stack/WFMac.h"
#include "TCPIP Stack/TCPIP.h"
#if defined(WF_CS_TRIS)


/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_DRIVER_RAW
#endif


/*
*********************************************************************************************************
*                                           DEFINES                               
*********************************************************************************************************
*/

// RAW register masks
#define WF_RAW_STATUS_REG_ERROR_MASK    ((UINT16)(0x0002))
#define WF_RAW_STATUS_REG_BUSY_MASK     ((UINT16)(0x0001))

/*
*********************************************************************************************************
*                                           LOCAL DATA TYPES                               
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL GLOBAL VARIABLES                               
*********************************************************************************************************
*/

BOOL g_HostRAWDataPacketReceived = FALSE;  // set TRUE by state machine in WFDriverCom.c
extern BOOL g_rxIndexSetBeyondBuffer;         // debug -- remove after test

BOOL     RawWindowReady[2];     /* for Tx and Rx, TRUE = ready for use, FALSE = not ready for use */
UINT8  RawWindowState[2];

extern BOOL g_WaitingForMgmtResponse;

/*
*********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES                               
*********************************************************************************************************
*/
static UINT16 RawMove(UINT16 rawId, UINT16 srcDest, BOOL rawIsDestination, UINT16 size);
static UINT16 WaitForRawMoveComplete(UINT8 rawId);


BOOL AllocateMgmtTxBuffer(UINT16 bytesNeeded)
{
    UINT16 bufAvail;
    UINT16 byteCount;
    
    /* get total bytes available for MGMT tx memory pool */
    bufAvail = Read16BitWFRegister(WF_HOST_WFIFO_BCNT1_REG) & 0x0fff; /* LS 12 bits contain length */                    
    
    /* if enough bytes available to allocate */
    if ( bufAvail >= bytesNeeded )
    {
        /* allocate and create the new Tx buffer (mgmt or data) */
        byteCount = RawMove(RAW_TX_ID, RAW_MGMT_POOL, TRUE, bytesNeeded);
        WF_ASSERT(byteCount != 0);
    }
    /* else not enough bytes available at this time to satisfy request */
    else
    {
        return FALSE;
    }
    
    RawWindowReady[RAW_TX_ID] = TRUE;
    SetRawWindowState(RAW_TX_ID, WF_RAW_MGMT_MOUNTED);
    
    return TRUE;
}    

void DeallocateMgmtRxBuffer(void)
{
     /* Unmount (release) mgmt packet now that we are done with it */
    RawMove(RAW_RX_ID, RAW_MGMT_POOL, FALSE, 0);
    SetRawRxMgmtInProgress(FALSE);
    g_WaitingForMgmtResponse = FALSE;
    
}    

BOOL AllocateDataTxBuffer(UINT16 bytesNeeded)
{
    UINT16 bufAvail;
    UINT16 byteCount;
    
    /* Ensure the MRF24W is awake (only applies if PS-Poll was enabled) */
    EnsureWFisAwake();

    
    /* get total bytes available for DATA tx memory pool */
    bufAvail = Read16BitWFRegister(WF_HOST_WFIFO_BCNT0_REG) & 0x0fff; /* LS 12 bits contain length */                    
    
        /* if enough bytes available to allocate */
    if ( bufAvail >= bytesNeeded )
    {
        /* allocate and create the new Tx buffer (mgmt or data) */
        byteCount = RawMove(RAW_TX_ID, RAW_DATA_POOL, TRUE, bytesNeeded);
        WF_ASSERT(byteCount != 0);
    }
    /* else not enough bytes available at this time to satisfy request */
    else
    {
        return FALSE;
    }
    
    RawWindowReady[RAW_TX_ID] = TRUE;
    SetRawWindowState(RAW_TX_ID, WF_RAW_DATA_MOUNTED);
    
    
    return TRUE;

}    

void DeallocateDataTxBuffer(void)
{
    RawMove(RAW_TX_ID, RAW_DATA_POOL, FALSE, 0);
    RawWindowReady[RAW_TX_ID] = FALSE;
    SetRawWindowState(RAW_TX_ID, WF_RAW_UNMOUNTED);
}   

void DeallocateDataRxBuffer(void)
{
    RawMove(RAW_RX_ID, RAW_DATA_POOL, FALSE, 0);
}    

/* if a mgmt msg mounted in RAW window then message handled by MRF24W.                */
/* If a data message mounted in RAW window then will be transmitted to 802.11 network */
void RawSendTxBuffer(UINT16 len)
{
    RawMove(RAW_TX_ID, RAW_MAC, FALSE, len);
    RawWindowReady[RAW_TX_ID] = FALSE;
    SetRawWindowState(RAW_TX_ID, WF_RAW_UNMOUNTED);

} 

/* mounts the most recent Rx message.  Could be a management or data message. */
UINT16 RawMountRxBuffer(void)
{
    UINT16 length;
    
    length = RawMove(RAW_RX_ID, RAW_MAC, TRUE, 0);
    
    RawWindowReady[RAW_RX_ID] = TRUE;
    SetRawWindowState(RAW_RX_ID, WF_RAW_DATA_MOUNTED);
    
    
    return length;
} 

/* Copies from source raw window to destination raw window, each from their current indexes. */
void RawToRawCopy(UINT8 rawDestId, UINT16 length)
{
     RawMove(rawDestId, RAW_COPY, TRUE, length);
 
}          


/*
*********************************************************************************************************
*                                   PushRawWindow()
*    
* Description : Pushes a RAW window onto the 1-level deep RAW stack.  The RAW window state is preserved
*               and is restored when PopRawWindow() is called.
*
* Argument(s) : rawId -- RAW window ID that is being pushed.
*
* Return(s)   : None
*
* Caller(s)   : WF Driver
*
* Notes:      : (1) The RAW architecture supports a 1-level deep stack.  Each time this function is called
*                   any state that had been previously saved is lost.
*
*********************************************************************************************************
*/
void PushRawWindow(UINT8 rawId)
{
    RawMove(rawId, RAW_STACK_MEM, FALSE, 0);
}

/*
*********************************************************************************************************
*                                   PopRawWindow()
*    
* Description : Pops a RAW window state from the 1-level deep RAW stack.  The RAW window state that was 
*               mounted prior to this call is lost.
*
* Argument(s) : rawId -- RAW window ID that is being popped.
*
* Return(s)   : byte count of the RAW window state that was saved and is now restored.  In other words, the
*               size, in bytes, of the RAW window when it was first created.
*               of the o
*
* Caller(s)   : WF Driver
*
* Notes:      : (1) The RAW architecture supports a 1-level deep stack.  When this fucntion is called the 
*                   RAW window state that had been mounted is lost.  If trying to pop a non-existent RAW
*                   window state (no push has taken place), the the returned byte count is 0.
*
*********************************************************************************************************
*/
UINT16 PopRawWindow(UINT8 rawId)
{
    UINT16 byteCount;

    byteCount = RawMove(rawId, RAW_STACK_MEM, TRUE, 0);

    return byteCount;
}
        


/*
*********************************************************************************************************
*                                   ScratchMount()
*    
* Description : Mounts Scratch using the specified RAW window.
*
* Argument(s) : rawId -- desired RAW window to mount Scratch to.
*
* Return(s)   : None
*
* Caller(s)   : WF Driver
*
* Notes:      : None
*
*********************************************************************************************************
*/
UINT16 ScratchMount(UINT8 rawId)
{
    UINT16 byteCount;
    
    byteCount = RawMove(rawId, RAW_SCRATCH_POOL, TRUE, 0);
    if (byteCount == 0)
    {
        /* work-around, somehow the scratch was already mounted to the other raw window */
        rawId = !rawId;
        //   WF_ASSERT(byteCount > 0);  /* scratch mount should always return value > 0 */
    }    


    SetRawWindowState(rawId, WF_SCRATCH_MOUNTED);
    return byteCount;
}    

/*
*********************************************************************************************************
*                                   ScratchUnmount()
*    
* Description : Unmounts Scratch from the specified RAW window.
*
* Argument(s) : rawId -- RAW window ID that scratch had been mounted to.
*
* Return(s)   : None
*
* Caller(s)   : WF Driver
*
* Notes:      : None
*
*********************************************************************************************************
*/
void ScratchUnmount(UINT8 rawId)
{
    RawMove(rawId, RAW_SCRATCH_POOL, FALSE, 0);
    if (rawId == RAW_RX_ID)
    {
        SetRawWindowState(RAW_RX_ID, WF_RAW_UNMOUNTED);
    }
    else
    {
        SetRawWindowState(RAW_TX_ID, WF_RAW_UNMOUNTED);        
    }    
        
}    


/*
*********************************************************************************************************
*                                   RawRead()
*    
* Description : Reads the specified number of bytes from a mounted RAW window from the specified starting
*               index;
*
* Argument(s) : rawId      -- RAW window ID being read from
*               startIndex -- start index within RAW window to read from
*               length     -- number of bytes to read from the RAW window
*               p_dest     -- pointer to Host buffer where read data is copied
*
* Return(s)   : error code
*
* Caller(s)   : WF Driver
*
* Notes:      : None
*
*********************************************************************************************************
*/
void RawRead(UINT8 rawId, UINT16 startIndex, UINT16 length, UINT8 *p_dest)
{
    RawSetIndex(rawId, startIndex);
    RawGetByte(rawId, p_dest, length);
}
 
/*
*********************************************************************************************************
*                                   RawWrite()
*    
* Description : Writes the specified number of bytes to a mounted RAW window at the specified starting
*               index
*
* Argument(s) : rawId      -- RAW window ID being written to
*               startIndex -- start index within RAW window to write to
*               length     -- number of bytes to write to RAW window
*               p_src      -- pointer to Host buffer write data
*
* Return(s)   : None
*
* Caller(s)   : WF Driver
*
* Notes:      : None
*
*********************************************************************************************************
*/
void RawWrite(UINT8 rawId, UINT16 startIndex, UINT16 length, UINT8 *p_src)
{
    /*set raw index in dest memory */
    RawSetIndex(rawId, startIndex);

    /* write data to RAW window */
    RawSetByte(rawId, p_src, length);
}    

/*****************************************************************************
 * FUNCTION: RawMove
 *
 * RETURNS: Number of bytes that were overlayed (not always applicable)
 *
 * PARAMS:
 *      rawId   - RAW ID
 *      srcDest - MRF24W object that will either source or destination of move
 *      rawIsDestination - TRUE if RAW engine is the destination, FALSE if its the source
 *      size    - number of bytes to overlay (not always applicable)
 *
 *  NOTES: Performs a RAW move operation between a RAW engine and a MRF24W object
 *****************************************************************************/
 static UINT16 RawMove(UINT16   rawId,           
                       UINT16   srcDest,         
                       BOOL     rawIsDestination,  
                       UINT16   size)              
{
    UINT16 byteCount;
    UINT8 regId;
    UINT8  regValue8;
    UINT16 ctrlVal = 0;

    if (rawIsDestination)
    {
        ctrlVal |= 0x8000;
    }
    
    /* fix later, simply need to ensure that size is 12 bits are less */
    ctrlVal |= (srcDest << 8);              /* defines are already shifted by 4 bits */
    ctrlVal |= ((size >> 8) & 0x0f) << 8;   /* MS 4 bits of size (bits 11:8)         */
    ctrlVal |= (size & 0x00ff);             /* LS 8 bits of size (bits 7:0)          */

    /* Clear the interrupt bit in the register */
    regValue8 = (rawId == RAW_ID_0)?WF_HOST_INT_MASK_RAW_0_INT_0:WF_HOST_INT_MASK_RAW_1_INT_0;
    Write8BitWFRegister(WF_HOST_INTR_REG, regValue8);

    /* write update control value to register to control register */
    regId = (rawId==RAW_ID_0)?RAW_0_CTRL_0_REG:RAW_1_CTRL_0_REG;
    Write16BitWFRegister(regId, ctrlVal);

    // Wait for the RAW move operation to complete, and read back the number of bytes, if any, that were overlayed
    byteCount = WaitForRawMoveComplete(rawId);

    return byteCount;
}

/*****************************************************************************
 * FUNCTION: RawSetIndex
 *
 * RETURNS: True is success, false if timed out, which means attempted to set
 *          raw index past end of raw window.  Not a problem as long as no read
 *          or write occurs.
 *
 * PARAMS:
 *      rawId - RAW ID
 *      index - desired index
 *
 *  NOTES: Sets the RAW index for the specified RAW engine.  If attempt to set RAW
 *         index outside boundaries of RAW window this function will time out.
 *****************************************************************************/
BOOL RawSetIndex(UINT16 rawId, UINT16 index)
{
    UINT8 regId;
    UINT16 regValue;
    UINT32 startTickCount;
    UINT32 maxAllowedTicks;

    // set the RAW index
    regId = (rawId==RAW_ID_0)?RAW_0_INDEX_REG:RAW_1_INDEX_REG;
    Write16BitWFRegister(regId, index);

    startTickCount = (UINT32)TickGet();
    maxAllowedTicks = TICKS_PER_SECOND / 200;   /* 5ms */
    
    regId = (rawId==RAW_ID_0)?RAW_0_STATUS_REG:RAW_1_STATUS_REG;
    
    while (1)
    {
        regValue = Read16BitWFRegister(regId);
        if ((regValue & WF_RAW_STATUS_REG_BUSY_MASK) == 0)
        {
            return TRUE;
        }
        
        /* if timed out then trying to set index past end of raw window, which is OK so long as the app */
        /* doesn't try to access it                                                                     */
        if (TickGet() - startTickCount >= maxAllowedTicks)
        {
            return FALSE;  /* timed out waiting for Raw set index to complete */
        }    
    }
}

/*****************************************************************************
 * FUNCTION: RawGetIndex
 *
 * RETURNS: Returns the current RAW index for the specified RAW engine.
 *
 * PARAMS:
 *      rawId - RAW ID
 *
 *  NOTES: None
 *****************************************************************************/
UINT16 RawGetIndex(UINT16 rawId)
{
    UINT8  regId;
    UINT16 index;

    regId = (rawId==RAW_ID_0)?RAW_0_INDEX_REG:RAW_1_INDEX_REG;
    index = Read16BitWFRegister(regId);

    return index;
}


//#define OUTPUT_RAW_TX_RX   
extern BOOL g_WaitingForMgmtResponse;
/*****************************************************************************
 * FUNCTION: RawGetByte
 *
 * RETURNS: error code
 *
 * PARAMS:
 *      rawId   - RAW ID
 *      pBuffer - Buffer to read bytes into
 *      length  - number of bytes to read
 *
 *  NOTES: Reads bytes from the RAW engine
 *****************************************************************************/
void RawGetByte(UINT16 rawId, UINT8 *pBuffer, UINT16 length)
{
    UINT8 regId;
#if defined(OUTPUT_RAW_TX_RX)
    UINT16 i;
#endif

    /* if reading a data message do following check */
    if (!g_WaitingForMgmtResponse)
    {
        // if RAW index previously set out of range and caller is trying to do illegal read
        if ( (rawId==RAW_RX_ID)         && 
              g_rxIndexSetBeyondBuffer  && 
              (GetRawWindowState(RAW_RX_ID) == WF_RAW_DATA_MOUNTED) ) 
        {
            WF_ASSERT(FALSE);  /* attempting to read past end of RAW buffer */
        }
    }

    regId = (rawId==RAW_ID_0)?RAW_0_DATA_REG:RAW_1_DATA_REG;
    ReadWFArray(regId, pBuffer, length);

#if defined(OUTPUT_RAW_TX_RX)
    for (i = 0; i < length; ++i)
    {
        char buf[16];
        sprintf(buf,"R: %#x\r\n", pBuffer[i]);
        putsUART(buf);
    }    
#endif

}


/*****************************************************************************
 * FUNCTION: RawSetByte
 *
 * RETURNS: None
 *
 * PARAMS:
 *      rawId   - RAW ID
 *      pBuffer - Buffer containing bytes to write
 *      length  - number of bytes to read
 *
 *  NOTES: Writes bytes to RAW window
 *****************************************************************************/
void RawSetByte(UINT16 rawId, UINT8 *pBuffer, UINT16 length)
{
    UINT8 regId;
#if defined(OUTPUT_RAW_TX_RX)
    UINT16 i;
#endif    


    /* if previously set index past legal range and now trying to write to RAW engine */
    if ( (rawId == 0) && g_rxIndexSetBeyondBuffer && (GetRawWindowState(RAW_TX_ID) == WF_RAW_DATA_MOUNTED) )
    {
        //WF_ASSERT(FALSE);  /* attempting to write past end of RAW window */
    }

    /* write RAW data to chip */
    regId = (rawId==RAW_ID_0)?RAW_0_DATA_REG:RAW_1_DATA_REG;
    WriteWFArray(regId, pBuffer, length);

#if defined(OUTPUT_RAW_TX_RX)
    for (i = 0; i < length; ++i)
    {
        char buf[16];
        sprintf(buf,"T: %#x\r\n", pBuffer[i]);
        putsUART(buf);
    }    
#endif

}

#if defined (__18CXX)
/*****************************************************************************
 * FUNCTION: RawSetByteROM
 *
 * RETURNS: True if successful, else FALSE
 *
 * PARAMS:
 *      rawId   - RAW ID
 *      pBuffer - Buffer containing bytes to write
 *      length  - number of bytes to read
 *
 *  NOTES: Reads bytes from the RAW engine.  Same as RawSetByte except
 *         using a ROM pointer instead of RAM pointer
 *****************************************************************************/
void RawSetByteROM(UINT16 rawId, ROM UINT8 *pBuffer, UINT16 length)
{
    UINT8 regId;

    regId = (rawId==RAW_ID_0)?RAW_0_DATA_REG:RAW_1_DATA_REG;
    WriteWFROMArray(regId, pBuffer, length);
}
#endif

/*****************************************************************************
 * FUNCTION: WaitForRawMoveComplete
 *
 * RETURNS: Number of bytes that were overlayed (not always applicable)
 *
 * PARAMS:
 *      rawId   - RAW ID
 *
 *  NOTES: Waits for a RAW move to complete.
 *****************************************************************************/
static UINT16 WaitForRawMoveComplete(UINT8 rawId)

{
    UINT8  rawIntMask;
    UINT16 byteCount;
    UINT8  regId;
    BOOL  intDisabled;
    #if defined(WF_DEBUG)
    UINT32 startTickCount;
    UINT32 maxAllowedTicks;
    #endif

    /* create mask to check against for Raw Move complete interrupt for either RAW0 or RAW1 */
    rawIntMask = (rawId == RAW_ID_0)?WF_HOST_INT_MASK_RAW_0_INT_0:WF_HOST_INT_MASK_RAW_1_INT_0;

    /* 
    These variables are shared with the ISR so need to be careful when setting them.
    the WFEintHandler() is the isr that will touch these variables but will only touch
    them if RawMoveState.waitingForRawMoveCompleteInterrupt is set to TRUE.
    RawMoveState.waitingForRawMoveCompleteInterrupt is only set TRUE here and only here.
    so as long as we set RawMoveState.rawInterrupt first and then set RawMoveState.waitingForRawMoveCompleteInterrupt 
    to TRUE, we are guranteed that the ISR won't touch RawMoveState.rawInterrupt and 
    RawMoveState.waitingForRawMoveCompleteInterrupt. 
    */
    RawMoveState.rawInterrupt  = 0;  
    RawMoveState.waitingForRawMoveCompleteInterrupt = TRUE;
    
    // save state of external interrupt here
    intDisabled = WF_EintIsDisabled();
    // if external interrupt is disabled, enable it because we need it for the while(1) loop to exit
    if(intDisabled)
    {
        WF_EintEnable();
    }
    else if(WF_EintIsPending())
    {
        WF_EintEnable();
    }

    #if defined(WF_DEBUG)
    // Before we enter the while loop, get the tick timer count and save it
    maxAllowedTicks = TICKS_PER_SECOND / 2;  /* 500 ms timeout */
    startTickCount = (UINT32)TickGet();
    #endif
    while (1)
    {
        /* if received an external interrupt that signalled the RAW Move */
        /* completed then break out of this loop                         */
        if(RawMoveState.rawInterrupt & rawIntMask)
        {
            break;
        }
        
        #if defined(WF_DEBUG)
        /* If timed out waiting for RAW Move complete than lock up */
        if (TickGet() - startTickCount >= maxAllowedTicks)
        {
            WF_ASSERT(FALSE);
        }
        #endif
        
    } /* end while */

    /* if interrupt was enabled by us here, we should disable it now that we're finished */
    if(intDisabled)
    {
        WF_EintDisable();
    }

    /* read the byte count and return it */
    regId = (rawId == RAW_ID_0)?WF_HOST_RAW0_CTRL1_REG:WF_HOST_RAW1_CTRL1_REG;
    byteCount = Read16BitWFRegister(regId); 

    return ( byteCount );
}




/*****************************************************************************
 * FUNCTION: SendRAWDataFrame
 *
 * RETURNS: TRUE or FALSE
 *
 * PARAMS:
 *      UINT8* pBuf -> pointer to the command buffer.
 *      UINT16 bufLen -> length in bytes of the buffer (pBuf).
 *
 *
 *  NOTES: SendRAWDataFrame sends a Data Transmit request to the WF chip
 *          using the Random Access Window (RAW) interface.  The pre-buffer
 *          is used by the WF MAC to send routing information for the packet
 *          while pBuf is the request that was submitted by the application.
 *          The order of operations are
 *              1) reserve a memory buffer of sufficient length on the WF chip
 *              using RawMove.
 *              2) Write the bytes for the pre-buffer and then the buffer
 *              using the RawSetByte. Because the bytes are written
 *              sequentially there is no need to call WFRawSetIndex
 *              to adjust the write position.
 *              3) instruct the WF chip that the command is ready for
 *              processing.
 *              4) perform any necessary cleanup.
 *****************************************************************************/
void SendRAWDataFrame(UINT16 bufLen)
{
    UINT8 txDataPreamble[4] = { WF_DATA_REQUEST_TYPE, WF_STD_DATA_MSG_SUBTYPE, 1, 0};

    RawWrite(RAW_TX_ID, 0, sizeof(txDataPreamble), txDataPreamble);
    
    RawSendTxBuffer(bufLen);
}



#else
// dummy func to keep compiler happy when module has no executeable code
void DriverRaw_EmptyFunc(void)
{
}
#endif /* WF_CS_TRIS */


/* EOF */
