/******************************************************************************

 MRF24W Driver Set/Get param messages
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFParamMsg.c
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
#if defined(WF_CS_TRIS)


/*
*********************************************************************************************************
*                                           DEFINES                               
*********************************************************************************************************
*/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_PARAM_MSG
#endif

#define MSG_PARAM_START_DATA_INDEX          (6)
#define MULTICAST_ADDRESS                   (6)
#define ADDRESS_FILTER_DEACTIVATE           (0)

#define ENABLE_MRF24WB0M                    (1)


/*******************************************************************************
  Function:    
    void WFEnableMRF24WB0MMode(void)

  Summary:
    Must be called to configure the MRF24WB0M for operations.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WFEnableMRF24WB0MMode(void)
{
    UINT8 buf[1] = {ENABLE_MRF24WB0M};
    
    SendSetParamMsg(PARAM_MRF24WB0M, buf, sizeof(buf)); 
}    

/*******************************************************************************
  Function:    
    void WFEnableBroadcastProbeResponse(void)

  Summary:
    This allows MRF24W to broadcast probe response in Adhoc mode

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    None.    

  Returns:
    None.    
      
  Remarks:
    None.    
  *****************************************************************************/

void WFEnableBroadcastProbeResponse(void)
{
    UINT8 buf[1] = {1};
    
    SendSetParamMsg(PARAM_BROADCAST_PROBE_RESPONSE, buf, sizeof(buf)); 
}   

