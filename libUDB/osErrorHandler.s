
.ifdef __dsPIC33F
.include "p33Fxxxx.inc"
.endif
.ifdef __dsPIC33E
.include "p33Exxxx.inc"
.endif

 
.section .nbss, bss, near
 
.global __TrapSrc, __dsr_pag, __dsw_pag, __tbl_pag
__TrapSrc:               .space 2
__dsr_pag:               .space 2
__dsw_pag:               .space 2
__tbl_pag:               .space 2

.global _wRegSave, _TrapAddr, _ErrorAddrHigh, _ErrorAddrLow
_wRegSave:               .space 32
_TrapAddr:               .space 2
_ErrorAddrHigh:          .space 2
_ErrorAddrLow:           .space 2

.section .const, psv

.global __lastDataAddr
__lastDataAddr:
	 
.section .osMain, code

.global __ReservedTrap0 
.weak __ReservedTrap0
__ReservedTrap0:
	rcall __TrapError

.global __OscillatorFail 
.weak __OscillatorFail
__OscillatorFail:
	rcall __TrapError

.global __AddressError 
.weak __AddressError
__AddressError:
	rcall __TrapError

.global __GenericHardError
.weak __GenericHardError
__GenericHardError:
	rcall __TrapError

.global __StackError 
.weak __StackError
__StackError:
	rcall __TrapError

.global __MathError 
.weak __MathError
__MathError:
	rcall __TrapError

.global __DMACError
.weak __DMACError
__DMACError:
	rcall __TrapError

.global __GenericSoftError
.weak __GenericSoftError
__GenericSoftError:
	rcall __TrapError

.global __ReservedTrap5 
.weak __ReservedTrap5
__ReservedTrap5:
	rcall __TrapError
