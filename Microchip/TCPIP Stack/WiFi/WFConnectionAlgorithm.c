/******************************************************************************

 MRF24W Driver Connection Algorithm
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
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


 Author         Date        Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 KH             27 Jan 2010 Created for MRF24W
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
    #define WF_MODULE_NUMBER    WF_MODULE_WF_CONNECTION_ALGORITHM
#endif

 
/*
*********************************************************************************************************
*                                           LOCAL DATA TYPES                               
*********************************************************************************************************
*/

/* header format for response to CP Get Element message */
typedef struct caElementResponseStruct
{
    tMgmtMsgRxHdr   mgmtHdr;                /* normal 4-byte hdr for all mgmt responses */
    UINT8           elementId;              /* index 4 */
    UINT8           elementDataLength;      /* index 5 */
    /* element data follows */
} tCAElementResponseHdr;    

/* WF_CM_DEBUG is used to retrieve connection management debug info from the WiFi chip.  Not used by applications */
#if defined(WF_CM_DEBUG)
/* header format for response to CM Info message */
typedef struct cmInfoResponseStruct
{
    tMgmtMsgRxHdr   mgmtHdr;                /* normal 4-byte hdr for all mgmt responses */
    //UINT8           infoId;               /* index 4                                  */
    //UINT8           infoDataLength;       /* index 5                                  */
    /* info data follows */
} tCMInfoResponseHdr;
#endif

/*
*********************************************************************************************************
*                                           LOCAL GLOBAL VARIABLES                               
*********************************************************************************************************
*/

static UINT8 g_EventNotificationAction = WF_NOTIFY_ALL_EVENTS;


/*
*********************************************************************************************************
*                                           LOCAL FUNCTION PROTOTYPES                               
*********************************************************************************************************
*/

static void LowLevel_CASetElement(UINT8 elementId, 
                                  UINT8 *p_elementData, 
                                  UINT8 elementDataLength);

static void LowLevel_CAGetElement(UINT8 elementId, 
                                  UINT8 *p_elementData, 
                                  UINT8 elementDataLength,
                                  UINT8 dataReadAction);

static void SetEventNotificationMask(UINT8 eventNotificationBitMask);

                       
#if defined(WF_USE_GROUP_SET_GETS)
/*****************************************************************************
  Function:
    void WF_CASetElements(tWFCAElements *p_elements)

  Summary:
    Writes all Connection Algorithm elements.

  Description:
    Sends a message to the MRF24W which sets all the Connection Algorithm 
    elements.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_elements - Pointer to the input structure (tWFCAElements) containing the 
                 connection algorithm elements.
                 
  Returns:
    None
      
  Remarks:
    None
*****************************************************************************/
void WF_CASetElements(tWFCAElements *p_elements)
{
    UINT8 i;
   
    /* Connection Profile lists not supported yet, make sure one is not defined */
    for (i = 0; i < WF_CP_LIST_LENGTH; ++i)
    {
        /* all elements in list should be 0xff, indicating CP list is unused */
        if (p_elements->connectionProfileList[i] != 0xff)
        {
            WF_ASSERT(FALSE);
        }    
    }    

    /* fix up listenInterval, minChannelTime, maxChannelTime, and probeDelay to correct endianness before sending message */
    p_elements->listenInterval = HSTOWFS(p_elements->listenInterval);
    p_elements->minChannelTime = HSTOWFS(p_elements->minChannelTime);
    p_elements->maxChannelTime = HSTOWFS(p_elements->maxChannelTime);        
    p_elements->probeDelay     = HSTOWFS(p_elements->probeDelay);        
    
    /* save application event notification bitmask */
    SetEventNotificationMask(p_elements->eventNotificationAction);

    /* temporarily override application event notification in app structure -- set to all events so WiFi driver will be notified */
    p_elements->eventNotificationAction = WF_NOTIFY_ALL_EVENTS;
        
    LowLevel_CASetElement(WF_CA_ELEMENT_ALL,      /* Element ID                   */
                          (UINT8 *)p_elements,    /* pointer to element data      */
                          sizeof(tWFCAElements)); /* number of element data bytes */
    
    /* put listenInterval, minChannelTime, maxChannelTime, and probeDelay back to host endianness */                                    
    p_elements->listenInterval = WFSTOHS(p_elements->listenInterval);
    p_elements->minChannelTime = WFSTOHS(p_elements->minChannelTime);
    p_elements->maxChannelTime = WFSTOHS(p_elements->maxChannelTime);
    p_elements->probeDelay     = WFSTOHS(p_elements->probeDelay);
        
    /* restore event notification in application structure to original value */
    p_elements->eventNotificationAction = GetEventNotificationMask();
                                    
} 

/*****************************************************************************
  Function:
    void WF_CASetElementsN(const tWFCAElements *p_elements)

  Summary:
    Writes all Connection Algorithm elements.

  Description:
    Sends a message to the MRF24W which sets all the Connection Algorithm
    elements.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_elements - Pointer to the input structure (tWFCAElements) containing the
                 connection algorithm elements.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void WF_CASetElementsN(const tWFCAElements *p_elements)
{
    UINT8 i;

    /* Connection Profile lists not supported yet, make sure one is not defined */
    for (i = 0; i < WF_CP_LIST_LENGTH; ++i)
    {
        /* all elements in list should be 0xff, indicating CP list is unused */
        if (p_elements->connectionProfileList[i] != 0xff)
        {
            WF_ASSERT(FALSE);
        }
    }

    LowLevel_CASetElement(WF_CA_ELEMENT_ALL,      /* Element ID                   */
                          (UINT8 *)p_elements,    /* pointer to element data      */
                          sizeof(tWFCAElements)); /* number of element data bytes */
}

