/*******************************************************************************
  RTL 8201FL PHY API for Microchip TCP/IP Stack
*******************************************************************************/

/*******************************************************************************
FileName:   eth_pic32_ext_phy_rtl8201FL.c
Description : Only RMII configuration is supported with RTLPHY8201FL PHY.
                   This PHY driver works in Default Ethernet IO mode with PIC32MX675F512H .
                   If Alternate Ethernet IO mode is used for other PIC devices, then configuration bit need 
                   to be changed w.r.t Alternate Ethernet IO.
                   To get the status of MAC link,  PHY Link Status need to be read twice.
                   Here Refresh value will be one for EthPhyGetLinkStatus(int refresh).

Copyright (C) 2012 released Microchip Technology Inc.  All rights
reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/

#include <plib.h>

#include "HardwareProfile.h"


// Compile only for PIC32MX with Ethernet MAC interface 
#if defined(__PIC32MX__) && defined(_ETH) && !defined(ENC100_INTERFACE_MODE) && !defined(ENC_CS_TRIS) && !defined(WF_CS_TRIS)

#include "TCPIP_Stack/ETHPIC32ExtPhy.h"

#include "TCPIP_Stack/Eth_pic32_ext_phy_rtl8201FL.h"

#include "TCPIP_Stack/TCPIP.h"




/****************************************************************************
 *                 interface functions
 ****************************************************************************/
/****************************************************************************
 * Function:		EthPhyConfigureCustomisedLED
 *
 * PreCondition:	- Communication to the PHY should have been established.
 *
 * Input:			void
 *
 * Output:		ETH_RES_OK - success,
 *				an error code otherwise
 *
 *
 * Side Effects:	None
 *
 * Overview:		This function is used to Customise LED functionality. If ETH_CFG_LINK
 *				set to 1 to config the Speed and Link to a specific parameter and set to 0 for 
 				a deafult Speed and Link configuration.
 *
 * Note:			None
 *****************************************************************************/
eEthRes EthPhyConfigureCustomisedLED(void)
{
	unsigned short LEDEna=0;
	int ledFlags ;
    char ethCnfLink = ETH_CFG_LINK;

	// Customised LED function selection in page 7,Register 19
	EthMIIMWriteStart(PHY_REG_PAGESEL, PHY_ADDRESS, PAGENUM_7);
	EthMIIMReadStart(PHY_REG_INT_WOL_LED_SEL, PHY_ADDRESS);
	LEDEna=EthMIIMReadResult();

	LEDEna |= _EN_CUSTOMISED_LED;  // enabled Customised LED
	EthMIIMWriteStart(PHY_REG_INT_WOL_LED_SEL, PHY_ADDRESS, LEDEna);
	EthMIIMWriteStart(PHY_REG_PAGESEL, PHY_ADDRESS, PAGENUM_0);

	// LED 0 = 10/100MBPS speed  and LED1 =  SPEED LINK+ACTIVITY  Configure for register 17 page 7
	EthMIIMWriteStart(PHY_REG_PAGESEL, PHY_ADDRESS, PAGENUM_7);
	EthMIIMReadStart(PHY_REG_CUSTOMISED_LED_SET, PHY_ADDRESS);
	LEDEna=EthMIIMReadResult();
	//For LED1 , Speed and Link Activity details - 
	LEDEna |= _EN_LED1_SEL_ACT|_EN_LED1_SEL_100M|_EN_LED1_SEL_10M;

	// set  LED0 Speed and Link activity details
    if(ethCnfLink == 1)
    {
        ledFlags =ETH_CFG_10?ETH_CFG_10:0;
        ledFlags|=ETH_CFG_100?ETH_CFG_100:0;

		// if the speed is configured to 10 MBPS
        if(ledFlags & ETH_CFG_10)
    	{
            LEDEna |= _EN_LED0_SEL_10M;
    	}
		// if the speed is configured to 100 MBPS
        if(ledFlags & ETH_CFG_100)
    	{
            LEDEna |= _EN_LED0_SEL_100M;
    	}
    }
    else
    {
        LEDEna |= _EN_LED0_SEL_100M |_EN_LED0_SEL_10M;
    }

	EthMIIMWriteStart(PHY_REG_CUSTOMISED_LED_SET, PHY_ADDRESS, LEDEna);
	EthMIIMWriteStart(PHY_REG_PAGESEL, PHY_ADDRESS, PAGENUM_0);
	
	return ETH_RES_OK;  
}



