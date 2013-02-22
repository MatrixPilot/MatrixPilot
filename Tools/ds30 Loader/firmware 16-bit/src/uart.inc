;------------------------------------------------------------------------------
;
; Product:			ds30 Loader
;
; File description:	UART functions for PIC24 and dsPIC
;
; Copyright: 		Copyright 2011-2012 Mikael Gustafsson
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
; Verify settings
;------------------------------------------------------------------------------	
		.ifdef USE_UART1
			.ifdef USE_UART2
				.error "Both uarts are specified"
			.endif

		   	.equ    UMODE,	    U1MODE					;uart mode
		   	.equ    USTA,  		U1STA					;uart status
		   	.equ    UBRG,		U1BRG					;uart baudrate
		   	.equ    UTXREG,		U1TXREG					;uart transmit
		   	.equ	URXREG,		U1RXREG					;uart receive
			.equ	UIFS,		IFS0					;uart interupt flag sfr
			.equ	URXIF,		U1RXIF					;uart received interupt flag
			.equ	UTXIF,		U1TXIF					;uart transmit interupt flag		   	
		.endif
		
		.ifdef USE_UART2
			.ifndef HAS_UART2
				.error "UART2 specified for a device that only has uart1"
			.endif	

		   	.equ    UMODE,	    U2MODE					;uart mode
		   	.equ    USTA,  		U2STA					;uart status
		   	.equ    UBRG,		U2BRG					;uart baudrate
		   	.equ    UTXREG,		U2TXREG					;uart transmit
		   	.equ	URXREG,		U2RXREG					;uart receive
			.equ	UIFS,		IFS1					;uart interupt flag sfr
			.equ	URXIF,		U2RXIF					;uart received interupt flag
			.equ	UTXIF,		U2TXIF					;uart transmit interupt flag		   	
		.endif	
		
		.ifdef USE_UART3
			.ifndef HAS_UART3
				.error "UART3 specified for a device that only has uart3"
			.endif	

		   	.equ    UMODE,	    U3MODE					;uart mode
		   	.equ    USTA,  		U3STA					;uart status
		   	.equ    UBRG,		U3BRG					;uart baudrate
		   	.equ    UTXREG,		U3TXREG					;uart transmit
		   	.equ	URXREG,		U3RXREG					;uart receive
			.equ	UIFS,		IFS5					;uart interupt flag sfr
			.equ	URXIF,		U3RXIF					;uart received interupt flag
			.equ	UTXIF,		U3TXIF					;uart transmit interupt flag		   	
		.endif
		
		.ifdef USE_UART4
			.ifndef HAS_UART4
				.error "UART4 specified for a device that only has uart4"
			.endif	

		   	.equ    UMODE,	    U4MODE					;uart mode
		   	.equ    USTA,  		U4STA					;uart status
		   	.equ    UBRG,		U4BRG					;uart baudrate
		   	.equ    UTXREG,		U4TXREG					;uart transmit
		   	.equ	URXREG,		U4RXREG					;uart receive
			.equ	UIFS,		IFS5					;uart interupt flag sfr
			.equ	URXIF,		U4RXIF					;uart received interupt flag
			.equ	UTXIF,		U4TXIF					;uart transmit interupt flag		   	
		.endif
		
		; Baud rate 
		.ifdef USE_BRGH
			.equ	UARTBR,		( (((FCY / BAUDRATE) / 2) - 1) / 2 )	/*brg calculation with rounding*/
		.else
			.equ	UARTBR,		( (((FCY / BAUDRATE) / 8) - 1) / 2 )	/*brg calculation with rounding*/
		.endif
		
		; Baudrate error
		.ifndef USE_ABAUD
			.ifdef USE_BRGH
				.equ REALBR,	( FCY / (4 * (UARTBR+1)) )
			.else
				.equ REALBR,	( FCY / (16 * (UARTBR+1)) )
			.endif
			.equ BAUDERR,	( (1000 * ( BAUDRATE - REALBR)) / BAUDRATE )
			.if ( (BAUDERR > 25) || (BAUDERR < -25) )				
				.error "Baudrate error is more than 2.5%. Remove this check or try another baudrate and/or clockspeed."
			.endif 
		.endif
		
		;
		.ifdef USE_TXENABLE
			.equ    TXE_DELAY_CNT,	( TXE_DELAY * FCY / 1000000 / 3 )
		.endif
		
					
;------------------------------------------------------------------------------
; CommInit()
;------------------------------------------------------------------------------		
CommInit:		
		.ifdef USE_BRGH	
			bset	UMODE, #BRGH
		.endif		
		.ifdef 	USE_ALTIO
			bset	U1MODE, #ALTIO
		.endif		
		
		;----------------------------------------------------------------------
		;UART without auto baudrate detection
		;----------------------------------------------------------------------
		.ifndef USE_ABAUD
			mov		#UARTBR, W0 		;set	
			mov 	W0, UBRG			; baudrate
			bset 	UMODE, #UARTEN		;enable uart
			bset 	USTA, #UTXEN		;enable transmit
			
			
		;----------------------------------------------------------------------
		;UART with auto baudrate detection
		;----------------------------------------------------------------------
		.else
			; PIC24F, PIC24FJ, PIC24H & dsPIC330FJ
			.ifndef IS_30F
				setm	UBRG			;errata workaround
				bset	UMODE, #ABAUD	;enable auto baudrate
				bset 	UMODE, #UARTEN	;enable uart			
				
				;------------------------------------------------------------------
				; Wait for auto baudrate to complete	
				;------------------------------------------------------------------
				; Init delay
				mov 	#BLSTART, WDEL1
				; Check for auto baud complete
arpt1:			clr		WDEL2
arptc:			clrwdt
				btss 	UMODE, #ABAUD
				bra		abaudok
		 		; Delay
anotrcv:		dec 	WDEL2, WDEL2
				bra 	nz, arptc
				dec 	WDEL1, WDEL1
				bra 	nz, arpt1
				; If we get here, abaud timed out	
				bra		exit	
				; Auto baud rate detection completed
abaudok:		bset 	USTA, #UTXEN		;enable transmit			
				SendL	OK					;confirm baudrate detection to application
			
			; dsPIC30F
			.else
				bset 	UMODE, #UARTEN		;enable UART
				bset 	U1MODE, #ABAUD      ;enable auto baudrate detection			
				; Setp timer 3 
		        clr 	T3CON             	;
		        bclr 	IEC0, #T3IE         ;disable timer 3 interrupt
		        setm 	PR3                 ;maximum period time
		        mov 	#0x8000, W0         ;start, 1:1 prescaler, internal cycle as clock source
		        mov 	W0, T3CON	
		        ; Setp input capture
				mov		#3, W0  	        ;capture mode, every rising edge
		        mov 	W0, IC1CON        	;
		        bclr 	IFS0, #IC1IF        ;clear input capture interrupt flag	
		        ; Begin autobaud detection
				mov 	#0x0004, W0			;detected 4 edges after first 4 edge, total 5 for 0x55
		        rcall 	WaitIC          	;wait for first rising edge
		        clr 	TMR3                ;reset timer 3
edgeloop:   	rcall 	WaitIC
		        dec 	W0, W0 
		        bra 	nz, edgeloop
		        ; Do calculation
		        mov 	TMR3, W0
		        add 	#0x40, W0           ;add 0.5(64/128) => rounding
		        asr 	W0, #7, W0          ;W0 = (Tend - Tstart/ 128) + 0.5
		        dec 	W0, W0			
				; Detection comleted, clean up
				bclr 	T3CON, #TON         ;disable timer 3
		        bclr 	IFS0, #IC1IF        ;clear input capture interrupt flag	        
		        ; Init uart
		        mov 	W0, U1BRG			;use calculated brg
		        bclr 	UMODE, #ABAUD      	;disable auto baudrate detection
		        bclr 	UMODE, #UARTEN		;disable uart
		        clr		USTA				;clear uart status
		        bset 	UMODE, #UARTEN		;enable uart
		        bset 	USTA, #UTXEN       	;enable tx			
				;confirm baudrate detection to application
				SendL	OK					
			.endif						
		.endif
		
		
		;----------------------------------------------------------------------
		; TX enable, make tx enable pin output and set to 0
		;----------------------------------------------------------------------
		.ifdef USE_TXENABLE
			bclr	TRISR_TXE, #TRISB_TXE
			bclr	LATR_TXE, #LATB_TXE
		.endif
		
		return
		
				
;------------------------------------------------------------------------------
; CommExit()
;------------------------------------------------------------------------------
CommExit:	
		bclr	UIFS, #URXIF	;clear uart received interupt flag
		bclr	UIFS, #UTXIF	;clear uart transmit interupt flag	
		clr 	UMODE			;disable uart
		clr		USTA			;disable uart transmit
		return
		
				
;------------------------------------------------------------------------------
; Receive()
; Returns: received byte in W0
;------------------------------------------------------------------------------
		; Init delay
Receive:mov 	#BLDELAY, WDEL1
		; Check for received byte
ReceiveInit:
rpt1:	clr		WDEL2
rptc:		
		.ifdef KICK_WD
			clrwdt					;clear watchdog
		.endif
		btss 	USTA, #URXDA		
		bra 	notrcv
		mov 	URXREG, W0			
		add 	WCRC, W0, WCRC		;add to checksum
		return
 		; Delay
notrcv:	dec 	WDEL2, WDEL2
		bra 	nz, rptc
		dec 	WDEL1, WDEL1
		bra 	nz, rpt1
		; If we get here, uart receive timed out
        bra		exit
	
		
;------------------------------------------------------------------------------
; Send()
; Arguments: W0 - byte to send
;------------------------------------------------------------------------------
Send:	; Enable tx
		.ifdef USE_TXENABLE
			bset	LATR_TXE, #LATB_TXE
            mov 	#TXE_DELAY_CNT, WDEL2
txe_del_lo:	dec 	WDEL2, WDEL2
            bra 	nz, txe_del_lo
        .else
			; Wait until transmit fifo is not full
txwait:		btsc	USTA, #UTXBF
			bra		txwait            
		.endif		
		;Send byte
		mov 	WREG, UTXREG
		; Disable tx 
		.ifdef USE_TXENABLE
			; Wait until transmit shift register is empty
			nop
txwait:		btss	USTA, #TRMT
			bra		txwait
			bclr	LATR_TXE, #LATB_TXE
		.endif
		; Send complete
		return

			
;------------------------------------------------------------------------------
; WaitIC()
;------------------------------------------------------------------------------
.ifdef IS_30F
.ifdef USE_ABAUD
WaitIC: mov 	#BLDELAY, WDEL1
wicol: 	clr 	WDEL2
wicil: 	clrwdt					;clear watchdog
		btsc 	IFS0, #IC1IF	;rising edge detected?
        bra 	EdgeDet
        dec 	WDEL2, WDEL2
        nop						;nop so we can use same delay as comm.
        nop
        bra 	nz, wicil
        dec 	WDEL1, WDEL1
        bra 	nz, wicol
        ; Timeou
        goto 	usrapp		        
EdgeDet:bclr 	IFS0, #IC1IF    ;clear input capture interrupt flag
        return
.endif
.endif
			

;------------------------------------------------------------------------------
; End of file
;------------------------------------------------------------------------------