/*****************************************************************************
  Function:
    void WF_CAGetElements(tWFCAElements *p_elements)

  Summary:
    Reads all Connection Algorithm elements.

  Description:
    Sends a message to the MRF24W which requests all the Connection Algorithm 
    elements.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_elements - Pointer to the output structure (tWFCAElements) where the 
                 connection algorithm elements are written.
                 
  Returns:
    None
      
  Remarks:
    None
*****************************************************************************/
void WF_CAGetElements(tWFCAElements *p_elements)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_ALL,      /* element ID              */
                         (UINT8 *)p_elements,     /* pointer to element data */
                          sizeof(tWFCAElements),  /* num data bytes          */
                          TRUE);                  /* read data, free buffer  */
    
    /* fix up endianness for 16-bit values */
    p_elements->listenInterval = HSTOWFS(p_elements->listenInterval);
    p_elements->minChannelTime = HSTOWFS(p_elements->minChannelTime);
    p_elements->maxChannelTime = HSTOWFS(p_elements->maxChannelTime);        
    p_elements->probeDelay     = HSTOWFS(p_elements->probeDelay);  
}
#endif /* WF_USE_GROUP_SET_GETS */  

#if defined(WF_USE_INDIVIDUAL_SET_GETS)
/*****************************************************************************
  Function:
    void WF_CASetScanType(UINT8 scanType)

  Summary:
    Sets the Connection Algorith scan type

  Description:
    Configures the Connection Algorithm for the desired scan type.
    In active scan, STA will transmit probe request frames and AP/routers will respond by
    transmitting probe response frames. For AP/Router with hidden SSID, active scan is used.
    In passive scan, AP/router will continuously transmit beacon frames for every beacon 
    interval (BI), any STAs may/will receive these beacons and know of existence of this 
    AP/router.

  Precondition:
    MACInit must be called first.

  Parameters:
    scanType - Desired scan type.  Either WF_ACTIVE_SCAN or WF_PASSIVE_SCAN. 
                 
  Returns:
    None
      
  Remarks:
    Active scanning causes the MRF24W to send probe requests.  Passive
    scanning implies the MRF24W only listens for beacons.
    Default is WF_ACTIVE_SCAN.
*****************************************************************************/
void WF_CASetScanType(UINT8 scanType)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_SCANTYPE,      /* Element ID                   */
                          &scanType,                   /* pointer to element data      */
                          sizeof(scanType));           /* number of element data bytes */
} 

/*****************************************************************************
  Function:
    void WF_CAGetScanType(UINT8 *p_scanType)

  Summary:
    Gets the Connection Algorithm scan type

  Description:
    Reads the current Connection Algorithm scan type.
    In active scan, STA will transmit probe request frames and AP/routers will respond by
    transmitting probe response frames. For AP/Router with hidden SSID, active scan is used.
    In passive scan, AP/router will continuously transmit beacon frames for every beacon 
    interval (BI), any STAs may/will receive these beacons and know of existence of this 
    AP/router.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_scanType - Pointer where Connection Algorithm scan type is written.
                         Either WF_ACTIVE_SCAN or WF_PASSIVE_SCAN. 
                 
  Returns:
    None
      
  Remarks:
    Active scanning causes the MRF24W to send probe requests.  Passive
    scanning implies the MRF24W only listens for beacons.  
    Default is WF_ACTIVE_SCAN.
*****************************************************************************/
void WF_CAGetScanType(UINT8 *p_scanType)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_SCANTYPE,       /* Element ID                   */
                          p_scanType,                   /* pointer to element data      */
                          1,                            /* number of element data bytes */
                          TRUE);                        /* read data, free buffer       */
}  

/*****************************************************************************
  Function:
    void WF_CASetMinChannelTime(UINT16 minChannelTime)

  Summary:
    Sets the minimum channel time (in milliseconds)

  Description:
    The minimum time (in milliseconds) the connection manager will wait for a 
    probe response after sending a probe request.  If no probe responses are 
    received in minChannelTime then the connection manager will go on to the 
    next channel, if any are left to scan, or quit.

  Precondition:
    MACInit must be called first.

  Parameters:
    minChannelTime - Minimum time to wait for a probe response (in milliseconds)
                 
  Returns:
    None
      
  Remarks:
    Default is 200ms
*****************************************************************************/
void WF_CASetMinChannelTime(UINT16 minChannelTime)
{
    minChannelTime = HSTOWFS(minChannelTime);
    LowLevel_CASetElement(WF_CA_ELEMENT_MIN_CHANNEL_TIME,   /* Element ID                   */
                          (UINT8 *)&minChannelTime,         /* pointer to element data      */
                          sizeof(minChannelTime));          /* number of element data bytes */
} 

/*****************************************************************************
  Function:
    void WF_CAGetMinChannelTime(UINT16 *p_minChannelTime)

  Summary:
    Gets the current Connection Algorithm minimum channel time.

  Description:
    Gets the minimum time the connection manager waits for a probe response after
    sending a probe request.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_minChannelTime - Pointer where minimum time to wait for a probe response 
                       (in milliseconds) will be written.
                 
  Returns:
    None
      
  Remarks:
    None
*****************************************************************************/
void WF_CAGetMinChannelTime(UINT16 *p_minChannelTime)
{
    
    LowLevel_CAGetElement(WF_CA_ELEMENT_MIN_CHANNEL_TIME,   /* Element ID                   */
                          (UINT8 *)p_minChannelTime,        /* pointer to element data      */
                          2,                                /* number of element data bytes */
                          TRUE);                            /* read data, free buffer       */
                          
    /* fix up endianess before returning */
    *p_minChannelTime = HSTOWFS(*p_minChannelTime);                              
}  

