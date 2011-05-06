
.include "p30fxxxx.inc"

; MATH_ERROR 1
; STACK_ERROR 2
; ADDRESS_ERROR 4
; OSC_FAIL 8
; unhandled interrupt 16 

.extern	_trap_flags
.extern _trap_source
.extern _osc_fail_count

__MathError:	mov.w #1,w0
				bra save_states

__StackError:	mov.w #2,w0
				bra save_states

__AddressError:	mov.w #4,w0
				bra save_states

__OscillatorFail: btss OSCCON , #CF
				bra FalseAlarm
				mov.w #8,w0
				bra save_states

__DefaultInterrupt:	mov.w #16,w0
				bra	save_states

save_states:	mov.w w0,_trap_flags
				pop.d w0
				mov.w w0, _trap_source
				and #0x7f,w1
				mov.w w1, _trap_source+2
				reset

FalseAlarm:		bclr.b	INTCON1, #OSCFAIL
				inc		_osc_fail_count
				retfie

.global __MathError
.global __StackError
.global __AddressError
.global __OscillatorFail
.global __DefaultInterrupt


