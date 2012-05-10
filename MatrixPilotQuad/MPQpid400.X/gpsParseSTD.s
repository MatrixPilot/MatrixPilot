	.file "../../libDCM/gpsParseSTD.c"
	.section	.debug_abbrev,info
.Ldebug_abbrev0:
	.section	.debug_info,info
.Ldebug_info0:
	.section	.debug_line,info
.Ldebug_line0:
	.section	.text,code
.Ltext0:
	.global	_bin_mode	; export
	.section	.const,psv,page
	.type	_bin_mode,@object
_bin_mode:
	.asciz	"$PSRF100,0,19200,8,1,0*39\015\012"
	.global	_mode_length	; export
	.section	.const,psv,page
	.align	2
	.type	_mode_length,@object
_mode_length:
	.word	9
	.global	_mode	; export
	.section	.const,psv,page
	.type	_mode,@object
_mode:
	.byte -122
	.byte 0
	.byte 0
	.byte 75
	.byte 0
	.byte 8
	.byte 1
	.byte 0
	.byte 0
	.global	_msg_parse	; export
	.section	.ndata,data,near
	.align	2
	.type	_msg_parse,@object
_msg_parse:
	.word	handle(_msg_B3)
	.global	_msg41parse	; export
	.section	.const,psv,page
	.align	2
	.type	_msg41parse,@object
_msg41parse:
	.word	_nav_valid_+1
	.word	_nav_valid_
	.word	_nav_type_+1
	.word	_nav_type_
	.word	_week_no_+1
	.word	_week_no_
	.word	_tow_+3
	.word	_tow_+2
	.word	_tow_+1
	.word	_tow_
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_lat_gps_+3
	.word	_lat_gps_+2
	.word	_lat_gps_+1
	.word	_lat_gps_
	.word	_long_gps_+3
	.word	_long_gps_+2
	.word	_long_gps_+1
	.word	_long_gps_
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_alt_sl_gps_+3
	.word	_alt_sl_gps_+2
	.word	_alt_sl_gps_+1
	.word	_alt_sl_gps_
	.word	_un
	.word	_sog_gps_+1
	.word	_sog_gps_
	.word	_cog_gps_+1
	.word	_cog_gps_
	.word	_un
	.word	_un
	.word	_climb_gps_+1
	.word	_climb_gps_
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_un
	.word	_svs_
	.word	_hdop_
	.word	_un
	.word	_un
	.word	_un
	.section	.text,code
	.align	2
	.global	_gps_nav_valid	; export
	.type	_gps_nav_valid,@function
_gps_nav_valid:
.LFB2:
.LM1:
	.set ___PA___,1
	lnk	#2
.LM2:
	mov	_nav_valid_,w0
	clr	w1
	mov	w1,[w14]
	cp0	w0
	.set ___BP___,0
	bra	nz,.L2
	mov	#1,w0
	mov	w0,[w14]
.L2:
	mov	[w14],w0
.LM3:
	ulnk	
	return	
	.set ___PA___,0
.LFE2:
	.align	2
	.global	_gps_startup_sequence	; export
	.type	_gps_startup_sequence,@function
_gps_startup_sequence:
.LFB3:
.LM4:
	.set ___PA___,1
	lnk	#2
	mov	w0,[w14]
.LM5:
	mov	#400,w0
	subr	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L5
.LM6:
	mov	#4800,w0
	mov	#0,w1
	call	_udb_gps_set_rate
	bra	.L13
.L5:
.LM7:
	mov	#300,w0
	subr	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L8
.LM8:
	mov	#_bin_mode,w0
	call	_gpsoutline
	bra	.L13
.L8:
.LM9:
	mov	#200,w0
	subr	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L10
.LM10:
	mov	_mode_length,w0
	mov	#_mode,w1
	call	_gpsoutbin
	bra	.L13
.L10:
.LM11:
	mov	#100,w0
	subr	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L13
.LM12:
	mov	#19200,w0
	mov	#0,w1
	call	_udb_gps_set_rate
.L13:
.LM13:
	ulnk	
	return	
	.set ___PA___,0
.LFE3:
	.global	_store_index	; export
	.section	.ndata,data,near
	.align	2
	.type	_store_index,@object
_store_index:
	.skip	2
	.section	.text,code
	.align	2
	.global	_msg_B3	; export
	.type	_msg_B3,@function
_msg_B3:
.LFB4:
.LM14:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM15:
	mov.b	#-96,w0
	subr.b	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L17
.LM16:
	mov	#handle(_msg_A0),w0
	mov	w0,_msg_parse
.L17:
.LM17:
	ulnk	
	return	
	.set ___PA___,0
.LFE4:
	.align	2
	.global	_msg_A0	; export
	.type	_msg_A0,@function
_msg_A0:
.LFB5:
.LM18:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM19:
	mov.b	#-94,w0
	subr.b	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L19
.LM20:
	clr	w0
	mov	w0,_store_index
.LM21:
	mov	#handle(_msg_A2),w0
	mov	w0,_msg_parse
	bra	.L22
.L19:
.LM22:
	mov	#handle(_msg_B3),w0
	mov	w0,_msg_parse
.L22:
.LM23:
	ulnk	
	return	
	.set ___PA___,0
.LFE5:
	.align	2
	.global	_msg_A2	; export
	.type	_msg_A2,@function
_msg_A2:
.LFB6:
.LM24:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM25:
	mov.b	[w14],w0
	mov.b	WREG,_payloadlength+1
.LM26:
	mov	#handle(_msg_PL1),w0
	mov	w0,_msg_parse
.LM27:
	ulnk	
	return	
	.set ___PA___,0
.LFE6:
	.align	2
	.global	_msg_PL1	; export
	.type	_msg_PL1,@function
_msg_PL1:
.LFB7:
.LM28:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM29:
	mov.b	[w14],w0
	mov.b	WREG,_payloadlength
.LM30:
	mov	_payloadlength,w0
	inc	w0,w0
	mov	w0,_payloadlength
.LM31:
	mov	#handle(_msg_PL2),w0
	mov	w0,_msg_parse
.LM32:
	ulnk	
	return	
	.set ___PA___,0
.LFE7:
	.align	2
	.global	_msg_PL2	; export
	.type	_msg_PL2,@function
_msg_PL2:
.LFB8:
.LM33:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM34:
	ze	[w14],w1
	mov	#41,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	z,.L29
	bra	.L28
.L29:
.LM35:
	mov	_payloadlength,w1
	mov	#92,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	nz,.L30
.LM36:
	mov	#handle(_msg_MSG41),w0
	mov	w0,_msg_parse
	bra	.L34
.L30:
.LM37:
	mov	#handle(_msg_B3),w0
	mov	w0,_msg_parse
.LM38:
	bra	.L34
.L28:
.LM39:
	mov	#handle(_msg_MSGU),w0
	mov	w0,_msg_parse
.L34:
.LM40:
	ulnk	
	return	
	.set ___PA___,0
.LFE8:
	.align	2
	.global	_msg_MSG41	; export
	.type	_msg_MSG41,@function
_msg_MSG41:
.LFB9:
.LM41:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM42:
	mov	_payloadlength,w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L36
.LM43:
	mov	_store_index,w1
	add	w1,w1,w2
	mov	#_msg41parse,w0
	add	w2,w0,w0
	mov	[w0],w0
	mov.b	[w14],[w0]
	inc	w1,w0
	mov	w0,_store_index
.LM44:
	mov	_payloadlength,w0
	dec	w0,w0
	mov	w0,_payloadlength
	bra	.L41
.L36:
.LM45:
	call	_udb_background_trigger
.LM46:
	mov.b	#-80,w0
	subr.b	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L39
.LM47:
	mov	#handle(_msg_B0),w0
	mov	w0,_msg_parse
	bra	.L41
.L39:
.LM48:
	mov	#handle(_msg_B3),w0
	mov	w0,_msg_parse
.L41:
.LM49:
	ulnk	
	return	
	.set ___PA___,0
.LFE9:
	.align	2
	.global	_msg_MSGU	; export
	.type	_msg_MSGU,@function
_msg_MSGU:
.LFB10:
.LM50:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM51:
	mov	_payloadlength,w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L43
.LM52:
	mov	_payloadlength,w0
	dec	w0,w0
	mov	w0,_payloadlength
	bra	.L48
.L43:
.LM53:
	mov.b	#-80,w0
	subr.b	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L46
.LM54:
	mov	#handle(_msg_B0),w0
	mov	w0,_msg_parse
	bra	.L48
.L46:
.LM55:
	mov	#handle(_msg_B3),w0
	mov	w0,_msg_parse
.L48:
.LM56:
	ulnk	
	return	
	.set ___PA___,0
