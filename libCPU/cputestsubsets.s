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
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED ?AS IS? WITHOUT WARRANTY OF ANY
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
;******************************************************************************
;   How to use this file:                                                     *
;   ======================                                                    *
;1. This file contains all the CPU self test software functions.              *
;2. The module is tested on processors dsPIC33FJ256GP710A, PIC24HJ256GP610A,  *
;   dsPIC33EP512MU810, PIC24EP512GU810, dsPIC33EP256GP506 and PIC24EP256GP206.*
;3. Each function will generate and return a 16-bit unique result.            *
;******************************************************************************

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
.include "../libCPU/cpuverification.inc"

;..............................................................................
;Global Declarations:
;..............................................................................
	.global _TestSubset_1
	.global _TestSubset_2
	.global _TestSubset_3
	.global _TestSubset_4
	.global _TestSubset_5
	.global _TestSubset_6
.ifdef __dsPIC33F
	.global _TestSubset_7
	.global _TestSubset_8
.endif
.ifdef __dsPIC33E
	.global _TestSubset_7
	.global _TestSubset_8
.endif

;==============================================================================
;Code Section in Program Memory
;==============================================================================

	.text                     ; Start of Code section
	.section .libTestCPU, code

;******************************************************************************
;
; Module: TestSubset_1()
; ======================
;*
;* Features tested:
;*      1.      Here all the move instructions are tested.
;*      2.      The addressing modes tested here are immediate,file register,
;*              register direct and indirect(with pre increment, post increment
;*              pre decrement,post decrement,literal offset,register offset)
;*      3.      Swap and exch instructions are tested.
;*      4.      Single word and double word instructions are tested.
;*      5.      Byte move instructions are also checked.
;*
;* Description:
;*      1.      All move instructions, swap and exch instructions are checked
;*              in this module.
;*      2.      Here the result of one instruction is used as source operand
;*              for the next instruction. Using this method the result of one
;*              instruction is made dependent on the previous instructions.
;*      3.      In order to generate a single 16 bit result some logical
;*              instructions(XOR,AND) are used.
;*      4.      The bit test and skip instructions are shifted from TestSubset4
;*              to here.
;*      5.      The final result is stored in w1 register.
;*
;******************************************************************************

_TestSubset_1:                    ; Start of TestSubset_1

;..............................................................................
; MOVE BLOCK:
;0xb29f is moved to w1.The value is so chosen that
;when it is moved to a file register and execution of
;'mov f' instruction from the file reg to same file
;reg affects the 'N' bit of status register.
;..............................................................................

   clr w2
   clr w7
   clr CpuTestRam
   clr CpuTestRam+2
   clr CpuTestRam+4
   clr CpuTestRam+6
   clr CpuTestRam+8
   clr CpuTestRam+10
   clr CpuTestRam+12

   mov #CpuTest1Data1,w1
   mov.b #CpuTest1Data2,w2
   mov w1,CpuTestRam
   mov CpuTestRam         ;Move contents of file reg 0x1100 to 0x1100 itself.This
                          ;affects the negative bit (N bit) of Status register
   mov SR,w5              ;Here the contents of SR (with N bit set) are shifted
                          ;to w5. 
   and #0x8,w5

   mov CpuTestRam ,wreg   
   mov [w2 - 0xfa],w3     ;w2 has 0x00fe. w2 - 0xfa = 0x0004. So contents from
                          ;0x0004(ie w2 reg) are moved to w3.
   mov.b [w3 - 0xf8],w2   ;w3 has 0x00fe. w3 - 0xf8 = 0x0006. So contents from
                          ;0x0006(ie w3 reg) are moved to w2.
   mov wreg,CpuTestRam+2 
   exch w0,w2             ;Exchange contents of w0 with w2
   mov wreg,CpuTestRam+4
   mov w2,[w5+0x4]        ;w5 contains 0x0008. Move contents of w2 to [0x000C].
                          ;0x000C is w6 register. So w2 is moved to w6.
   mov w5,w4              ;w5 is moved to w4 to be used as register offset in
                          ;subsequent instructions.
   mov.b w6,[w4+0x6]      ;w4 contains 0x0008. So byte move w6 to 0x000E will
                          ;write into w7.
   sl w5,w4	
   add #CpuTest1Data3,w4  ;This is done so that when we swap w4 we will get the
                          ;data ram address (0x1100) used here.
   swap w4                ;Swap the upper and lower bytes of w4. W4 will
                          ;contain 0x1100 after the execution of this
                          ;instruction.
   mov.d [w4++],w2
   mov.d w4,w0
   mov [--w5],[++w4]      ;Move contents from 0x0006 (ie w3) to 0x1106.
   mov.d w0,[++w4]        ;Move double word from w0,w1 to 0x110a,0x110c
                          ;respectively.
   mov.d [w4--],w2        ;Move double word from 0x110c,0x110a to w3,w2
                          ;respectively. w7 will have 0x1106 after post
                          ;decrement.
   mov  w5,w6             ;This is done to use w6 as register offset in
                          ;subsequent instructions.
   mov.d [w5--],w0        ;w5 will have 0x0006 before post decrement.
                          ;Move double word from 0x0006(w3),0x0004(w2) 
                          ;to w0,w1 respectively. w5 will have 0x0002(w1) 
                          ;after post decrement.

   add #CpuTest1Data3,w3  ;This is done so that when we swap w3 we will get the
                          ;data ram address (0x1100) used here.
   swap w3                ;Swap the upper and lower bytes of w3. w3 will now
                          ;have 0x1000.
   mov [w0+w6],[w3+w0]    ;w0,w3,w6 have 0x0008,0x1100,0x0006 respectively. So
                          ;w0+w6 =0x000E(w7) is moved to w3+w0=0x1108.
   mov.d [w4--],w6        ;Move double word from 0x1106,0x1108 to w6,w7
                          ;respectively. w4 will have 0x1102 after post decrement.
   mov.b w5,[w4]          ;Byte move w5 to 0x1102.
   mov   w2,w0            ;The results of above instructions are in w2,w1,w6,w7
                          ;and locations 0x1102,0x1104. So to use double word move,
                          ;w2 is moved to w0.
   mov.d [w4],w2          ;Move double word from 0x1102,0x1104 to w2,w3
                          ;respectively.
   mov.d w2,[w4--]        ;Move double word from w2,w3 to 0x1102,0x1104
                          ;respectively. w4 will have 0x08fe after post
                          ;decrement.

;..............................................................................
;Bit test and bit manipulation block
;..............................................................................

   bclr CpuTestRam+2,#1   ;File reg 0x1102 contains 0xb202. Clear the 2nd bit.
                          ;File reg 0x1102 will then contain 0xb200.
   bclr w1,#1             ;w1 contains 0x1106. Clearing the 2nd bit w1 will
                          ;have 0x1104.
   bset CpuTestRam+2,#3   ;Setting the 4th bit in file register.
   bset w1,#0             ;Setting the 1st bit in w1.
   btst CpuTestRam+2,#4   ;Bit test 5th bit in file register 0x1102 and the
                          ;complement is stored in Z bit of SR. This doesn't
                          ;alter the content of 0x1102. 0x1102 has 0xb208.
                          ;So here Z bit gets set and SR contains 0x0002.
   bsw w4,w1              ;Bit write complement of status bit Z to the bit in w4.
                          ;The position is indicated by four lsb of w1. w4,w1 have
                          ;0x08FE,0x1105 respectively. After execution w4 has 0x08DE.
   btg w0,#9              ;Bit toggle 10th bit in w0. So w0 will have 0x0B04
                          ;after execution.
   btst.c w0,#9           ;Bit test 10th bit in w0 and the value is stored in
                          ;C bit of SR. This doesn't alter the contents of w0.
                          ;w0 has 0x0B04.So here C bit gets set. SR contains
                          ;0x0003 (Z,C = 1).
   bsw.c w1,w4            ;Bit write status bit C to the bit specified in w1.
                          ;The position is indicated by four lsb of w4.w4,w1
                          ;have 0x08DE,0x1105 respectively. After execution w1
                          ;will have 0x5105.
   btg CpuTestRam+2,#6    ;Bit toggle 7th bit in file register 0x1102. So 0x1102
                          ;will have 0xB248 after execution.
   btst.c w0, w1          ;Bit test the specified bit in w0 and the value is
                          ;stored in C bit of SR. The position is given by the
                          ;four lsb of w1. This doesn't alter the contents of w0.
                          ;Here C bit gets cleared. SR contains 0x0002 (Z=1).
   btst.z w1,#14          ;Bit test 15th bit in w1 and the complement is stored
                          ;in Z bit of SR. This doesn't alter the contents of w1.
                          ;w1 has 0x5105. So here Z bit gets cleared. SR contains
                          ;0x0000 (Z,C = 0).

   btst.z w1, w0          ;Bit test the specified bit in w1 and the complement
                          ;of the result is stored in Z bit of SR. The position
                          ;is given by the four lsb of w0. This doesn't alter the
                          ;content of w1,w0. SR contains 0x0002 (Z = 1).
   btsts.c w4,#12         ;Bit test 13th bit in w4 and the result is stored in
                          ;C bit of SR. This alters the contents of w4. w4 has
                          ;0x18DE. Here C bit gets cleared since the 13th bit in
                          ;w4 is 0. SR contains 0x0002 (C = 0).
   btsts CpuTestRam+4,#10 ;Bit test 11th bit in file register 0x1104 and the
                          ;complement is stored in Z bit of SR. This alters the
                          ;content of 0x1104. 0x1104 has 0x00FE since 11th bit is
                          ;set. So here Z bit gets cleared as 11th bit in 0x1104
                          ;is one.SR contains 0x0002 (Z = 1).
    btsts.z w1,#13        ;Bit test 14th bit in w1 and the complement is stored
                          ;in Z bit of SR. This alters the content of w1. w1 has
                          ;0x7105 after execution. So here Z bit gets set since
                          ;the 14th bit in w1 is 0. SR contains 0x0002(Z = 1).
   fbcl w4,w2             ;First bit change from MSB is stored in w2. The result
                          ;is sign extended.
   ff1r w4,w3             ;First occurrence of 1 from right is checked for w4
                          ;and stored in w3 with sign extension.
   xor w2,w3,w5           ;XOR w2 with w3 and store the result in w5. This is
                          ;done to get a unique 16 bit result. Here two 16 bit
                          ;results are 'xor'ed to produce one 16 bit value.
   ff1l w5,w2             ;First occurrence of 1 from left is checked for w5 and
                          ;stored in w2 with sign extension.

;..............................................................................
;Bit compare and skip instructions
;..............................................................................

   btss CpuTestRam+2,#1   ;Bit test 2nd bit in file reg 0x1102 and skip the next
                          ;instruction if set. The value is so chosen that the
                          ;next instruction gets executed.
   btss CpuTestRam+2,#3   ;Bit test 4th bit in file reg 0x1102 and skip the next
                          ;instruction if set. The value is so chosen that the
                          ;next instruction gets skipped. So file register 0x1102
                          ;is not cleared.
   clr CpuTestRam+2

   btsc CpuTestRam+2,#6   ;Bit test 7th bit in file reg 0x1102 and skip the next
                          ;instruction if cleared. The value is so chosen that
                          ;the next instruction gets executed.
   btsc CpuTestRam+2,#1   ;Bit test 2nd bit in file reg 0x1102 and skip the next
                          ;instruction if cleared. The value is so chosen that
                          ;the next instruction gets skipped. So file register
                          ;0x1102 is not cleared.
   clr CpuTestRam+2

   btsc w4,#11            ;Bit test 12th bit in w4, skip the next instruction
                          ;if cleared. The value is so chosen that the next
                          ;instruction gets executed.
   btsc w4,#5             ;Bit test 6th bit in w4 and skip the next instruction
                          ;if cleared. The value is so chosen that the next
                          ;instruction gets skipped. So w4 is not cleared.
   clr w4

   btss w4,#10            ;Bit test 11th bit in w4 and skip the next
                          ;instruction if set. The value is so chosen that the
                          ;next instruction gets executed.
   btss w4,#7             ;Bit test 8th bit in w4 and skip the next instruction
                          ;if set. The value is so chosen that the next
                          ;instruction gets skipped. So w4 is not cleared.
   clr w4

;..............................................................................
;The result is available in w0,w1,w4,w5,0x1102,0x1104. So to consolidate the
;result into one 16 bit value we use XOR and ADD instructions. The final
;result will be available in w0.
;..............................................................................

   xor CpuTestRam+2,wreg
   xor CpuTestRam+4,wreg
   xor w1,w6,w3
   xor w3,w2,w5
   xor w5,w7,w3
   add w3,w0,w1          
   mov w1,w0

return                   ; End of TestSubset_1

