/******************************************************************************

 MRF24W Driver Tx Power functions
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller (MRF24WB0MA/B, MRF24WG0MA/B)
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFTxPower.c
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

#include "TCPIP Stack/TCPIP.h"
#if defined(WF_CS_TRIS) && defined(WF_USE_TX_POWER_CONTROL_FUNCTIONS)

/*
*********************************************************************************************************
*                                           DEFINES                               
*********************************************************************************************************
*/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_TX_POWER
#endif

#define TX_THOTTLE_ENABLE_BIT_MASK   ((UINT8)0x01)


#define WF_ZERO_DB_VALUE        (64)   

#define DEFAULT_MIN_TX_POWER        (-10) /* dB */


/*
*********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES                              
*********************************************************************************************************
*/

#if defined (MRF24WG)
/*******************************************************************************
  Function:    
    void WF_TxPowerSetMax(INT8 maxTxPower)

  Summary:
    Sets the Tx max power on the MRF24WG0MA/B.

  Description:
    After initialization the  MRF24WG0MA/B max Tx power is determined by a 
    factory-set value.  This function can set a different maximum 
    Tx power levels.  However, this function can never set a maximum Tx power 
    greater than the factory-set value, which can be read via 
    WF_TxPowerGetFactoryMax(). 

  Precondition:
    MACInit must be called first.

  Parameters:
    maxTxPower - valid range (0 to 18 dBm)

  Returns:
     None.
      
  Remarks:
      No conversion of units needed, input to  MRF24WG0MA/B is in dBm.
  *****************************************************************************/
void WF_TxPowerSetMax(INT8 maxTxPower)
{
    INT8  factoryMaxPower;
    UINT8 msgData[2];
    INT16 max = (INT16)maxTxPower;

    WF_TxPowerGetFactoryMax(&factoryMaxPower);
    WF_ASSERT(maxTxPower <= factoryMaxPower); /* cannot set max tx power greater than factor-set max tx power */
    
    msgData[0] = (INT8)(max >> 8);      /* msb of max power */
    msgData[1] = (INT8)(max & 0xff);    /* lsb of max power */
    
    SendSetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData)); 
}

/*******************************************************************************
  Function:    
    void WF_TxPowerGetMax(INT8 *p_maxTxPower)

  Summary:
    Gets the Tx max power on the MRF24WG0MA/B.

  Description:
    After initialization the  MRF24WG0MA/B max Tx power is determined by a 
    factory-set value.  This function can set a different maximum 
    Tx power levels.  However, this function can never set a maximum Tx power 
    greater than the factory-set value, which can be read via 
    WF_TxPowerGetFactoryMax(). 

  Precondition:
    MACInit must be called first.

  Parameters:
    p_maxTxPower - Pointer to location to write the maxTxPower

  Returns:
     None.
      
  Remarks:
      No conversion of units needed, input to MRF24WG0MA/B is in dBm.
  *****************************************************************************/ 
void WF_TxPowerGetMax(INT8 *p_maxTxPower)
{
    UINT8 msgData[6];
    INT16 tmp;
    
    SendGetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData)); 

    /* max tx power is a signed 16-bit value stored in the [1:0] msg data */
    tmp = ((INT16)(msgData[0]) << 8);
    tmp |= (INT16)msgData[1];
    *p_maxTxPower = (INT8)tmp;
}
#else /* !defined (MRF24WG) */
/*******************************************************************************
  Function:    
    void WF_TxPowerSetMinMax(INT8 minTxPower, INT8 maxTxPower)

  Summary:
    Sets the Tx min and max power on the MRF24WB0MA/B.

  Description:
    After initialization the MRF24WB0MA/B max Tx power is determined by a 
    factory-set value.  This function can set a different minimum and maximum 
    Tx power levels.  However, this function can never set a maximum Tx power 
    greater than the factory-set value, which can be read via 
    WF_TxPowerGetFactoryMax(). 

  Precondition:
    MACInit must be called first.

  Parameters:
    minTxPower - Desired minTxPower (-10 to 10dB) 
    maxTxPower - Desired maxTxPower (-10 to 10dB)

  Returns:
     None.
      
  Remarks:
      No conversion of units needed, input to MRF24WB0MA/B is in dB.
  *****************************************************************************/