.LFE10:
	.align	2
	.global	_msg_B0	; export
	.type	_msg_B0,@function
_msg_B0:
.LFB11:
.LM57:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM58:
	mov.b	#-77,w0
	subr.b	w0,[w14],[w15]
	.set ___BP___,0
	bra	nz,.L50
.LM59:
	mov	#handle(_msg_B3),w0
	mov	w0,_msg_parse
	bra	.L53
.L50:
.LM60:
	mov	#handle(_msg_B3),w0
	mov	w0,_msg_parse
.L53:
.LM61:
	ulnk	
	return	
	.set ___PA___,0
.LFE11:
	.align	2
	.global	_commit_gps_data	; export
	.type	_commit_gps_data,@function
_commit_gps_data:
.LFB12:
.LM62:
	.set ___PA___,1
	lnk	#0
.LM63:
	mov	_week_no_,w0
	mov	w0,_week_no
.LM64:
	mov	_tow_,w0
	mov	_tow_+2,w1
	mov	w0,_tow
	mov	w1,_tow+2
.LM65:
	mov	_lat_gps_,w0
	mov	_lat_gps_+2,w1
	mov	w0,_lat_gps
	mov	w1,_lat_gps+2
.LM66:
	mov	_long_gps_,w0
	mov	_long_gps_+2,w1
	mov	w0,_long_gps
	mov	w1,_long_gps+2
.LM67:
	mov	_alt_sl_gps_,w0
	mov	_alt_sl_gps_+2,w1
	mov	w0,_alt_sl_gps
	mov	w1,_alt_sl_gps+2
.LM68:
	mov	_sog_gps_,w1
	mov	w1,_sog_gps
.LM69:
	mov	_cog_gps_,w0
	mov	w0,_cog_gps
.LM70:
	mov	_climb_gps_,w1
	mov	w1,_climb_gps
.LM71:
	mov.b	_hdop_,WREG
	mov.b	WREG,_hdop
.LM72:
	mov.b	_svs_,WREG
	mov.b	WREG,_svs
.LM73:
	ulnk	
	return	
	.set ___PA___,0
.LFE12:
	.section	.nbss,bss,near
	.type	_payloadlength,@object
	.global	_payloadlength
	.align	2
_payloadlength:	.space	2
	.type	_un,@object
	.global	_un
_un:	.space	1
	.type	_svs_,@object
	.global	_svs_
_svs_:	.space	1
	.type	_lat_gps_,@object
	.global	_lat_gps_
	.align	2
_lat_gps_:	.space	4
	.type	_long_gps_,@object
	.global	_long_gps_
	.align	2
_long_gps_:	.space	4
	.type	_alt_sl_gps_,@object
	.global	_alt_sl_gps_
	.align	2
_alt_sl_gps_:	.space	4
	.type	_tow_,@object
	.global	_tow_
	.align	2
_tow_:	.space	4
	.type	_nav_valid_,@object
	.global	_nav_valid_
	.align	2
_nav_valid_:	.space	2
	.type	_nav_type_,@object
	.global	_nav_type_
	.align	2
_nav_type_:	.space	2
	.type	_sog_gps_,@object
	.global	_sog_gps_
	.align	2
_sog_gps_:	.space	2
	.type	_cog_gps_,@object
	.global	_cog_gps_
	.align	2
_cog_gps_:	.space	2
	.type	_climb_gps_,@object
	.global	_climb_gps_
	.align	2
_climb_gps_:	.space	2
	.type	_week_no_,@object
	.global	_week_no_
	.align	2
_week_no_:	.space	2
	.type	_hdop_,@object
	.global	_hdop_
_hdop_:	.space	1
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
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB3
	.4byte	.LFE3-.LFB3
.LEFDE2:
.LSFDE4:
	.4byte	.LEFDE4-.LASFDE4
.LASFDE4:
	.4byte	.Lframe0
	.4byte	.LFB4
	.4byte	.LFE4-.LFB4
.LEFDE4:
.LSFDE6:
	.4byte	.LEFDE6-.LASFDE6
.LASFDE6:
	.4byte	.Lframe0
	.4byte	.LFB5
	.4byte	.LFE5-.LFB5
.LEFDE6:
.LSFDE8:
	.4byte	.LEFDE8-.LASFDE8
.LASFDE8:
	.4byte	.Lframe0
	.4byte	.LFB6
	.4byte	.LFE6-.LFB6