/*****************************************************************************
  Function:
    void WF_CASetMaxChannelTime(UINT16 maxChannelTime)

  Summary:
    Sets the maximum channel time (in milliseconds)

  Description:
    The maximum time (in milliseconds) the connection manager will wait for a 
    probe response after sending a probe request.  If no probe responses are 
    received in maxChannelTime then the connection manager will go on to the 
    next channel, if any are left to scan, or quit.

  Precondition:
    MACInit must be called first.

  Parameters:
    maxChannelTime - Maximum time to wait for a probe response (in milliseconds)
                 
  Returns:
    None
      
  Remarks:
    Default is 400ms
*****************************************************************************/
void WF_CASetMaxChannelTime(UINT16 maxChannelTime)
{
    maxChannelTime = HSTOWFS(maxChannelTime);  /* fix up endianness before sending */
    LowLevel_CASetElement(WF_CA_ELEMENT_MAX_CHANNEL_TIME,   /* Element ID                   */
                          (UINT8 *)&maxChannelTime,         /* pointer to element data      */
                          sizeof(maxChannelTime));          /* number of element data bytes */
} 

/*****************************************************************************
  Function:
    void WF_CAGetMaxChannelTime(UINT16 *p_maxChannelTime)

  Summary:
    Gets the Max Channel Time (in milliseconds)

  Description:
    Gets the maximum time the connection manager waits for a probe response after
    sending a probe request.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_maxChannelTime - Pointer where maximum channel time is written
                 
  Returns:
    None
      
  Remarks:
    Default is 400ms
*****************************************************************************/
void WF_CAGetMaxChannelTime(UINT16 *p_maxChannelTime)
{
    
    LowLevel_CAGetElement(WF_CA_ELEMENT_MAX_CHANNEL_TIME,   /* Element ID                   */
                          (UINT8 *)p_maxChannelTime,        /* pointer to element data      */
                          sizeof(UINT16),                   /* number of element data bytes */
                          TRUE);                            /* read data, free buffer       */
                          
    /* fix up endianess before returning */
    *p_maxChannelTime = HSTOWFS(*p_maxChannelTime);                              
}  

/*****************************************************************************
  Function:
    void WF_CASetProbeDelay(UINT16 probeDelay)

  Summary:
    Sets the Probe Delay (in microseconds)

  Description:
    The number of microseconds to delay before transmitting a probe request
    following the channel change event.

  Precondition:
    MACInit must be called first.

  Parameters:
    probeDelay - Desired probe delay
                 
  Returns:
    None
      
  Remarks:
    Default is 20uS
*****************************************************************************/
void WF_CASetProbeDelay(UINT16 probeDelay)
{
    probeDelay = HSTOWFS(probeDelay);                   /* fix up endianness before sending */
    LowLevel_CASetElement(WF_CA_ELEMENT_PROBE_DELAY,    /* Element ID                       */
                          (UINT8 *)&probeDelay,         /* pointer to element data          */
                          sizeof(probeDelay));          /* number of element data bytes     */
} 

/*****************************************************************************
  Function:
    void WF_CAGetProbeDelay(UINT16 probeDelay)

  Summary:
    Gets the Probe Delay (in microseconds)

  Description:
    The number of microseconds to delay before transmitting a probe request
    following the channel change event.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_probeDelay - Pointer to where probe delay is written
                 
  Returns:
    None
      
  Remarks:
    Default is 20uS
*****************************************************************************/
void WF_CAGetProbeDelay(UINT16 *p_probeDelay)
{
    
    LowLevel_CAGetElement(WF_CA_ELEMENT_PROBE_DELAY,   /* Element ID                   */
                          (UINT8 *)p_probeDelay,       /* pointer to element data      */
                          sizeof(UINT16),              /* number of element data bytes */
                          TRUE);                       /* read data, free buffer       */
                          
    /* fix up endianess before returning */
    *p_probeDelay = HSTOWFS(*p_probeDelay);                              
}  

/*****************************************************************************
  Function:
    void WF_CASetScanCount(UINT8 scanCount)

  Summary:
    Sets the scan count

  Description:
    The number of times the Connection Manager will scan a channel while attempting
    to find a particular WiFi network.

  Precondition:
    MACInit must be called first.

  Parameters:
    scanCount - Desired scan count
                 
  Returns:
    None
      
  Remarks:
    Default is 1
*****************************************************************************/
void WF_CASetScanCount(UINT8 scanCount)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_SCAN_COUNT,    /* Element ID                   */
                          &scanCount,                  /* pointer to element data      */
                          sizeof(scanCount));          /* number of element data bytes */
}

/*****************************************************************************
  Function:
    void WF_CAGetScanCount(UINT8 *p_scanCount)

  Summary:
    Gets the scan count

  Description:
    The number of times the Connection Manager will scan a channel while attempting
    to find a particular WiFi network.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_scanCount - Pointer to where scan count is written
                 
  Returns:
    None
      
  Remarks:
    Default is 1
*****************************************************************************/
void WF_CAGetScanCount(UINT8 *p_scanCount)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_SCAN_COUNT,     /* Element ID                   */
                          p_scanCount,                  /* pointer to element data      */
                          1,                            /* number of element data bytes */
                          TRUE);                        /* read data, free buffer       */
}
            

/*****************************************************************************
  Function:
    void WF_CASetRssi(UINT8 rssi)

  Summary:
    Sets the RSSI threshold

  Description:
    Specifies the RSSI behavior when connecting.  This value is only used if
      1) The current Connection Profile has not defined an SSID or BSSID
      2) An SSID is defined in the current Connection Profile and multiple
         access points are discovered with the same SSID.
         
     Values:
       0      : Connect to the first network found
       1 - 254 (MRF24WB), 1 - 128 (MRF24WG): Only connect to a network if the RSSI
           is greater than or equal to the specified value
       255:     Connect to the highest RSSI found

       Note that RSSI is a relative value with no units -- it is not correlated to dBm.
       
  Precondition:
    MACInit must be called first.

  Parameters:
    scanCount - Desired scan count
                 
  Returns:
    None
      
  Remarks:
    Default is 255
*****************************************************************************/
void WF_CASetRssi(UINT8 rssi)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_RSSI,  /* Element ID                   */
                          &rssi,                /* pointer to element data      */
                          sizeof(rssi));        /* number of element data bytes */
   
}       

/*****************************************************************************
  Function:
    void WF_CAGetRssi(UINT8 p_rssi)

  Summary:
    Gets the RSSI threshold

  Description:
    See WF_CASetRssi.  Note that this function only retrieves the RSSI threshold 
    used during the connection -- this is not the current RSSI of an existing connection.
    If it is desired to retrieve the current RSSI state then a scan must be performed and
    the scan result will contain the current RSSI state.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_rssi - Pointer to where RSSI value is written
                 
  Returns:
    None
      
  Remarks:
    Default is 255
*****************************************************************************/
void WF_CAGetRssi(UINT8 *p_rssi)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_RSSI,      /* Element ID                   */
                          p_rssi,                  /* pointer to element data      */
                          1,                       /* number of element data bytes */
                          TRUE);                   /* read data, free buffer       */
}          


/*****************************************************************************
  Function:
    void WF_CASetConnectionProfileList(UINT8 cpList[WF_CP_LIST_LENGTH])

  Summary:
    Not currently supported

  Description:
    Not currently supported

  Precondition:
    MACInit must be called first.

  Parameters:
    cpList - Array of connection profile ID's used to create CP list
                 
  Returns:
    None
      
  Remarks:
    Not currently supported.  The list size is always WF_CP_LIST_SIZE.
    The list should start at index 0.  Unused entries in the list must be set 
    to 0xff.
*****************************************************************************/
void WF_CASetConnectionProfileList(UINT8 cpList[WF_CP_LIST_LENGTH])
{
    WF_ASSERT(FALSE);       /* Connection Profile lists are not currently supported */
#if 0
    LowLevel_CASetElement(WF_CA_ELEMENT_CP_LIST,   /* Element ID                   */
                          cpList,                  /* pointer to element data      */
                          WF_CP_LIST_LENGTH);      /* number of element data bytes */
#endif   

}       

/*****************************************************************************
  Function:
    void WF_CAGetConnectionProfileList(UINT8 cpList[WF_CP_LIST_LENGTH])

  Summary:
    Not currently supported

  Description:
    Not currently supported

  Precondition:
    MACInit must be called first.

  Parameters:
    cpList - Array of connection profile ID's used to create CP list
                 
  Returns:
    None
      
  Remarks:
    Not currently supported.  The list size is always WF_CP_LIST_SIZE.
*****************************************************************************/
void WF_CAGetConnectionProfileList(UINT8 cpList[WF_CP_LIST_LENGTH])
{
    WF_ASSERT(FALSE);       /* Connection Profile lists are not currently supported */
    #if 0
    LowLevel_CAGetElement(WF_CA_ELEMENT_CP_LIST,    /* Element ID                   */
                          cpList,                   /* pointer to element data      */
                          WF_CP_LIST_LENGTH,        /* number of element data bytes */
                          TRUE);                    /* read data, free buffer       */
    #endif                          
}

/*****************************************************************************
  Function:
    void WF_CASetListRetryCount(UINT8 listRetryCount)

  Summary:
    Sets the list retry count

  Description:
    Number of times to cycle through Connection Profile List before giving up on 
    the connection attempt.  Since lists are not yet supported, this function 
    actually sets the number of times the Connection Manager will try to connect
    with the current Connection Profile before giving up.

  Precondition:
    MACInit must be called first.

  Parameters:
    listRetryCount - 0 to 254 or WF_RETRY_FOREVER (255)
                 
  Returns:
    None
      
  Remarks:
    None
*****************************************************************************/
void WF_CASetListRetryCount(UINT8 listRetryCount)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_LIST_RETRY_COUNT,   /* Element ID                   */
                          &listRetryCount,                  /* pointer to element data      */
                          sizeof(listRetryCount));          /* number of element data bytes */
} 

/*****************************************************************************
  Function:
    void WF_CAGetListRetryCount(UINT8 *p_listRetryCount)

  Summary:
    Gets the list retry count

  Description:
    See description in WF_CASetListRetryCount()

  Precondition:
    MACInit must be called first.

  Parameters:
    p_listRetryCount - Pointer to where list retry count is written.
                 
  Returns:
    None
      
  Remarks:
    None
*****************************************************************************/
void WF_CAGetListRetryCount(UINT8 *p_listRetryCount)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_LIST_RETRY_COUNT,       /* Element ID                   */
                          p_listRetryCount,                     /* pointer to element data      */
                          1,                                    /* number of element data bytes */
                          TRUE);                                /* read data, free buffer       */
}          
      
