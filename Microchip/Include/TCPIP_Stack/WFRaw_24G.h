/******************************************************************************

 MRF24W Driver Raw driver
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFRaw.h
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
*                                       DEFINES
*********************************************************************************************************
*/

/* Supported RAW Windows */
#define RAW_ID_0                        (0)
#define RAW_ID_1                        (1)
#define RAW_ID_2                        (2)
#define RAW_ID_3                        (3)
#define RAW_ID_4                        (4)
#define RAW_ID_5                        (5)

/* Usage of RAW Windows */
#define RAW_DATA_RX_ID                  RAW_ID_0
#define RAW_DATA_TX_ID                  RAW_ID_1
#define RAW_MGMT_RX_ID                  RAW_ID_2
#define RAW_MGMT_TX_ID                  RAW_ID_3
#define RAW_SCRATCH_ID                  RAW_ID_4
#define RAW_UNUSED_ID                   RAW_ID_5

// Source/Destination objects on the MRF24W
#define RAW_MAC                         (0x00)   /* Cmd processor (aka MRF24W MAC)              */
#define RAW_MGMT_POOL                   (0x10)   /* For 802.11 Management packets                  */
#define RAW_DATA_POOL                   (0x20)   /* Data Memory pool used for tx and rx operations */
#define RAW_SCRATCH_POOL                (0x30)   /* Scratch object                                 */
#define RAW_STACK_MEM                   (0x40)   /* single level stack to save state of RAW        */
#define RAW_COPY                        (0x70)   /* RAW to RAW copy                                */

/*---------------------*/
/* 8-bit RAW registers */
/*---------------------*/
#define RAW_0_DATA_REG                  (0x20)   /* Data Rx       */
#define RAW_1_DATA_REG                  (0x21)   /* Data Tx       */
#define RAW_2_DATA_REG                  (0x06)   /* Mgmt Rx       */
#define RAW_3_DATA_REG                  (0x07)   /* Mgmt Tx       */
#define RAW_4_DATA_REG                  (0x08)   /* Scratch Tx/Rx */
#define RAW_5_DATA_REG                  (0x09)   /* not used      */

/*----------------------*/
/* 16 bit RAW registers */
/*----------------------*/
#define RAW_0_CTRL_0_REG                (0x25)      /* RAW 0 -- Data Rx       */
#define RAW_0_CTRL_1_REG                (0x26)
#define RAW_0_INDEX_REG                 (0x27)
#define RAW_0_STATUS_REG                (0x28)

#define RAW_1_CTRL_0_REG                (0x29)      /* RAW 1 -- Data Tx       */
#define RAW_1_CTRL_1_REG                (0x2a)
#define RAW_1_INDEX_REG                 (0x2b)
#define RAW_1_STATUS_REG                (0x2c)

#define RAW_2_CTRL_0_REG                (0x18)      /* RAW 2 -- Mgmt Rx       */
#define RAW_2_CTRL_1_REG                (0x19)
#define RAW_2_INDEX_REG                 (0x1a)
#define RAW_2_STATUS_REG                (0x1b)

#define RAW_3_CTRL_0_REG                (0x1c)      /* RAW 3 -- Mgmt Tx       */
#define RAW_3_CTRL_1_REG                (0x1d)
#define RAW_3_INDEX_REG                 (0x1e)
#define RAW_3_STATUS_REG                (0x1f)

#define RAW_4_CTRL_0_REG                (0x0a)      /* RAW 4 -- Scratch Tx/Rx */
#define RAW_4_CTRL_1_REG                (0x0b)
#define RAW_4_INDEX_REG                 (0x0c)
#define RAW_4_STATUS_REG                (0x0d)

#define RAW_5_CTRL_0_REG                (0x0e)      /* RAW 5 -- Not used       */
#define RAW_5_CTRL_1_REG                (0x0f)
#define RAW_5_INDEX_REG                 (0x22)
#define RAW_5_STATUS_REG                (0x23)


/* RAW Window states */
#define WF_RAW_UNMOUNTED            (0)
#define WF_SCRATCH_MOUNTED          (1)
#define WF_RAW_DATA_MOUNTED         (2)
#define WF_RAW_MGMT_MOUNTED         (3)

/* sets and gets the state of RAW data tx/rx windows */
#define SetRawWindowState(rawId, state)    RawWindowState[rawId] = state         
#define GetRawWindowState(rawId)           RawWindowState[rawId]

/* these macros set a flag bit if the raw index is set past the end of the raw window, or clear the */
/* flag bit if the raw index is set within the raw window.                                              */
#define SetIndexOutOfBoundsFlag(rawId)      g_RawIndexPastEnd |= g_RawAccessOutOfBoundsMask[rawId]
#define ClearIndexOutOfBoundsFlag(rawId)    g_RawIndexPastEnd &= ~g_RawAccessOutOfBoundsMask[rawId]
#define isIndexOutOfBounds(rawId)           ((g_RawIndexPastEnd & g_RawAccessOutOfBoundsMask[rawId]) > 0)

/* macros to get and set the Rx data buffer size (size of the currently mounted Rx data packet */
#define SetRxDataPacketLength(length)      g_rxBufferLength = length
#define GetRxDataPacketLength()            g_rxBufferLength

/*
*********************************************************************************************************
*                                       GLOBALS
*********************************************************************************************************
*/
extern BOOL   g_HostRAWDataPacketReceived;
extern UINT8  RawWindowState[2];     /* see RAW Window states above */
extern UINT8  g_RawIndexPastEnd;
extern UINT16 g_rxBufferLength;
extern ROM UINT8 g_RawAccessOutOfBoundsMask[];


/*
*********************************************************************************************************
*                                       FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void   RawInit(void);

BOOL    RawSetIndex(UINT16 rawId, UINT16 index);
UINT16  RawGetIndex(UINT16 rawId);
void    RawGetByte(UINT16 rawId, UINT8 *pBuffer, UINT16 length);
void    RawSetByte(UINT16 rawId, UINT8 *pBuffer, UINT16 length);
void    RawSetByteROM(UINT16 rawId, ROM UINT8 *pBuffer, UINT16 length);
void    RawGetMgmtRxBuffer(UINT16 *p_numBytes);
void    RawFreeRxMgmtBuffer(void);
void    SendRAWDataFrame(UINT16 bufLen);
void    PushRawWindow(UINT8 rawId);
UINT16  PopRawWindow(UINT8 rawId);
void    ScratchUnmount(UINT8 rawId);
UINT16  ScratchMount(UINT8 rawId);
void    RawRead(UINT8 rawId, UINT16 startIndex, UINT16 length, UINT8 *p_dest);
void    RawWrite(UINT8 rawId, UINT16 startIndex, UINT16 length, UINT8 *p_src);
BOOL    AllocateMgmtTxBuffer(UINT16 bytesNeeded);
void    DeallocateMgmtRxBuffer(void);
BOOL    AllocateDataTxBuffer(UINT16 bytesNeeded);

void    DeallocateDataRxBuffer(void);

#if 0
void    DeallocateDataTxBuffer(void);
void    RawSendTxBuffer(UINT16 len);
#endif
UINT16  RawMountRxBuffer(UINT8 rawId);
void    RawToRawCopy(UINT8 rawDestId, UINT8 rawSourceId, UINT16 length);
#if 0
    /* Not needed for MCHP */
    void RawSendUntamperedData(UINT8 *pReq, UINT16 len);
#endif    
