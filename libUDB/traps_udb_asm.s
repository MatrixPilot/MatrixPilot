
.ifdef __dsPIC33F
.include "p33Fxxxx.inc"
.endif
.ifdef __dsPIC30F
.include "p30fxxxx.inc"
.endif
.ifdef __dsPIC33E
.include "p33Exxxx.inc"
.endif

; MATH_ERROR 1
; STACK_ERROR 2
; ADDRESS_ERROR 4
; OSC_FAIL 8
; unhandled interrupt 16
; DMACError 32

.extern	_trap_flags
.extern _trap_source
.extern _osc_fail_count

.ifndef __dsPIC33E
__MathError:	mov.w #1,w0
				bra save_states
.else
.endif

__StackError:	mov.w #2,w0
				bra save_states

__AddressError:	mov.w #4,w0
				bra save_states

__OscillatorFail:
.ifdef __dsPIC30F
				btss OSCCON , #CF
				bra FalseAlarm
.else
				inc _osc_fail_count
.endif
				mov.w #8,w0
				bra save_states

.ifndef __dsPIC30F
__DMACError:	mov.w #32,w0
				bra save_states
.endif

__DefaultInterrupt:	mov.w #16,w0
				bra	save_states

save_states:	mov.w w0,_trap_flags
				pop.d w0
				mov.w w0, _trap_source
				and #0x7f,w1
				mov.w w1, _trap_source+2
				reset

.ifdef __dsPIC30F
FalseAlarm:		bclr.b	INTCON1, #OSCFAIL
				inc		_osc_fail_count
				retfie
.endif

.global __MathError
.global __StackError
.global __AddressError
.global __OscillatorFail
.global __DefaultInterrupt
.ifndef __dsPIC30F
.global __DMACError
.endif