/*******************************************************************************
  Function:
    void WF_CASetEventNotificationAction(UINT8 eventNotificationAction)

  Summary:
    Sets the WiFi events that the host wishes to be notified of.

  Description:
    Sets the Event Notification Action used by the Connection Algorithm.  The
    bit mask for the allowable entries is as follows:
    
    <table>
        Bit     Event
        ---     -----
        0       WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL  
        1       WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        2       WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        3       WF_NOTIFY_CONNECTION_PERMANENTLY_LOST 
        4       WF_NOTIFY_CONNECTION_REESTABLISHED 
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    eventNotificationAction - Bit mask indicating which events the host wants
                               to be notifed of.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetEventNotificationAction(UINT8 eventNotificationAction)
{    
    /* Remember what events application wants to be notified of.  The MRF24W will inform the WiFi driver    */
    /* of all events, but only events the application wants to see will ripple up to WF_ProcessEvent().     */
    SetEventNotificationMask(eventNotificationAction);
   
}    

/*******************************************************************************
  Function:    
    void WF_CAGetEventNotificationAction(UINT8 *p_eventNotificationAction)

  Summary:
    Reads the Connection Algorithm event notification action.

  Description:
    Gets the Event Notification Action used by the Connection Algorithm.  The
    value read back will be a bit mask that corresponds to the following table:

    <table>
        Bit     Event
        ---     -----
        0       WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL  
        1       WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        2       WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        3       WF_NOTIFY_CONNECTION_PERMANENTLY_LOST 
        4       WF_NOTIFY_CONNECTION_REESTABLISHED 
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    p_eventNotificationAction - Pointer to where returned value is written.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetEventNotificationAction(UINT8 *p_eventNotificationAction)
{
    *p_eventNotificationAction = GetEventNotificationMask();
}  

/*******************************************************************************
  Function:    
    void WF_CASetBeaconTimeoutAction(UINT8 beaconTimeoutAction)

  Summary:
    Action to take if a connection is lost due to a beacon timeout.

  Description:
    Sets the Beacon Timeout Action used by the Connection Algorithm.

  Precondition:
    MACInit must be called first.

  Parameters:
    beaconTimeoutAction - Action to take if a connection is lost due 
                           to a beacon timeout.  Choices are either:
                           * WF_ATTEMPT_TO_RECONNECT
                           * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetBeaconTimeoutAction(UINT8 beaconTimeoutAction)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_BEACON_TIMEOUT_ACTION,   /* Element ID                   */
                          &beaconTimeoutAction,                  /* pointer to element data      */
                          sizeof(beaconTimeoutAction));          /* number of element data bytes */
   
}      

/*******************************************************************************
  Function:    
    void WF_CAGetBeaconTimeoutAction(UINT8 *p_beaconTimeoutAction)

  Summary:
    Reads the Connection Algorithm beacon timeout action.

  Description:
    Gets the Beacon Timeout Action used by the Connection Algorithm.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_beaconTimeoutAction - Pointer where returned value is written.  The value
                             will be either:
                             * WF_ATTEMPT_TO_RECONNECT
                             * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetBeaconTimeoutAction(UINT8 *p_beaconTimeoutAction)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_BEACON_TIMEOUT_ACTION,           /* Element ID                   */
                          p_beaconTimeoutAction,                          /* pointer to element data      */
                          1,                                              /* number of element data bytes */
                          TRUE);                                          /* read data, free buffer       */
   
}

/*******************************************************************************
  Function:    
    void WF_CASetDeauthAction(UINT8 deauthAction)

  Summary:
    Sets the DeauthAction used by the Connection Algorithm.

  Description:
    Action to take if a connection is lost due to receiving a deauthentification 
    message from an AP.

  Precondition:
    MACInit must be called first.    

  Parameters:
    deauthAction - Action to take in the event of a deauthentication.
                    Allowable values are:
                    * WF_ATTEMPT_TO_RECONNECT
                    * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetDeauthAction(UINT8 deauthAction)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_DEAUTH_ACTION,    /* Element ID                   */
                          &deauthAction,                  /* pointer to element data      */
                          sizeof(deauthAction));          /* number of element data bytes */
}      

/*******************************************************************************
  Function:    
    void WF_CAGetDeauthAction(UINT8 *p_deauthAction)

  Summary:
    Reads the Connection Algorithm deauth action.

  Description:
    Gets the  DeauthAction used by the Connection Algorithm.

  Precondition:
    MACInit must be called first. 

  Parameters:
    p_deauthAction - Pointer where returned value is written.  The value will
                      be either:
                      * WF_ATTEMPT_TO_RECONNECT
                      * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetDeauthAction(UINT8 *p_deauthAction)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_DEAUTH_ACTION,    /* Element ID                   */
                          p_deauthAction,                  /* pointer to element data      */
                          1,                               /* number of element data bytes */
                          TRUE);                           /* read data, free buffer       */
}

/*******************************************************************************
  Function:    
    void WF_CASetChannelList(UINT8 *p_channelList, UINT8 numChannels)

  Summary:
    Sets the channel list.

  Description:
    Sets the Channel List used by the Connection Algorithm.

  Precondition:
    MACInit must be called first. 

  Parameters:
    p_channelList - Pointer to channel list.
    numChannels   - Number of channels in p_channelList.  If set to 0, the
                     MRF24W will use all valid channels for the current 
                     regional domain.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetChannelList(UINT8 *p_channelList, UINT8 numChannels)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_CHANNEL_LIST,  /* Element ID                   */
                          p_channelList,               /* pointer to element data      */
                          numChannels);                /* number of element data bytes */
}

#if !defined(MRF24WG)
#define RAW_MGMT_RX_ID   RAW_RX_ID
#endif
/*******************************************************************************
  Function:    
    void WF_CAGetChannelList(UINT8 *p_channelList, UINT8 *p_numChannels)

  Summary:
    Gets the channel list.

  Description:
    Gets the Channel List used by the Connection Algorithm.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_channelList - Pointer to where channel list will be returned
    p_numChannels - Pointer to where number of channels in list will be 
                     returned

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetChannelList(UINT8 *p_channelList, UINT8 *p_numChannels)
{
    tCAElementResponseHdr mgmtHdr;
    
    /* send request, wait for mgmt response, do not read and do not free up response buffer */
     LowLevel_CAGetElement(WF_CA_ELEMENT_CHANNEL_LIST,    /* Element ID      */
                           NULL,                          /* do not read     */
                           0,                             /* do not read     */
                           FALSE);                        /* do not read, do not free mgmt buffer */

    /* at this point, management response is mounted and ready to be read */  

    /* read managment header */
    RawRead(RAW_MGMT_RX_ID, 0, sizeof(tCAElementResponseHdr), (UINT8 *)&mgmtHdr);
    
    /* extract data length (which will be channel list length) */
    *p_numChannels = mgmtHdr.elementDataLength;

    RawRead(RAW_MGMT_RX_ID, sizeof(tCAElementResponseHdr), *p_numChannels, p_channelList);
    
    /* free management buffer */
    DeallocateMgmtRxBuffer();
}
    
/*******************************************************************************
  Function:    
    void WF_CASetListenInterval(UINT16 listenInterval)

  Summary:
    Sets the listen interval.

  Description:
    Sets the listen interval used by the Connection Algorithm.  This value is
    measured in 100ms intervals, the default beacon period of APs.

    <table>
        Value   Description
        -----   -----------
        1       MRF24W wakes up every 100ms to receive buffered messages.
        2       MRF24W wakes up every 200ms to receive buffered messages.
        ...     ...
        65535   MRF24W wakes up every 6535.5 seconds (~109 minutes) to
                 receive buffered messages.
    </table>

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled. 

  Parameters:
    listenInterval - Number of 100ms intervals between instances when 
                      the MRF24W wakes up to receive buffered messages 
                      from the network.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetListenInterval(UINT16 listenInterval)
{
    /* correct endianness before sending message */
    listenInterval = HSTOWFS(listenInterval);

    LowLevel_CASetElement(WF_CA_ELEMENT_LISTEN_INTERVAL,    /* Element ID                   */
                          (UINT8 *)&listenInterval,         /* pointer to element data      */
                          sizeof(listenInterval));          /* number of element data bytes */
}      

#if defined(MRF24WG)
/*******************************************************************************
  Function:    
    void WF_CASetDtimInterval(UINT16 dtimInterval)

  Summary:
    Sets the dtim interval for MRF24WG0MA/B.

  Description:
    Sets the dtim interval used by the Connection Algorithm for MRF24WG0MA/B.  

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled. 

  Parameters:
    dtimInterval -- Number of DTIM intervals between instances when 
                      the MRF24W wakes up to receive buffered messages 
                      from the network.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetDtimInterval(UINT16 dtimInterval)
{
    /* correct endianness before sending message */
    dtimInterval = HSTOWFS(dtimInterval);

    LowLevel_CASetElement(WF_CA_ELEMENT_DTIM_INTERVAL,    /* Element ID                   */
                          (UINT8 *)&dtimInterval,         /* pointer to element data      */
                          sizeof(dtimInterval));          /* number of element data bytes */
}      
#endif /* MRF24WG */

/*******************************************************************************
  Function:    
    void WF_CAGetListenInterval(UINT16 *p_listenInterval)

  Summary:
    Gets the listen interval.    

  Description:
    Gets the Listen Interval used by the Connection Algorithm.  This value is
    measured in 100ms intervals, the default beacon period of APs.

    <table>
        Value   Description
        -----   -----------
        1       MRF24W wakes up every 100ms to receive buffered messages.
        2       MRF24W wakes up every 200ms to receive buffered messages.
        ...     ...
        65535   MRF24W wakes up every 6535.5 seconds (~109 minutes) to
                 receive buffered messages.
    </table>

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled. 

  Parameters:
    p_listenInterval - Pointer to where listen interval is returned

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetListenInterval(UINT16 *p_listenInterval)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_LISTEN_INTERVAL,     /* Element ID                   */
                         (UINT8 *)p_listenInterval,          /* pointer to element data      */
                          sizeof(UINT16),                    /* number of element data bytes */
                          TRUE);                             /* read data, free buffer       */

    /* fix endianness before returning value */
    *p_listenInterval = WFSTOHS(*p_listenInterval);
}  

#if defined(MRF24WG)
/*******************************************************************************
  Function:    
    void WF_CASetBeaconPeriod(UINT16 beaconPeriod)

  Summary:
    Sets the beacon period in Adhoc start mode for MRF24WG0MA/B

  Description:
    Sets the beacon period used by the Connection Algorithm for MRF24WG0MA/B.  

  Precondition:
    MACInit must be called first.

  Parameters:
    beaconPeriod - beacon period in adhoc start mode by ms resoluation 

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetBeaconPeriod(UINT16 beaconPeriod)
{
    /* correct endianness before sending message */
    beaconPeriod = HSTOWFS(beaconPeriod);

    LowLevel_CASetElement(WF_CA_ELEMENT_BEACON_PERIOD,    /* Element ID                   */
                          (UINT8 *)&beaconPeriod,         /* pointer to element data      */
                          sizeof(beaconPeriod));          /* number of element data bytes */
}      

/*******************************************************************************
  Function:    
    void WF_CAGetBeaconPeriod(UINT16 *beaconPeriod)

  Summary:
    Retrieves beacon period in Adhoc start mode for MRF24WG0MA/B.

  Description:
    Gets Beacon period in Adhoc start mode for MRF24WG0MA/B.

  Precondition:
    MACInit must be called first.

  Parameters:
    listenInterval -- pointer to where beacon period is returned

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetBeaconPeriod(UINT16 *beaconPeriod)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_BEACON_PERIOD,   /* Element ID                   */
                          (UINT8 *)beaconPeriod,         /* pointer to element data      */
                          sizeof(*beaconPeriod),         /* number of element data bytes */
                          TRUE);                         /* read data, free buffer       */

    /* fix endianness before returning value */
    *beaconPeriod = WFSTOHS(*beaconPeriod);
} 
 

