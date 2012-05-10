	.file "../main.c"
	.section	.debug_abbrev,info
.Ldebug_abbrev0:
	.section	.debug_info,info
.Ldebug_info0:
	.section	.debug_line,info
.Ldebug_line0:
	.section	.text,code
.Ltext0:
	.global	_didCalibrate	; export
	.section	.ndata,data,near
	.type	_didCalibrate,@object
_didCalibrate:
	.skip	1
	.global	_max_tilt	; export
	.section	.const,psv,page
	.align	2
	.type	_max_tilt,@object
_max_tilt:
	.word	31
	.global	_flight_mode	; export
	.section	.ndata,data,near
	.align	2
	.type	_flight_mode,@object
_flight_mode:
	.skip	2
	.global	_udb_throttle_enable	; export
	.type	_udb_throttle_enable,@object
_udb_throttle_enable:
	.skip	1
	.global	_tailFlash	; export
	.align	2
	.type	_tailFlash,@object
_tailFlash:
	.skip	2
	.section	.text,code
	.align	2
	.global	_main	; export
	.type	_main,@function
_main:
.LFB2:
.LM1:
	.set ___PA___,1
	lnk	#0
.LM2:
	call	_udb_init
.LM3:
	call	_dcm_init
.LM4:
	mov	#_pid_gains,w1
	mov	#6,w2
	mov	#1024,w0
	call	_eeprom_SequentialRead
.LM5:
	mov	#25614,w0
	mov	#3,w1
	call	_udb_serial_set_rate
.LM6:
	bset.b	_LATEbits,#2
.LM7:
	bset.b	_LATDbits,#4
.LM8:
	mov	_max_tilt,w0
	call	_sine
	mov	#500,w2
	repeat	#18-1
	div.sw	w0,w2
	mov	w0,_commanded_tilt_gain
.LM9:
	call	_udb_run
.LM10:
	clr	w0
.LM11:
	ulnk	
	return	
	.set ___PA___,0
.LFE2:
	.align	2
	.global	_check_failsafe	; export
	.type	_check_failsafe,@function
_check_failsafe:
.LFB3:
.LM12:
	.set ___PA___,1
	lnk	#0
.LM13:
	mov	_udb_pwIn+10,w1
	mov	#2999,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L4
.LM14:
	mov.b	#1,w0
	mov.b	WREG,_udb_throttle_enable
	bra	.L7
.L4:
.LM15:
	clr.b	w0
	mov.b	WREG,_udb_throttle_enable
.L7:
.LM16:
	ulnk	
	return	
	.set ___PA___,0
.LFE3:
	.align	2
	.global	_check_flight_mode	; export
	.type	_check_flight_mode,@function
_check_flight_mode:
.LFB4:
.LM17:
	.set ___PA___,1
	lnk	#0
.LM18:
	mov	_udb_pwIn+12,w1
	mov	#2499,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L9
.LM19:
	mov	#2,w0
	mov	w0,_flight_mode
	bra	.L14
.L9:
.LM20:
	mov	_udb_pwIn+12,w1
	mov	#3499,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L12
.LM21:
	mov	#1,w0
	mov	w0,_flight_mode
	bra	.L14
.L12:
.LM22:
	clr	w0
	mov	w0,_flight_mode
.L14:
.LM23:
	ulnk	
	return	
	.set ___PA___,0
.LFE4:
	.align	2
	.global	_storeGain	; export
	.type	_storeGain,@function
_storeGain:
.LFB5:
.LM24:
	.set ___PA___,1
	lnk	#4
	mov	w0,[w14+2]
.LM25:
	mov	[w14+2],w0
	cp0	w0
	.set ___BP___,0
	bra	lt,.L19
	mov	[w14+2],w0
	sub	w0,#3,[w15]
	.set ___BP___,0
	bra	gt,.L19
.LBB2:
.LM26:
	mov	[w14+2],w0
	add	w0,w0,w0
	mov	w0,w1
	mov	#1024,w0
	add	w1,w0,w0
	mov	w0,[w14]
.LM27:
	mov	[w14+2],w0
	add	w0,w0,w1
	mov	#_pid_gains,w0
	add	w1,w0,w0
	mov	[w0],w0
	mov.b	w0,w1
	mov	[w14],w0
	inc	[w14],[w14]
	call	_eeprom_ByteWrite
.LM28:
	mov	[w14+2],w0
	add	w0,w0,w1
	mov	#_pid_gains,w0
	add	w1,w0,w0
	mov	[w0],w0
	lsr	w0,#8,w0
	mov.b	w0,w1
	mov	[w14],w0
	call	_eeprom_ByteWrite
.L19:
.LBE2:
.LM29:
	ulnk	
	return	
	.set ___PA___,0
