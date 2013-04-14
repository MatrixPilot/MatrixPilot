/**********************************************************************
; *
; *	 Big Integer Assembly Helpers
; *  Library for Microchip TCP/IP Stack
; *	  - Accelerates processing for BigInt functions
; *
; *********************************************************************
; * FileName:        BigInt_helper.S
; * Dependencies:    None
; * Processor:       PIC24F, PIC24H, dsPIC30F, dsPIC33F
; * Compiler:        Microchip C30 v3.12 or higher
; * Company:         Microchip Technology, Inc.
; *
; * Software License Agreement
; *
; * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
; * reserved.
; *
; * Microchip licenses to you the right to use, modify, copy, and
; * distribute:
; * (i)  the Software when embedded on a Microchip microcontroller or
; *      digital signal controller product ("Device") which is
; *      integrated into Licensee's product; or
; * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
; *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
; *		used in conjunction with a Microchip ethernet controller for
; *		the sole purpose of interfacing with the ethernet controller.
; *
; * You should refer to the license agreement accompanying this
; * Software for additional information regarding your rights and
; * obligations.
; *
; * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
; * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
; * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
; * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
; * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
; * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
; * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
; * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
; * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
; * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
; * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
; *
; *
; * Author               Date		Comment
; *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; * Elliott Wood	     6/20/07	Original
; * Howard Schlunder	 9/12/07	Ported to 16-bit PICs
; ********************************************************************/
.equ VALID_ID,0
.ifdecl __dsPIC30F
	.include "p30fxxxx.inc"
.endif
.ifdecl __dsPIC33F
	.include "p33fxxxx.inc"
.endif
.ifdecl __PIC24H
	.include "p24hxxxx.inc"
.endif
.ifdecl __PIC24F
	.include "p24fxxxx.inc"
.endif
.ifdecl __dsPIC33E
	.include "p33Exxxx.inc"
.endif
.ifdecl __PIC24E
	.include "p24Exxxx.inc"
.endif
.if VALID_ID <> 1
	//.error "Processor ID not specified in generic include files.  New ASM30 assembler needs to be downloaded?"
	.equ C,0
.endif

//#define __GENERIC_TYPE_DEFS_H_
//#define __COMPILER_H
//#include "HardwareProfile.h"
//#include "TCPIPConfig.h"
//#if (defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)) && !defined(ENC100_INTERFACE_MODE)


.section BigInt_helper_vars,bss,near
.align	2
.global	__iA
.global __xA
.global __iB
.global __xB
.global __iR
.global __wC
__iA:	.space 2	;*_iA, starting index for A (lower memory address, least significant byte/word)
__xA:	.space 2	;*_xA, end index for A (higher memory address, most significant byte/word)
__iB:	.space 2	;*_iB, starting index for B (lower memory address, least significant byte/word)
__xB:	.space 2	;*_xB, end index for B (higher memory address, most significant byte/word)
__iR:	.space 2	;*_iR, starting index for Res (lower memory address, least significant byte/word)
__wC:	.space 2	; value of B for _mas (little endian word)


.text

;***************************************************************************
; Function: 	void _addBI()
;
; PreCondition: _iA and _iB are loaded with the address of the LSB of the BigInt
;				_xA and _xB are loaded with the address of the MSB of the BigInt
;				a.size >= b.magnitude
;
; Input: 		A and B, the BigInts to add
;
; Output: 		A = A + B
;
; Side Effects: None
;
; Overview: 	Quickly performs the bulk addition of two BigInts
;
; Note:			Function works
;***************************************************************************
	.global	__addBI
__addBI:
	inc2	__xA, WREG			; W0 = one word past _xA
	mov.w	__iA, W2
	mov.w	__xB, W3
	mov.w	__iB, W4

	inc2	W3, W3				; B end address needs to be one word past
	bclr	SR, #C				; C = 0

; Perform addition of all B words
aDoAdd:
	mov.w	[W4++], W1			; Read B[i]
	addc.w	W1, [W2], [W2++]	; A[i] = A[i] + B[i] + C
	xor.w	W3, W4, W5			; Is ++i past the MSB of B?
	bra		NZ, aDoAdd			; MSB not finished, go repeat loop

	bra		aFinalCarryCheck

; Add in final carry out and propagate forward as needed
aDoFinalCarry:
	addc.w	W5, [W2], [W2++]	; W5 == 0, A[i] = A[i] + C