;******************************************************************************
;                                                                             *
; Module: TestSubset_2()                                                      *
; ======================                                                      *
;                                                                             *
; Features tested:                                                            *
; ================                                                            *
;1. PSV and Table accesses from different sections(4 locations) of            *
;   program memory.                                                           *
;2. All bits of the Program memory Address Bus are toggled.                   *
;3. All bits of Data Memory Address Bus are toggled.                          *
;4. All bits of X Data Read and Write Buses are toggled.                      *
;5. CPU registers tested for read/write operations are w0-w15, SPLIM, TBLPAG, *
;   PSVPAG.                                                                   *
;6. Tested Read After Write(RAW) dependency.                                  *
;7. Tested NOP and NOPR instructions.                                         *
;                                                                             *
; Description:                                                                *
; ============                                                                *
;1. The result of this TestSubset is a unique 16-bit value which will         *
;   validate all the instructions tested in this TestSubset.                  *
;   In order to get the unique result, individual feature results are         *
;   added/subtracted to/from w0.                                              *
;2. This TestSubset is a generic function which will work for all dsPIC33F,   *
;   PIC24H, dsPIC33E and PIC24E family devices.                               *
;   Conditional assembly directives are used to satisfy various program       *
;   memory and data memory sizes.                                             *
;3. The locations in program memory and data memory are selected such that    *
;   all the possible bits of buses (X data read/write, Y data read and        *
;   program memory address) are toggled, irrespective of memory sizes.        *
;4. The program memory is filled at the locations 0x005554, 0x00AAAA,         *
;   0x029554 with data 0x00AA55, 0xFF55AA, 0x005555 respectively through      *
;   definitions in the CPUverification.inc file.                              *
;5. The locations in the user program memory are selected such that each bit  *
;   in the program memory address bus is toggled at least once.               *
;6. Tested PSV read accesses from the locations 0x005554 and 0x029554         *
;7. Tested Table read accesses from the locations 0x00AAAA and 0xFF0000       *
;8. Tested Table read byte accesses.                                          *
;9.Tested Table read access in the user and configuration space.              *
;10.The locations in the Data memory are selected such that each bit in the   *
;   Data memory address bus is toggled at least once.                         *
;   XAGU(R/W)-Bus: 0x0000-0x0020, 0x0032(TBLPAG), 0x0034(PSVPAG/DSRPAG),      *
;                  0x0044(CORCON), 0x1100, 0x1102, 0x1AAA, 0x1554,            *
;                  0x1555, 0x3000, 0x47FE, 0xD554, 0x9554(PSV access)         *
;   PSV read map to the Data memory locations: 0xD554, 0x9554.                *
;11.The data written to the Data memory locations is selected such that       *
;   each bit in the X Data Read and Write Bus is toggled at least once.       *
;   X-W Bus: 0x0001, 0x0054, 0x0055, 0x5555, 0xAAAA, 0xFFFF                   *
;   X-R Bus: 0x0001, 0x5554, 0x5555, 0xAAAA, 0xFFFF, 0xAA55 (PSV read data)   *
;12.Tested Read After Write(RAW) dependency. The stalls are observed in IDE   *
;   for the scenarios:                                                        *
;       Destination addressing mode     Source addressing mode                *
;   a.  Direct                          Indirect                              *
;   b.  Direct                          Indirect with modification            *
;   c.  Indirect with modification      Indirect                              *
;13.If the CPU has failed to add one instruction cycle delay when a RAW       *
;   condition occurs, the next pre-fetch instruction will take old data,      *
;   thus implicitly testing the RAW dependency detection feature.             *
;14.The above all features are tested using read/writes to                    *
;   the registers w0-w15, TBLPAG, PSVPAG/DSRPAG.                              *
;15.The testing of read/writes to the SPLIM register is explicitly done.      *
;16.Testing of NOP and NOPR instructions.                                     *
;17.PCL and PCH are captured before and after executing NOP and NOPR. If these*
;   instructions fails to increment the PC then the result will be affected.  *
;18.In order to get result independent of PCL and PCH, PCL and PCH are        *
;   subtracted first and later added to w0 during the testing of NOP and NOPR *
;19.The registers w8-w14, PSVPAG/DSRPAG, TBLPAG, SPLIM are stored on to the   *
;   stack at the start of TestSubset and retrieved at the end of TestSubset.  *
;                                                                             *
;******************************************************************************

_TestSubset_2:                    ; Start of TestSubset_2

;..............................................................................
;1. Saving of CORCON, SPLIM, PSVPAG/DSRPAG, TBLPAG, w8-w14 on to the stack
;..............................................................................
                                  ; Saving of SPLIM, PSVPAG/DSRPAG, TBLPAG,
                                  ; w8-w14 on to the stack
        push    CORCON
        push    SPLIM
.ifdef __dsPIC33F
        push    PSVPAG
.endif
.ifdef __PIC24H
        push    PSVPAG
.endif
.ifdef __dsPIC33E
        push    DSRPAG
.endif
.ifdef __PIC24E
        push    DSRPAG
.endif
        push    TBLPAG
        push    w8
        push    w9
        push    w10
        push    w11
        push    w12
        push    w13
        push    w14

;..............................................................................
;PSV access from the location 0x005554:
;1. The Program Space Visibility is enabled by setting the PSV bit(CORCON<2>) 
;   in dsPIC33F/PIC24H and by setting DSRPAG to a suitable value in dsPIC33E/PIC24E.
;2. As the Most Significant bit of the EA is '1', PSVPAG/DSRPAG is concatenated with
;   the lower 15-bits of EA to form a 23-bit program space address.
;3. The lower word 0xAA55 of 0x00AA55 is moved to w3.
;4. The RAW dependency is checked with the Destination addressing mode(Direct)
;   using w2, Source addressing mode(Indirect) using w2.
;5. Just to reiterate: In order to get the unique result, individual feature
;   results are added/subtracted to w0.
;6. PSVPAG/DSRPAG and w3 are added to the w0.
;7. Write to the PSVPAG/DSRPAG, w0, w1, w2, w3, are performed.
;8. Read from PSVPAG/DSRPAG, w0, w1, w2 and w3 are performed.
;9. The Data memory location 0xD554 is accessed through PSV.
;..............................................................................
.ifdef __dsPIC33F
        bset.b  CORCONL, #PSV     ; Enable PSV access
        mov     #psvpage(Label_1), w1
        mov     w1, PSVPAG
        mov     PSVPAG, WREG
        mov     #psvoffset(Label_1), w2
.endif
.ifdef __PIC24H
        bset.b  CORCONL, #PSV     ; Enable PSV access
        mov     #psvpage(Label_1), w1
        mov     w1, PSVPAG
        mov     PSVPAG, WREG
        mov     #psvoffset(Label_1), w2
.endif
.ifdef __dsPIC33E
        mov     #psvpage(Label_1), w1
        movpag  w1, DSRPAG
        mov     DSRPAG, WREG
        mov     #psvoffset(Label_1), w2
.endif
.ifdef __PIC24E
        mov     #psvpage(Label_1), w1
        movpag  w1, DSRPAG
        mov     DSRPAG, WREG
        mov     #psvoffset(Label_1), w2
.endif

        mov     [w2], w3          ; RAW dependency. CPU stall will occur
        add     w0, w3, w0        ; w3=0xAA55


;..............................................................................
;PSV access from the location 0x029554:
;1. The data at location 0x029554 in the program memory is accessed using PSV
;2. The lower word 0x5555 of 0x005555 is moved to w6.
;3. The RAW dependency is checked with the Destination addressing mode(Direct)
;   using w5, Source addressing mode(Indirect) using w5.
;4. w6 is subtracted from w0 and stored to w0.
;5. The Program Space Visibility is disabled by clearing the PSV bit(CORCON<2>) 
;   in the case of dsPIC33F/PIC24H.
;6. Write to the registers PSVPAG/DSRPAG, w0, w4, w5, w6 are performed.
;7. Read from w0, w4, w5, w6 are performed.
;8. The Data memory location 0x9554 is accessed through PSV.
;..............................................................................
.if (PROGRAM_FLASH_SIZE == 256) || (PROGRAM_FLASH_SIZE == 512)     
	.ifdef __dsPIC33F
        mov     #psvpage(Label_3), w4
        mov     w4, PSVPAG
        mov     #psvoffset(Label_3), w5
	.endif
	.ifdef __PIC24H
        mov     #psvpage(Label_3), w4
        mov     w4, PSVPAG
        mov     #psvoffset(Label_3), w5
	.endif
	.ifdef __dsPIC33E
        mov     #psvpage(Label_3), w4
        movpag  w4, DSRPAG
        mov     #psvoffset(Label_3), w5
	.endif
	.ifdef __PIC24E
        mov     #psvpage(Label_3), w4
        movpag  w4, DSRPAG
        mov     #psvoffset(Label_3), w5
	.endif
        mov     [w5], w6          ; RAW dependency. CPU stall will occur
        sub     w0, w6, w0        ; w6=0x5555
.endif

	.ifdef __dsPIC33F	
        bclr.b  CORCONL, #PSV     ; Disable PSV access
	.endif
	.ifdef __PIC24H	
        bclr.b  CORCONL, #PSV     ; Disable PSV access
	.endif

;..............................................................................
;Table read access from the location 0x00AAAA:
;1. The 8-bit TBLPAG register is concatenated with a 16-bit EA to arrive at a
;   full 24-bit program address space.
;2. Table byte reads are performed to ensure that the program memory address
;   Bus LSb is toggled.
;3. The lowest byte 0xAA of 0xFF55AA is moved to w7 and added to w0.
;4. The lower byte 0x55 of 0xFF55AA is moved to w7 and added to w0.
;5. The 0xFF of 0xFF55AA is moved to w4 and added to w0.
;6. The RAW dependency is checked with the Destination addressing mode(Direct)
;   using w8, Source addressing mode(Indirect with modification) using w8.
;7. Write to the TBLPAG, w0, w4, w7, w8 are performed.
;8. Read from TBLPAG, w0, w4, w7, w8 are performed.
;..............................................................................
        mov     #tblpage(Label_2), w7
	.ifdef __dsPIC33F	
        mov     w7, TBLPAG
	.endif
	.ifdef __PIC24H	
        mov     w7, TBLPAG
	.endif
	.ifdef __dsPIC33E	
        movpag  w7, TBLPAG
	.endif
	.ifdef __PIC24E	
        movpag  w7, TBLPAG
	.endif
        add     TBLPAG, WREG      ; TBLPAG is added to w0
        mov     #tbloffset(Label_2), w8
                                  ; Table read access byte wise
                                  ; post increment w0
        tblrdl.b [w8++], w7       ; RAW dependency. CPU stall will occur
        add     w0, w7, w0
        tblrdl.b [w8--], w7       ; Table read access
        add     w0, w7, w0
        tblrdh  [w8], w4          ; Table read access
        add     w0, w4, w0


;..............................................................................
;Table read access from the location 0xFF0000:
;1. Table read access in the configuration space.
;2. Move the upper two nibble address of program memory(0xFF) to TBLPAG.
;3. Move the lower word address of program memory(0x0000) to w9.
;4. The upper data byte is read to w5 through tblrdh instruction
;   and added to w0.
;5. The lower word is read to w6 through tblrdl instruction and added to w0.
;6. The table read of location 0xFF0000 will give Device ID. It is specific to
;   the device which will give us the information of mask and variant number.
;7. Write to the TBLPAG, w0, w5, w6, w9 are performed.
;8. Read from TBLPAG, w0, w5, w6 and w9 are performed.
;..............................................................................
        mov     #CpuTest2Data1, w9
	.ifdef __dsPIC33F	
        mov     w9, TBLPAG
	.endif
	.ifdef __PIC24H	
        mov     w9, TBLPAG
	.endif
	.ifdef __dsPIC33E	
        movpag  w9, TBLPAG
	.endif
	.ifdef __PIC24E	
        movpag  w9, TBLPAG
	.endif
        add     TBLPAG, WREG      ; TBLPAG is added to w0
        mov     #CpuTest2Data2, w9
                                  ; Table read access
        tblrdh  [w9], w5          ; RAW dependency. CPU stall will occur
        add     w0, w5, w0
        tblrdl  [w9], w6          ; Table read access
        add     w0, w6, w0


;..............................................................................
;Write data to the Data memory through X Bus:
;1. The values 0x5555, 0xAAAA, 0x0001, 0xFFFF, 0x1AAA, 0x1554, 0x3000, 0x47FE
;   are written to the registers w10, w11, w1, w4, w12, w13, w14 and w8
;   respectively depending on the data memory sizes.
;2. The values 0x5555, 0x5554 are written to data memory locations 0x1AAA and
;   0x1554 irrespective of data memory size. Byte writes to data memory are
;   also performed.
;3. The value 0x0001 is written to data memory location 0x3000 if the
;   data memory size is either 16 or 28/30/32 Kbytes.
;4. The value 0xFFFF is written to data memory location 0x47FE if the
;   data memory size is 28/30/32 Kbytes.
;5. The locations in the data memory are selected such that all the possible
;   bits of buses(XAGU-R/W, X-R/W) are toggled, irrespective of memory sizes
;6. The values which are written to Data memory locations are read and
;   added/subtracted to the w0.
;7. Write to w0, w1, w4, w8, w10, w11, w12, w13 and w14 are performed.
;8. Read from w0, w1, w4, w8, w10, w12, w13 and w14 are performed.
;..............................................................................
        mov     #CpuTest2Data3, w10
        mov     #CpuTest2Data4, w11
.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     #CpuTest2Data5, w1
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     #CpuTest2Data6, w4
.endif
        mov     #CpuTestRam1, w12
        mov     #CpuTestRam2, w13
.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     #CpuTestRam3, w14
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     #CpuTestRam4, w8
.endif
        mov.b   w13, [w13++]
        mov     w10, [w12]
        mov.b   w13, [w13--]
.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     w1, [w14]
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     w4, [w8]
.endif
        add     w0, [w12], w0
        add     w0, [w13], w0
.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        add     w0, [w14], w0     ; 0x3000 is added to w0
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        sub     w0, [w8], w0      ; 0x47FE location contents are added to w0
.endif

;..............................................................................
;RAW dependency checking:
;1. 0xFFFF is written to the location 0x1102.
;2. w10=0x5555, w11=0xAAAA which are added and written to the location 0x1100.
;   The w9 is post incremented.
;3. The data memory location 0x1102 is read to w14 which is added to w0.
;4. If CPU has not introduced stall then w11=0x1100 location value is read,
;   which will affect the result.
;5. The RAW dependency is checked with the Destination addressing mode
;   (Indirect with modification)using w9, Source addressing mode(Indirect).
;6. 0xFFFE is written to SPLIM and SPLIM is read and value is added to w0.
;7. Write to SPLIM, w0, w9, and w14 are performed.
;8. Read from SPLIM, w0, w4, w9, w10, w11 and w14 are performed.
;..............................................................................
        mov     #CpuTestRam, w9
        mov     w11, CpuTestRam+2 ; 0xAAAA is written to the location 0x1102
        add     w10, w11, [w9++]  ; 0xFFFF is written to the location 0x1100
                                  ; post-increment w9
        mov     [w9], w14         ; 0xAAAA is moved to w14 RAW dependency.
                                  ; CPU stall will occur
        sub     w0, w14, w0
                                  ; 0xFFFE is written to SPLIM
        setm    SPLIM
        add     SPLIM, WREG       ; SPLIM is read and added to w0

;..............................................................................
;1. Testing of NOP and NOPR instructions.
;2. PCL and PCH are captured before and after executing NOP and NOPR. If these
;   instructions fails to increment the PC then the result will be affected.
;3. Inorder to get result independent of PCL and PCH, PCH and PCL are
;   subtracted first and later added to w0 during the testing of NOP and NOPR
;..............................................................................
        mov     PCH, w8
        sub     w0, w8, w0
        mov     PCL, w9
        sub     w0, w9, w0
        nop
        nopr
        add     PCL, WREG
        add     PCH, WREG

                                  ; To validate writing to the same location
                                  ; when the same TestSubset is called once
                                  ; again a different value is written into it.
                                  ; Locations 0x1AAA and 0x1554 will be
                                  ; modified by other TestSubsets.