.LFE5:
	.align	2
	.global	_storeGains	; export
	.type	_storeGains,@function
_storeGains:
.LFB6:
.LM30:
	.set ___PA___,1
	lnk	#0
.LM31:
	mov	#_pid_gains,w1
	mov.b	#8,w2
	mov	#1024,w0
	call	_eeprom_PageWrite
.LM32:
	ulnk	
	return	
	.set ___PA___,0
.LFE6:
	.align	2
	.global	_adjust_gain	; export
	.type	_adjust_gain,@function
_adjust_gain:
.LFB7:
.LM33:
	.set ___PA___,1
	lnk	#4
	mov	w0,[w14]
	mov	w1,[w14+2]
.LM34:
	mov	[w14+2],w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L23
.LM35:
	mov	#1,w0
	mov	w0,_tailFlash
.LM36:
	mov	#819,w0
	mov	w0,[w14+2]
	bra	.L25
.L23:
.LM37:
	mov	#2,w0
	mov	w0,_tailFlash
.LM38:
	mov	#-819,w0
	mov	w0,[w14+2]
.L25:
.LM39:
	mov	[w14],w3
	mov	[w14],w0
	add	w0,w0,w1
	mov	#_pid_gains,w0
	add	w1,w0,w0
	mov	[w0],w1
	mov	[w14+2],w0
	add	w1,w0,w2
	add	w3,w3,w1
	mov	#_pid_gains,w0
	add	w1,w0,w0
	mov	w2,[w0]
.LM40:
	mov	[w14],w2
	mov	[w14],w0
	add	w0,w0,w1
	mov	#_pid_gains,w0
	add	w1,w0,w0
	mov	[w0],w3
	add	w2,w2,w1
	mov	#_pid_gains,w0
	add	w1,w0,w0
	mov	w3,[w0]
.LM41:
	ulnk	
	return	
	.set ___PA___,0
.LFE7:
	.global	_gainAdjIndex	; export
	.section	.ndata,data,near
	.align	2
	.type	_gainAdjIndex,@object
_gainAdjIndex:
	.word	0
	.word	1
	.word	2
	.section	.nbss,bss,near
	.align	2
_lastGainChVal.9032:	.space	2
	.align	2
_gainIndex.9031:	.space	2
	.section	.ndata,data,near
	.align	2
	.type	_gainState.9030,@object
_gainState.9030:
	.word	-1
	.section	.text,code
	.align	2
	.global	_check_gain_adjust	; export
	.type	_check_gain_adjust,@function
_check_gain_adjust:
.LFB8:
.LM42:
	.set ___PA___,1
	lnk	#6
.LM43:
	mov	_gainState.9030,w0
	mov	w0,[w14+4]
	mov	[w14+4],w0
	cp0	w0
	.set ___BP___,0
	bra	z,.L30
	mov	[w14+4],w0
	sub	w0,#1,[w15]
	.set ___BP___,0
	bra	z,.L31
	mov	[w14+4],w0
	add	w0,#1,[w15]
	.set ___BP___,0
	bra	z,.L29
	bra	.L41
.L29:
.LM44:
	mov	_dcm_flags,w0
	mov	#-32768,w1
	and	w0,w1,w0
	cp0	w0
	.set ___BP___,0
	bra	z,.L41
.LM45:
	mov	_udb_pwIn+16,w0
	mov	w0,_lastGainChVal.9032
.LM46:
	clr	w0
	mov	w0,_gainState.9030
.LM47:
	bra	.L41
.L30:
.LM48:
	mov.b	_udb_throttle_enable,WREG
	cp0.b	w0
	.set ___BP___,0
	bra	z,.L41
.LM49:
	mov	#1,w0
	mov	w0,_gainState.9030
.LM50:
	mov	_udb_pwIn+16,w0
	mov	w0,_lastGainChVal.9032
.LM51:
	bra	.L41
.L31:
.LM52:
	mov	_flight_mode,w0
	add	w0,w0,w1
	mov	#_gainAdjIndex,w0
	add	w1,w0,w0
	mov	[w0],w0
	mov	w0,_gainIndex.9031
.LM53:
	mov.b	_udb_throttle_enable,WREG
	cp0.b	w0
	.set ___BP___,0
	bra	z,.L36
.LBB3:
.LM54:
	mov	_udb_pwIn+16,w1
	mov	_lastGainChVal.9032,w0
	sub	w1,w0,[w14]
.LM55:
	mov	[w14],w0
	mov	w0,[w14+2]
	mov	[w14+2],w0
	cp0	w0
	.set ___BP___,0
	bra	ge,.L38
	mov	[w14+2],w0
	neg	w0,w0
	mov	w0,[w14+2]
