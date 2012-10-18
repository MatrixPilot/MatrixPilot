	.file "/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/educational/fixedPointTrig.X/traps.c"
	.section	.debug_abbrev,info
.Ldebug_abbrev0:
	.section	.debug_info,info
.Ldebug_info0:
	.section	.debug_line,info
.Ldebug_line0:
	.section	.text,code
.Ltext0:
	.section	.isr.text,code
	.align	2
	.global	__OscillatorFail	; export
	.type	__OscillatorFail,@function
__OscillatorFail:
.LFB0:
.LSM0:
	.set ___PA___,1
	lnk	#0
.LSM1:
	bclr.b	_INTCON1bits,#1
.L2:
.LSM2:
	bra	.L2
.LFE0:
	.align	2
	.global	__AddressError	; export
	.type	__AddressError,@function
__AddressError:
.LFB1:
.LSM3:
	.set ___PA___,1
	lnk	#0
.LSM4:
	bclr.b	_INTCON1bits,#3
.L4:
.LSM5:
	bra	.L4
.LFE1:
	.align	2
	.global	__StackError	; export
	.type	__StackError,@function
__StackError:
.LFB2:
.LSM6:
	.set ___PA___,1
	lnk	#0
.LSM7:
	bclr.b	_INTCON1bits,#2
.L6:
.LSM8:
	bra	.L6
.LFE2:
	.align	2
	.global	__MathError	; export
	.type	__MathError,@function
__MathError:
.LFB3:
.LSM9:
	.set ___PA___,1
	lnk	#0
.LSM10:
	bclr.b	_INTCON1bits,#4
.L8:
.LSM11:
	bra	.L8
.LFE3:
	.align	2
	.global	__DMACError	; export
	.type	__DMACError,@function
__DMACError:
.LFB4:
.LSM12:
	.set ___PA___,1
	lnk	#0
.LSM13:
	bclr.b	_INTCON1bits,#5
.L10:
.LSM14:
	bra	.L10
.LFE4:
	.align	2
	.global	__AltOscillatorFail	; export
	.type	__AltOscillatorFail,@function
__AltOscillatorFail:
.LFB5:
.LSM15:
	.set ___PA___,1
	lnk	#0
.LSM16:
	bclr.b	_INTCON1bits,#1
.L12:
.LSM17:
	bra	.L12
.LFE5:
	.align	2
	.global	__AltAddressError	; export
	.type	__AltAddressError,@function
__AltAddressError:
.LFB6:
.LSM18:
	.set ___PA___,1
	lnk	#0
.LSM19:
	bclr.b	_INTCON1bits,#3
.L14:
.LSM20:
	bra	.L14
.LFE6:
	.align	2
	.global	__AltStackError	; export
	.type	__AltStackError,@function
__AltStackError:
.LFB7:
.LSM21:
	.set ___PA___,1
	lnk	#0
.LSM22:
	bclr.b	_INTCON1bits,#2
.L16:
.LSM23:
	bra	.L16
.LFE7:
	.align	2
	.global	__AltMathError	; export
	.type	__AltMathError,@function
__AltMathError:
.LFB8:
.LSM24:
	.set ___PA___,1
	lnk	#0
.LSM25:
	bclr.b	_INTCON1bits,#4
.L18:
.LSM26:
	bra	.L18
.LFE8:
	.align	2
	.global	__AltDMACError	; export
	.type	__AltDMACError,@function
__AltDMACError:
.LFB9:
.LSM27:
	.set ___PA___,1
	lnk	#0
.LSM28:
	bclr.b	_INTCON1bits,#5
.L20:
.LSM29:
	bra	.L20
.LFE9:
	.align	2
	.global	__DefaultInterrupt	; export
	.type	__DefaultInterrupt,@function
__DefaultInterrupt:
.LFB10:
.LSM30:
	.set ___PA___,1
	lnk	#0
.L22:
.LSM31:
	bra	.L22
.LFE10:
	.section	.debug_frame,info
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.byte	0
	.uleb128 0x1
	.sleb128 2
	.byte	0x21
	.byte	0x12
	.uleb128 0xf
	.sleb128 -2
	.byte	0x9
	.uleb128 0x21
	.uleb128 0xf
	.align	4
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB0
	.4byte	.LFE0-.LFB0
	.align	4
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB1
	.4byte	.LFE1-.LFB1
	.align	4
.LEFDE2:
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
	.align	4
