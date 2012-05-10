	.file "../../libDCM/estWind.c"
	.section	.debug_abbrev,info
.Ldebug_abbrev0:
	.section	.debug_info,info
.Ldebug_info0:
	.section	.debug_line,info
.Ldebug_line0:
	.section	.text,code
.Ltext0:
	.global	_groundVelocityHistory	; export
	.section	.ndata,data,near
	.align	2
	.type	_groundVelocityHistory,@object
_groundVelocityHistory:
	.skip	6
	.global	_fuselageDirectionHistory	; export
	.align	2
	.type	_fuselageDirectionHistory,@object
_fuselageDirectionHistory:
	.skip	6
	.global	_estimatedWind	; export
	.align	2
	.type	_estimatedWind,@object
_estimatedWind:
	.skip	6
	.section	.text,code
	.align	2
	.global	_estimateWind	; export
	.type	_estimateWind,@function
_estimateWind:
.LFB2:
.LM1:
	.set ___PA___,1
	lnk	#0
.LM2:
	ulnk	
	return	
	.set ___PA___,0
.LFE2:
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
	.byte	0xc
	.uleb128 0xf
	.uleb128 0xfffffffc
	.byte	0x9
	.uleb128 0x21
	.uleb128 0xf
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB2
	.4byte	.LFE2-.LFB2
.LEFDE0:
	.section	.text,code
.Letext0:
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
	.ascii	"../../libDCM"
	.byte 0
	.ascii	"/opt/microchip/mplabc30/v3.30c/bin/bin/../../include"
	.byte 0
	.ascii	"/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/dsPIC33F/h"
	.byte 0
	.ascii	"/opt/microchip/mplabc30/v3.30c/bin/bin/../../support/generic/h"
	.byte 0
	.byte	0x0
	.asciz	"../libUDB/libUDB_defines.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"p33FJ256GP710A.h"
	.uleb128 0x3
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"stddef.h"
	.uleb128 0x2
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"stdlib.h"
	.uleb128 0x2
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"dsp.h"
	.uleb128 0x4
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"../libUDB/libUDB.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"libDCM_defines.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"libDCM.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"estWind.c"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.byte	0x0
.LELTP0:
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM1
	.byte	0x4
	.uleb128 0x9
	.byte	0x33
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM2
	.byte	0x70
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.Letext0
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_info,info
	.4byte	0x1d7
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.Ldebug_line0
	.4byte	.Letext0
	.4byte	.Ltext0
	.asciz	"GNU C 4.0.3 (dsPIC30, Microchip v3_30) (A) Build date: Jun  2 2011"
	.byte	0x1
	.asciz	"../../libDCM/estWind.c"
	.ascii	"/home/markw/Dropbox/autopilots/UAVdevboard/UDB4/mplabx_projects/MPQp"
	.asciz	"id400/MatrixPilotQuad/MPQpid400.X"
	.uleb128 0x2
	.asciz	"unsigned char"
	.byte	0x1
	.byte	0x8
	.uleb128 0x2
	.asciz	"int"
	.byte	0x2
	.byte	0x5
	.uleb128 0x2
	.asciz	"long int"
	.byte	0x4
	.byte	0x5
	.uleb128 0x3
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7
	.uleb128 0x2
	.asciz	"char"
	.byte	0x1
	.byte	0x6
	.uleb128 0x2
	.asciz	"short unsigned int"
	.byte	0x2
	.byte	0x7
	.uleb128 0x2
	.asciz	"long unsigned int"
	.byte	0x4
	.byte	0x7
	.uleb128 0x2
	.asciz	"float"
	.byte	0x4
	.byte	0x4
	.uleb128 0x4
	.byte	0x1
	.asciz	"estimateWind"
	.byte	0x9
	.byte	0x20
	.byte	0x1
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x1
	.byte	0x5e
	.uleb128 0x5
	.4byte	0x16c
	.4byte	0xea
	.uleb128 0x6
	.4byte	0x16c
	.byte	0x2
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7
	.uleb128 0x7
	.asciz	"estimatedWind"
	.byte	0x9
	.byte	0x1b
	.4byte	0x15c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_estimatedWind
	.uleb128 0x7
	.asciz	"groundVelocityHistory"
	.byte	0x9
	.byte	0x18
	.4byte	0x15c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_groundVelocityHistory
	.uleb128 0x7
	.asciz	"fuselageDirectionHistory"
	.byte	0x9
	.byte	0x19
	.4byte	0x15c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_fuselageDirectionHistory
	.byte	0x0
	.section	.debug_abbrev,info
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x10
	.uleb128 0x6
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x25
	.uleb128 0x8
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x1b
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
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
	.uleb128 0x5
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,info
	.4byte	0x68
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x1db
	.4byte	0x140
	.asciz	"estimateWind"
	.4byte	0x173
	.asciz	"estimatedWind"
	.4byte	0x18f
	.asciz	"groundVelocityHistory"
	.4byte	0x1b3
	.asciz	"fuselageDirectionHistory"
	.4byte	0x0
	.section	.debug_aranges,info
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,info
.LASF0:
	.asciz	"unsigned int"

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

	.set ___PA___,0
	.end
