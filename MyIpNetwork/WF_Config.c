/******************************************************************************

 MRF24W Driver Customization
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WF_Config.c
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
 KH                 27 Jan 2010 Created for MRF24W
******************************************************************************/


#include "HardwareProfile.h"

#if defined(WF_CS_TRIS)

/*==========================================================================*/
/*                                  INCLUDES                                */
/*==========================================================================*/
#include "TCPIP Stack/TCPIP.h"

#if defined ( EZ_CONFIG_SCAN )
#include "TCPIP Stack/WFEasyConfig.h"
#endif /* EZ_CONFIG_SCAN */


/*==========================================================================*/
/*                                  DEFINES                                 */
/*==========================================================================*/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_CONFIG
#endif


#if defined(STACK_USE_DHCP_CLIENT)
void RenewDhcp(void);
#endif

extern BOOL GetAppPowerSaveMode(void);
extern void ReenablePowerSaveMode(void);

#if defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST)
extern tPassphraseReady g_WpsPassphrase;
#endif    /* defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST) */

/*****************************************************************************
 * FUNCTION: WF_ProcessEvent
 *
 * RETURNS:  None
 *
 * PARAMS:   event      -- event that occurred
 *           eventInfo  -- additional information about the event.  Not all events
 *                         have associated info, in which case this value will be
 *                         set to WF_NO_ADDITIONAL_INFO (0xff)
 *           extraInfo - more additional information about the event.
 *                            WPA Passphrase that is sent to the host in case the host wants to speed 
 *                            up connection by doing the passphrase to key calculation
 *
 *  NOTES:   The Host application must NOT directly call this function.  This 
 *           function is called by the WiFi Driver code when an event occurs
 *           that may need processing by the Host CPU.  
 *
 *           No other WiFi Driver function should be called from this function, with the
 *           exception of WF_ASSERT.  It is recommended that if the application wishes to be 
 *           notified of an event that it simply set a flag and let application code in the 
 *           main loop handle the event.  
 *
 *           WFSetFuncState must be called when entering and leaving this function.  
 *           When WF_DEBUG is enabled this allows a runtime check if any illegal WF functions 
 *           are called from within this function.
 *
 *           For events that the application is not interested in simply leave the
 *           case statement empty.
  *
 *           Customize this function as needed for your application.
 *****************************************************************************/
void WF_ProcessEvent(UINT8 event, UINT16 eventInfo, UINT8 *extraInfo)
{
    #if defined(STACK_USE_UART)
    char buf[8];
    #endif  /* defined(STACK_USE_UART) */

    /* this function tells the WF driver that we are in this function */
    WFSetFuncState(WF_PROCESS_EVENT_FUNC, WF_ENTERING_FUNCTION);
      
    switch (event)
    {
        /*--------------------------------------*/
        case WF_EVENT_CONNECTION_SUCCESSFUL:
        /*--------------------------------------*/   
            #if defined(STACK_USE_UART)
            putrsUART("Event: Connection Successful\r\n"); 
            #endif
            break;
        
        /*--------------------------------------*/            
        case WF_EVENT_CONNECTION_FAILED:
        case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
        case WF_EVENT_CONNECTION_PERMANENTLY_LOST:            
        /*--------------------------------------*/
            #if defined(STACK_USE_UART)       
            WF_OutputConnectionDebugMsg(event, eventInfo);
            #endif
            break;

        /*--------------------------------------*/    
        case WF_EVENT_CONNECTION_REESTABLISHED:
        /*--------------------------------------*/
            #if defined(STACK_USE_UART)
            putrsUART("Event: Connection Reestablished\r\n");
            #endif
            break;
            
        /*--------------------------------------*/
        case WF_EVENT_SCAN_RESULTS_READY:
        /*--------------------------------------*/  
            #if defined(STACK_USE_UART)
            putrsUART("Event: Scan Results Ready,");
            sprintf(buf, " %d", eventInfo);
            putsUART(buf);
            putrsUART("results\r\n");
            #endif /* STACK_USE_UART */

            #if defined ( EZ_CONFIG_SCAN ) && !defined(__18CXX)
            WFScanEventHandler(eventInfo);
            #endif /* EZ_CONFIG_SCAN */
            break;
            
        case WF_EVENT_KEY_CALCULATION_REQUEST:
            #if defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST)
                g_WpsPassphrase.valid = TRUE;
                memcpy((void *)&g_WpsPassphrase.passphrase, (void *)extraInfo, sizeof(g_WpsPassphrase.passphrase));
                #if defined(STACK_USE_UART)
                    putrsUART("Event: WPS pass phrase Ready\r\n");
                #endif /* STACK_USE_UART */
            #endif  /* defined(DERIVE_KEY_FROM_PASSPHRASE_IN_HOST) */    
            break;  
        
        default:
            WF_ASSERT(FALSE);  /* unknown event */
            break;
    }        
    
    /* Informs the WF driver that we are leaving this function */
    WFSetFuncState(WF_PROCESS_EVENT_FUNC, WF_LEAVING_FUNCTION);
}    


#endif /* WF_CS_TRIS */

