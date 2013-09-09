/******************************************************************************

 MRF24W Driver Com Layer
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFDriverCom.c
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

#include "TCPIP Stack/TCPIP.h"  // need this to access STACK_USE_DHCP_CLIENT define
#if defined(WF_CS_TRIS)


/*
*********************************************************************************************************
*                                           DEFINES      
*********************************************************************************************************
*/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_DRIVER_COM
#endif

// HOST_RESET_REG masks
#define HR_CPU_RST_N_MASK                     ((UINT16)0x01 << 15)
#define HR_DISABLE_DOWNLOAD_SCRAMBLER_MASK    ((UINT16)0x01 << 14)
#define HR_FORCE_CPU_CLK_FREEZE_MASK          ((UINT16)0x01 << 13)
#define HR_HOST_ANA_SPI_EN_MASK               ((UINT16)0x01 << 12)
#define HR_HOST_ANA_SPI_DIN_MASK              ((UINT16)0x01 << 11)
#define HR_HOST_ANA_SPI_DOUT_MASK             ((UINT16)0x01 << 10)
#define HR_HOST_ANA_SPI_CLK_MASK              ((UINT16)0x01 << 9)
#define HR_HOST_ANA_SPI_CSN_MASK              ((UINT16)0x07 << 6)   // 8:6
#define HR_RESERVED_2_MASK                    ((UINT16)0x01 << 5)
#define HR_HOST_SPI_DISABLE_MASK              ((UINT16)0x01 << 4)
#define HR_HOST_ENABLE_NEW_PROG_MASK          ((UINT16)0x01 << 3)
#define HR_HOST_ENABLE_DOWNLOAD_MASK          ((UINT16)0x01 << 2)
#define HR_HOST_FAST_RESET_MASK               ((UINT16)0x01 << 1)
#define HR_HOST_RESET_MASK                    ((UINT16)0x01 << 0)

// This block of defines needed to restart PLL
#define ANALOG_PORT_3_REG_TYPE           ((UINT32)0x09)   /* 16-bit analog register in SPI Port 3                                          */
#define ANALOG_PORT_2_REG_TYPE           ((UINT32)0x08)   /* 16-bit analog register in SPI Port 2                                          */
#define ANALOG_PORT_1_REG_TYPE           ((UINT32)0x0a)   /* 16-bit analog register in SPI Port 1                                          */
#define ANALOG_PORT_0_REG_TYPE           ((UINT32)0x0b)   /* 16-bit analog register in SPI Port 0                                          */

#define SPI_WRITE_MASK                          (UINT8)0x00     // bit 0 = 0
#define SPI_READ_MASK                           (UINT8)0x01     // bit 0 = 1
#define SPI_AUTO_INCREMENT_ENABLED_MASK         (UINT8)0x00     // bit 1 = 0
#define SPI_AUTO_INCREMENT_DISABLED_MASK        (UINT8)0x02     // bit 1 = 1

#define PLL9_REG   ((UINT32)(9 * 2)) // SPI Port 3 Registers (Port 5 if going through Master SPI controller)
#define PLL8_REG   ((UINT32)(8 * 2))
#define PLL7_REG   ((UINT32)(7 * 2))
#define PLL6_REG   ((UINT32)(6 * 2)) 
#define PLL5_REG   ((UINT32)(5 * 2)) 
#define PLL4_REG   ((UINT32)(4 * 2)) 
#define PLL3_REG   ((UINT32)(3 * 2)) 
#define PLL2_REG   ((UINT32)(2 * 2)) 
#define PLL1_REG   ((UINT32)(1 * 2)) 
#define PLL0_REG   ((UINT32)(0 * 2)) 
// end PLL block

#define OSC0_REG   ((UINT32)(0 * 2)) 
#define OSC1_REG   ((UINT32)(1 * 2)) 
#define OSC2_REG   ((UINT32)(2 * 2)) 
#define PLDO_REG            ((UINT32)(3 * 2))
#define BIAS_REG            ((UINT32)(4 * 2))
#define ANALOG_SPARE_REG    ((UINT32)(5 * 2))

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES      
*********************************************************************************************************
*/

/* Functions that are called from the External Interrupt routine use these global */
/* variables instead of local variables to avoid stack corruption on CPU's that   */
/* that use overlay memory.  See note in WFEintHandler() function.                */
static UINT8 g_txBuf[3];
static UINT8 g_rxBuf[3];

static UINT8 g_HostIntSaved = 0;

// Keep these as static globals instead of local variables in the Eint Handler.
// If declared as local variables, causes stack corruption in PIC18
static UINT8  g_EintHostIntRegValue;
static UINT8  g_EintHostIntMaskRegValue;
static UINT8  g_EintHostInt;

static BOOL             g_MgmtReadMsgReady;                  /* TRUE if rx mgmt msg to process, else FALSE              */
static volatile BOOL    g_ExIntNeedsServicing;               /* TRUE if external interrupt needs processing, else FALSE */

#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
    static BOOL g_EnablePowerSaveMode = FALSE;
#endif

tRawMoveState RawMoveState;
extern BOOL g_WaitingForMgmtResponse;
BOOL g_DhcpSuccessful = FALSE;
BOOL g_WiFiConnectionChanged = FALSE;
BOOL g_WiFiConnection = FALSE;
BOOL g_dhcpInProgress = FALSE;


extern BOOL g_rxDtim;

/*
*********************************************************************************************************
*                                       LOCAL FUNCTION PROTOTYPES      
*********************************************************************************************************
*/

static void ProcessMgmtRxMsg(void);
static void ChipReset(void);
static void ProcessInterruptServiceResult(void);
static UINT8 GetSpiPortWithBitBang(UINT8 regType);
static void WriteAnalogRegisterBitBang(UINT8 regType, UINT16 address, UINT16 value);
static void ResetPll(void);


static BOOL isDhcpInProgress(void);

extern BOOL isSleepNeeded(void);
extern void SetSleepNeeded(void);
extern void ClearSleepNeeded(void);
extern BOOL GetAppPowerSaveMode(void);

void WiFiTask(void)
{
    #if defined (WF_USE_POWER_SAVE_FUNCTIONS) 
    //--------------------------------------------------------------------------
    // if not waiting for a mgmt response and the application wants PS-Poll Mode
    //--------------------------------------------------------------------------
    if ((!g_WaitingForMgmtResponse) && (GetAppPowerSaveMode() == TRUE))
    {
        // else if changed from connected to disconnected, or vice-versa
        if (g_WiFiConnectionChanged == TRUE)
        {
            g_WiFiConnectionChanged = FALSE;

            // if lost connection
            if (g_WiFiConnection == FALSE)
            {
               WFConfigureLowPowerMode(WF_LOW_POWER_MODE_OFF);
            }
            // else connected (or reconnected)  
            else
            {
                // if not using DHCP
                if (AppConfig.Flags.bIsDHCPEnabled == FALSE)
                {
                    WF_PsPollEnable(g_rxDtim);
                    WFConfigureLowPowerMode(WF_LOW_POWER_MODE_ON);
                }    
                // note: if using DHCP, another case will reenable PS-Poll mode
            } 
        }            
        // else if app is using DHCP and we just got an IP address via DHCP
        else if ((AppConfig.Flags.bIsDHCPEnabled == TRUE) && (g_DhcpSuccessful == TRUE))
        {
           g_DhcpSuccessful = FALSE; 
           WF_PsPollEnable(g_rxDtim);
           WFConfigureLowPowerMode(WF_LOW_POWER_MODE_ON);
        }    
        // if application wants PS-Poll, but the driver disabled it to send a message (and not waiting for DHCP)
        else if ( g_WiFiConnection == TRUE && isSleepNeeded() && !isDhcpInProgress() )
        {
            ClearSleepNeeded();
            WFConfigureLowPowerMode(WF_LOW_POWER_MODE_ON);
        }  
    }    
    #endif /* WF_USE_POWER_SAVE_FUNCTIONS */

} 


/*****************************************************************************
 * FUNCTION: WFProcess
 *
 * RETURNS:  None
 *
 * PARAMS:   None
 *
 *  NOTES:   This function is called from WFProcess.  It does the following:
 *             1) checks for and processes MRF24W external interrupt events
 *             2) checks for and processes received management messages from the MRF24W
 *             3) maintains the PS-Poll state (if applicable)
 *           
 *****************************************************************************/
void WFProcess(void)
{
    #if defined(__18CXX)
        static UINT16 len;
    #else
        UINT16 len;
    #endif
    
    //----------------------------------------------------------
    // if there is a MRF24W External interrupt (EINT) to process
    //----------------------------------------------------------
    if (g_ExIntNeedsServicing == TRUE)
    {
        g_ExIntNeedsServicing = FALSE;
        ProcessInterruptServiceResult();
    }
    //----------------------------------------
    // else if there is management msg to read
    //----------------------------------------
    else if (g_MgmtReadMsgReady == TRUE)
    {
        RawGetMgmtRxBuffer(&len);

        // handle received managment message
        g_MgmtReadMsgReady = FALSE;
        ProcessMgmtRxMsg();

        // reenable interrupts
        WF_EintEnable();

    }
}

static BOOL isDhcpInProgress(void)
    {
    return g_dhcpInProgress;
}    


void SignalDHCPSuccessful()
{
    g_DhcpSuccessful = TRUE;
    g_dhcpInProgress = FALSE;
}    


void SetDhcpProgressState(void)
{
    g_dhcpInProgress = TRUE;

    // disable power save mode while DHCP in progress
    if (GetAppPowerSaveMode() == TRUE)
    {
        WFConfigureLowPowerMode(WF_LOW_POWER_MODE_OFF);
    }
}    

void SignalWiFiConnectionChanged(BOOL state)
{
    g_WiFiConnectionChanged = TRUE;
    g_WiFiConnection = state;
}



