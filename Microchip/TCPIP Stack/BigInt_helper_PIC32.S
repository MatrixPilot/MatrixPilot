/**********************************************************************
 *
 * Big Integer Assembly Helpers
 * Library for Microchip TCP/IP Stack
 * - Accelerates processing for BigInt functions
 *
 **********************************************************************
 * FileName:        BigInt_helper_PIC32.S
 * Dependencies:    None
 * Processor:       PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Abhay Deshmukh     03/04/08      Original
 *********************************************************************/

#define __GENERIC_TYPE_DEFS_H_
#define __COMPILER_H
#include "HardwareProfile.h"
#include "TCPIPConfig.h"
#include "TCPIP Stack/SSLClientSize.h"

#if (defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)) && (!defined(ENC100_INTERFACE_MODE) || (SSL_RSA_CLIENT_SIZE > 1024))

#include <p32xxxx.h> 

#define  MAX_UNSIGNED_INT_VALUE     0xFFFFFFFF

#define  ONE_VALUE                  0x00000001

#define  ZERO_VALUE                 0x00000000

.bss
.align    4 
.global   _iA
.global   _xA
.global   _iB
.global   _xB
.global   _iR
.global   _wC
_iA:          .word         4   /* _iA, starting index for A (lower memory address, least significant byte/word) */
_xA:          .word         4   /* _xA, end index for A (higher memory address, most significant byte/word) */
_iB:          .word         4   /* _iB, starting index for B (lower memory address, least significant byte/word) */
_xB:          .word         4   /* _xB, end index for B (higher memory address, most significant byte/word) */
_iR:          .word         4   /* _iR, starting index for Res (lower memory address, least significant byte/word) */
_wC:          .word         4   /*  value of B for _mas (little endian word) */

 .set noreorder 

.text

/***************************************************************************
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
;***************************************************************************/

/* Note :- There is no Carry/Borrow Flag in STATUS Register of PIC32...So Workarounds were done */

    .global	_addBI

 .ent _addBI

_addBI:
    la     t0, _xA            # Load adress of _xA
    lw     t0, 0(t0)          # Load the Contents of memory pointing by t0 register
    
    la     t1, _iA
    addiu  t0, t0, 4          # Store the address present in _xA & increment it by 4 ( since every access is 32 bit )
    lw     t1, 0(t1)          # Store the address present in __iA
    
    la     t2, _xB
    lw     t2, 0(t2)

    la     t3, _iB
    addiu  t2, t2, 4          # Store the address present in _xB & increment it by 4
    lw     t3, 0(t3)          # Store the address present in _iB

    la    t6, ZERO_VALUE      # Store 0x00000000 in t6

aDoAdd:
    lw     t4, 0(t3)          # Place the LSW of _iB in t4
    addiu  t3, t3, 4          # t3 = t3 + t4
    lw     t5, 0(t1)          # Place the LSW of _iA in t5
    addu   t5, t5, t6         # t5 = t5 + t6
    addu   t7, t5, t4         # t7 = t5 + t4
    sw     t7, 0(t1)          # 4 bytes of data transfered from address specified by t1 to t5
    sltu   t8, t7, t5         # if ( t7 < t5 ) then ( t8 = 1 )
    movz   t8, t6, t5
    move   t6, t8             # PrevCarry = carry
    bne    t3, t2, aDoAdd     # if t3 != t2 then Jump to 'aDoAdd'
    addiu  t1, t1, 4
    beqz   t6, addDone
    nop

AddCarry:
    beq    t0, t1, addDone      # if t0 = t1 then Jump to 'nextDoadd'
    lw     t7, 0(t1)
    addiu  t5, t7, 1
    sw     t5, 0(t1)          # Store the Contents of t5 in memory pointed by t1 register
    addiu  t1, t1, 4
    beqz   t7, AddCarry
    nop

addDone:
    jr     ra                # return to function from where you are called
    nop

 .end _addBI


/***************************************************************************
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
;***************************************************************************/
    .global _subBI

 .ent _subBI

_subBI:
    la     t0, _xA
    lw     t0, 0(t0)           # Store the address present in __xA & increment it by 4
    
    la     t1, _iA            # Store the address present in __iA
    addiu  t0, t0, 4
    lw     t1, 0(t1)
    
    la     t2, _xB            # Store the address present in __xB & increment it by 4
    lw     t2, 0(t2)

    la     t3, _iB            # Store the address present in __iB
    addiu  t2, t2, 4
    lw     t3, 0(t3)

    la     t6, ZERO_VALUE      # Store 0x00000000 in t6