/*******************************************************************************
  Function:    
    void WF_CAGetDtimInterval(UINT16 *p_dtimInterval)

  Summary:
    Gets the dtim interval for MRF24WG0MA/B.    

  Description:
    Gets the DTIM Interval used by the Connection Algorithm for MRF24WG0MA/B. 

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled. 

  Parameters:
    p_dtimInterval -- pointer to where listen interval is returned

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetDtimInterval(UINT16 *p_dtimInterval)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_DTIM_INTERVAL,     /* Element ID                   */
                          (UINT8 *)p_dtimInterval,         /* pointer to element data      */
                          sizeof(UINT16),                  /* number of element data bytes */
                          TRUE);                           /* read data, free buffer       */

    /* fix endianness before returning value */
    *p_dtimInterval = WFSTOHS(*p_dtimInterval);
}
#endif /* MRF24WG */  

/*******************************************************************************
  Function:    
    void WF_CASetBeaconTimeout(UINT8 beaconTimeout)

  Summary:
    Sets the beacon timeout value.

  Description:
    Sets the Beacon Timeout used by the Connection Algorithm.

    <table>
        Value   Description
        -----   -----------
        0       No monitoring of the beacon timeout condition.  The host will
                 not be notified of this event.
        1-255   Number of beacons missed before disconnect event occurs and 
                 beaconTimeoutAction occurs.  If enabled, host will receive
                 an event message indicating connection temporarily or 
                 permanently lost, and if retrying, a connection successful
                 event.
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    beaconTimeout - Number of beacons that can be missed before the action in 
                    beaconTimeoutAction is taken.

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CASetBeaconTimeout(UINT8 beaconTimeout)
{
    LowLevel_CASetElement(WF_CA_ELEMENT_BEACON_TIMEOUT,    /* Element ID                   */
                          &beaconTimeout,                  /* pointer to element data      */
                          sizeof(beaconTimeout));          /* number of element data bytes */
}      

/*******************************************************************************
  Function:    
    void WF_CAGetBeaconTimeout(UINT8 *p_beaconTimeout)

  Summary:
    Reads the beacon timeout value.

  Description:
    Gets the Beacon Timeout used by the Connection Algorithm.

    <table>
        Value   Description
        -----   -----------
        0       No monitoring of the beacon timeout condition.  The host will
                 not be notified of this event.
        1-255   Number of beacons missed before disconnect event occurs and 
                 beaconTimeoutAction occurs.  If enabled, host will receive
                 an event message indicating connection temporarily or 
                 permanently lost, and if retrying, a connection successful
                 event.
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    p_beaconTimeout - Pointer where beacon timeout value is written

  Returns:
    None.
      
  Remarks:
    None.
 *****************************************************************************/
void WF_CAGetBeaconTimeout(UINT8 *p_beaconTimeout)
{
    LowLevel_CAGetElement(WF_CA_ELEMENT_BEACON_TIMEOUT,    /* Element ID                   */
                          p_beaconTimeout,                 /* pointer to element data      */
                          1,                               /* number of element data bytes */
                          TRUE);                           /* read data, free buffer       */
}

/*******************************************************************************
  Function:    
    static void SetEventNotificationMask(UINT8 eventNotificationBitMask)

  Summary:
    Sets the event notification mask.

  Description:
    Sets the event notification mask for the Connection Algorithm.  Allowable
    values are:
    
    <table>
        Value   Event
        -----   -----
        0x01    WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL
        0x02    WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        0x04    WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        0x08    WF_NOTIFY_CONNECTION_PERMANENTLY_LOST
        0x10    WF_NOTIFY_CONNECTION_REESTABLISHED
        0x1f    WF_NOTIFY_ALL_EVENTS
    </table>

  Precondition:
    MACInit must be called first.
      
  Parameters:
    eventNotificationBitMask - Bit mask defining which events the host will be
                               notified of.

  Returns:
    None.
      
  Remarks:
    None.
  *****************************************************************************/
static void SetEventNotificationMask(UINT8 eventNotificationBitMask)
{
    g_EventNotificationAction = eventNotificationBitMask;
}    

/*******************************************************************************
  Function:    
    UINT8 GetEventNotificationMask(void)

  Summary:
    Gets the event notification mask.

  Description:
    Gets the event notification mask for the Connection Algorithm.  Retruned
    values are:

    <table>
        Value   Event
        -----   -----
        0x01    WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL
        0x02    WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        0x04    WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        0x08    WF_NOTIFY_CONNECTION_PERMANENTLY_LOST
        0x10    WF_NOTIFY_CONNECTION_REESTABLISHED
        0x1f    WF_NOTIFY_ALL_EVENTS
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    A UINT8 of the event notification bit mask.
      
  Remarks:
    None.
  *****************************************************************************/
UINT8 GetEventNotificationMask(void)
{
    return g_EventNotificationAction;
}
#endif /* WF_USE_INDIVIDUAL_SET_GETS */
    
