/******************************************************************************

 MRF24WB0M Driver Data Tx/Rx
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24WB0M WiFi controller
  -Reference: MRF24WB0M Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFDataTxRx.c
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


 Author                Date        Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 KH                 27 Jan 2010 Created for MRF24WB0M
******************************************************************************/

/*
*********************************************************************************************************
*                                           INCLUDES                               
*********************************************************************************************************
*/

#include "TCPIP Stack/WFMac.h"
#if defined(WF_CS_TRIS) && defined(WF_USE_DATA_TX_RX_FUNCTIONS)

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_DATA_TX_RX
#endif

/*
*********************************************************************************************************
*                                           DEFINES                               
*********************************************************************************************************
*/

#define WF_SIZE_OF_TX_DATA_PREAMBLE    (sizeof(tTxDataPreamble))
#define WF_SIZE_OF_RX_DATA_PREAMBLE    (sizeof(tRxDataPreamble))

/* The following are offsets within a RX data packet when mounted into a RAW window on the MRF24WB0M. */
#define RAW_RX_TYPE_INDEX               (0)     /* [0]              */ 
#define RAW_RX_SUBTYPE_INDEX            (1)     /* [1]              */
#define RAW_RX_RSSI_INDEX               (2)     /* [2] - [3]        */
#define RAW_RX_ARRIV_TIME_TH_INDEX      (4)     /* [4] - [5]        */
#define RAW_RX_ARRIV_TIME_BH_INDEX      (6)     /* [6] - [7]        */
#define RAW_RX_DATA_LENGTH_INDEX        (8)     /* [8] - [9]        */
#define RAW_RX_SNAP_HEADER_INDEX        (10)    /* [10] - [15]      */
#define RAW_RX_DEST_ADD_INDEX           (16)    /* [16] - [21]      */
#define RAW_RX_SRC_ADD_INDEX            (22)    /* [22] - [27]      */    

/*
*********************************************************************************************************
*                                           LOCAL DATA TYPES                               
*********************************************************************************************************
*/

typedef struct txDataPreambleStruct
{
    UINT8  type;
    UINT8  subType;
    UINT8  h2;
    UINT8  h3;
} tTxDataPreamble; 

typedef struct rxDataPreambleStruct
{
    UINT8  type;          /* always WF_DATA_RX_INDICATE_TYPE (3) */
    UINT8  subType;       /* always 1                            */             
} tRxDataPreamble; 

#if 0
/* This is the format of a RAW receive packet as created by the MRF24WB0M when it receives a data packet. */
/* This structure starts at RAW index 0 within the RAW Rx buffer.                                      */
typedef struct
{
    tRxDataPreamble   preamble;               /* 2 bytes (type and subtype)                                                     */               
    UINT16  rssi;                             /* the value of the MRF24WB0M RSSI when the data frame was received               */
    UINT16  arrivalTime_th;                   /* value of the 32-bit MRF24WB0M system clock when the frame arrived (bits 31-16) */
    UINT16  arrivalTime_bh;                   /* MRF24WB0M system clock (bits 15-0)                                             */ 
    UINT16  dataLen;                          /* byte length of the payload which immediately follows this data structure       */
    UINT8   snapHeader[6];                    /* WiFi-specific Snap header                                                      */
    UINT8   dstAddr[WF_MAC_ADDRESS_LENGTH];   /* MAC Address to which the data frame was directed.                              */
    UINT8   srcAddr[WF_MAC_ADDRESS_LENGTH];   /* MAC Address of the Station that sent the Data frame.                           */
} tWFRxDataIndicate; /* 28 bytes */
#endif

/*
*********************************************************************************************************
*                                   WF_TxSendCompleteDataPacket()
*    
* Description : Allocates MRF24WB0M memory, writes out the packet to that memory, and directs the MRF24WB0M 
*               to transmit the packet.
*
* Argument(s) : p_txData - pointer to Tx data packet 
*               length   - length, in bytes, of Tx data packet
*
* Return(s)   : Error code
*
* Caller(s)   : Application
*
* Notes:      : None
*
*********************************************************************************************************
*/
void WF_TxSendCompleteDataPacket(UINT8  *p_txData, 
                                 UINT16  length)
                                    