.LEFDE8:
.LSFDE10:
	.4byte	.LEFDE10-.LASFDE10
.LASFDE10:
	.4byte	.Lframe0
	.4byte	.LFB7
	.4byte	.LFE7-.LFB7
.LEFDE10:
.LSFDE12:
	.4byte	.LEFDE12-.LASFDE12
.LASFDE12:
	.4byte	.Lframe0
	.4byte	.LFB8
	.4byte	.LFE8-.LFB8
.LEFDE12:
.LSFDE14:
	.4byte	.LEFDE14-.LASFDE14
.LASFDE14:
	.4byte	.Lframe0
	.4byte	.LFB9
	.4byte	.LFE9-.LFB9
.LEFDE14:
.LSFDE16:
	.4byte	.LEFDE16-.LASFDE16
.LASFDE16:
	.4byte	.Lframe0
	.4byte	.LFB10
	.4byte	.LFE10-.LFB10
.LEFDE16:
.LSFDE18:
	.4byte	.LEFDE18-.LASFDE18
.LASFDE18:
	.4byte	.Lframe0
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
.LEFDE18:
.LSFDE20:
	.4byte	.LEFDE20-.LASFDE20
.LASFDE20:
	.4byte	.Lframe0
	.4byte	.LFB12
	.4byte	.LFE12-.LFB12