/*******************************************************************************
  Function:    
    static void LowLevel_CASetElement(UINT8 elementId, 
                                      UINT8 *p_elementData, 
                                      UINT8 elementDataLength)

  Summary:
    Set an element of the connection algorithm on the MRF24W.

  Description:
    Low-level function to send the appropriate management message to the
    MRF24W to set the Connection Algorithm element.
    
  Precondition:
    MACInit must be called first.

  Parameters:
    elementId - Element that is being set
    p_elementData - Pointer to element data
    elementDataLength - Number of bytes pointed to by p_elementData

  Returns:
    None.
    
  Remarks:
    All Connection Algorithm 'Set Element' functions call this function
    to construct the management message.  The caller must fix up any endian
    issues prior to calling this function.
 *****************************************************************************/
static void LowLevel_CASetElement(UINT8 elementId, 
                                  UINT8 *p_elementData, 
                                  UINT8 elementDataLength)
{
    UINT8  hdrBuf[4];

    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;           /* indicate this is a mgmt msg     */
    hdrBuf[1] = WF_CA_SET_ELEMENT_SUBTYPE;      /* mgmt request subtype            */     
    hdrBuf[2] = elementId;                      /* Element ID                      */
    hdrBuf[3] = elementDataLength;              /* number of bytes of element data */
    
    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                p_elementData,
                elementDataLength);
    
    /* wait for mgmt response, free after it comes in, don't need data bytes */
    WaitForMgmtResponse(WF_CA_SET_ELEMENT_SUBTYPE, FREE_MGMT_BUFFER);
}   

/*******************************************************************************
  Function:    
    static void LowLevel_CAGetElement(UINT8 elementId, 
                                      UINT8 *p_elementData, 
                                      UINT8 elementDataLength,
                                      UINT8 dataReadAction)

  Summary:
    Get an element of the connection algorithm on the MRF24W.

  Description:
    Low-level function to send the appropriate management message to the
    MRF24W to get the Connection Algorithm element.

  Precondition:
    MACInit must be called first.

  Parameters:
    elementId - Element that is being read
    p_elementData - Pointer to where element data will be written
    elementDataLength - Number of element data bytes that will be read
    dataReadAction - If TRUE then read data per paramters and free mgmt response buffer.
                      If FALSE then return after response received, do not read any data as the 
                      caller will do that, and don't free buffer, as caller will do that as well.

  Returns:
    None.

  Remarks:
    All Connection Algorithm 'Get Element' functions call this function to 
    construct the management message.  The caller must fix up any endian issues
    after getting the data from this function.
 *****************************************************************************/
static void LowLevel_CAGetElement(UINT8 elementId, 
                                  UINT8 *p_elementData, 
                                  UINT8 elementDataLength,
                                  UINT8 dataReadAction)    /* TRUE or FALSE */
{
    UINT8  hdrBuf[4];
            
    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;       /* indicate this is a mgmt msg     */
    hdrBuf[1] = WF_CA_GET_ELEMENT_SUBTYPE;  /* mgmt request subtype            */     
    hdrBuf[2] = elementId;                  /* Element ID                      */
    hdrBuf[3] = 0;                          /* not used                        */

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    if (dataReadAction == (UINT8)TRUE)
    {
        /* wait for mgmt response, read desired data, and then free response buffer */
        WaitForMgmtResponseAndReadData(WF_CA_GET_ELEMENT_SUBTYPE, 
                                       elementDataLength,                   /* num data bytes to read                */
                                       sizeof(tCAElementResponseHdr),       /* index of first byte of element data   */
                                       p_elementData);                      /* where to write element data           */
    }
    else
    {
        /* wait for mgmt response, don't read any data bytes, do not release mgmt buffer */
        WaitForMgmtResponse(WF_CA_GET_ELEMENT_SUBTYPE, DO_NOT_FREE_MGMT_BUFFER);
    }                                                    
}

void WF_DisableModuleConnectionManager(void)
{
    WF_CASetListRetryCount(0 /* MY_DEFAULT_LIST_RETRY_COUNT */);
    WF_CASetDeauthAction(WF_DO_NOT_ATTEMPT_TO_RECONNECT);
    WF_CASetBeaconTimeoutAction(WF_DO_NOT_ATTEMPT_TO_RECONNECT);
}
  
#if defined(WF_CM_DEBUG)
static void LowLevel_CMInfo(UINT8 infoId, 
                            UINT8 *p_infoData, 
                            UINT8 infoDataLength,
                            UINT8 dataReadAction)    /* TRUE or FALSE */
{
    UINT8  hdrBuf[4];
            
    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;       /* indicate this is a mgmt msg     */
    hdrBuf[1] = WF_CM_INFO_SUBTYPE;         /* mgmt request subtype            */     
    hdrBuf[2] = infoId;                     /* info ID                         */
    hdrBuf[3] = 0;                          /* not used                        */

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    if (dataReadAction == (UINT8)TRUE)
    {
        /* wait for mgmt response, read desired data, and then free response buffer */
        WaitForMgmtResponseAndReadData(WF_CM_INFO_SUBTYPE, 
                                       infoDataLength,                   /* num data bytes to read                */
                                       sizeof(tCMInfoResponseHdr),       /* index of first byte of info data   */
                                       p_infoData);                      /* where to write info data           */
    }
    else
    {
        /* wait for mgmt response, don't read any data bytes, do not release mgmt buffer */
        WaitForMgmtResponse(WF_CM_INFO_SUBTYPE, DO_NOT_FREE_MGMT_BUFFER);
    }                                                    
}

void WF_CMInfoGetFSMStats(tWFCMInfoFSMStats *p_info)
{
    LowLevel_CMInfo(WF_CM_INFO_GET_FSM_STATS,      /* info ID              */
                    (UINT8 *)p_info,  /* pointer to info data */
                    sizeof(tWFCMInfoFSMStats),  /* num data bytes          */
                    TRUE);                  /* read data, free buffer  */
}
#endif /* WF_CM_DEBUG */  

#endif /* WF_CS_TRIS */