/*****************************************************************************
 * FUNCTION: ProcessInterruptServiceResult
 *
 * RETURNS: N/A
 *
 * PARAMS:
 *      N/A
 *
 *
 *  NOTES: Processes EXINT from MRF24W.  Called by WFProcess().
 *****************************************************************************/
static void ProcessInterruptServiceResult(void)
{
    UINT8  hostIntRegValue;
    UINT8  hostIntMaskRegValue;
    UINT8  hostInt;

    /* read hostInt register to determine cause of interrupt */
    hostIntRegValue = Read8BitWFRegister(WF_HOST_INTR_REG);

    // OR in the saved interrupts during the time when we were waiting for raw complete, set by WFEintHandler()
    hostIntRegValue |= g_HostIntSaved;
    
    // done with the saved interrupts, clear variable
    g_HostIntSaved = 0;


    hostIntMaskRegValue  = Read8BitWFRegister(WF_HOST_MASK_REG);

    // AND the two registers together to determine which active, enabled interrupt has occurred
    hostInt = hostIntRegValue & hostIntMaskRegValue;

    /* if received a level 2 interrupt (either a mgmt tx or mgmt rx Raw move complete occurred */
    if((hostInt & WF_HOST_INT_MASK_INT2) == WF_HOST_INT_MASK_INT2)
    {
        UINT16 hostInt2;

        hostInt2 = Read16BitWFRegister(WF_HOST_INTR2_REG);
        if (hostInt2 & WF_HOST_INT_MASK_MAIL_BOX_0_WRT)
        {
            // the MRF24WG module CPU has hit an assert condition
            // display the assert info and do a WF_ASSERT
			#if defined(WF_DEBUG)
            #if defined(MRF24WG)
                WF_DisplayModuleAssertInfo();
            #endif
            #endif
            WF_ASSERT(FALSE);
        }

        WF_ASSERT(FALSE); /* This should not happen. Other than the module assert, this interrupt is only used to     */
                          /* signify RAW Move Complete for RAW2/3/4.  This event should be cleared in EintHandler().  */
        /* clear this interrupt */
        Write16BitWFRegister(WF_HOST_INTR2_REG, WF_HOST_INT_MASK_INT2);
    }
    /* else if got a FIFO 1 Threshold interrupt (Management Fifo).  Mgmt Rx msg ready to proces. */
    else if((hostInt & WF_HOST_INT_MASK_FIFO_1_THRESHOLD) == WF_HOST_INT_MASK_FIFO_1_THRESHOLD)
    {
        /* clear this interrupt */
        Write8BitWFRegister(WF_HOST_INTR_REG, WF_HOST_INT_MASK_FIFO_1_THRESHOLD);
        
        // notify MAC state machine that management message needs to be processed
        g_MgmtReadMsgReady = TRUE; 
    }
    /* else if got a FIFO 0 Threshold Interrupt (Data Fifo).  Data Rx msg ready to process. */
    else if((hostInt & WF_HOST_INT_MASK_FIFO_0_THRESHOLD) == WF_HOST_INT_MASK_FIFO_0_THRESHOLD)
    {
        /* clear this interrupt */
        Write8BitWFRegister(WF_HOST_INTR_REG, WF_HOST_INT_MASK_FIFO_0_THRESHOLD);
        
        /* notify MAC state machine that data message needs to be processed */
        g_HostRAWDataPacketReceived = TRUE;  
    }
    /* else got a Host interrupt that we don't handle */
    else if(hostInt)
    {
        /* clear this interrupt */
        Write8BitWFRegister(WF_HOST_INTR_REG, hostInt);
        WF_EintEnable();
    }
    /* we got a spurious interrupt (no bits set in register) */
    else
    {
        /* spurious interrupt */
        WF_EintEnable();
    }
}


/*****************************************************************************
 * FUNCTION: Read8BitWFRegister
 *
 * RETURNS: register value
 *
 * PARAMS:
 *      regId -- ID of 8-bit register being read
 *
 *  NOTES: Reads WF 8-bit register
 *****************************************************************************/
UINT8 Read8BitWFRegister(UINT8 regId)
{
    g_txBuf[0] = regId | WF_READ_REGISTER_MASK;
    WF_SpiEnableChipSelect();
    
    WFSpiTxRx(g_txBuf, 
              1,
              g_rxBuf,
              2);

    WF_SpiDisableChipSelect();
    
    return g_rxBuf[1];   /* register value returned in the second byte clocking */
}

/*****************************************************************************
 * FUNCTION: Write8BitWFRegister
 *
 * RETURNS: None
 *
 * PARAMS:
 *      regId -- ID of 8-bit register being written to
 *      value -- value to write
 *
 *  NOTES: Writes WF 8-bit register
 *****************************************************************************/
void Write8BitWFRegister(UINT8 regId, UINT8 value)
{
    g_txBuf[0] = regId | WF_WRITE_REGISTER_MASK;
    g_txBuf[1] = value;
    
    WF_SpiEnableChipSelect();

    WFSpiTxRx(g_txBuf, 
              2,
              g_rxBuf,
              1);

    WF_SpiDisableChipSelect();
}