.if (RAM_SIZE >= 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     w0, CpuTestRam3   ; CpuTestRam3 = 0x3000
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        mov     w0, CpuTestRam4   ; CpuTestRam4 = 0x47FE
.endif

        pop     w14
        pop     w13
        pop     w12
        pop     w11
        pop     w10
        pop     w9
        pop     w8
        pop     TBLPAG
.ifdef __dsPIC33F
        pop     PSVPAG
.endif
.ifdef __PIC24H
        pop     PSVPAG
.endif
.ifdef __dsPIC33E
        pop     DSRPAG
.endif
.ifdef __PIC24E
        pop     DSRPAG
.endif
        pop     SPLIM
        pop     CORCON

return                            ; End of TestSubset_2

;******************************************************************************
;                                                                             *
; Module: TestSubset_3()                                                      *
; ======================                                                      *
;                                                                             *
; Features Tested:                                                            *
; =================                                                           *
;1. All conditional branch and GOTO instructions with alternative conditions. *
;2. Program Counter behavior during above program flow change operations.     *
;3. All function call and return operations.                                  *
;4. Automatic context save on stack                                           *
;5. All Stack and Shadow operations                                           *
;6. DISI instruction.                                                         *
;                                                                             *
; Description:                                                                *
; ============                                                                *
;1. The result of this TestSubset is a unique 16-bit value which will         *
;   validate all the instructions tested in this TestSubset.                  *
;2. Testing of DISI Instruction:                                              *
;   a) Set the DISICNT to 15 (dsPIC33F/PIC24H) or 19 (dsPIC33E/PIC24E).       *
;   b) To make sure that DISICNT is loaded correctly, add the value to WREG.  *
;      All the intermediate feature results are added/subtracted to WREG to   *
;      make the result unique. Addition is performed here to make sure that   *
;      SR register bits DC,N,OV,Z and C comes to known values '0'.            *
;      These bits will be used for next coming branch instructions.           *
;   c) To make sure that DISI bit in INTCON2 is set, move the value of        *
;      INTCON2 to w2. The value stored in w2 will be used after pop.s         *
;      instruction in this module to modify the result.                       *
;   d) To check the Interrupt-disabling functionality INT0 interrupt is       *
;      enabled and flag bit is set before the DISCNT decrements to zero.      *
;   e) Use of alternate vector table is set in the INTCON2 to avoid           *
;      interfering with the user's application's interrupt vectors.           *
;   f) In the Interrupt Service Routine for INT0 the w0 is added with some    *
;      constant value to affect the result. The interrupt flag bit is also    *
;      cleared.(of course, the ISR will be serviced only when DISI feature    *
;      fails.                                                                 *
;   g) Just before the DISICNT is expected to decrement to zero,              *
;      the interrupt flag and enable bits are cleared.                        *
;3. w1 and w3 are loaded with values 0xF4 and 0x03 respectively.              *
;4. Now push.s will save all the values of w0-w3 and SR bits (DC,N,OV,Z,C)    *
;   Here w0=0x0F, w1=0xF4, w2=0xC000, w3=0x03, SR bits DC,N,OV,Z,C all '0'.   *
;5. These values will be retrieved with pop.s instruction in the function     *
;   _CALL_2 and the same values will be used to modify the result.            *
;6. All branch instructions are tested except branch on OA,OB,SA,SB which     *
;   will be tested in TestSubset_7.                                           *
;7. If branch succeeds then it will move to the next branch otherwise it will *
;   modify the running 16-bit result by doing addition/subtraction to w0.     *
;8. Tested alternate conditions for branches.(branch on TRUE/FALSE).          *
;9. If branch on false condition fails the program flow is moved to the END3  *
;   to avoid infinite loop in branch.                                         *
;                                                                             *
;10.Unconditional Jump and Unconditional indirect Jump are tested using goto  *
;   instructions. If it fails it will modify the running 16-bit result.       *
;11.The program counter behavior  during the program flow change is captured  *
;   by adding the PCL and PCH to w0.                                          *
;12.In order to get result independent of PCL, PCL is subtracted first and    *
;   later added to w0 during the program flow change.                         *
;                                                                             *
;13.All function calls and returns are tested. (Relative call, Computed       *
;   Relative call, Call Subroutine, Call indirect Subroutine, return, retlw)  *
;14.In Relative call(_RCALL_1), the retlw is used to return a constant value  *
;   to w5 which will be added to the running 16-bit result.                   *
;   This will validate Relative call and retlw.                               *
;15.In all other calls the running 16-bit result is modified with manipulation*
;16.w0-w3 and SR bits(DC,N,OV,Z,C) are modified atleast once before being     *
;   retrieved with pop.s instruction.                                         *
;                                                                             *
;17.In Call indirect Subroutine(_CALL_1) automatic context save on            *
;   stack is tested. This is done by moving stack pointer(w15) value to w7    *
;   and retrieved values  of PCH and PCL which are added to w0.               *
;18.The LNK and ULNK are also tested the Call indirect Subroutine(_CALL_1).   *
;   The difference  of w15 and w7 is a constant, which is because of LNK      *
;   and is added to w0. similarly after ULNK.                                 *
;                                                                             *
;19.In Call Subroutine(_CALL_2) all push and pop instructions are tested.     *
;20.w0(Running 16-bit result) is saved on to the stack.                       *
;21.pop.s will retrieve all w0-w3 registers and SR(DC,N,OV,Z,C) bits.         *
;   i.e w0=0x0F, w1=0xF4, w2=0xC000, w3=0x03, SR bits DC,N,OV,Z,C all '0'.    *
;   The w0-w3 and SR bits will be read by some means and modified             *
;   to affect the result which will validate push.s and pop.s.                *
;   w0 will validate DISICNT, w2 will validate INTCON2 bits.                  *
;22.The running 16-bit result is saved to location 0x1100 from                 *
;   the stack(push w0).                                                       *
;23.The SR bits are and'ed with w0(which will validate SR bits and DISICNT).  *
;24.The running 16-bit result value is saved on to the stack from             *
;   the location 0x900. Next it is saved to w6.(If w6 contains correct running*
;   16-bit result then it will validate 'push ws, pop f, push f, pop wd').    *
;   The w0 and w6 are added to w0. Now w0 contains the running 16-bit result. *
;25.push.D and pop.D are used to the read the contents of w2:w3               *
;   which are saved to w4:w5.                                                 *
;26.w4 and w5 are added to w0 which will validate push.D and pop.D.           *
;27.The running 16-bit result is written to SPIBUF.                           *
;                                                                             *
; Returns: Nil                                                                *
; ========                                                                    *
;                                                                             *
; Global Data: Nil                                                            *
; ============                                                                *
;                                                                             *
; Arguments: Nil                                                              *
; ==========                                                                  *

;                                                                             *
;******************************************************************************

_TestSubset_3:                    ; Start of TestSubset_3

;..............................................................................
;1. Save IFS0, IEC0, INTCON2, DISICNT onto the stack
;..............................................................................
        push    IFS0              ; saving IFS0, IEC0, INTCON2, DISICNT onto
        push    IEC0              ; the stack
        push    INTCON2
        push    DISICNT

;..............................................................................
;1. Clear w0 (To make sure that w0 indeed contains nothing before we add
;   DISICNT.)
;2. Alternate vector table(ALTIVT) bit is set in INTCON2 (dsPIC33F/PIC24H only)
;3. INT0 interrupt is enabled by setting bit in IEC0<0>
;4. DISI instruction is executed with immediate operand as #0x0F. So for the
;   next 15 instructions all the interrupts with priority <7 will be blocked.
;5. To make sure that DISICNT is loaded with value 0xF, the DISICNT is added
;   to WREG. This addition make sure that SR register bits DC,N,OV,Z and C
;   comes to known values '0'.
;6. To make sure that DISI bit in INTCON2 is set, move the value of
;   INTCON2 to w2. The value stored in w2 will be used after pop.s
;   instruction in this module to modify the result.
;7. To check the Interrupt-disabling functionality INT0 interrupt is generated
;   by setting the flag bit before the DISCNT decrements to zero.
;8. w1 and w3 are loaded with values 0xF4 and 0x03 respectively.
;9. Now push.s will save all the values of w0-w3 and SR bits (DC,N,OV,Z,C)
;   Here w0=0x0F, w1=0xF4, w2=0xC000, w3=0x03, SR bits DC,N,OV,Z,C all '0'.
;10.These values will be retrieved with pop.s instruction in the function
;   _CALL_2 and the same values will be used to modify the result.
;..............................................................................
        clr     WREG              ; clear wreg
        bset    INTCON2, #15      ; Use alternate vector table in the case of dsPIC33F/PIC24H
								  ; Global interrupt enable in the case of dsPIC33E/PIC24E	
        bset    IEC0, #0          ; Interrupt request INT0 is enabled
        disi    #CpuTest3Data1    ; Block all interrupts with priority < 7
                                  ; for next 15 (dsPIC33F/PIC24H) or 19 (dsPIC33E/PIC24E) instructions.
        add     DISICNT, WREG
        mov     INTCON2, w2
        bset    IFS0, #0          ; set the bit5 in IFS0 to generate
                                  ; INT0 interrupt and should not be serviced
                                  ; because DISI is active
        mov     #CpuTest3Data2, w1
        mov     #CpuTest3Data3, w3
        push.s                    ; save w0-w3 and SR- DC,N,OV,Z,C bits

;..............................................................................
;1. SR bits: DC=0, N=0, OV=0, Z=0, C=0.
;2. Tested for Branch on TRUE conditions: GE, LTU, NOV, NZ.
;3. Tested for Branch on FALSE conditions: GTU, LE, OV, Z.
;4. If branch on false condition fails the program flow is moved to the END3
;   to avoid infinite loop in branch.
;5. If any branch fails it will modify the running 16-bit result.
;6. Just before the DISICNT is expected to decrement to zero, the interrupt
;   flag and enable bits are cleared.
;..............................................................................
        bra     L_UNCONDITIONAL_3 ; Branch Unconditionally
                                  ; skips if branch succeeds
        add     w0, #CpuTest3Data4, w0
L_UNCONDITIONAL_3:
                                  ; Computed branch
                                  ; w3=0x0003, so PC=(PC+2)+(2*w3)->PC
        bra     w3                ; PC goes to L_COMPUTED_BRANCH_3
                                  ; skips if branch succeeds
        add     #CpuTest3Data5, w0
L_FALSE_3:
                                  ; If branch on false condition fails
                                  ; the program flow is moved to the END3
                                  ; to avoid infinite loop in branch.
        add     #CpuTest3Data6, w0; result will be affected
        bra     END_3
L_COMPUTED_BRANCH_3:
        bclr    IFS0, #0          ; clear the interrupt
        bclr    IEC0, #0          ; disable the interrupt

        bra     GE, L_GE_3        ; branch if greater than or equal
                                  ; skips if branch succeeds
        add     #CpuTest3Data7, w0
L_GE_3:
        bra     GTU, L_FALSE_3    ; branch if unsigned greater than
        bra     LTU, L_LTU_3      ; branch if unsigned less than
                                  ; skips if branch succeeds
        add     #CpuTest3Data8, w0
L_LTU_3:
        bra     LE, L_FALSE_3     ; branch if less than
        bra     NOV, L_NOV_3      ; branch if not Overflow
                                  ; skips if branch succeeds
        add     #CpuTest3Data9, w0
L_NOV_3:
        bra     OV, L_FALSE_3     ; branch if Overflow
        bra     NZ, L_NZ_3        ; branch if not Zero
                                  ; skips if branch succeeds
        add     #CpuTest3Data10, w0
L_NZ_3:
        bra     Z, L_FALSE_3      ; branch if Zero

;..............................................................................
;1. w1=0xF4 is decremented by 1 which will cause to change the SR bits.
;2. SR bits: DC=1, N=1, OV=0, Z=0, C=1.
;3. Tested for Branch on TRUE conditions: C, GEU, LT, N.
;4. Tested for Branch on FALSE conditions: NC, GT, LEU, NN.
;5. If any branch fails it will modify the running 16-bit result.
;..............................................................................
        dec.b   w1, w1            ; DC=1, N=1, OV=0, Z=0, C=1
        bra     C, L_C_3          ; branch if carry
                                  ; skips if branch succeeds
        add     #CpuTest3Data11, w0
L_C_3:
        bra     NC, L_FALSE_3     ; branch if not carry
        bra     GEU, L_GEU_3      ; branch if Unsigned greater than or equal
                                  ; skips if branch succeeds
        add     #CpuTest3Data12, w0
L_GEU_3:
        bra     GT, L_FALSE_3     ; branch if greater than
        bra     LT, L_LT_3        ; branch if less than
        add     #CpuTest3Data13, w0         ; skips if branch succeeds
L_LT_3:
        bra     LEU, L_FALSE_3    ; branch if unsigned less than or equal
        bra     N, L_N_3          ; branch if negative
                                  ; skips if branch succeeds
        add     #CpuTest3Data14, w0
L_N_3:
        bra     NN, L_FALSE_3     ; branch if not negative

;..............................................................................
;1. Unconditional Jump makes the PC to goto _THERE_1.
;2. The program counter behavior during the program flow change is captured
;   by adding the PCL and PCH to w0.
;3. Inorder to get result independent of PCL, PCL is subtracted first and later
;   added to w0 during the program flow change.
;..............................................................................
        mov     PCL, w4           ; PCL is subtracted from w0
        sub     w0, w4, w0
        goto    _THERE_1
                                  ; skips if Unconditional jump succeeds
        add     #CpuTest3Data15, w0
_THERE_1:
        add     PCH, WREG         ; PC is captured to see the behavior
        add     PCL, WREG         ; during program flow change.
                                  ; PCL is added to w0

;..............................................................................
;1. Call the Sub routine _RCALL_1 using Relative Call.
;2. The retlw is used to return a constant value to w5
;   which will be added to the running 16-bit result i.e w0.
;3. Unconditional indirect jump is tested here.
;..............................................................................
        rcall   _RCALL_1          ; Relative Call
        add     w5, w0, w0        ; The w5=0x57 is added to w0
                                  ; Computed Relative Call
                                  ; w3=0x0003,so PC=(PC+2)+(2*w3)->PC
        rcall   w3                ; PC goes to _RCALL_2

                                  ; move the lower program memory address of
                                  ; _THERE_2 to w2
        mov     #tbloffset(_THERE_2), w2
        goto    w2                ; goto address indirectly to _THERE_2
                                  ; skips if Unconditional indirect-
                                  ; jump succeeds
        add     #CpuTest3Data16, w0
_RCALL_2:
                                  ; Modify the running 16-bit result.
        add     #CpuTest3Data17, w0
        return                    ; PC will return to the next instruction
                                  ; after rcall w3

