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
#ifndef __WF_RAW_H
#define __WF_RAW_H

#define RAW_ID_0                        (0)
#define RAW_ID_1                        (1)

// RAW0 used for Rx, RAW1 used for Tx
#define RAW_RX_ID                       RAW_ID_0
#define RAW_TX_ID                       RAW_ID_1
#define RAW_INVALID_ID                  (0xff)

// Source/Destination objects on the MRF24W
#define RAW_MAC                         (0x00)   /* Cmd processor (aka MRF24W MAC)                 */
#define RAW_MGMT_POOL                   (0x10)   /* For 802.11 Management packets                  */
#define RAW_DATA_POOL                   (0x20)   /* Data Memory pool used for tx and rx operations */
#define RAW_SCRATCH_POOL                (0x30)   /* Scratch object                                 */
#define RAW_STACK_MEM                   (0x40)   /* single level stack to save state of RAW        */
#define RAW_COPY                        (0x70)   /* RAW to RAW copy                                */

/* 8-bit RAW registers */
#define RAW_0_DATA_REG                  (0x20)
#define RAW_1_DATA_REG                  (0x21)

/* 16 bit RAW registers */
#define RAW_0_CTRL_0_REG                (0x25)
#define RAW_0_CTRL_1_REG                (0x26)
#define RAW_0_INDEX_REG                 (0x27)
#define RAW_0_STATUS_REG                (0x28)
#define RAW_1_CTRL_0_REG                (0x29)
#define RAW_1_CTRL_1_REG                (0x2a)
#define RAW_1_INDEX_REG                 (0x2b)
#define RAW_1_STATUS_REG                (0x2c)

/* RAW Window states */
#define WF_RAW_UNMOUNTED            (0)
#define WF_SCRATCH_MOUNTED          (1)
#define WF_RAW_DATA_MOUNTED         (2)
#define WF_RAW_MGMT_MOUNTED         (3)

#define SetRawWindowState(rawId, state)    RawWindowState[rawId] = state         
#define GetRawWindowState(rawId)           RawWindowState[rawId]

/*
*********************************************************************************************************
*                                       GLOBALS
*********************************************************************************************************
*/
extern BOOL   g_HostRAWDataPacketReceived;
extern BOOL   RawWindowReady[2];     /* for Tx and Rx, TRUE = ready for use, FALSE = not ready for use */
extern UINT8  RawWindowState[2];     /* see RAW Window states above                                    */


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
BOOL    RawGetMgmtRxBuffer(UINT16 *p_numBytes);
void    RawFreeRxMgmtBuffer(void);
void    SetRawRxMgmtInProgress(BOOL action);
BOOL    isRawRxMgmtInProgress(void);
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
void    DeallocateDataTxBuffer(void);
void    DeallocateDataRxBuffer(void);
void    RawSendTxBuffer(UINT16 len);
UINT16  RawMountRxBuffer(void);
void    RawToRawCopy(UINT8 rawdestId, UINT16 length);
#if 0
    /* Not needed for MCHP */
    void RawSendUntamperedData(UINT8 *pReq, UINT16 len);
#endif    

#endif /* __WF_RAW_H */