sDoSub:
    move   t8,$0
    lw     t4, 0(t3)          # Place the LSW of __iB in t4
    addiu  t3, t3, 4          # t3 = t3 + t4
    lw     t5, 0(t1)          # Place the LSW of __iA in t5
    movz   t8, t6, t5
    subu   t5, t5, t6         # t8 = t5 - t6
    sltu   t7, t5, t4         # if ( t8 < t4 ) then ( t7 = 1 )
    subu   t5, t5, t4         # t5 = t5 - t4
    sw     t5, 0(t1)          # 4 bytes of data transfered from address specified by t1 to t5
    or     t7, t7, t8
    move   t6, t7
    bne    t3, t2, sDoSub
    addiu  t1, t1, 4
    beqz   t6, subDone
    nop

SubBorrow:
    beq    t0, t1, subDone
    lw     t7, 0(t1)
    addiu  t5, t7, -1
    sw     t5, 0(t1)
    addiu  t1, t1, 4
    beq    t7,$0, SubBorrow
    nop

subDone:
    jr     ra
    nop

 .end _subBI


/***************************************************************************
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
;***************************************************************************/
    .global _zeroBI

 .ent _zeroBI

_zeroBI:
    
    la     t0, _xA
    lw     t0, 0(t0)           # Store the address present in __xA & increment it by 4
    
    la     t1, _iA            # Store the address present in __iA
    addiu  t0, t0, 4
    lw     t1, 0(t1)

zDoZero:
    sw     $0, 0(t1)          # The value of $0 register is always 0
    addiu  t1, t1, 4
    bne    t0, t1, zDoZero
    nop
    jr      ra
    nop

 .end _zeroBI


/***************************************************************************
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
;***************************************************************************/
    .global _msbBI

 .ent _msbBI

_msbBI:
    
    la     t2, _xA
    lw     t0, 0(t2)           # Store the address present in __xA & increment it by 4

    la     t1, _iA            # Store the address present in __iA
    lw     t1, 0(t1)

msbLoop:
    lw     t3, 0(t0)
    bne    t3, $0, msbDone
    nop
    addiu  t0, t0, -4
    bne    t0, t1, msbLoop
    nop

msbDone:
    sw     t0, 0(t2)
    jr     ra
    nop

 .end _msbBI


/***************************************************************************
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
; Note:			Function works. An Assumption has been made that 'R' is cleared to
;               'Zero' before calling this function.
;***************************************************************************/

    .global _mulBI

 .ent _mulBI

_mulBI:

    la     t0, _xA
    lw     t0, 0(t0)           # Store the address present in _xA & increment it by 4
    
    la     t1, _iA            # Store the address present in _iA
    addiu  t0, t0, 4
    lw     t1, 0(t1)
    
    la     t2, _xB            # Store the address present in _xB & increment it by 4
    lw     t2, 0(t2)

    la     t3, _iB            # Store the address present in _iB
    addiu  t2, t2, 4
    lw     t3, 0(t3)

    la     t4, _iR            # Store the address present in _iR
    lw     t4, 0(t4)

    la     a0, ONE_VALUE                    # Store 0x00000001 in a0

    addiu  t6, $0, 0                        # Store 0x0 in t6

mLoopB:
    beq  t3, t2, mDone
    lw   t7, 0(t3)

    beq  t7, $0, mContinue
    addu t8, t4, t6
    
    mthi  $0           # Move 0 to HI Register.

    addiu t5, t1, 0

mLoopA:
    mfhi  a1           # Store the contents from HI Register to 'a1' register
    lw    a3,0(t8)
    addu  a2, a1, a3
    mtlo  a2           # Move the contents from 'a1' register to LO register
    sltu  a1, a2, a1
    bgtz  a1, mHiPutOne
    nop
    b     mMultAdd
    mthi  $0

mHiPutOne:
    mthi  a0           # Move the contents from 'a0' register to HI register

mMultAdd:
    lw    a1, 0(t5)
    maddu a1, t7       # Multiply a1 with t7 then add with HI || LO register ( i.e HI || LO + a1 * t7 )
    addiu t5, t5, 4
    mflo  a3           # Store the contents from LO Register to 'a3' register
    sw    a3,0(t8)
    addiu t8, t8, 4
    bne   t5,t0, mLoopA
    nop
    mfhi  a1           # Store the contents from HI Register to 'a1' register
    sw    a1, 0(t8)    # Move the contents from 'a1' register to LO register

