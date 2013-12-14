;------------------------------------------------------------------------------
;
; Product:			ds30 Loader
;
; File description:	main firmwarefile for PIC24 and dsPIC
;
; Copyright: 		Copyright 2009-2012 Mikael Gustafsson
;
; Version			4.0.3 March 2012
;
; Webpage: 			http://mrmackey.no-ip.org/elektronik/ds30loader/
;
; History:			4.0.3 Improvement: added delay for tx enable pin
;					      Change: size for PIC24F
;					4.0.2 -
;					4.0.1 Improvement: code structure
;                   4.0.0 First version released
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
; Register usage
;------------------------------------------------------------------------------
		;.equiv	MIXED,		W0		;immediate
		;.equiv	MIXED,		W1		;immediate
		.equiv	WBUFPTR,	W2		;buffer pointer
		.equiv	WCNT,		W3		;loop counter
		.equiv	WADDR2,		W4		;memory pointer
		.equiv	WADDR,		W5		;memory pointer
		.equiv	WPPSTEMP1,	W6		;used to restore pps register
		.equiv	WPPSTEMP2,	W7		;used to restore pps register
		.equiv	WTEMP1,		W8		;
		.equiv	WTEMP2,		W9		;
		.equiv	WDEL1,		W10		;delay outer
		.equiv	WDEL2,		W11		;delay inner
		.equiv	WDOERASE,	W12		;flag indicated erase should be done before next write
		.equiv	WCMD,		W13		;command
		.equiv 	WCRC, 		W14		;checksum
		.equiv	WSTPTR,		W15		;stack pointer


;------------------------------------------------------------------------------
; Includes
;------------------------------------------------------------------------------
		.include "settings.inc"	
		.include "user_code.inc"


;------------------------------------------------------------------------------
; Defines
;------------------------------------------------------------------------------			
		.equiv	VERMAJ,			4						/*firmware version major*/
		.equiv	VERMIN,			0						/*fimrware version minor*/
		.equiv	VERREV,			3						/*firmware version revision*/

		.equiv 	HELLO, 			0xC1		
		.equiv 	OK, 			'K'						/*erase/write ok*/
		.equiv 	CHECKSUMERR,	'N'						/*checksum error*/
		.equiv	VERFAIL,		'V'						/*verification failed*/
		.equiv  BLPROT,     	'P'                     /*bl protection tripped*/		
		.equiv  UCMD,     		'U'                     /*unknown command*/		
		
		.equiv	PWOK,			0xFE	

		.equiv	BLSTART,		( BLINIT * (FCY / 1000) / (65536 * 7) )				/*count for boot receive delay*/
		.equiv	BLDELAY,		( BLTIME * (FCY / 1000) / (65536 * 7) )				/*count for receive delay*/
			
		.equiv	STARTADDR,		( FLASHSIZE - BLPLP * PAGESIZER * ROWSIZEW * 2 )	/*bootloader placement*/		
		.equiv	BLSTARTROW,		(STARTADDR / ROWSIZEW / 2)	
		.equiv	BLENDROW,		(STARTADDR / ROWSIZEW / 2 + (BLSIZEP*PAGESIZER) - 1)			


;------------------------------------------------------------------------------
; Variables
;------------------------------------------------------------------------------
		.equiv	BUFFERSIZE,		(ROWSIZEW*3 + 8/*MAC*/ + 1/*checksum*/)
		.bss	_buffer, BUFFERSIZE, 8		
		

;------------------------------------------------------------------------------
; Globals
;------------------------------------------------------------------------------
        .global _buffer


;------------------------------------------------------------------------------
; Send macro
;------------------------------------------------------------------------------
		.macro SendL char
			mov		#\char, W0
			rcall 	Send
		.endm
		

;------------------------------------------------------------------------------
; Reset vector
;------------------------------------------------------------------------------
        .global __reset
				

;------------------------------------------------------------------------------
; GOTO user application
;------------------------------------------------------------------------------
		.section *, code, address(STARTADDR-4)
usrapp:	nopr						;these two instructions will be replaced
		nopr						;with a goto to the user application by the pc program
		

;------------------------------------------------------------------------------
; Start of bootloader code
;------------------------------------------------------------------------------
		.section *, code, address(STARTADDR)
__reset:mov		#__SP_init, WSTPTR	;initalize the Stack Pointer	

      			
;------------------------------------------------------------------------------
; Init 
;------------------------------------------------------------------------------
		UserInit					;macro in user_code.inc
		clr		WDOERASE     			
		rcall	CommInit
		
		
;------------------------------------------------------------------------------
; Receive hello
;------------------------------------------------------------------------------
		mov		#HELLOTRIES, WCNT
rhello:	mov		#BLSTART, WDEL1
		rcall 	ReceiveInit
		sub 	#HELLO, W0
		bra		z, rhellofinished		
		; Not hello received
		dec		WCNT, WCNT
		bra		z, exit
		bra		rhello		
rhellofinished:


;------------------------------------------------------------------------------
; Send device id and firmware version
;------------------------------------------------------------------------------
		SendL 	( DEVICEID & 0xff )
		SendL	( ((DEVICEID&0x100)>>1) + VERMAJ )
		SendL	( ((DEVICEID&0x200)>>2) + (VERMIN<<4) + VERREV )
		

;------------------------------------------------------------------------------
; Main
;------------------------------------------------------------------------------
Main:	SendL 	OK		
main1:	clr 	WCRC					;init checksum		
		
			
		;----------------------------------------------------------------------
		; Receive address
		;----------------------------------------------------------------------
		; Upper byte
		rcall 	Receive
		mov 	W0, TBLPAG
		; High byte, use PR1 as temporary sfr
		rcall 	Receive		
		mov.b	WREG, PR1+1
		; Low byte, use PR1 as temporary sfr
		rcall 	Receive
		mov.b	WREG, PR1
		;
		mov		PR1, WREG
		mov		W0,	WADDR
		mov		W0, WADDR2
		
		
		;----------------------------------------------------------------------
		; Receive command
		;----------------------------------------------------------------------
		rcall 	Receive
		mov		W0, WCMD
		

		;----------------------------------------------------------------------
		; Receive nr of data bytes that will follow
		;----------------------------------------------------------------------
		rcall 	Receive				
		mov 	W0, WCNT
		mov		W0, WTEMP1
	

		;----------------------------------------------------------------------
		; Receive data		
		;----------------------------------------------------------------------
		mov 	#_buffer, WBUFPTR
rcvdata:
		rcall 	Receive		
		mov.b 	W0, [WBUFPTR++]		
		dec		WCNT, WCNT	
		bra 	nz, rcvdata			;last byte received is checksum			
			
				
		;----------------------------------------------------------------------
		; Check checksum
		;----------------------------------------------------------------------
		cp0.b 	WCRC
		bra 	z, chksumok
		SendL 	CHECKSUMERR
		bra 	main1					
chksumok:
		
		
		;----------------------------------------------------------------------
		; 0x00 goto protection
		;----------------------------------------------------------------------	
		.ifdef	PROT_GOTO
			; Only for write command
			btss	WCMD,	#1		
			bra		protgotook		
			; Only first row
			cp0		TBLPAG
			bra		nz, protgotook
			cp0		PR1
			bra		nz, protgotook			
			;
			mov 	#_buffer, WBUFPTR
			; 1st word upper byte = goto instruction
			mov.b 	#0x04, W0
			mov.b	W0, [WBUFPTR++] 		
			; 1st word  low byte = low address byte
			mov.b 	#(0xff & STARTADDR), W0
			mov.b 	W0, [WBUFPTR++] 
			; 1st word high byte = high address byte
			mov.b 	#(0xff & (STARTADDR>>8)), W0
			mov.b 	W0, [WBUFPTR++]	
			;2nd word upper byte = unused
			clr.b	[WBUFPTR++]	
			; 2nd word  low byte = upper address byte
			mov.b 	#(0xff & (STARTADDR>>16)), W0
			mov.b 	W0, [WBUFPTR++]  
			; 2nd word high byte = unused
			clr.b 	[WBUFPTR++]  