.LEFDE4:
.LSFDE6:
	.4byte	.LEFDE6-.LASFDE6
.LASFDE6:
	.4byte	.Lframe0
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
	.align	4
.LEFDE6:
.LSFDE8:
	.4byte	.LEFDE8-.LASFDE8
.LASFDE8:
	.4byte	.Lframe0
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
	.align	4
.LEFDE8:
.LSFDE10:
	.4byte	.LEFDE10-.LASFDE10
.LASFDE10:
	.4byte	.Lframe0
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
	.align	4
.LEFDE10:
.LSFDE12:
	.4byte	.LEFDE12-.LASFDE12
.LASFDE12:
	.4byte	.Lframe0
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
	.align	4
.LEFDE12:
.LSFDE14:
	.4byte	.LEFDE14-.LASFDE14
.LASFDE14:
	.4byte	.Lframe0
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
	.align	4
.LEFDE14:
.LSFDE16:
	.4byte	.LEFDE16-.LASFDE16
.LASFDE16:
	.4byte	.Lframe0
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
	.align	4
.LEFDE16:
.LSFDE18:
	.4byte	.LEFDE18-.LASFDE18
.LASFDE18:
	.4byte	.Lframe0
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
	.align	4
.LEFDE18:
.LSFDE20:
	.4byte	.LEFDE20-.LASFDE20
.LASFDE20:
	.4byte	.Lframe0
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
	.align	4
.LEFDE20:
	.section	.text,code
