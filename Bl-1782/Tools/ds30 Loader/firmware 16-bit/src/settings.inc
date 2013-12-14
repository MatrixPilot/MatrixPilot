;------------------------------------------------------------------------------
;
; Title:			ds30 loader
;
; File description:	user settings and configuration bits for PIC24 and dsPIC
;
; Copyright: 		Copyright 2009-2012 Mikael Gustafsson
;                                                                             
;------------------------------------------------------------------------------

;-----------------------------------------------------------------------------
;    This file is part of ds30 Loader.
;
;    ds30 Loader is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation.
;
;    ds30 Loader is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with ds30 Loader. If not, see <http://www.gnu.org/licenses/>.
;------------------------------------------------------------------------------


		
;------------------------------------------------------------------------------
; Device
;------------------------------------------------------------------------------
        ;.equ __24F16KA102, 		1			;xxx uncomment and change if using PIC24F device
        ;.equ __24FJ128GA010, 		1			;xxx uncomment and change if using PIC24FJ device
        ;.equ __24HJ128GP504, 		1			;xxx uncomment and change if using PIC24HJ device
        ;.equ __30F4013, 			1			;xxx uncomment and change if using dsPIC30F device
        .equ __33FJ256GP710A, 		1			;xxx uncomment and change if using dsPIC33FJ device


;------------------------------------------------------------------------------
; Includes
;------------------------------------------------------------------------------
        .include	"devices.inc"


;------------------------------------------------------------------------------
; User preferences
;------------------------------------------------------------------------------
		.equiv 	FCY, 				25000000	;xxx speed of internal cycle clock[Hz], used to calculate uart brg and delay
;		.equiv 	FCY, 				16000000	;xxx speed of internal cycle clock[Hz], used to calculate uart brg and delay
		.equiv	BLINIT,				3000		;xxx hello receive timeout [ms]
		.equiv	HELLOTRIES,			2			;xxx number of non hello characters received before branching to the user application    
		.equiv	BLTIME,				3000		;xxx data receive timeout [ms]
		

;------------------------------------------------------------------------------
; UART settings
;------------------------------------------------------------------------------		
        .equiv	USE_UART1,			1			;xxx uncomment to use uart1
        ;.equiv	USE_UART2,			1			;xxx uncomment to use uart2
        ;.equiv	USE_UART3,			1			;xxx uncomment to use uart3
        ;.equiv	USE_UART4,			1			;xxx uncomment to use uart4
		;.equ	USE_ALTIO,			1			;xxx uncomment to use uart 1 alternative i/o, only for dsPIC30F
;		.equiv 	BAUDRATE, 			9600		;xxx baudrate
 		.equiv 	BAUDRATE, 			115200		;xxx baudrate
        ;.equiv	USE_ABAUD,			1			;xxx uncomment to use auto baud rate detection, READ ERRATA FIRST        
        .equiv	USE_BRGH,			1			;xxx uncomment to use high baud rates, READ ERRATA FIRST, not available for PIC24F		
		
		;.equiv	USE_TXENABLE,		1			;xxx uncomment to use a tx enable pin		
		.ifdef USE_TXENABLE
			.equ    TXE_DELAY, 		10			;xxx time in us to wait before transmitting after pulling the tx enable pin high
			.equiv	TRISR_TXE,		TRISA		;xxx tris register containing tx enable
			.equiv 	LATR_TXE,		LATA		;xxx port register containing tx enable
			.equiv	TRISB_TXE,		TRISA1		;xxx tris bit for tx enable
			.equiv	LATB_TXE,		RA1			;xxx port bit for tx enable
		.endif
		
		
;------------------------------------------------------------------------------
; Advanced settings
;------------------------------------------------------------------------------
		;.equiv	KICK_WD,			1			;xxx uncomment to kick the wd in the rx loop, ONLY enable if watchdog is enabled
		.equiv	WRITE_VER,			1			;xxx do flash write verification
		.equiv	EWRITE_VER,			1			;xxx do eeprom write verification
		.equiv	PROT_GOTO,			1			;protect goto at 0x00
		.equiv	PROT_BL,			1			;protect bootloader
				
		.ifdef	IS_24F
			.equiv	BLPLP,			7			;bootloader placement, pages from end
			.equiv	BLSIZEP,		7			;bootloader size [pages], used by bootloader protection
		.endif
				
		.ifdef	IS_24FJ
			.equiv	BLPLP,			2			;bootloader placement, pages from end
			.equiv	BLSIZEP,		1			;bootloader size [pages], used by bootloader protection
		.endif
		
		.ifdef IS_24HJ
			.equiv	BLPLP,			1			;bootloader placement, pages from end
			.equiv	BLSIZEP,		1			;bootloader size [pages], used by bootloader protection
		.endif
				
		.ifdef	IS_30F
			.equiv	BLPLP,			9			;bootloader placement, pages from end
			.equiv	BLSIZEP,		9			;bootloader size [pages], used by bootloader protection
		.endif
		
		.ifdef IS_33FJ
			.equiv	BLPLP,			1			;bootloader placement, pages from end
			.equiv	BLSIZEP,		1			;bootloader size [pages], used by bootloader protection
		.endif