/*******************************************************************************
  Function:    
  WF_SetLinkDownThreshold(UINT8 threshold)

  Summary:
    Can be called to set link down threshold

  Description:

  Precondition:
  MACInit must be called first.

  Parameters:
    threshold -- if transmission fails successively over this threshold, then RF reports WF_LINK_DOWN 
    event.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/

void WF_SetLinkDownThreshold(UINT8 threshold)
{ 
    SendSetParamMsg(PARAM_LINK_DOWN_THRESHOLD, &threshold, sizeof(threshold)); 
}  

#if !defined(MRF24WG)
/*******************************************************************************
  Function:    
    void WFEnableDeferredPowerSave(void)

  Summary:
    This allows MRF24W to enter power save after DHCP process done

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    None.    

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/

void WFEnableDeferredPowerSave(void)
{
    UINT8 buf[1] = {1};
    
    SendSetParamMsg(PARAM_DEFERRED_POWERSAVE, buf, sizeof(buf)); 
}  
#endif

#if defined (WF_AGGRESSIVE_PS)
#if !defined(MRF24WG)
/*******************************************************************************
  Function:    
    void WFEnableAggressivePowerSave(void)

  Summary:
    This allows MRF24W to turn off RF power quicker in PS mode

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    None.    

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/

void WFEnableAggressivePowerSave(void)
{
    UINT8 buf[1] = {1};
    
    SendSetParamMsg(PARAM_AGGRESSIVE_PS, buf, sizeof(buf)); 
}   
#endif
#endif

/*******************************************************************************
  Function:    
    void WFGetMRF24WB0MVersion(UINT8 *p_version)

  Summary:
    Retrieves the MRF24W version from the device.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_version - Pointer to location to store version number.

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WFGetMRF24WB0MVersion(UINT8 *p_version)
{
    SendGetParamMsg(PARAM_MRF24WB0M, p_version, 1);
}    

/*******************************************************************************
  Function:    
    void WF_GetDeviceInfo(tWFDeviceInfo *p_deviceInfo)

  Summary:
    Retrieves WF device information

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_deviceInfo - Pointer where device info will be written

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_GetDeviceInfo(tWFDeviceInfo *p_deviceInfo)
{
    UINT8  msgData[2];

    SendGetParamMsg(PARAM_SYSTEM_VERSION, msgData, sizeof(msgData));
    
    p_deviceInfo->deviceType   = MRF24WB0M_DEVICE;
    p_deviceInfo->romVersion   = msgData[0];
    p_deviceInfo->patchVersion = msgData[1];
}

/*******************************************************************************
  Function:    
    void WF_SetMacAddress(UINT8 *p_mac)

  Summary:
    Uses a different MAC address for the MRF24W

  Description:
    Directs the MRF24W to use the input MAC address instead of its 
    factory-default MAC address.  This function does not overwrite the factory 
    default, which is in FLASH memory – it simply tells the MRF24W to use a 
    different MAC.

  Precondition:
    MACInit must be called first.  Cannot be called when the MRF24W is in a
    connected state.

  Parameters:
    p_mac  - Pointer to 6-byte MAC that will be sent to MRF24W

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_SetMacAddress(UINT8 *p_mac)
{
    SendSetParamMsg(PARAM_MAC_ADDRESS, p_mac, WF_MAC_ADDRESS_LENGTH); 
}   

/*******************************************************************************
  Function:    
    void WF_GetMacAddress(UINT8 *p_mac)

  Summary:
    Retrieves the MRF24W MAC address

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_mac  - Pointer where mac will be written (must point to a 6-byte buffer)

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_GetMacAddress(UINT8 *p_mac)
{
    SendGetParamMsg(PARAM_MAC_ADDRESS, p_mac, WF_MAC_ADDRESS_LENGTH);
}   

#if defined(WF_USE_MULTICAST_FUNCTIONS)
/*******************************************************************************
  Function:    
    void WF_SetMultiCastFilter(UINT8 multicastFilterId, 
                               UINT8 multicastAddress[6])

  Summary:
    Sets a multicast address filter using one of the two multicast filters.

  Description:
    This function allows the application to configure up to two Multicast 
    Address Filters on the MRF24W.  If two active multicast filters are set 
    up they are OR’d together – the MRF24W will receive and pass to the Host 
    CPU received packets from either multicast address.  
    The allowable values for the multicast filter are:
    * WF_MULTICAST_FILTER_1
    * WF_MULTICAST_FILTER_2

    By default, both Multicast Filters are inactive.

  Precondition:
    MACInit must be called first.

  Parameters:
    multicastFilterId - WF_MULTICAST_FILTER_1 or WF_MULTICAST_FILTER_2
    multicastAddress  - 6-byte address (all 0xFF will inactivate the filter)

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_SetMultiCastFilter(UINT8 multicastFilterId,
                           UINT8 multicastAddress[6])
{
    int i;
    BOOL deactivateFlag = TRUE;
    UINT8 msgData[8];
    
    WF_ASSERT( (multicastFilterId == WF_MULTICAST_FILTER_1) || (multicastFilterId == WF_MULTICAST_FILTER_2) );
    
    /* check if all 6 bytes of the address are 0xff, implying that the caller wants to deactivate */
    /* the multicast filter.                                                                      */
    for (i = 0; i < 6; ++i)
    {
        /* if any byte is not 0xff then a presume a valid multicast address */
        if (multicastAddress[i] != 0xff)
        {
            deactivateFlag = FALSE;
            break;
        }    
    }  
    
    msgData[0] = multicastFilterId;     /* Address Compare Register number to use   */
    if (deactivateFlag)
    {
        msgData[1] = ADDRESS_FILTER_DEACTIVATE;
    }    
    else
    {
        msgData[1] = MULTICAST_ADDRESS;     /* type of address being used in the filter */  
    }
    memcpy(&msgData[2], (void *)multicastAddress, WF_MAC_ADDRESS_LENGTH);

    SendSetParamMsg(PARAM_COMPARE_ADDRESS, msgData, sizeof(msgData) ); 
}    