;..............................................................................
;1. Call the sub routine _CALL_1 using 'Call indirect Subroutine'
;2. Call the sub routine _CALL_2 using 'Call Subroutine'
;3. Inorder to get result independent of PCL, PCL is subtracted first and later
;   added to w0 during automatic context save on stack.
;..............................................................................
_THERE_2:
                                  ; move the lower program memory address of
                                  ; _CALL_1 to w3
        mov     #tbloffset(_CALL_1), w3
        mov     PCL, w4           ; PCL is subtracted from w0
        sub     w0, w4, w0
        call    w3                ; PC will move to _CALL_1
        call    _CALL_2           ; PC will move to _CALL_2

; Retrieving DISICNT, INTCON2, IEC0, IFS0 from the stack
END_3:
        pop     DISICNT
        pop     INTCON2
        pop     IEC0
        pop     IFS0

return                            ; return of TestSubset_3

;..............................................................................
;Sub Routine Section for TestSubset_3:
;..............................................................................
;..............................................................................
;1. The automatic context save on stack is tested.
;2. Move address pointer value to w7.
;3. With the help of w7 read the PCH and PCL which are saved on stack.
;4. Link frame pointer. Allocate 64 bytes.
;5. The LNK and ULNK are tested w.r.t w15.
;6. The saving/retrieving of w14 to/from stack will increment/decrement w15
;   irrespective of contents of w14.
;7. The value w7 is subtracted from w15 to get a constant which validates LNK,
;   is added to w0.
;8. Similarly ULNK will affect w15. once again w7 is subtracted from w15 to
;   get a constant which validates ULNK, is added to w0.
;..............................................................................
_CALL_1:
        mov     w15, w7
        lnk     #CpuTest3Data19   ; link frame pointer (0x40)
                                  ; To avoid stall after one instruction
        add     w0, [--w7], w0    ; PCH and PCL are read and added to w0
        add     w0, [--w7], w0
        sub     w15, w7, w5
        add     w0, w5, w0
        ulnk                      ; Unlink frame pointer
        sub     w15, w7, w5
        add     w0, w5, w0
        return

;..............................................................................
;1. w0 contains the running 16-bit result which is saved on stack.
;2. w0-w3 and SR bits(DC,N,OV,Z,C) are modified at least once before being
;   retrieved with pop.s instruction.
;3. pop.s will retrieve all w0-w3 registers and SR(DC,N,OV,Z,C) bits.
;   i.e w0=0x0F, w1=0xF4, w2=0xC000, w3=0x03, SR bits DC,N,OV,Z,C all '0'.
;   The w0-w3 and SR bits will be read by some means and used
;   to affect the result will which will validate push.s and pop.s.
;   w0(0xF) will validate DISICNT, w2(C000) will validate INTCON2 bits.
;4. The running 16-bit result is saved to location 0x1100 from
;   the stack(push w0).
;5. The SR bits are and'ed with w0(which will validate SR bits and DISICNT).
;   w0 contains partial results which validates SR and w1 after pop.s
;6. The running 16-bit result value is saved on to the stack from
;   the location 0x1100. Next it is saved to w6 (if w6 contains correct running
;   16-bit result then it will validate 'push ws, pop f, push f, pop wd').
;   The w0 and w6 are added to w0. Now w0 contains the running 16-bit result.
;7. push.D and pop.D are used to the read the contents of w2:w3,
;   which are saved to w4:w5.
;8. w4 and w5 are added to w0 which will validate push.D and pop.D
;   and w2, w3 after pop.s
;..............................................................................
_CALL_2:
        push    w0                ; save running 16-bit result on stack

        pop.s                     ; Will retrieve all w0-w3 and SR(DC,N,OV,Z,C)
                                  ; save running 16-bit result to
        pop     CpuTestRam        ; the location 0x1100
        and     SR, WREG          ; w0=0xF, SR=0x0
        push    CpuTestRam        ; save running 16-bit result to stack
        add     w1, w0, w0        ; w1 is added to w0.
        pop     w6                ; To avoid stall, after one instruction
                                  ; result is moved to w6.
        add     w0, w6, w0        ; w0 contains partial results which validates
                                  ; SR and w1 after pop.s
        push.D  w2
        pop.D   w4                ; CPU stall RAW detection destination
                                  ; indirect with modifications and
                                  ; source indirect with modification
        add     w4, w5, w4
        add     w0, w4, w0
        return

;..............................................................................
;1. The retlw is used to return a constant value to w5 which will be later
;   added to the running 16-bit result.
;   This will validate Relative call and retlw.
;..............................................................................
_RCALL_1:
                                  ; 0x57 is moved to w5 which will be added to
                                  ; w0 in the caller function
        retlw   #CpuTest3Data20, w5

                                  ; End of TestSubset_3

;******************************************************************************
;*
;* Module: TestSubset_4()
;*
;* Features covered:
;*      1.      Here all the logical, data rotate and shift,compare,compare and
;*              skip instructions are tested.
;*      2.      The addressing modes tested here are immediate,file register,
;*              register direct and indirect(with pre increment)
;*      3.      Byte instructions are also checked.
;*      4.      Branch instructions tested for false condition are NOV,Z,NN,
;*              NC and NZ.
;*
;* Description:
;*      1.      All logic instructions: AND,CLR,COM,IOR,NEG,SETM and XOR
;*              instructions are checked.
;*      2.      All data rotate and shift instructions are checked.
;*      3.      All compare, compare and skip instructions are checked.
;*      4.      Z,OV,DC,N,C bits of the status register are checked for their
;*              behaviour.
;*      5.      Here the result of one instruction is used as source operand
;*              for the next instruction. Using this method the result of one
;*              instruction is made dependent on the previous instructions.
;*      6.      In order to generate a unique 16 bit result some logical
;*              instructions (XOR,AND,CLR) are shifted from their original
;*              block to some other block like IOR ..etc.
;*      7.      To show the behaviour of status bits, some branch instructions
;*              are performed.
;*      9.      The bit test and skip instructions are shifted to TestSubset_1.
;*      10.     The final result is 0x00F8 and is stored in w6 register.
;*
;******************************************************************************

 _TestSubset_4:                   ; Start of TestSubset_4


;Initially 0x1100 is loaded to W1. The data ram address 0x1100,W0,W2 is made
;0xFFFF using SETM instruction.
   clr w3
   mov #CpuTestRam,W1
   setm CpuTestRam
   setm WREG
   setm W2
;..............................................................................
;AND block
;Here few other instructions SL,AND,XOR are also used to introduce
;interdependency of the result with all previous instructions.
;..............................................................................

   and #CpuTest4Data1,W2
   and.b W2,#CpuTest4Data2,W3
   and W0,W3,[++W1]
   xor CpuTestRam +2
   sl CpuTestRam +2
   and CpuTestRam +2,WREG
;..............................................................................
;IOR block
;Here few other instructions SL,CLR are used to introduce
;interdependency of the result with all previous instructions.
;..............................................................................

   clr CpuTestRam +2
   ior CpuTestRam +2
   sl  CpuTestRam +2,WREG
   ior CpuTestRam +2,WREG
   ior.b #CpuTest4Data3,W0
   ior W0,W1,w4
   ior W4,#CpuTest4Data4,W3

;..............................................................................
;XOR block
;Here few other instructions CLR,AND are also used to introduce
;interdependency of the result with all previous instructions.
;..............................................................................

   clr  WREG
   xor #CpuTest4Data1,W0
   xor W0,#CpuTest4Data5,W4
   and CpuTestRam
   xor CpuTestRam ,WREG
   xor W0,W4,[W1]     ;here W1 contains 0x1102.So the value is written to data
                      ;ram address 0x1102 which is used in next instruction.
;..............................................................................
;ROTATE block
;..............................................................................

   rrc CpuTestRam +2
   rrc CpuTestRam +2,WREG
   rrc w0,[w1]        ;here W1 contains 0x1102.So the value is written to data
                      ;ram address 0x1102 which is used in next instruction.

   rrnc CpuTestRam +2
   rrnc CpuTestRam +2,WREG
   rrnc w0,[w1]

   rlc CpuTestRam +2
   rlc CpuTestRam +2,WREG
   rlc w0,[w1]

   rlnc CpuTestRam +2
   rlnc CpuTestRam +2,WREG
   rlnc w0,w5

;..............................................................................
;COM block
;..............................................................................
   com CpuTestRam +2
   com CpuTestRam +2,WREG
   com W0,[W1]
;..............................................................................
;NEG block
;..............................................................................
   neg CpuTestRam +2
   neg CpuTestRam +2,WREG
   neg w0,[w1]
;..............................................................................
;SHIFT block
;..............................................................................
   lsr CpuTestRam +2
   lsr CpuTestRam +2,WREG
   lsr W0,W6
   asr W5,#7,W7
   asr W6,W7,W2

   asr CpuTestRam +2
   asr CpuTestRam +2,WREG
   asr W0,W6
   sl w6,#3,w7
   sl w7,w6
   lsr W6,#13,W7
   lsr W5,W7,W6
   lsr W6,#4,W5
   sl w2,w5,w0
   clr  W5

;..............................................................................
;COMPARE block
;1.Here all the instructions change status register bits.To have the result
;  dependent on these instructions the status register value is added to
;  WREG recursively whenever SR bits gets updated.Thus the result in W0
;  is linked to the execution of all compare instructions.
;2.The address location 0x042 refers to status register.
;..............................................................................
   cp.b w1,w7
   btsc SR,#8
   cp CpuTestRam +2
   bra NOV,L_SPI_4    ;Here OV =1. SO no branch occurs
   cp0 SR             ;0x042 refers to status register SR.This instruction
                      ;compares SR - 0x0000.Since previous instruction clears
                      ;the SR ,so the result of compare is true.
   bra Z,L_SPI_4
   cpb.b w7,#CpuTest4Data6
   bra NN,L_SPI_4
   cpb CpuTestRam     ;after the execution of this instruction makes the
                      ;status register all zeros.
   bra NOV,L_SPI_4


   cp0 w5
   bra NZ,L_SPI_4
   cp w2,#CpuTest4Data7
   bra NC,L_SPI_4
   cpb w1,w5
   btsc SR,#8

;..............................................................................
;COMPARE and SKIP block
;1)The operands for the instructions are chosen such that after execution
;  if the condition is true it skips ADD instructions.
;2)The result of this block is 0x7A5C and is stored in W2.
;..............................................................................
   cpseq.b w1,w7      ;The values are such that on execution the next
                      ;instructions gets skipped.
   add #CpuTest4Data8,w0
   cpseq w6,w5        ;The values are such that the next instruction doesnt
                      ;get skipped
   cpsgt w1,w4        ;The values are such that on execution the next
                      ;instructions gets skipped.
   add #CpuTest4Data8,w0
   cpsgt w2,w4        ;The values are such that the next instruction doesnt
                      ;get skipped
   cpslt w4,w1        ;The values are such that on execution the next
                      ;instructions gets skipped.
   add #CpuTest4Data8,w0
   cpslt w4,w2        ;The values are such that the next instruction doesnt
                      ;get skipped
   cpsne w2,w3        ;The values are such that on execution the next
                      ;instructions gets skipped.
   add #CpuTest4Data8,w0
   cpsne.b w7,w1      ;The values are such that the next instruction doesnt
                      ;get skipped

   sub w3,w0,w2       ;The result in W3 was unchecked.So this instructions
                      ;combines W3 and W0 to give a single 16 bit final result
                      ;0x7A5C which is stored in W2.
;..............................................................................
;Byte instructions
;Here few logical byte instructions are checked.
;..............................................................................
   sl.b w2,w3
   xor.b #CpuTest4Data9,w3
   neg.b w3,w4
   lsr.b w4,w5
   com.b w5,w6        ;This instructions complements W5 and stores it in W6.
                      ;The final result 0x00F8 is stored in W6.

L_SPI_4:
   mov w6,w0          ;The final result is written to W0.

return                ; End of TestSubset_4


;******************************************************************************
;*
;* Module: TestSubset_5()
;*
;* Features covered:
;*      1.      Here all the ADD,ADDC,SUB,SUBB,SUBBR,INC,INC2,DEC,DEC2,SE
;*              and ZE instructions are tested.
;*      2.      The addressing modes tested here are immediate,file register,
;*              register direct.
;*      3.      Byte instructions are also checked.
;*      4.      Branch instructions tested for true condition are GT,GTU,LE,
;*              LEU,NC,NN,OV.
;*      5.      Branch instructions tested for false condition are C,GE,GEU,
;*              LT,LTU,N,NOV.
;*      6.      DIV.UD is tested.
;*
;* Description:
;*      1.      All ADD,ADDC,SUB,SUBB,SUBBR,INC,INC2,DEC,DEC2,SE and ZE
;*              instructions are checked.
;*      2.      Here the result of one instruction is used as source operand
;*              for the next instruction. Using this method the result of one
;*              instruction is made dependent on the previous instructions.
;*      3.      Some logical, data rotate and shift instructions in byte
;*              mode are also used, to test their byte mode operation.
;*      4.      Some of the branch instructions are tested here.
;*      5.      To show the behaviour of status bits, the value in status bit
;*              is 'xor'ed with w0 register recursively.Here to nullify the
;*              the affect of past values(from application program) of SR,
;*              the w0 value is 'and'ed with complement of SR.
;*      6.      Divide unsigned double word is performed here.
;*      7.      The final result is stored in w2 register.
;*
;******************************************************************************

 _TestSubset_5:                 ; Start of TestSubset_5

;This is done to make the test case independent of previous SR status values.
;To capture the SR values it is 'xor'ed with wreg.
;Here first SR is cleared.So whenever SR values are 'xor'with wreg
;it is 'and'ed with w7 to nullify the affect of previous SR values.
   mov     #0x010F,w7

   clr     CpuTestRam2          ;To clear the previous content of 0x1554
   setm.b  CpuTestRam2          ;0x1554<7:0> = #0xff.
   inc     CpuTestRam2          ;0x1554 = 0x1554 + #1
   clr     w0                   ;w0 is cleared before byte instruction to
                                ;clear the previous value in upper byte.
   inc.b   CpuTestRam2,wreg     ;wreg<7:0> = 0x1554 + #1
   inc     w0,w1                ;w1 = w0 + #1
   inc2.b  CpuTestRam2          ;0x1554<7:0> = 0x1554 + #2
   inc2    CpuTestRam2,wreg     ;wreg = 0x1554 + #2
   inc2    w1,w2                ;w2 = w1 + #2
   add     CpuTestRam2          ;0x1554 = 0x1554 + wreg
   add     CpuTestRam2,wreg     ;wreg = 0x1554 + wreg
   add     #CpuTest5Data1,w2    ;w2 = w2 + #0x3ff
   xor     SR,wreg              ;to store the contents of status register.
   and     w0,w7,w0             ;to nullify past value(from main application)
                                ;of SR
   add     w2,#CpuTest5Data2,w3 ;w3 = w2 + #0x1f
   rlc.b   w2,w1                ;Rotate Left with Carry in byte mode.C = 0 here
   add.b   w3,w1,w4             ;w4<7:0> = w3 + w1
   se      w4,w5                ;Move w4<7:0> to w5<7:0> and sign extend
                                ;w5<15:8>.
   bra     LTU,L_FALSE_5        ;Branch if (!c).So here no branch takes place
   bra     GTU,L_GTU_5          ;Branch if (c&&!z).Here branch occurs.
