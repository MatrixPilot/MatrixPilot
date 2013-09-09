/******************************************************************************

 MRF24W Driver Connection Manager
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller (MRF24WB0MA/B, MRF24WG0MA/B)
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFConnectionManager.c
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


/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_CONNECTION_MANAGER
#endif

/*
*********************************************************************************************************
*                                           LOCAL GLOBAL VARIABLES                               
*********************************************************************************************************
*/

static BOOL g_LogicalConnection = FALSE;


/*******************************************************************************
  Function:    
    void WF_CMConnect(UINT8 CpId)

  Summary:
    Commands the MRF24W to start a connection.

  Description:
    Directs the Connection Manager to scan for and connect to a WiFi network.
    This function does not wait until the connection attempt is successful, but 
    returns immediately.  See WF_ProcessEvent for events that can occur as a 
    result of a connection attempt being successful or not.

    Note that if the Connection Profile being used has WPA or WPA2 security
    enabled and is using a passphrase, the connection manager will first 
    calculate the PSK key, and then start the connection process.  The key 
    calculation can take up to 30 seconds.

  Precondition:
    MACInit must be called first.

  Parameters:
    CpId - If this value is equal to an existing Connection Profile�s ID than 
            only that Connection Profile will be used to attempt a connection to 
            a WiFi network.  
            If this value is set to WF_CM_CONNECT_USING_LIST then the 
            connectionProfileList will be used to connect, starting with the 
            first Connection Profile in the list.

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_CMConnect(UINT8 CpId)
{
    UINT8  hdrBuf[4];

    /* Write out header portion of msg (which is whole msg, there is no data) */
    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;    /* indicate this is a mgmt msg     */
    hdrBuf[1] = WF_CM_CONNECT_SUBYTPE;   /* mgmt request subtype            */  
    hdrBuf[2] = CpId;
    hdrBuf[3] = 0;   

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    /* wait for mgmt response, free after it comes in, don't need data bytes */
    WaitForMgmtResponse(WF_CM_CONNECT_SUBYTPE, FREE_MGMT_BUFFER);
}

/*******************************************************************************
  Function:    
    static BOOL WF_CMIsDisconnectAllowed(void)

  Summary:
    Determines whether disconnection is allowed. 

  Description:
    Determines current connection status and to determine whether disconnection is needed. 
    
  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    TRUE - Currently connected. Disconnection is allowed. 
    FALSE - Not connected all all to any network. So no need for disconnection process.
      
  Remarks:
    None.
  *****************************************************************************/
static BOOL
WF_CMIsDisconnectAllowed(void)
{
    UINT8   profileID;
    UINT8   profileIDState;
  
    WF_CMCheckConnectionState(&profileIDState, &profileID);
    if (profileIDState == WF_CSTATE_CONNECTED_INFRASTRUCTURE || profileIDState == WF_CSTATE_CONNECTED_ADHOC)
    {
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
  Function:    
    UINT16 WF_CMDisconnect(void)

  Summary:
    Commands the MRF24W to close any open connections and/or to cease
    attempting to connect.

  Description:
    Directs the Connection Manager to close any open connection or connection 
    attempt in progress.  No further attempts to connect are taken until 
    WF_CMConnect() is called.  
    
  Precondition:
    MACInit must be called.

  Parameters:
    None.

  Returns:
    Operation results. Success or Failure
      
  Remarks:
    None.
  *****************************************************************************/
UINT16 WF_CMDisconnect(void)
{
    UINT8  hdrBuf[2];

    /* WARNING !!! :
    * Disconnect is allowed only in connected state.
    * If module FW is in the midst of connection (or reconnection) process, then
    * disconnect can hammer connection process, and furthermore it may cause
    * fatal failure in module FW operation. To be safe to use disconnect, we strongly
    * recommend you to disable module FW connection manager by uncommenting
    * #define DISABLE_MODULE_FW_CONNECT_MANAGER_IN_INFRASTRUCTURE
    * in WF_Config.h
    */
    if (!WF_CMIsDisconnectAllowed())
    {
        return WF_ERROR_DISCONNECT_FAILED;
    }

    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;
    hdrBuf[1] = WF_CM_DISCONNECT_SUBYTPE;

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);
 
    /* wait for mgmt response, free after it comes in, don't need data bytes */
    WaitForMgmtResponse(WF_CM_DISCONNECT_SUBYTPE, FREE_MGMT_BUFFER);

    /* set state to no connection */
    SetLogicalConnectionState(FALSE);
    
    return WF_SUCCESS;
}    
    
/*******************************************************************************
  Function:    
    void WF_CMGetConnectionState(UINT8 *p_state, UINT8 *p_currentCpId)

  Summary:
    Returns the current connection state.
    Caution when using WF_CMGetConnectionState, as it may cause redirection issues 
    when using iPhone iOS6.1, even though redirection on laptop is functional.
    Users are encouraged to use 1 profile ID for MRF24W based on v5 stack SW. This function is
    retained for backward compatibility.
    In v6 stack SW, we are keeping to 1 profile ID for MRF24W and changing stack SW to have
    capability to handle multiple profile IDs. 

  Description:
     Returns the current connection state.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_state - Pointer to location where connection state will be written
    p_currentCpId - Pointer to location of current connection profile ID that
                     is being queried.

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_CMGetConnectionState(UINT8 *p_state, UINT8 *p_currentCpId)
{
    UINT8  hdrBuf[2];
    UINT8  msgData[2];
    
    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;
    hdrBuf[1] = WF_CM_GET_CONNECTION_STATUS_SUBYTPE;

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    /* wait for mgmt response, read data, free after read */
    WaitForMgmtResponseAndReadData(WF_CM_GET_CONNECTION_STATUS_SUBYTPE, 
                                   sizeof(msgData),                  /* num data bytes to read          */
                                   MGMT_RESP_1ST_DATA_BYTE_INDEX,    /* only used if num data bytes > 0 */
                                   msgData);                         /* only used if num data bytes > 0 */
    
    *p_state       = msgData[0];        /* connection state */
    *p_currentCpId = msgData[1];        /* current CpId     */
    
    if ((*p_state == WF_CSTATE_CONNECTED_INFRASTRUCTURE) || (*p_state == WF_CSTATE_CONNECTED_ADHOC))
    {
        SetLogicalConnectionState(TRUE);
    }
    else
    {
        SetLogicalConnectionState(FALSE);
    }        
}  