protgotook:
		.endif
			
		
		;----------------------------------------------------------------------
		; Check command
		;----------------------------------------------------------------------
		mov 	#_buffer, WBUFPTR		; Init pointer	
		
					
		; Erase page
		.ifdef HAS_PAGES
			btsc	WCMD,	#0		
			bra		eraseact		
		.endif
		
		; Write row
		btsc	WCMD,	#1		
		bra		blprot
		
		; Write EEROM word
		.if EESIZEB > 0
			btsc	WCMD, #2
			bra		eeprom		
		.endif
		
		; Write config byte/word
		.ifdef CFG_WR_CODE
			btsc	WCMD, #3
			bra		config
		.endif
		
		; Else, unknown command
		SendL   UCMD		
		bra     main1	
		
					
		;----------------------------------------------------------------------
		; "Erase activation"
		;----------------------------------------------------------------------	
		.ifdef HAS_PAGES	
eraseact:	mov		#0xffff, WDOERASE
			bra		Main
		.endif
					
					
		;----------------------------------------------------------------------
		; Bootloader protection
		;----------------------------------------------------------------------
blprot:	
		.ifdef PROT_BL
			; Calculate row number of received address
			mov		TBLPAG, W1
			mov		WADDR, W0
			mov		#(ROWSIZEW*2), WTEMP1
			repeat	#17
			div.ud	W0, WTEMP1;		W = received address / (rowsizew*2)
			; Received row < bl start row = OK
			mov		#BLSTARTROW, WTEMP1
			cp		W0, WTEMP1
			bra		N, blprotok
			; Received row > bl end row = OK
			mov		#BLENDROW, WTEMP1
			cp		WTEMP1, W0
			bra		N, blprotok		
			; Protection tripped
			SendL   BLPROT		
		    bra     main1
			; Restore WADDR2
blprotok:	mov		WADDR, WADDR2
		.endif
		
					
		;----------------------------------------------------------------------		
		; Erase page
		;----------------------------------------------------------------------		
		.ifdef HAS_PAGES
			btss	WDOERASE, #0
			bra		program
			tblwtl	WADDR, [WADDR]			;set base address of erase block
			; Erase
			mov 	#FL_ER_CODE, W0
			rcall 	Write	
			; Erase finished
			clr		WDOERASE
		.endif
		
		
		;----------------------------------------------------------------------		
		; Write row
		;----------------------------------------------------------------------		
program:
		; Erase row
		.ifndef	HAS_PAGES		
			tblwtl	WADDR, [WADDR]			;set base address of erase block
			mov 	#FL_ER_CODE, W0
			rcall 	Write
		.endif		
		
		; Load latches
		mov 	#ROWSIZEW, WCNT
latlo:	tblwth.b 	[WBUFPTR++], [WADDR] 	;upper byte
		tblwtl.b	[WBUFPTR++], [WADDR++] 	;low byte
		tblwtl.b	[WBUFPTR++], [WADDR++] 	;high byte	
		dec 	WCNT, WCNT
		bra 	nz, latlo
		; Write flash row
		mov 	#FL_WR_CODE, W0		
		rcall 	Write		
		
		; Verify row
		.ifdef WRITE_VER
		mov 	#ROWSIZEW, WCNT
		mov 	#_buffer, WBUFPTR	
		; Verify upper byte
verrow:	tblrdh.b [WADDR2], W0
		cp.b	W0, [WBUFPTR++]
		bra		NZ, vfail	
		; Verify low byte
		tblrdl.b [WADDR2++], W0
		cp.b	W0, [WBUFPTR++]
		bra		NZ, vfail
		; Verify high byte
		tblrdl.b [WADDR2++], W0
		cp.b	W0, [WBUFPTR++]
		bra		NZ, vfail
		;
		dec		WCNT, WCNT
		bra 	nz, verrow
		.endif
		bra		Main
		
		
		;----------------------------------------------------------------------
		; Erase, write & verify eeprom word
		;----------------------------------------------------------------------
		.if EESIZEB > 0
			;Erase eeprom word
eeprom:		tblwtl	WADDR, [WADDR]				;set base address of erase block
			mov 	#EE_ER_CODE, W0
			rcall 	Write	
			; Load latch
			tblwtl 	[WBUFPTR], [WADDR]
			; Write eeprom word
			mov 	#EE_WR_CODE, W0		
			rcall 	Write
			; Verify eeprom word
			.ifdef EWRITE_VER
				tblrdl	[WADDR], W0
				cp		W0, [WBUFPTR]
				bra		nz, vfail
			.endif
			bra		Main
		.endif
		
		
		;----------------------------------------------------------------------
		; Verify fail
		;----------------------------------------------------------------------
vfail:	SendL	VERFAIL
		bra		main1
		
						
		;----------------------------------------------------------------------
		; Write config word, dsPIC30F
		;----------------------------------------------------------------------		
		.ifdef CFG_TYPE1
			; Load latch
config:		tblwtl 	[WBUFPTR], [WADDR]
			; Write config word
			mov 	#CFG_WR_CODE, W0
			rcall 	Write
			; Write finished
			bra		Main
		.endif
						
		;----------------------------------------------------------------------
		; Write config byte, PIC24H & dsPIC33FJ
		;----------------------------------------------------------------------
		.ifdef CFG_TYPE2
config:		tblwtl.b WADDR, [WADDR]		;set base address of erase block
			; Erase	
			mov 	#CFG_ER_CODE, W0
			rcall 	Write
			; Load latch
			tblwtl.b [WBUFPTR], [WADDR]			
			; Write
			mov 	#CFG_WR_CODE, W0
			rcall 	Write
			; Write finished
			bra 	Main
		.endif

				
;------------------------------------------------------------------------------
; Write()
;------------------------------------------------------------------------------
Write:	mov 	W0, NVMCON
		mov 	#0x55, W0
		mov 	W0, NVMKEY
		mov 	#0xAA, W0
		mov 	W0, NVMKEY
		bset 	NVMCON, #WR
		nop
		nop	
		; Wait for erase/write to finish	
compl:	btsc	NVMCON, #WR		
		bra 	compl				
		return   
	
	
;------------------------------------------------------------------------------
; User specific exit code go here
;------------------------------------------------------------------------------
exit:
		  
		
;------------------------------------------------------------------------------
; Exit point, clean up and load user application
;------------------------------------------------------------------------------	
		mov 	#__SP_init, WSTPTR	;reinitialize the stack pointer
		rcall	CommExit		
		clr		PR1					;clear PR1, was used as temporary sfr
		
		UserExit


;------------------------------------------------------------------------------
; Load user application
;------------------------------------------------------------------------------
        bra 	usrapp


;------------------------------------------------------------------------------
; Functions
;------------------------------------------------------------------------------
		.include "uart.inc"	
		
		
;------------------------------------------------------------------------------
; Validate user settings
;------------------------------------------------------------------------------
				
		; Internal cycle clock
		.ifdef IS_24F
			.if FCY > 16000000
				.error "Fcy specified is out of range"
			.endif	
		.endif
		.ifdef IS_24FJ
			.if FCY > 16000000
				.error "Fcy specified is out of range"
			.endif	
		.endif
		.ifdef IS_24HJ
			.if FCY > 40000000
				.error "Fcy specified is out of range"
			.endif	
		.endif
		.ifdef IS_30F
			.if FCY > 30000000
				.error "Fcy specified is out of range"
			.endif	
		.endif
		.ifdef IS_33FJ
			.if FCY > 40000000
				.error "Fcy specified is out of range"
			.endif	
		.endif	
		
		; Alternative i/o
		.ifndef USE_UART1
			.ifdef USE_ALTIO
				.error "Alternative i/o is only available for UART 1
			.endif
		.endif
		.ifndef IS_30F
			.ifdef USE_ALTIO
				.error "Alternative i/o is only available for dsPIC30F
			.endif
		.endif
	
;------------------------------------------------------------------------------
; End of code
;------------------------------------------------------------------------------
		.end