L_FALSE_5:
   mov     #CpuTest5Data3,w2    ;If any branch instructions fails then it will
                                ;branch here i.e L_FALSE.Here the value 0x1234
                                ;is written into W0 register and sent to SPI.
                                ;So this affects the expected 16 bit result.
   bra     L_SPI_5
L_GTU_5:
   addc    CpuTestRam2          ;0x1554 = 0x1554 + wreg + (C). C = 1 here
   bra     C,L_FALSE_5          ;branch if (C).Here branch doesn't occur.
   daw.b   w0                   ;decimal adjust w0.This can work in byte mode
                                ;only.
   addc    #CpuTest5Data1, w5   ;w5 = w5 + #0x3ff + (C). C = 0 here.
   dec.b   CpuTestRam2          ;0x1554<7:0> = 0x1554 - #1.decrement by one
   addc    w5,#CpuTest5Data2,w6 ;w6 = w5 + #0x1f + (C). C = 1 here.
   asr.b   w6,w1                ;Arithmetic Shift Right in byte mode
   rrnc.b  w1,w6                ;Rotate Right with No Carry in byte mode.(C=1).
   addc    CpuTestRam2,wreg     ;wreg = 0x1554 + wreg + (C).carry bit is 1 here
   bra     LEU,L_LEU_5          ;branch if (!C||Z).Here branch occurs.
   bra     L_FALSE_5

L_LEU_5:
   dec     w6,w2                ;w2 = w6 - #1
   addc.b  w0,w2,w1             ;w1<7:0> = w0 + w2 + (C) .carry bit is 1 here.
   bra     GEU,L_FALSE_5        ;branch if (C).Here branch doesn't occur.
   bra     GE,L_FALSE_5         ;branch if ((N&&OV)||(!N&&!OV)).Here branch
                                ;doesn't occur.
   xor     SR,wreg              ;to store the contents of status register.
   and     w0,w7,w0             ;to nullify past value(from main application) of SR
   bra     N,L_FALSE_5          ;branch if (N).Here branch doesn't occur
   bra     LT,L_FALSE_5         ;branch if ((N&&!OV)||(!N&&OV)).Here branch
                                ;doesn't occur.
   bra     NN,L_NN_5            ;branch if (!N).Here branch occurs
   bra     L_FALSE_5

L_NN_5:
   dec2.b  CpuTestRam2          ;0x1554<7:0> = 0x1554 - #2
   dec2    CpuTestRam2,wreg     ;wreg = 0x1554 - #2
   dec2    w1,w2                ;w2 = w1 - #2
   rlnc.b  w0,w3                ;Rotate Left with No Carry in byte mode.(C = 1)
   rrc.b   w3,w0                ;Rotate Right with Carry in byte mode.C=1 here.
   sub     CpuTestRam2          ;0x1554 = 0x1554 - wreg
   bra     LE,L_LE_5            ;branch if (Z||(N&&!OV)||(!N&&OV)).Here branch
                                ;occurs
   bra     L_FALSE_5

L_LE_5:
   sub.b   CpuTestRam2,wreg     ;wreg<7:0>  = 0x1554 - wreg
   sub     #CpuTest5Data1,w2    ;w2 = w2 - #0x3ff
   sub     w2,#CpuTest5Data2,w3 ;w3 = w2 - #0x1f
   subb    CpuTestRam2          ;0x1554 = 0x1554 - wreg - (!c). C = 1 here.
   subb    CpuTestRam2,wreg     ;wreg = 0x1554 - wreg - (!c). C = 1 here.
   ze      w0,w4                ;move wo<7:0> to w4<7:0> and zero extend
                                ;w4<15:8>.
   bra     GT,L_GT_5            ;branch if ((!Z&&N&&OV)||(!Z&&!N&&!OV)).Here
                                ;branch occurs.
   bra     L_FALSE_5

L_GT_5:
   subb    #CpuTest5Data1,w4    ;w4 = w4 - #0x3ff - (!c). C = 1 here.
   dec     CpuTestRam2,wreg     ;wreg = 0x1554 - #1
   subb    w4,#CpuTest5Data2,w5 ;w5 = w4 - #0x1f - (!c). C = 1 here.
   subb.b  w5,w3,w1             ;w1<7:0> = w5 - w3 - (!c). C = 1 here.
   bra     NC,L_NC_5            ;branch if (!C).Here branch occurs.
   bra     L_FALSE_5

L_NC_5:
   subbr.b CpuTestRam2          ;0x1554<7:0> = wreg - 0x1554 - (!c). C = 0 here.
   subbr   CpuTestRam2,wreg     ;wreg = wreg - 0x1554 - (!c). C = 0 here.
   subbr   w1,#CpuTest5Data2,w3 ;w3 = #0x1f - w1 - (!c). C = 0 here.
   clr.b   w0                   ;w0<7:0> = 0x00.Used to cover CLR in byte mode.
   subr    CpuTestRam2          ;0x1554 = wreg - 0x1554
   subr    CpuTestRam2,wreg     ;wreg = wreg - 0x1554
   subr    w0,#CpuTest5Data2,w2 ;w2 = #0x1f - w0
   subbr   w3,w2,w4             ;w4 = w2 - w3 - (!c). C = 0 here
   subr.b  w6,w7,w5             ;w5<7:0> = w7 - w6.

;Divide unsigned double word
   repeat #17
   div.ud w4,w2

   bra     NOV,L_FALSE_5        ;branch if (!OV).Here branch doesn't occur.
   bra     OV,L_OV_5            ;branch if (OV).Here branch occurs.
   bra     L_FALSE_5

L_OV_5:
   sub     w0,w1,w2 

L_SPI_5:
   mov     w2,w0                ;writing the result to W0.

return                          ; End of TestSubset_5

;******************************************************************************
;                                                                             *
; Module: TestSubset_6()                                                      *
; ======================                                                      *
;                                                                             *
; Features Tested:                                                            *
; =================                                                           *
;1. All MUL instructions.                                                     *
;2. DIV instructions. DIV.S, DIV.SD and DIV.U are tested here,                *
;   DIV.UD is tested in TestSubset_5 and DIVF is tested in TestSubset_8       *
;3. Divide by zero arithmetic math error trap generation.                     *
;4. REPEAT instruction.                                                       *
;5. Interrupt call and return operations.                                     *
;6. Automatic context save on stack.                                          *
;7. SR flags IPL<2:0>, RA and CORCON flag IPL3                                *
;                                                                             *
; Description:                                                                *
; ============                                                                *
;1. Tested all multiplication instructions like                               *
;   MUL.B f             W2 = f<7:0> * WREG<7:0>                               *
;   MUL.SS Wb,Ws,Wnd    {Wnd + 1,Wnd} = sign(Wb) * sign(Ws)                   *
;   MUL.SU Wb,#lit5,Wnd {Wnd + 1,Wnd} = sign(Wb) * unsign(lit5)               *
;   MUL.SU Wb,Ws,Wnd    {Wnd + 1,Wnd} = sign(Wb) * unsign(Ws)                 *
;   MUL.US Wb,Ws,Wnd    {Wnd + 1,Wnd} = unsign(Wb) * sign(Ws)                 *
;   MUL.UU Wb,#lit5,Wnd {Wnd + 1,Wnd} = unsign(Wb) * unsign(lit5)             *
;   MUL.UU Wb,Ws,Wnd    {Wnd + 1,Wnd} = unsign(Wb) * unsign(Ws)               *
;2. Tested DIV instructions like                                              *
;   DIV.U Wm, Wn        Unsigned 16/16-bit integer divide                     *
;   DIV.S Wm, Wn        Signed 16/16-bit integer divide                       *
;   DIV.SD Wm, Wn       Signed 32/16-bit integer divide                       *
;3. DIV.S is tested for divide by zero arithmetic math error trap generation. *
;   After trap is cleared the same DIV.S is tested to check whether division  *
;   instruction can be recovered after the trap or not.                       *
;4. MUL f is tested for byte operation. Because this is the only instruction  *
;   which provides for an 8-bit multiply.                                     *
;5. REPEAT instruction is tested by testing RCOUNT register.                  *
;6. SR flags IPL<2:0>, RA and CORCON flag IPL3 are tested.                    *
;7. The SFRs modified are INTCON1, INTCON2, RCOUNT, CORCON.                   *
;8. The working register modified are w0-w7                                   *
;9.The data memory locations accessed are: 0x1AAA                             *
;                                                                             *
;******************************************************************************

_TestSubset_6:                   ; Start of TestSubset_6

;..............................................................................
;1. Saving of SFR's INTCON1, INTCON2, RCOUNT, CORCON on to the stack.
;..............................................................................
        push    INTCON1
.ifdef __dsPIC33F
        push    INTCON2
.endif
.ifdef __PIC24H
        push    INTCON2
.endif
        push    RCOUNT
        push    CORCON

;..............................................................................
;1. Alternate vector table(ALTIVT) bit is set in INTCON2 (dsPIC33F/PIC24H only)
;2. Move 0xF042 to w0 and store the value 0xF042 into the data memory
;   at the location 0x1AAA.
;3. mov the address 0xAAA to w3 and 0x8001 to w4.
;4. Multiply the contents w4 with the contents of [w3] and store the result
;   to w6:w7. Both source operands and the result are interpreted as two's
;   complement signed numbers. Register indirect addressing is used for second
;   operand.
;5. Unsigned integer divide is performed. The result of previous mul.ss
;   instruction is used as i/p values for this division. The 16-bit quotient
;   of the divide operation is stored in W0, and the 16-bit remainder is
;   stored in W1 by default. The result of this division will be used as i/p of
;   other multiplication or division. This cycle continues so that the
;   end result will validate all the features tested.
;6. Multiply the contents of w0 with the contents of w1, and store the 32-bit
;   result w6:w7.
;..............................................................................
.ifdef __dsPIC33F
        bset    INTCON2, #15      ; Use alternate vector table
.endif
.ifdef __PIC24H
        bset    INTCON2, #15      ; Use alternate vector table
.endif

        mov     #CpuTest6Data1, w0; 0xF042 is moved to w0
        mov     w0, CpuTestRam1   ; 0xF042 is stored at 0x0AAA
        mov     #CpuTestRam1, w3  ; 0x0AAA is moved to w3
        mov     #CpuTest6Data2, w4; 0x8001 is moved to w4

        mul.ss  w4, [w3], w6      ; Integer 16x16-bit Signed Multiply

        repeat  #17               ; Unsigned 16/16-bit integer divide
        div.u   w6, w7

        mul.su  w0, w1, w6        ; Integer 16x16-bit Signed-Unsigned Multiply

;..............................................................................
;1. Generate the divide by zero math error trap using div.s instruction
;   with the operands as w6 and w7.
;2. Before going to math trap ISR the PC is captured to check for the context
;   save on to the stack.
;3. In order to get result independent of PCL and PCH, PCH and PCL are
;   subtracted first and later added to w5 during the testing of context save
;   on to the stack.
;4. Tested interrupt call and return operations.
;5. In the math error trap ISR the RCOUNT is cleared so that effectively next
;   instruction after the div.s instruction will be executed.
;6. The w1:w0 are of no use after the divide by zero error.
;..............................................................................
        mov     PCH, w5
        sub     w4, w5, w5
        mov     PCL, w2
        sub     w5, w2, w5
                                  ; refer special issue
        repeat  #17               ; divide by zero arithmetic error trap
        div.s   w6, w7            ; generation

;..............................................................................
;1. Multiply the contents of w4 with unsigned short literal 0x07 and store the
;   result to w2:w3.
;2. Signed 16/16-bit integer divide is performed once again to check whether
;   division operation can recovered after the math error trap or not.
;3. Multiply the contents of w0 with w1 and store the result to w6:w7.
;4. Signed 32/16-bit integer divide is performed. The 16-bit quotient of the
;   divide operation is stored in W0, and the 16-bit remainder is stored
;   in W1 by default.
;5. Multiply the contents of w0 with the contents of w1 and store the result
;   to w6:w7.
;6. Multiply the contents of w6 with the 5-bit literal 0x1F and store the
;   result to w2:w3.
;7. w2, w3 validates above operations. w5 validates operations in math trap ISR
;8. w2, w3, w5 and w7 are added and stored to produce the final result in w0.
;..............................................................................
                                  ; Integer 16x16-bit Signed-
                                  ; Unsigned Short Literal Multiply
        mul.su  w4, #CpuTest6Data3, w2

        repeat  #17               ; Signed 16/16-bit integer divide
        div.s   w2, w3

        mul.us  w0, w1, w6        ; Integer 16x16-bit Unsigned-Signed Multiply

                                  ; mov #17(0x11) to w4
        mov     #CpuTest6Data6, w4
        repeat  w4                ; Signed 32/16-bit integer divide
        div.sd  w6, w3

        mul.uu  w0, w1, w6        ; Integer 16x16-bit Unsigned Multiply
                                  ; Integer 16x16-bit Unsigned-
                                  ; Short Literal Multiply
        mul.uu  w6, #CpuTest6Data4, w2

        add     w2, w3, w2
        add     w5, w7, w5
        add     w2, w5, w0

; Retrieving of SFR's INTCON1, INTCON2, RCOUNT, CORCON from the stack.
;..............................................................................
        pop     CORCON
        pop     RCOUNT
.ifdef __dsPIC33F
        pop     INTCON2
.endif
.ifdef __PIC24H
        pop     INTCON2
.endif
        pop     INTCON1

return                            ; End of TestSubset_6

.ifdef __dsPIC33F

