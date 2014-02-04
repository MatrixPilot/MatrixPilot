;*********************************************************************
;                                                                    *
;                       Software License Agreement                   *
;                                                                    *
;   The software supplied herewith by Microchip Technology           *
;   Incorporated (the "Company") for its dsPIC controller            *
;   is intended and supplied to you, the Company's customer,         *
;   for use solely and exclusively on Microchip dsPIC                *
;   products. The software is owned by the Company and/or its        *
;   supplier, and is protected under applicable copyright laws. All  *
;   rights are reserved. Any use in violation of the foregoing       *
;   restrictions may subject the user to criminal sanctions under    *
;   applicable laws, as well as to civil liability for the breach of *
;   the terms and conditions of this license.                        *
;                                                                    *
;   THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           *
;   WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    *
;   BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    *
;   FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     *
;   COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  *
;   INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  *
;                                                                    *
;   (c) Copyright 2003 Microchip Technology, All rights reserved.    *
;*********************************************************************

	; Local inclusions.
	.nolist
	.include	"dspcommon.inc"		; fractsetup
	.list

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.section .libdsp, code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _VectorPower: Vector Power.
;
; Operation:
;	powVal = sum (srcV[n] * srcV[n]), with
;
; n in {0, 1,... , numElems-1}
;
; Input:
;	w0 = number elements in vector(s) (numElems)
;	w1 = ptr to source vector (srcV)
; Return:
;	w0 = power value (powVal)
;
; System resources usage:
;	{w0..w2}	used, not restored
;	{w4}		used, not restored
;	 AccuA		saved, used, restored
;	 CORCON		saved, used, restored
;
; DO and REPEAT instruction usage.
;	1 level DO intructions
;	no REPEAT instruction
;
; Program words (24-bit instructions):
;	12
;
; Cycles (including C-function call and return overheads):
;	16 + 2*numElems
;............................................................................

	.global	_VectorPower	; export
_VectorPower:

;	save the 40 bit A accumulator
	push	ACCAL
	push	ACCAH
	push	ACCAU

;............................................................................

	; Save working registers.
	; None to save...

;............................................................................

	; Prepare CORCON for fractional computation.
	push	CORCON
	fractsetup	w2

;............................................................................

	; Prepare operation.
	dec	w0,w0				; w0 = numElems-1
	clr	a				; a = 0

;	save the do loop registers
	push	DCOUNT
	push	DOSTARTL
	push	DOSTARTH
	push	DOENDL
	push	DOENDH

	; Perform operation.
	do	w0,_endPow		; {	; do (numElems-1)+1 times
	mov	[w1++],w4			; w4 = srcV[n]
						; w1-> srcV[n+1]
_endPow:
	mac	w4*w4,a				; a += srcV[n]*srcV[n]

;	restore the do loop registers
	pop		DOENDH
	pop		DOENDL
	pop		DOSTARTH
	pop		DOSTARTL
	pop		DCOUNT

; }

;............................................................................

	sac.r	a,w0				; powVal return value

;............................................................................

	; restore CORCON.
	pop	CORCON

;............................................................................

	; Restore working registers.
	; None to restore...

;............................................................................

;	restore the 40 bit A accumulator
	pop		ACCAU
	pop		ACCAH
	pop		ACCAL

	return	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF
