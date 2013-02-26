/******************************************************************************

 MRF24W Driver Medium Access Control (MAC) Layer
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFMac.c
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


 Author               Date        Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Michael Palladino    10/13/07    Original
 KO                   31 Oct 2008 Port to PIC24F and PIC32 for TCP/IP stack v4.52
 KH                   19 Jun 2009 Modified MACMemCopyAsync to support TCB to TCB copy
******************************************************************************/

/*
*********************************************************************************************************
*                                           INCLUDES                               
*********************************************************************************************************
*/

#include "TCPIP Stack/WFMac.h"

#if defined(WF_CS_TRIS) 

#include "TCPIP Stack/TCPIP.h"

#if defined( WF_CONSOLE )
#include "TCPIP Stack/WFConsole.h"
#include "IperfApp.h"
#endif 


/*
*********************************************************************************************************
*                                           DEFINES                               
*********************************************************************************************************
*/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_MAC
#endif

#define SNAP_VAL        (0xaa)
#define SNAP_CTRL_VAL   (0x03)
#define SNAP_TYPE_VAL   (0x00)

#define ETHER_IP        (0x00)
#define ETHER_ARP       (0x06)

#if defined( __PIC32MX__ )   
    #define IPL_MASK  ((UINT32)0x3f << 10)
#endif

#define SNAP_SIZE (6)

#define ENC_PREAMBLE_SIZE    (sizeof(ENC_PREAMBLE))
#define ENC_PREAMBLE_OFFSET  (10)

#define WF_RX_PREAMBLE_SIZE   (sizeof(tWFRxPreamble))
#define WF_TX_PREAMBLE_OFFSET (0)

#define WF_TX_PREAMBLE_SIZE   (sizeof(tWFTxPreamble))

#define MCHP_DATA_PACKET_SIZE (4 + MAX_PACKET_SIZE + 4)

//============================================================================
//                                  Rx/Tx Buffer Constants
// Used to correlate former Ethernet packets to MRF24W packets.
//============================================================================
#define ENC_RX_BUF_TO_RAW_RX_BUF_ADJUSTMENT          ((RXSTART + ENC_PREAMBLE_SIZE)   - (ENC_PREAMBLE_OFFSET   + WF_RX_PREAMBLE_SIZE))
#define ENC_TX_BUF_TO_RAW_TX_BUF_ADJUSTMENT          ((TXSTART + WF_TX_PREAMBLE_SIZE) - (WF_TX_PREAMBLE_OFFSET + WF_TX_PREAMBLE_SIZE))
#define ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT    (BASE_SCRATCH_ADDR)

//============================================================================
//                                  RAW Constants
//============================================================================
#define ENC_RD_PTR_ID               (0)
#define ENC_WT_PTR_ID               (1)


/*
*********************************************************************************************************
*                                           LOCAL MACROS                               
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL DATA TYPES                               
*********************************************************************************************************
*/
#if defined( WF_CONSOLE_IFCFGUTIL )
extern tWFHibernate WF_hibernate;
#endif

typedef struct
{
    UINT8 type;
    UINT8 subType;
} tRxPreamble;

/* A header appended at the start of all RX frames by the hardware */
typedef struct _ENC_PREAMBLE
{
    WORD            NextPacketPointer;
    RXSTATUS        StatusVector;
    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Type;
} ENC_PREAMBLE;

typedef struct
{
    UINT8         snap[SNAP_SIZE];
    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Type;
} tWFRxPreamble;


typedef struct
{
    UINT8  reserved[4];
} tWFTxPreamble;




/*
*********************************************************************************************************
*                                           LOCAL GLOBAL VARIABLES                               
*********************************************************************************************************
*/

static BOOL   g_wasDiscarded;

static UINT8  g_encPtrRAWId[2];  /* indexed by ENC_RD_PTR_ID (0) and ENC_WT_PTR_ID (1).  Values can be:           */
                                 /*   RAW_RX_ID, RAW_TX_ID, BACKE_TCB_ADDR, RAW_INVALID_ID                        */

/* keeps track of ENC read and write indexes */
static UINT16 g_encIndex[2];       /* index 0 stores current ENC read index, index 1 stores current ENC write index */

static UINT16 g_rxBufferSize;
static UINT16 g_txPacketLength;
static BOOL   g_txBufferFlushed;
static BOOL   g_mgmtRxInProgress = FALSE;
static BOOL   g_mgmtAppWaiting = FALSE;
static UINT16 g_sizeofScratchMemory = 0;
BOOL g_rxIndexSetBeyondBuffer;        // debug -- remove after test


/*
*********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES                               
*********************************************************************************************************
*/

static BOOL isMgmtTxBufAvailable(void);
static UINT16 MACIFService(void);

#if defined ( WF_CONSOLE ) && defined ( EZ_CONFIG_SCAN ) && !defined(__18CXX)
extern void WFDisplayScanMgr(void);
#endif

extern void WF_Connect(void);

/*****************************************************************************
 * FUNCTION: SyncENCPtrRAWState
 *
 * RETURNS: None
 *
 * PARAMS:
 *          encPtrId -- Identifies if trying to do a read or write to ENC RAM (RAW Window).
 *                      Values are ENC_RD_PTR_ID, ENC_WT_PTR_ID.  g_encIndex[] must be
 *                      valid before this function is called.
 *
 *  NOTES: Any time stack code changes the index within the 'logical' Ethernet RAM
 *         this function must be called to assure the RAW driver is synced up with
 *         where the stack code thinks it is within the Ethernet RAM.  This applies
 *         to reading/writing tx data, rx data, or tcb data
 *****************************************************************************/