;******************************************************************************
;*
;* Module: TestSubset_7()
;*
;* Features covered:
;*      1.      All DSP instructions, including accumulator based instructions
;*              and MAC-class instructions are checked.
;*      2.      Here the DSP MAC register indirect addressing with post
;*              increment,pre increment and register offset is checked.
;*      3.      Checking of Y data read bus along with toggling all bits in Y
;*              address bus is done here.
;*      4.      Both DSP accumulators A and B are checked.
;*      5.      17*17 integer and fractional multiplication is checked
;*      6.      Saturation logic, overflow and rounding is also tested.
;*      7.      Barrel shifter unit is tested by using accumulator shift
;*              instructions.
;*      8.      Math error trap generation for accumulator A,B overflow,
;*              accumulator B catastrophic overflow.
;*      9.      Branch instructions tested for true condition are SA,SB,OA,OB.
;*      10.     Branch instructions tested for false condition are SA,SB.
;*
;* Description:
;*      1.      All DSP instructions, including accumulator based instructions
;*              and MAC-class instructions are checked.
;*      2.      Here the DSP MAC register indirect addressing with post
;*              increment,pre increment and register offset is checked.
;*      3.      Checking of Y data read bus along with toggling all bits in Y
;*              address bus is done here.
;*      4.      Math error trap generation for accumulator A,B overflow,
;*              accumulator B catastrophic overflow.
;*      5.      In the trap service routine literal 1 is added to w0 register
;*              and w0 content is added at the end with w6 to get the final 16
;*              bit result in w1.
;*      6.      Here the result of one instruction is used as source operand
;*              for the next instruction. Using this method the result of one
;*              instruction is made dependent on the previous instructions.
;*      7.      To check the status register bits behaviour some branch
;*              instructions are used.
;*      8.      The final result is stored in w1 register.
;*
;******************************************************************************

_TestSubset_7:              ; Start of TestSubset_7

;..............................................................................
;1. Saving of CORCON, w8-w14 on to the stack
;..............................................................................
   push    CORCON            ; Saving of CORCON,w8-w13
                             ; on to the stack
.ifdef __dsPIC33F
        push    INTCON2
.endif
.ifdef __PIC24H
        push    INTCON2
.endif
   push    w8
   push    w10
   push    w11
   push    w12
   push    w13
   push    DCOUNT
   push    DOSTARTL
   push    DOSTARTH
   push    DOENDL
   push    DOENDH

;Loading memory locations into w10,w11 registers to access the Y data RAM
;address. The addresses are chosen such that they collectively toggle all the 
;address bits of Y data read bus.

.ifdef __dsPIC33F
        bset    INTCON2, #15      ; Use alternate vector table
.endif
.ifdef __PIC24H
        bset    INTCON2, #15      ; Use alternate vector table
.endif

   mov #0x0700,w0            ;INTCON1 settings are moved to w0
   ior INTCON1               ;Acc A overflow trap,Acc B overflow trap and
                             ;catastrophic Acc overflow trap enabled.(OVATE
                             ;OVBTE,COVTE bits set)

;The toggling of Y address bus is done by choosing the Y address location such
;a way that all bits are toggled.The X address location 0x110C is loaded to w8
;to use it for MAC prefetch operations.
   mov #CpuTestRam + 0xC,w8
   mov #CpuTestRamY1,w10
   mov #CpuTestRamY2,w11

;Here the used address locations are loaded with values.
   mov w11,[w8]
   mov w10,[w11]
   mov w8,[w10]

;Here Acc A is cleared first and then loaded with value in w8 shifted by 8 to
;its left. This causes Acc overflow into guard bits A<39:32>. Math Error trap
;occurs due to Acc A overflow. Here both OVAERR and MATHERR bits get set.
   clr A,[w8]-=6,w4,[w11]+=2,w6
   clr B
   lac w8,#-8,A           ;This causes arithmetic trap due to Acc A overflow
   bclr INTCON1,#OVATE
   bra OA,L_OA_7
   bra L_FALSE_7

;If any of the branch conditions fail the program control comes here. 0x1234
;is loaded into result register and branch to L_SPI where the result register
;is moved to SPI1 buffer register and transmitted
L_FALSE_7:
  mov #CpuTest7Data1,w1
  bra L_SPI_7

;Control comes here when branch on overflow of Acc A occurs
L_OA_7:
   mov #0x10E3,w3
   mov w3,CORCON             ;Unsigned,Acc A saturation,Acc B saturation,Data
                             ;space write saturation,normal Acc saturation mode
                             ;Biased rounding,Integer mode
   mov w11,w13

   mac w4*w4,A
   bra OA,L_FALSE_7
   bra SA,L_SA_7
   bra L_FALSE_7

L_SA_7:
   sac A,#-8,[w8]
   bclr CORCON,#SATDW        ;Data space write saturation disabled
   mac w4*w6,B,[w8]+=2,w5,[w10],w7,[w13]+=2
                             ;Here biased rounding mode is set.So 1 is added to
                             ;Acc A<31:16> and stored in AWB(Accumulator Write
                             ;Back).
   sac.r B,#7,[w8]           ;Here since Acc B<15:0> < 0x8000, the Acc B is
                             ;rounded down (add nothing)and written to AWB.
   neg B
   sub B
   bra SB,L_SB_7
   bra L_FALSE_7

L_SB_7:
   bclr SR,#SAB               ;Clears SA,SB and SAB bits
   bra SA,L_FALSE_7
   bra SB,L_FALSE_7

   sub w10,w11,w12
   mpy w5*w7,A,[w8]+=4,w4,[w11+w12],w6
   add B
   edac w6*w6,B,[w8]-=4,[w11]-=2,w5
   bclr CORCON,#SATA
   bclr CORCON,#SATB

;This causes Acc overflow into guard bits A<39:32> and catasrophic overflow as
;the sign bit B<39> gets destroyed. Math Error trap occurs due to Acc B overflow
;and Acc B catastrophic overflow. So OVBERR,COVBERR and MATHERR bits get set.

   sftac B,#-9                ;shift value is chosen that it will sturate Acc B
                              ;to 0x8000000000.
   bclr INTCON1,#OVBTE
   bra OB,L_OB_7
   bra L_FALSE_7

L_OB_7:
   mov #0x00F0,w2
   mov w2,CORCON              ;Signed,Acc A saturation,Acc B saturation,Data
                              ;space write saturation,Acc super saturation mode
                              ;Unbiased rounding,fractional mode.
   ed   w5*w5,A,[w8]-=2,[w11]+=2,w6

;Arithmetic trap occurs due to Acc shift error.
;So SFTAERR, MATHERR bits get set.
   sftac A,w6
   neg B
   btsc SR,#OAB
   add A
   mpy.n w4*w6,B,[w8]+=6,w5,[w11]-=2,w7
   bra OB,L_FALSE_7
   msc w5*w7,B,[w8]-=4,w4,[w11]+=4,w6,[w13]+=2
   and w13,w4,w2
   movsac B,[w8],w5,[w11],w7,w13
   mpy w6*w6,A
   add w13,#4,A
   sac A,#0,w4
   xor w4,w2,w6
   xor w6,w5,w4
   add w0,w4,w0

   do w5, L_DO                 ;DO Wn is tested here
     add DCOUNT,wreg           ;DO count is added to W0 to test DO Wn
     add CORCON,wreg           ;Testing of DL bits of CORCON
     bset CORCON,#EDT          ;Early termination of DO Loop
     nop                       ;Added to avoid re-entering of the DO loop 
L_DO:                          ;after the EDT bit is set.
     xor w0,w7,w1              

L_SPI_7:
   mov w1,w0

;Restoring the previous values
   POP    DOENDH
   POP    DOENDL
   POP    DOSTARTH
   POP    DOSTARTL
   POP    DCOUNT
   pop    w13
   pop    w12
   pop    w11
   pop    w10
   pop    w8
.ifdef __dsPIC33F
        pop     INTCON2
.endif
.ifdef __PIC24H
        pop     INTCON2
.endif
   pop    CORCON

return                       ; End of TestSubset_7

;******************************************************************************
;                                                                             *
; Module: TestSubset_8()                                                      *
; ======================                                                      *
;                                                                             *
; Features Tested:                                                            *
; =================                                                           *
;1. The operation of Modulo Addressing(both byte and word modes) while        *
;   accessing the circular buffers.                                           *
;2. The operation of Bit-reversed Addressing while accessing bit-reverse      *
;   addressed buffers.                                                        *
;3. Operation of DO loop.                                                     *
;4. Signed 16/16-bit fractional divide(DIVF).                                 *
;                                                                             *
; Description:                                                                *
; ============                                                                *
;1. Tested operation Modulo addressing (both byte and word modes)             *
;   while accessing circular buffers.                                         *
;2. Write some constant values to Y memory which can be read through          *
;   Modulo addressing(Y space).                                               *
;3. 0x1100-0x104 is filled with values by using                               *
;   Modulo Addressing(X Data space, Incrementing, word)                       *
;4. Testing of DO loop is also performed here. The DO loop control registers  *
;   added/subtracted to w0 such that w0(result) is independent of PCL or PCH. *
;   These addition/subtraction will be done when the loop count is '0',       *
;   that is at the last loop iteration. DCOUNT register is added to w0        *
;   in each loop iteration. The testing of DA bit in SR register is also done.*
;5. The values written to the data memory locations are shown below. If more  *
;   than one value is shown across an address means, its the updated value    *
;   which is because of circular buffers.                                     *
;   address     Values(w1) written to data memory                             *
;   0x1100       0x7F54, 0x7F5A                                               *
;   0x1102       0x7F56                                                       *
;   0x1104       0x7F58                                                       *
;6. 0x1106-0x1107 is filled with values by using                              *
;   Modulo Addressing(X Data space, Decrementing, byte)                       *
;7. The values written to the data memory locations are shown below.          *
;   The contents of [w2] are used for writing to data memory.                 *
;   address     Values[w2] written to data memory                             *
;   0x1106       0x27                                                         *
;   0x1107       0x3C, 0x54                                                   *
;   So the location 0x906 contains 0x5427(word aligned)                       *
;8. 0x1108-0x110A is filled with values by using                              *
;   Bit-Reversed Addressing(X Data space)                                     *
;9. The values written to the data memory locations are shown below.          *
;   address     Values written to data memory                                 *
;   0x1108       7F5A, 7F58                                                   *
;   0x110A       7F56, 5427                                                   *
;10.The values which are written to Y memory are read through                 *
;   Modulo Addressing(Y Data space, Decrementing, Word).                      *
;   Paralleled to this the 0x1104-0x110A are also read which will be modified *
;   to get unique result. 0x1100-0x1102 are also read to modify the result.   *
;11.Signed 16/16-bit fractional divide is performed. The 16-bit quotient of   *
;   the divide operation is stored in W0, and the 16-bit remainder is         *
;   stored in W1.                                                             *
;12.The final unique 16-bit result is which is there in w0 is written to      *
;   SPIBUF register.                                                          *
;13.Saving and retrieving of SFR's w8,w10,DCOUNT,DOSTARTL,DOSTARTH,DOENDL,    *
;   DOENDH,MODCON,XMODSRT,XMODEND,YMODSRT,YMODEND and XBREV is done.          *
;14.The memory locations accessed are:                                        *
;   0x1100-0x110A,                                                            *
;   0x1AAC, 0x1AAE  if RAM_SIZE=8                                             *
;   0x2AAC, 0x2AAE  if RAM_SIZE=16                                            *
;   0x6AAC, 0x6AAE  if RAM_SIZE=28/30/32/52                                   *
;15.The expected result is 0x1D7D                                             *
;                                                                             *
;******************************************************************************

_TestSubset_8:                   ; Start of TestSubset_8

;..............................................................................
;1. Saving of SFR's w8,w10,DCOUNT,DOSTARTL,DOSTARTH,DOENDL,DOENDH,MODCON,XMODSRT,
;   XMODEND,YMODSRT,YMODEND and XBREV on to the stack.
;..............................................................................
        push    w8
        push    w10
        push    DCOUNT
        push    DOSTARTL
        push    DOSTARTH
        push    DOENDL
        push    DOENDH
        push    MODCON
        push    XMODSRT
        push    XMODEND
        push    YMODSRT
        push    YMODEND
        push    XBREV

;..............................................................................
;Writing data to Y memory locations:
;1. mov 0x273C to w0 and 0x7F54 to w1.
;2. Store the contents of w0 to Y memory at 0x1AAC or 0x2AAC or 0x4AAC or 0x6AAC or 0xAAAC
;   depending on the RAM_SIZE 8 or 16 or 30 respectively.
;3. Store the contents of w1 to Y memory at 0x1AAE or 0x2AAE or 0x4AAE or 0x6AAE or 0x8AAE
;   depending on the RAM_SIZE 8 or 16 or 30 respectively.
;..............................................................................
        mov     #CpuTest8Data1, w0
        mov     #CpuTest8Data2, w1

        mov     #(CpuTestRamY2+0x2), w2
        mov     #(CpuTestRamY2+0x4), w10
        mov     w0, [w2]
        mov     w1, [w10]

;..............................................................................
; Modulo Addressing(X Data space, Incrementing, word):
;1. Modulo addressing with circular buffers supported by X data space.
;2. Modulo Addressing Initialization for Incrementing Modulo Buffers.
;3. The word sized modulo buffers are implemented.
;4. The Buffer length is 8 bytes.
;5. Buffer starting address is taken as 0x1100, which is loaded into XMODSRT.
;6. The Buffer end address is 0x1105, which is loaded into XMODEND.
;7. 0x8FF3 is loaded into MODCON where w3 is used to access the circular-
;   buffers and set the XMODEN bit in MODCON.
;8. Testing of DO loop is also performed here. The DO loop control registers
;   added/subtracted to w0 such that w0(result) is independent of PCL or PCH.
;   These addition/subtraction will be done when the loop count is '0',
;   that is at the last loop iteration. DCOUNT register is added to w0
;   in each loop iteration. The testing of DA bit in SR register is also done.
;9. The values written to the data memory locations are shown below. If more
;   than one value is shown across an address means, its the updated value
;   which is because of circular buffers.
;
;   address     Values(w1) written to data memory
;   0x1100       0x7F54, 0x7F5A
;   0x1102       0x7F56
;   0x1104       0x7F58
;10.w3 register address will be adjusted to 0x1100 automatically
;   at the end of the buffer.
;..............................................................................
        mov     #CpuTestRam, w3
        mov     w3, XMODSRT       ; 0x1100 is loaded into XMODSRT
        mov     #(CpuTestRam+0x5), w4
        mov     w4, XMODEND       ; 0x1105 is loaded into XMODEND
        mov     #CpuTest8Data3, w5
        mov     w5, MODCON        ; 0x8FF3 is loaded into MODCON

                                  ; do loop count of 0x3
        do      #CpuTest8Data8, XMODULO
        add     DCOUNT, WREG
        btsc    SR, #DA           ; DA=0 in the last iteration
        bra     LOOP              ; if DA=1 branch to LOOP
        mov     DOSTARTL, w5      ; sub/add DO control registers to w0
        sub     w0, w5, w0        ; such that w0 is independent of PCL or PCH
        mov     DOSTARTH, w5
        sub     w0, w5, w0
        add     DOENDL, WREG
        add     DOENDH, WREG