.Letext0:
	.section	.debug_info,info
	.4byte	0x40d
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.asciz	"GNU C 4.5.1 (XC16, Microchip v1_10) (A) Build date: Jul 26 2012"
	.byte	0x1
	.asciz	"traps.c"
	.ascii	"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkW"
	.asciz	"hitehorn/educational/fixedPointTrig.X"
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.asciz	"unsigned int"
	.uleb128 0x3
	.asciz	"tagINTCON1BITS"
	.byte	0x2
	.byte	0x2
	.byte	0xe9
	.4byte	0x225
	.uleb128 0x4
	.asciz	"OSCFAIL"
	.byte	0x2
	.byte	0xeb
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0xe
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"STKERR"
	.byte	0x2
	.byte	0xec
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0xd
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"ADDRERR"
	.byte	0x2
	.byte	0xed
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0xc
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"MATHERR"
	.byte	0x2
	.byte	0xee
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0xb
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"DMACERR"
	.byte	0x2
	.byte	0xef
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0xa
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"DIV0ERR"
	.byte	0x2
	.byte	0xf0
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x9
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"SFTACERR"
	.byte	0x2
	.byte	0xf1
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"COVTE"
	.byte	0x2
	.byte	0xf2
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x7
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"OVBTE"
	.byte	0x2
	.byte	0xf3
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x6
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"OVATE"
	.byte	0x2
	.byte	0xf4
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x5
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"COVBERR"
	.byte	0x2
	.byte	0xf5
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x4
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"COVAERR"
	.byte	0x2
	.byte	0xf6
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x3
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"OVBERR"
	.byte	0x2
	.byte	0xf7
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x2
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"OVAERR"
	.byte	0x2
	.byte	0xf8
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x1
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"NSTDIS"
	.byte	0x2
	.byte	0xf9
	.4byte	0xcb
	.byte	0x2
	.byte	0x1
	.byte	0x10
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0x5
	.asciz	"INTCON1BITS"
	.byte	0x2
	.byte	0xfa
	.4byte	0xdb
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.asciz	"signed char"
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.asciz	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.asciz	"long int"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.asciz	"long long int"
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.asciz	"unsigned char"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.asciz	"long unsigned int"
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.asciz	"long long unsigned int"
	.uleb128 0x6
	.byte	0x1
	.asciz	"_OscillatorFail"
	.byte	0x1
	.byte	0x3e
	.byte	0x1
	.4byte	.LFB0
	.4byte	.LFE0
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_AddressError"
	.byte	0x1
	.byte	0x44
	.byte	0x1
	.4byte	.LFB1
	.4byte	.LFE1
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_StackError"
	.byte	0x1
	.byte	0x49
	.byte	0x1
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_MathError"
	.byte	0x1
	.byte	0x4f
	.byte	0x1
	.4byte	.LFB3
	.4byte	.LFE3
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_DMACError"
	.byte	0x1
	.byte	0x55
	.byte	0x1
	.4byte	.LFB4
	.4byte	.LFE4
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_AltOscillatorFail"
	.byte	0x1
	.byte	0x5e
	.byte	0x1
	.4byte	.LFB5
	.4byte	.LFE5
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_AltAddressError"
	.byte	0x1
	.byte	0x64
	.byte	0x1
	.4byte	.LFB6
	.4byte	.LFE6
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_AltStackError"
	.byte	0x1
	.byte	0x6a
	.byte	0x1
	.4byte	.LFB7
	.4byte	.LFE7
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_AltMathError"
	.byte	0x1
	.byte	0x70
	.byte	0x1
	.4byte	.LFB8
	.4byte	.LFE8
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_AltDMACError"
	.byte	0x1
	.byte	0x76
	.byte	0x1
	.4byte	.LFB9
	.4byte	.LFE9
	.byte	0x1
	.byte	0x5e
	.uleb128 0x6
	.byte	0x1
	.asciz	"_DefaultInterrupt"
	.byte	0x1
	.byte	0x84
	.byte	0x1
	.4byte	.LFB10
	.4byte	.LFE10
	.byte	0x1
	.byte	0x5e
	.uleb128 0x7
	.4byte	.LASF0
	.byte	0x2
	.byte	0xfb
	.4byte	0x3fe
	.byte	0x1
	.byte	0x1
	.uleb128 0x8
	.4byte	0x225
	.uleb128 0x7
	.4byte	.LASF0
	.byte	0x2
	.byte	0xfb
	.4byte	0x3fe
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.section	.debug_abbrev,info
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0x8
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x1b
	.uleb128 0x8
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0xd
	.uleb128 0xb
	.uleb128 0xc
	.uleb128 0xb
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,info
	.4byte	0xdb
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x411
	.4byte	0x2ab
	.asciz	"_OscillatorFail"
	.4byte	0x2ca
	.asciz	"_AddressError"
	.4byte	0x2e7
	.asciz	"_StackError"
	.4byte	0x302
	.asciz	"_MathError"
	.4byte	0x31c
	.asciz	"_DMACError"
	.4byte	0x336
	.asciz	"_AltOscillatorFail"
	.4byte	0x358
	.asciz	"_AltAddressError"
	.4byte	0x378
	.asciz	"_AltStackError"
	.4byte	0x396
	.asciz	"_AltMathError"
	.4byte	0x3b3
	.asciz	"_AltDMACError"
	.4byte	0x3d0
	.asciz	"_DefaultInterrupt"
	.4byte	0x0
	.section	.debug_pubtypes,info
	.4byte	0x31
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x411
	.4byte	0xdb
	.asciz	"tagINTCON1BITS"
	.4byte	0x225
	.asciz	"INTCON1BITS"
	.4byte	0x0
	.section	.debug_aranges,info
	.4byte	0x14
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	0x0
	.4byte	0x0
	.section	.debug_line,info
	.4byte	.LELT0-.LSLT0
.LSLT0:
	.2byte	0x2
	.4byte	.LELTP0-.LASLTP0
.LASLTP0:
	.byte	0x1
	.byte	0x1
	.byte	0xf6
	.byte	0xf5
	.byte	0xa
	.byte	0x0
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.byte	0x1
	.ascii	"/opt/microchip/xc16/v1.10/bin/bin/../../support/dsPIC33F/h"
	.byte 0
	.byte	0x0
	.asciz	"traps.c"
	.uleb128 0x0
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"p33FJ256GP710A.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.byte	0x0
.LELTP0:
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.Letext0
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM0
	.byte	0x52
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM1
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM2
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE0
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM3
	.byte	0x58
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM4
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM5
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE1
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM6
	.byte	0x5d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM7
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM8
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE2
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM9
	.byte	0x63
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM10
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM11
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE3
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM12
	.byte	0x69
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM13
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM14
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE4
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM15
	.byte	0x72
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM16
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM17
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE5
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM18
	.byte	0x78
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM19
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM20
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE6
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM21
	.byte	0x7e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM22
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM23
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE7
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM24
	.byte	0x84
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM25
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM26
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE8
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM27
	.byte	0x8a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM28
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM29
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE9
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM30
	.byte	0x98
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM31
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE10
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_str,info
.LASF0:
	.asciz	"INTCON1bits"
	.section	.text,code

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

; MCHP configuration words

	.set ___PA___,0
	.end
