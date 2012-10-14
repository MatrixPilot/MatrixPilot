	.file "../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/libDCM/mathlibNAV.c"
	.section	.debug_abbrev,info
.Ldebug_abbrev0:
	.section	.debug_info,info
.Ldebug_info0:
	.section	.debug_line,info
.Ldebug_line0:
	.section	.text,code
.Ltext0:
	.global	_sintab	; export
	.section	.const,psv,page
	.align	2
	.type	_sintab,@object
_sintab:
	.word	0
	.word	402
	.word	804
	.word	1205
	.word	1606
	.word	2006
	.word	2404
	.word	2801
	.word	3196
	.word	3590
	.word	3981
	.word	4370
	.word	4756
	.word	5139
	.word	5520
	.word	5897
	.word	6270
	.word	6639
	.word	7005
	.word	7366
	.word	7723
	.word	8076
	.word	8423
	.word	8765
	.word	9102
	.word	9434
	.word	9760
	.word	10080
	.word	10394
	.word	10702
	.word	11003
	.word	11297
	.word	11585
	.word	11866
	.word	12140
	.word	12406
	.word	12665
	.word	12916
	.word	13160
	.word	13395
	.word	13623
	.word	13842
	.word	14053
	.word	14256
	.word	14449
	.word	14635
	.word	14811
	.word	14978
	.word	15137
	.word	15286
	.word	15426
	.word	15557
	.word	15679
	.word	15791
	.word	15893
	.word	15986
	.word	16069
	.word	16143
	.word	16207
	.word	16261
	.word	16305
	.word	16340
	.word	16364
	.word	16379
	.word	16384
	.section	.text,code
	.align	2
	.global	_sine	; export
	.type	_sine,@function
_sine:
.LFB0:
.LSM0:
	.set ___PA___,1
	lnk	#4
	mov.b	w0,[w14+2]
.LSM1:
	mov.b	[w14+2],w0
	se	w0,w0
	mov	w0,[w14]
.LSM2:
	mov	[w14],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	lt,.L2