/****************************************************************************
 * Function:        EthPhyConfigureMII
 *
 * PreCondition: Communication to the PHY should have been established.
 *
 * Input:           cFlags - the requested open flags: ETH_PHY_CFG_RMII/ETH_PHY_CFG_MII
 *
 * Output:         ETH_RES_OK - success,
 *                     an error code otherwise
 *
 *
 * Side Effects:  None
 *
 * Overview:     This function configures the PHY only for RMII operation mode. Both LED0 and LED1 
 *			  are customised with respect to Speed and Activity. The customised LED function 
 *                     will be called after RMII mode initialization.
 * Note:            None
 *****************************************************************************/
eEthRes EthPhyConfigureMII(eEthPhyCfgFlags cFlags)
{
	unsigned short	phyReg=0;

	if((cFlags&ETH_PHY_CFG_RMII) == 0)
	{
        return ETH_RES_CFG_ERR;     // MII configuration not supported for now!
	}

	// RMII mode setting is in page 7, Register 16 , 3rd bit is used for MII or RMII mode
	EthMIIMWriteStart(PHY_REG_PAGESEL, PHY_ADDRESS, PAGENUM_7);
	
	EthMIIMReadStart(PHY_REG_RMII_MODE_SEL, PHY_ADDRESS);
	phyReg=EthMIIMReadResult();
	
    phyReg|=_RMII_MODE_SELECT_MASK;
	
	EthMIIMWriteStart(PHY_REG_RMII_MODE_SEL, PHY_ADDRESS, phyReg);	// update the RMII and Bypass Register	
	EthMIIMWriteStart(PHY_REG_PAGESEL, PHY_ADDRESS, PAGENUM_0);

    EthPhyConfigureCustomisedLED();
	return ETH_RES_OK;  
}


 /****************************************************************************
 * Function:        EthPhyConfigureMdix
 *
 * PreCondition:    - Communication to the PHY should have been established.
 *
 * Input:           oFlags - the requested open flags: ETH_OPEN_MDIX_AUTO, ETH_OPEN_MDIX_NORM/ETH_OPEN_MDIX_SWAP
 *
 * Output:          ETH_RES_OK - success,
 *                  an error code otherwise
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function configures the MDIX mode for the PHY.
 *
 * Note:            None
 *****************************************************************************/
eEthRes EthPhyConfigureMdix(eEthOpenFlags oFlags)
{
	unsigned short	phyReg;

	EthMIIMReadStart(PHY_REG_FIBER_MODE_LOOPBACK_AUTO_MDIX, PHY_ADDRESS);
	phyReg=EthMIIMReadResult();

	if(oFlags&ETH_OPEN_MDIX_AUTO)
	{	// enable Auto-MDIX
		phyReg|=_EN_AUTO_MDIX;
	}
	else
	{	// no Auto-MDIX
		phyReg&=~(_EN_AUTO_MDIX);	// disable Auto-MDIX
		if(oFlags&ETH_OPEN_MDIX_SWAP)
		{
		// force MDI
		   phyReg|=_FORCED_MDI;	// swap FORCED MDI
		}
		else
		{
		   phyReg&=~(_FORCED_MDI);	// normal FORCED MDIX
		}
		
	}	
	return ETH_RES_OK;	
//	return (oFlags&ETH_OPEN_MDIX_AUTO)?ETH_RES_OK:ETH_RES_CFG_ERR;
}
/****************************************************************************
 * Function:        EthPhyMIIMAddress
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          PHY MIIM address
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the address the PHY uses for MIIM transactions			     
 *
 * Note:            None
 *****************************************************************************/
unsigned int EthPhyMIIMAddress(void)
{
	return PHY_ADDRESS;
}


/****************************************************************************
 * Function:        EthPhyMIIMClock
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          PHY MIIM clock, Hz
 *
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the maximum clock frequency that the PHY can use for the MIIM transactions
 *
 * Note:            None
 *****************************************************************************/
unsigned int EthPhyMIIMClock(void)
{
	return 2500000;		//  2.5 MHz max clock supported
}


#endif  // ( ((__PIC32_FEATURE_SET__ >= 500) && (__PIC32_FEATURE_SET__ <= 799) && defined (_ETH)) && defined(TCPIP_IF_PIC32INT) )

