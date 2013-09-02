; *********************************************************************
; *
; *	 Big Integer Assembly Helpers
; *  Library for Microchip TCP/IP Stack
; *	  - Accelerates processing for BigInt functions
; *
; *********************************************************************
; * FileName:        BigInt_helper.asm
; * Dependencies:    None
; * Processor:       PIC18
; * Compiler:        Microchip C18 v3.30 or higher
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
; * Elliott Wood	     06/20/07	Original
; * Howard Schlunder	 11/15/07	Converted to little endian
; ********************************************************************/
 
BIGINT_VARS		udata
	global _iA
	global _iB
	global _iBr		;  when B is a ROM pointer
	global _iR
	global _xA
	global _xB
	global _xBr		;  when B is a ROM pointer
	global _wC
_iA			res	2	;*_iA, starting pointer for A (low address)
_xA			res	2	;*_xA, end pointer for A (high address)
_iB			res 2	;*_iB, starting pointer for B (low address)
_xB			res	2	;*_xB, end pointer for B (high address)
_iBr		res	3	;*_iBr, starting pointer for B in ROM (low address)
_xBr		res	3	;*_xBr, end pointer for B in ROM (high address)
_iR			res	2	;*_iR, starting pointer for multiplication Result (low address)
_wC			res 1	;Value of C for _masBI (scalar) and _mul (temp storage)

CarryByte	res 1	;Value of carry for _mul and _sqr (scalar)
CarryH		res 1	;High value of carry for _sqr (scalar)

BIGINT_CODE		code

#include p18cxxx.inc
#include P18MACRO.INC


;***************************************************************************
; Function: 	void _addBI()
;
; PreCondition: _iA and _iB are loaded with the LSB of each BigInt
;				_xA and _xB are loaded with the MSB of each BigInt
;				A.size >= B.magnitude
;				A and B must both be 2048 bits or less (256 bytes)
;
; Input: 		A and B, the BigInts to add
;
; Output: 		A = A + B
;
; Side Effects: None
;
; Stack Req: 	2 bytes
;
; Overview: 	Quickly performs the bulk addition of two BigInts
;***************************************************************************
	GLOBAL	_addBI
_addBI:
    Stk2PushFromReg FSR2L		;Save FSR2 on the stack
	banksel _xA					;select the assembly pointer bank
	movff	_iA+0x0,FSR0L		;Put iA in FSR0
	movff	_iA+0x1,FSR0H
	movff	_iB+0x0,FSR2L		;Put iB in FSR2
	movff	_iB+0x1,FSR2H
	
	; Predecrement A and B pointers
	movf	POSTDEC0, W
	movf	POSTDEC2, W

	; Calculate how many adds are needed divided by 8, store count in 
	; PRODH
	; Note: This assumes there are no more than 256 adds to do
	decf	_iB+0x0, W
	subwf	_xB+0x0, W
	andlw	0xF8
	movwf	PRODH
	swapf	PRODH, F
	rlncf	PRODH, F
	
	incf	PRODH, F		; Preincrement so we can start the loop with a decrement
	clrf	PRODL			; Start out with no carry
	bra		aTest8Add

	; Add 8 bytes of B into A at a time.  Doing it in such large 
	; chunks saves loop and branch overhead
aDo8Add:
	rrcf	PRODL, F		; Load carry in value
	movf	PREINC2, W
	addwfc	PREINC0, F
	movf	PREINC2, W
	addwfc	PREINC0, F
	movf	PREINC2, W
	addwfc	PREINC0, F
	movf	PREINC2, W
	addwfc	PREINC0, F
	movf	PREINC2, W
	addwfc	PREINC0, F
	movf	PREINC2, W
	addwfc	PREINC0, F
	movf	PREINC2, W
	addwfc	PREINC0, F
	movf	PREINC2, W
	addwfc	PREINC0, F
	rlcf	PRODL, F		; Save carry out value
aTest8Add:	
	decf	PRODH, F
	bnz		aDo8Add


	rrcf	PRODL, F		; Load carry in value
	bra		aTestResidualAdd


	; Add up to 7 bytes of B into A, one byte at a time.
aDoResidualAdd:
	movf	PREINC2, W
	addwfc	PREINC0, F