aFinalCarryCheck:
	bra		NC, aDone
	xor.w	W0, W2, W6			; See if max address reached
	bra		NZ, aDoFinalCarry

aDone:
	return


;***************************************************************************
; Function: 	void _subBI()
;
; PreCondition: _iA and _iB are loaded with the address of the LSB of the BigInt
;				_xA and _xB are loaded with the address of the MSB of the BigInt
;
; Input: 		A and B, the BigInts to subtract
;
; Output: 		A = A - B
;
; Side Effects: None
;
; Overview: 	Quickly performs the bulk subtraction of two BigInts
;
; Note:			Function works
;***************************************************************************
	.global	__subBI
__subBI:
	inc2	__xA, WREG			; W0 = one word past _xA
	mov.w	__iA, W2
	mov.w	__xB, W3
	mov.w	__iB, W4

	inc2	W3, W3				; B end address needs to be one word past
	bset	SR, #C				; Borrow = 0

; Perform subtraction of all B words
sDoSub:
	mov.w	[W4++], W1			; Read B[i]
	subbr.w	W1, [W2], [W2++]	; A[i] = A[i] - B[i] - Borrow
	xor.w	W3, W4, W5			; Is ++i past the MSB?
	bra		NZ, sDoSub			; MSB not finished, go repeat loop

	bra		sFinalBorrowCheck

; Perform final borrow and propagate forward as needed
sDoFinalBorrow:
	subbr.w	W5, [W2], [W2++]	; W5 == 0, A[i] = A[i] - 0 - Borrow
sFinalBorrowCheck:
	bra		C, sDone
	xor.w	W0, W2, W6			; See if max address reached
	bra		NZ, sDoFinalBorrow

sDone:
	return


;***************************************************************************
; Function: 	void _zeroBI()
;
; PreCondition: _iA is loaded with the address of the LSB of the BigInt
;				_xA is loaded with the address of the MSB of the BigInt
;
; Input: 		None
;
; Output: 		A = 0
;
; Side Effects: None
;
; Overview: 	Sets all words from _iA to _xA to zero
;
; Note:			Function works
;***************************************************************************
	.global	__zeroBI
__zeroBI:
	inc2	__xA, WREG
	mov		__iA, W1

zDoZero:
	clr		[W1++]
	cp		W0, W1
	bra		NZ, zDoZero

	return


;***************************************************************************
; Function: 	void _msbBI()
;
; PreCondition: _iA is loaded with the address of the LSB of the BigInt buffer
;				_xA is loaded with the address of the right most byte of the BigInt buffer
;
; Input: 		None
;
; Output: 		_xA is now pointing to the MSB of the BigInt
;
; Side Effects: None
;
; Overview: 	Finds the MSB (first non-zero word) of the BigInt, starting 
;				from the right-most word and testing to the left.  This 
;				function will stop if _iA is reached.
;
; Note:			Function works
;***************************************************************************
	.global	__msbBI
__msbBI:
	mov		__xA, W0
	
msbLoop:
	cp		__iA
	bra		Z, msbDone
	cp0		[W0--]
	bra		Z, msbLoop
	
	inc2	W0, W0

msbDone:
	mov		W0, __xA
	return


;***************************************************************************
; Function: 	void _mulBI()
;
; PreCondition: _iA and _iB are loaded with the address of the LSB of each BigInt
;				_xA and _xB are loaded with the address of the MSB of the BigInt
;				_iR is loaded with the LSB address of the destination result memory
;				_iR memory must be zeroed and have enough space (_xB-_iB+_xA-_iA words)
;
; Input: 		A and B, the BigInts to multiply
;
; Output: 		R = A * B
;
; Side Effects: None
;
; Overview: 	Performs the bulk multiplication of two BigInts
;
; Note:			Function works
;***************************************************************************
	.global	__mulBI
__mulBI:
	push	W8
	push	W9

	; Decement xA, xB (to match termination case)
	; W0 = xA + 2	(+2 for termination case)
	; W1 used for iA
	; W2 = xB + 2	(+2 for termination case)
	; W3 used for iB
	; W6:W7 used for multiplication result
	; W8 used for iR
	; W9 = 0x0000
	inc2	__xA, WREG			
	mov		__xB, W2			
	inc2	W2, W2				

	mov		__iB, W3
	mov		__iR, W8			; W8 = R for B loop
	clr		W9

