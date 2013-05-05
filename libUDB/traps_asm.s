
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

/*
.extern	_trap_flags
.extern _trap_source
.extern _osc_fail_count

__MathError:	mov.w #1,w0
				bra save_states

__StackError:	mov.w #2,w0
				bra save_states

__AddressError:	mov.w #4,w0
				bra save_states

__OscillatorFail:inc _osc_fail_count
				mov.w #8,w0
				bra save_states

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
 */


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

/*
.global _StackPtr
.section .bss
_StackPtr: .space 2
 */

.global _SP_start
.global _SP_limit
.global _SP_current
.global _sleep
.global _idle

.section .text

_sleep:
		pwrsav  #SLEEP_MODE     ; Put the device into SLEEP mode
        return

_idle:
		pwrsav  #IDLE_MODE      ; Put the device into IDLE mode
        return

_SP_start:
		mov #__SP_init, w0
        return

_SP_limit:
		mov SPLIM, w0
        return

_SP_current:
		mov w15, w0
        return

.global _getErrLoc
.section .text

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

/*
.global __AddressError
.global _AddrErrInstructionAddrLow, _AddrErrInstructionAddrHigh

.section .bss
_AddrErrInstructionAddrLow: .space 2
_AddrErrInstructionAddrHigh: .space 2

.section .text

__AddressError:
        mov     #_AddrErrInstructionAddrHigh, w1
        pop     [w1--]                  ;Pop the Program Counter (PC) from the stack
        pop     [w1++]                  ;Remember that the PC(PCH and PCL) is stacked
        bclr    [w1], #7                ;along with the SRL byte and IPL3 bit from
        inc     w1, w1                  ;CORCON. So we need to extract just the 24-bit
        clr.b   [w1]                    ;PCH:PCL information from the stack
        mov     #_AddrErrInstructionAddrLow, w1  ;Decrement that value by 2
        mov     #2, w2                  ;Store the 24-bit result into 2x16-bit words
        subr    w2, [w1], [w1++]        ;in RAM at- AddrErrInstructionAddrHigh:Low
        clr     w2
        subbr   w2, [w1], [w1]
        bclr    INTCON1, #ADDRERR       ;Clear the trap flag
StayTrappedAddrErr:                     ;Stay in this routine
        bra     StayTrappedAddrErr
        ;Place a breakpoint above and halt the Debugging process and view
        ;the variables named: AddrErrInstructionAddrHigh:Low
        retfie
 */

.global _gentrap

.section .text
_gentrap:
        mov     #0xFFFF, w0     ;Load an unimplemented address into w0
        mov     #0xFFFF, w0     ;Load an unimplemented address into w1

trap_causing_mov_instruction:
        mov     [w0], [w1]      ;Perform an illegal mov instruction that
                                ;the tools will not be able to detect at
                                ;compile-time.
                                ;This mov instruction tries to fetch and
                                ;store values from and to unimplemented addresses
                                ;Also, it performs a misaligned access.
done:   bra     done            ;Code execution never reaches here.


