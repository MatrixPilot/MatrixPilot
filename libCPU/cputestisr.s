/*****************************************************************************
 *
 * Software License Agreement
 *
 * Copyright © 2012 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 *****************************************************************************/
;======================================================================
;   Interrupt Serivce Routines(Assembly):
;   This file defines the Math Error trap handler and INT0 interrupt 
;   handler, which are used by the CPU Self test algorithm.
;
;   Note: Only Alternate Interrupt Vectors are used in the case of dsPIC33F and PIC24H.
;======================================================================

    .ifdef __dsPIC33F
        .include "p33Fxxxx.inc"        ; Include file for dsPIC33F device
    .endif
    .ifdef __PIC24H
        .include "p24Hxxxx.inc"        ; Include file for PIC24H device
    .endif
    .ifdef __dsPIC33E
        .include "p33Exxxx.inc"        ; Include file for dsPIC33E device
    .endif
    .ifdef __PIC24E
        .include "p24Exxxx.inc"        ; Include file for PIC24E device
    .endif
	
	.ifdef __dsPIC33F
	.global __AltINT0Interrupt
        .global __AltMathError	
	.endif
	.ifdef __PIC24H
        .global __AltINT0Interrupt
        .global __AltMathError	
	.endif
	.ifdef __dsPIC33E
        .global __INT0Interrupt
        .global __MathError	
	.endif
	.ifdef __PIC24E
        .global __INT0Interrupt
        .global __MathError	
	.endif
   
;..............................................................................
;Code Section in Program Memory
;..............................................................................
        .text                     ; Start of Code section
        .org    0x050           ; place this ISR starting from the offset 0x100 from the code base location

;..............................................................................
; ISR for INT0 interrupt: for TestSubset 3
;..............................................................................

.ifdef __dsPIC33F
__AltINT0Interrupt:               
.endif
.ifdef __PIC24H
__AltINT0Interrupt:               
.endif
.ifdef __dsPIC33E
__INT0Interrupt:               	
.endif
.ifdef __PIC24E
__INT0Interrupt:               
.endif

        add     #CpuTest3Data18, w0
        bclr    IFS0, #0          ; If DISI fails and ISR is serviced
                                  ; then to avoid infinite loop in ISR
                                  ; interrupt flag will be cleared
                                  ; but result will be added by 0xFF
        retfie
                                  ; Of course this ISR will not be serviced
                                  ; if DISI works properly

;..............................................................................
; ISR for Math Error trap: for TestSubsets 6 & 7
;..............................................................................

.ifdef __dsPIC33F
__AltMathError:               
.endif
.ifdef __PIC24H
__AltMathError:               
.endif
.ifdef __dsPIC33E
__MathError:               	
.endif
.ifdef __PIC24E
__MathError:               
.endif

	btsc    INTCON1, #DIV0ERR ; Check the trap is caused by divide by zero
                                  ; If DIV0ERR=1 then branch to
                                  ; TestSubset_6MathISR to execute TestSubset_6
                                  ; specific code else execute TestSubset_7
                                  ; specific code
        bra     TestSubset_6MathISR

TestSubset_7MathISR:              ; The following code is for TestSubset_7:
        add #1,w0
        bclr INTCON1, #MATHERR
        retfie

                                  ; The following code is for TestSubset_6:

;..............................................................................
;1. Tested SR flags IPL<2:0>, RA and CORCON flag IPL3.
;2. The known SR<7:4> bits are moved to w0.
;3. Multiply the default working register WREG with the register RCOUNT and
;   store the result to w2. An 8-bit multiply operation is performed here.
;4. This multiplication will validate the RCOUNT register value.
;5. In order to test the context save on to the stack during interrupt,
;   the address pointer is moved to w3.
;6. With the help of w3, read the PCL, PCH, SR<7:0> and IPL3 bits, save
;   them on the stack and add to w5.
;7. As this is a math error trap, the IPL3 bit will be set. This bit is tested 
;   with the BTSC instruction. If it is cleared then it will effect w5 which, 
;   in turn, effects the final result.
;8. The Math Error Trap status flag in the INTCON1 is cleared.
;9. The RCOUNT register is cleared to stop the division instruction which 
;   caused divide-by-zero error.
;..............................................................................
TestSubset_6MathISR:
        mov     #CpuTest6Data5, w0
        and     SR, WREG          ; IPL<2:0> and RA bits are catured to w0
                                  ; Integer Unsigned Multiply f and WREG
        mul.b   RCOUNT            ; validating RCOUNT and mul.b by f
        mov     w15, w3           ; mov stack address pointer to w3
                                  ; SR<7:0>, IPL3 and PCH are read from stack
        add     w5, [--w3], w5    ; and added to w5
        add     w5, [--w3], w5    ; PCL is read from stack and added to w5
        btsc    CORCON, #IPL3
        add     w5, w2, w5        ; executed as IPL3=1
        bclr    INTCON1, #MATHERR ; clear the matherror flag
        clr     RCOUNT            ; clear the RCOUNT register
        retfie