.L38:
	mov	[w14+2],w0
	sub	w0,#20,[w15]
	.set ___BP___,0
	bra	le,.L41
.LM56:
	mov	_udb_pwIn+16,w0
	mov	w0,_lastGainChVal.9032
.LM57:
	mov	_gainIndex.9031,w0
	mov	[w14],w1
	call	_adjust_gain
	bra	.L41
.L36:
.LBE3:
.LM58:
	clr	w0
	mov	w0,_gainState.9030
.LM59:
	mov	_gainIndex.9031,w0
	call	_storeGain
.LM60:
	mov.b	#1,w0
	mov.b	WREG,_sendGains
.L41:
.LM61:
	ulnk	
	return	
	.set ___PA___,0
.LFE8:
	.align	2
	.global	_update_pid_gains	; export
	.type	_update_pid_gains,@function
_update_pid_gains:
.LFB9:
.LM62:
	.set ___PA___,1
	lnk	#0
.LM63:
	mov	_udb_flags,w0
	mov	#128,w1
	and	w0,w1,w0
	cp0	w0
	.set ___BP___,0
	bra	z,.L45
.LM64:
	call	_check_failsafe
.LM65:
	call	_check_flight_mode
.LM66:
	call	_check_gain_adjust
.L45:
.LM67:
	ulnk	
	return	
	.set ___PA___,0
.LFE9:
	.section	.nbss,bss,near
	.align	2
_lastUptime.9073:	.space	2
	.section	.text,code
	.align	2
	.global	_run_background_task	; export
	.type	_run_background_task,@function
_run_background_task:
.LFB10:
.LM68:
	.set ___PA___,0
	lnk	#0
.LM69:
	mov	_uptime,w2
	mov	_uptime+2,w3
	mov	_lastUptime.9073,w0
	asr	w0,#15,w1
	sub	w2,w0,w2
	subb	w3,w1,w3
	mov	#19,w0
	mov	#0,w1
	sub	w2,w0,[w15]
	subb	w3,w1,[w15]
	.set ___BP___,0
	bra	leu,.L47
.LM70:
	mov	_uptime,w0
	mov	_uptime+2,w1
	mov	w0,_lastUptime.9073
.LM71:
	call	_update_pid_gains
.L47:
.LM72:
	pwrsav #1
.LM73:
	ulnk	
	return	
	.set ___PA___,0
.LFE10:
	.align	2
	.global	_udb_background_callback_periodic	; export
	.type	_udb_background_callback_periodic,@function
_udb_background_callback_periodic:
.LFB11:
.LM74:
	.set ___PA___,1
	lnk	#0
.LM75:
	mov.b	_didCalibrate,WREG
	cp0.b	w0
	.set ___BP___,0
	bra	nz,.L60
.LM76:
	mov.b	_LATEbits,WREG
	ze	w0,w0
	lsr	w0,w0
	btg.b	w0,#0
	and.b	w0,#1,w0
	and.b	w0,#1,w0
	and.b	w0,#1,w0
	and.b	w0,#1,w0
	add.b	w0,w0,w2
	mov	#_LATEbits,w1
	mov.b	[w1],w1
	mov.b	#-3,w0
	and.b	w1,w0,w0
	ior.b	w0,w2,w0
	mov.b	WREG,_LATEbits
.LM77:
	mov	_udb_flags,w1
	mov	#128,w0
	and	w1,w0,w0
	cp0	w0
	.set ___BP___,0
	bra	z,.L60
	mov	_dcm_flags,w0
	mov	#-32768,w1
	and	w0,w1,w0
	cp0	w0
	.set ___BP___,0
	bra	z,.L60
.LM78:
	clr	w0
	mov	w0,_lowVoltageWarning
.LM79:
	mov	#10,w0
	mov	w0,_tailFlash
.LM80:
	mov	_primary_voltage+2,w1
	mov	#9750,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L55
	mov	_primary_voltage+2,w1
	mov	#16899,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L55
.LM81:
	mov	_primary_voltage+2,w1
	mov	#12700,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L58
.LM82:
	mov	#12900,w0
	mov	w0,_lowVoltageWarning
.LM83:
	mov	#4,w0
	mov	w0,_tailFlash
	bra	.L55
.L58:
.LM84:
	mov	#9750,w0
	mov	w0,_lowVoltageWarning
.LM85:
	mov	#3,w0
	mov	w0,_tailFlash
.L55:
.LM86:
	call	_udb_servo_record_trims
.LM87:
	call	_dcm_calibrate
.LM88:
	mov.b	#1,w0
	mov.b	WREG,_didCalibrate
.L60:
.LM89:
	ulnk	
	return	
	.set ___PA___,0
.LFE11:
	.align	2
	.global	_dcm_callback_gps_location_updated	; export
	.type	_dcm_callback_gps_location_updated,@function
