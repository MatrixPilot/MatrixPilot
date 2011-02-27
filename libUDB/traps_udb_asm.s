


; MATH_ERROR 1
; STACK_ERROR 2
; ADDRESS_ERROR 4
; OSC_FAIL 8

.extern	_trap_flags
.extern _trap_source

__MathError:	mov.w _trap_flags,w0
				bset w0,#0
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset

__StackError:	mov.w _trap_flags,w0
				bset w0,#1
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset

__AddressError:	mov.w _trap_flags,w0
				bset w0,#2
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset


__OscillatorFail:	mov.w _trap_flags,w0
				bset w0,#3
				mov.w w0,_trap_flags
				pop	w0
				pop w0
				mov.w w0,_trap_source
				reset

.global __MathError
.global __StackError
.global __AddressError
.global __OscillatorFail