/*******************************************************************************
  Function:    
    BOOL WFisConnected()

  Summary:
    Query the connection status of the MRF24W.

  Description:
    Determine the fine granularity status of the connection state of the MRF24W.

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    TRUE if the MRF24W is either connected or attempting to connect.
    FALSE for all other conditions.
      
  Remarks:
    None.
  *****************************************************************************/
BOOL WFisConnected()
{
    return g_LogicalConnection;   
}      

/*******************************************************************************
  Function:    
    void SetLogicalConnectionState(BOOL state)

  Summary:
    Sets the logical connection state.

  Description:
    Logically, if the MRF24W is either connected or trying to connect, then
    it is "connected".  For all other scenarios, the MRF24W is "not
    connected".

  Precondition:
    MACInit must be called first.

  Parameters:
    state - Current logical connection state of the MRF24W.

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void SetLogicalConnectionState(BOOL state)
{
    g_LogicalConnection = state;
}


/*******************************************************************************
  Function:    
    void WF_CMCheckConnectionState(UINT8 *p_state, UINT8 *p_currentCpId)

  Summary:
    Returns the current connection state.

  Description:
    Returns the current connection state.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_state - Pointer to location where connection state will be written
    p_currentCpId - Pointer to location of current connection profile ID that
                     is being queried.

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
void WF_CMCheckConnectionState(UINT8 *p_state, UINT8 *p_currentCpId)
{
    UINT8  hdrBuf[2];
    UINT8  msgData[2];
    
    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;
    hdrBuf[1] = WF_CM_GET_CONNECTION_STATUS_SUBYTPE;

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    /* wait for mgmt response, read data, free after read */
    WaitForMgmtResponseAndReadData(WF_CM_GET_CONNECTION_STATUS_SUBYTPE, 
                                   sizeof(msgData),                  /* num data bytes to read          */
                                   MGMT_RESP_1ST_DATA_BYTE_INDEX,    /* only used if num data bytes > 0 */
                                   msgData);                         /* only used if num data bytes > 0 */
    
    *p_state       = msgData[0];        /* connection state */
    *p_currentCpId = msgData[1];        /* current CpId     */
}  

#if defined (MRF24WG)
/*******************************************************************************
  Function:    
    void WF_CMGetConnectContext(tWFConnectContext *p_ctx)

  Summary:
    Retrieves WF connection context for MRF24WG0MA/B

  Description:
    Retrieves WF connection context for MRF24WG0MA/B

  Precondition:
    MACInit must be called first.

  Parameters:
    p_ctx -- pointer where connect context will be written

  Returns:
    None.
      
  Remarks:
      None.
 *****************************************************************************/
void WF_CMGetConnectContext(tWFConnectContext *p_ctx)
{
    SendGetParamMsg(PARAM_CONNECT_CONTEXT, (UINT8 *)p_ctx, sizeof(*p_ctx));
}
#endif /* MRF24WG */ 

#if defined(__C32__)
/*******************************************************************************
  Function:    
    void WF_ConvPassphrase2Key(UINT8 key_len, UINT8 *key, UINT8 ssid_len, UINT8 *ssid)

  Summary:
    Allow host to convert passphrase to key  

  Description:
   DERIVE_KEY_FROM_PASSPHRASE_IN_HOST and __C32__ must be enabled.
   This function is called in WPS or WPA or WPA2 security modes.
   Convert WPS/WPA/WPA2 passphrase to key. Allows host (eg PIC32) to perform conversion of 
   the passphrase to the key by itself instead of relying on RF module FW. This is recommended 
   only for PIC microprocessors that has high computational bandwidth and sufficient memory space. 
   As a benchmark, MRF24WB0M will take 32 sec and MRF24WG0M will take 25 sec for this computation.

  Precondition:
    MACInit must be called first.

  Parameters:
    key_len : key length
    key : passphrase as an input. key as an output
    ssid_len : ssid length
    ssid : ssid

  Returns:
    None.
      
  Remarks:
    MRF24W will generate an event (WF_EVENT_KEY_CALCULATION_REQUEST) to PIC32 and set 
    g_WpsPassphrase.valid to TRUE. Upon receipt of this event, PIC32 will invoke this function 
    WF_ConvPassphrase2Key to convert the passphrase to key. Once this conversion is completed, 
    PIC32 will call WF_SetPSK to pass the converted key to MRF24W.
 *****************************************************************************/
void
WF_ConvPassphrase2Key(UINT8 key_len, UINT8 *key, UINT8 ssid_len, UINT8 *ssid)
{
    UINT8 psk[32];

    key[key_len] = '\0';
    pbkdf2_sha1((const char *)key, (const char *)ssid, ssid_len, 4096, (UINT8 *)psk, 32);
    memcpy(key, psk, 32);
}
#endif /* __C32__ */

#endif /* WF_CS_TRIS */