_dcm_callback_gps_location_updated:
.LFB12:
.LM90:
	.set ___PA___,1
	lnk	#0
.LM91:
	ulnk	
	return	
	.set ___PA___,0
.LFE12:
	.section	.nbss,bss,near
	.align	2
_telCounter.9121:	.space	2
	.align	2
_pidCounter.9120:	.space	2
	.section	.text,code
	.align	2
	.global	_dcm_servo_callback_prepare_outputs	; export
	.type	_dcm_servo_callback_prepare_outputs,@function
_dcm_servo_callback_prepare_outputs:
.LFB13:
.LM92:
	.set ___PA___,1
	lnk	#0
.LM93:
	mov	_udb_flags,w0
	mov	#128,w1
	and	w0,w1,w0
	cp0	w0
	.set ___BP___,0
	bra	z,.L64
.LM94:
	bclr.b	_LATEbits,#2
	bra	.L66
.L64:
.LM95:
	bset.b	_LATEbits,#2
.L66:
.LM96:
	mov	_pidCounter.9120,w0
	inc	w0,w0
	mov	w0,_pidCounter.9120
	mov	_pidCounter.9120,w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L67
.LM97:
	clr	w0
	mov	w0,_pidCounter.9120
.LM98:
	call	_motorCntrl
.L67:
.LM99:
	mov.b	_didCalibrate,WREG
	cp0.b	w0
	.set ___BP___,0
	bra	z,.L75
.LM100:
	mov.b	_udb_throttle_enable,WREG
	cp0.b	w0
	.set ___BP___,0
	bra	nz,.L71
	mov.b	_sendGains,WREG
	cp0.b	w0
	.set ___BP___,0
	bra	z,.L75
.L71:
.LM101:
	mov	_telCounter.9121,w0
	inc	w0,w0
	mov	w0,_telCounter.9121
	mov	_telCounter.9121,w0
	sub	w0,#9,[w15]
	.set ___BP___,0
	bra	le,.L75
.LM102:
	clr	w0
	mov	w0,_telCounter.9121
.LM103:
	call	_send_telemetry
.L75:
.LM104:
	ulnk	
	return	
	.set ___PA___,0
.LFE13:
	.align	2
	.global	_udb_callback_radio_did_turn_off	; export
	.type	_udb_callback_radio_did_turn_off,@function
_udb_callback_radio_did_turn_off:
.LFB14:
.LM105:
	.set ___PA___,1
	lnk	#0
.LM106:
	ulnk	
	return	
	.set ___PA___,0
.LFE14:
	.section	.nbss,bss,near
	.type	_commanded_tilt_gain,@object
	.global	_commanded_tilt_gain
	.align	2
_commanded_tilt_gain:	.space	2
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
.LSFDE22:
	.4byte	.LEFDE22-.LASFDE22
.LASFDE22:
	.4byte	.Lframe0
	.4byte	.LFB13
	.4byte	.LFE13-.LFB13
.LEFDE22:
.LSFDE24:
	.4byte	.LEFDE24-.LASFDE24
.LASFDE24:
	.4byte	.Lframe0
	.4byte	.LFB14
	.4byte	.LFE14-.LFB14
