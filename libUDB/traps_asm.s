
.ifdef __dsPIC33F
.include "p33Fxxxx.inc"
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

__MathError:	mov.w #1,w0
				bra save_states

__StackError:	mov.w #2,w0
				bra save_states

__AddressError:	mov.w #4,w0
				bra save_states

;TODO TomP - This was removed during the IP stack merge.
;__OscillatorFail:inc _osc_fail_count
;				mov.w #8,w0
;				bra save_states

__DMACError:	mov.w #32,w0
				bra save_states

__DefaultInterrupt:	mov.w #16,w0
				bra	save_states

save_states:	mov.w w0,_trap_flags
				pop.d w0
				mov.w w0, _trap_source
				and #0x7f,w1
				mov.w w1, _trap_source+2
				reset

.global __MathError
.global __StackError
.global __AddressError
.global __OscillatorFail
.global __DefaultInterrupt
.global __DMACError


.global _getErrLoc

; Stack Growth from Trap Error
;1. PC[15:0]            <--- Trap Address
;2. SR[7:0]:IPL3:PC[22:16]
;3. RCOUNT
;4. W0
;5. W1
;6. W2
;7. W3
;8. W4
;9. W5
;10. W6
;11. W7
;12. OLD FRAME POINTER [W14]
;13. PC[15:0]           <---- W14 
;14. 0:PC[22:16]
;15.                    <---- W15

_getErrLoc:
        mov    w14,w2
        sub    w2,#24,w2
        mov    [w2++],w0
        mov    [w2++],w1 
        mov    #0x7f,w3     ; Mask off non-address bits
        and    w1,w3,w1

        mov    #2,w2        ; Decrement the address by 2
        sub    w0,w2,w0
        clr    w2
        subb   w1,w2,w1
        return
     