LOOP:
        mov     w1, [w3++]        ; contents of w1 are written to data memory
XMODULO:inc2    w1, w1            ; increment contents of w1 by 2

;..............................................................................
; Modulo Addressing(X Data space, Decrementing, byte):
;1. Modulo addressing with circular buffers supported by X data space.
;2. Modulo Addressing Initialization for Decrementing Modulo Buffers.
;3. The byte sized modulo buffers are implemented.
;4. The Buffer length is 2 bytes.
;5. Buffer end address is taken as 0x1107, which is loaded into XMODEND.
;6. The Buffer starting address is 0x1106, which is loaded into XMODSRT.
;7. The contents of MODCON are not changed, where w3 is used to access the
;   circular buffers.
;8. The values written to the data memory locations are shown below. If more
;   than one value is shown across an address means, its the updated value
;   which is because of circular buffers.
;   The contents of [w2] are used for writing to data memory.
;   address     Values[w2] written to data memory
;   0x1106       0x27
;   0x1107       0x3C, 0x54
;   So the location 0x1106 contains 0x5427(word aligned)
;9. w3 register address will be adjusted to 0x1107 automatically
;   at the end of the buffer.
;..............................................................................
        mov     #(CpuTestRam+0x7), w3
        mov     w3, XMODEND       ; 0x1107 is loaded into XMODEND
        mov     #(CpuTestRam+0x6), w4
        mov     w4, XMODSRT       ; 0x1106 is loaded into XMODSRT
                                  ; repeat with a count of 0x2
        repeat  #CpuTest8Data9
        mov.b   [w2++], [w3--]    ; at the end w3,w4=0x1106, w2=0xAAAF/0x6AAF/0x4AAF/0x2AAF/0x1AAF

;..............................................................................
;1. Move the value 0x1100 to w5.
;2. Store the contents of w5 to w8.
;3. Move the contents of data memory at the location 0x1100 to w6.
;4. The Accumulator A is cleared and the location 0x1102 contents are moved to
;   w7. The contents of w6 and w7 are used as input operands for
;   next mac instruction.
;5. Increment the w4 contents by 2. Now w4 becomes 0x1108.
;6. w4 will be used to access the Bit-reversed circular buffers and
;   w8 will be used to read the X space data memory locations in the next
;   mac instruction.
;..............................................................................
        mov     #CpuTestRam, w5   ; mov 0x1100 to w5
        mov     w5, w8            ; mov 0x1100 to w8
                                  ; mov 0x1100 location contents to w6 and
        mov     [w8++], w6        ; post increment w8.
                                  ; clr accumulator A and mov 0x1102 location
                                  ; contents to w7. post increment w8.
        clr     A, [w8]+=2, w7    ; now w8=0x1104

        inc2    w4, w4            ; w4=1108

;..............................................................................
; Bit-Reversed Addressing(X Data space):
;1. 0x04FF is loaded into the MODCON, where w4 is used to access the
;   bit-reverse addressed buffers.
;2. 0x8001 is loaded into the XBREV, which will set the BREN bit and loads the
;   buffer modifier value as 0x1 (i.e Buffer size = 2 words).
;3. w5 contains 0x1100. The w4 contains 1108.
;4. w4 register address will be adjusted to 0x1108 automatically
;   at the end of the buffer.
;5. The values written to the data memory locations are shown below. If more
;   than one value is shown across an address means, its the updated value
;   which is because of bit-reverse addressed buffers.
;   address     Values written to data memory
;   0x1108       7F5A, 7F58
;   0x110A       7F56, 5427
;6. The locations 0x1100 and 0x1102 contents are already moved to w6 and w7.
;   The locations 0x1104-0x110A will be read through the next mac instruction
;   and modified, so that all the above features tested can be validated.
;..............................................................................
        mov     #CpuTest8Data4, w3
        mov     w3, MODCON        ; 0x04FF is loaded into MODCON
        mov     #CpuTest8Data5, w3
        mov     w3, XBREV         ; 0x8001 is loaded into XBREV
        repeat  #CpuTest8Data8    ; repeat with a count of 0x3
        mov     [w5++], [w4++]

;..............................................................................
; Modulo Addressing(Y Data space, Decrementing, Word):
;1. Modulo addressing with circular buffers supported by Y data space.
;2. Modulo Addressing Initialization for Decrementing Modulo Buffers.
;3. The word sized modulo buffers are implemented.
;4. The Buffer length is 4 bytes.
;5. The w2 contains 0x8AAF/0x6AAF/0x4AAF/0x2AAF/0x1AAF depending on the RAM_SIZE which will
;   be moved to YMODEND.
;6. 0xAC is moved to w2(byte operation) so that w2 contains
;   0x8AAC/0x6AAC/0x4AAC/0x2AAC/0x1AAC which is loaded into YMODSRT.
;7. 0x4FAF is loaded into MODCON which will set YMODEN and w10 will be used
;   to access the circular buffers.
;8. w10 register address will be adjusted automatically at the end of
;   the buffer.
;9. 0x1104-0x110A will be read through the next mac instruction
;   and modified, so that all the above features tested can be validated.
;10.The last read of w6 and w7 are used as operands for division instruction.
;11.Add ACCH to wreg.
;..............................................................................
        mov     w2,YMODEND        ; load Y end address
        mov.b   #CpuTest8Data6, w2; 0xAC is moved to lower byte of w2
        mov     w2, YMODSRT       ; load Y start address
        mov     #CpuTest8Data7, w3
        mov     w3, MODCON        ; 0x4FAF is loaded into MODCON

        repeat  #CpuTest8Data8    ; repeat with a count of 0x3
                                  ; multiply w6*w7 and add to Acc A
                                  ; [w8]->w6, [w10]->w7
                                  ; post increment w8 and post decrement w10
        mac     w6*w7, A, [w8]+=2,w6, [w10]-=2, w7
        add     ACCAH, WREG

;..............................................................................
; Testing DIVF instruction:
;1. mov running 16-bit result to w2 as w0 and w1 will be modified because of
;   divf instruction.
;2. Signed 16/16-bit fractional divide is performed. The 16-bit quotient of the
;   divide operation is stored in W0, and the 16-bit remainder is stored in W1.
;3. w0, w1, w2 are added and the final result is stored in w0.
;..............................................................................
        mov     w0, w2
        repeat  #17
        divf    w7, w6            ; Signed 16/16-bit fractional divide

        add     w1, w2, w1
        add     w1, w0, w0        ; The final 16-bit result written to w0

; Retrieving of SFR's w8,w10,DCOUNT,DOSTARTL,DOSTARTH,DOENDL,DOENDH,MODCON,
; XMODSRT,XMODEND,YMODSRT,YMODEND and XBREV from the stack.
        pop     XBREV
        pop     YMODEND
        pop     YMODSRT
        pop     XMODEND
        pop     XMODSRT
        pop     MODCON
        pop     DOENDH
        pop     DOENDL
        pop     DOSTARTH
        pop     DOSTARTL
        pop     DCOUNT
        pop     w10
        pop     w8

	return                            ; End of TestSubset_8

.endif

.ifdef __dsPIC33E

;******************************************************************************
;*
;* Module: TestSubset_7()
;*
;* Features covered:
;*      1.      All DSP instructions, including accumulator based instructions
;*              and MAC-class instructions are checked.
;*      2.      Here the DSP MAC register indirect addressing with post
;*              increment,pre increment and register offset is checked.
;*      3.      Checking of Y data read bus along with toggling all bits in Y
;*              address bus is done here.
;*      4.      Both DSP accumulators A and B are checked.
;*      5.      17*17 integer and fractional multiplication is checked
;*      6.      Saturation logic, overflow and rounding is also tested.
;*      7.      Barrel shifter unit is tested by using accumulator shift
;*              instructions.
;*      8.      Math error trap generation for accumulator A,B overflow,
;*              accumulator B catastrophic overflow.
;*      9.      Branch instructions tested for true condition are SA,SB,OA,OB.
;*      10.     Branch instructions tested for false condition are SA,SB.
;*
;* Description:
;*      1.      All DSP instructions, including accumulator based instructions
;*              and MAC-class instructions are checked.
;*      2.      Here the DSP MAC register indirect addressing with post
;*              increment,pre increment and register offset is checked.
;*      3.      Checking of Y data read bus along with toggling all bits in Y
;*              address bus is done here.
;*      4.      Math error trap generation for accumulator A,B overflow,
;*              accumulator B catastrophic overflow.
;*      5.      In the trap service routine literal 1 is added to w0 register
;*              and w0 content is added at the end with w6 to get the final 16
;*              bit result in w1.
;*      6.      Here the result of one instruction is used as source operand
;*              for the next instruction. Using this method the result of one
;*              instruction is made dependent on the previous instructions.
;*      7.      To check the status register bits behaviour some branch
;*              instructions are used.
;*      8.      The final result is stored in w1 register.
;*
;******************************************************************************

_TestSubset_7:              ; Start of TestSubset_7

;..............................................................................
;1. Saving of CORCON, w8-w14 on to the stack
;..............................................................................
   push    CORCON            ; Saving of CORCON,w8-w13
                             ; on to the stack
   push    w8
   push    w10
   push    w11
   push    w12
   push    w13
   push    DCOUNT
   push    DOSTARTL
   push    DOSTARTH
   push    DOENDL
   push    DOENDH

;Loading memory locations into w10,w11 registers to access the Y data RAM
;address. The addresses are chosen such that they collectively toggle all the 
;address bits of Y data read bus.

   mov #0x0700,w0            ;INTCON1 settings are moved to w0
   ior INTCON1               ;Acc A overflow trap,Acc B overflow trap and
                             ;catastrophic Acc overflow trap enabled.(OVATE
                             ;OVBTE,COVTE bits set)

;The toggling of Y address bus is done by choosing the Y address location such
;a way that all bits are toggled.The X address location 0x110C is loaded to w8
;to use it for MAC prefetch operations.
   mov #CpuTestRam + 0xC,w8
   mov #CpuTestRamY1,w10
   mov #CpuTestRamY2,w11

;Here the used address locations are loaded with values.
   mov w11,[w8]
   mov w10,[w11]
   mov w8,[w10]

;Here Acc A is cleared first and then loaded with value in w8 shifted by 8 to
;its left. This causes Acc overflow into guard bits A<39:32>. Math Error trap
;occurs due to Acc A overflow. Here both OVAERR and MATHERR bits get set.
   clr A,[w8]-=6,w4,[w11]+=2,w6
   clr B
   lac w8,#-8,A           ;This causes arithmetic trap due to Acc A overflow
   bclr INTCON1,#OVATE
   bra OA,L_OA_7
   bra L_FALSE_7

;If any of the branch conditions fail the program control comes here. 0x1234
;is loaded into result register and branch to L_SPI where the result register
;is moved to SPI1 buffer register and transmitted
L_FALSE_7:
  mov #CpuTest7Data1,w1
  bra L_SPI_7

;Control comes here when branch on overflow of Acc A occurs
L_OA_7:
   mov #0x10E3,w3
   mov w3,CORCON             ;Unsigned,Acc A saturation,Acc B saturation,Data
                             ;space write saturation,normal Acc saturation mode
                             ;Biased rounding,Integer mode
   mov w11,w13

   mac w4*w4,A
   bra OA,L_FALSE_7
   bra SA,L_SA_7
   bra L_FALSE_7

L_SA_7:
   sac A,#-8,[w8]
   bclr CORCON,#SATDW        ;Data space write saturation disabled
   mac w4*w6,B,[w8]+=2,w5,[w10],w7,[w13]+=2
                             ;Here biased rounding mode is set.So 1 is added to
                             ;Acc A<31:16> and stored in AWB(Accumulator Write
                             ;Back).
   sac.r B,#7,[w8]           ;Here since Acc B<15:0> < 0x8000, the Acc B is
                             ;rounded down (add nothing)and written to AWB.
   neg B
   sub B
   bra SB,L_SB_7
   bra L_FALSE_7

L_SB_7:
   bclr SR,#SAB               ;Clears SA,SB and SAB bits
   bra SA,L_FALSE_7
   bra SB,L_FALSE_7

   sub w10,w11,w12
   mpy w5*w7,A,[w8]+=4,w4,[w11+w12],w6
   add B
   edac w6*w6,B,[w8]-=4,[w11]-=2,w5
   bclr CORCON,#SATA
   bclr CORCON,#SATB

;This causes Acc overflow into guard bits A<39:32> and catasrophic overflow as
;the sign bit B<39> gets destroyed. Math Error trap occurs due to Acc B overflow
;and Acc B catastrophic overflow. So OVBERR,COVBERR and MATHERR bits get set.

   sftac B,#-9                ;shift value is chosen that it will sturate Acc B
                              ;to 0x8000000000.
   bclr INTCON1,#OVBTE
   bra OB,L_OB_7
   bra L_FALSE_7

L_OB_7:
   mov #0x00F0,w2
   mov w2,CORCON              ;Signed,Acc A saturation,Acc B saturation,Data
                              ;space write saturation,Acc super saturation mode
                              ;Unbiased rounding,fractional mode.
   ed   w5*w5,A,[w8]-=2,[w11]+=2,w6

;Arithmetic trap occurs due to Acc shift error.
;So SFTAERR, MATHERR bits get set.
   sftac A,w6
   neg B
   btsc SR,#OAB
   add A
   mpy.n w4*w6,B,[w8]+=6,w5,[w11]-=2,w7
   bra OB,L_FALSE_7
   msc w5*w7,B,[w8]-=4,w4,[w11]+=4,w6,[w13]+=2
   and w13,w4,w2
   movsac B,[w8],w5,[w11],w7,w13
   mpy w6*w6,A
   add w13,#4,A
   sac A,#0,w4
   xor w4,w2,w6
   xor w6,w5,w4
   add w0,w4,w0

   do w5, L_DO                 ;DO Wn is tested here
     add DCOUNT,wreg           ;DO count is added to W0 to test DO Wn
     add CORCON,wreg           ;Testing of DL bits of CORCON
     bset CORCON,#EDT          ;Early termination of DO Loop
     nop                       ;Added to avoid re-entering of the DO loop 
L_DO:                          ;after the EDT bit is set.
     xor w0,w7,w1              

L_SPI_7:
   mov w1,w0