.LEFDE24:
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
	.ascii	".."
	.byte 0
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
	.uleb128 0x2
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"p33FJ256GP710A.h"
	.uleb128 0x4
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"stddef.h"
	.uleb128 0x3
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"stdlib.h"
	.uleb128 0x3
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"dsp.h"
	.uleb128 0x5
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"../libUDB/libUDB.h"
	.uleb128 0x2
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"libDCM_defines.h"
	.uleb128 0x2
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"libDCM.h"
	.uleb128 0x2
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"main.c"
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
	.byte	0x45
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM2
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM3
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM4
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM5
	.byte	0x21
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM6
	.byte	0x16
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
	.byte	0x17
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
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM14
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM15
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM16
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM17
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM18
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM19
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM20
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM21
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM22
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM23
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM24
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM25
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM26
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM27
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM28
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM29
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM30
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM31
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM32
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM33
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM34
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM35
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM36
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM37
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM38
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM39
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM40
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM41
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM42
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM43
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM44
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM45
	.byte	0x16
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
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM49
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM50
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM51
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM52
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM53
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM54
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM55
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM56
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM57
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM58
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM59
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM60
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM61
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM62
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM63
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM64
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM65
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM66
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM67
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM68
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM69
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM70
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM71
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM72
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM73
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM74
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM75
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM76
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM77
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM78
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM79
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM80
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM81
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM82
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM83
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM84
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM85
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM86
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM87
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM88
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM89
	.byte	0x1e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM90
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM91
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM92
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM93
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM94
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM95
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM96
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM97
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM98
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM99
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM100
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM101
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM102
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM103
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM104
	.byte	0x22
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM105
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM106
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.Letext0
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_info,info
	.4byte	0xa84
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.Ldebug_line0
	.4byte	.Letext0
	.4byte	.Ltext0
	.asciz	"GNU C 4.0.3 (dsPIC30, Microchip v3_30) (A) Build date: Jun  2 2011"
	.byte	0x1
	.asciz	"../main.c"
	.ascii	"/home/markw/Dropbox/autopilots/UAVdevboard/UDB4/mplabx_projects/MPQp"
	.asciz	"id400/MatrixPilotQuad/MPQpid400.X"
	.uleb128 0x2
	.asciz	"unsigned char"
	.byte	0x1
	.byte	0x8
	.uleb128 0x3
	.4byte	0x103
	.asciz	"ww"
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.uleb128 0x4
	.asciz	"W0"
	.byte	0x1
	.byte	0x1d
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"W1"
	.byte	0x1
	.byte	0x1d
	.4byte	0x103
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x2
	.asciz	"int"
	.byte	0x2
	.byte	0x5
	.uleb128 0x2
	.asciz	"long int"
	.byte	0x4
	.byte	0x5
	.uleb128 0x5
	.4byte	0x139
	.asciz	"longww"
	.byte	0x4
	.byte	0x1
	.byte	0x21
	.uleb128 0x6
	.asciz	"WW"
	.byte	0x1
	.byte	0x21
	.4byte	0x10a
	.uleb128 0x6
	.asciz	"_"
	.byte	0x1
	.byte	0x21
	.4byte	0xdd
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7
	.uleb128 0x8
	.4byte	0x29c
	.asciz	"tagLATDBITS"
	.byte	0x2
	.byte	0x2
	.2byte	0x9a3
	.uleb128 0x9
	.asciz	"LATD0"
	.byte	0x2
	.2byte	0x9a4
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xf
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD1"
	.byte	0x2
	.2byte	0x9a5
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xe
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD2"
	.byte	0x2
	.2byte	0x9a6
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xd
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD3"
	.byte	0x2
	.2byte	0x9a7
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xc
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD4"
	.byte	0x2
	.2byte	0x9a8
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xb
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD5"
	.byte	0x2
	.2byte	0x9a9
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xa
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD6"
	.byte	0x2
	.2byte	0x9aa
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x9
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD7"
	.byte	0x2
	.2byte	0x9ab
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD8"
	.byte	0x2
	.2byte	0x9ac
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x7
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD9"
	.byte	0x2
	.2byte	0x9ad
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x6
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD10"
	.byte	0x2
	.2byte	0x9ae
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x5
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD11"
	.byte	0x2
	.2byte	0x9af
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x4
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD12"
	.byte	0x2
	.2byte	0x9b0
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x3
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD13"
	.byte	0x2
	.2byte	0x9b1
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x2
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD14"
	.byte	0x2
	.2byte	0x9b2
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x1
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATD15"
	.byte	0x2
	.2byte	0x9b3
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x10
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0xa
	.asciz	"LATDBITS"
	.byte	0x2
	.2byte	0x9b4
	.4byte	0x140
	.uleb128 0x8
	.4byte	0x363
	.asciz	"tagLATEBITS"
	.byte	0x2
	.byte	0x2
	.2byte	0x9d2
	.uleb128 0x9
	.asciz	"LATE0"
	.byte	0x2
	.2byte	0x9d3
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xf
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATE1"
	.byte	0x2
	.2byte	0x9d4
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xe
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATE2"
	.byte	0x2
	.2byte	0x9d5
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xd
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATE3"
	.byte	0x2
	.2byte	0x9d6
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xc
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATE4"
	.byte	0x2
	.2byte	0x9d7
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xb
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATE5"
	.byte	0x2
	.2byte	0x9d8
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xa
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATE6"
	.byte	0x2
	.2byte	0x9d9
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x9
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.asciz	"LATE7"
	.byte	0x2
	.2byte	0x9da
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0xa
	.asciz	"LATEBITS"
	.byte	0x2
	.2byte	0x9db
	.4byte	0x2ad
	.uleb128 0xb
	.asciz	"boolean"
	.byte	0x1
	.byte	0x9c
	.4byte	0x383
	.uleb128 0x2
	.asciz	"char"
	.byte	0x1
	.byte	0x6
	.uleb128 0x3
	.4byte	0x3e2
	.asciz	"udb_flag_bits"
	.byte	0x2
	.byte	0x1
	.byte	0xa8
	.uleb128 0xc
	.asciz	"unused"
	.byte	0x1
	.byte	0xa9
	.4byte	0x139
	.byte	0x2
	.byte	0x6
	.byte	0xa
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"a2d_read"
	.byte	0x1
	.byte	0xaa
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x9
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"radio_on"
	.byte	0x1
	.byte	0xab
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0x2
	.asciz	"short unsigned int"
	.byte	0x2
	.byte	0x7
	.uleb128 0x2
	.asciz	"long unsigned int"
	.byte	0x4
	.byte	0x7
	.uleb128 0x5
	.4byte	0x436
	.asciz	"udb_fbts_byte"
	.byte	0x2
	.byte	0x6
	.byte	0x77
	.uleb128 0x6
	.asciz	"_"
	.byte	0x6
	.byte	0x77
	.4byte	0x38b
	.uleb128 0x6
	.asciz	"B"
	.byte	0x6
	.byte	0x77
	.4byte	0x383
	.byte	0x0
	.uleb128 0x2
	.asciz	"float"
	.byte	0x4
	.byte	0x4
	.uleb128 0x3
	.4byte	0x5b5
	.asciz	"dcm_flag_bits"
	.byte	0x2
	.byte	0x7
	.byte	0x22
	.uleb128 0xc
	.asciz	"unused"
	.byte	0x7
	.byte	0x23
	.4byte	0x139
	.byte	0x2
	.byte	0x4
	.byte	0xc
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"rollpitch_req"
	.byte	0x7
	.byte	0x24
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xb
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"gps_history_valid"
	.byte	0x7
	.byte	0x25
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0xa
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"dead_reckon_enable"
	.byte	0x7
	.byte	0x26
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x9
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"reckon_req"
	.byte	0x7
	.byte	0x27
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x8
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"first_mag_reading"
	.byte	0x7
	.byte	0x28
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x7
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"mag_drift_req"
	.byte	0x7
	.byte	0x29
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x6
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"yaw_req"
	.byte	0x7
	.byte	0x2a
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x5
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"skip_yaw_drift"
	.byte	0x7
	.byte	0x2b
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x4
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"nav_capable"
	.byte	0x7
	.byte	0x2c
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x3
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"nmea_passthrough"
	.byte	0x7
	.byte	0x2d
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x2
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"init_finished"
	.byte	0x7
	.byte	0x2e
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x1
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0xc
	.asciz	"calib_finished"
	.byte	0x7
	.byte	0x2f
	.4byte	0x139
	.byte	0x2
	.byte	0x1
	.byte	0x10
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0x5
	.4byte	0x5de
	.asciz	"dcm_fbts_word"
	.byte	0x2
	.byte	0x8
	.byte	0x5b
	.uleb128 0x6
	.asciz	"_"
	.byte	0x8
	.byte	0x5b
	.4byte	0x43f
	.uleb128 0x6
	.asciz	"W"
	.byte	0x8
	.byte	0x5b
	.4byte	0x103
	.byte	0x0
	.uleb128 0xd
	.byte	0x1
	.asciz	"main"
	.byte	0x9
	.byte	0x32
	.byte	0x1
	.4byte	0x103
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x1
	.byte	0x5e
	.uleb128 0xe
	.byte	0x1
	.asciz	"check_failsafe"
	.byte	0x9
	.byte	0x56
	.4byte	.LFB3
	.4byte	.LFE3
	.byte	0x1
	.byte	0x5e
	.uleb128 0xf
	.byte	0x1
	.asciz	"check_flight_mode"
	.byte	0x9
	.byte	0x5e
	.byte	0x1
	.4byte	.LFB4
	.4byte	.LFE4
	.byte	0x1
	.byte	0x5e
	.uleb128 0x10
	.4byte	0x67e
	.byte	0x1
	.asciz	"storeGain"
	.byte	0x9
	.byte	0x68
	.byte	0x1
	.4byte	.LFB5
	.4byte	.LFE5
	.byte	0x1
	.byte	0x5e
	.uleb128 0x11
	.asciz	"index"
	.byte	0x9
	.byte	0x68
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x12
	.4byte	.LBB2
	.4byte	.LBE2
	.uleb128 0x13
	.asciz	"address"
	.byte	0x9
	.byte	0x6b
	.4byte	0x139
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.byte	0x1
	.asciz	"storeGains"
	.byte	0x9
	.byte	0x71
	.byte	0x1
	.4byte	.LFB6
	.4byte	.LFE6
	.byte	0x1
	.byte	0x5e
	.uleb128 0x10
	.4byte	0x6d8
	.byte	0x1
	.asciz	"adjust_gain"
	.byte	0x9
	.byte	0x75
	.byte	0x1
	.4byte	.LFB7
	.4byte	.LFE7
	.byte	0x1
	.byte	0x5e
	.uleb128 0x11
	.asciz	"index"
	.byte	0x9
	.byte	0x75
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x11
	.asciz	"delta"
	.byte	0x9
	.byte	0x75
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.byte	0x0
	.uleb128 0x10
	.4byte	0x766
	.byte	0x1
	.asciz	"check_gain_adjust"
	.byte	0x9
	.byte	0x85
	.byte	0x1
	.4byte	.LFB8
	.4byte	.LFE8
	.byte	0x1
	.byte	0x5e
	.uleb128 0x13
	.asciz	"gainState"
	.byte	0x9
	.byte	0x86
	.4byte	0x103
	.byte	0x5
	.byte	0x3
	.4byte	_gainState.9030
	.uleb128 0x13
	.asciz	"gainIndex"
	.byte	0x9
	.byte	0x87
	.4byte	0x103
	.byte	0x5
	.byte	0x3
	.4byte	_gainIndex.9031
	.uleb128 0x13
	.asciz	"lastGainChVal"
	.byte	0x9
	.byte	0x88
	.4byte	0x103
	.byte	0x5
	.byte	0x3
	.4byte	_lastGainChVal.9032
	.uleb128 0x12
	.4byte	.LBB3
	.4byte	.LBE3
	.uleb128 0x13
	.asciz	"gain_delta"
	.byte	0x9
	.byte	0xa3
	.4byte	0x103
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.byte	0x1
	.asciz	"update_pid_gains"
	.byte	0x9
	.byte	0xb2
	.byte	0x1
	.4byte	.LFB9
	.4byte	.LFE9
	.byte	0x1
	.byte	0x5e
	.uleb128 0x14
	.4byte	0x7c5
	.byte	0x1
	.asciz	"run_background_task"
	.byte	0x9
	.byte	0xbe
	.4byte	.LFB10
	.4byte	.LFE10
	.byte	0x1
	.byte	0x5e
	.uleb128 0x13
	.asciz	"lastUptime"
	.byte	0x9
	.byte	0xc1
	.4byte	0x103
	.byte	0x5
	.byte	0x3
	.4byte	_lastUptime.9073
	.byte	0x0
	.uleb128 0xf
	.byte	0x1
	.asciz	"udb_background_callback_periodic"
	.byte	0x9
	.byte	0xd5
	.byte	0x1
	.4byte	.LFB11
	.4byte	.LFE11
	.byte	0x1
	.byte	0x5e
	.uleb128 0xf
	.byte	0x1
	.asciz	"dcm_callback_gps_location_updated"
	.byte	0x9
	.byte	0xfd
	.byte	0x1
	.4byte	.LFB12
	.4byte	.LFE12
	.byte	0x1
	.byte	0x5e
	.uleb128 0x15
	.4byte	0x890
	.byte	0x1
	.asciz	"dcm_servo_callback_prepare_outputs"
	.byte	0x9
	.2byte	0x104
	.byte	0x1
	.4byte	.LFB13
	.4byte	.LFE13
	.byte	0x1
	.byte	0x5e
	.uleb128 0x16
	.asciz	"pidCounter"
	.byte	0x9
	.2byte	0x105
	.4byte	0x103
	.byte	0x5
	.byte	0x3
	.4byte	_pidCounter.9120
	.uleb128 0x16
	.asciz	"telCounter"
	.byte	0x9
	.2byte	0x106
	.4byte	0x103
	.byte	0x5
	.byte	0x3
	.4byte	_telCounter.9121
	.byte	0x0
	.uleb128 0x17
	.byte	0x1
	.asciz	"udb_callback_radio_did_turn_off"
	.byte	0x9
	.2byte	0x131
	.byte	0x1
	.4byte	.LFB14
	.4byte	.LFE14
	.byte	0x1
	.byte	0x5e
	.uleb128 0x18
	.asciz	"LATDbits"
	.byte	0x2
	.2byte	0x9b5
	.4byte	0x8d3
	.byte	0x1
	.byte	0x1
	.uleb128 0x19
	.4byte	0x29c
	.uleb128 0x18
	.asciz	"LATEbits"
	.byte	0x2
	.2byte	0x9dc
	.4byte	0x8eb
	.byte	0x1
	.byte	0x1
	.uleb128 0x19
	.4byte	0x363
	.uleb128 0x1a
	.4byte	0x8fb
	.4byte	0x103
	.uleb128 0x1b
	.byte	0x0
	.uleb128 0x1c
	.asciz	"udb_pwIn"
	.byte	0x6
	.byte	0x66
	.4byte	0x8f0
	.byte	0x1
	.byte	0x1
	.uleb128 0x1c
	.asciz	"udb_flags"
	.byte	0x6
	.byte	0x77
	.4byte	0x40d
	.byte	0x1
	.byte	0x1
	.uleb128 0x1c
	.asciz	"dcm_flags"
	.byte	0x8
	.byte	0x5b
	.4byte	0x5b5
	.byte	0x1
	.byte	0x1
	.uleb128 0x1d
	.asciz	"didCalibrate"
	.byte	0x9
	.byte	0x18
	.4byte	0x374
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_didCalibrate
	.uleb128 0x1a
	.4byte	0x959
	.4byte	0x139
	.uleb128 0x1b
	.byte	0x0
	.uleb128 0x1c
	.asciz	"pid_gains"
	.byte	0x9
	.byte	0x1f
	.4byte	0x94e
	.byte	0x1
	.byte	0x1
	.uleb128 0x1c
	.asciz	"uptime"
	.byte	0x9
	.byte	0x20
	.4byte	0x3f8
	.byte	0x1
	.byte	0x1
	.uleb128 0x1d
	.asciz	"max_tilt"
	.byte	0x9
	.byte	0x22
	.4byte	0x993
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_max_tilt
	.uleb128 0x1e
	.4byte	0x103
	.uleb128 0x1d
	.asciz	"commanded_tilt_gain"
	.byte	0x9
	.byte	0x23
	.4byte	0x103
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_commanded_tilt_gain
	.uleb128 0x1d
	.asciz	"flight_mode"
	.byte	0x9
	.byte	0x26
	.4byte	0x103
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_flight_mode
	.uleb128 0x1d
	.asciz	"udb_throttle_enable"
	.byte	0x9
	.byte	0x2b
	.4byte	0x374
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_udb_throttle_enable
	.uleb128 0x1d
	.asciz	"tailFlash"
	.byte	0x9
	.byte	0x2d
	.4byte	0x139
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_tailFlash
	.uleb128 0x1c
	.asciz	"primary_voltage"
	.byte	0x9
	.byte	0x2f
	.4byte	0x116
	.byte	0x1
	.byte	0x1
	.uleb128 0x1c
	.asciz	"lowVoltageWarning"
	.byte	0x9
	.byte	0x30
	.4byte	0x139
	.byte	0x1
	.byte	0x1
	.uleb128 0x1c
	.asciz	"sendGains"
	.byte	0x9
	.byte	0x81
	.4byte	0x374
	.byte	0x1
	.byte	0x1
	.uleb128 0x1a
	.4byte	0xa65
	.4byte	0x103
	.uleb128 0x1f
	.4byte	0xa65
	.byte	0x2
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7
	.uleb128 0x1d
	.asciz	"gainAdjIndex"
	.byte	0x9
	.byte	0x83
	.4byte	0xa55
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_gainAdjIndex
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
	.uleb128 0x38
	.uleb128 0xa
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
	.uleb128 0x5
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
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
	.uleb128 0xa
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
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
	.uleb128 0xc
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
	.uleb128 0xd
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
	.uleb128 0xe
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
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
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
	.uleb128 0x10
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
	.uleb128 0x11
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
	.uleb128 0x12
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
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
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
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
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
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
	.uleb128 0x16
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x17
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
	.uleb128 0x18
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0x19
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1a
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
	.uleb128 0x21
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.uleb128 0x1c
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
	.uleb128 0x1d
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
	.uleb128 0x1e
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1f
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,info
	.4byte	0x1c3
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0xa88
	.4byte	0x5de
	.asciz	"main"
	.4byte	0x5f6
	.asciz	"check_failsafe"
	.4byte	0x613
	.asciz	"check_flight_mode"
	.4byte	0x634
	.asciz	"storeGain"
	.4byte	0x67e
	.asciz	"storeGains"
	.4byte	0x698
	.asciz	"adjust_gain"
	.4byte	0x6d8
	.asciz	"check_gain_adjust"
	.4byte	0x766
	.asciz	"update_pid_gains"
	.4byte	0x786
	.asciz	"run_background_task"
	.4byte	0x7c5
	.asciz	"udb_background_callback_periodic"
	.4byte	0x7f5
	.asciz	"dcm_callback_gps_location_updated"
	.4byte	0x826
	.asciz	"dcm_servo_callback_prepare_outputs"
	.4byte	0x890
	.asciz	"udb_callback_radio_did_turn_off"
	.4byte	0x933
	.asciz	"didCalibrate"
	.4byte	0x97c
	.asciz	"max_tilt"
	.4byte	0x998
	.asciz	"commanded_tilt_gain"
	.4byte	0x9ba
	.asciz	"flight_mode"
	.4byte	0x9d4
	.asciz	"udb_throttle_enable"
	.4byte	0x9f6
	.asciz	"tailFlash"
	.4byte	0xa6c
	.asciz	"gainAdjIndex"
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