/*****************************************************************************
 * FUNCTION: Read16BitWFRegister
 *
 * RETURNS: register value
 *
 * PARAMS:
 *      regId -- ID of 16-bit register being read
 *
 *  NOTES: Reads WF 16-bit register
 *****************************************************************************/
UINT16 Read16BitWFRegister(UINT8 regId)
{
    g_txBuf[0] = regId | WF_READ_REGISTER_MASK;
    WF_SpiEnableChipSelect();
    
    WFSpiTxRx(g_txBuf, 
              1,
              g_rxBuf,
              3);

    WF_SpiDisableChipSelect();
    
    return (((UINT16)g_rxBuf[1]) << 8) | ((UINT16)(g_rxBuf[2]));
}

/*****************************************************************************
 * FUNCTION: Write16BitWFRegister
 *
 * RETURNS: None
 *
 * PARAMS:
 *      regId -- ID of 16-bit register being written to
 *      value -- value to write
 *
 *  NOTES: Writes WF 16-bit register
 *****************************************************************************/
void Write16BitWFRegister(UINT8 regId, UINT16 value)
{
    g_txBuf[0] = regId | WF_WRITE_REGISTER_MASK;
    g_txBuf[1] = (UINT8)(value >> 8);       /* MS byte being written     */
    g_txBuf[2] = (UINT8)(value & 0x00ff);   /* LS byte being written     */
    
    WF_SpiEnableChipSelect();

    WFSpiTxRx(g_txBuf, 
              3,
              g_rxBuf,
              1);

    WF_SpiDisableChipSelect();
}

/*****************************************************************************
 * FUNCTION: WriteWFArray
 *
 * RETURNS: None
 *
 * PARAMS:
 *      regId  -- Raw register being written to
 *      pBuf   -- pointer to array of bytes being written
 *      length -- number of bytes in pBuf
 *
 *  NOTES: Writes a data block to specified raw register
 *****************************************************************************/
void WriteWFArray(UINT8 regId, UINT8 *p_Buf, UINT16 length)
{
    g_txBuf[0] = regId;

    WF_SpiEnableChipSelect();

    /* output cmd byte */
    WFSpiTxRx(g_txBuf, 
              1,
              g_rxBuf,
              1);

    /* output data array bytes */
    WFSpiTxRx(p_Buf, 
              length,
              g_rxBuf,
              1);

    WF_SpiDisableChipSelect();
}

/*****************************************************************************
 * FUNCTION: ReadWFArray
 *
 * RETURNS: None
 *
 * PARAMS:
 *      regId  -- Raw register being read from
 *      pBuf   -- pointer where to write out bytes
 *      length -- number of bytes to read
 *
 *  NOTES: Reads a block of data from a raw register
 *****************************************************************************/
void ReadWFArray(UINT8  regId, UINT8 *p_Buf, UINT16 length)
{
    WF_SpiEnableChipSelect();
    
    /* output command byte */
    g_txBuf[0] = regId | WF_READ_REGISTER_MASK;
    WFSpiTxRx(g_txBuf, 
              1,
              g_rxBuf,
              1);

    /* read data array */
    WFSpiTxRx(g_txBuf, 
              1,   /* garbage tx byte */
              p_Buf,
              length);

    WF_SpiDisableChipSelect();
}

#if defined (__18CXX)
/*****************************************************************************
 * FUNCTION: WriteWFROMArray
 *
 * RETURNS: None
 *
 * PARAMS:
 *      regId  -- Raw register being written to
 *      pBuf   -- pointer to array of bytes being written
 *      length -- number of bytes in pBuf
 *
 *  NOTES: Writes a data block (in ROM) to specified raw register.  This function
 *         is only needed for the Microchip PIC18.
 *****************************************************************************/
void WriteWFROMArray(UINT8 regId, ROM UINT8 *p_Buf, UINT16 length)
{
    g_txBuf[0] = regId;

    WF_SpiEnableChipSelect();

    /* output cmd byte */
    WFSpiTxRx(g_txBuf, 
              1,
              g_rxBuf,
              1);

    /* output data array bytes */
    WFSpiTxRx_Rom(p_Buf, 
                  length,
                  g_rxBuf,
                  1);


    WF_SpiDisableChipSelect();
}
#endif 

#include "TCPIP Stack/TCPIP.h"



/*****************************************************************************
 * FUNCTION: ChipReset
 *
 * RETURNS: N/A
 *
 * PARAMS:
 *      N/A
 *
 *
 *  NOTES: Performs the necessary SPI operations to cause the MRF24W to do a soft
 *         reset.
 *
 *         This function waits for the MRF24WG to complete its initialization before
 *         returning to the caller.  The largest part of the wait is for the MRF24WG
 *         to download any patch code in FLASH into its RAM.
 *****************************************************************************/
static void ChipReset(void)
{
    UINT16 value;
    UINT32 timeoutPeriod;
    UINT32 startTickCount;

    /* clear the power bit to disable low power mode on the MRF24W */
    Write16BitWFRegister(WF_PSPOLL_H_REG, 0x0000);

    /* Set HOST_RESET bit in register to put device in reset */
    Write16BitWFRegister(WF_HOST_RESET_REG, Read16BitWFRegister(WF_HOST_RESET_REG) | WF_HOST_RESET_MASK);

    /* Clear HOST_RESET bit in register to take device out of reset */
    Write16BitWFRegister(WF_HOST_RESET_REG, Read16BitWFRegister(WF_HOST_RESET_REG) & ~WF_HOST_RESET_MASK);

    /* after reset is started poll register to determine when HW reset has completed */
    timeoutPeriod = TICKS_PER_SECOND * 3;  /* 3000 ms */
    startTickCount = (UINT32)TickGet();
    do
    {
        Write16BitWFRegister(WF_INDEX_ADDR_REG, WF_HW_STATUS_REG);
        value = Read16BitWFRegister(WF_INDEX_DATA_REG);
        if (TickGet() - startTickCount >= timeoutPeriod)
        {
            WF_ASSERT(FALSE);
        }   
    } while ( (value & WF_HW_STATUS_NOT_IN_RESET_MASK) == 0);

    
    /* if SPI not connected will read all 1's */
    WF_ASSERT(value != 0xffff);

    /* now that chip has come out of HW reset, poll the FIFO byte count register     */
    /* which will be set to a non-zero value when the MRF24W initialization is   */
    /* complete.                                                                     */
    startTickCount = (UINT32)TickGet();
    do
    {
        value = Read16BitWFRegister(WF_HOST_WFIFO_BCNT0_REG);
        if (TickGet() - startTickCount >= timeoutPeriod)
        {
            WF_ASSERT(FALSE);
        } 
    } while (value == 0);    
}



/*****************************************************************************
 * FUNCTION: HostInterrupt2RegInit
 *
 * RETURNS: N/A
 *
 * PARAMS:
 *      hostIntrMaskRegMask - The bit mask to be modified.
 *      state               - One of WF_INT_DISABLE, WF_INT_ENABLE where
 *                             Disable implies clearing the bits and enable sets 
 *                             the bits.
 *
 *
 *  NOTES: Initializes the 16-bit Host Interrupt register on the WiFi device with the
 *          specified mask value either setting or clearing the mask register as 
 *          determined by the input parameter state. 
 *****************************************************************************/
static void HostInterrupt2RegInit(UINT16 hostIntMaskRegMask,
                                  UINT8  state)
{
    UINT16 int2MaskValue;

    /* Host Int Register is a status register where each bit indicates a specific event  */
    /* has occurred. In addition, writing a 1 to a bit location in this register clears  */
    /* the event.                                                                        */

    /* Host Int Mask Register is used to enable those events activated in Host Int Register */
    /* to cause an interrupt to the host                                                    */

    /* read current state of int2 mask reg */
    int2MaskValue = Read16BitWFRegister(WF_HOST_INTR2_MASK_REG);

    /* if caller is disabling a set of interrupts */
    if (state == WF_INT_DISABLE)
    {
        /* zero out that set of interrupts in the interrupt mask copy */
        int2MaskValue &= ~hostIntMaskRegMask;
    }
    /* else caller is enabling a set of interrupts */
    else
    {
        /* set to 1 that set of interrupts in the interrupt mask copy */
        int2MaskValue |= hostIntMaskRegMask;
    }
    
    /* write out new interrupt mask value */
    Write16BitWFRegister(WF_HOST_INTR2_MASK_REG, int2MaskValue);

    /* ensure that pending interrupts from those updated interrupts are cleared */
    Write16BitWFRegister(WF_HOST_INTR2_REG, hostIntMaskRegMask);
    
}

/*****************************************************************************
 * FUNCTION: HostInterruptRegInit
 *
 * RETURNS: N/A
 *
 * PARAMS:
 *      hostIntrMaskRegMask - The bit mask to be modified.
 *      state -  one of WF_EXINT_DISABLE, WF_EXINT_ENABLE where
 *                Disable implies clearing the bits and enable sets the bits.
 *
 *
 *  NOTES: Initializes the 8-bit Host Interrupt register on the MRF24W with the
 *      specified mask value either setting or clearing the mask register
 *      as determined by the input parameter state.  The process requires
 *      2 spi operations which are performed in a blocking fashion.  The
 *      function does not return until both spi operations have completed.
 *****************************************************************************/
static void HostInterruptRegInit(UINT8 hostIntrMaskRegMask,
                                 UINT8 state)
{
    UINT8 hostIntMaskValue;

    /* Host Int Register is a status register where each bit indicates a specific event  */
    /* has occurred. In addition, writing a 1 to a bit location in this register clears  */
    /* the event.                                                                        */

    /* Host Int Mask Register is used to enable those events activated in Host Int Register */
    /* to cause an interrupt to the host                                                    */

    /* read current state of Host Interrupt Mask register */
    hostIntMaskValue = Read8BitWFRegister(WF_HOST_MASK_REG);

    /* if caller is disabling a set of interrupts */
    if (state == WF_INT_DISABLE)
    {
        /* zero out that set of interrupts in the interrupt mask copy */
        hostIntMaskValue = (hostIntMaskValue & ~hostIntrMaskRegMask);
    }
    /* else caller is enabling a set of interrupts */
    else
    {
        /* set to 1 that set of interrupts in the interrupt mask copy */
        hostIntMaskValue = (hostIntMaskValue & ~hostIntrMaskRegMask) | hostIntrMaskRegMask;
    }

    /* write out new interrupt mask value */
    Write8BitWFRegister(WF_HOST_MASK_REG, hostIntMaskValue);

    /* ensure that pending interrupts from those updated interrupts are cleared */
    Write8BitWFRegister(WF_HOST_INTR_REG, hostIntrMaskRegMask);
    
    
}


/*****************************************************************************
 * FUNCTION: WFEintHandler
 *
 * RETURNS: N/A
 *
 * PARAMS:
 *      N/A
 *
 *
 *  NOTES: This function must be called once, each time an external interrupt
 *      is received from the WiFi device.   The WiFi Driver will schedule any
 *      subsequent SPI communication to process the interrupt.
 *
 * IMPORTANT NOTE: This function, and functions that are called by this function
 *                 must NOT use local variables.  The PIC18, or any other processor
 *                 that uses overlay memory will corrupt the logical stack within
 *                 overlay memory if the interrupt uses local variables.  
 *                 If local variables are used within an interrupt routine the toolchain 
 *                 cannot properly determine how not to overwrite local variables in 
 *                 non-interrupt releated functions, specifically the function that was 
 *                 interrupted.
 *****************************************************************************/
void WFEintHandler(void)
{
    /*--------------------------------------------------------*/    
    /* if driver is waiting for a RAW Move Complete interrupt */
    /*--------------------------------------------------------*/
    if (RawMoveState.waitingForRawMoveCompleteInterrupt)
    {
        /* read hostInt register and hostIntMask register to determine cause of interrupt */
        g_EintHostIntRegValue      = Read8BitWFRegister(WF_HOST_INTR_REG);
        g_EintHostIntMaskRegValue  = Read8BitWFRegister(WF_HOST_MASK_REG);
    
        // AND the two registers together to determine which active, enabled interrupt has occurred
        g_EintHostInt = g_EintHostIntRegValue & g_EintHostIntMaskRegValue;

        /* if a RAW0/RAW1 Move Complete interrupt occurred or a level 2 interrupt occurred, indicating */
        /* that a RAW2-5 Move Complete interrupt occurred                                           */
        if (g_EintHostInt & (WF_HOST_INT_MASK_RAW_0_INT_0 | WF_HOST_INT_MASK_RAW_1_INT_0 | WF_HOST_INT_MASK_INT2))
        {
            /* save the copy of the active interrupts */
            RawMoveState.rawInterrupt = g_EintHostInt;
            RawMoveState.waitingForRawMoveCompleteInterrupt = FALSE;
            
            /* if no other interrupts occurred other than a RAW0/RAW1/RAW2/RAW3/RAW4 Raw Move Complete */
            if((g_EintHostInt & ~(WF_HOST_INT_MASK_RAW_0_INT_0 | WF_HOST_INT_MASK_RAW_1_INT_0 | WF_HOST_INT_MASK_INT2)) == 0)
            {
                /* clear the RAW interrupts, re-enable interrupts, and exit */
                Write8BitWFRegister(WF_HOST_INTR_REG, (WF_HOST_INT_MASK_RAW_0_INT_0 | 
                                                       WF_HOST_INT_MASK_RAW_1_INT_0 | 
                                                       WF_HOST_INT_MASK_INT2));
                Write16BitWFRegister(WF_HOST_INTR2_REG, (WF_HOST_INT_MASK_RAW_2_INT_0 | 
                                                         WF_HOST_INT_MASK_RAW_3_INT_0 | 
                                                         WF_HOST_INT_MASK_RAW_4_INT_0 |
                                                         WF_HOST_INT_MASK_RAW_5_INT_0));

                WF_EintEnable();
                return;
            }
            /* else we got a RAW0/RAW1/RAW2/RAW3/RAW4/RAW5 Raw Move Completet interrupt, but, there is also at */
            /* least one other interrupt present                                                               */
            else
            {
                // save the other interrupts and clear them, along with the Raw Move Complete interrupts
                // keep interrupts disabled
                Write16BitWFRegister(WF_HOST_INTR2_REG, (WF_HOST_INT_MASK_RAW_2_INT_0   | 
                                                         WF_HOST_INT_MASK_RAW_3_INT_0   | 
                                                         WF_HOST_INT_MASK_RAW_4_INT_0   |
                                                         WF_HOST_INT_MASK_RAW_5_INT_0));
                g_HostIntSaved |= (g_EintHostInt & ~(WF_HOST_INT_MASK_RAW_0_INT_0 | WF_HOST_INT_MASK_RAW_1_INT_0 | WF_HOST_INT_MASK_INT2));
                Write8BitWFRegister(WF_HOST_INTR_REG, g_EintHostInt);
                
            }
        }
        /*--------------------------------------------------------------------------------------------------*/        
        /* else we did not get a 'RAW Move Complete' interrupt, but we did get at least one other interrupt */
        /*--------------------------------------------------------------------------------------------------*/
        else
        {
            g_HostIntSaved |= g_EintHostInt;
            Write8BitWFRegister(WF_HOST_INTR_REG, g_EintHostInt);
            WF_EintEnable();
        }
    }

    // Once we're in here, external interrupts have already been disabled so no need to call WF_EintDisable() in here
   
    /* notify state machine that an interrupt occurred */
    g_ExIntNeedsServicing = TRUE;
}


void ReenablePowerSaveMode(void)
{
#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
    g_EnablePowerSaveMode = TRUE;
#endif    
}  


/*****************************************************************************
 * FUNCTION: WFHardwareInit
 *
 * RETURNS:  error code
 *
 * PARAMS:   None
 *
 *  NOTES:   Initializes CPU Host hardware interfaces (SPI, External Interrupt).
 *           Also resets the MRF24W.
 *****************************************************************************/
void WFHardwareInit(void)
{
    UINT8  mask8;
    UINT16 mask16;
    g_MgmtReadMsgReady = FALSE;
    g_ExIntNeedsServicing = FALSE;

    RawMoveState.rawInterrupt  = 0;
    RawMoveState.waitingForRawMoveCompleteInterrupt = FALSE;   /* not waiting for RAW move complete */

    /* initialize the SPI interface */
    WF_SpiInit();

    /* Toggle the module into and then out of hibernate */
    WF_SetCE_N(WF_HIGH); /* disable module */
    WF_SetCE_N(WF_LOW);  /* enable module  */

    /* Toggle the module into and out of reset */
    WF_SetRST_N(WF_LOW);            // put module into reset
    WF_SetRST_N(WF_HIGH);           // take module out of of reset

    /* Silicon work-around -- needed for A1 silicon to initialize PLL values correctly */
    ResetPll(); 

    /* Soft reset the MRF24W (using SPI bus to write/read MRF24W registers */
    ChipReset();
    
    /* disable the interrupts gated by the 16-bit host int register */
    HostInterrupt2RegInit(WF_HOST_2_INT_MASK_ALL_INT, (UINT16)WF_INT_DISABLE);
    
    /* disable the interrupts gated the by main 8-bit host int register */
    HostInterruptRegInit(WF_HOST_INT_MASK_ALL_INT, WF_INT_DISABLE);
    
    /* Initialize the External Interrupt for the MRF24W allowing the MRF24W to interrupt */
    /* the Host from this point forward.                                                       */
    WF_EintInit();
    WF_EintEnable();
    
    
    /* enable the following MRF24W interrupts in the INT1 8-bit register */
    mask8 = (WF_HOST_INT_MASK_FIFO_1_THRESHOLD |     /* Mgmt Rx Msg interrupt                  */
             WF_HOST_INT_MASK_FIFO_0_THRESHOLD |     /* Data Rx Msg interrupt                  */
             WF_HOST_INT_MASK_RAW_0_INT_0      |     /* RAW0 Move Complete (Data Rx) interrupt */
             WF_HOST_INT_MASK_RAW_1_INT_0      |     /* RAW1 Move Complete (Data Tx) interrupt */
             WF_HOST_INT_MASK_INT2);                 /* Interrupt 2 interrupt                  */
    HostInterruptRegInit(mask8, WF_INT_ENABLE);

    /* enable the following MRF24W interrupts in the INT2 16-bit register */
    mask16 = (WF_HOST_INT_MASK_RAW_2_INT_0     |    /* RAW2 Move Complete (Mgmt Rx) interrupt */
              WF_HOST_INT_MASK_RAW_3_INT_0     |    /* RAW3 Move Complete (Mgmt Tx) interrupt */
              WF_HOST_INT_MASK_RAW_4_INT_0     |    /* RAW4 Move Complete (Scratch) interrupt */
              WF_HOST_INT_MASK_RAW_5_INT_0     |    /* RAW5 Move Complete (Scratch) interrupt */
              WF_HOST_INT_MASK_MAIL_BOX_0_WRT);
    HostInterrupt2RegInit(mask16, WF_INT_ENABLE);

     /* Disable PS-Poll mode */
    WFConfigureLowPowerMode(WF_LOW_POWER_MODE_OFF);

}



