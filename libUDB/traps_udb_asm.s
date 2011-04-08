
.include "p30fxxxx.inc"

; MATH_ERROR 1
; STACK_ERROR 2
; ADDRESS_ERROR 4
; OSC_FAIL 8

.extern	_trap_flags
.extern _trap_source
.extern _osc_fail_count

__MathError:	mov.w #1,w0
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset

__StackError:	mov.w #2,w0
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset

__AddressError:	mov.w #4,w0
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset


__OscillatorFail: 
				btss OSCCON , #CF
				bra FalseAlarm
				mov.w #8,w0
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset

FalseAlarm:		bclr.b	INTCON1, #OSCFAIL
				inc		_osc_fail_count
				retfie

.global __MathError
.global __StackError
.global __AddressError
.global __OscillatorFail


