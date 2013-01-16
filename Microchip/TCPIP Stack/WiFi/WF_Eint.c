/******************************************************************************

 MRF24W Driver Exernal Interrupt
 Module for Microchip TCP/IP Stack
  -Provides access to MRF24W WiFi controller
  -Reference: MRF24W Data sheet, IEEE 802.11 Standard

*******************************************************************************
 FileName:      WF_Eint.c
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
 Michael Palladino    10/13/07    Original
 KO                   31 Oct 2008 Port to PIC24F and PIC32 for TCP/IP stack v4.52
 KH                   19 Jun 2009 Modified MACMemCopyAsync to support TCB to TCB copy
 KH                   27 Jan 2010 Updated for MRF24W
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
    #define WF_MODULE_NUMBER  WF_MODULE_WF_EINT
#endif


/*****************************************************************************
 * FUNCTION:WF_EintIsDisabled
 *
 * RETURNS: TRUE if MRF24W External Interrupt is disabled, else returns FALSE
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to determine if the MRF24W External Interrupt 
 *          is disabled.  
 *****************************************************************************/
BOOL WF_EintIsDisabled(void)
{
    return(WF_INT_IE == 0);  /* works for PIC18, PIC24, and PIC32 */
}

BOOL WF_EintIsPending(void)
{
    return(((WF_INT_IO == 0) && (WF_INT_IF == 0)));  /* works for PIC18, PIC24, and PIC32 */
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*                                  PIC18 Interrupt Routines                                                          */
/*====================================================================================================================*/
/*====================================================================================================================*/
#if defined( __18CXX )
/*****************************************************************************
 * PIC18 INTERRUPT SERVICE ROUTINE (Called from LowISR() in MainDemo.c)
 *****************************************************************************/
void WFEintISR(void)
{
    // if EINT enabled
    if ( WF_INT_IE == 1 )
    {
        // if EINT event occurred
        if ( WF_INT_IF == 1 )
        {
            // clear EINT
            WF_INT_IF = 0;
            WF_INT_IE = 0;         // disable external interrupt

            // invoke handler
            WFEintHandler();
        }
    }
}

/*****************************************************************************
 * FUNCTION:WF_EintEnable (Specific to PIC18)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to enable the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintEnable(void)
{
    // if interrupt line is low, then we may have missed a falling edge
    // while the interrupt was disabled.
    if ( WF_INT_IO == 0 )
    {
        // if the interrupt pin is active
        // then the MRF24W has another event that needs to be serviced.
        // This means that the MRF24W will never generate another falling edge
        // required to trigger the interrupt... So, we must force an interrupt.
        // force the EINT
        WF_INT_IF = 1;
    }

    /* enable the external interrupt */
    WF_INT_IE = 1;
}

/*****************************************************************************
 * FUNCTION:WF_EintDisable (Specific to PIC18)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to disable the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintDisable(void)
{
    /* disable the external interrupt */
    WF_INT_IE = 0;
}

/*****************************************************************************
 * FUNCTION:WF_EintInit (Specific to PIC18)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to initialize the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintInit(void)
{
    /* disable the external interrupt */
    WF_INT_IE = 0;
//  WF_INT_IP = 0;

    /* configure IO pin as input and External Interrupt pin*/
    /* assume port b pullups were enabled before entry */
    WF_INT_TRIS = 1;
    WF_INT_EDGE = 0;   /* falling edge triggered */

    /* clear and enable the interrupt */
    WF_INT_IF = 0;
    WF_INT_IE = 1;
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*                                  PIC24 Interrupt Routines                                                          */
/*====================================================================================================================*/
/*====================================================================================================================*/
#elif defined( __C30__ )

/*****************************************************************************
 * PIC24 INTERRUPT SERVICE ROUTINE
 *****************************************************************************/
void __attribute__((interrupt, auto_psv)) _INT1Interrupt(void)
{
    // clear EINT
    if (WF_INT_IF && WF_INT_IE)
    {
        WF_INT_IF = 0;
        WF_INT_IE = 0;         /* disable external interrupt */
        // invoke handler
        WFEintHandler();
    }
}