static void ProcessMgmtRxMsg(void)
{
    UINT8 msgType;
    
    /* read first byte from Mgmt Rx message (msg type) */
    RawRead(RAW_MGMT_RX_ID, 0, 1, &msgType);

    /* if not a management response or management indicate then fatal error */
    WF_ASSERT( (msgType == WF_MGMT_CONFIRM_TYPE) || (msgType == WF_MGMT_INDICATE_TYPE) );

    if (msgType == WF_MGMT_CONFIRM_TYPE)
    {
        /* signal that a mgmt response has been received */
        SignalMgmtConfirmReceivedEvent();
    }
    else  /* must be WF_MGMT_INDICATE_TYPE */
    {
        /* handle the mgmt indicate */
        WFProcessMgmtIndicateMsg();
    }    
}


// When bit-banging, determines which SPI port to use based on the type of register we are accessing
static UINT8 GetSpiPortWithBitBang(UINT8 regType)
{
    if (regType == ANALOG_PORT_3_REG_TYPE)
    {
        return 2;
    }
    else if (regType == ANALOG_PORT_2_REG_TYPE)
    {
        return 3;
    }
    else if (regType == ANALOG_PORT_1_REG_TYPE)
    {
        return 1;
    }
    else if (regType == ANALOG_PORT_0_REG_TYPE)
    {
        return 0;
    }
    else
    {
        return 0xff; // should never happen
    }
}

static void WriteAnalogRegisterBitBang(UINT8 regType, UINT16 address, UINT16 value)
{
    UINT8  spiPort;
    UINT16 hrVal;
    UINT8  bitMask8;
    UINT16 bitMask16;
    UINT8  i;
    UINT8  regAddress;

    spiPort = GetSpiPortWithBitBang(regType);   // extract SPI port (0-3) from the register type

    // Enable the on-chip SPI and select the desired bank (0-3)
    hrVal = (HR_HOST_ANA_SPI_EN_MASK | (spiPort << 6));
    Write16BitWFRegister(WF_HOST_RESET_REG, hrVal);

    // create register address byte
    regAddress = (address << 2) | SPI_AUTO_INCREMENT_ENABLED_MASK | SPI_WRITE_MASK;

    // bit-bang the regAddress byte, MS bit to LS bit
    bitMask8 = 0x80;        // start with MS bit of byte being bit-banged out
    for (i = 0; i < 8; ++i)
    {
        hrVal &= ~(HR_HOST_ANA_SPI_DOUT_MASK | HR_HOST_ANA_SPI_CLK_MASK); // zero out DOUT and CLK

        // mask out ADDRESS bit being clocked and write to HOST_ANA_SPI_DOUT (bit 10) in HOST_RESET_REG with the HOST_ANA_SPI_CLK low
        if ((regAddress & bitMask8) > 0)
        {
            hrVal |= HR_HOST_ANA_SPI_DOUT_MASK;
        }    
        Write16BitWFRegister(WF_HOST_RESET_REG, hrVal);

        // now toggle SPI clock high, on rising edge this bit is clocked out
        hrVal |= HR_HOST_ANA_SPI_CLK_MASK;
        Write16BitWFRegister(WF_HOST_RESET_REG, hrVal);

        bitMask8 >>= 1; //  # get to next bit in address byte
    }

    // bit bang data from MS bit to LS bit
    bitMask16 = 0x8000;        // start with MS bit of byte being bit-banged out
    for (i = 0; i < 16; ++i)
    {
        hrVal &= ~(HR_HOST_ANA_SPI_DOUT_MASK | HR_HOST_ANA_SPI_CLK_MASK); // zero out DOUT and CLK

        // mask in data bit being clock out and write to HOST_ANA_SPI_DOUT (bit 10) in HOST_RESET_REG with the HOST_ANA_SPI_CLK low
        if ((value & bitMask16) > 0)
        {
            hrVal |= HR_HOST_ANA_SPI_DOUT_MASK;
        }

        Write16BitWFRegister(WF_HOST_RESET_REG, hrVal);

        // now toggle SPI clock high, on rising edge this bit is clocked out
        hrVal |= HR_HOST_ANA_SPI_CLK_MASK;
        Write16BitWFRegister(WF_HOST_RESET_REG, hrVal);

        bitMask16 = bitMask16 >> 1;  // go to next bit in data byte
    }

    // Disable the on-chip SPI
    hrVal &= ~HR_HOST_ANA_SPI_EN_MASK;
    Write16BitWFRegister(WF_HOST_RESET_REG, hrVal);
}


static void ResetPll(void)
{
    // shuttle MRF24WG workaround (benign to production MRF24WG)
    WriteAnalogRegisterBitBang(ANALOG_PORT_3_REG_TYPE, PLL0_REG, 0x8021);
    WriteAnalogRegisterBitBang(ANALOG_PORT_3_REG_TYPE, PLL0_REG, 0x6021);

    // production MRF24WG workaround (benign to shuttle MRF24WG)
    WriteAnalogRegisterBitBang(ANALOG_PORT_1_REG_TYPE, OSC0_REG, 0x6b80);
    WriteAnalogRegisterBitBang(ANALOG_PORT_1_REG_TYPE, BIAS_REG, 0xc000);
}    


#else
// dummy func to keep compiler happy when module has no executeable code
void WFDriverCom_EmptyFunc(void)
{
}

#endif /* WF_CS_TRIS */
