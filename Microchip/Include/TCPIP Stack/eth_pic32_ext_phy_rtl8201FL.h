/*******************************************************************************
  RTL 8201FL PHY definitions

  Summary:
    
  Description:
*******************************************************************************/

/*******************************************************************************
FileName:  eth_pic32_ext_phy_rtl8201FL.h
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

#ifndef _NAT_RTL8201FL_H_

#define _NAT_RTL8201FL_H_

typedef enum
{
	PHY_REG_MACR		= 0x0D,
	PHY_REG_MAADR 		= 0x0E,
	PHY_REG_PSMR 		= 0x18,
	PHY_REG_FMLR 		= 0x1C,  // fiber mode and loopback register
	PHY_REG_IISDR		= 0x1E,  // Interrupt indicator and SNR display register
	PHY_REG_PAGESEL		= 0x1F,  // page select register

	PHY_REG_RMII_MODE_SEL = 0x10,
	PHY_REG_INT_WOL_LED_SEL = 0x13,  // register 19 , page 7
	PHY_REG_CUSTOMISED_LED_SET = 0x11,  // Register 17 , page 7
	PHY_REG_EEE_LEDS_ENA = 0x12,	// Register 18 , page 7
	PHY_REG_FIBER_MODE_LOOPBACK_AUTO_MDIX = 0x1D,
	
	
}ePHY_VENDOR_REG;

// basic registers
//

typedef union {
  struct {    
    unsigned :7;
    unsigned COLTEST:1;
    unsigned DUPLEX:1;
    unsigned AN_RESTART:1;
    unsigned ISOLATE:1;
    unsigned PDWN:1;
    unsigned AN_ENABLE:1;
    unsigned SPEED:1;
    unsigned LOOPBACK:1;
    unsigned RESET:1;
  };
  struct {
    unsigned short w:16;
  };
} __BMCONbits_t;	// reg 0: PHY_REG_BMCON
#define	_BMCON_COLTEST_MASK	0x0080
#define	_BMCON_DUPLEX_MASK	0x0100
#define	_BMCON_AN_RESTART_MASK	0x0200
#define	_BMCON_ISOLATE_MASK	0x0400
#define	_BMCON_PDWN_MASK	0x0800
#define	_BMCON_AN_ENABLE_MASK	0x1000
#define	_BMCON_SPEED_MASK	0x2000
#define	_BMCON_LOOPBACK_MASK	0x4000
#define	_BMCON_RESET_MASK	0x8000



typedef union {
  struct {    
    unsigned EXTEND_ABLE:1;
    unsigned JABBER_DET:1;
    unsigned LINK_STAT:1;
    unsigned AN_ABLE:1;
    unsigned REM_FAULT:1;
    unsigned AN_COMPLETE:1;
    unsigned PREAMBLE_SUPPRESS:1;
    unsigned :4;
    unsigned BASE10T_HDX:1;
    unsigned BASE10T_FDX:1;
    unsigned BASE100TX_HDX:1;
    unsigned BASE100TX_FDX:1;
    unsigned BASE100T4:1;
  };
  struct {
    unsigned short w:16;
  };
} __BMSTATbits_t;	// reg 1: PHY_REG_BMSTAT
#define	_BMSTAT_EXTEND_ABLE_MASK	0x0001
#define	_BMSTAT_JABBER_DET_MASK		0x0002
#define	_BMSTAT_LINK_STAT_MASK		0x0004
#define	_BMSTAT_AN_ABLE_MASK		0x0008
#define	_BMSTAT_REM_FAULT_MASK		0x0010
#define	_BMSTAT_AN_COMPLETE_MASK	0x0020
#define	_BMSTAT_PREAMBLE_SUPPRESS_MASK	0x0040
#define	_BMSTAT_BASE10T_HDX_MASK	0x0800
#define	_BMSTAT_BASE10T_FDX_MASK	0x1000
#define	_BMSTAT_BASE100TX_HDX_MASK	0x2000
#define	_BMSTAT_BASE100TX_FDX_MASK	0x4000
#define	_BMSTAT_BASE100T4_MASK		0x8000


#define	_BMSTAT_NEGOTIATION_MASK	(_BMSTAT_BASE10T_HDX_MASK|_BMSTAT_BASE10T_FDX_MASK| \
					_BMSTAT_BASE100TX_HDX_MASK|_BMSTAT_BASE100TX_FDX_MASK|_BMSTAT_BASE100T4_MASK)		// negotiation field mask
#define	_BMSTAT_NEGOTIATION_POS		11		// negotiation field position
#define	_BMSTAT_NEGOTIATION_LENGTH	5		// negotiation field length


// extended registers
//

typedef union {
  struct {
    unsigned OUI_MSB:16;
  };
  struct {
    unsigned short w:16;
  };
} __PHYID1bits_t;	// reg 2: PHY_REG_PHYID1



typedef union {
  struct {
    unsigned MNF_REV:4;    
    unsigned MNF_MODEL:6;
    unsigned OUI_LSB:6;
  };
  struct {
    unsigned short w:16;
  };
} __PHYID2bits_t;	// reg 3: PHY_REG_PHYID2
#define	_PHYID2_MNF_REV_MASK	0x000f
#define	_PHYID2_MNF_MODEL_MASK	0x03f0
#define	_PHYID2_OUI_LSB_MASK	0xfc00



typedef union {
  struct {
    unsigned PROT_SEL:5;    
    unsigned BASE10T:1;
    unsigned BASE10T_FDX:1;
    unsigned BASE100TX:1;
    unsigned BASE100TX_FDX:1;
    unsigned BASE100T4:1;
    unsigned PAUSE:1;			// NOTE: the PAUSE fields coding for SMSC is reversed!
    unsigned ASM_DIR:1;			// typo in the data sheet?	
    unsigned :1;
    unsigned REM_FAULT:1;
    unsigned :1;
    unsigned NP_ABLE:1;
  };
  struct {
    unsigned short w:16;
  };
} __ANADbits_t;		// reg 4: PHY_REG_ANAD
#define	_ANAD_PROT_SEL_MASK		0x001f
#define	_ANAD_BASE10T_MASK		0x0020
#define	_ANAD_BASE10T_FDX_MASK		0x0040
#define	_ANAD_BASE100TX_MASK		0x0080
#define	_ANAD_BASE100TX_FDX_MASK	0x0100
#define	_ANAD_BASE100T4_MASK		0x0200
#define	_ANAD_PAUSE_MASK		0x0400
#define	_ANAD_ASM_DIR_MASK		0x0800
#define	_ANAD_REM_FAULT_MASK		0x2000
#define	_ANAD_NP_ABLE_MASK		0x8000

#define	_ANAD_NEGOTIATION_MASK		(_ANAD_BASE10T_MASK|_ANAD_BASE10T_FDX_MASK|_ANAD_BASE100TX_MASK|_ANAD_BASE100TX_FDX_MASK| \
					_ANAD_BASE100T4_MASK)	// negotiation field mask
#define	_ANAD_NEGOTIATION_POS		5		// negotiation field position
#define	_ANAD_NEGOTIATION_LENGTH	5		// negotiation field length


typedef union {
  struct {    
    unsigned PROT_SEL:5;    
    unsigned BASE10T:1;
    unsigned BASE10T_FDX:1;
    unsigned BASE100TX:1;
    unsigned BASE100TX_FDX:1;
    unsigned BASE100T4:1;
    unsigned PAUSE:1;
    unsigned ASM_DIR:1;		
    unsigned :1;
    unsigned REM_FAULT:1;
    unsigned ACK:1;
    unsigned NP_ABLE:1;
  };
  struct {
    unsigned short w:16;
  };
} __ANLPADbits_t;	// reg 5: PHY_REG_ANLPAD
#define	_ANLPAD_PROT_SEL_MASK		0x001f
#define	_ANLPAD_BASE10T_MASK		0x0020
#define	_ANLPAD_BASE10T_FDX_MASK	0x0040
#define	_ANLPAD_BASE100TX_MASK		0x0080
#define	_ANLPAD_BASE100TX_FDX_MASK	0x0100
#define	_ANLPAD_BASE100T4_MASK		0x0200
#define	_ANLPAD_PAUSE_MASK		0x0400
#define	_ANLPAD_ASM_DIR_MASK		0x0800
#define	_ANLPAD_REM_FAULT_MASK		0x2000
#define	_ANLPAD_ACK_MASK		0x4000
#define	_ANLPAD_NP_ABLE_MASK		0x8000

typedef union {
  struct {    
    unsigned MESSAGE:11;
    unsigned TOGGLE:1;		
    unsigned ACK2:1;
    unsigned MSGP:1;
    unsigned ACK:1;
    unsigned NP:1;
  };
  struct {
    unsigned short w:16;
  };
} __ANLPADNPbits_t;	// reg 5: PHY_REG_ANLPADNP: next page

#define	_ANLPADNP_MESSAGE_MASK		0x7ff
#define	_ANLPADNP_TOGGLE_MASK		0x0800
#define	_ANLPADNP_ACK2_MASK		0x1000
#define	_ANLPADNP_MSGP_MASK		0x2000
#define	_ANLPADNP_ACK_MASK		0x4000
#define	_ANLPADNP_NP_MASK		0x8000




typedef union {
  struct {    
    unsigned LP_AN_ABLE:1;
    unsigned PAGE_RX:1;
    unsigned NP_ABLE:1;
    unsigned LP_NP_ABLE:1;
    unsigned PDF:1;
    unsigned :11;
  };
  struct {
    unsigned short w:16;
  };
} __ANEXPbits_t;	// reg 6: PHY_REG_ANEXP 
#define	_ANEXP_LP_AN_ABLE_MASK		0x0001
#define	_ANEXP_PAGE_RX_MASK		0x0002
#define	_ANEXP_NP_ABLE_MASK		0x0004
#define	_ANEXP_LP_NP_ABLE_MASK		0x0008
#define	_ANEXP_PDF_MASK			0x0010




typedef union {
  struct {    
    unsigned MESSAGE:11;
    unsigned TOGGLE:1;
    unsigned ACK2:1;
    unsigned MSGP:1;
    unsigned :1;
    unsigned NP:1;
  };
  struct {
    unsigned short w:16;
  };
} __ANNPTRbits_t;	// reg 7: PHY_REG_ANNPTR 
#define	_ANNPTR_MESSAGE_MASK		0x7ff
#define	_ANNPTR_TOGGLE_MASK		0x0800
#define	_ANNPTR_ACK2_MASK		0x1000
#define	_ANNPTR_MSGP_MASK		0x2000
#define	_ANNPTR_NP_MASK			0x8000

typedef union {
  struct {    
    unsigned MESSAGE:11;
    unsigned TOGGLE:1;
    unsigned ACK2:1;
    unsigned MSGP:1;
    unsigned ACK:1;
    unsigned NP:1;
  };
  struct {
    unsigned short w:16;
  };
} __ANLPRNPbits_t;	// reg 8: PHY_REG_ANLPRNP 
#define	_ANLPRNP_MESSAGE_MASK		0x7ff
#define	_ANLPRNP_TOGGLE_MASK		0x0800
#define	_ANLPRNP_ACK2_MASK		0x1000
#define	_ANLPRNP_MSGP_MASK		0x2000
#define	_ANLPRNP_ACK_MASK		0x4000
#define	_ANLPRNP_NP_MASK		0x8000


// RMII mode setting register
typedef union{
	struct{
		unsigned :3;
		unsigned RMII_CLKDIR:1;
		unsigned RMII_TX_OFFSET:4;
		unsigned RMII_RX_OFFSET:4;
		unsigned RMII_MODE:1;
		unsigned RMII_RXDV:1;
		unsigned RMII_RXDSEL:1;
		unsigned :1;
	};
	struct {
    unsigned short w:16;
  }; 
}_RMIIMODESETTINgbits_t;   // reg 0x10 page 7 RMII setting mode register

#define _RMII_CLK_DIR_MASK 		0x1000
#define _RMII_TX_OFFSET_MASK 	0x0f00
#define _RMII_RX_OFFSET_MASK	0x00f0
#define _RMII_MODE_SELECT_MASK	0x0008
#define _RMII_RXDV_SEL_MASK		0x0004
#define _RMII_RXDSEL_MASK		0x0002
#define _RMII_MODE_DEFAULT_SETTING_MASK 0x1FFA


//AUTO MDIX or Forced MDI/MDIX mode setting register 28
typedef union{
	struct{
		unsigned :10;
		unsigned FX_MODE:1;
		unsigned :2;
		unsigned EN_AUTO_MDIX:1;
		unsigned FORCED_MDI:1;
		unsigned :1;
	};
	struct {
    unsigned short w:16;
  }; 
}_FIBERMODEANDLOOPBACKMODE_t;   // reg 0x1D Fiber mode, loopback mode, AUTOMDIX and forced MDIX
#define _EN_AUTO_MDIX 		0x0004
#define _EN_FIBER_MODE 		0x0020   // enable fiber mode
#define _FORCED_MDI			0x0002   // use swapped MDIX elase normal forced MDIX


//Customised LEDs Setting Register 
typedef union{
	struct{
		unsigned :4;
		unsigned LED2_SEL:4;
		unsigned LED1_SEL_ACT:1;
		unsigned :1;
		unsigned LED1_SEL_100M:1;
		unsigned LED1_SEL_10M:1;
		unsigned LED0_SEL_ACT:1;
		unsigned :1;
		unsigned LED0_SEL_100M:1;
		unsigned LED0_SEL_10M:1;
	};
	struct {
    unsigned short w:16;
  }; 
}_CUSTOMIZEDLEDS_t;   // reg 0x11 , Page 7

#define _EN_LED1_SEL_ACT 		0x0080
#define _EN_LED1_SEL_100M		0x0020
#define _EN_LED1_SEL_10M		0x0010

#define _EN_LED0_SEL_ACT 		0x0008
#define _EN_LED0_SEL_100M		0x0002
#define _EN_LED0_SEL_10M		0x0001



//Page 7 register 19 Interrupt,WOL Enable, And LEDs Function 
typedef union{
	struct{
		unsigned :2;
		unsigned int_linkchg:1;
		unsigned int_dupchg:1;
		unsigned int_anerr:1;
		unsigned rg_led0_wol_sel:1;
		unsigned :3;
		unsigned led_sel:2;
		unsigned en_customised_led:1;
		unsigned :2;
		unsigned en10MlPi:1;
	};
	struct {
    unsigned short w:16;
  }; 
}_INT_WOL_LEDS_t;   // reg 0x13 , Page 7

#define _LINK_CHANGE_INT_MASK 							0x2000
#define _DUPLEX_CHANGE_INT_MASK							0x1000
#define _NWAY_ERROR_INT_MASK							0x0800
#define _LED_WOL_FUN_SEL								0x0400
#define _LED_FUNCTION_SELC_ACTALL_LINK100				0x0000
#define _LED_FUNCTION_SELC_LINKALL_ACTALL_LINK100		0x0010
#define _LED_FUNCTION_SELC_ACTALL_LINK10_LINK100		0x0020
#define _LED_FUNCTION_SELC_ACT10_LINK10_LINK100_ACT100	0x0030
#define _EN_CUSTOMISED_LED 								0x0008
#define _EN_10M_PI										0x0001

typedef enum
{
	PAGENUM_0=0,
	PAGENUM_4=4,
	PAGENUM_7=7,
}ePAGENUMBERSEL;

#endif	// _NAT_RTL8201FL_H_