void WF_TxPowerSetMinMax(INT8 minTxPower, INT8 maxTxPower)
{
    INT8  factoryMaxPower;
    UINT8 msgData[4];  /* need to input to chip two signed 16-bit values, max power followed by min power */
    INT16 max;
    INT16 min;
    
    max = (INT16)maxTxPower;
    min = (INT16)minTxPower;
    
    WF_ASSERT(minTxPower <= maxTxPower);

    WF_TxPowerGetFactoryMax(&factoryMaxPower);
    WF_ASSERT(maxTxPower <= factoryMaxPower); /* cannot set max tx power greater than factor-set max tx power */
    
    msgData[0] = (INT8)(max >> 8);      /* msb of max power */
    msgData[1] = (INT8)(max & 0xff);    /* lsb of max power */

    msgData[2] = (INT8)(min >> 8);      /* msb of min power */
    msgData[3] = (INT8)(min & 0xff);    /* lsb of min power */
    
    SendSetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData)); 
}

/*******************************************************************************
  Function:    
    void WF_TxPowerGetMinMax(INT8 *p_minTxPower, INT8 *p_maxTxPower)

  Summary:
    Gets the Tx min and max power on the MRF24WB0MA/B.

  Description:
    After initialization the MRF24WB0MA/B max Tx power is determined by a 
    factory-set value.  This function can set a different minimum and maximum 
    Tx power levels.  However, this function can never set a maximum Tx power 
    greater than the factory-set value, which can be read via 
    WF_TxPowerGetFactoryMax(). 

  Precondition:
    MACInit must be called first.

  Parameters:
    p_minTxPower - Pointer to location to write the minTxPower
    p_maxTxPower - Pointer to location to write the maxTxPower

  Returns:
     None.
      
  Remarks:
      No conversion of units needed, input to MRF24WB0MA/B is in dB.
  *****************************************************************************/ 
void WF_TxPowerGetMinMax(INT8 *p_minTxPower, INT8 *p_maxTxPower)
{
    UINT8 msgData[6];
    INT16 tmp;
    
    SendGetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData)); 

    /* max tx power is a signed 16-bit value stored in the [1:0] msg data */
    tmp = ((INT16)(msgData[0]) << 8);
    tmp |= (INT16)msgData[1];
    *p_maxTxPower = (INT8)tmp;
    
    /* min tx power is a signed 16-bit value stored in the [3:2] msg data */
    tmp = ((INT16)(msgData[2]) << 8);
    tmp |= (INT16)msgData[3];
    *p_minTxPower = (INT8)tmp;
    
}

/*******************************************************************************
  Function:    
    void WF_FixTxRateWithMaxPower(BOOL oneMegaBps)

  Summary:
    Fix transmission rate with maximum power for MRF24WB0MA/B. 

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    oneMegaBps - When true, that is 1 mbps. Otherwise 2 mbps

  Returns:
     None.
      
  Remarks:
     None.
  *****************************************************************************/  
void WF_FixTxRateWithMaxPower(BOOL oneMegaBps)
{ 
    UINT8 buf[1];

    buf[1] = oneMegaBps ? 0x20 : 0x40;    /* or 2 Mbps */
    SendSetParamMsg(PARAM_TX_THROTTLE_TABLE_ON_OFF, buf, sizeof(buf)); 
}
#endif /* defined (MRF24WG) */
  
/*******************************************************************************
  Function:    
    void WF_TxPowerGetFactoryMax(INT8 *p_factoryMaxTxPower)

  Summary:
    Retrieves the factory-set max Tx power from the MRF24WB0MA/B and MRF24WG0MA/B.

  Description:
     For MRF24WB, desired maxTxPower (-10 to 10 dBm), in 1dB steps
     For MRF24WG, desired maxTxPower ( 0 to 18 dBm), in 1dB steps

  Precondition:
    MACInit must be called first.

  Parameters:
    p_factoryMaxTxPower - 
    for MRF24WB, desired maxTxPower (-10 to 10 dBm), in 1dB steps
    for MRF24WG, desired maxTxPower ( 0 to 18 dBm), in 1dB steps

  Returns:
     None.
      
  Remarks:
     None.
  *****************************************************************************/  
void WF_TxPowerGetFactoryMax(INT8 *p_factoryMaxTxPower)
{
    UINT8 msgData[2];

    /* read max and min factory-set power levels */
    SendGetParamMsg(PARAM_FACTORY_SET_TX_MAX_POWER, msgData, sizeof(msgData)); 

    /* msgData[0] = max power, msgData[1] = min power */
    *p_factoryMaxTxPower = msgData[0];  
}
    
    
    
#endif /* WF_CS_TRIS && WF_USE_TX_POWER_CONTROL_FUNCTIONS */