/*******************************************************************************
  Function:    
    void WF_GetMultiCastFilter(UINT8 multicastFilterId,
                               UINT8 multicastAddress[6])

  Summary:
    Gets a multicast address filter from one of the two multicast filters.

  Description:
    Gets the current state of the specified Multicast Filter.  

    Normally would call SendGetParamMsg, but this GetParam returns all 6 address 
    filters + 2 more bytes for a total of 48 bytes plus header. So, doing this 
    msg manually to not require a large stack allocation to hold all the data.                                                                                  
    
    Exact format of returned message is:                                                                
    [0]     -- always mgmt response (2)
    [1]     -- always WF_GET_PARAM_SUBTYPE (16)
    [2]     -- result (1 if successful)
    [3]     -- mac state (not used)
    [4]     -- data length (length of response data starting at index 6)
    [5]     -- not used
    [6-11]  -- Compare Address 0 address
    [12]    -- Compare Address 0 group
    [13]    -- Compare Address 0 type
    [14-19] -- Compare Address 1 address
    [20]    -- Compare Address 1 group
    [21]    -- Compare Address 1 type
    [22-27] -- Compare Address 2 address
    [28]    -- Compare Address 2 group
    [29]    -- Compare Address 2 type
    [30-35] -- Compare Address 3 address
    [36]    -- Compare Address 3 group
    [37]    -- Compare Address 3 type
    [38-43] -- Compare Address 4 address
    [44]    -- Compare Address 4 group
    [45]    -- Compare Address 4 type
    [46-51] -- Compare Address 5 address
    [52]    -- Compare Address 5 group
    [53]    -- Compare Address 5 type

  Precondition:
    MACInit must be called first.

  Parameters:
    multicastFilterId - WF_MULTICAST_FILTER_1 or WF_MULTICAST_FILTER_2
    multicastAddress - 6-byte address

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_GetMultiCastFilter(UINT8 multicastFilterId,
                           UINT8 multicastAddress[6])
{
    UINT8  hdr[4];
    UINT8  paramData[8];
    UINT8  startIndex;

    WF_ASSERT( (multicastFilterId == WF_MULTICAST_FILTER_1) || (multicastFilterId == WF_MULTICAST_FILTER_2) );

    
    hdr[0] = WF_MGMT_REQUEST_TYPE;
    hdr[1] = WF_GET_PARAM_SUBTYPE;
    hdr[2] = 0x00;                      /* MS 8 bits of param Id, always 0 */
    hdr[3] = PARAM_COMPARE_ADDRESS;     /* LS 8 bits of param ID           */
    
    SendMgmtMsg(hdr,             /* header           */
                sizeof(hdr),     /* size of header   */
                NULL,            /* no data          */
                0);              /* no data          */

    if (multicastFilterId == WF_MULTICAST_FILTER_1)
    {
        startIndex = 38; /* index of first byte of index 4 address filter */
    }    
    else
    {
        startIndex = 46; /* index of first byte of index 5 address filter */
    }    

    WaitForMgmtResponseAndReadData(WF_GET_PARAM_SUBTYPE,       /* expected subtype                           */ 
                                   sizeof(paramData),          /* num data bytes to read                     */
                                   startIndex,                 /* starting at this index                     */
                                   paramData);                 /* write the response data here               */
    
    
    memcpy((void *)multicastAddress, (void *)&paramData[0], 6);
}                                   

#endif /* WF_USE_MULTICAST_FUNCTIONS */

/*******************************************************************************
  Function:    
    void WF_SetTxDataConfirm(UINT8 state)

  Summary:
    Enables or disables Tx data confirmation management messages.

  Description:
    Enables or disables the MRF24W Tx data confirm mgmt message.  Data
    confirms should always be disabled.

  Precondition:
    MACInit must be called first.

  Parameters:
    state - WF_DISABLED or WF_ENABLED

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_SetTxDataConfirm(UINT8 state)
{
    SendSetParamMsg(PARAM_CONFIRM_DATA_TX_REQ, &state, 1); 
}   

/*******************************************************************************
  Function:    
    void WF_GetTxDataConfirm(UINT8 *p_txDataConfirm)

  Summary:
    Retrives the current state of Tx data confirmation management messages.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_txDataConfirm - Pointer to location where Tx data confirmation state is

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_GetTxDataConfirm(UINT8 *p_txDataConfirm)
{    
    SendGetParamMsg(PARAM_CONFIRM_DATA_TX_REQ, p_txDataConfirm, 1);
}   

/*******************************************************************************
  Function:    
    void WF_SetRegionalDomain(UINT8 regionalDomain)

  Summary:
    Enables or disables the MRF24W Regional Domain.

  Description:
    Sets the regional domain on the MRF24W.  Note that this function does not 
    overwrite the factory-set regional domain in FLASH.  By default the 
    MRF24W will use the factory-set regional domain.  It is invalid to call 
    this function while in a connected state.

    Valid values for the regional domain are:
    * WF_DOMAIN_FCC     
    * WF_DOMAIN_IC      
    * WF_DOMAIN_ETSI    
    * WF_DOMAIN_SPAIN   
    * WF_DOMAIN_FRANCE  
    * WF_DOMAIN_JAPAN_A 
    * WF_DOMAIN_JAPAN_B

  Precondition:
    MACInit must be called first.  This function must not be called while in a
    connected state.

  Parameters:
    regionalDomain - Value to set the regional domain to

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_SetRegionalDomain(UINT8 regionalDomain)
{
    SendSetParamMsg(PARAM_REGIONAL_DOMAIN, &regionalDomain, 1); 
} 

/*******************************************************************************
  Function:    
    void WF_GetRegionalDomain(UINT8 *p_regionalDomain)

  Summary:
    Retrieves the MRF24W Regional domain

  Description:
    Gets the regional domain on the MRF24W.  Allowable values are:
    * WF_DOMAIN_FCC     
    * WF_DOMAIN_IC      
    * WF_DOMAIN_ETSI    
    * WF_DOMAIN_SPAIN   
    * WF_DOMAIN_FRANCE  
    * WF_DOMAIN_JAPAN_A 
    * WF_DOMAIN_JAPAN_B

  Precondition:
    MACInit must be called first.

  Parameters:
    p_regionalDomain - Pointer where the regional domain value will be written

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_GetRegionalDomain(UINT8 *p_regionalDomain)
{
    SendGetParamMsg(PARAM_REGIONAL_DOMAIN, p_regionalDomain, 1);
}   

/*******************************************************************************
  Function:    
    void WF_SetRtsThreshold(UINT16 rtsThreshold)

  Summary:
    Sets the RTS Threshold.

  Description:
    Sets the RTS/CTS packet size threshold for when RTS/CTS frame will be sent.  
    The default is 2347 bytes – the maximum for 802.11.  It is recommended that 
    the user leave the default at 2347 until they understand the performance and 
    power ramifications of setting it smaller.  Valid values are from 0 to
    WF_RTS_THRESHOLD_MAX (2347).

  Precondition:
    MACInit must be called first.

  Parameters:
    rtsThreshold - Value of the packet size threshold

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_SetRtsThreshold(UINT16 rtsThreshold)
{
    WF_ASSERT(rtsThreshold <= WF_RTS_THRESHOLD_MAX);
    
     /* correct endianness before sending message */
    rtsThreshold = HSTOWFS(rtsThreshold);
    
    SendSetParamMsg(PARAM_RTS_THRESHOLD, (UINT8 *)&rtsThreshold, sizeof(rtsThreshold)); 
} 

/*******************************************************************************
  Function:    
    void WF_GetRtsThreshold(UINT16 *p_rtsThreshold)

  Summary:
    Gets the RTS Threshold

  Description:
    Gets the RTS/CTS packet size threshold.  

  Precondition:
    MACInit must be called first.

  Parameters:
    p_rtsThreshold - Pointer to where RTS threshold is written

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_GetRtsThreshold(UINT16 *p_rtsThreshold)
{
    SendGetParamMsg(PARAM_RTS_THRESHOLD, (UINT8 *)p_rtsThreshold, sizeof(UINT16)); 

     /* correct endianness before sending message */
    *p_rtsThreshold = HSTOWFS(*p_rtsThreshold);
} 

/*******************************************************************************
  Function:    
    void WF_GetMacStats(tWFMacStats *p_macStats)

  Summary:
    Gets MAC statistics.  

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_macStats - Pointer to where MAC statistics are written

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_GetMacStats(tWFMacStats *p_macStats)
{
    UINT32 *p_value;
    UINT8  numElements;
    UINT8  i;
    
    SendGetParamMsg(PARAM_STAT_COUNTERS, (UINT8 *)p_macStats, sizeof(tWFMacStats));
    
    /* calculate number of 32-bit counters in the stats structure and point to first element */
    numElements = sizeof(tWFMacStats) / sizeof(UINT32);
    p_value = (UINT32 *)p_macStats;
    
    /* correct endianness on all counters in structure */    
    for (i = 0; i < numElements; ++i)
    {
        *p_value = WFTOHL(*p_value);
        ++p_value;        
    }    
}

/*******************************************************************************
  Function:    
    void SendSetParamMsg(UINT8 paramType, 
                         UINT8 *p_paramData, 
                         UINT8 paramDataLength)

  Summary:
    Sends a SetParam Mgmt request to MRF24W and waits for response.

  Description:
    Index Set Param Request
    ----- -----------------
    0     type            (always 0x02 signifying a mgmt request)
    1     subtype         (always 0x10 signifying a Set Param Msg)
    2     param ID [msb]  (MS byte of parameter ID being requested, e.g. 
                           PARAM_SYSTEM_VERSION)
    3     param ID [lsb]  (LS byte of parameter ID being requested. e.g. 
                           PARAM_SYSTEM_VERSION)
    4     payload[0]      first byte of param data
    N     payload[n]      Nth byte of payload data
            
    Index  Set Param Response
    ------ ------------------
    0      type           (always 0x02 signifying a mgmt response)
    1      subtype        (always 0x10 signifying a Param Response Msg
    2      result         (1 if successful -- any other value indicates failure
    3      mac state      (not used)

  Precondition:
    MACInit must be called first.

  Parameters:
    paramType - Parameter type associated with the SetParam msg.
    p_paramData - pointer to parameter data
    paramDataLength - Number of bytes pointed to by p_paramData

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void SendSetParamMsg(UINT8 paramType, 
                     UINT8 *p_paramData, 
                     UINT8 paramDataLength)
{
    UINT8 hdr[4];
    
    hdr[0] = WF_MGMT_REQUEST_TYPE;
    hdr[1] = WF_SET_PARAM_SUBTYPE;
    hdr[2] = 0x00;                      /* MS 8 bits of param Id, always 0 */
    hdr[3] = paramType;                 /* LS 8 bits of param ID           */

    SendMgmtMsg(hdr,               /* header            */
                sizeof(hdr),       /* size of header    */
                p_paramData,       /* param data        */
                paramDataLength);  /* param data length */

    /* wait for MRF24W management response; free response because not needed */
    WaitForMgmtResponse(WF_SET_PARAM_SUBTYPE, FREE_MGMT_BUFFER); 
}    

/*******************************************************************************
  Function:    
    void SendGetParamMsg(UINT8 paramType, 
                         UINT8 *p_paramData, 
                         UINT8 paramDataLength)

  Summary:
    Sends a GetParam Mgmt request to MRF24W and waits for response.

  Description:  
    After response is received the param data is read from message and written 
    to p_paramData.  It is up to the caller to fix up endianness.
     
    Index Get Param Request
    ----- -----------------
    0     type            (always 0x02 signifying a mgmt request)
    1     subtype         (always 0x10 signifying a Get Param Msg)
    2     param ID [msb]  (MS byte of parameter ID being requested, e.g. 
                           PARAM_SYSTEM_VERSION)
    3     param ID [lsb]  (LS byte of parameter ID being requested, e.g. 
                           PARAM_SYSTEM_VERSION)
           
    Index  Get Param Response
    ------ ------------------
    0      type           (always 0x02 signifying a mgmt response)
    1      subtype        (always 0x10 signifying a Param Response Msg
    2      result         (1 if successful -- any other value indicates failure
    3      mac state      (not used)
    4      data length    Length of response data starting at index 6 (in bytes)
    5      not used         
    6      Data[0]        first byte of returned parameter data
    N      Data[N]        Nth byte of param data

  Precondition:
    MACInit must be called first.

  Parameters:

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void SendGetParamMsg(UINT8 paramType, 
                     UINT8 *p_paramData, 
                     UINT8 paramDataLength)
{
    UINT8 hdr[4];
    
    hdr[0] = WF_MGMT_REQUEST_TYPE;
    hdr[1] = WF_GET_PARAM_SUBTYPE;
    hdr[2] = 0x00;                      /* MS 8 bits of param Id, always 0 */
    hdr[3] = paramType;                 /* LS 8 bits of param ID           */
    
    SendMgmtMsg(hdr,             /* header           */
                sizeof(hdr),     /* size of header   */
                NULL,            /* no data          */
                0);              /* no data          */

    WaitForMgmtResponseAndReadData(WF_GET_PARAM_SUBTYPE,       /* expected subtype                           */ 
                                   paramDataLength,            /* num data bytes to read                     */
                                   MSG_PARAM_START_DATA_INDEX, /* data for GetParam always starts at index 6 */
                                   p_paramData);               /* write the response data here               */
}    
#endif /* WF_CS_TRIS */
