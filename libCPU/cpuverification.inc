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
;==========================================================================
;  Assembly Include File
;  This header file defines RROGRAM_FLASH_SIZE and RAM_SIZE for the device
;  being used. This header file also defines Data RAM addresses and
;  values used by the 16-bit CPU Self-Test Library software.
;
;  Note that the processor must be selected before this file is included.
;  The processor may be selected the following ways:
;
;       1. Command line switch:
;               C:\> pic30-as file.s -p33FJ256GP710A
;       2. Placing a ".equ" directive before the ".include":
;               .equ __33FJ256GP710A, 1
;               .include "p33FJ256GP710A.inc"
;       3. Setting the processor in the MPLAB IDE Project Dialog
;==========================================================================

    ; global declarations of PROGRAM_FLASH_SIZE and RAM_SIZE
    .global PROGRAM_FLASH_SIZE
    .global RAM_SIZE

.ifdef __dsPIC33FJ256GP710A
    .equ  PROGRAM_FLASH_SIZE, 256
    .equ  RAM_SIZE, 30
.endif
.ifdef __PIC24HJ256GP610A
    .equ  PROGRAM_FLASH_SIZE, 256
    .equ  RAM_SIZE, 30
.endif
.ifdef __dsPIC33EP512MU810
    .equ  PROGRAM_FLASH_SIZE, 512
    .equ  RAM_SIZE, 52
.endif
.ifdef __PIC24EP512GU810
    .equ  PROGRAM_FLASH_SIZE, 512
    .equ  RAM_SIZE, 52
.endif
.ifdef __dsPIC33EP64MC506
    .equ  PROGRAM_FLASH_SIZE, 64
    .equ  RAM_SIZE, 8
.endif

; For any device other than the 5 devices listed above, un-comment these 
; two lines and customize the values of PROGRAM_FLASH_SIZE and RAM_SIZE 
; based on the specific device being used.

#ifdef __dsPIC33FJ256GP710A__
    .equ  PROGRAM_FLASH_SIZE, 256
    .equ  RAM_SIZE, 30
#endif

#ifndef __dsPIC33EP512MU810__
   .equ  PROGRAM_FLASH_SIZE, 512
   .equ  RAM_SIZE, 52
#endif

;==============================================================================
;Constants stored in Program space for TestSubset_2:
;   The program memory is filled at the locations 0x005554, 0x00AAAA,
;   0x029554 with data 0x00AA55, 0xFF55AA, 0x005555 respectively.
;==============================================================================
        .section CpuTestPM1, code, address(0x005554)
Label_1:.pword  0x00AA55

        .section CpuTestPM2, code, address(0x00AAAA)
Label_2:.pword  0xFF55AA

; If program memory size is 256 or 512 KB, place the constant 0x005555 at 0x029554
.if (PROGRAM_FLASH_SIZE == 256) || (PROGRAM_FLASH_SIZE == 512)     
        .section CpuTestPM3, code, address(0x029554)
Label_3:.pword  0x005555
.endif

;==============================================================================
;Data RAM allocation for CPUverification:
;==============================================================================
;..............................................................................
; Global declarations for address locations
;..............................................................................
        .global CpuTestRam
        .global CpuTestRam1
        .global CpuTestRam2
.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        .global CpuTestRam3
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        .global CpuTestRam4
.endif

.ifdef __dsPIC33F                 ; for dsPIC33F family devices only
        .global CpuTestRamY1
        .global CpuTestRamY2
.endif

.ifdef __dsPIC33E                 ; for dsPIC33E family devices only
        .global CpuTestRamY1
        .global CpuTestRamY2
.endif

;..............................................................................
; RAM allocation for CPUverification
;..............................................................................

             .equ WordLocations, 7
             .section *, bss,near,address(0x1100)
CpuTestRam:  .space 2*WordLocations

             .section *,bss,address(0x1AAA)
CpuTestRam1: .space 2
             .section *, bss,address(0x1554)
CpuTestRam2: .space 2

.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
             .section *,bss,address(0x3000)
CpuTestRam3: .space 2
.endif

.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
             .section *,bss,address(0x47FE)
CpuTestRam4: .space 2
.endif

.ifdef __dsPIC33F                 ; for dsPIC33F family devices only
                  .equ Y2locations,3
    .if RAM_SIZE == 8
                  .section *,bss,ymemory,address(0x1D54)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x1AAA)
    CpuTestRamY2: .space 2*Y2locations

    .elseif RAM_SIZE == 16
                  .section *,bss,ymemory,address(0x3554)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x2AAA)
    CpuTestRamY2: .space 2*Y2locations

    .elseif RAM_SIZE == 30
                  .section *,bss,ymemory,address(0x5554)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x6AAA)
    CpuTestRamY2: .space 2*Y2locations
    .endif