/*****************************************************************************
 * FUNCTION: WF_EintEnable (Specific to PIC24)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to enable the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintEnable(void)
{
    // if interrupt line is low, then we may have missed a falling edge
    // while the interrupt was disabled.
    if ( WF_INT_IO == 0 )
    {
        // if the interrupt pin is active
        // then the MRF24W has another event that needs to be serviced.
        // This means that the MRF24W will never generate another falling edge
        // required to trigger the interrupt... So, we must force an interrupt.
        // force the EINT
        WF_INT_IF = 1;
    }

    /* enable the external interrupt */
    WF_INT_IE = 1;
}


/*****************************************************************************
 * FUNCTION: WF_EintDisable (Specific to PIC24)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to disable the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintDisable(void)
{
    /* disable the external interrupt */
    WF_INT_IE = 0;
}

/*****************************************************************************
 * FUNCTION: WF_EintInit (Specific to PIC24)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to initialize the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintInit(void)
{
    /* disable the external interrupt */
    WF_INT_IE = 0;

    /* configure IO pin as input and External Interrupt pin*/
    /* set the I/O high since we do not have pull-ups */
    WF_INT_IO   = 1;
    WF_INT_TRIS = 1;
    WF_INT_EDGE = 1;   /* falling edge triggered */

    /* clear and enable the interrupt */
    WF_INT_IF = 0;
    WF_INT_IE = 1;
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*                                  PIC32 Interrupt Routines                                                          */
/*====================================================================================================================*/
/*====================================================================================================================*/
#elif defined( __PIC32MX__ )

/*****************************************************************************
 * PIC32 INTERRUPT SERVICE ROUTINE
 *****************************************************************************/
#if defined( MRF24W_IN_SPI2 )
void __attribute((interrupt(ipl3), vector(_EXTERNAL_3_VECTOR), nomips16)) _WFInterrupt(void)
#else
void __attribute((interrupt(ipl3), vector(_EXTERNAL_1_VECTOR), nomips16)) _WFInterrupt(void)
#endif
{
    // clear EINT
    if (WF_INT_IF && WF_INT_IE)
    {
        WF_INT_IF_CLEAR = WF_INT_BIT;
        WF_INT_IE_CLEAR = WF_INT_BIT;         /* disable external interrupt */
        
        /* invoke handler */
        WFEintHandler();
    }
}

/*****************************************************************************
 * FUNCTION:WF_EintEnable (Specific to PIC32)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to enable the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintEnable(void)
{
    // if interrupt line is low, then we may have missed a falling edge
    // while the interrupt was disabled.
    if ( WF_INT_IO == 0 )
    {
        // if the interrupt pin is active
        // then the MRF24W has another event that needs to be serviced.
        // This means that the MRF24W will never generate another falling edge
        // required to trigger the interrupt... So, we must force an interrupt.
        // force the EINT
        WF_INT_IF_SET = WF_INT_BIT;
    }

    /* enable the external interrupt */
    WF_INT_IE_SET = WF_INT_BIT;
}


/*****************************************************************************
 * FUNCTION:WF_EintDisable (Specific to PIC32)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to disable the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintDisable(void)
{
    /* disable the external interrupt */
    WF_INT_IE_CLEAR = WF_INT_BIT;
}

/*****************************************************************************
 * FUNCTION:WF_EintInit (Specific to PIC32)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to initialize the MRF24W External Interrupt.  
 *****************************************************************************/
void WF_EintInit(void)
{
    /* disable the external interrupt */
    WF_INT_IE_CLEAR = WF_INT_BIT;

    /* configure IO pin as input and External Interrupt pin*/
    /* set the I/O high since we do not have pull-ups */
    WF_INT_IO   = 1;
    WF_INT_TRIS = 1;
    WF_INT_EDGE = 0;   /* falling edge triggered */

    /* clear and enable the interrupt */
    WF_INT_IF_CLEAR    = WF_INT_BIT;
    WF_INT_IPCCLR      = WF_INT_IPC_MASK;
    WF_INT_IPCSET      = WF_INT_IPC_VALUE;
    WF_INT_IE_SET      = WF_INT_BIT;
}
#endif 

#else
    // dummy func to keep compiler happy when module has no executeable code
    void MCHP_Eint_EmptyFunc(void)
    {
    }

#endif /* WF_CS_TRIS */
