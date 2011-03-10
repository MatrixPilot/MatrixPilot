


; MATH_ERROR 1
; STACK_ERROR 2
; ADDRESS_ERROR 4
; OSC_FAIL 8

.extern	_trap_flags
.extern _trap_source

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


__OscillatorFail: mov.w #8,w0
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset

.global __MathError
.global __StackError
.global __AddressError
.global __OscillatorFail