aTestResidualAdd:
	movf	FSR2L, W
	xorwf	_xB+0x0, W
	bnz		aDoResidualAdd

	; Carry forward the carry out (in A) if needed
	bnc		aDone

aDoFinalCarry:
	movf	FSR0L, W
	xorwf	_xA+0x0, W
	bz		aDone
	incf	PREINC0, F
	bc		aDoFinalCarry

aDone:
	Stk2PopToReg	FSR2L		;restore FSR2 from stack
	return


;***************************************************************************
; Function: 	void _addBIROM()
;
; PreCondition: _iA and _iBr are loaded with the LSB of each BigInt
;				_xA and _xBr are loaded with the MSB of each BigInt
;				A.size >= B.magnitude
;				A and B must both be 2048 bits or less (256 bytes)
;
; Input: 		A: a BigInt in RAM
;				B: a BigInt in ROM
;
; Output: 		A = A + B
;
; Side Effects: Overwrites TBLPTRU:TBLPTRH:TBLPTRL registers
;
; Stack Req: 	0 bytes
;
; Overview: 	Quickly performs the bulk addition of two BigInts
;***************************************************************************
	GLOBAL	_addBIROM
_addBIROM:
	banksel _xA					;Select the assembly pointer bank
	movff	_iA+0x0,FSR0L		;Put iA in FSR0
	movff	_iA+0x1,FSR0H
	movff	_iBr+0x0,TBLPTRL	;Put iB in TBLPTR
	movff	_iBr+0x1,TBLPTRH
	movff	_iBr+0x2,TBLPTRU		
	
	; Predecrement A and B pointers
	movf	POSTDEC0, W
	tblrd*-	

	; Calculate how many adds are needed divided by 8, store count in 
	; PRODH
	; Note: This assumes there are no more than 256 adds to do
	decf	_iBr+0x0, W
	subwf	_xBr+0x0, W
	andlw	0xF8
	movwf	PRODH
	swapf	PRODH, F
	rlncf	PRODH, F
	
	incf	PRODH, F		; Preincrement so we can start the loop with a decrement
	clrf	PRODL			; Start out with no carry
	bra		aRTest8Add

	; Add 8 bytes of B into A at a time.  Doing it in such large 
	; chunks saves loop and branch overhead
aRDo8Add:
	rrcf	PRODL, F		; Load carry in value
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F
	rlcf	PRODL, F		; Save carry out value
aRTest8Add:	
	decf	PRODH, F
	bnz		aRDo8Add


	rrcf	PRODL, F		; Load carry in value
	bra		aRTestResidualAdd


	; Add up to 7 bytes of B into A, one byte at a time.
aRDoResidualAdd:
	tblrd+*
	movf	TABLAT, W
	addwfc	PREINC0, F

aRTestResidualAdd:
	movf	TBLPTRL, W
	xorwf	_xBr+0x0, W
	bnz		aRDoResidualAdd

	; Carry forward the carry out (in A) if needed
	bnc		aRDone

aRDoFinalCarry:
	movf	FSR0L, W
	xorwf	_xA+0x0, W
	bz		aRDone
	incf	PREINC0, F
	bc		aRDoFinalCarry

aRDone:
	return


;***************************************************************************
; Function: 	void _subBI()
;
; PreCondition: _iA and _iB are loaded with the LSB of each BigInt
;				_xA and _xB are loaded with the MSB of each BigInt
;				A.size >= B.magnitude
;				A and B must both be 2048 bits or less (256 bytes)
;
; Input: 		A and B, the BigInts to subtract
;
; Output: 		A = A - B
;
; Side Effects: None
;
; Stack Req: 	2 bytes
;
; Overview: 	Quickly performs the bulk subtraction of two BigInts
;***************************************************************************
	GLOBAL	_subBI
_subBI:
    Stk2PushFromReg FSR2L		;Save FSR2 on the stack
	banksel _xA					;select the assembly pointer bank
	movff	_iA+0x0,FSR0L		;Put iA in FSR0
	movff	_iA+0x1,FSR0H
	movff	_iB+0x0,FSR2L		;Put iB in FSR2
	movff	_iB+0x1,FSR2H
	
	; Predecrement A and B pointers
	movf	POSTDEC0, W
	movf	POSTDEC2, W

	; Calculate how many subtracts are needed divided by 8, store 
	; count in PRODH
	; Note: This assumes there are no more than 256 subtracts to do
	decf	_iB+0x0, W
	subwf	_xB+0x0, W
	andlw	0xF8
	movwf	PRODH
	swapf	PRODH, F
	rlncf	PRODH, F
	
	incf	PRODH, F		; Preincrement so we can start the loop with a decrement
	setf	PRODL			; Start out with no borrow
	bra		sTest8Subtract

	; Add 8 bytes of B into A at a time.  Doing it in such large 
	; chunks saves loop and branch overhead
sDo8Subtract:
	rrcf	PRODL, F		; Load borrow in value
	movf	PREINC2, W
	subwfb	PREINC0, F
	movf	PREINC2, W
	subwfb	PREINC0, F
	movf	PREINC2, W
	subwfb	PREINC0, F
	movf	PREINC2, W
	subwfb	PREINC0, F
	movf	PREINC2, W
	subwfb	PREINC0, F
	movf	PREINC2, W
	subwfb	PREINC0, F
	movf	PREINC2, W
	subwfb	PREINC0, F
	movf	PREINC2, W
	subwfb	PREINC0, F
	rlcf	PRODL, F		; Save borrow out value
sTest8Subtract:	
	decf	PRODH, F
	bnz		sDo8Subtract


	rrcf	PRODL, F		; Load borrow in value
	bra		sTestResidualSubtract


	; Subtract up to 7 bytes of B from A, one byte at a time.
sDoResidualSubtract:
	movf	PREINC2, W
	subwfb	PREINC0, F

sTestResidualSubtract:
	movf	FSR2L, W
	xorwf	_xB+0x0, W
	bnz		sDoResidualSubtract

	; Carry forward the borrow out (in A) if needed
	bc		sDone

sDoFinalCarry:
	movf	FSR0L, W
	xorwf	_xA+0x0, W
	bz		sDone
	decf	PREINC0, F
	bnc		sDoFinalCarry

sDone:
	Stk2PopToReg	FSR2L		;restore FSR2 from stack
	return


;***************************************************************************
; Function: 	void _subBIROM()
;
; PreCondition: _iA and _iBr are loaded with the LSB of each BigInt
;				_xA and _xBr are loaded with the MSB of each BigInt
;				A.size >= B.magnitude
;				A and B must both be 2048 bits or less (256 bytes)
;
; Input: 		A: a BigInt in RAM
;				B: a BigInt in ROM
;
; Output: 		A = A - B
;
; Side Effects: Overwrites TBLPTRU:TBLPTRH:TBLPTRL registers
;
; Stack Req: 	None
;
; Overview: 	Quickly performs the bulk subtraction of two BigInts
;***************************************************************************
	GLOBAL	_subBIROM
_subBIROM:
	banksel _xA					;Select the assembly pointer bank
	movff	_iA+0x0,FSR0L		;Put iA in FSR0
	movff	_iA+0x1,FSR0H
	movff	_iBr+0x0,TBLPTRL	;Put iB in TBLPTR
	movff	_iBr+0x1,TBLPTRH
	movff	_iBr+0x2,TBLPTRU		
	
	; Predecrement A and B pointers
	movf	POSTDEC0, W
	tblrd*-	

	; Calculate how many adds are needed divided by 8, store count in 
	; PRODH
	; Note: This assumes there are no more than 256 adds to do
	decf	_iBr+0x0, W
	subwf	_xBr+0x0, W
	andlw	0xF8
	movwf	PRODH
	swapf	PRODH, F
	rlncf	PRODH, F
	
	incf	PRODH, F		; Preincrement so we can start the loop with a decrement
	setf	PRODL			; Start out with no borrow
	bra		sRTest8Subtract

	; Subtract 8 bytes of B from A at a time.  Doing it in such large 
	; chunks saves loop and branch overhead
sRDo8Subtract:
	rrcf	PRODL, F		; Load borrow in value
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F
	rlcf	PRODL, F		; Save borrow out value
sRTest8Subtract:	
	decf	PRODH, F
	bnz		sRDo8Subtract


	rrcf	PRODL, F		; Load borrow in value
	bra		sRTestResidualSubtract


	; Subtract up to 7 bytes of B from A, one byte at a time.
sRDoResidualSubtract:
	tblrd+*
	movf	TABLAT, W
	subwfb	PREINC0, F

sRTestResidualSubtract:
	movf	TBLPTRL, W
	xorwf	_xBr+0x0, W
	bnz		sRDoResidualSubtract

	; Carry forward the borrow out (in A) if needed
	bc		sRDone

sRDoFinalCarry:
	movf	FSR0L, W
	xorwf	_xA+0x0, W
	bz		sRDone
	incf	PREINC0, F
	bnc		sRDoFinalCarry

sRDone:
	return


;***************************************************************************
; Function: 	void _zeroBI()
;
; PreCondition: _iA is loaded with the LSB of the BigInt
;				_xA is loaded with the MSB the BigInt
;
; Input: 		A: a BigInt
;
; Output: 		A = 0
;
; Side Effects: None
;
; Stack Req: 	None
;
; Overview: 	Sets all bytes from _iA to _xA to zero
;***************************************************************************
	GLOBAL	_zeroBI
_zeroBI:
	banksel _xA					;select the assembly pointer bank
	movff	_iA+0x0,FSR0L		;Put iA-1 in FSR0
	movff	_iA+0x1,FSR0H
	movf	POSTDEC0, W
	
zLoop:
	clrf	PREINC0				;set byte to zero
	movf	FSR0L,W				;check if A is at MSB
	xorwf	_xA+0x0,W			;test low byte first
	bnz		zLoop				;add to A, and continue
	movf	FSR0H,W				;check high byte
	xorwf	_xA+0x1,W			;
	bnz		zLoop				;if A is not at MSB, we're not done
	
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
;***************************************************************************
	GLOBAL	_msbBI
_msbBI:
	banksel _xA					; Select the correct bank
	movff	_xA+0x0,FSR0L		; Put xA in FSR0
	movff	_xA+0x1,FSR0H

msbLoop:
	movf	FSR0L, W
	subwf	_iA+0x0, W
	bz		msbFound
	movf	POSTDEC0, W			; load the next value
	bz		msbLoop				; if value isn't zero, this is the MSB
	
	movf	PREINC0, W			; Correct the pointer's last decrement
	
	; Copy FSR0 back to _xA and return
msbFound:
	movff	FSR0L,_xA+0x0		; Move FSR0 back to iA
	movff	FSR0H,_xA+0x1		
	return


;***************************************************************************
; Function: 	void _mulBI()
;
; PreCondition: _iA and _iB are loaded with the LSB of each BigInt
;				_xA and _xB are loaded with the MSB of each BigInt
;				_iR is the LSB of the accumulator BigInt
;				_iR is zeroed, and has enough space
;				A and B must both by 2048 bits or less (256 bytes max)
;
; Input: 		A and B, the BigInts to multiply
;
; Output: 		R = A * B
;
; Side Effects: None
;
; Stack Req: 	2 bytes
;
; Overview: 	Performs the bulk multiplication of two BigInts
;***************************************************************************
	GLOBAL	_mulBI
_mulBI:
    Stk2PushFromReg FSR2L		;Save FSR2 on the stack
	banksel _xA					;select the assembly pointer bank

	; Predecrement B and R pointers
	clrf	WREG
	decf	_iB+0x0, F
	subwfb	_iB+0x1, F
	decf	_iR+0x0, F
	subwfb	_iR+0x1, F

mLoadB:
	; Load B pointer to FSR2 and increment B pointer
	; Increment iR to match the new starting position
	; Fetch new B value
	movff	_iB+0x0, FSR2L
	movff	_iB+0x1, FSR2H	
	clrf	WREG
mLoadBAgain:
	incf	_iB+0x0, F
	addwfc	_iB+0x1, F
	incf	_iR+0x0, F
	addwfc	_iR+0x1, F
	movf	PREINC2, W
	bz		mLoadBAgain			; No need to multiply this byte if it is zero
	movwf	_wC

	; Restore iA start pointer in FSR0
	movff	_iA+0x0, FSR0L
	movff	_iA+0x1, FSR0H

	; Store new iR start pointer in FSR2
	movff	_iR+0x0, FSR2L
	movff	_iR+0x1, FSR2H

	; Clear carry byte
	clrf	CarryByte

mLoopA:
	movf	CarryByte, W		;load carry byte
	addwf	INDF2, F			;add to accumulator
	clrf	CarryByte			;clear carry byte
	rlcf	CarryByte, F		;if a carry occurred, save to CarryByte
	movf	_wC, W				;load wC
	mulwf	POSTINC0			;calculate wC * A[iA++]
	movf	PRODL,W				;load low result byte
	addwf	POSTINC2, F			;add to accumulator, and move ptr
	movf	PRODH,W				;load high result byte
	addwfc	CarryByte, F		;add to carry byte, along with
								;  any carry from previous addition

	; See if A loop is done
	decf	FSR0L, W
	xorwf	_xA+0x0, W
	bnz		mLoopA

	; If A loop is done, finish out the carrying
	movff	CarryByte, INDF2		;save carry byte (always adding to zero)

	; See if B loop is done
	movf	_xB+0x0, W
	xorwf	_iB+0x0, W
	bnz		mLoadB

mDone:
	Stk2PopToReg	FSR2L		;restore old FSR2
	return
	

;***************************************************************************
; Function: 	void _mulBIROM()
;
; PreCondition: _iA and _iBr are loaded with the LSB of each BigInt
;				_xA and _xBr are loaded with the MSB of each BigInt
;				_iR is the LSB of the accumulator BigInt
;				_iR is zeroed, and has enough space
;
; Input: 		A: BigInt in RAM
;				B: BigInt in ROM
;
; Output: 		R = A * B
;
; Side Effects: Overwrites TBLPTRU:TBLPTRH:TBLPTRL
;
; Stack Req: 	2 bytes
;
; Overview: 	Performs the bulk multiplication of two BigInts
;***************************************************************************
	GLOBAL	_mulBIROM
_mulBIROM:
    Stk2PushFromReg FSR2L		;Save FSR2 on the stack
	banksel _xA					;select the assembly pointer bank

	; Predecrement B and R pointers
	movff	_iBr+0x0, TBLPTRL
	movff	_iBr+0x1, TBLPTRH
	movff	_iBr+0x2, TBLPTRU
	tblrd*-
	clrf	WREG
	decf	_iR+0x0, F
	subwfb	_iR+0x1, F

mRLoadB:
	; Increment iR to match the new starting position
	; Fetch new B value
	clrf	WREG
mRLoadBAgain:
	incf	_iR+0x0, F
	addwfc	_iR+0x1, F
	tblrd+*
	movf	TABLAT, W
	bz		mRLoadBAgain			; No need to multiply this byte if it is zero
	movwf	_wC

	; Restore iA start pointer in FSR0
	movff	_iA+0x0, FSR0L
	movff	_iA+0x1, FSR0H

	; Store new iR start pointer in FSR2
	movff	_iR+0x0, FSR2L
	movff	_iR+0x1, FSR2H

	; Clear carry byte
	clrf	CarryByte

mRLoopA:
	movf	CarryByte, W		;load carry byte
	addwf	INDF2, F			;add to accumulator
	clrf	CarryByte			;clear carry byte
	rlcf	CarryByte, F		;if a carry occurred, save to CarryByte
	movf	_wC, W				;load wC
	mulwf	POSTINC0			;calculate wC * A[iA++]
	movf	PRODL,W				;load low result byte
	addwf	POSTINC2, F			;add to accumulator, and move ptr
	movf	PRODH,W				;load high result byte
	addwfc	CarryByte, F		;add to carry byte, along with
								;  any carry from previous addition

	; See if A loop is done
	decf	FSR0L, W
	xorwf	_xA+0x0, W
	bnz		mRLoopA

	; If A loop is done, finish out the carrying
	movff	CarryByte, INDF2		;save carry byte (always adding to zero)

	; See if B loop is done
	movf	_xBr+0x0, W
	xorwf	TBLPTRL, W
	bnz		mRLoadB

mRDone:
	Stk2PopToReg	FSR2L		;restore old FSR2
	return


;***************************************************************************
; Function: 	void _sqrBI()
;
; PreCondition: _iA is loaded with the LSB of the BigInt
;				_xA is loaded with the MSB of the BigInt
;				_iR is the LSB of the accumulator BigInt
;				_iR is zeroed, and has enough space
;
; Input: 		A: Source BigInt to square
;				R: Output location
;
; Output: 		R = A * A
;
; Side Effects: None
;
; Stack Req: 	6 bytes
;
; Overview: 	Performs the bulk multiplication of two BigInts
;***************************************************************************
	GLOBAL	_sqrBI
_sqrBI:
    Stk2PushFromReg FSR2L		;Save FSR2 on the stack
	banksel _iA					;select the assembly pointer bank

	;decement iA (to set up for termination case)
	clrf 	WREG				;load zero to W
	decf	_iA,F				;decrement iA
	subwfb	_iA+0x1,F			;borrow if needed

	;set up for outer loop over all values of A
	Stk2PushFromReg	_iR			;save initial iR to stack
	Stk2PushFromReg _iA			;save initial iA to stack

qOuterLoop:
	Stk2PopToReg	FSR0L		;pop next iA from stack
	Stk2PopToReg	FSR2L		;pop next iR from stack

	;check if outer loop is done
	movf	FSR0L,W				;check if low byte of A ptr
	xorwf	_xA+0x0,W			;  equals low byte of stop ptr
	bnz		qNextOut			;if not, work on next byte of A
	movf	FSR0H,W				;check if high byte of A ptr
	xorwf	_xA+0x1,W			;  equals high byte of stop ptr
	bz		qDone				;if so, terminate

qNextOut:
	;save next value of iR
	movf	PREINC2,W			;decrement iR twice for next value
	movf	PREINC2,W
	Stk2PushFromReg	FSR2L		;save next value of iR to stack
	movf	POSTDEC2,W			;restore iR value
	movf	POSTDEC2,W

	;save next value of iA
	movf	PREINC0,W			;decrement iA for next value
	Stk2PushFromReg	FSR0L		;save next value of iA to stack

	;load wC with value of A[iA--]
	;check wC==0, if so then we can skip this byte
	movf	POSTDEC0,W			;restore iA and copy value
	bz		qOuterLoop			;if B==0, continue to next byte
	movwf	_wC					;  into temporary byte
	
	;set up for inner loop over all remaining values in A
	clrf	CarryByte			;clear carry bytes
	clrf	CarryH

	;first result only gets accumulated once
	mulwf	PREINC0				;square first byte (W = B above)
	movf	PRODL,W				;load PRODL
	addwf	POSTINC2,F			;accumulate
	movf	PRODH,W				;load PRODH
	addwfc	CarryByte,F			;save carry byte (with prev carry)

qInnerLoop:
	;if A isn't complete, keep looping
	movf	FSR0L,W				;check if low byte of A ptr
	xorwf	_xA+0x0,W			;  equals low byte of stop ptr
	bnz		qInnerByte			;if not, continue looping
	movf	FSR0H,W				;check if high byte of A ptr
	xorwf	_xA+0x1,W			;  equals high byte of stop ptr
	bz		qInnerDone			;if not, continue looping

qInnerByte:
	;all future bytes get accumulated twice
	movf	CarryByte,W			;load carry byte
	addwf	INDF2,F				;add to accumulator
	movff	CarryH,CarryByte	;move high carry byte down
	clrf	CarryH
	btfsc	STATUS,C			;if a carry occurred
	incf	CarryByte,F			;  add 1 to the carry byte
	movf	_wC,W				;load B
	mulwf	PREINC0				;calculate B * A[iA--]
	bcf		STATUS,C			;multiply product by 2
	rlcf	PRODL,F				
	rlcf	PRODH,F
	btfsc	STATUS,C			;if a carry occurrs
	incf	CarryH,F			;  save 1 to the CarryH byte
	movf	PRODL,W				;load low result byte
	addwf	POSTINC2,F			;add to accumulator, and move ptr
	movf	PRODH,W				;load high result byte
	addwfc	CarryByte,F			;add to carry byte, along with
								;  any carry from previous addition
	btfsc	STATUS,C			;if a carry occurrs
	incf	CarryH,F			;  save 1 to the CarryH byte
	bra		qInnerLoop
	

qInnerDone
	;A is complete, finish out the carrying
	movf	CarryByte,W			;accumulate the carry bytes
	addwf	POSTINC2,F
	movf	CarryH,W
	addwfc	INDF2
	bra		qOuterLoop

qDone:
	;movff	FSR2L,_iR+0x0
	;movff	FSR2H,_iR+0x1
	Stk2PopToReg	FSR2L		;restore old FSR2
	return


;***************************************************************************
; Function: 	void _masBI()
;
; PreCondition: _iB is loaded with the LSB of the modulus BigInt
;				_xB is loaded with the MSB of the modulus BigInt
;				_wC is loaded with the 8 bit integer by which to multiply
;				_iR is the starting LSB of the decumulator BigInt
;				B must be 256 bytes or less
;
; Input: 		B: a BigInt to multiply with wC
;				wC: a simple scalar
;				R: a BigInt decumulator to subtract the result from
;
; Output: 		R = R - (B * wC)
;
; Side Effects: None
;
; Stack Req: 	2 bytes
;
; Overview: 	Performs a Multiply And Subtract function.  This is used in
;				the modulus calculation.
;
; Note:			As an optimization, the final borrow forward propagation 
;				is commented out below.  This results in an a potentially 
;				incorrect result in R, but for the BigIntMod() algorithm, 
;				the final borrow forward propagation would be both unneeded 
;				and unwanted.  For BigIntMod(), this underflow is repaired 
;				by adding B back into R one or two times.
;***************************************************************************
	GLOBAL	_masBI
_masBI:
    Stk2PushFromReg FSR2L		;Save FSR2 on the stack
	banksel _xB					;select the assembly pointer bank
	
	; Load B pointer into FSR2 and R pointer into FSR0
	movff	_iR+0x0, FSR0L		;load iR into FSR0
	movff	_iR+0x1, FSR0H
	movff	_iB+0x0, FSR2L		;load iB into FSR2
	movff	_iB+0x1, FSR2H
	
	; Predecrement B pointer
	movf	POSTDEC2, W	
	
	; Calculate loop counters for /4 and /1
masLoop4Init:
	clrf	CarryByte
	movf	FSR2L, W
	subwf	_xB+0x0, W
	bcf		STATUS, C
	rrcf	WREG, W
	rlcf	CarryByte
	rrcf	WREG, W
	btfsc	STATUS, C
	bsf		CarryByte, 1
	movwf	CarryH

	movf	CarryH, F
	bz		masLoopInit		

	clrf	WREG

masLoop4:
	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	mulwf	PREINC2				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W

	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	mulwf	PREINC2				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W

	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	mulwf	PREINC2				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W
	
	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	mulwf	PREINC2				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W
	
	; See if we've looped through all (B bytes)/4
	decfsz	CarryH, F
	bra		masLoop4


masLoopInit:
	movf	CarryByte, F
	bz		masFinalBorrow		

masLoop:
	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	mulwf	PREINC2				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W

	; See if we've looped through all B bytes in the residual
	decfsz	CarryByte, F
	bra		masLoop

	; If B is complete, finish out the borrow and return
masFinalBorrow:
	subwf	POSTINC0, F

	Stk2PopToReg	FSR2L		;restore old FSR2
	return


;***************************************************************************
; Function: 	void _masBIROM()
;
; PreCondition: _iBr is loaded with the LSB of the modulus BigInt
;				_xBr is loaded with the MSB of the modulus BigInt
;				_wC is loaded with the 8 bit integer by which to multiply
;				_iR is the starting LSB of the decumulator BigInt
;				Br must be 256 bytes or less
;
; Input: 		Br: a BigInt in ROM to multiply with wC
;				wC: a simple scalar
;				R: a BigInt decumulator in RAM to subtract the result from
;
; Output: 		R = R - (Br * wC)
;
; Side Effects: Overwrites TBLPTRU:TBLPTRH:TBLPTRL
;
; Stack Req: 	None
;
; Overview: 	Performs a Multiply And Subtract function.  This is used in
;				the modulus calculation.
;
; Note:			As an optimization, the final borrow forward propagation 
;				is commented out below.  This results in an a potentially 
;				incorrect result in R, but for the BigIntMod() algorithm, 
;				the final borrow forward propagation would be both unneeded 
;				and unwanted.  For BigIntMod(), this underflow is repaired 
;				by adding B back into R one or two times.
;***************************************************************************
	GLOBAL	_masBIROM
_masBIROM:
	banksel _xB					;select the assembly pointer bank
	
	; Load B pointer into TBLPTR and R pointer into FSR0
	movff	_iR+0x0, FSR0L		;load iR into FSR0
	movff	_iR+0x1, FSR0H
	movff	_iBr+0x0, TBLPTRL	;load iBr into TBLPTR
	movff	_iBr+0x1, TBLPTRH
	movff	_iBr+0x2, TBLPTRU
	
	; Predecrement Br pointer and set up carry byte
	tblrd*-

	; Calculate loop counters for /4 and /1
masRLoop4Init:
	clrf	CarryByte
	movf	TBLPTRL, W
	subwf	_xBr+0x0, W
	bcf		STATUS, C
	rrcf	WREG, W
	rlcf	CarryByte
	rrcf	WREG, W
	btfsc	STATUS, C
	bsf		CarryByte, 1
	movwf	CarryH

	movf	CarryH, F
	bz		masRLoopInit		

	clrf	WREG

masRLoop4:
	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	tblrd+*
	mulwf	TABLAT				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W

	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	tblrd+*
	mulwf	TABLAT				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W

	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	tblrd+*
	mulwf	TABLAT				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W

	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	tblrd+*
	mulwf	TABLAT				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W
	
	; See if we've looped through all B bytes
	decfsz	CarryH, F
	bra		masRLoop4


masRLoopInit:
	movf	CarryByte, F
	bz		masRFinalBorrow		

masRLoop:
	subwf	INDF0, F			;subtract from decumulator
	movf	_wC, W				;load B
	tblrd+*
	mulwf	TABLAT				;calculate wC * B[iB--]
	btfss	STATUS, C			;if a borrow occurred
	incf	PRODH, F			;  save 1 to the carry byte
	movf	PRODL, W			;load low result byte
	subwf	POSTINC0, F			;subtract from decumulator, and move ptr
	movf	PRODH, W
	btfss	STATUS, C
	incf	PRODH, W
	
	; See if we've looped through all B bytes in the residual
	decfsz	CarryByte, F
	bra		masRLoop

	; If B is complete, finish out the borrow and return
masRFinalBorrow:
	subwf	POSTINC0, F

	return
	

;***************************************************************************
; Function: 	void _copyBI()
;
; PreCondition: _iA and _iB are loaded with the LSB of each BigInt
;				_xA and _xB are loaded with the MSB of each BigInt
;				B must be 256 bytes or less
;
; Input: 		A: the destination
;				B: the source
;
; Output: 		A = B
;
; Side Effects: None
;
; Stack Req: 	2 bytes
;
; Overview: 	Copies a value from BigInt B into BigInt A.  If A has more 
;				bytes allocated than B, then the most significant bytes in 
;				A are zero padded.  If A has less bytes allocated than B, 
;				then the most significant bytes are truncated off in A.
;***************************************************************************
	GLOBAL	_copyBI
_copyBI:
    Stk2PushFromReg FSR2L		;Save FSR2 on the stack
	banksel _xA					;select the assembly pointer bank

	;Load and predecrement iA, iB (to set up termination case)
	movff	_iA+0x0, FSR0L		;load iA into FSR0
	movff	_iA+0x1, FSR0H
	movff	_iB+0x0, FSR2L		;load iB into FSR2
	movff	_iB+0x1, FSR2H
	movf	POSTDEC0, W
	movf	POSTDEC2, W
	
cLoop:
	movff	PREINC2, PREINC0	;copy B to A
	movf	FSR2L, W			;check if low byte of B ptr
	xorwf	_xB+0x0, W			;  equals low byte of stop ptr
	bz		cZeroTest			;if so, B is done, so clear rest of A

	movf	FSR0L, W			;check if low byte of A ptr
	xorwf	_xA+0x0, W			;  equals low byte of stop ptr
	bnz		cLoop				;if not, continue
	movf	FSR0H, W			;check if high byte of A ptr
	xorwf	_xA+0x1, W			;  equals high byte of stop ptr
	bnz		cLoop				;if so, terminate

	bra		cZeroTest

cZero:
	clrf	PREINC0				;set A byte to zero

cZeroTest:
	movf	FSR0L, W			;check if low byte of A ptr
	xorwf	_xA+0x0, W			;  equals low byte of stop ptr
	bnz		cZero				;if not, continue
	movf	FSR0H, W			;check if high byte of A ptr
	xorwf	_xA+0x1, W			;  equals high byte of stop ptr
	bnz		cZero				;if so, terminate

	Stk2PopToReg	FSR2L		;restore old FSR2
	return


  end