static void SyncENCPtrRAWState(UINT8 encPtrId)
{
    UINT8     rawId;
    UINT16    rawIndex;
    UINT16    byteCount;
    UINT32    startTickCount;
    UINT32    maxAllowedTicks;

    EnsureWFisAwake();

    /*----------------------------------------------------*/
    /* if encPtr[encPtrId] in the enc rx or enc tx buffer */
    /*----------------------------------------------------*/
    if ( g_encIndex[encPtrId] < BASE_SCRATCH_ADDR/*BASE_TCB_ADDR*/ )
    {
        /*--------------------------------------*/
        /* if encPtr[encPtrId] in enc Rx buffer */
        /*--------------------------------------*/
        if ( g_encIndex[encPtrId] < TXSTART )
        {
            /* set the rawId */
            rawId = RAW_RX_ID;

            /* Convert encPtr index to Raw Index */
            rawIndex = g_encIndex[encPtrId] - ENC_RX_BUF_TO_RAW_RX_BUF_ADJUSTMENT;

            // encPtr[encPtrId] < (RXSTART + ENC_PREAMBLE_SIZE) is an error since we don't have
            // the same preamble as the ENC chip
            WF_ASSERT( g_encIndex[encPtrId] >= (RXSTART + ENC_PREAMBLE_SIZE) );
        }
        /*----------------------------------------*/
        /* else encPtr[encPtrId] in enc Tx buffer */
        /*----------------------------------------*/
        else
        {
            /* if the Tx data raw window has not yet been allocated (the stack is accessing a new Tx data packet) */
            if ( !RawWindowReady[RAW_TX_ID] )
            {
                /* Before we enter the while loop, get the tick timer count and save it */
                maxAllowedTicks = TICKS_PER_SECOND * 6;  /* 2 second timeout, needed if data traffic and scanning occurring */
                startTickCount = (UINT32)TickGet();
                
                /* Retry until MRF24W has drained it's prior TX pkts -  multiple sockets & flows can load the MRF24W10 */
                /* The AllocateDataTxBuffer call may not succeed immediately.                                              */
                while ( !AllocateDataTxBuffer(MCHP_DATA_PACKET_SIZE) )
                {
                    /* If timed out than lock up */
                    if (TickGet() - startTickCount >= maxAllowedTicks)
                    {
                        WF_ASSERT(FALSE);  /* timeout occurred */
                    }
                } /* end while */
            } 

            /* set the rawId */
            rawId = RAW_TX_ID;

            /* convert enc Ptr index to raw index */
            rawIndex = g_encIndex[encPtrId] - ENC_TX_BUF_TO_RAW_TX_BUF_ADJUSTMENT;

            /* encPtr[encPtrId] < BASE_TX_ADDR is an error since we don't have the same  */
            /* pre-BASE_TX_ADDR or post tx buffer as the ENC chip                        */
            WF_ASSERT((g_encIndex[encPtrId] >= BASE_TX_ADDR) && (g_encIndex[encPtrId] <= (BASE_TX_ADDR + MAX_PACKET_SIZE)));
        }


        /* Buffer should always be ready and enc pointer should always be valid. */
        /* For the RX buffer this assert can only happen before we have received */
        /* the first data packet.  For the TX buffer this could only be the case */
        /* after a macflush() where we couldn't re-mount a new buffer and before */
        /* a macistxready() that successfully re-mounts a new tx buffer.         */
        WF_ASSERT(RawWindowReady[rawId]);

        /*-----------------------------------------------------------------------------*/
        /* if the RAW buffer is ready but not mounted, or to put it another way, if    */
        /* the RAW buffer was saved, needs to be restored, and it is OK to restore it. */
        /*-----------------------------------------------------------------------------*/
        if (GetRawWindowState(rawId) != WF_RAW_DATA_MOUNTED) 
        {
            /* if the buffer is not mounted then it must be restored from Mem       */
            /* a side effect is that if the scratch buffer was mounted in the raw   */
            /* then it will no longer be mounted                                    */
            byteCount = PopRawWindow(rawId);
            WF_ASSERT(byteCount > 0);

           // set the buffer state
           SetRawWindowState(rawId, WF_RAW_DATA_MOUNTED); 
        }
    } 
    /*------------------------------------------------*/
    /* else encPtr[encPtrId] is in the enc tcb buffer */
    /*------------------------------------------------*/
    else
    {
        /* if Rx Raw Window already mounted onto Scratch */
        if ( GetRawWindowState(RAW_RX_ID) == WF_SCRATCH_MOUNTED )
        {
            rawId = RAW_RX_ID;
        }
        /* else if Tx Raw Window already mounted onto Scratch */
        else if (GetRawWindowState(RAW_TX_ID) == WF_SCRATCH_MOUNTED)
        {
            rawId = RAW_TX_ID;
        }
        /* else Scratch not yet mounted, so need to decide which RAW window to use */
        else
        {
            if ( (g_encPtrRAWId[1 - encPtrId]) == RAW_RX_ID )
            {
                /* use the Tx RAW window to write to scratch */
                rawId = RAW_TX_ID;
            }
            else
            {
                // the other enc pointer is in use in the tx buffer raw or invalid
                // so use the rx buffer raw to mount the scratch buffer

                // set the rawId
                rawId = RAW_RX_ID;
            }

            // if we currently have a buffer mounted then we need to save it
            // need to check for both data and mgmt packets
            if ( (GetRawWindowState(rawId) == WF_RAW_DATA_MOUNTED) || (GetRawWindowState(rawId) == WF_RAW_MGMT_MOUNTED) )
            {
                PushRawWindow(rawId);
            }

            // mount the scratch window in the selected raw
            if (ScratchMount(rawId) == 0)
            {
                /* work-around, somehow the scratch was already mounted to the other raw window */                
                rawId = !rawId;
            }    
        }

        /* convert Enc ptr index to raw index */
        rawIndex = g_encIndex[encPtrId] - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT;
    }


    /* Set RAW index. If false is returned this means that index set beyond end of raw window, which */
    /* is OK so long as no read or write access is attempted, hence the flag setting.                */
    if (RawSetIndex(rawId, rawIndex) == FALSE)
    {
        if ( rawId == RAW_RX_ID )
        {
            g_rxIndexSetBeyondBuffer = TRUE;
        }
    }
    else
    {
        if ( rawId == RAW_RX_ID)
        {
            g_rxIndexSetBeyondBuffer = FALSE;
        }
    }

    // if we fail the set index we should...
    // use a case statement to determine the object that is mounted (rawId==0, could be rxbuffer object or scratch object)
    // (rawId==1, could be txbuffer or scratch object
    // dismount the object in the appropriate manner (rxbuffer ... save operation, txbuffer save operation, scratch save operation)
    // set the index to 0
    // mark the RawWindowState[rawId] = WF_RAW_UNMOUNTED
    // mark the g_encPtrRAWId[encPtrId] = RAW_INVALID_ID


    // set the g_encPtrRAWId
    g_encPtrRAWId[encPtrId] = rawId;

    // if the opposite encPtr was pointing to the raw window
    // that was re-configured by this routine then it is
    // no longer in sync
    if ( g_encPtrRAWId[1-encPtrId] == g_encPtrRAWId[encPtrId] )
    {
        g_encPtrRAWId[1-encPtrId] = RAW_INVALID_ID;
    }
}

#if defined(STACK_CLIENT_MODE) && defined(USE_GRATUITOUS_ARP)
//following is the workaround algorithm for the 11Mbps broadcast bugfix

extern BOOL ARPSendPkt(DWORD SrcIPAddr, DWORD DestIPAddr, BYTE op_req );
int WFArpBroadcastIntervalSec = 5; //interval in seconds, default to 5, can be changed

/*****************************************************************************
 * FUNCTION: WFPeriodicGratuitousArp
 *
 * RETURNS: None
 *
 * PARAMS:
 *          None
 *
 *  NOTES: this is a workaround algorithm for a bug appearing on some APs: they broadcasts
           ARP Request over basic rate at 11Mpbs, that leaves our devices in dark. Here
           we do ARP Request in the beginning for all the memebers in the subnet, then 
           periodically do Gratuitous ARP to keep ourselves alive with the AP
 *****************************************************************************/
void WFPeriodicGratuitousArp(void)
{
    static DWORD oldTime = 0, currTime;
    static BYTE op_req = ARP_OPERATION_REQ;

    if (!MACIsLinked())
    {
        return;
    }    

    currTime = TickGet();
    
    if ( (currTime < oldTime) //wrap-around case
            ||
         ((currTime - oldTime) > WFArpBroadcastIntervalSec*TICK_SECOND)
        )
    {
        op_req = op_req == ARP_OPERATION_REQ ? ARP_OPERATION_RESP : ARP_OPERATION_REQ;
        ARPSendPkt(*(DWORD *)&AppConfig.MyIPAddr, *(DWORD *)&AppConfig.MyIPAddr, op_req );    
        oldTime = currTime;
    }
}
#endif //defined(STACK_CLIENT_MODE) && defined(USE_GRATUITOUS_ARP)

/*****************************************************************************
 * FUNCTION: MACProcess
 *
 * RETURNS: None
 *
 * PARAMS:
 *          None
 *
 *  NOTES: Called form main loop to support 802.11 operations
 *****************************************************************************/
void MACProcess(void)
{
    
    // Let 802.11 processes have a chance to run
    WFProcess();

    #if defined( WF_CONSOLE_IFCFGUTIL )
           if (WF_hibernate.wakeup_notice && WF_hibernate.state == WF_HB_WAIT_WAKEUP) 
         {
            DelayMs(200);
    
            WF_hibernate.state = WF_HB_NO_SLEEP;
            StackInit();
            #if defined(WF_CONSOLE) && !defined(STACK_USE_EZ_CONFIG)
                IperfAppInit();
            #endif
    
            WF_Connect();
        }
    #endif
    
    
    #if defined(WF_CONSOLE_DEMO)
        IperfAppCall();
    #endif

#if !defined (WF_EASY_CONFIG_DEMO)
    #if defined(WF_CONSOLE_IFCFGUTIL) 
    wait_console_input:
    #endif
           
    #if defined(WF_CONSOLE)
        WFConsoleProcess();
        #if defined( WF_CONSOLE_IFCFGUTIL )
            if (WF_hibernate.state == WF_HB_NO_SLEEP)
                IperfAppCall();
            #elif !defined(STACK_USE_EZ_CONFIG)
                IperfAppCall();
            #endif
            WFConsoleProcessEpilogue();
    #endif
    
    #if defined( WF_CONSOLE_IFCFGUTIL )
        if (WF_hibernate.state != WF_HB_NO_SLEEP) 
        {
            if (WF_hibernate.state == WF_HB_ENTER_SLEEP) 
            {
                SetLogicalConnectionState(FALSE);
                #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
                    WF_HibernateEnable();
                #endif
                WF_hibernate.state = WF_HB_WAIT_WAKEUP;
            }
            if (WF_hibernate.wakeup_notice) 
            {
                //continue;
            }    
            else
            {
                goto wait_console_input;
            }                
        }
    #endif             
#endif /* !defined (WF_EASY_CONFIG_DEMO) */
    
    /* SG. Deadlock avoidance code when two applications contend for the one tx pipe                              */
    /* ApplicationA is a data plane application, and applicationB is a control plane application                  */
    /* In this scenario, the data plane application could be the WiFi manager, and the control plane application  */
    /* a sockets application.  If the sockets application keeps doing a IsUDPPutReady() and never follows with    */
    /* a UDPFlush, then the link manager will be locked out.   This would be catescrophic if an AP connection     */
    /* goes down, then the link manager could never re-establish connection.  Why?  The link manager is a control */
    /* plane application, which does mgmt request/confirms.                                                       */
    /*                                                                                              */
    /* Sequence of events:                                                                          */
    /* T0: ApplicationA will issue a call like UDPIsPutReady(), which results in a AllocateDataTxBuffer */
    /* T1: ApplicationB attempts a mgmt request with IsTxMbmtReady() call.  The call fails.         */
    /* T3: Stack process runs and does not deallocate the tx pipe from the data plane application.  */
    /* T4: ApplicationB attempts N+1th time, and fails.                                             */
    if ( g_mgmtAppWaiting )
    {
        if ( GetRawWindowState(RAW_TX_ID) == WF_RAW_DATA_MOUNTED )
        {
            /* deallocate the RAW on MRF24W - return memory to pool */
            DeallocateDataTxBuffer();

            if ( g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_RX_ID )
            {
                g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_INVALID_ID;
            }

            if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_TX_ID )
            {
                g_encPtrRAWId[ENC_WT_PTR_ID] = RAW_INVALID_ID;
            }
        }

        /* This else is important in that it gives the main loop one iteration for the mgmt application to get it's timeslice  */
        /* Otherwise, a data plane task could snatch away the tx pipe again, especially if it's placed before                  */
        /* the link manager in the main()'s while(1) blk.  This code is functionally coupled with isRawRxMgmtInProgress()    */
        /* as it will keep the dataplane application locked out for 1 iteration, until this else is executed on N+2 iteration    */
        else
        {
            g_mgmtAppWaiting = FALSE;
        }
    }

#if defined(STACK_CLIENT_MODE) && defined(USE_GRATUITOUS_ARP)
    //following is the workaround algorithm for the 11Mbps broadcast bugfix
    WFPeriodicGratuitousArp();
#endif     
}

 /******************************************************************************
 * Function:        UINT16 WFGetTCBSize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Number of bytes in the TCB
 *
 * Side Effects:    None
 *
 * Overview:        Returns number of bytes available in TCP Control Block (TCB) so
 *                  higher-layer code can determine if the number of bytes available
 *                  can support the structures designated to be stored in the TCB.
 *
 * Note:            When running with WiFi the TCB is contained in the Scratch Memory
 *                  on the MRF24W.
 *****************************************************************************/
UINT16 WFGetTCBSize(void)
{
    return g_sizeofScratchMemory;
}


 /******************************************************************************
 * Function:        void MACInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACInit sets up the PIC's SPI module and all the
 *                  registers in the MRF24W so that normal operation can
 *                  begin.
 *
 * Note:            None
 *****************************************************************************/
void MACInit(void)
{
    WF_Init();
}

void RawInit(void)
{
    // By default, Scratch is mounted to RAW 1 after reset.  In order to mount it on RAW0
    // we need to first unmount it from RAW 1.
    ScratchUnmount(RAW_TX_ID);

    // mount scratch temporarily to see how many bytes it has, then unmount it
    g_sizeofScratchMemory = ScratchMount(RAW_RX_ID);  /* put back in if need to know size of scratch */
    ScratchUnmount(RAW_RX_ID);

    RawWindowReady[RAW_RX_ID] = FALSE;

    g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_RX_ID;
    g_encIndex[ENC_RD_PTR_ID]    = BASE_SCRATCH_ADDR;//BASE_TCB_ADDR;

    // don't mount tx raw at init because it's needed for raw mgmt messages
    RawWindowReady[RAW_TX_ID]    = FALSE;
    SetRawWindowState(RAW_TX_ID, WF_RAW_UNMOUNTED);

    g_encPtrRAWId[ENC_WT_PTR_ID]  = RAW_INVALID_ID;
    g_encIndex[ENC_WT_PTR_ID]     = BASE_TX_ADDR;                 // set tx encode ptr (index) to start of tx buf + 4 bytes

    g_wasDiscarded    = TRUE;                                     // set state such that last rx packet was discarded
    g_rxBufferSize    = 0;                                        // current rx buffer length (none) is 0 bytes
    g_txPacketLength  = 0;                                        // current tx packet length (none) is 0 bytes
    g_txBufferFlushed = TRUE;                                     // tx buffer is flushed

    // from ENC MAC init
    // encWrPtr is left pointing to BASE_TX_ADDR
    // encRdPtr is not initialized... we leave it pointing to BASE_TCB_ADDR

    g_rxIndexSetBeyondBuffer = FALSE;
}    




/******************************************************************************
 * Function:        BOOL MACIsLinked(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: If the PHY reports that a link partner is present
 *                        and the link has been up continuously since the last
 *                        call to MACIsLinked()
 *                  FALSE: If the PHY reports no link partner, or the link went
 *                         down momentarily since the last call to MACIsLinked()
 *
 * Side Effects:    None
 *
 * Overview:        Returns the PHSTAT1.LLSTAT bit.
 *
 * Note:            None
 *****************************************************************************/
BOOL MACIsLinked(void)
{
    return ( WFisConnected() );
}


/******************************************************************************
 * Function:        BOOL MACIsTxReady(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: If no Ethernet transmission is in progress
 *                  FALSE: If a previous transmission was started, and it has
 *                         not completed yet.  While FALSE, the data in the
 *                         transmit buffer and the TXST/TXND pointers must not
 *                         be changed.
 *
 * Side Effects:    None
 *
 * Overview:        Returns the ECON1.TXRTS bit
 *
 * Note:            None
 *****************************************************************************/
BOOL MACIsTxReady(void)
{
    BOOL result = TRUE;

    /* if waiting for a management response then block data tx until */
    /* mgmt response received                                        */
    if (isRawRxMgmtInProgress())
    {
        WFProcess();   // allow mgmt message to be received (stack can call this
                       // function in an infinite loop so need to allow WiFi state
                       // machines to run.
        return FALSE;
    }
    
    if ( !RawWindowReady[RAW_TX_ID] )
    {
        SetRawWindowState(RAW_TX_ID, WF_RAW_UNMOUNTED);

        if ( g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_TX_ID )
        {
            g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_INVALID_ID;
        }

        if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_TX_ID )
        {
            g_encPtrRAWId[ENC_WT_PTR_ID] = RAW_INVALID_ID;
        }

        // create the new tx buffer
        if (!AllocateDataTxBuffer(MCHP_DATA_PACKET_SIZE) )
        {
            result = FALSE;
        }
    }

    return result;
}

// determines if a RAW Tx buf is ready for a management msg, and if so, creates the RAW tx buffer.
// Returns TRUE if successful, else FALSE.
BOOL WFisTxMgmtReady(void)
{
    BOOL res = TRUE;

    if (isMgmtTxBufAvailable())
    {
        // create and mount tx buffer to hold RAW Mgmt packet
        if (AllocateMgmtTxBuffer(WF_MAX_TX_MGMT_MSG_SIZE))
        {
            res = TRUE;

            /* Bug. This flag must be set otherwise the data path does not know */
            /* that the tx pipe has been mounted for mgmt operation.  SG */
            SetRawRxMgmtInProgress(TRUE); 
        }
        else
        {
            res = FALSE;
        }
    }
    // else Tx RAW not available for Mgmt packet
    else
    {
        res = FALSE;

        /* See comment in MACProcess */
        g_mgmtAppWaiting = TRUE;
    }

    return res;
}


void WFFreeMgmtTx(void)
{
    // This flag needs to clear so data path can proceed.
    SetRawRxMgmtInProgress(FALSE);
}

static BOOL isMgmtTxBufAvailable(void)
{
    // if the Tx RAW buf is not being used for a data packet or scratch, then it
    // is available for a Mgmt packet.
    if ((RawWindowReady[RAW_TX_ID] == FALSE)                   
                                 &&
        ((GetRawWindowState(RAW_TX_ID) == WF_RAW_UNMOUNTED) ||  (GetRawWindowState(RAW_TX_ID) == WF_SCRATCH_MOUNTED)))
    {
        SetRawWindowState(RAW_TX_ID, WF_RAW_UNMOUNTED);
        return TRUE;
    }
    else
    {
        return FALSE;
    }


}

void SendRAWManagementFrame(UINT16 bufLen)
{
    /* Instruct WF chip to transmit the packet data in the raw window */
    RawSendTxBuffer(bufLen);

    /* let tx buffer be reused (for either data or management tx) */
    WFFreeMgmtTx();
}




// returns TRUE if able to acquire the RAW Rx window for the purpose
// of processing a management receive message

BOOL RawGetMgmtRxBuffer(UINT16 *p_numBytes)
{
    BOOL res = TRUE;
   // UINT16  numBytes;
    *p_numBytes = 0;

    // if Raw Rx is not currently mounted, or the Scratch is mounted
    if (GetRawWindowState(RAW_RX_ID) == WF_RAW_DATA_MOUNTED)
    {
        // save whatever was mounted to Raw Rx
        PushRawWindow(RAW_RX_ID); 
    }

    // mount the mgmt pool rx data, returns number of bytes in mgmt msg.  Index
    // defaults to 0.
    *p_numBytes = RawMountRxBuffer();
    
    /* Should never receive a mgmt msg with 0 bytes */
    WF_ASSERT(*p_numBytes > 0);

    // set flag so we do not try to mount an incoming data packet until after the rx Mgmt msg
    // has been handled.
    SetRawRxMgmtInProgress(TRUE);


    return res;
}


void SetRawRxMgmtInProgress(BOOL action)
{
    if (action == FALSE)
    {
//        RawWindowReady[RAW_RX_ID] = TRUE;
        SetRawWindowState(RAW_RX_ID, WF_RAW_UNMOUNTED);
    }

    g_mgmtRxInProgress = action;
}



BOOL isRawRxMgmtInProgress(void)
{
     return g_mgmtRxInProgress;   // RawMgtmAppWaiting flag not needed and was causing problems
}


/******************************************************************************
 * Function:        void MACDiscardRx(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Marks the last received packet (obtained using
 *                  MACGetHeader())as being processed and frees the buffer
 *                  memory associated with it
 *
 * Note:            It is safe to call this function multiple times between
 *                  MACGetHeader() calls.  Extra packets won't be thrown away
 *                  until MACGetHeader() makes it available.
 *****************************************************************************/
void MACDiscardRx(void)
{
    g_wasDiscarded = TRUE;
}


/******************************************************************************
 * Function:        WORD MACGetFreeRxSize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          A WORD estimate of how much RX buffer space is free at
 *                  the present time.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
WORD MACGetFreeRxSize(void)
{
    WORD size;

    if ( g_wasDiscarded )
    {
        size = RXSIZE - 1;
    }
    else
    {
        if ( (RXSTOP - RXSTART) > g_rxBufferSize )
        {
            size = (RXSTOP - RXSTART) - g_rxBufferSize;
        }
        else
        {
            size = 0;
        }
    }

    return size;
}

/*****************************************************************************
 * FUNCTION: MACIFService
 *
 *
 * RETURNS: Number of bytes in the Data Rx packet if one is received, else 0.
 *
 * PARAMS:  None
 *
 *  NOTES: Called by MACGetHeader() to see if any data packets have been received.
 *         If the MRF24W has received a data packet and the data packet is not
 *         a management data packet, then this function returns the number of
 *         bytes in the data packet. Otherwise it returns 0.
 *****************************************************************************/