{
    WF_TxDataAllocateBuffer(length);   
    WF_TxDataWrite(p_txData, length, 0);
    WF_TxDataSendPacket(length);
}                                         

/*
*********************************************************************************************************
*                                   WF_RxReceiveCompleteDataPacket()
*    
* Description : Reads an entire data rx packet from MRF24WB0M memory and copies it to host memory, then 
*               frees the MRF24WB0M memory so it can be reused.
*
* Argument(s) : p_rxData - pointer to where to copy received packet
*               length   - length, in bytes, of the Rx packet to copy
*
* Return(s)   : Error code
*
* Caller(s)   : Application
*
* Notes:      : None
*
*********************************************************************************************************
*/
void WF_RxReceiveCompleteDataPacket(UINT8  *p_rxData, 
                                        UINT16  length)
{
    /* read the entire receive data packet into host memory */
    WF_RxDataReadPacket(p_rxData, length, 0);
    
    /* free MRF24WB0M memory that held the rx data packet */    
    WF_RxDataDeallocateBuffer();
}                                            


/*
*********************************************************************************************************
*                                   WF_TxDataAllocateBuffer()
*    
* Description : Allocates a Tx Data Packet buffer on the MRF24WB0M
*
* Argument(s) : txDataPacketLength - length, in bytes, of memory needed
*
* Return(s)   : None
*
* Caller(s)   : Application
*
* Notes:      : (1) Allocates memory on the MRF24WB0M to hold a Tx Data Packet
*
*               (2) Do not assume this function always succeeds.  During busy periods the MRF24WB0M may not
*                   have available memory until the previous Tx data packet has finished transmitting.
*
*********************************************************************************************************
*/
void WF_TxDataAllocateBuffer(UINT16 txDataPacketLength)
{
    /* Ensure the MRF24WB0M is awake (only applies if PS-Poll was enabled) */
    EnsureWFisAwake();
    
    if (!AllocateDataTxBuffer(txDataPacketLength))
    {
        WF_ASSERT(FALSE);
    }    
}
   
   
/*
*********************************************************************************************************
*                                   WF_TxDataWrite()
*    
* Description : Writes Tx packet data from Host CPU to MRF24WB0M tx buffer
*
* Argument(s) : p_txData   - Pointer to Tx data
*               length     - length, in bytes, of Tx data
*               startIndex - Index within MRF24WB0M Tx data block to start writing
*
* Return(s)   : None
*
* Caller(s)   : Application
*
* Notes:      : (1) MRF24WB0M Tx data block must first be allocated via WF_TxDataAllocateBuffer()
*
*               (2) Current support only for full length Tx data messaages. In other words, startIndex
*                   must be 0, and length must be the total length of the Tx data packet. 
*
*********************************************************************************************************
*/
void WF_TxDataWrite(UINT8  *p_txData, 
                    UINT16  length,
                    UINT16  startIndex)
{
    WF_ASSERT(startIndex == 0);

    if (length == 0)
    {
        return;
    }    

    /* Ensure the MRF24WB0M is awake (only applies if PS-Poll was enabled) */
    EnsureWFisAwake();
    
    /* set the RAW index to desired location, offset by the Tx data preamble */
    RawSetIndex(RAW_TX_ID, startIndex + WF_SIZE_OF_TX_DATA_PREAMBLE);

    /* write tx data to chip */
    WriteWFArray(RAW_1_DATA_REG, p_txData, length);
}                                
   