.endif

.ifdef __dsPIC33E                 ; for dsPIC33E family devices only
                  .equ Y2locations,3
    .if RAM_SIZE == 4
                  .section *,bss,ymemory,address(0x1D54)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x1AAA)
    CpuTestRamY2: .space 2*Y2locations

    .elseif RAM_SIZE == 8
                  .section *,bss,ymemory,address(0x2D54)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x2AAA)
    CpuTestRamY2: .space 2*Y2locations

    .elseif RAM_SIZE == 16
                  .section *,bss,ymemory,address(0x3554)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x4AAA)
    CpuTestRamY2: .space 2*Y2locations

    .elseif RAM_SIZE == 28
                  .section *,bss,ymemory,address(0x5554)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x6AAA)
    CpuTestRamY2: .space 2*Y2locations

    .elseif RAM_SIZE == 32
                  .section *,bss,ymemory,address(0x5554)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0x6AAA)
    CpuTestRamY2: .space 2*Y2locations

    .elseif RAM_SIZE == 52
                  .section *,bss,ymemory,address(0xB554)
    CpuTestRamY1: .space 2
                  .section *,bss,ymemory,address(0xCAAA)
    CpuTestRamY2: .space 2*Y2locations
    .endif
.endif

;==============================================================================
;Data Values used in TestSubsets:
;==============================================================================
;..............................................................................
;Global declarations for data :
;..............................................................................
                                  ; TestSubset_1
        .global CpuTest1Data1
        .global CpuTest1Data2
        .global CpuTest1Data3
                                  ; TestSubset_2
        .global CpuTest2Data1
        .global CpuTest2Data2
        .global CpuTest2Data3
        .global CpuTest2Data4
.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        .global CpuTest2Data5
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        .global CpuTest2Data6
.endif
                                  ; TestSubset_3
        .global CpuTest3Data1
        .global CpuTest3Data2
        .global CpuTest3Data3
        .global CpuTest3Data4
        .global CpuTest3Data5
        .global CpuTest3Data6
        .global CpuTest3Data7
        .global CpuTest3Data8
        .global CpuTest3Data9
        .global CpuTest3Data10
        .global CpuTest3Data11
        .global CpuTest3Data12
        .global CpuTest3Data13
        .global CpuTest3Data14
        .global CpuTest3Data15
        .global CpuTest3Data16
        .global CpuTest3Data17
        .global CpuTest3Data18
        .global CpuTest3Data19
        .global CpuTest3Data20
                                  ; TestSubset_4
        .global CpuTest4Data1
        .global CpuTest4Data2
        .global CpuTest4Data3
        .global CpuTest4Data4
        .global CpuTest4Data5
        .global CpuTest4Data6
        .global CpuTest4Data7
        .global CpuTest4Data8
        .global CpuTest4Data9
                                  ; TestSubset_5
        .global CpuTest5Data1
        .global CpuTest5Data2
        .global CpuTest5Data3
                                  ; TestSubset_6
        .global CpuTest6Data1
        .global CpuTest6Data2
        .global CpuTest6Data3
        .global CpuTest6Data4
        .global CpuTest6Data5
        .global CpuTest6Data6

.ifdef __dsPIC33F                 ; for dsPIC33F family devices only
                                  ; TestSubset_7
        .global CpuTest7Data1
                                  ; TestSubset_8
        .global CpuTest8Data1
        .global CpuTest8Data2
        .global CpuTest8Data3
        .global CpuTest8Data4
        .global CpuTest8Data5
        .global CpuTest8Data6
        .global CpuTest8Data7
        .global CpuTest8Data8
        .global CpuTest8Data9
.endif

.ifdef __dsPIC33E                 ; for dsPIC33E family devices only
                                  ; TestSubset_7
        .global CpuTest7Data1
                                  ; TestSubset_8
        .global CpuTest8Data1
        .global CpuTest8Data2
        .global CpuTest8Data3
        .global CpuTest8Data4
        .global CpuTest8Data5
        .global CpuTest8Data6
        .global CpuTest8Data7
        .global CpuTest8Data8
        .global CpuTest8Data9
.endif