;------------------------------------------------------------------------------
; Configuration bits, these macros can be found at the end of the inc-files located in 
; c:\Program files\Microchip\MPLAB ASM30 Suite\Support\dsPIC33F\inc
;
; These can also be set in MPLAB IDE instead, they are found in Configure->Configuration bits...
;------------------------------------------------------------------------------
		.ifdef	IS_24F
			;config	__FBS,	BSS_OFF/*Boot Segment Code Protect*/ & BWRP_OFF/*Boot Segment Write Protect Enable*/
			;config	__FGS,	GCP_OFF/*General Segment Code Protect*/ & GWRP_OFF/*General Segment Write Protect Enable*/
			config	__FOSCSEL,	FNOSC_FRC/*Initial Oscillator Select*/ & IESO_OFF/*Two Speed Start-up*/
			config	__FOSC,	FCKSM_CSDCMD/*Clock switching and Fail-Safe Clock monitor*/ & SOSCSEL_SOSCLP/*Secondary Oscillator Select*/ & POSCFREQ_MS/*Primary Oscillator Frequency Range*/ & OSCIOFNC_ON/*OSCO Pin Configuration*/ & POSCMOD_XT/*Oscillator Selection*/
			;config	__FWDT,	FWDTEN_OFF/*Watchdog Timer*/ & WINDIS_OFF/*Windowed WDT*/ & FWPSA_PR32/*Watchdog prescaler*/ & WDTPS_PS1/*Watchdog postscale*/
			;config	__FPOR, MCLRE_ON/*Master Clear Enable*/ & BORV_LPBOR/*Brown Out Voltage*/ & I2C1SEL_PRI/*I2C1 pins Select*/ & PWRTEN_OFF/*Power Up Timer*/ & BOREN_BOR0/*Brown Out Reset*/
			;config	__FICD, BKBUG_OFF/*Background Debugger*/ & 	ICS_PGx1/*ICD pins select*/
			;config	__FDS,	DSWDTEN_OFF/*Deep Sleep Watchdog Timer*/ & DSBOREN_OFF/*Deep Sleep BOR*/ & RTCOSC_LPRC/*RTCC Reference Oscillator Select*/ & DSWDTOSC_LPRC/*Deep Sleep Watchdog Oscillator Clock Select*/ & DSWDTPS_DSWDTPS0/*Deep Sleep Watchdog Postscale*/
		.endif
				
		.ifdef	IS_24FJ
			config	__CONFIG1, JTAGEN_OFF/*JTAG*/ & GCP_OFF/*code protect*/ & GWRP_OFF/*write protect*/ & BKBUG_OFF/*background debugger*/ & COE_OFF/*clip-on emulation mode*/ & ICS_PGx2/*icd pins select*/ & FWDTEN_OFF/*watchdog timer*/ & WINDIS_OFF/*windowed wdt*/ & FWPSA_PR32/*watchdog prescalar*/ & WDTPS_PS1/*watchdog postscalar*/
			config	__CONFIG2, IESO_OFF/*two-speed startup*/ & FNOSC_FRC/*oscillator selection*/ & FCKSM_CSDCMD/*clock switching and clock monitor*/ & OSCIOFNC_OFF/*OSC0/RC15 function*/ & POSCMOD_XT/*oscillator selection*/
			;config 	__CONFIG3, WPEND_WPSTARTMEM/*write protect range selection*/ & WPCFG_WPCFGDIS/*write protection config page*/ & WPDIS_WPEN/*write protection*/ & WPFP_WPFP8/*Write Protection Flash Page*/
		.endif
		
		.ifdef IS_24HJ
			;config __FBS, 		RBS_NO_RAM & BSS_NO_FLASH & BWRP_WRPROTECT_OFF		;xxx boot segment
			;config __FSS, 		RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF		;xxx secure segment
			;config __FGS,		GSS_OFF & GCP_OFF & GWRP_OFF						;xxx general segment
			config 	__FOSCSEL, 	FNOSC_FRC & IESO_OFF 								;xxx oscillator mode
			config 	__FOSC, 	FCKSM_CSDCMD & OSCIOFNC_ON & POSCMD_XT				;xxx clock switching/clock monitor, osc2 & oscillator selection
			;config	__FWDT,		FWDTEN_OFF & WINDIS_OFF								;xxx watchdog
			;config	__FPOR,		FPWRT_PWR1											;xxx pwm, power-on reset
			;config	__FICD,		BKBUG_ON & COE_ON & JTAGEN_OFF & ICS_PGD1
		.endif
		
		.ifdef	IS_30F
			config 	__FOSC, 	CSW_FSCM_OFF & FRC_PLL16							;xxx oscillator
			config 	__FWDT, 	WDT_OFF												;xxx watchdog
			config 	__FBORPOR,	PBOR_OFF & PWRT_OFF & MCLR_EN						;xxx power-on reset, brown-out reset, master clear
			;config __FGS, 		CODE_PROT_OFF & GWRP_OFF							;xxx code protection
			config 	__FICD, 	ICS_PGD												;xxx comm channel select
		.endif
		
		.ifdef IS_33FJ
			;config 	__FBS, 		RBS_NO_RAM & BSS_NO_FLASH & BWRP_WRPROTECT_OFF	;xxx boot segment
			;config 	__FSS, 		RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF	;xxx secure segment
			;config	__FGS,		GSS_OFF & GCP_OFF & GWRP_OFF						;xxx general segment
			config 	__FOSCSEL, 	FNOSC_PRIPLL & IESO_OFF 							;xxx oscillator mode
			config 	__FOSC, 	FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_XT				;xxx clock switching/clock monitor, osc2 & oscillator selection
			config	__FWDT,		WDTPOST_PS256 & WDTPRE_PR128 & FWDTEN_OFF & WINDIS_OFF		;xxx watchdog
			config	__FPOR,		FPWRT_PWR1											;xxx pwm, power-on reset
			config	__FICD,		JTAGEN_OFF & ICS_PGD1								;xxx 
		.endif


;------------------------------------------------------------------------------
; End of file
;------------------------------------------------------------------------------