static UINT16 MACIFService(void)
{
    UINT16 byteCount = 0; /* num bytes returned */
    tRxPreamble wfPreamble;

    // if no rx data packet to process or not yet finished with mgmt rx processing
    if (!g_HostRAWDataPacketReceived)
    {
        return byteCount;
    }
    
    /* if made it here then External interrupt has signalled a data packet has been received */
    
    
    g_HostRAWDataPacketReceived = FALSE; /* clear flag for next data packet */

    /* Mount Read FIFO to RAW Rx window.  Allows use of RAW engine to read rx data packet. */
    /* Function call returns number of bytes in the data packet.                           */
    byteCount = RawMountRxBuffer();
    WF_ASSERT(byteCount > 0);         /* byte count should never be 0 */
    
    // now that buffer mounted it is safe to reenable interrupts
    WF_EintEnable();

    RawGetByte(RAW_RX_ID, (UINT8*)&wfPreamble, sizeof(tRxPreamble));
    WF_ASSERT(wfPreamble.type == WF_DATA_RX_INDICATE_TYPE);

    return byteCount;
}

 
/******************************************************************************
 * Function:        BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
 *
 * PreCondition:    None
 *
 * Input:           *remote: Location to store the Source MAC address of the
 *                           received frame.
 *                  *type: Location of a BYTE to store the constant
 *                         MAC_UNKNOWN, ETHER_IP, or ETHER_ARP, representing
 *                         the contents of the Ethernet type field.
 *
 * Output:          TRUE: If a packet was waiting in the RX buffer.  The
 *                        remote, and type values are updated.
 *                  FALSE: If a packet was not pending.  remote and type are
 *                         not changed.
 *
 * Side Effects:    Last packet is discarded if MACDiscardRx() hasn't already
 *                  been called.
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
{
    UINT16 len;
    tWFRxPreamble header;
    
    g_rxIndexSetBeyondBuffer = FALSE;

    /* if we currently have a rx buffer mounted then we need to save it */
    if ( GetRawWindowState(RAW_RX_ID) == WF_RAW_DATA_MOUNTED )
    {
        /* save state of Rx RAW window */
        PushRawWindow(RAW_RX_ID);
    }

    /* RAW 0 is now unmounted (and available) */
    SetRawWindowState(RAW_RX_ID, WF_RAW_UNMOUNTED);

    if ( g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_RX_ID )
    {
        g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_INVALID_ID;
    }

    if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_RX_ID )
    {
        g_encPtrRAWId[ENC_WT_PTR_ID] = RAW_INVALID_ID;
    }

    len = MACIFService();
    if ( len == 0 )
    {
        return FALSE;
    }

    /* read preamble header */
    RawRead(RAW_RX_ID, ENC_PREAMBLE_OFFSET, WF_RX_PREAMBLE_SIZE, (UINT8 *)&header);  

    /* as a sanity check verify that the expected bytes contain the SNAP header */
    if (!(header.snap[0] == SNAP_VAL        && 
          header.snap[1] == SNAP_VAL        &&
          header.snap[2] == SNAP_CTRL_VAL   &&
          header.snap[3] == SNAP_TYPE_VAL   && 
          header.snap[4] == SNAP_TYPE_VAL   && 
          header.snap[5] == SNAP_TYPE_VAL) )
    {
        /* if a vendor proprietary packet, throw away */
        DeallocateDataRxBuffer();
        return FALSE;
    }
  
    // Make absolutely certain that any previous packet was discarded
    g_wasDiscarded = TRUE;

    /* we can flush any saved RAW state now by saving and restoring the current rx buffer.  */
    PushRawWindow(RAW_RX_ID);
    PopRawWindow(RAW_RX_ID); 
    
    // set RAW pointer to 802.11 payload
    RawSetIndex(RAW_RX_ID, (ENC_PREAMBLE_OFFSET + WF_RX_PREAMBLE_SIZE));
    
    g_rxBufferSize = len;
    /////    RawWindowReady[RAW_RX_ID] = TRUE;
    /////SetRawWindowState(RAW_RX_ID, WF_RAW_DATA_MOUNTED);   
    g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_RX_ID;
    g_encIndex[ENC_RD_PTR_ID]    = RXSTART + sizeof(ENC_PREAMBLE);

    // The EtherType field, like most items transmitted on the Ethernet medium
    // are in big endian.
    header.Type.Val = swaps(header.Type.Val);

    // Return the Ethernet frame's Source MAC address field to the caller
    // This parameter is useful for replying to requests without requiring an
    // ARP cycle.
    memcpy((void*)remote->v, (void*)header.SourceMACAddr.v, sizeof(*remote));

    // Return a simplified version of the EtherType field to the caller
    *type = MAC_UNKNOWN;
    if( (header.Type.v[1] == 0x08u) &&
        ((header.Type.v[0] == ETHER_IP) || (header.Type.v[0] == ETHER_ARP)) )
    {
        *type = header.Type.v[0];
    }

    // Mark this packet as discardable
    g_wasDiscarded = FALSE;

    return TRUE;
}

/******************************************************************************
 * Function:        void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen)
 *
 * PreCondition:    MACIsTxReady() must return TRUE.
 *
 * Input:           *remote: Pointer to memory which contains the destination
 *                           MAC address (6 bytes)
 *                  type: The constant ETHER_ARP or ETHER_IP, defining which
 *                        value to write into the Ethernet header's type field.
 *                  dataLen: Length of the Ethernet data payload
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            Because of the dataLen parameter, it is probably
 *                  advantagous to call this function immediately before
 *                  transmitting a packet rather than initially when the
 *                  packet is first created.  The order in which the packet
 *                  is constructed (header first or data first) is not
 *                  important.
 *****************************************************************************/
void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen)
{
    UINT8 buf[14];
    g_txBufferFlushed = FALSE;
    g_txPacketLength = dataLen + (WORD)sizeof(ETHER_HEADER) + WF_TX_PREAMBLE_SIZE;

    // Set the SPI write pointer to the beginning of the transmit buffer (post WF_TX_PREAMBLE_SIZE)
    g_encIndex[ENC_WT_PTR_ID] = TXSTART + WF_TX_PREAMBLE_SIZE;
    SyncENCPtrRAWState(ENC_WT_PTR_ID);

    /*  write the Ethernet destination address to buffer (6 bytes) */
    memcpy(&buf[0], (void *)remote, sizeof(*remote));
    /* write snap header to buffer (6 bytes) */
    buf[6] =  SNAP_VAL;         
    buf[7] =  SNAP_VAL;
    buf[8] =  SNAP_CTRL_VAL;
    buf[9] =  SNAP_TYPE_VAL;
    buf[10] = SNAP_TYPE_VAL;
    buf[11] = SNAP_TYPE_VAL;
    /* Write the appropriate Ethernet Type WORD for the protocol being used */
    buf[12] = 0x08;                 
    buf[13] = (type == MAC_IP) ? ETHER_IP : ETHER_ARP;

    /* write buffer to RAW window */
    MACPutArray((BYTE *)buf, sizeof(buf));
}



/******************************************************************************
 * Function:        void MACFlush(void)
 *
 * PreCondition:    A packet has been created by calling MACPut() and
 *                  MACPutHeader().
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACFlush causes the current TX packet to be sent out on
 *                  the Ethernet medium.  The hardware MAC will take control
 *                  and handle CRC generation, collision retransmission and
 *                  other details.
 *
 * Note:            After transmission completes (MACIsTxReady() returns TRUE),
 *                  the packet can be modified and transmitted again by calling
 *                  MACFlush() again.  Until MACPutHeader() or MACPut() is
 *                  called (in the TX data area), the data in the TX buffer
 *                  will not be corrupted.
 *****************************************************************************/
void MACFlush(void)
{
    /* this function should not be called if no tx buffer is ready to transmit */
    WF_ASSERT(RawWindowReady[RAW_TX_ID]);
   
    /* this function should not be called after the current tx buffer has been transmitted */
    WF_ASSERT(!g_txBufferFlushed);

    g_txBufferFlushed = TRUE;

    /* If the RAW engine is not currently mounted */
    if ( GetRawWindowState(RAW_TX_ID) != WF_RAW_DATA_MOUNTED )
    {
        /* then it must have been saved, so pop it */
        PopRawWindow(RAW_TX_ID);
    }

    // at this point the txbuffer should be mounted and ready to go

    /* can't send a tx packet of 0 bytes! */
    WF_ASSERT(g_txPacketLength != 0);

     /* Ensure the MRF24W is awake (only applies if PS-Poll was enabled) */
    EnsureWFisAwake();
    SendRAWDataFrame(g_txPacketLength);
    
    // make sure to de-sync any affected pointers
    if ( g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_TX_ID )
    {
        g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_INVALID_ID;
    }

    if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_TX_ID )
    {
        g_encPtrRAWId[ENC_WT_PTR_ID] = RAW_INVALID_ID;
    }
}


/******************************************************************************
 * Function:        void MACSetReadPtrInRx(WORD offset)
 *
 * PreCondition:    A packet has been obtained by calling MACGetHeader() and
 *                  getting a TRUE result.
 *
 * Input:           offset: WORD specifying how many bytes beyond the Ethernet
 *                          header's type field to relocate the SPI read
 *                          pointer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        SPI read pointer are updated.  All calls to
 *                  MACGet() and MACGetArray() will use these new values.
 *
 * Note:            RXSTOP must be statically defined as being > RXSTART for
 *                  this function to work correctly.  In other words, do not
 *                  define an RX buffer which spans the 0x1FFF->0x0000 memory
 *                  boundary.
 *****************************************************************************/
void MACSetReadPtrInRx(WORD offset)
{
    g_encIndex[ENC_RD_PTR_ID] = RXSTART + sizeof(ENC_PREAMBLE) + offset;
    SyncENCPtrRAWState(ENC_RD_PTR_ID);
}


/******************************************************************************
 * Function:        WORD MACSetWritePtr(WORD Address)
 *
 * PreCondition:    None
 *
 * Input:           Address: Address to seek to
 *
 * Output:          WORD: Old EWRPT location
 *
 * Side Effects:    None
 *
 * Overview:        SPI write pointer is updated.  All calls to
 *                  MACPut() and MACPutArray() will use this new value.
 *
 * Note:            None
 *****************************************************************************/
PTR_BASE MACSetWritePtr(PTR_BASE address)
{
    PTR_BASE oldVal;

    oldVal = g_encIndex[ENC_WT_PTR_ID];

    g_encIndex[ENC_WT_PTR_ID] = address;

    SyncENCPtrRAWState(ENC_WT_PTR_ID);

    return oldVal;
}

/******************************************************************************
 * Function:        WORD MACSetReadPtr(WORD Address)
 *
 * PreCondition:    None
 *
 * Input:           Address: Address to seek to
 *
 * Output:          WORD: Old ERDPT value
 *
 * Side Effects:    None
 *
 * Overview:        SPI write pointer is updated.  All calls to
 *                  MACPut() and MACPutArray() will use this new value.
 *
 * Note:            None
 *****************************************************************************/
PTR_BASE MACSetReadPtr(PTR_BASE address)
{
    PTR_BASE oldVal;

    oldVal = g_encIndex[ENC_RD_PTR_ID];

    g_encIndex[ENC_RD_PTR_ID] = address;
    SyncENCPtrRAWState(ENC_RD_PTR_ID);

    return oldVal;
}


/******************************************************************************
 * Function:        WORD MACCalcRxChecksum(WORD offset, WORD len)
 *
 * PreCondition:    None
 *
 * Input:           offset  - Number of bytes beyond the beginning of the
 *                          Ethernet data (first byte after the type field)
 *                          where the checksum should begin
 *                  len     - Total number of bytes to include in the checksum
 *
 * Output:          16-bit checksum as defined by RFC 793.
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a checksum calculation in the MAC
 *                  buffer itself
 *
 * Note:            None
 *****************************************************************************/
WORD MACCalcRxChecksum(WORD offset, WORD len)
{
    WORD temp;
    UINT16 rdSave;

    // Add the offset requested by firmware plus the Ethernet header
    temp = RXSTART + sizeof(ENC_PREAMBLE) + offset;

    rdSave = g_encIndex[ENC_RD_PTR_ID];

    g_encIndex[ENC_RD_PTR_ID] = temp;
    SyncENCPtrRAWState(ENC_RD_PTR_ID);

    temp = CalcIPBufferChecksum(len);

    g_encIndex[ENC_RD_PTR_ID] = rdSave;
    SyncENCPtrRAWState(ENC_RD_PTR_ID);

    return temp;
}


/******************************************************************************
 * Function:        void MACMemCopyAsync(WORD destAddr, WORD sourceAddr, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           destAddr:   Destination address in the Ethernet memory to
 *                              copy to.  If the MSb is set, the current EWRPT
 *                              value will be used instead.
 *                  sourceAddr: Source address to read from.  If the MSb is
 *                              set, the current ERDPT value will be used
 *                              instead.
 *                  len:        Number of bytes to copy
 *
 * Output:          Byte read from the MRF24W's RAM
 *
 * Side Effects:    None
 *
 * Overview:        Bytes are asynchrnously transfered within the buffer.  Call
 *                  MACIsMemCopyDone() to see when the transfer is complete.
 *
 * Note:            If a prior transfer is already in progress prior to
 *                  calling this function, this function will block until it
 *                  can start this transfer.
 *
 *                  If a negative value is used for the sourceAddr or destAddr
 *                  parameters, then that pointer will get updated with the
 *                  next address after the read or write.
 *****************************************************************************/