;Restoring the previous values
   POP    DOENDH
   POP    DOENDL
   POP    DOSTARTH
   POP    DOSTARTL
   POP    DCOUNT
   pop    w13
   pop    w12
   pop    w11
   pop    w10
   pop    w8
   pop    CORCON

return                       ; End of TestSubset_7

;******************************************************************************
;                                                                             *
; Module: TestSubset_8()                                                      *
; ======================                                                      *
;                                                                             *
; Features Tested:                                                            *
; =================                                                           *
;1. The operation of Modulo Addressing(both byte and word modes) while        *
;   accessing the circular buffers.                                           *
;2. The operation of Bit-reversed Addressing while accessing bit-reverse      *
;   addressed buffers.                                                        *
;3. Operation of DO loop.                                                     *
;4. Signed 16/16-bit fractional divide(DIVF).                                 *
;                                                                             *
; Description:                                                                *
; ============                                                                *
;1. Tested operation Modulo addressing (both byte and word modes)             *
;   while accessing circular buffers.                                         *
;2. Write some constant values to Y memory which can be read through          *
;   Modulo addressing(Y space).                                               *
;3. 0x1100-0x104 is filled with values by using                               *
;   Modulo Addressing(X Data space, Incrementing, word)                       *
;4. Testing of DO loop is also performed here. The DO loop control registers  *
;   added/subtracted to w0 such that w0(result) is independent of PCL or PCH. *
;   These addition/subtraction will be done when the loop count is '0',       *
;   that is at the last loop iteration. DCOUNT register is added to w0        *
;   in each loop iteration. The testing of DA bit in SR register is also done.*
;5. The values written to the data memory locations are shown below. If more  *
;   than one value is shown across an address means, its the updated value    *
;   which is because of circular buffers.                                     *
;   address     Values(w1) written to data memory                             *
;   0x1100       0x7F54, 0x7F5A                                               *
;   0x1102       0x7F56                                                       *
;   0x1104       0x7F58                                                       *
;6. 0x1106-0x1107 is filled with values by using                              *
;   Modulo Addressing(X Data space, Decrementing, byte)                       *
;7. The values written to the data memory locations are shown below.          *
;   The contents of [w2] are used for writing to data memory.                 *
;   address     Values[w2] written to data memory                             *
;   0x1106       0x27                                                         *
;   0x1107       0x3C, 0x54                                                   *
;   So the location 0x906 contains 0x5427(word aligned)                       *
;8. 0x1108-0x110A is filled with values by using                              *
;   Bit-Reversed Addressing(X Data space)                                     *
;9. The values written to the data memory locations are shown below.          *
;   address     Values written to data memory                                 *
;   0x1108       7F5A, 7F58                                                   *
;   0x110A       7F56, 5427                                                   *
;10.The values which are written to Y memory are read through                 *
;   Modulo Addressing(Y Data space, Decrementing, Word).                      *
;   Paralleled to this the 0x1104-0x110A are also read which will be modified *
;   to get unique result. 0x1100-0x1102 are also read to modify the result.   *
;11.Signed 16/16-bit fractional divide is performed. The 16-bit quotient of   *
;   the divide operation is stored in W0, and the 16-bit remainder is         *
;   stored in W1.                                                             *
;12.The final unique 16-bit result is which is there in w0 is written to      *
;   SPIBUF register.                                                          *
;13.Saving and retrieving of SFR's w8,w10,DCOUNT,DOSTARTL,DOSTARTH,DOENDL,    *
;   DOENDH,MODCON,XMODSRT,XMODEND,YMODSRT,YMODEND and XBREV is done.          *
;14.The memory locations accessed are:                                        *
;   0x1100-0x110A,                                                            *
;   0x1AAC, 0x1AAE  if RAM_SIZE=8                                             *
;   0x2AAC, 0x2AAE  if RAM_SIZE=16                                            *
;   0x6AAC, 0x6AAE  if RAM_SIZE=28/30/32/52                                   *
;15.The expected result is 0x1D7D                                             *
;                                                                             *
;******************************************************************************

_TestSubset_8:                   ; Start of TestSubset_8

;..............................................................................
;1. Saving of SFR's w8,w10,DCOUNT,DOSTARTL,DOSTARTH,DOENDL,DOENDH,MODCON,XMODSRT,
;   XMODEND,YMODSRT,YMODEND and XBREV on to the stack.
;..............................................................................
        push    w8
        push    w10
        push    DCOUNT
        push    DOSTARTL
        push    DOSTARTH
        push    DOENDL
        push    DOENDH
        push    MODCON
        push    XMODSRT
        push    XMODEND
        push    YMODSRT
        push    YMODEND
        push    XBREV

;..............................................................................
;Writing data to Y memory locations:
;1. mov 0x273C to w0 and 0x7F54 to w1.
;2. Store the contents of w0 to Y memory at 0x1AAC or 0x2AAC or 0x4AAC or 0x6AAC or 0xAAAC
;   depending on the RAM_SIZE 8 or 16 or 30 respectively.
;3. Store the contents of w1 to Y memory at 0x1AAE or 0x2AAE or 0x4AAE or 0x6AAE or 0x8AAE
;   depending on the RAM_SIZE 8 or 16 or 30 respectively.
;..............................................................................
        mov     #CpuTest8Data1, w0
        mov     #CpuTest8Data2, w1

        mov     #(CpuTestRamY2+0x2), w2
        mov     #(CpuTestRamY2+0x4), w10
        mov     w0, [w2]
        mov     w1, [w10]

;..............................................................................
; Modulo Addressing(X Data space, Incrementing, word):
;1. Modulo addressing with circular buffers supported by X data space.
;2. Modulo Addressing Initialization for Incrementing Modulo Buffers.
;3. The word sized modulo buffers are implemented.
;4. The Buffer length is 8 bytes.
;5. Buffer starting address is taken as 0x1100, which is loaded into XMODSRT.
;6. The Buffer end address is 0x1105, which is loaded into XMODEND.
;7. 0x8FF3 is loaded into MODCON where w3 is used to access the circular-
;   buffers and set the XMODEN bit in MODCON.
;8. Testing of DO loop is also performed here. The DO loop control registers
;   added/subtracted to w0 such that w0(result) is independent of PCL or PCH.
;   These addition/subtraction will be done when the loop count is '0',
;   that is at the last loop iteration. DCOUNT register is added to w0
;   in each loop iteration. The testing of DA bit in SR register is also done.
;9. The values written to the data memory locations are shown below. If more
;   than one value is shown across an address means, its the updated value
;   which is because of circular buffers.
;
;   address     Values(w1) written to data memory
;   0x1100       0x7F54, 0x7F5A
;   0x1102       0x7F56
;   0x1104       0x7F58
;10.w3 register address will be adjusted to 0x1100 automatically
;   at the end of the buffer.
;..............................................................................
        mov     #CpuTestRam, w3
        mov     w3, XMODSRT       ; 0x1100 is loaded into XMODSRT
        mov     #(CpuTestRam+0x5), w4
        mov     w4, XMODEND       ; 0x1105 is loaded into XMODEND
        mov     #CpuTest8Data3, w5
        mov     w5, MODCON        ; 0x8FF3 is loaded into MODCON

                                  ; do loop count of 0x3
        do      #CpuTest8Data8, XMODULO
        add     DCOUNT, WREG
        btsc    SR, #DA           ; DA=0 in the last iteration
        bra     LOOP              ; if DA=1 branch to LOOP
        mov     DOSTARTL, w5      ; sub/add DO control registers to w0
        sub     w0, w5, w0        ; such that w0 is independent of PCL or PCH
        mov     DOSTARTH, w5
        sub     w0, w5, w0
        add     DOENDL, WREG
        add     DOENDH, WREG
LOOP:
        mov     w1, [w3++]        ; contents of w1 are written to data memory
XMODULO:inc2    w1, w1            ; increment contents of w1 by 2

;..............................................................................
; Modulo Addressing(X Data space, Decrementing, byte):
;1. Modulo addressing with circular buffers supported by X data space.
;2. Modulo Addressing Initialization for Decrementing Modulo Buffers.
;3. The byte sized modulo buffers are implemented.
;4. The Buffer length is 2 bytes.
;5. Buffer end address is taken as 0x1107, which is loaded into XMODEND.
;6. The Buffer starting address is 0x1106, which is loaded into XMODSRT.
;7. The contents of MODCON are not changed, where w3 is used to access the
;   circular buffers.
;8. The values written to the data memory locations are shown below. If more
;   than one value is shown across an address means, its the updated value
;   which is because of circular buffers.
;   The contents of [w2] are used for writing to data memory.
;   address     Values[w2] written to data memory
;   0x1106       0x27
;   0x1107       0x3C, 0x54
;   So the location 0x1106 contains 0x5427(word aligned)
;9. w3 register address will be adjusted to 0x1107 automatically
;   at the end of the buffer.
;..............................................................................
        mov     #(CpuTestRam+0x7), w3
        mov     w3, XMODEND       ; 0x1107 is loaded into XMODEND
        mov     #(CpuTestRam+0x6), w4
        mov     w4, XMODSRT       ; 0x1106 is loaded into XMODSRT
                                  ; repeat with a count of 0x2
        repeat  #CpuTest8Data9
        mov.b   [w2++], [w3--]    ; at the end w3,w4=0x1106, w2=0xAAAF/0x6AAF/0x4AAF/0x2AAF/0x1AAF

;..............................................................................
;1. Move the value 0x1100 to w5.
;2. Store the contents of w5 to w8.
;3. Move the contents of data memory at the location 0x1100 to w6.
;4. The Accumulator A is cleared and the location 0x1102 contents are moved to
;   w7. The contents of w6 and w7 are used as input operands for
;   next mac instruction.
;5. Increment the w4 contents by 2. Now w4 becomes 0x1108.
;6. w4 will be used to access the Bit-reversed circular buffers and
;   w8 will be used to read the X space data memory locations in the next
;   mac instruction.
;..............................................................................
        mov     #CpuTestRam, w5   ; mov 0x1100 to w5
        mov     w5, w8            ; mov 0x1100 to w8
                                  ; mov 0x1100 location contents to w6 and
        mov     [w8++], w6        ; post increment w8.
                                  ; clr accumulator A and mov 0x1102 location
                                  ; contents to w7. post increment w8.
        clr     A, [w8]+=2, w7    ; now w8=0x1104

        inc2    w4, w4            ; w4=1108

;..............................................................................
; Bit-Reversed Addressing(X Data space):
;1. 0x04FF is loaded into the MODCON, where w4 is used to access the
;   bit-reverse addressed buffers.
;2. 0x8001 is loaded into the XBREV, which will set the BREN bit and loads the
;   buffer modifier value as 0x1 (i.e Buffer size = 2 words).
;3. w5 contains 0x1100. The w4 contains 1108.
;4. w4 register address will be adjusted to 0x1108 automatically
;   at the end of the buffer.
;5. The values written to the data memory locations are shown below. If more
;   than one value is shown across an address means, its the updated value
;   which is because of bit-reverse addressed buffers.
;   address     Values written to data memory
;   0x1108       7F5A, 7F58
;   0x110A       7F56, 5427
;6. The locations 0x1100 and 0x1102 contents are already moved to w6 and w7.
;   The locations 0x1104-0x110A will be read through the next mac instruction
;   and modified, so that all the above features tested can be validated.
;..............................................................................
        mov     #CpuTest8Data4, w3
        mov     w3, MODCON        ; 0x04FF is loaded into MODCON
        mov     #CpuTest8Data5, w3
        mov     w3, XBREV         ; 0x8001 is loaded into XBREV
        repeat  #CpuTest8Data8    ; repeat with a count of 0x3
        mov     [w5++], [w4++]

;..............................................................................
; Modulo Addressing(Y Data space, Decrementing, Word):
;1. Modulo addressing with circular buffers supported by Y data space.
;2. Modulo Addressing Initialization for Decrementing Modulo Buffers.
;3. The word sized modulo buffers are implemented.
;4. The Buffer length is 4 bytes.
;5. The w2 contains 0x8AAF/0x6AAF/0x4AAF/0x2AAF/0x1AAF depending on the RAM_SIZE which will
;   be moved to YMODEND.
;6. 0xAC is moved to w2(byte operation) so that w2 contains
;   0x8AAC/0x6AAC/0x4AAC/0x2AAC/0x1AAC which is loaded into YMODSRT.
;7. 0x4FAF is loaded into MODCON which will set YMODEN and w10 will be used
;   to access the circular buffers.
;8. w10 register address will be adjusted automatically at the end of
;   the buffer.
;9. 0x1104-0x110A will be read through the next mac instruction
;   and modified, so that all the above features tested can be validated.
;10.The last read of w6 and w7 are used as operands for division instruction.
;11.Add ACCH to wreg.
;..............................................................................
        mov     w2,YMODEND        ; load Y end address
        mov.b   #CpuTest8Data6, w2; 0xAC is moved to lower byte of w2
        mov     w2, YMODSRT       ; load Y start address
        mov     #CpuTest8Data7, w3
        mov     w3, MODCON        ; 0x4FAF is loaded into MODCON

        repeat  #CpuTest8Data8    ; repeat with a count of 0x3
                                  ; multiply w6*w7 and add to Acc A
                                  ; [w8]->w6, [w10]->w7
                                  ; post increment w8 and post decrement w10
        mac     w6*w7, A, [w8]+=2,w6, [w10]-=2, w7
        add     ACCAH, WREG

;..............................................................................
; Testing DIVF instruction:
;1. mov running 16-bit result to w2 as w0 and w1 will be modified because of
;   divf instruction.
;2. Signed 16/16-bit fractional divide is performed. The 16-bit quotient of the
;   divide operation is stored in W0, and the 16-bit remainder is stored in W1.
;3. w0, w1, w2 are added and the final result is stored in w0.
;..............................................................................
        mov     w0, w2
        repeat  #17
        divf    w7, w6            ; Signed 16/16-bit fractional divide

        add     w1, w2, w1
        add     w1, w0, w0        ; The final 16-bit result written to w0

; Retrieving of SFR's w8,w10,DCOUNT,DOSTARTL,DOSTARTH,DOENDL,DOENDH,MODCON,
; XMODSRT,XMODEND,YMODSRT,YMODEND and XBREV from the stack.
        pop     XBREV
        pop     YMODEND
        pop     YMODSRT
        pop     XMODEND
        pop     XMODSRT
        pop     MODCON
        pop     DOENDH
        pop     DOENDL
        pop     DOSTARTH
        pop     DOSTARTL
        pop     DCOUNT
        pop     w10
        pop     w8

return                            ; End of TestSubset_8

.endif

;--------End of All Code Sections ---------------------------------------------

.end                              ; End of program code in this file