;..............................................................................
;Data definitions for all test subsets:
;..............................................................................
                                  ; TestSubset_1
        .equ CpuTest1Data1,0xB29F
        .equ CpuTest1Data2,0xFE
        .equ CpuTest1Data3,1
                                  ; TestSubset_2
        .equ CpuTest2Data1, 0x00FF
        .equ CpuTest2Data2, 0x0000
        .equ CpuTest2Data3, 0x5555
        .equ CpuTest2Data4, 0xAAAA
.if (RAM_SIZE == 16) || (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        .equ CpuTest2Data5, 0x0001
.endif
.if (RAM_SIZE == 28) || (RAM_SIZE == 30) || (RAM_SIZE == 32) || (RAM_SIZE == 52)
        .equ CpuTest2Data6, 0xFFFF
.endif
                                  ; TestSubset_3
.ifdef __dsPIC33F              ; for dsPIC33F family devices only                             
        .equ CpuTest3Data1, 0x000F
.endif    
.ifdef __PIC24H                 ; for PIC24H family devices only                             
        .equ CpuTest3Data1, 0x000F
.endif 
.ifdef __dsPIC33E              ; for dsPIC33E family devices only                             
        .equ CpuTest3Data1, 0x0013
.endif 
.ifdef __PIC24E                 ; for PIC24E family devices only                             
        .equ CpuTest3Data1, 0x0013
.endif 
        .equ CpuTest3Data2, 0x00F4
        .equ CpuTest3Data3, 0x0003
        .equ CpuTest3Data4, 0x0011
        .equ CpuTest3Data5, 0x0022
        .equ CpuTest3Data6, 0x0033
        .equ CpuTest3Data7, 0x0044
        .equ CpuTest3Data8, 0x0055
        .equ CpuTest3Data9, 0x0066
        .equ CpuTest3Data10, 0x0077
        .equ CpuTest3Data11, 0x0088
        .equ CpuTest3Data12, 0x0099
        .equ CpuTest3Data13, 0x00AA
        .equ CpuTest3Data14, 0x00BB
        .equ CpuTest3Data15, 0x00CC
        .equ CpuTest3Data16, 0x00DD
        .equ CpuTest3Data17, 0x00EE
        .equ CpuTest3Data18, 0x00FF
        .equ CpuTest3Data19, 0x0040
        .equ CpuTest3Data20, 0x0057
                                  ; TestSubset_4
        .equ CpuTest4Data1,0x1FF
        .equ CpuTest4Data2,0x1F
        .equ CpuTest4Data3,0xAA
        .equ CpuTest4Data4,0x19
        .equ CpuTest4Data5,0x13
        .equ CpuTest4Data6,0x06
        .equ CpuTest4Data7,0x13
        .equ CpuTest4Data8,1
        .equ CpuTest4Data9,0x7
                                  ; TestSubset_5
        .equ CpuTest5Data1,0x3FF
        .equ CpuTest5Data2,0x1F
        .equ CpuTest5Data3,0x1234
                                  ; TestSubset_6
        .equ CpuTest6Data1, 0xF042
        .equ CpuTest6Data2, 0x8001
        .equ CpuTest6Data3, 0x0007
        .equ CpuTest6Data4, 0x001F
        .equ CpuTest6Data5, 0x00F0
        .equ CpuTest6Data6, 0x0011

.ifdef __dsPIC33F                 ; for dsPIC33F family devices only
                                  ; TestSubset_7
        .equ CpuTest7Data1,0x1234
                                  ; TestSubset_8
        .equ CpuTest8Data1, 0x273C
        .equ CpuTest8Data2, 0x7F54
        .equ CpuTest8Data3, 0x8FF3
        .equ CpuTest8Data4, 0x04FF
        .equ CpuTest8Data5, 0x8001
        .equ CpuTest8Data6, 0x00AC
        .equ CpuTest8Data7, 0x4FAF
        .equ CpuTest8Data8, 0x0003
        .equ CpuTest8Data9, 0x0002
.endif

.ifdef __dsPIC33E                 ; for dsPIC33E family devices only
                                  ; TestSubset_7
        .equ CpuTest7Data1,0x1234
                                  ; TestSubset_8
        .equ CpuTest8Data1, 0x273C
        .equ CpuTest8Data2, 0x7F54
        .equ CpuTest8Data3, 0x8FF3
        .equ CpuTest8Data4, 0x04FF
        .equ CpuTest8Data5, 0x8001
        .equ CpuTest8Data6, 0x00AC
        .equ CpuTest8Data7, 0x4FAF
        .equ CpuTest8Data8, 0x0003
        .equ CpuTest8Data9, 0x0002
.endif