void MACMemCopyAsync(PTR_BASE destAddr, PTR_BASE sourceAddr, WORD len)
{
    UINT16 readSave = 0, writeSave = 0;
    BOOL updateWritePointer;
    BOOL updateReadPointer;
    UINT8 rawScratchId = 0xff;  /* garbage value to avoid compiler warning */
    UINT8 copyBuf[8];
    UINT16 writeIndex, readIndex;
    UINT16 bytesLeft;
    UINT16 origRawIndex;


    EnsureWFisAwake();   
    
    if( ((WORD_VAL*)&destAddr)->bits.b15 )
    {
        updateWritePointer = TRUE;
        destAddr = g_encIndex[ENC_WT_PTR_ID];
        if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_INVALID_ID )
        {
            SyncENCPtrRAWState(ENC_WT_PTR_ID);
        }
    }
    else
    {
        updateWritePointer = FALSE;
        writeSave = g_encIndex[ENC_WT_PTR_ID];
        g_encIndex[ENC_WT_PTR_ID] = destAddr;
        SyncENCPtrRAWState(ENC_WT_PTR_ID);
    }

    if( ((WORD_VAL*)&sourceAddr)->bits.b15 )
    {
        updateReadPointer = TRUE;
        sourceAddr = g_encIndex[ENC_RD_PTR_ID];
        if ( g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_INVALID_ID )
        {
            SyncENCPtrRAWState(ENC_RD_PTR_ID);
        }
    }
    else
    {
        updateReadPointer = FALSE;
        readSave = g_encIndex[ENC_RD_PTR_ID];
        g_encIndex[ENC_RD_PTR_ID] = sourceAddr;
        SyncENCPtrRAWState(ENC_RD_PTR_ID);
    }
    
    // if copying bytes from TCB to TCB
    // This is a special case because we cannot do a RAW copy within the same RAW window
    // but we can easily copy Scratch data from one section of Scratch to another section of Scratch.
    if ( (len > 0u) && (destAddr >= BASE_SCRATCH_ADDR/*BASE_TCB_ADDR*/) && (sourceAddr >= BASE_SCRATCH_ADDR/*BASE_TCB_ADDR*/) )
    {
        bytesLeft = len;
        
        // if Raw Rx window mounted to scratch
        if (GetRawWindowState(RAW_RX_ID) == WF_SCRATCH_MOUNTED)
        {
            rawScratchId = RAW_RX_ID;
        }
        // else if Raw Tx window mounted to scratch
        else if (GetRawWindowState(RAW_TX_ID) == WF_SCRATCH_MOUNTED)   
        {
            rawScratchId = RAW_TX_ID;
        }
        else
        {
            WF_ASSERT(FALSE);  /* this should never happen (that can't mount scratch on either RAW window) */
        }         
        
        // save the current RAW index in this scratch window
        origRawIndex = RawGetIndex(rawScratchId);
        
        // If TCB src block does not overlap TCB dest block, or if destAddr > sourceAddr.
        // We can do a forward copy.
        if ( ((sourceAddr + len) <= destAddr) ||    // end of source before dest  (no overlap)
             ((destAddr + len) <= sourceAddr) ||    // end of dest before source  (no overlap)
              (destAddr < sourceAddr)               // dest before source (overlap)              
           )
        {
           
            // map read index from TCB address to Scratch Index
            readIndex  = sourceAddr - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT;
            writeIndex = destAddr   - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT;
            
            while (bytesLeft > 0)
            {
                // if a full copyBuf worth of bytes to copy
                if (bytesLeft >= sizeof(copyBuf))
                {
                    RawRead(rawScratchId, readIndex, sizeof(copyBuf), copyBuf);
                    RawWrite(rawScratchId, writeIndex, sizeof(copyBuf), copyBuf);

                    // index to next block in source and dest
                    readIndex  += sizeof(copyBuf);
                    writeIndex += sizeof(copyBuf);
                    bytesLeft  -= sizeof(copyBuf);
                }
                // else less than a full copyBuf left to copy
                else
                {
                    if (bytesLeft > 0)
                    {
                        RawRead(rawScratchId, readIndex, bytesLeft, copyBuf);
                        RawWrite(rawScratchId, writeIndex, bytesLeft, copyBuf);
                        bytesLeft = 0;
                    }    
                }        
            }    
        } // end while
        // else start of TCB dest block within TCB src block --> destAddr > sourcAddr
        // Do a backward copy.
        else if (destAddr > sourceAddr)
        {
            // map read index from TCB address to Scratch Index
            readIndex  = sourceAddr - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT + len - 1;
            writeIndex = destAddr   - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT + len - 1;
            
            while (bytesLeft > 0)
            {
                // if a full copyBuf worth of bytes to copy
                if (bytesLeft >= sizeof(copyBuf))
                {
                    RawRead(rawScratchId,  readIndex  - sizeof(copyBuf) + 1, sizeof(copyBuf), copyBuf);
                    RawWrite(rawScratchId, writeIndex - sizeof(copyBuf) + 1, sizeof(copyBuf), copyBuf);

                    // index to next block in source and dest
                    readIndex  -= sizeof(copyBuf);
                    writeIndex -= sizeof(copyBuf);
                    bytesLeft  -= sizeof(copyBuf);
                }
                // else less than a full copyBuf left to copy
                else
                {
                    if (bytesLeft > 0)
                    {
                        RawRead(rawScratchId, readIndex - bytesLeft + 1, bytesLeft - 1, copyBuf);
                        RawWrite(rawScratchId, writeIndex - bytesLeft + 1, bytesLeft - 1, copyBuf);
                        bytesLeft = 0;
                    }    
                }        
            } // end while    
        }    
        // restore raw index to where it was when this function was called
        RawSetIndex(rawScratchId, origRawIndex); 

    } 
    // else if not copying from TCB to TCB and there is at least one byte to copy
    else if ( len > 0)
    {
        // Check if app is trying to copy data within same RAW window (can't do that)
        if ( (g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_INVALID_ID) ||
             (g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_INVALID_ID) )
        {
            WF_ASSERT(FALSE);
        }
        RawToRawCopy(g_encPtrRAWId[ENC_WT_PTR_ID], len);
    }

    if ( !updateReadPointer )
    {
        g_encIndex[ENC_RD_PTR_ID] = readSave;
        SyncENCPtrRAWState(ENC_RD_PTR_ID);
    }

    if ( !updateWritePointer )
    {
        g_encIndex[ENC_WT_PTR_ID] = writeSave;
        SyncENCPtrRAWState(ENC_WT_PTR_ID);
    }
    
    
} /* end MACMemCopyAsync */


BOOL MACIsMemCopyDone(void)
{
    return TRUE;
}


/******************************************************************************
 * Function:        BYTE MACGet()
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  ERDPT must point to the place to read from.
 *
 * Input:           None
 *
 * Output:          Byte read from the MRF24W's RAM
 *
 * Side Effects:    None
 *
 * Overview:        MACGet returns the byte pointed to by ERDPT and
 *                  increments ERDPT so MACGet() can be called again.  The
 *                  increment will follow the receive buffer wrapping boundary.
 *
 * Note:            None
 *****************************************************************************/
BYTE MACGet()
{
    BYTE result;

    if ( g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_INVALID_ID )
    {
        SyncENCPtrRAWState(ENC_RD_PTR_ID);
    }

    RawGetByte(g_encPtrRAWId[ENC_RD_PTR_ID], &result, 1);

    g_encIndex[ENC_RD_PTR_ID] += 1;

    return result;
}//end MACGet


/******************************************************************************
 * Function:        WORD MACGetArray(BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  ERDPT must point to the place to read from.
 *
 * Input:           *val: Pointer to storage location
 *                  len:  Number of bytes to read from the data buffer.
 *
 * Output:          Byte(s) of data read from the data buffer.
 *
 * Side Effects:    None
 *
 * Overview:        Burst reads several sequential bytes from the data buffer
 *                  and places them into local memory.  With SPI burst support,
 *                  it performs much faster than multiple MACGet() calls.
 *                  ERDPT is incremented after each byte, following the same
 *                  rules as MACGet().
 *
 * Note:            None
 *****************************************************************************/
WORD MACGetArray(BYTE *val, WORD len)
{
    WORD i = 0;
    UINT8 byte;

    if ( g_encPtrRAWId[ENC_RD_PTR_ID] == RAW_INVALID_ID )
    {
        SyncENCPtrRAWState(ENC_RD_PTR_ID);
    }

    if ( val )
    {
        RawGetByte(g_encPtrRAWId[ENC_RD_PTR_ID], val, len);
    }
    else
    {
        // Read the data
        while(i<len)
        {
            RawGetByte(g_encPtrRAWId[ENC_RD_PTR_ID], &byte, 1);
            i++;
        }
    }
    g_encIndex[ENC_RD_PTR_ID] += len;

    return len;
}//end MACGetArray


/******************************************************************************
 * Function:        void MACPut(BYTE val)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  EWRPT must point to the location to begin writing.
 *
 * Input:           Byte to write into the MRF24W buffer memory
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPut outputs the Write Buffer Memory opcode/constant
 *                  (8 bits) and data to write (8 bits) over the SPI.
 *                  EWRPT is incremented after the write.
 *
 * Note:            None
 *****************************************************************************/
void MACPut(BYTE val)
{
    UINT8 byte;
    
    byte = val;

    if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_INVALID_ID )
    {
        SyncENCPtrRAWState(ENC_WT_PTR_ID);
    }

    RawSetByte(g_encPtrRAWId[ENC_WT_PTR_ID], &byte, 1);

    g_encIndex[ENC_WT_PTR_ID] += 1;
}//end MACPut


/******************************************************************************
 * Function:        void MACPutArray(BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  EWRPT must point to the location to begin writing.
 *
 * Input:           *val: Pointer to source of bytes to copy.
 *                  len:  Number of bytes to write to the data buffer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPutArray writes several sequential bytes to the
 *                  MRF24W RAM.  It performs faster than multiple MACPut()
 *                  calls.  EWRPT is incremented by len.
 *
 * Note:            None
 *****************************************************************************/
void MACPutArray(BYTE *val, WORD len)
{
    if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_INVALID_ID )
    {
        SyncENCPtrRAWState(ENC_WT_PTR_ID);
    }

    RawSetByte(g_encPtrRAWId[ENC_WT_PTR_ID], val, len);

    g_encIndex[ENC_WT_PTR_ID] += len;
}//end MACPutArray


/******************************************************************************
 * Function:        void MACPutROMArray(ROM BYTE *val, WORD len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  EWRPT must point to the location to begin writing.
 *
 * Input:           *val: Pointer to source of bytes to copy.
 *                  len:  Number of bytes to write to the data buffer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPutArray writes several sequential bytes to the
 *                  MRF24W RAM.  It performs faster than multiple MACPut()
 *                  calls.  EWRPT is incremented by len.
 *
 * Note:            None
 *****************************************************************************/
#if defined(__18CXX)
void MACPutROMArray(ROM BYTE *val, WORD len)
{
    if ( g_encPtrRAWId[ENC_WT_PTR_ID] == RAW_INVALID_ID )
    {
        SyncENCPtrRAWState(ENC_WT_PTR_ID);
    }

    RawSetByteROM(g_encPtrRAWId[ENC_WT_PTR_ID], val, len);

    g_encIndex[ENC_WT_PTR_ID] += len;
}//end MACPutROMArray
#endif


/*****************************************************************************
  Function:
    WORD CalcIPBufferChecksum(WORD len)

  Summary:
    Calculates an IP checksum in the MAC buffer itself.

  Description:
    This function calculates an IP checksum over an array of input data 
    existing in the MAC buffer.  The checksum is the 16-bit one's complement 
    of one's complement sum of all words in the data (with zero-padding if 
    an odd number of bytes are summed).  This checksum is defined in RFC 793.

  Precondition:
    TCP is initialized and the MAC buffer pointer is set to the start of
    the buffer.

  Parameters:
    len - number of bytes to be checksummed

  Returns:
    The calculated checksum.

  Remarks:
    All Microchip MACs should perform this function in hardware.
  ***************************************************************************/
WORD CalcIPBufferChecksum(WORD len)
{
    DWORD_VAL Checksum;
    
    BYTE DataBuffer[20]; // Must be an even size
    WORD ChunkLen;
    WORD *DataPtr;

    Checksum.w[0] = 0;  
    Checksum.w[1] = 0;
    while(len)
    {
        // Obtain a chunk of data (less SPI overhead compared 
        // to requesting one byte at a time)
        ChunkLen = len > sizeof(DataBuffer) ? sizeof(DataBuffer) : len;
        MACGetArray(DataBuffer, ChunkLen);
        len -= ChunkLen;

        // Take care of a last odd numbered data byte
        if(((WORD_VAL*)&ChunkLen)->bits.b0)
        {
            DataBuffer[ChunkLen] = 0x00;
            ChunkLen++;
        }

        // Calculate the checksum over this chunk
        DataPtr = (WORD*)&DataBuffer[0];
        while(ChunkLen)
        {
            Checksum.Val += *DataPtr++;
            ChunkLen -= 2;
        }
    }

    // Do an end-around carry (one's complement arrithmatic)
    Checksum.Val = (DWORD)Checksum.w[0] + (DWORD)Checksum.w[1];

    // Do another end-around carry in case if the prior add 
    // caused a carry out
    Checksum.w[0] += Checksum.w[1];

    // Return the resulting checksum
    Checksum.w[0] = ~Checksum.w[0];

    return Checksum.w[0];
}



/******************************************************************************
 * Function:        void MACPowerDown(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerDown puts the MRF24W in low power sleep mode. In
 *                  sleep mode, no packets can be transmitted or received.
 *                  All MAC and PHY registers should not be accessed.
 *
 * Note:            If a packet is being transmitted while this function is
 *                  called, this function will block until it is it complete.
 *                  If anything is being received, it will be completed.
 *****************************************************************************/
void MACPowerDown(void)
{
}//end MACPowerDown


/******************************************************************************
 * Function:        void MACPowerUp(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerUp returns the MRF24W back to normal operation
 *                  after a previous call to MACPowerDown().  Calling this
 *                  function when already powered up will have no effect.
 *
 * Note:            If a link partner is present, it will take 10s of
 *                  milliseconds before a new link will be established after
 *                  waking up.  While not linked, packets which are
 *                  transmitted will most likely be lost.  MACIsLinked() can
 *                  be called to determine if a link is established.
 *****************************************************************************/
void MACPowerUp(void)
{
}//end MACPowerUp

#if 0
/* Not needed for MCHP */

void RawSendUntamperedData(UINT8 *pReq, UINT16 len)
{
    BOOL    res;
    UINT8 preambleBuf[2];
    UINT16 byteCount;

    if (GetTxRawWindowState() == WF_RAW_DATA_MOUNTED)
    {
        WF_ASSERT(FALSE);
    }        

    // RAW memory alloc
    res = AllocateDataTxBuffer(len);
    
    /* if could not allocate enough bytes */
    if (res != TRUE)
    {
        // Release whatever has been allocated.
        DeallocateDataTxBuffer();
        WF_ASSERT(FALSE);
    }

    /* fill out 2 byte preamble of request message */
    preambleBuf[0] = WF_DATA_REQUEST_TYPE;            // indicate this is a data msg
    preambleBuf[1] = WF_UNTAMPERED_DATA_MSG_SUBTYPE;  // untampered data subtype

    /* write out preamble */
    RawWrite(RAW_TX_ID, 0, sizeof(preambleBuf), preambleBuf);

    // write out payload
    RawSetByte(RAW_TX_ID, (UINT8 *) pReq, len);

    // Instruct WF chip to transmit the packet data in the raw window
    RawSendTxBuffer(len + sizeof(preambleBuf));

}
#endif

#else
/* dummy func to keep C18 compiler happy when module has no executeable code */
void WFMac_EmptyFunc(void)
{
    ;
}
#endif /* WF_CS_TRIS*/