mLoopB:
	cp		W3, W2				; Compare current iB and xB
	bra		Z, mDone
	
	inc2	W8, W8
	mov		[W3++], W5			; Get current iB word
;	bra		Z, mLoopB			; Skip this iB if it is zero
	dec2	W8, W4				; W4 = iR for A loop
	mov		__iA, W1			; Load iA

mLoopA:
	mul.uu	W5, [W1++], W6		; W7:W6 = B * A
	add		W6, [W4], [W4++]	; R = R + (B*A)
	addc	W7, [W4], [W4]
	bra		NC, mFinishedCarrying
	mov		W4, W6
mKeepCarrying:
	addc	W9, [++W6], [W6]	; Add in residual carry to MSB of R and carry forward if it causes a carry out
	bra		C, mKeepCarrying
mFinishedCarrying:
	
	cp		W1, W0
	bra		NZ, mLoopA
	bra		mLoopB

mDone:
	pop		W9
	pop		W8
	return


;***************************************************************************
; Function: 	void _sqrBI()
;
; PreCondition: _iA is loaded with the address of the LSB of the BigInt
;				_xA is loaded with the address of the MSB of the BigInt
;				_iR is loaded with the LSB address of the destination result memory
;				_iR memory must be zeroed and have enough space (_xB-_iB+_xA-_iA words)
;
; Input: 		A, the BigInt to square
;
; Output: 		R = A * A
;
; Side Effects: None
;
; Overview: 	Squares BigInt A and stores result in R
;
; Note:			Function works
;***************************************************************************
	.global	__sqrBI
__sqrBI:
	mov		__iA, W0
	mov		W0, __iB
	mov		__xA, W0
	mov		W0, __xB
	bra		__mulBI


;***************************************************************************
; Function: 	void _masBI()
;
; PreCondition: _iB is loaded with the LSB of the modulus BigInt
;				_xB is loaded with the MSB of the modulus BigInt
;				_wC is loaded with the 16 bit integer by which to multiply
;				_iR is the starting LSB of the decumulator BigInt
;
; Input: 		B (BigInt) and C (16-bit int) to multiply
;
; Output: 		R = R - (B * C)
;
; Side Effects: None
;
; Overview: 	Performs a Multiply And Subtract function.  This is used in
;				the modulus calculation to save several steps.  A BigInt (iB/xB)
;				is multiplied by a single word and subtracted rather than
;				accumulated.
;
; Note:			Decumulator is the opposite of an accumulator,
;				if that wasn't obvious
;
; Note:			Function works
;***************************************************************************
	.global	__masBI
__masBI:
	; Increment xB (to match termination case)
	inc2	__xB, WREG
	mov		__iB, W1
	mov		__wC, W2
	mov		__iR, W3
	clr		W5					; Carry word

masLoop:
	subr	W5, [W3], [W3]		; Subtract carry word from R
	clr		W5					; Clear carry word
	btss	SR, #C				; If a borrow occured
	inc		W5, W5				;	save 1 to the carry word
	
	mul.uu	W2, [W1++], W6		; W7:W6 = B * C
	subr	W6, [W3], [W3++]	; R = R - (B * C)
	btg		SR, #C
	addc	W7, W5, W5

	cpseq	W1, W0				; Compare current B and xB
	bra		masLoop
	
	subr	W5, [W3], [W3]		; Finish borrowing
	
masDone:
	return


;***************************************************************************
; Function: 	void _copyBI()
;
; PreCondition: _iA and _iB are loaded with the address of the LSB of each BigInt
;				_xA and _xB are loaded with the address of the MSB of each BigInt
;
; Input: 		A and B, the destination and source
;
; Output: 		A = B
;
; Side Effects: None
;
; Stack Req: 	
;
; Overview: 	Copies a value from one BigInt to another
;
; Note:			Function works
;***************************************************************************
	.global	__copyBI
__copyBI:
	; Incrmenet xA, xB (to match termination case)
	inc2	__xA, WREG
	mov		W0, W1
	mov		__iA, W2
	inc2	__xB, WREG
	mov		__iB, W4

cLoop:
	mov		[W4++], [W2++]
	cp		W4, W0
	bra		Z, cZeroLoopTest
	cp		W2, W1
	bra		NZ, cLoop
	return

cZeroLoop:
	clr		[W2++]
cZeroLoopTest:
	cp		W2, W1
	bra		NZ, cZeroLoop
	return	

//#endif