mContinue:
    addiu t3, t3, 4
    b     mLoopB
    addiu t6, t6, 4

mDone:
    jr      ra
    nop

 .end _mulBI


/***************************************************************************
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
;***************************************************************************/
	.global	_sqrBI

 .ent _sqrBI

_sqrBI:

    la     t0, _xA
    lw     t0, 0(t0)           # Store the address present in _xA & increment it by 4
    
    la     t1, _xB            # Store the address present in _xB & increment it by 4
    sw     t0, 0(t1)

    la     t0, _iA            # Store the address present in _iA
    lw     t0, 0(t0)
    
    la     t1, _iB            # Store the address present in _iB

    b      _mulBI
    sw     t0, 0(t1)

 .end _sqrBI

/***************************************************************************
; Function: 	void _masBI()
;
; PreCondition: _iB is loaded with the LSB of the modulus BigInt
;				_xB is loaded with the MSB of the modulus BigInt
;				_wC is loaded with the 32 bit unsigned integer by which to multiply
;				_iR is the starting LSB of the decumulator BigInt
 .end __sqrBI
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
;***************************************************************************/
    .global _masBI

 .ent _masBI

_masBI:

    la     t2, _xB            # Store the address present in _xB & increment it by 4
    lw     t2, 0(t2)

    la     t3, _iB            # Store the address present in _iB
    addiu  t2, t2, 4
    lw     t3, 0(t3)

    la     t1, _wC            # Store the address present in _wC
    lw     t1, 0(t1)

    la     t4, _iR            # Store the address present in _iR
    lw     t4, 0(t4)

    la     a1, ZERO_VALUE
    
    subu   t8, t2, t3
    addiu  t8, t8, 4
    addiu  t9, t4, 0
    addiu  a0, $0, 1

masLoop:
    
    mthi   $0
    mfhi   t6

mMultandSub:
    mtlo   t6
    mthi   $0
    lw     t5, 0(t3)
    maddu  t5, t1
    nop
    mflo   t6
    
mCalc:
    move   a2, $0
    lw     t5, 0(t4)
    movz   a2, a1, t5
    subu   t5, t5, a1
    sltu   t7, t5, t6         # if ( t5 < t6 ) then ( t7 = 1 )
    subu   t5, t5, t6         # t5 = t5 - t6
    sw     t5, 0(t4)          # 4 bytes of data transfered from address specified by t4 to t5
    or     t7, t7, a2
    move   a1, t7
    addiu  t4, t4, 4
    addiu  t3, t3, 4
    bne    t3, t2, mMultandSub
    mfhi   t6
    move   a2, $0
    lw     t5, 0(t4)
    movz   a2, a1, t5
    subu   t5, t5, a1
    sltu   t7, t5, t6         # if ( t5 < t6 ) then ( t7 = 1 )
    subu   t5, t5, t6         # t5 = t5 - t6
    sw     t5, 0(t4)          # 4 bytes of data transfered from address specified by t4 to t5
    or     t7, t7, a2
    move   a1, t7
    addiu  t4, t4, 4
    beqz   a1, masDone

mSubBorrow:
    subu   t7, t4, t9
    beq    t7, t8, masDone
    lw     t7, 0(t4)
    addiu  t6, t7, -1
    sw     t6, 0(t4)
    addiu  t4, t4, 4
    beq    t7, $0, mSubBorrow
    nop

masDone:
    jr     ra
    nop
 
 .end _masBI


/***************************************************************************
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
;***************************************************************************/
    .global	_copyBI

 .ent _copyBI

_copyBI:

    la     t0, _xA
    lw     t0, 0(t0)           # Store the address present in __xA & increment it by 4
    
    la     t1, _iA            # Store the address present in __iA
    addiu  t0, t0, 4
    lw     t1, 0(t1)
    
    la     t2, _xB            # Store the address present in __xB & increment it by 4
    lw     t2, 0(t2)

    la     t3, _iB            # Store the address present in __iB
    addiu  t2, t2, 4
    lw     t3, 0(t3)

cLoop:
    lw     t4, 0(t3)
    sw     t4, 0(t1)
    addiu  t3, t3, 4
    beq    t3, t2, cZeroLoopTest
    addiu  t1, t1, 4
    bne    t1,t0, cLoop
    nop
    jr      ra
    nop

cZeroLoop:
    sw     $0, 0(t1)
    addiu  t1, t1, 4

cZeroLoopTest:
    bne    t1,t0, cZeroLoop
    nop

cend:
    jr      ra
    nop
 
 .end _copyBI

 .set reorder

#endif
