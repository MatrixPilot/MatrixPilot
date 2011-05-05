
.include "p33fxxxx.inc"

; MATH_ERROR 1
; STACK_ERROR 2
; ADDRESS_ERROR 4
; OSC_FAIL 8

.extern	_trap_flags
.extern _trap_source
.extern _osc_fail_count

__MathError:	mov.w _trap_flags,w0
				bset w0,#0
				bra saveTraps

__StackError:	mov.w _trap_flags,w0
				bset w0,#1
				bra saveTraps

__AddressError:	mov.w _trap_flags,w0
				bset w0,#2
				bra saveTraps

__OscillatorFail:inc _osc_fail_count
				mov.w _trap_flags,w0
				bset w0,#3
				bra saveTraps  ; this really is not required, but it will prevent trouble 
								; if more trap handlers are added.

saveTraps:		mov.w w0,_trap_flags
				pop.d w0
				mov.w w0, _trap_source
				and #0x7f,w1
				mov.w w1, _trap_source+2
				reset


.global __MathError
.global __StackError
.global __AddressError
.global __OscillatorFail