.LSM3:
	mov	#64,w0
	mov	[w14],w1
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L3
.LSM4:
	mov	#128,w0
	sub	w0,[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	bra	.L4
.L3:
.LSM5:
	mov	[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	bra	.L4
.L2:
.LSM6:
	neg	[w14],[w14]
.LSM7:
	mov	#64,w0
	mov	[w14],w1
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L5
.LSM8:
	mov	#128,w0
	sub	w0,[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	neg	w0,w0
	bra	.L4
.L5:
.LSM9:
	mov	[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	neg	w0,w0
.L4:
.LSM10:
	ulnk	
	return	
	.set ___PA___,0
.LFE0:
	.align	2
	.global	_arcsine	; export
	.type	_arcsine,@function
_arcsine:
.LFB1:
.LSM11:
	.set ___PA___,1
	lnk	#6
	mov	w8,[w15++]
	mov	w0,[w14+4]
.LSM12:
	mov.b	#32,w0
	mov.b	w0,[w14+3]
.LSM13:
	mov.b	#64,w0
	mov.b	w0,[w14]
.LSM14:
	mov.b	#32,w0
	mov.b	w0,[w14+1]
.LSM15:
	mov	[w14+4],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	le,.L7
.LSM16:
	mov.b	#1,w0
	mov.b	w0,[w14+2]
	bra	.L8
.L7:
.LSM17:
	setm.b	w0
	mov.b	w0,[w14+2]
.LSM18:
	mov	[w14+4],w0
	neg	w0,w0
	mov	w0,[w14+4]
.L8:
.LSM19:
	mov	[w14+4],w1
	mov	#16384,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	nz,.L11
.LSM20:
	mov.b	[w14+2],w0
	mov	#6,w1
	sl	w0,w1,w0
	bra	.L10
.L15:
.LSM21:
	se	[w14],w0
	asr	w0,w0
	mov.b	w0,[w14+3]
.LSM22:
	mov.b	[w14+3],w0
	rcall	_sine
	mov	[w14+4],w1
	sub	w0,w1,[w15]
	.set ___BP___,0
	bra	nz,.L12
.LSM23:
	mov.b	[w14+2],w1
	mov.b	[w14+3],w0
	mul.ss	w1,w0,w0
	mov	w0,w0
	mov.b	w0,w0
	bra	.L10
.L12:
.LSM24:
	mov.b	[w14+3],w0
	rcall	_sine
	mov	w0,w8
	mov.b	[w14+3],w0
	dec.b	w0,w0
	rcall	_sine
	add	w8,w0,w0
	asr	w0,w1
	mov	[w14+4],w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	ge,.L13
.LSM25:
	mov.b	[w14],w1
	mov.b	[w14+1],w0
	add.b	w1,w0,w0
	mov.b	w0,[w14]
	bra	.L14
.L13:
.LSM26:
	mov.b	[w14],w1
	mov.b	[w14+1],w0
	sub.b	w1,w0,w0
	mov.b	w0,[w14]
.L14:
.LSM27:
	mov.b	[w14+1],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,[w14+1]
.L11:
.LSM28:
	mov.b	[w14+1],w0
	sub.b	w0,#0,[w15]
	.set ___BP___,0
	bra	gt,.L15
.LSM29:
	se	[w14],w0
	asr	w0,w0
	mov.b	w0,w0
	mov.b	w0,w1
	mov.b	[w14+2],w0
	mul.ss	w1,w0,w0
	mov	w0,w0
	mov.b	w0,w0
.L10:
.LSM30:
	mov	[--w15],w8
	ulnk	
	return	
	.set ___PA___,0
.LFE1:
	.align	2
	.global	_cosine	; export
	.type	_cosine,@function
_cosine:
.LFB2:
.LSM31:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LSM32:
	mov.b	[w14],w0
	add.b	#64,w0
	rcall	_sine
.LSM33:
	ulnk	
	return	
	.set ___PA___,0
.LFE2:
	.align	2
	.global	_rotate	; export
	.type	_rotate,@function
_rotate:
.LFB3:
.LSM34:
	.set ___PA___,1
	lnk	#16
	mov	w0,[w14+12]
	mov.b	w1,[w14+14]
.LSM35:
	mov.b	[w14+14],w0
	rcall	_sine
	mov	w0,[w14]
.LSM36:
	mov.b	[w14+14],w0
	rcall	_cosine
	mov	w0,[w14+2]
.LSM37:
	mov	[w14+12],w0
	mov	[w0],w1
	mov	[w14+2],w0
	mul.ss	w0,w1,w2
	mov	[w14+12],w0
	mov	[w0+2],w1
	mov	[w14],w0
	mul.ss	w0,w1,w0
	sub	w2,w0,w2
	subb	w3,w1,w3
	sl	w3,#2,w0
	lsr	w2,#14,w1
	ior	w0,w1,w1
	sl	w2,#2,w0
	mov	w0,[w14+8]
	mov	w1,[w14+10]
.LSM38:
	mov	[w14+10],w0
	mov	w0,[w14+4]
.LSM39:
	mov	[w14+12],w0
	mov	[w0],w1
	mov	[w14],w0
	mul.ss	w0,w1,w2
	mov	[w14+12],w0
	mov	[w0+2],w1
	mov	[w14+2],w0
	mul.ss	w0,w1,w0
	add	w0,w2,w2
	addc	w1,w3,w3
	sl	w3,#2,w0
	lsr	w2,#14,w1
	ior	w0,w1,w1
	sl	w2,#2,w0
	mov	w0,[w14+8]
	mov	w1,[w14+10]
.LSM40:
	mov	[w14+10],w1
	mov	w1,[w14+6]
.LSM41:
	mov	[w14+12],w0
	mov	[w14+4],w1
	mov	w1,[w0]
.LSM42:
	mov	[w14+12],w0
	mov	[w14+6],w1
	mov	w1,[w0+2]
.LSM43:
	ulnk	
	return	
	.set ___PA___,0
.LFE3:
	.align	2
	.global	_rect_to_polar	; export
	.type	_rect_to_polar,@function
_rect_to_polar:
.LFB4:
.LSM44:
	.set ___PA___,1
	lnk	#8
	mov	w0,[w14+6]
.LSM45:
	clr.b	w0
	mov.b	w0,[w14]
.LSM46:
	mov.b	#64,w0
	mov.b	w0,[w14+1]
.LSM47:
	mov.b	#7,w0
	mov.b	w0,[w14+3]
.LSM48:
	mov	[w14+6],w0
	mov	[w0],w1
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L19
.LSM49:
	mov	[w14+6],w0
	mov	[w0],w1
.LSM50:
	mov	#-254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	lt,.L19
.LSM51:
	mov	[w14+6],w0
	mov	[w0+2],w1
.LSM52:
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L19
.LSM53:
	mov	[w14+6],w0
	mov	[w0+2],w1
.LSM54:
	mov	#-254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	lt,.L19
.LSM55:
	mov	#6,w0
	mov	w0,[w14+4]
.LSM56:
	mov	[w14+6],w0
	mov	[w0],w0
	sl	w0,#6,w1
	mov	[w14+6],w0
	mov	w1,[w0]
.LSM57:
	mov	[w14+6],w0
	mov	[w0+2],w0
	sl	w0,#6,w1
	mov	[w14+6],w0
	mov	w1,[w0+2]
.LSM58:
	bra	.L21
.L19:
.LSM59:
	clr	w0
	mov	w0,[w14+4]
.LSM60:
	bra	.L21
.L23:
.LSM61:
	mov.b	[w14+1],w0
	mov.b	w0,[w14+2]
.LSM62:
	mov	[w14+6],w0
	mov	[w0+2],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	le,.L22
	mov.b	[w14+2],w0
	neg.b	w0,w0
	mov.b	w0,[w14+2]
.L22:
.LSM63:
	mov.b	[w14+2],w1
	mov	[w14+6],w0
	rcall	_rotate
.LSM64:
	mov.b	[w14],w1
	mov.b	[w14+2],w0
	add.b	w1,w0,w0
	mov.b	w0,[w14]
.LSM65:
	mov.b	[w14+1],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,[w14+1]
.LSM66:
	mov.b	[w14+3],w0
	dec.b	w0,w0
	mov.b	w0,[w14+3]
.L21:
.LSM67:
	mov.b	[w14+3],w0
	sub.b	w0,#0,[w15]
	.set ___BP___,0
	bra	gt,.L23
.LSM68:
	mov	[w14+6],w0
	mov	[w0+2],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	le,.L24
	dec.b	[w14],[w14]
.L24:
.LSM69:
	mov	[w14+6],w0
	mov	[w0],w1
	mov	[w14+4],w0
	asr	w1,w0,w1
	mov	[w14+6],w0
	mov	w1,[w0]
.LSM70:
	mov	[w14+6],w0
	mov	[w0+2],w1
	mov	[w14+4],w0
	asr	w1,w0,w1
	mov	[w14+6],w0
	mov	w1,[w0+2]
.LSM71:
	neg.b	[w14],w0
.LSM72:
	ulnk	
	return	
	.set ___PA___,0
.LFE4:
	.align	2
	.global	_rect_to_polar16	; export
	.type	_rect_to_polar16,@function
_rect_to_polar16:
.LFB5:
.LSM73:
	.set ___PA___,1
	lnk	#10
	mov	w0,[w14+8]
.LSM74:
	clr.b	w0
	mov.b	w0,[w14+4]
.LSM75:
	mov.b	#64,w0
	mov.b	w0,[w14+5]
.LSM76:
	mov.b	#7,w0
	mov.b	w0,[w14+7]
.LSM77:
	mov	[w14+8],w0
	mov	[w0],w1
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L26
.LSM78:
	mov	[w14+8],w0
	mov	[w0],w1
.LSM79:
	mov	#-254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	lt,.L26
.LSM80:
	mov	[w14+8],w0
	mov	[w0+2],w1
.LSM81:
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L26
.LSM82:
	mov	[w14+8],w0
	mov	[w0+2],w1
.LSM83:
	mov	#-254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	lt,.L26
.LSM84:
	mov	#6,w0
	mov	w0,[w14]
.LSM85:
	mov	[w14+8],w0
	mov	[w0],w0
	sl	w0,#6,w1
	mov	[w14+8],w0
	mov	w1,[w0]
.LSM86:
	mov	[w14+8],w0
	mov	[w0+2],w0
	sl	w0,#6,w1
	mov	[w14+8],w0
	mov	w1,[w0+2]
.LSM87:
	bra	.L28
.L26:
.LSM88:
	clr	w0
	mov	w0,[w14]
.LSM89:
	bra	.L28
.L30:
.LSM90:
	mov.b	[w14+5],w0
	mov.b	w0,[w14+6]
.LSM91:
	mov	[w14+8],w0
	mov	[w0+2],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	le,.L29
	mov.b	[w14+6],w0
	neg.b	w0,w0
	mov.b	w0,[w14+6]
.L29:
.LSM92:
	mov.b	[w14+6],w1
	mov	[w14+8],w0
	rcall	_rotate
.LSM93:
	mov.b	[w14+4],w1
	mov.b	[w14+6],w0
	add.b	w1,w0,w0
	mov.b	w0,[w14+4]
.LSM94:
	mov.b	[w14+5],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,[w14+5]
.LSM95:
	mov.b	[w14+7],w0
	dec.b	w0,w0
	mov.b	w0,[w14+7]
.L28:
.LSM96:
	mov.b	[w14+7],w0
	sub.b	w0,#0,[w15]
	.set ___BP___,0
	bra	gt,.L30
.LSM97:
	mov.b	[w14+4],w0
	neg.b	w0,w0
	mov.b	w0,[w14+4]
.LSM98:
	mov.b	[w14+4],w0
	se	w0,w0
	sl	w0,#8,w0
	mov	w0,[w14+2]
.LSM99:
	mov	[w14+8],w0
	mov	[w0],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	le,.L31
.LSM100:
	mov	[w14+8],w0
	mov	[w0],w2
	mov	[w14+8],w0
	mov	[w0+2],w1
	mov	#10430,w0
	mul.ss	w0,w1,w0
	mov	w1,[w15++]
	repeat	#18-1
	div.sd	w0,w2
	mov	[--w15],w1
	mov	[w14+2],w1
	add	w1,w0,w0
	mov	w0,[w14+2]
.L31:
.LSM101:
	mov	[w14+8],w0
	mov	[w0],w1
	mov	[w14],w0
	asr	w1,w0,w1
	mov	[w14+8],w0
	mov	w1,[w0]
.LSM102:
	mov	[w14+8],w0
	mov	[w0+2],w1
	mov	[w14],w0
	asr	w1,w0,w1
	mov	[w14+8],w0
	mov	w1,[w0+2]
.LSM103:
	mov	[w14+2],w0
.LSM104:
	ulnk	
	return	
	.set ___PA___,0
.LFE5:
	.align	2
	.global	_sqrt_int	; export
	.type	_sqrt_int,@function
_sqrt_int:
.LFB6:
.LSM105:
	.set ___PA___,1
	lnk	#8
	mov	w0,[w14+6]
.LSM106:
	clr	w0
	mov	w0,[w14]
.LSM107:
	mov	#255,w0
	mov	w0,[w14+2]
.LSM108:
	mov	#3,w0
	mov	w0,[w14+4]
.LSM109:
	mov	[w14+6],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	nz,.L39
.LSM110:
	clr	w0
	bra	.L34
.L36:
.LSM111:
	mov	[w14+6],w0
	sl	w0,#2,w0
	mov	w0,[w14+6]
.LSM112:
	inc	[w14],[w14]
	bra	.L35
.L39:
.LSM113:
	nop	
.L35:
	mov	[w14+6],w1
	mov	#-16384,w0
	and	w1,w0,w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	z,.L36
.LSM114:
	mov	[w14+6],w0
	lsr	w0,w0
	mov	w0,[w14+6]
.LSM115:
	bra	.L37
.L38:
.LSM116:
	mov	[w14+4],w0
	dec	w0,w0
	mov	w0,[w14+4]
.LSM117:
	mov	[w14+2],w0
	lsr	w0,w1
	mov	[w14+6],w2
	mov	[w14+2],w0
	mov	w0,w3
	mov	w1,[w15++]
	repeat	#18-1
	div.uw	w2,w3
	mov	w1,w2
	mov	[--w15],w1
	add	w1,w0,w0
	mov	w0,[w14+2]
.L37:
.LSM118:
	mov	[w14+4],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	nz,.L38
.LSM119:
	mov	[w14],w0
	mov	[w14+2],w1
	lsr	w1,w0,w0
	mov	w0,[w14+2]
.LSM120:
	mov	[w14+2],w0
.L34:
.LSM121:
	ulnk	
	return	
	.set ___PA___,0
.LFE6:
	.align	2
	.global	_sqrt_long	; export
	.type	_sqrt_long,@function
_sqrt_long:
.LFB7:
.LSM122:
	.set ___PA___,1
	lnk	#10
	mov	w0,[w14+6]
	mov	w1,[w14+8]
.LSM123:
	clr	w0
	mov	w0,[w14]
.LSM124:
	setm	w0
	mov	w0,[w14+2]
.LSM125:
	mov	#3,w0
	mov	w0,[w14+4]
.LSM126:
	mov	[w14+6],w2
	mov	[w14+8],w3
	mov	#65535,w0
	mov	#0,w1
	sub	w2,w0,[w15]
	subb	w3,w1,[w15]
	.set ___BP___,0
	bra	gtu,.L47
.LSM127:
	mov	[w14+6],w0
	rcall	_sqrt_int
	bra	.L42
.L44:
.LSM128:
	mov	[w14+6],w2
	mov	[w14+8],w3
	sl	w3,#2,w0
	lsr	w2,#14,w1
	ior	w0,w1,w1
	sl	w2,#2,w0
	mov	w0,[w14+6]
	mov	w1,[w14+8]
.LSM129:
	inc	[w14],[w14]
	bra	.L43
.L47:
.LSM130:
	nop	
.L43:
	mov	[w14+6],w2
	mov	[w14+8],w3
	mov	#0,w0
	mov	#49152,w1
	mov	w2,w4
	mov	w3,w2
	mov	w0,w3
	mov	w1,w0
	and	w4,w3,w4
	and	w2,w0,w0
	mov	#0,w1
	sl	w0,#0,w1
	mov	#0,w0
	mov.d	w0,w2
	mul.uu	w4,#1,w0
	ior	w2,w0,w2
	ior	w3,w1,w3
	sub	w2,#0,[w15]
	subb	w3,#0,[w15]
	.set ___BP___,0
	bra	z,.L44
.LSM131:
	mov	[w14+6],w0
	mov	[w14+8],w1
	
	lsr w1,w1
	rrc	w0,w0
	mov	w0,[w14+6]
	mov	w1,[w14+8]
.LSM132:
	bra	.L45
.L46:
.LSM133:
	mov	[w14+4],w0
	dec	w0,w0
	mov	w0,[w14+4]
.LSM134:
	mov	[w14+2],w0
	lsr	w0,w2
	mov	[w14+6],w0
	mov	[w14+8],w1
	mov	[w14+2],w3
	mov	w1,[w15++]
	repeat	#18-1
	div.ud	w0,w3
	mov	[--w15],w1
	add	w2,w0,w0
	mov	w0,[w14+2]
.L45:
.LSM135:
	mov	[w14+4],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	nz,.L46
.LSM136:
	mov	[w14],w0
	mov	[w14+2],w1
	lsr	w1,w0,w0
	mov	w0,[w14+2]
.LSM137:
	mov	[w14+2],w0
.L42:
.LSM138:
	ulnk	
	return	
	.set ___PA___,0
.LFE7:
	.align	2
	.global	_vector2_mag	; export
	.type	_vector2_mag,@function
_vector2_mag:
.LFB8:
.LSM139:
	.set ___PA___,1
	lnk	#8
	mov	w0,[w14+4]
	mov	w1,[w14+6]
.LSM140:
	mov	[w14+4],w0
	mov	[w14+4],w1
	mul.ss	w0,w1,w2
	mov	[w14+6],w0
	mov	[w14+6],w1
	mul.ss	w0,w1,w0
	add	w0,w2,w0
	addc	w1,w3,w1
	mov.d	w0,[w14]
.LSM141:
	mov.d	[w14],w0
	rcall	_sqrt_long
.LSM142:
	ulnk	
	return	
	.set ___PA___,0
.LFE8:
	.align	2
	.global	_vector3_mag	; export
	.type	_vector3_mag,@function
_vector3_mag:
.LFB9:
.LSM143:
	.set ___PA___,1
	lnk	#10
	mov	w0,[w14+4]
	mov	w1,[w14+6]
	mov	w2,[w14+8]
.LSM144:
	mov	[w14+4],w0
	mov	[w14+4],w1
	mul.ss	w0,w1,w2
	mov	[w14+6],w0
	mov	[w14+6],w1
	mul.ss	w0,w1,w0
	add	w0,w2,w2
	addc	w1,w3,w3
	mov	[w14+8],w0
	mov	[w14+8],w1
	mul.ss	w0,w1,w0
	add	w0,w2,w0
	addc	w1,w3,w1
	mov.d	w0,[w14]
.LSM145:
	mov.d	[w14],w0
	rcall	_sqrt_long
.LSM146:
	ulnk	
	return	
	.set ___PA___,0
.LFE9:
	.align	2
	.global	_vector2_normalize	; export
	.type	_vector2_normalize,@function
_vector2_normalize:
.LFB10:
.LSM147:
	.set ___PA___,1
	lnk	#6
	mov	w0,[w14+2]
	mov	w1,[w14+4]
.LSM148:
	mov	[w14+4],w0
	inc2	w0,w0
	mov	[w0],w1
	mov	[w14+4],w0
	mov	[w0],w0
	rcall	_vector2_mag
	mov	w0,[w14]
.LSM149:
	mov	[w14],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	z,.L51
.LSM150:
	mov	[w14],w2
	mov	[w14+4],w0
	mov	[w0],w1
	mov	#16384,w0
	mul.ss	w0,w1,w0
	mov	w0,[w15++]
	repeat	#18-1
	div.sd	w0,w2
	mov	w0,w1
	mov	[--w15],w0
	mov	[w14+2],w0
	mov	w1,[w0]
.LSM151:
	mov	[w14+2],w0
	inc2	w0,w2
	mov	[w14],w3
	mov	[w14+4],w0
	inc2	w0,w0
	mov	[w0],w1
	mov	#16384,w0
	mul.ss	w0,w1,w0
	mov	w1,[w15++]
	repeat	#18-1
	div.sd	w0,w3
	mov	[--w15],w1
	mov	w0,[w2]
	bra	.L52
.L51:
.LSM152:
	mov	[w14+2],w0
	inc2	w0,w0
	clr	w1
	mov	w1,[w0]
	mov	[w0],w1
	mov	[w14+2],w0
	mov	w1,[w0]
.L52:
.LSM153:
	mov	[w14],w0
.LSM154:
	ulnk	
	return	
	.set ___PA___,0
.LFE10:
	.align	2
	.global	_vector3_normalize	; export
	.type	_vector3_normalize,@function
_vector3_normalize:
.LFB11:
.LSM155:
	.set ___PA___,1
	lnk	#6
	mov	w0,[w14+2]
	mov	w1,[w14+4]
.LSM156:
	mov	[w14+4],w0
	add	w0,#4,w0
	mov	[w0],w2
	mov	[w14+4],w0
	inc2	w0,w0
	mov	[w0],w1
	mov	[w14+4],w0
	mov	[w0],w0
	rcall	_vector3_mag
	mov	w0,[w14]
.LSM157:
	mov	[w14],w0
	sub	w0,#0,[w15]
	.set ___BP___,0
	bra	z,.L54
.LSM158:
	mov	[w14],w2
	mov	[w14+4],w0
	mov	[w0],w1
	mov	#16384,w0
	mul.ss	w0,w1,w0
	mov	w0,[w15++]
	repeat	#18-1
	div.sd	w0,w2
	mov	w0,w1
	mov	[--w15],w0
	mov	[w14+2],w0
	mov	w1,[w0]
.LSM159:
	mov	[w14+2],w0
	inc2	w0,w2
	mov	[w14],w3
	mov	[w14+4],w0
	inc2	w0,w0
	mov	[w0],w1
	mov	#16384,w0
	mul.ss	w0,w1,w0
	mov	w1,[w15++]
	repeat	#18-1
	div.sd	w0,w3
	mov	[--w15],w1
	mov	w0,[w2]
.LSM160:
	mov	[w14+2],w0
	add	w0,#4,w2
	mov	[w14],w3
	mov	[w14+4],w0
	add	w0,#4,w0
	mov	[w0],w1
	mov	#16384,w0
	mul.ss	w0,w1,w0
	mov	w1,[w15++]
	repeat	#18-1
	div.sd	w0,w3
	mov	[--w15],w1
	mov	w0,[w2]
	bra	.L55
.L54:
.LSM161:
	mov	[w14+2],w0
	inc2	w0,w0
	mov	[w14+2],w1
	add	w1,#4,w1
	clr	w2
	mov	w2,[w1]
	mov	[w1],w1
	mov	w1,[w0]
	mov	[w0],w1
	mov	[w14+2],w0
	mov	w1,[w0]
.L55:
.LSM162:
	mov	[w14],w0
.LSM163:
	ulnk	
	return	
	.set ___PA___,0
.LFE11:
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
.LSFDE22:
	.4byte	.LEFDE22-.LASFDE22
.LASFDE22:
	.4byte	.Lframe0
	.4byte	.LFB11
	.4byte	.LFE11-.LFB11
	.align	4
.LEFDE22:
	.section	.text,code
.Letext0:
	.section	.debug_info,info
	.4byte	0x70c
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.asciz	"GNU C 4.5.1 (XC16, Microchip v1_10) (A) Build date: Jul 26 2012"
	.byte	0x1
	.ascii	"../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/lib"
	.asciz	"DCM/mathlibNAV.c"
	.ascii	"/home/markw/MPLABXProjects/gentlenav/branches/MatrixPillotQuad_MarkW"
	.asciz	"hitehorn/educational/fractionalArithmetic"
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.asciz	"unsigned char"
	.uleb128 0x3
	.asciz	"ww"
	.byte	0x4
	.byte	0x2
	.byte	0x1d
	.4byte	0x153
	.uleb128 0x4
	.asciz	"W0"
	.byte	0x2
	.byte	0x1d
	.4byte	0x153
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"W1"
	.byte	0x2
	.byte	0x1d
	.4byte	0x153
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.asciz	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.asciz	"long int"
	.uleb128 0x5
	.asciz	"longww"
	.byte	0x4
	.byte	0x2
	.byte	0x21
	.4byte	0x189
	.uleb128 0x6
	.asciz	"WW"
	.byte	0x2
	.byte	0x21
	.4byte	0x15a
	.uleb128 0x6
	.asciz	"_"
	.byte	0x2
	.byte	0x21
	.4byte	0x12d
	.byte	0x0
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.asciz	"unsigned int"
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.asciz	"char"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.asciz	"long long int"
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.asciz	"long long unsigned int"
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.asciz	"short unsigned int"
	.uleb128 0x3
	.asciz	"relative2D"
	.byte	0x4
	.byte	0x3
	.byte	0x1b
	.4byte	0x20e
	.uleb128 0x4
	.asciz	"x"
	.byte	0x3
	.byte	0x1b
	.4byte	0x153
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"y"
	.byte	0x3
	.byte	0x1b
	.4byte	0x153
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x2
	.byte	0x4
	.byte	0x4
	.asciz	"float"
	.uleb128 0x7
	.byte	0x1
	.asciz	"sine"
	.byte	0x1
	.byte	0x29
	.byte	0x1
	.4byte	0x153
	.4byte	.LFB0
	.4byte	.LFE0
	.byte	0x1
	.byte	0x5e
	.4byte	0x256
	.uleb128 0x8
	.4byte	.LASF0
	.byte	0x1
	.byte	0x29
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x9
	.asciz	"angle_int"
	.byte	0x1
	.byte	0x2c
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.asciz	"signed char"
	.uleb128 0x7
	.byte	0x1
	.asciz	"arcsine"
	.byte	0x1
	.byte	0x48
	.byte	0x1
	.4byte	0x256
	.4byte	.LFB1
	.4byte	.LFE1
	.byte	0x1
	.byte	0x5e
	.4byte	0x2d3
	.uleb128 0xa
	.asciz	"y"
	.byte	0x1
	.byte	0x48
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xb
	.4byte	.LASF0
	.byte	0x1
	.byte	0x4d
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 3
	.uleb128 0x9
	.asciz	"doubleangle"
	.byte	0x1
	.byte	0x4e
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x9
	.asciz	"step"
	.byte	0x1
	.byte	0x4f
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 1
	.uleb128 0x9
	.asciz	"sign"
	.byte	0x1
	.byte	0x50
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.byte	0x0
	.uleb128 0x7
	.byte	0x1
	.asciz	"cosine"
	.byte	0x1
	.byte	0x73
	.byte	0x1
	.4byte	0x153
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x1
	.byte	0x5e
	.4byte	0x300
	.uleb128 0x8
	.4byte	.LASF0
	.byte	0x1
	.byte	0x73
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xc
	.byte	0x1
	.asciz	"rotate"
	.byte	0x1
	.byte	0x79
	.byte	0x1
	.4byte	.LFB3
	.4byte	.LFE3
	.byte	0x1
	.byte	0x5e
	.4byte	0x386
	.uleb128 0xa
	.asciz	"xy"
	.byte	0x1
	.byte	0x79
	.4byte	0x386
	.byte	0x2
	.byte	0x7e
	.sleb128 12
	.uleb128 0x8
	.4byte	.LASF0
	.byte	0x1
	.byte	0x79
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 14
	.uleb128 0x9
	.asciz	"cosang"
	.byte	0x1
	.byte	0x7d
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x9
	.asciz	"sinang"
	.byte	0x1
	.byte	0x7d
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x9
	.asciz	"newx"
	.byte	0x1
	.byte	0x7d
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x9
	.asciz	"newy"
	.byte	0x1
	.byte	0x7d
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x9
	.asciz	"accum"
	.byte	0x1
	.byte	0x7e
	.4byte	0x166
	.byte	0x2
	.byte	0x7e
	.sleb128 8
	.byte	0x0
	.uleb128 0xd
	.byte	0x2
	.4byte	0x1e2
	.uleb128 0x7
	.byte	0x1
	.asciz	"rect_to_polar"
	.byte	0x1
	.byte	0x8b
	.byte	0x1
	.4byte	0x256
	.4byte	.LFB4
	.4byte	.LFE4
	.byte	0x1
	.byte	0x5e
	.4byte	0x409
	.uleb128 0xa
	.asciz	"xy"
	.byte	0x1
	.byte	0x8b
	.4byte	0x386
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x9
	.asciz	"theta"
	.byte	0x1
	.byte	0x92
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0xb
	.4byte	.LASF1
	.byte	0x1
	.byte	0x93
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 1
	.uleb128 0xb
	.4byte	.LASF2
	.byte	0x1
	.byte	0x94
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x9
	.asciz	"steps"
	.byte	0x1
	.byte	0x95
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 3
	.uleb128 0xb
	.4byte	.LASF3
	.byte	0x1
	.byte	0x96
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.byte	0x0
	.uleb128 0x7
	.byte	0x1
	.asciz	"rect_to_polar16"
	.byte	0x1
	.byte	0xb8
	.byte	0x1
	.4byte	0x153
	.4byte	.LFB5
	.4byte	.LFE5
	.byte	0x1
	.byte	0x5e
	.4byte	0x49a
	.uleb128 0xa
	.asciz	"xy"
	.byte	0x1
	.byte	0xb8
	.4byte	0x386
	.byte	0x2
	.byte	0x7e
	.sleb128 8
	.uleb128 0xb
	.4byte	.LASF3
	.byte	0x1
	.byte	0xbf
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x9
	.asciz	"theta16"
	.byte	0x1
	.byte	0xc0
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x9
	.asciz	"theta"
	.byte	0x1
	.byte	0xc1
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xb
	.4byte	.LASF1
	.byte	0x1
	.byte	0xc2
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 5
	.uleb128 0xb
	.4byte	.LASF2
	.byte	0x1
	.byte	0xc3
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x9
	.asciz	"steps"
	.byte	0x1
	.byte	0xc4
	.4byte	0x256
	.byte	0x2
	.byte	0x7e
	.sleb128 7
	.byte	0x0
	.uleb128 0x7
	.byte	0x1
	.asciz	"sqrt_int"
	.byte	0x1
	.byte	0xeb
	.byte	0x1
	.4byte	0x189
	.4byte	.LFB6
	.4byte	.LFE6
	.byte	0x1
	.byte	0x5e
	.4byte	0x4f3
	.uleb128 0xa
	.asciz	"sqr"
	.byte	0x1
	.byte	0xeb
	.4byte	0x189
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0xb
	.4byte	.LASF4
	.byte	0x1
	.byte	0xee
	.4byte	0x189
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0xb
	.4byte	.LASF5
	.byte	0x1
	.byte	0xef
	.4byte	0x189
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xb
	.4byte	.LASF6
	.byte	0x1
	.byte	0xf1
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.byte	0x0
	.uleb128 0xe
	.byte	0x1
	.asciz	"sqrt_long"
	.byte	0x1
	.2byte	0x105
	.byte	0x1
	.4byte	0x189
	.4byte	.LFB7
	.4byte	.LFE7
	.byte	0x1
	.byte	0x5e
	.4byte	0x552
	.uleb128 0xf
	.asciz	"sqr"
	.byte	0x1
	.2byte	0x105
	.4byte	0x552
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x10
	.4byte	.LASF4
	.byte	0x1
	.2byte	0x108
	.4byte	0x189
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0x10
	.4byte	.LASF5
	.byte	0x1
	.2byte	0x109
	.4byte	0x189
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x10
	.4byte	.LASF6
	.byte	0x1
	.2byte	0x10b
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.byte	0x0
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.asciz	"long unsigned int"
	.uleb128 0xe
	.byte	0x1
	.asciz	"vector2_mag"
	.byte	0x1
	.2byte	0x120
	.byte	0x1
	.4byte	0x189
	.4byte	.LFB8
	.4byte	.LFE8
	.byte	0x1
	.byte	0x5e
	.4byte	0x5b8
	.uleb128 0xf
	.asciz	"x"
	.byte	0x1
	.2byte	0x120
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xf
	.asciz	"y"
	.byte	0x1
	.2byte	0x120
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x11
	.asciz	"magsqr"
	.byte	0x1
	.2byte	0x122
	.4byte	0x552
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xe
	.byte	0x1
	.asciz	"vector3_mag"
	.byte	0x1
	.2byte	0x127
	.byte	0x1
	.4byte	0x189
	.4byte	.LFB9
	.4byte	.LFE9
	.byte	0x1
	.byte	0x5e
	.4byte	0x616
	.uleb128 0xf
	.asciz	"x"
	.byte	0x1
	.2byte	0x127
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xf
	.asciz	"y"
	.byte	0x1
	.2byte	0x127
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0xf
	.asciz	"z"
	.byte	0x1
	.2byte	0x127
	.4byte	0x153
	.byte	0x2
	.byte	0x7e
	.sleb128 8
	.uleb128 0x11
	.asciz	"magsqr"
	.byte	0x1
	.2byte	0x129
	.4byte	0x552
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xe
	.byte	0x1
	.asciz	"vector2_normalize"
	.byte	0x1
	.2byte	0x12e
	.byte	0x1
	.4byte	0x189
	.4byte	.LFB10
	.4byte	.LFE10
	.byte	0x1
	.byte	0x5e
	.4byte	0x670
	.uleb128 0x12
	.4byte	.LASF5
	.byte	0x1
	.2byte	0x12e
	.4byte	0x670
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xf
	.asciz	"input"
	.byte	0x1
	.2byte	0x12e
	.4byte	0x670
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x10
	.4byte	.LASF7
	.byte	0x1
	.2byte	0x130
	.4byte	0x189
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xd
	.byte	0x2
	.4byte	0x153
	.uleb128 0xe
	.byte	0x1
	.asciz	"vector3_normalize"
	.byte	0x1
	.2byte	0x13e
	.byte	0x1
	.4byte	0x189
	.4byte	.LFB11
	.4byte	.LFE11
	.byte	0x1
	.byte	0x5e
	.4byte	0x6d0
	.uleb128 0x12
	.4byte	.LASF5
	.byte	0x1
	.2byte	0x13e
	.4byte	0x670
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xf
	.asciz	"input"
	.byte	0x1
	.2byte	0x13e
	.4byte	0x670
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x10
	.4byte	.LASF7
	.byte	0x1
	.2byte	0x140
	.4byte	0x189
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0x13
	.4byte	0x153
	.4byte	0x6e0
	.uleb128 0x14
	.4byte	0x189
	.byte	0x40
	.byte	0x0
	.uleb128 0x15
	.asciz	"sintab"
	.byte	0x1
	.byte	0x1f
	.4byte	0x6f0
	.byte	0x1
	.byte	0x1
	.uleb128 0x16
	.4byte	0x6d0
	.uleb128 0x17
	.asciz	"sintab"
	.byte	0x1
	.byte	0x1f
	.4byte	0x70a
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_sintab
	.uleb128 0x16
	.4byte	0x6d0
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
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x17
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
	.uleb128 0x2e
	.byte	0x1
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
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
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
	.uleb128 0x9
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
	.uleb128 0xa
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
	.uleb128 0xb
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
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
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
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0x2e
	.byte	0x1
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
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x5
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
	.uleb128 0x10
	.uleb128 0x34
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
	.uleb128 0x11
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
	.uleb128 0x12
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
	.uleb128 0x13
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
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
	.uleb128 0x16
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x17
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
	.4byte	0xd1
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x710
	.4byte	0x217
	.asciz	"sine"
	.4byte	0x265
	.asciz	"arcsine"
	.4byte	0x2d3
	.asciz	"cosine"
	.4byte	0x300
	.asciz	"rotate"
	.4byte	0x38c
	.asciz	"rect_to_polar"
	.4byte	0x409
	.asciz	"rect_to_polar16"
	.4byte	0x49a
	.asciz	"sqrt_int"
	.4byte	0x4f3
	.asciz	"sqrt_long"
	.4byte	0x567
	.asciz	"vector2_mag"
	.4byte	0x5b8
	.asciz	"vector3_mag"
	.4byte	0x616
	.asciz	"vector2_normalize"
	.4byte	0x676
	.asciz	"vector3_normalize"
	.4byte	0x6f5
	.asciz	"sintab"
	.4byte	0x0
	.section	.debug_pubtypes,info
	.4byte	0x2f
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x710
	.4byte	0x12d
	.asciz	"ww"
	.4byte	0x166
	.asciz	"longww"
	.4byte	0x1e2
	.asciz	"relative2D"
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
	.ascii	"../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/lib"
	.ascii	"DCM"
	.byte 0
	.ascii	"../../../../../gentlenav/branches/MatrixPillotQuad_MarkWhitehorn/lib"
	.ascii	"DCM/../libUDB"
	.byte 0
	.byte	0x0
	.asciz	"mathlibNAV.c"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"../libUDB/libUDB_defines.h"
	.uleb128 0x1
	.uleb128 0x0
	.uleb128 0x0
	.asciz	"libDCM_defines.h"
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
	.byte	0x3e
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM1
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM2
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM3
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM4
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM5
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM6
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM7
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM8
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM9
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM10
	.byte	0x17
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
	.4byte	.LSM11
	.byte	0x5f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM12
	.byte	0x15
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
	.4byte	.LSM15
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM16
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM17
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM18
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM19
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM20
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM21
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM22
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM23
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM24
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM25
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM26
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM27
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM28
	.byte	0x3
	.sleb128 -15
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM29
	.byte	0x25
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM30
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
	.4byte	.LSM31
	.byte	0x87
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM32
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM33
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
	.4byte	.LSM34
	.byte	0x8d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM35
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM36
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM37
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM38
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM39
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM40
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM41
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM42
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM43
	.byte	0x16
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
	.4byte	.LSM44
	.byte	0x9f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM45
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM46
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM47
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM48
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM49
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM50
	.byte	0x13
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM51
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM52
	.byte	0x13
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM53
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM54
	.byte	0x13
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM55
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM56
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM57
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM58
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM59
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM60
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM61
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM62
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM63
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM64
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM65
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM66
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM67
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM68
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM69
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM70
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM71
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM72
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
	.4byte	.LSM73
	.byte	0xcc
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM74
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM75
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM76
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM77
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM78
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM79
	.byte	0x13
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM80
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM81
	.byte	0x13
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM82
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM83
	.byte	0x13
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM84
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM85
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM86
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM87
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM88
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM89
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM90
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM91
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM92
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM93
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM94
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM95
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM96
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM97
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM98
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM99
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM100
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM101
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM102
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM103
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM104
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
	.4byte	.LSM105
	.byte	0x3
	.sleb128 235
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM106
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM107
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM108
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM109
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM110
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM111
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM112
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM113
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM114
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM115
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM116
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM117
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM118
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM119
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM120
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM121
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
	.4byte	.LSM122
	.byte	0x3
	.sleb128 261
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM123
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM124
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM125
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM126
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM127
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM128
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM129
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM130
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM131
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM132
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM133
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM134
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM135
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM136
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM137
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM138
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
	.4byte	.LSM139
	.byte	0x3
	.sleb128 288
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM140
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM141
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM142
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
	.4byte	.LSM143
	.byte	0x3
	.sleb128 295
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM144
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM145
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM146
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
	.4byte	.LSM147
	.byte	0x3
	.sleb128 302
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM148
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM149
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM150
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM151
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM152
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM153
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM154
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE10
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM155
	.byte	0x3
	.sleb128 318
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM156
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM157
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM158
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM159
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM160
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM161
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM162
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LSM163
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LFE11
	.byte	0x0
	.uleb128 0x1
	.byte	0x1
.LELT0:
	.section	.debug_str,info
.LASF6:
	.asciz	"iterations"
.LASF1:
	.asciz	"delta_theta"
.LASF5:
	.asciz	"result"
.LASF4:
	.asciz	"binary_point"
.LASF2:
	.asciz	"theta_rot"
.LASF3:
	.asciz	"scaleShift"
.LASF0:
	.asciz	"angle"
.LASF7:
	.asciz	"magnitude"
	.section	.text,code

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

; MCHP configuration words

	.set ___PA___,0
	.end