.LEFDE20:
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
	.asciz	"gpsParseSTD.c"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"libDCM_internal.h"
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
	.byte	0x8b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM2
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM3
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM4
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM5
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM6
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM7
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM8
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM9
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM10
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM11
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM12
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM13
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM14
	.byte	0x39
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM15
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM16
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM17
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM18
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM19
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM20
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM21
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM22
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM23
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM24
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM25
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM26
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM27
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM28
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM29
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM30
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM31
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM32
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM33
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM34
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM35
	.byte	0x23
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM36
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM37
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM38
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM39
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM40
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM41
	.byte	0x2e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM42
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM43
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM44
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM45
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM46
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM47
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM48
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM49
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM50
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM51
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM52
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM53
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM54
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM55
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM56
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM57
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM58
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM59
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM60
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM61
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM62
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM63
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM64
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM65
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM66
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM67
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM68
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM69
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM70
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM71
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM72
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM73
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.Letext0
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_info,info
	.4byte	0x6d7
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.Ldebug_line0
	.4byte	.Letext0
	.4byte	.Ltext0
	.asciz	"GNU C 4.0.3 (dsPIC30, Microchip v3_30) (A) Build date: Jun  2 2011"
	.byte	0x1
	.asciz	"../../libDCM/gpsParseSTD.c"
	.ascii	"/home/markw/Dropbox/autopilots/UAVdevboard/UDB4/mplabx_projects/MPQp"
	.asciz	"id400/MatrixPilotQuad/MPQpid400.X"
	.uleb128 0x2
	.4byte	0x103
	.asciz	"bb"
	.byte	0x2
	.byte	0x1
	.byte	0x1b
	.uleb128 0x3
	.asciz	"B0"
	.byte	0x1
	.byte	0x1b
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x3
	.asciz	"B1"
	.byte	0x1
	.byte	0x1b
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.byte	0x0
	.uleb128 0x4
	.asciz	"unsigned char"
	.byte	0x1
	.byte	0x8
	.uleb128 0x2
	.4byte	0x156
	.asciz	"bbbb"
	.byte	0x4
	.byte	0x1
	.byte	0x1c
	.uleb128 0x3
	.asciz	"B0"
	.byte	0x1
	.byte	0x1c
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x3
	.asciz	"B1"
	.byte	0x1
	.byte	0x1c
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x3
	.asciz	"B2"
	.byte	0x1
	.byte	0x1c
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x3
	.asciz	"B3"
	.byte	0x1
	.byte	0x1c
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x3
	.byte	0x0
	.uleb128 0x2
	.4byte	0x17c
	.asciz	"ww"
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.uleb128 0x3
	.asciz	"W0"
	.byte	0x1
	.byte	0x1d
	.4byte	0x17c
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x3
	.asciz	"W1"
	.byte	0x1
	.byte	0x1d
	.4byte	0x17c
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x4
	.asciz	"int"
	.byte	0x2
	.byte	0x5
	.uleb128 0x5
	.4byte	0x1a5
	.asciz	"intbb"
	.byte	0x2
	.byte	0x1
	.byte	0x1f
	.uleb128 0x6
	.asciz	"BB"
	.byte	0x1
	.byte	0x1f
	.4byte	0x17c
	.uleb128 0x6
	.asciz	"_"
	.byte	0x1
	.byte	0x1f
	.4byte	0xdd
	.byte	0x0
	.uleb128 0x5
	.4byte	0x1d4
	.asciz	"longbbbb"
	.byte	0x4
	.byte	0x1
	.byte	0x20
	.uleb128 0x6
	.asciz	"WW"
	.byte	0x1
	.byte	0x20
	.4byte	0x1d4
	.uleb128 0x6
	.asciz	"_"
	.byte	0x1
	.byte	0x20
	.4byte	0x156
	.uleb128 0x6
	.asciz	"__"
	.byte	0x1
	.byte	0x20
	.4byte	0x114
	.byte	0x0
	.uleb128 0x4
	.asciz	"long int"
	.byte	0x4
	.byte	0x5
	.uleb128 0x7
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7
	.uleb128 0x8
	.asciz	"boolean"
	.byte	0x1
	.byte	0x9c
	.4byte	0x1f6
	.uleb128 0x4
	.asciz	"char"
	.byte	0x1
	.byte	0x6
	.uleb128 0x4
	.asciz	"short unsigned int"
	.byte	0x2
	.byte	0x7
	.uleb128 0x4
	.asciz	"long unsigned int"
	.byte	0x4
	.byte	0x7
	.uleb128 0x4
	.asciz	"float"
	.byte	0x4
	.byte	0x4
	.uleb128 0x9
	.byte	0x1
	.asciz	"gps_nav_valid"
	.byte	0x9
	.byte	0x78
	.byte	0x1
	.4byte	0x1e7
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x1
	.byte	0x5e
	.uleb128 0xa
	.4byte	0x28f
	.byte	0x1
	.asciz	"gps_startup_sequence"
	.byte	0x9
	.byte	0x7e
	.byte	0x1
	.4byte	.LFB3
	.4byte	.LFE3
	.byte	0x1
	.byte	0x5e
	.uleb128 0xb
	.asciz	"gpscount"
	.byte	0x9
	.byte	0x7d
	.4byte	0x1e0
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xa
	.4byte	0x2b8
	.byte	0x1
	.asciz	"msg_B3"
	.byte	0x9
	.byte	0xb1
	.byte	0x1
	.4byte	.LFB4
	.4byte	.LFE4
	.byte	0x1
	.byte	0x5e
	.uleb128 0xc
	.4byte	.LASF1
	.byte	0x9
	.byte	0xb0
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xa
	.4byte	0x2e1
	.byte	0x1
	.asciz	"msg_A0"
	.byte	0x9
	.byte	0xbe
	.byte	0x1
	.4byte	.LFB5
	.4byte	.LFE5
	.byte	0x1
	.byte	0x5e
	.uleb128 0xc
	.4byte	.LASF1
	.byte	0x9
	.byte	0xbd
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xa
	.4byte	0x30a
	.byte	0x1
	.asciz	"msg_A2"
	.byte	0x9
	.byte	0xcc
	.byte	0x1
	.4byte	.LFB6
	.4byte	.LFE6
	.byte	0x1
	.byte	0x5e
	.uleb128 0xc
	.4byte	.LASF1
	.byte	0x9
	.byte	0xcb
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xa
	.4byte	0x334
	.byte	0x1
	.asciz	"msg_PL1"
	.byte	0x9
	.byte	0xd3
	.byte	0x1
	.4byte	.LFB7
	.4byte	.LFE7
	.byte	0x1
	.byte	0x5e
	.uleb128 0xc
	.4byte	.LASF1
	.byte	0x9
	.byte	0xd2
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xa
	.4byte	0x35e
	.byte	0x1
	.asciz	"msg_PL2"
	.byte	0x9
	.byte	0xdb
	.byte	0x1
	.4byte	.LFB8
	.4byte	.LFE8
	.byte	0x1
	.byte	0x5e
	.uleb128 0xc
	.4byte	.LASF1
	.byte	0x9
	.byte	0xda
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xd
	.4byte	0x38c
	.byte	0x1
	.asciz	"msg_MSG41"
	.byte	0x9
	.2byte	0x116
	.byte	0x1
	.4byte	.LFB9
	.4byte	.LFE9
	.byte	0x1
	.byte	0x5e
	.uleb128 0xe
	.4byte	.LASF1
	.byte	0x9
	.2byte	0x115
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xd
	.4byte	0x3b9
	.byte	0x1
	.asciz	"msg_MSGU"
	.byte	0x9
	.2byte	0x12d
	.byte	0x1
	.4byte	.LFB10
	.4byte	.LFE10
	.byte	0x1
	.byte	0x5e
	.uleb128 0xe
	.4byte	.LASF1
	.byte	0x9
	.2byte	0x12c
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xd
	.4byte	0x3e4
	.byte	0x1
	.asciz	"msg_B0"
	.byte	0x9
	.2byte	0x141
	.byte	0x1
	.4byte	.LFB11
	.4byte	.LFE11
	.byte	0x1
	.byte	0x5e
	.uleb128 0xe
	.4byte	.LASF1
	.byte	0x9
	.2byte	0x140
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xf
	.byte	0x1
	.asciz	"commit_gps_data"
	.byte	0x9
	.2byte	0x14e
	.byte	0x1
	.4byte	.LFB12
	.4byte	.LFE12
	.byte	0x1
	.byte	0x5e
	.uleb128 0x10
	.asciz	"lat_gps"
	.byte	0x8
	.byte	0x75
	.4byte	0x1a5
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"long_gps"
	.byte	0x8
	.byte	0x75
	.4byte	0x1a5
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"alt_sl_gps"
	.byte	0x8
	.byte	0x75
	.4byte	0x1a5
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"tow"
	.byte	0xa
	.byte	0x34
	.4byte	0x1a5
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"sog_gps"
	.byte	0xa
	.byte	0x35
	.4byte	0x183
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"cog_gps"
	.byte	0xa
	.byte	0x35
	.4byte	0x183
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"climb_gps"
	.byte	0xa
	.byte	0x35
	.4byte	0x183
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"week_no"
	.byte	0xa
	.byte	0x35
	.4byte	0x183
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"hdop"
	.byte	0xa
	.byte	0x39
	.4byte	0x103
	.byte	0x1
	.byte	0x1
	.uleb128 0x10
	.asciz	"svs"
	.byte	0xa
	.byte	0x39
	.4byte	0x103
	.byte	0x1
	.byte	0x1
	.uleb128 0x11
	.asciz	"payloadlength"
	.byte	0x9
	.byte	0x21
	.4byte	0x183
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_payloadlength
	.uleb128 0x12
	.4byte	0x4d5
	.4byte	0x1f6
	.uleb128 0x13
	.4byte	0x4d5
	.byte	0x1b
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7
	.uleb128 0x11
	.asciz	"bin_mode"
	.byte	0x9
	.byte	0x2d
	.4byte	0x4f3
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_bin_mode
	.uleb128 0x14
	.4byte	0x4c5
	.uleb128 0x11
	.asciz	"mode_length"
	.byte	0x9
	.byte	0x2f
	.4byte	0x512
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_mode_length
	.uleb128 0x14
	.4byte	0x1e0
	.uleb128 0x12
	.4byte	0x527
	.4byte	0x103
	.uleb128 0x13
	.4byte	0x4d5
	.byte	0x8
	.byte	0x0
	.uleb128 0x11
	.asciz	"mode"
	.byte	0x9
	.byte	0x30
	.4byte	0x53a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_mode
	.uleb128 0x14
	.4byte	0x517
	.uleb128 0x15
	.4byte	0x54b
	.byte	0x1
	.uleb128 0x16
	.4byte	0x103
	.byte	0x0
	.uleb128 0x11
	.asciz	"msg_parse"
	.byte	0x9
	.byte	0x38
	.4byte	0x563
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_msg_parse
	.uleb128 0x17
	.byte	0x2
	.4byte	0x53f
	.uleb128 0x11
	.asciz	"un"
	.byte	0x9
	.byte	0x3a
	.4byte	0x103
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_un
	.uleb128 0x11
	.asciz	"svs_"
	.byte	0x9
	.byte	0x3f
	.4byte	0x103
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_svs_
	.uleb128 0x11
	.asciz	"lat_gps_"
	.byte	0x9
	.byte	0x58
	.4byte	0x1a5
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_lat_gps_
	.uleb128 0x11
	.asciz	"long_gps_"
	.byte	0x9
	.byte	0x58
	.4byte	0x1a5
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_long_gps_
	.uleb128 0x11
	.asciz	"alt_sl_gps_"
	.byte	0x9
	.byte	0x58
	.4byte	0x1a5
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_alt_sl_gps_
	.uleb128 0x11
	.asciz	"tow_"
	.byte	0x9
	.byte	0x58
	.4byte	0x1a5
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_tow_
	.uleb128 0x11
	.asciz	"nav_valid_"
	.byte	0x9
	.byte	0x59
	.4byte	0x183
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_nav_valid_
	.uleb128 0x11
	.asciz	"nav_type_"
	.byte	0x9
	.byte	0x59
	.4byte	0x183
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_nav_type_
	.uleb128 0x11
	.asciz	"sog_gps_"
	.byte	0x9
	.byte	0x59
	.4byte	0x183
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_sog_gps_
	.uleb128 0x11
	.asciz	"cog_gps_"
	.byte	0x9
	.byte	0x59
	.4byte	0x183
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_cog_gps_
	.uleb128 0x11
	.asciz	"climb_gps_"
	.byte	0x9
	.byte	0x59
	.4byte	0x183
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_climb_gps_
	.uleb128 0x11
	.asciz	"week_no_"
	.byte	0x9
	.byte	0x59
	.4byte	0x183
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_week_no_
	.uleb128 0x11
	.asciz	"hdop_"
	.byte	0x9
	.byte	0x5a
	.4byte	0x103
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_hdop_
	.uleb128 0x12
	.4byte	0x69c
	.4byte	0x69c
	.uleb128 0x13
	.4byte	0x4d5
	.byte	0x5b
	.byte	0x0
	.uleb128 0x17
	.byte	0x2
	.4byte	0x103
	.uleb128 0x11
	.asciz	"msg41parse"
	.byte	0x9
	.byte	0x5c
	.4byte	0x6bb
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_msg41parse
	.uleb128 0x14
	.4byte	0x68c
	.uleb128 0x11
	.asciz	"store_index"
	.byte	0x9
	.byte	0xa9
	.4byte	0x17c
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_store_index
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
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
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
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
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
	.uleb128 0x5
	.uleb128 0x17
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
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
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
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
	.uleb128 0x8
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
	.uleb128 0x9
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
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
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
	.uleb128 0xb
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
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
	.uleb128 0xe
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
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
	.uleb128 0x10
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
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x11
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
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x27
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x16
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x17
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,info
	.4byte	0x1b2
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x6db
	.4byte	0x232
	.asciz	"gps_nav_valid"
	.4byte	0x253
	.asciz	"gps_startup_sequence"
	.4byte	0x28f
	.asciz	"msg_B3"
	.4byte	0x2b8
	.asciz	"msg_A0"
	.4byte	0x2e1
	.asciz	"msg_A2"
	.4byte	0x30a
	.asciz	"msg_PL1"
	.4byte	0x334
	.asciz	"msg_PL2"
	.4byte	0x35e
	.asciz	"msg_MSG41"
	.4byte	0x38c
	.asciz	"msg_MSGU"
	.4byte	0x3b9
	.asciz	"msg_B0"
	.4byte	0x3e4
	.asciz	"commit_gps_data"
	.4byte	0x4a9
	.asciz	"payloadlength"
	.4byte	0x4dc
	.asciz	"bin_mode"
	.4byte	0x4f8
	.asciz	"mode_length"
	.4byte	0x527
	.asciz	"mode"
	.4byte	0x54b
	.asciz	"msg_parse"
	.4byte	0x569
	.asciz	"un"
	.4byte	0x57a
	.asciz	"svs_"
	.4byte	0x58d
	.asciz	"lat_gps_"
	.4byte	0x5a4
	.asciz	"long_gps_"
	.4byte	0x5bc
	.asciz	"alt_sl_gps_"
	.4byte	0x5d6
	.asciz	"tow_"
	.4byte	0x5e9
	.asciz	"nav_valid_"
	.4byte	0x602
	.asciz	"nav_type_"
	.4byte	0x61a
	.asciz	"sog_gps_"
	.4byte	0x631
	.asciz	"cog_gps_"
	.4byte	0x648
	.asciz	"climb_gps_"
	.4byte	0x661
	.asciz	"week_no_"
	.4byte	0x678
	.asciz	"hdop_"
	.4byte	0x6a2
	.asciz	"msg41parse"
	.4byte	0x6c0
	.asciz	"store_index"
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
.LASF1:
	.asciz	"gpschar"
.LASF0:
	.asciz	"unsigned int"

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

	.set ___PA___,0
	.end
