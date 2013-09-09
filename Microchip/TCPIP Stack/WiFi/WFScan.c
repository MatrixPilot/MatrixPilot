/******************************************************************************

 MRF24W Driver Scan functions
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WFScan.c
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


 Author              Date        Comment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 KH                 27 Jan 2010 Created for MRF24W
******************************************************************************/

/*
*********************************************************************************************************
*                                           INCLUDES                               
*********************************************************************************************************
*/

#include "TCPIP Stack/WFMac.h"
#if defined(WF_CS_TRIS) && defined(WF_USE_SCAN_FUNCTIONS)


/*
*********************************************************************************************************
*                                           INCLUDES                               
*********************************************************************************************************
*/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_SCAN
#endif

static BOOL WF_CMIsHostScanAllowed(void)
{
    UINT8   profileID;
    UINT8   profileIDState;
  
    WF_CMCheckConnectionState(&profileIDState, &profileID);
    if (profileIDState == WF_CSTATE_CONNECTION_IN_PROGRESS || profileIDState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
        return FALSE;

    return TRUE;
}

/*******************************************************************************
  Function:    
    UINT16 WF_Scan(UINT8 CpId)

  Summary:
    Commands the MRF24W to start a scan operation.  This will generate the 
    WF_EVENT_SCAN_RESULTS_READY event.

  Description:
    Directs the MRF24W to initiate a scan operation utilizing the input 
    Connection Profile ID.  The Host Application will be notified that the scan 
    results are ready when it receives the WF_EVENT_SCAN_RESULTS_READY event.  
    The eventInfo field for this event will contain the number of scan results.  
    Once the scan results are ready they can be retrieved with 
    WF_ScanGetResult().

    Scan results are retained on the MRF24W until:
    1.  Calling WF_Scan() again (after scan results returned from previous 
        call).
    2.  MRF24W reset.

    MRF24WB0M & MRF24WG0M support up to max of 60 scan results (SSIDs).

  Precondition:
    MACInit must be called first.

  Parameters:
    CpId - Connection Profile to use.  
            If the CpId is valid then the values from that Connection Profile 
            will be used for filtering scan results.  If the CpId is set to 
            WF_SCAN_ALL (0xFF) then a default filter will be used.

            Valid CpId
            * If CP has a defined SSID only scan results with that SSID are 
               retained.  
            * If CP does not have a defined SSID then all scanned SSID’s will be 
               retained
            * Only scan results from Infrastructure or AdHoc networks are 
               retained, depending on the value of networkType in the Connection Profile
            * The channel list that is scanned will be determined from 
               channelList in the Connection Algorithm (which must be defined 
               before calling this function).

            CpId is equal to WF_SCAN_ALL
            * All scan results are retained (both Infrastructure and Ad Hoc 
               networks).
            * All channels within the MRF24W’s regional domain will be 
               scanned.
            * No Connection Profiles need to be defined before calling this 
               function.
            * The Connection Algorithm does not need to be defined before 
               calling this function.

  Returns:
    Operation results. Success or Failure
      
  Remarks:
    None.
  *****************************************************************************/
UINT16 WF_Scan(UINT8 CpId)
{
    UINT8   hdr[4];

    /* WARNING !!! : 
    * Host scan is allowed only in idle or connected state. 
    * If module FW is in the midst of connection ( or reconenction) process, then
    * host scan can hammer connection process, and furthermore it may cause
    * fatal failure in module FW operation. To be safte to use host scan, we strongly
    * recommend you to disable module FW connection manager by uncommenting
    * #define DISABLE_MODULE_FW_CONNECT_MANAGER_IN_INFRASTRUCTURE    
    * in WF_Config.h
    */
    if (!WF_CMIsHostScanAllowed())    
        return WF_ERROR_OPERATION_CANCELLED;
    
    hdr[0] = WF_MGMT_REQUEST_TYPE;
    hdr[1] = WF_SCAN_START_SUBTYPE; 
    hdr[2] = CpId;                  /* Connection Profile ID */
    hdr[3] = 0;                     /* not used              */
    
    SendMgmtMsg(hdr,             /* header           */
                sizeof(hdr),     /* size of header   */
                NULL,            /* no data          */
                0);              /* no data          */

    /* wait for mgmt response, free it after it comes in (no data needed) */
    WaitForMgmtResponse(WF_SCAN_START_SUBTYPE, FREE_MGMT_BUFFER); 

    return WF_SUCCESS;
}

/*******************************************************************************
  Function:    
    void WF_ScanGetResult(UINT8 listIndex, tWFScanResult  *p_scanResult)

  Summary:
    Read scan results back from MRF24W. 

  Description:
    After a scan has completed this function is used to read one or more of the 
    scan results from the MRF24W.  The scan results will be written 
    contiguously starting at p_scanResult (see tWFScanResult structure for 
    format of scan result).    
    MRF24WB0M & MRF24WG0M support up to max of 60 scan results (SSIDs). 

  Precondition:
    MACInit must be called first.  WF_EVENT_SCAN_RESULTS_READY event must have
    already occurrerd.

  Parameters:
    listIndex - Index (0-based list) of the scan entry to retrieve.
    p_scanResult - Pointer to location to store the scan result structure

  Returns:
    None.
      
  Remarks:
     p_scanResult->rssi contains signal strength RSSI.
     MRF24WB : RSSI_MAX (200) , RSSI_MIN (106).
     MRF24WG : RSSI_MAX (128) , RSSI_MIN (43).
  *****************************************************************************/
void WF_ScanGetResult(UINT8          listIndex, 
                       tWFScanResult  *p_scanResult)
{
    
    UINT8   hdr[4];
    /* char rssiChan[48]; */ /* reference for how to retrieve RSSI */
    
    hdr[0] = WF_MGMT_REQUEST_TYPE;
    hdr[1] = WF_SCAN_GET_RESULTS_SUBTYPE; 
    hdr[2] = listIndex;        /* scan result index to read from */
    hdr[3] = 1;                /* number of results to read            */
    
    SendMgmtMsg(hdr,             /* header           */
                sizeof(hdr),     /* size of header   */
                NULL,            /* no data          */
                0);              /* no data          */

    /* index 4 contains number of scan results returned, index 5 is first byte of first scan result */
    WaitForMgmtResponseAndReadData(WF_SCAN_GET_RESULTS_SUBTYPE,         /* expected subtype               */ 
                                   sizeof(tWFScanResult),               /* num data bytes to read         */
                                   5,                                   /* starting at this index         */
                                  (UINT8 *)p_scanResult);               /* write the response data here   */
                                  

    /* fix up endianness on the two 16-bit values in the scan results */
    p_scanResult->beaconPeriod = WFSTOHS(p_scanResult->beaconPeriod);
    p_scanResult->atimWindow   = WFSTOHS(p_scanResult->atimWindow);

    /* reference for how to retrieve RSSI */
    /* Display SSID  & Channel */ 
    /* sprintf(rssiChan, "  => RSSI: %u, Channel: %u\r\n",  p_scanResult->rssi, p_scanResult->channel);  */
    /* putsUART(rssiChan); */
}                        


#endif /* WF_CS_TRIS && WF_USE_SCAN_FUNCTIONS */