/*
.global __ReservedTrap6 
.weak __ReservedTrap6
__ReservedTrap6:
	rcall __TrapError     .global __ReservedTrap7 
.weak __ReservedTrap7
__ReservedTrap7:
	rcall __TrapError     .global __INT0Interrupt 
.weak __INT0Interrupt
__INT0Interrupt:
	rcall __TrapError     .global __IC1Interrupt 
.weak __IC1Interrupt
__IC1Interrupt:
	rcall __TrapError     .global __OC1Interrupt 
.weak __OC1Interrupt
__OC1Interrupt:
	rcall __TrapError     .global __T1Interrupt 
.weak __T1Interrupt
__T1Interrupt:
	rcall __TrapError     .global __IC2Interrupt 
.weak __IC2Interrupt
__IC2Interrupt:
	rcall __TrapError     .global __OC2Interrupt 
.weak __OC2Interrupt
__OC2Interrupt:
	rcall __TrapError     .global __T2Interrupt 
.weak __T2Interrupt
__T2Interrupt:
	rcall __TrapError     .global __T3Interrupt 
.weak __T3Interrupt
__T3Interrupt:
	rcall __TrapError     .global __SPI1Interrupt 
.weak __SPI1Interrupt
__SPI1Interrupt:
	rcall __TrapError     .global __U1RXInterrupt 
.weak __U1RXInterrupt
__U1RXInterrupt:
	rcall __TrapError     .global __U1TXInterrupt 
.weak __U1TXInterrupt
__U1TXInterrupt:
	rcall __TrapError     .global __ADCInterrupt 
.weak __ADCInterrupt
__ADCInterrupt:
	rcall __TrapError     .global __NVMInterrupt 
.weak __NVMInterrupt
__NVMInterrupt:
	rcall __TrapError     .global __SI2CInterrupt 
.weak __SI2CInterrupt
__SI2CInterrupt:
	rcall __TrapError     .global __MI2CInterrupt 
.weak __MI2CInterrupt
__MI2CInterrupt:
	rcall __TrapError     .global __CNInterrupt 
.weak __CNInterrupt
__CNInterrupt:
	rcall __TrapError     .global __INT1Interrupt 
.weak __INT1Interrupt
__INT1Interrupt:
	rcall __TrapError     .global __IC7Interrupt 
.weak __IC7Interrupt
__IC7Interrupt:
	rcall __TrapError     .global __IC8Interrupt 
.weak __IC8Interrupt
__IC8Interrupt:
	rcall __TrapError     .global __OC3Interrupt 
.weak __OC3Interrupt
__OC3Interrupt:
	rcall __TrapError     .global __OC4Interrupt 
.weak __OC4Interrupt
__OC4Interrupt:
	rcall __TrapError     .global __T4Interrupt 
.weak __T4Interrupt
__T4Interrupt:
	rcall __TrapError     .global __T5Interrupt 
.weak __T5Interrupt
__T5Interrupt:
	rcall __TrapError     .global __INT2Interrupt 
.weak __INT2Interrupt
__INT2Interrupt:
	rcall __TrapError     .global __U2RXInterrupt 
.weak __U2RXInterrupt
__U2RXInterrupt:
	rcall __TrapError     .global __U2TXInterrupt 
.weak __U2TXInterrupt
__U2TXInterrupt:
	rcall __TrapError     .global __SPI2Interrupt 
.weak __SPI2Interrupt
__SPI2Interrupt:
	rcall __TrapError     .global __C1Interrupt 
.weak __C1Interrupt
__C1Interrupt:
	rcall __TrapError     .global __IC3Interrupt 
.weak __IC3Interrupt
__IC3Interrupt:
	rcall __TrapError     .global __IC4Interrupt 
.weak __IC4Interrupt
__IC4Interrupt:
	rcall __TrapError     .global __IC5Interrupt 
.weak __IC5Interrupt
__IC5Interrupt:
	rcall __TrapError     .global __IC6Interrupt 
.weak __IC6Interrupt
__IC6Interrupt:
	rcall __TrapError     .global __OC5Interrupt 
.weak __OC5Interrupt
__OC5Interrupt:
	rcall __TrapError     .global __OC6Interrupt 
.weak __OC6Interrupt
__OC6Interrupt:
	rcall __TrapError     .global __OC7Interrupt 
.weak __OC7Interrupt
__OC7Interrupt:
	rcall __TrapError     .global __OC8Interrupt 
.weak __OC8Interrupt
__OC8Interrupt:
	rcall __TrapError     .global __INT3Interrupt 
.weak __INT3Interrupt
__INT3Interrupt:
	rcall __TrapError     .global __INT4Interrupt 
.weak __INT4Interrupt
__INT4Interrupt:
	rcall __TrapError     .global __C2Interrupt 
.weak __C2Interrupt
__C2Interrupt:
	rcall __TrapError     .global __PWMInterrupt 
.weak __PWMInterrupt
__PWMInterrupt:
	rcall __TrapError     .global __QEIInterrupt 
.weak __QEIInterrupt
__QEIInterrupt:
	rcall __TrapError     .global __DCIInterrupt 
.weak __DCIInterrupt
__DCIInterrupt:
	rcall __TrapError     .global __LVDInterrupt 
.weak __LVDInterrupt
__LVDInterrupt:
	rcall __TrapError     .global __FLTAInterrupt 
.weak __FLTAInterrupt
__FLTAInterrupt:
	rcall __TrapError     .global __FLTBInterrupt 
.weak __FLTBInterrupt
__FLTBInterrupt:
	rcall __TrapError     .global __Interrupt45 
.weak __Interrupt45
__Interrupt45:
	rcall __TrapError     .global __Interrupt46 
.weak __Interrupt46
__Interrupt46:
	rcall __TrapError     .global __Interrupt47 
.weak __Interrupt47
__Interrupt47:
	rcall __TrapError     .global __Interrupt48 
.weak __Interrupt48
__Interrupt48:
	rcall __TrapError     .global __Interrupt49 
.weak __Interrupt49
__Interrupt49:
	rcall __TrapError     .global __Interrupt50 
.weak __Interrupt50
__Interrupt50:
	rcall __TrapError     .global __Interrupt51 
.weak __Interrupt51
__Interrupt51:
	rcall __TrapError     .global __Interrupt52 
.weak __Interrupt52
__Interrupt52:
	rcall __TrapError     .global __Interrupt53 
.weak __Interrupt53
__Interrupt53:
	rcall __TrapError
 */	 

.section .text

.global __TrapError 
.weak __TrapError
__TrapError:
	mov     w0, _wRegSave
	bset    RCON, #TRAPR

	mov     DSRPAG, w0
	mov     w0, __dsr_pag
	mov     DSWPAG, w0
	mov     w0, __dsw_pag
	mov     TBLPAG, w0
	mov     w0, __tbl_pag

	mov     #_wRegSave + 2, w0
	mov     w1,  [w0++]
	mov.d   w2,  [w0++]
	mov.d   w4,  [w0++]
	mov.d   w6,  [w0++]
	mov.d   w8,  [w0++]
	mov.d   w10, [w0++]
	mov.d   w12, [w0++]
	mov     w14, [w0++]
	mov     #8,  w1         ; Remove 4 words (Trap Addr and Trap Error call)
	sub.w   w15, w1, [w0++] ; from the reported stack pointer
	pop     w3              ; Get Trap Addr High
	pop     w2              ; Get Trap Addr Low
	mov     #tbloffset(__ReservedTrap0) + 2, w1
	sub     w2, w1, [w0++]  ; compute original trap addr
	pop     w2              ; Get Error Addr High
	and     w2, #7, [w0++]  ;  just the address
	pop     [w0++]          ; Get Error Addr Low
;	call    __psv_init      ; Insure that PSV is set correctly
	call    _osError_Report ; call our C routine to report the problem
 
.global __Trapped
/*
 * In case OsError_Report returns -- just loop
 */
__Trapped:
	bra     __Trapped
 
.global _Address_Error
/*
 * Test this code by causing a Address Error
 */
_Address_Error:
	mov     #1, w0
	mov     [w0], w1
	return;