/*
*********************************************************************************************************
*                                   WF_TxDataSendPacket()
*    
* Description : Directs the MRF24WB0M to transmit a Tx data packet to the 802.11 network.
*
* Argument(s) : length - length, in bytes, of data packet
*
* Return(s)   : Error code
*
* Caller(s)   : Application
*
* Notes:      : (1) MRF24WB0M Tx data block must first be allocated via WF_TxDataAllocateBuffer()
*
*               (2) Current support only for full length Tx data messaages. In other words, startIndex
*                   must be 0, and length must be the total length of the Tx data packet. 
*
*********************************************************************************************************
*/
void WF_TxDataSendPacket(UINT16 length)
{
    tTxDataPreamble txPreamble;

    /* fill in tx data preamble with correct header */    
    txPreamble.type    = WF_DATA_REQUEST_TYPE;
    txPreamble.subType = WF_STD_DATA_MSG_SUBTYPE;
    txPreamble.h2      = 1;
    txPreamble.h3      = 0;
    
    /* Ensure the MRF24WB0M is awake (only applies if PS-Poll was enabled) */
    EnsureWFisAwake();
    
    /* write out preamble to MRF24WB0M buffer to the two extra bytes at start of allocated buffer */
    RawSetIndex(RAW_TX_ID, 0);      
    WriteWFArray(RAW_1_DATA_REG, (UINT8 *)&txPreamble, WF_SIZE_OF_TX_DATA_PREAMBLE );
    
    /* now tell MRF24WB0M to transmit the tx data packet */
    RawSendTxBuffer(length);
}


/*
*********************************************************************************************************
*                                   WF_RxDataReadPacket()
*    
* Description : Reads all or part of an Rx data packet from MRF24WB0M memory to Host CPU memory.
*
* Argument(s) : p_rxData   - pointer to where Rx data packet will be written
*               length     - Number of bytes to read from MRF24WB0M memory
*               startIndex - start index within MRF24WB0M memory to start read from
*
* Return(s)   : None
*
* Caller(s)   : Application
*
* Notes:      : None
*
*********************************************************************************************************
*/
void WF_RxDataReadPacket(UINT8  *p_rxData,
                         UINT16 length,
                         UINT16 startIndex)
{
#if !defined(USE_WF_HOST_BUFFER)    
    UINT16 byteCount;
#endif
         
    WF_ASSERT(startIndex == 0);
    
    /* if application calls this function, and gHostRAWDataPacketReceived is not TRUE, then error, because */
    /* driver has not received a data packet.                                                              */
    if (!g_HostRAWDataPacketReceived)
    {
        WF_ASSERT(FALSE);
    }  
    
    g_HostRAWDataPacketReceived = FALSE;     /* clear flag for next data packet */
    
    /* Ensure the MRF24WB0M is awake (only applies if PS-Poll was enabled) */    
    EnsureWFisAwake();
    
#if !defined(USE_WF_HOST_BUFFER)  /* when testing with MCHP stack the packet is already mounted */
    /* Mount Read FIFO to RAW Rx window. Size of Rx data packet is returned */
    byteCount = RawMountRxBuffer();
    WF_ASSERT(byteCount > 0);
#endif
        
    /* now that buffer mounted it is safe to reenable interrupts */
    WF_EintEnable();
    
    /* read the requested bytes into callers buffer */
    RawRead(RAW_RX_ID, RAW_RX_DEST_ADD_INDEX + startIndex, length, p_rxData); 
}      
                               


/*
*********************************************************************************************************
*                                   WF_RxDataDeallocateBuffer()
*    
* Description : Releases the MRF24WB0M memory containing an Rx data packet after the application is done with
*               it.
*
* Argument(s) : None
*
* Return(s)   : None
*
* Caller(s)   : Application
*
* Notes:      : (1) Must be called after the application completes processing of an Rx data packet.
*
*********************************************************************************************************
*/
void WF_RxDataDeallocateBuffer(void)
{
    /* Ensure the MRF24WB0M is awake (only applies if PS-Poll was enabled) */
    EnsureWFisAwake();
    
    DeallocateDataRxBuffer();
}    

#endif /* WF_CS_TRIS && WF_USE_DATA_TX_RX_FUNCTIONS */


