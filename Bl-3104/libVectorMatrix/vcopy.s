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
	; none...
	.list

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.section .libdsp, code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _VectorCopy: initialization to zero of FIR filter delay.
;
; Operation:
;	dstV[n] = srcV[n], 0 <= n < numElems
;
; Input:
;	w0 = number elements to copy from source vector (numElems)
;	w1 = ptr to destination vector (dstV)
;	w2 = ptr to source vector (srcV)
; Return:
;	w0 = ptr to destination vector (dstV)
;
; System resources usage:
;	{w0..w3}	used, not restored
;
; DO and REPEAT instruction usage.
;	no DO instructions
;	1 level REPEAT intruction
;
; Program words (24-bit instructions):
;	6
;
; Cycles (including C-function call and return overheads):
;	12 + numElems
;............................................................................

	.global	_VectorCopy	; export
_VectorCopy:

	push	RCOUNT

;............................................................................

	mov	w1,w3				; save return value (dstV)

	dec	w0,w0				; w0 = numElems-1
	repeat	w0				; do (numElems-1)+1 times
	mov	[w2++],[w1++]			; dstV[n] = srcV[n]
						; w1-> dstV[n+1]
						; w2-> srcV[n+1]

	mov	w3,w0				; restore return value

;............................................................................

	pop		RCOUNT

	return	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF
