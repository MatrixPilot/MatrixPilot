	.file "../../libDCM/mathlibNAV.c"
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
.LFB2:
.LM1:
	.set ___PA___,1
	lnk	#6
	mov.b	w0,[w14+2]
.LM2:
	mov.b	[w14+2],w0
	se	w0,w0
	mov	w0,[w14]
.LM3:
	cp0	[w14]
	.set ___BP___,0
	bra	lt,.L2
.LM4:
	mov	#64,w0
	subr	w0,[w14],[w15]
	.set ___BP___,0
	bra	le,.L4
.LM5:
	mov	#128,w0
	sub	w0,[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	mov	w0,[w14+4]
	bra	.L6
.L4:
.LM6:
	mov	[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	mov	w0,[w14+4]
	bra	.L6
.L2:
.LM7:
	neg	[w14],[w14]
.LM8:
	mov	#64,w0
	subr	w0,[w14],[w15]
	.set ___BP___,0
	bra	le,.L7
.LM9:
	mov	#128,w0
	sub	w0,[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	neg	w0,w0
	mov	w0,[w14+4]
	bra	.L6
.L7:
.LM10:
	mov	[w14],w0
	add	w0,w0,w1
	mov	#_sintab,w0
	add	w1,w0,w0
	mov	[w0],w0
	neg	w0,w0
	mov	w0,[w14+4]
.L6:
	mov	[w14+4],w0
.LM11:
	ulnk	
	return	
	.set ___PA___,0
.LFE2:
	.align	2
	.global	_arcsine	; export
	.type	_arcsine,@function
_arcsine:
.LFB3:
.LM12:
	.set ___PA___,1
	lnk	#8
	mov	w8,[w15++]
	mov	w0,[w14+4]
.LM13:
	mov.b	#32,w0
	mov.b	w0,[w14+3]
.LM14:
	mov.b	#64,w0
	mov.b	w0,[w14+2]
.LM15:
	mov.b	#32,w0
	mov.b	w0,[w14+1]
.LM16:
	mov	[w14+4],w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L11
.LM17:
	mov.b	#1,w0
	mov.b	w0,[w14]
	bra	.L13
.L11:
.LM18:
	setm.b	w0
	mov.b	w0,[w14]
.LM19:
	mov	[w14+4],w0
	neg	w0,w0
	mov	w0,[w14+4]
.L13:
.LM20:
	mov	[w14+4],w1
	mov	#16384,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	nz,.L17
.LM21:
	se	[w14],w0
	sl	w0,#6,w0
	se	w0,w0
	mov	w0,[w14+6]
	bra	.L16
.L18:
.LM22:
	mov.b	[w14+2],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,[w14+3]
.LM23:
	mov.b	[w14+3],w0
	call	_sine
	mov	w0,w1
	mov	[w14+4],w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	nz,.L19
.LM24:
	mov.b	[w14],w1
	mov.b	[w14+3],w0
	mul.ss	w1,w0,w0
	se	w0,w0
	mov	w0,[w14+6]
	bra	.L16
.L19:
.LM25:
	mov.b	[w14+3],w0
	call	_sine
	mov	w0,w8
	mov.b	[w14+3],w0
	dec.b	w0,w0
	call	_sine
	add	w8,w0,w0
	asr	w0,w1
	mov	[w14+4],w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	ge,.L21
.LM26:
	mov.b	[w14+2],w0
	mov.b	[w14+1],w1
	add.b	w0,w1,w0
	mov.b	w0,[w14+2]
	bra	.L23
.L21:
.LM27:
	mov.b	[w14+2],w0
	mov.b	[w14+1],w1
	sub.b	w0,w1,w0
	mov.b	w0,[w14+2]
.L23:
.LM28:
	mov.b	[w14+1],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,[w14+1]
.L17:
.LM29:
	mov.b	[w14+1],w0
	cp0.b	w0
	.set ___BP___,0
	bra	gt,.L18
.LM30:
	mov.b	[w14+2],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,w1
	mov.b	[w14],w0
	mul.ss	w1,w0,w0
	se	w0,w0
	mov	w0,[w14+6]
.L16:
	mov	[w14+6],w0
.LM31:
	mov	[--w15],w8
	ulnk	
	return	
	.set ___PA___,0
.LFE3:
	.align	2
	.global	_cosine	; export
	.type	_cosine,@function
_cosine:
.LFB4:
.LM32:
	.set ___PA___,1
	lnk	#2
	mov.b	w0,[w14]
.LM33:
	mov.b	#64,w1
	add.b	w1,[w14],w0
	call	_sine
.LM34:
	ulnk	
	return	
	.set ___PA___,0
.LFE4:
	.align	2
	.global	_rotate	; export
	.type	_rotate,@function
_rotate:
.LFB5:
.LM35:
	.set ___PA___,1
	lnk	#16
	mov	w0,[w14+12]
	mov.b	w1,[w14+14]
.LM36:
	mov.b	[w14+14],w0
	call	_sine
	mov	w0,[w14+4]
.LM37:
	mov.b	[w14+14],w0
	call	_cosine
	mov	w0,[w14+6]
.LM38:
	mov	[w14+12],w0
	mov	[w0],w1
	mov	[w14+6],w0
	mul.ss	w0,w1,w2
	mov	[w14+12],w0
	mov	[w0+2],w1
	mov	[w14+4],w0
	mul.ss	w0,w1,w0
	sub	w2,w0,w0
	subb	w3,w1,w1
	sl	w1,#2,w2
	lsr	w0,#14,w1
	ior	w2,w1,w1
	sl	w0,#2,w0
	mov	w0,[w14+8]
	mov	w1,[w14+10]
.LM39:
	mov	[w14+10],w0
	mov	w0,[w14+2]
.LM40:
	mov	[w14+12],w0
	mov	[w0],w1
	mov	[w14+4],w0
	mul.ss	w0,w1,w2
	mov	[w14+12],w0
	mov	[w0+2],w1
	mov	[w14+6],w0
	mul.ss	w0,w1,w0
	add	w0,w2,w0
	addc	w1,w3,w1
	sl	w1,#2,w2
	lsr	w0,#14,w1
	ior	w2,w1,w1
	sl	w0,#2,w0
	mov	w0,[w14+8]
	mov	w1,[w14+10]
.LM41:
	mov	[w14+10],w0
	mov	w0,[w14]
.LM42:
	mov	[w14+12],w0
	mov	[w14+2],w1
	mov	w1,[w0]
.LM43:
	mov	[w14+12],w0
	mov	[w14],w2
	mov	w2,[w0+2]
.LM44:
	ulnk	
	return	
	.set ___PA___,0
.LFE5:
	.align	2
	.global	_rect_to_polar	; export
	.type	_rect_to_polar,@function
_rect_to_polar:
.LFB6:
.LM45:
	.set ___PA___,1
	lnk	#8
	mov	w0,[w14+6]
.LM46:
	clr.b	w0
	mov.b	w0,[w14+5]
.LM47:
	mov.b	#64,w0
	mov.b	w0,[w14+4]
.LM48:
	mov.b	#7,w0
	mov.b	w0,[w14+2]
.LM49:
	mov	[w14+6],w0
	mov	[w0],w1
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L31
	mov	[w14+6],w0
	mov	[w0],w1
	mov	#-255,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L31
	mov	[w14+6],w0
	mov	[w0+2],w1
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L31
	mov	[w14+6],w0
	mov	[w0+2],w1
	mov	#-255,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L31
.LM50:
	mov	#6,w0
	mov	w0,[w14]
.LM51:
	mov	[w14+6],w0
	mov	[w0],w0
	sl	w0,#6,w1
	mov	[w14+6],w0
	mov	w1,[w0]
.LM52:
	mov	[w14+6],w0
	mov	[w0+2],w0
	sl	w0,#6,w1
	mov	[w14+6],w0
	mov	w1,[w0+2]
.LM53:
	bra	.L37
.L31:
.LM54:
	clr	w0
	mov	w0,[w14]
.LM55:
	bra	.L37
.L38:
.LM56:
	mov.b	[w14+4],w0
	mov.b	w0,[w14+3]
.LM57:
	mov	[w14+6],w0
	mov	[w0+2],w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L39
	mov.b	[w14+3],w0
	neg.b	w0,w0
	mov.b	w0,[w14+3]
.L39:
.LM58:
	mov.b	[w14+3],w1
	mov	[w14+6],w0
	call	_rotate
.LM59:
	mov.b	[w14+5],w1
	mov.b	[w14+3],w0
	add.b	w1,w0,w0
	mov.b	w0,[w14+5]
.LM60:
	mov.b	[w14+4],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,[w14+4]
.LM61:
	mov.b	[w14+2],w0
	dec.b	w0,w0
	mov.b	w0,[w14+2]
.L37:
.LM62:
	mov.b	[w14+2],w0
	cp0.b	w0
	.set ___BP___,0
	bra	gt,.L38
.LM63:
	mov	[w14+6],w0
	mov	[w0+2],w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L42
	mov.b	[w14+5],w0
	dec.b	w0,w0
	mov.b	w0,[w14+5]
.L42:
.LM64:
	mov	[w14+6],w0
	mov	[w0],w1
	mov	[w14],w0
	asr	w1,w0,w1
	mov	[w14+6],w0
	mov	w1,[w0]
.LM65:
	mov	[w14+6],w0
	mov	[w0+2],w1
	mov	[w14],w0
	asr	w1,w0,w1
	mov	[w14+6],w0
	mov	w1,[w0+2]
.LM66:
	mov.b	[w14+5],w0
	neg.b	w0,w0
	se	w0,w0
.LM67:
	ulnk	
	return	
	.set ___PA___,0
.LFE6:
	.align	2
	.global	_rect_to_polar16	; export
	.type	_rect_to_polar16,@function
_rect_to_polar16:
.LFB7:
.LM68:
	.set ___PA___,1
	lnk	#10
	mov	w0,[w14+8]
.LM69:
	clr.b	w0
	mov.b	w0,[w14+3]
.LM70:
	mov.b	#64,w0
	mov.b	w0,[w14+2]
.LM71:
	mov.b	#7,w0
	mov.b	w0,[w14]
.LM72:
	mov	[w14+8],w0
	mov	[w0],w1
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L46
	mov	[w14+8],w0
	mov	[w0],w1
	mov	#-255,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L46
	mov	[w14+8],w0
	mov	[w0+2],w1
	mov	#254,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	gt,.L46
	mov	[w14+8],w0
	mov	[w0+2],w1
	mov	#-255,w0
	sub	w1,w0,[w15]
	.set ___BP___,0
	bra	le,.L46
.LM73:
	mov	#6,w0
	mov	w0,[w14+6]
.LM74:
	mov	[w14+8],w0
	mov	[w0],w0
	sl	w0,#6,w1
	mov	[w14+8],w0
	mov	w1,[w0]
.LM75:
	mov	[w14+8],w0
	mov	[w0+2],w0
	sl	w0,#6,w1
	mov	[w14+8],w0
	mov	w1,[w0+2]
.LM76:
	bra	.L52
.L46:
.LM77:
	clr	w0
	mov	w0,[w14+6]
.LM78:
	bra	.L52
.L53:
.LM79:
	mov.b	[w14+2],w0
	mov.b	w0,[w14+1]
.LM80:
	mov	[w14+8],w0
	mov	[w0+2],w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L54
	mov.b	[w14+1],w0
	neg.b	w0,w0
	mov.b	w0,[w14+1]
.L54:
.LM81:
	mov.b	[w14+1],w1
	mov	[w14+8],w0
	call	_rotate
.LM82:
	mov.b	[w14+3],w1
	mov.b	[w14+1],w0
	add.b	w1,w0,w0
	mov.b	w0,[w14+3]
.LM83:
	mov.b	[w14+2],w0
	se	w0,w0
	asr	w0,w0
	mov.b	w0,[w14+2]
.LM84:
	dec.b	[w14],[w14]
.L52:
.LM85:
	cp0.b	[w14]
	.set ___BP___,0
	bra	gt,.L53
.LM86:
	mov.b	[w14+3],w0
	neg.b	w0,w0
	mov.b	w0,[w14+3]
.LM87:
	mov.b	[w14+3],w0
	se	w0,w0
	sl	w0,#8,w0
	mov	w0,[w14+4]
.LM88:
	mov	[w14+8],w0
	mov	[w0],w0
	cp0	w0
	.set ___BP___,0
	bra	le,.L57
.LM89:
	mov	[w14+8],w0
	mov	[w0],w2
	mov	[w14+8],w0
	mov	[w0+2],w1
	mov	#10430,w0
	mul.ss	w0,w1,w0
	mov	w0,[w15++]
	repeat	#18-1
	div.sd	w0,w2
	mov	w0,w1
	mov	[--w15],w0
	mov	[w14+4],w0
	add	w0,w1,w0
	mov	w0,[w14+4]
.L57:
.LM90:
	mov	[w14+8],w0
	mov	[w0],w1
	mov	[w14+6],w0
	asr	w1,w0,w1
	mov	[w14+8],w0
	mov	w1,[w0]
.LM91:
	mov	[w14+8],w0
	mov	[w0+2],w1
	mov	[w14+6],w0
	asr	w1,w0,w1
	mov	[w14+8],w0
	mov	w1,[w0+2]
.LM92:
	mov	[w14+4],w0
.LM93:
	ulnk	
	return	
	.set ___PA___,0
.LFE7:
	.align	2
	.global	_sqrt_int	; export
	.type	_sqrt_int,@function
_sqrt_int:
.LFB8:
.LM94:
	.set ___PA___,1
	lnk	#10
	mov	w0,[w14+6]
.LM95:
	clr	w0
	mov	w0,[w14+4]
.LM96:
	mov	#255,w0
	mov	w0,[w14+2]
.LM97:
	mov	#3,w0
	mov	w0,[w14]
.LM98:
	mov	[w14+6],w0
	cp0	w0
	.set ___BP___,0
	bra	nz,.L64
.LM99:
	clr	w0
	mov	w0,[w14+8]
	bra	.L63
.L65:
.LM100:
	mov	[w14+6],w0
	sl	w0,#2,w0
	mov	w0,[w14+6]
.LM101:
	mov	[w14+4],w0
	inc	w0,w0
	mov	w0,[w14+4]
.L64:
.LM102:
	mov	[w14+6],w0
	mov	#-16384,w1
	and	w0,w1,w0
	cp0	w0
	.set ___BP___,0
	bra	z,.L65
.LM103:
	mov	[w14+6],w0
	lsr	w0,w0
	mov	w0,[w14+6]
.LM104:
	bra	.L67
.L68:
.LM105:
	dec	[w14],[w14]
.LM106:
	mov	[w14+2],w0
	lsr	w0,w1
	mov	[w14+6],w0
	mov	[w14+2],w2
	mov	w1,[w15++]
	repeat	#18-1
	div.uw	w0,w2
	mov	w1,w2
	mov	[--w15],w1
	add	w1,w0,w0
	mov	w0,[w14+2]
.L67:
.LM107:
	cp0	[w14]
	.set ___BP___,0
	bra	nz,.L68
.LM108:
	mov	[w14+4],w0
	mov	[w14+2],w1
	lsr	w1,w0,w0
	mov	w0,[w14+2]
.LM109:
	mov	[w14+2],w0
	mov	w0,[w14+8]
.L63:
	mov	[w14+8],w0
.LM110:
	ulnk	
	return	
	.set ___PA___,0
.LFE8:
	.align	2
	.global	_sqrt_long	; export
	.type	_sqrt_long,@function
_sqrt_long:
.LFB9:
.LM111:
	.set ___PA___,1
	lnk	#16
	mov	w0,[w14+6]
	mov	w1,[w14+8]
.LM112:
	clr	w0
	mov	w0,[w14+4]
.LM113:
	setm	w0
	mov	w0,[w14+2]
.LM114:
	mov	#3,w0
	mov	w0,[w14]
.LM115:
	mov	[w14+6],w2
	mov	[w14+8],w3
	mov	#65535,w0
	mov	#0,w1
	sub	w2,w0,[w15]
	subb	w3,w1,[w15]
	.set ___BP___,0
	bra	gtu,.L75
.LM116:
	mov	[w14+6],w0
	call	_sqrt_int
	mov	w0,[w14+10]
	bra	.L74
.L76:
.LM117:
	mov	[w14+6],w0
	mov	[w14+8],w1
	sl	w1,#2,w2
	lsr	w0,#14,w1
	ior	w2,w1,w1
	sl	w0,#2,w0
	mov	w0,[w14+6]
	mov	w1,[w14+8]
.LM118:
	mov	[w14+4],w0
	inc	w0,w0
	mov	w0,[w14+4]
.L75:
.LM119:
	mov	[w14+6],w0
	mov	[w14+8],w1
	mov	#0,w2
	mov	#-16384,w3
	mov	w0,w4
	mov	w1,w5
	mov	w2,w0
	mov	w3,w1
	and	w4,w0,w2
	and	w5,w1,w0
	mul.uu	w0,#1,w4
	mov	w4,[w14+12]
	mov	w5,[w14+14]
	mov	[w14+12],w0
	mov	[w14+14],w1
	sl	w0,#0,w1
	mov	#0,w0
	mov	w0,[w14+12]
	mov	w1,[w14+14]
	mov	[w14+12],w0
	mov	[w14+14],w1
	mul.uu	w2,#1,w4
	mov	w4,[w14+12]
	mov	w5,[w14+14]
	mov	[w14+12],w2
	mov	[w14+14],w3
	ior	w0,w2,w0
	ior	w1,w3,w1
	sub	w0,#0,[w15]
	subb	w1,#0,[w15]
	.set ___BP___,0
	bra	z,.L76
.LM120:
	mov	[w14+6],w0
	mov	[w14+8],w1
	
	lsr w1,w1
	rrc	w0,w0
	mov	w0,[w14+6]
	mov	w1,[w14+8]
.LM121:
	bra	.L78
.L79:
.LM122:
	dec	[w14],[w14]
.LM123:
	mov	[w14+2],w0
	lsr	w0,w3
	mov	[w14+6],w0
	mov	[w14+8],w1
	mov	[w14+2],w2
	mov	w1,[w15++]
	repeat	#18-1
	div.ud	w0,w2
	mov	[--w15],w1
	add	w3,w0,w0
	mov	w0,[w14+2]
.L78:
.LM124:
	cp0	[w14]
	.set ___BP___,0
	bra	nz,.L79
.LM125:
	mov	[w14+4],w0
	mov	[w14+2],w1
	lsr	w1,w0,w0
	mov	w0,[w14+2]
.LM126:
	mov	[w14+2],w3
	mov	w3,[w14+10]
.L74:
	mov	[w14+10],w0
.LM127:
	ulnk	
	return	
	.set ___PA___,0
.LFE9:
	.align	2
	.global	_vector2_mag	; export
	.type	_vector2_mag,@function
_vector2_mag:
.LFB10:
.LM128:
	.set ___PA___,1
	lnk	#8
	mov	w0,[w14+4]
	mov	w1,[w14+6]
.LM129:
	mov	[w14+4],w1
	mov	[w14+4],w0
	mul.ss	w1,w0,w2
	mov	[w14+6],w1
	mov	[w14+6],w0
	mul.ss	w1,w0,w0
	add	w0,w2,w0
	addc	w1,w3,w1
	mov.d	w0,[w14]
.LM130:
	mov.d	[w14],w0
	call	_sqrt_long
.LM131:
	ulnk	
	return	
	.set ___PA___,0
.LFE10:
	.align	2
	.global	_vector3_mag	; export
	.type	_vector3_mag,@function
_vector3_mag:
.LFB11:
.LM132:
	.set ___PA___,1
	lnk	#10
	mov	w0,[w14+4]
	mov	w1,[w14+6]
	mov	w2,[w14+8]
.LM133:
	mov	[w14+4],w1
	mov	[w14+4],w0
	mul.ss	w1,w0,w2
	mov	[w14+6],w1
	mov	[w14+6],w0
	mul.ss	w1,w0,w0
	add	w0,w2,w2
	addc	w1,w3,w3
	mov	[w14+8],w1
	mov	[w14+8],w0
	mul.ss	w1,w0,w0
	add	w0,w2,w0
	addc	w1,w3,w1
	mov.d	w0,[w14]
.LM134:
	mov.d	[w14],w0
	call	_sqrt_long
.LM135:
	ulnk	
	return	
	.set ___PA___,0
.LFE11:
	.align	2
	.global	_vector2_normalize	; export
	.type	_vector2_normalize,@function
_vector2_normalize:
.LFB12:
.LM136:
	.set ___PA___,1
	lnk	#6
	mov	w0,[w14+2]
	mov	w1,[w14+4]
.LM137:
	mov	[w14+4],w0
	inc2	w0,w0
	mov	[w0],w1
	mov	[w14+4],w0
	mov	[w0],w0
	call	_vector2_mag
	mov	w0,[w14]
.LM138:
	cp0	[w14]
	.set ___BP___,0
	bra	z,.L87
.LM139:
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
.LM140:
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
	bra	.L89
.L87:
.LM141:
	mov	[w14+2],w0
	inc2	w0,w1
	clr	w0
	mov	w0,[w1]
	mov	[w1],w1
	mov	[w14+2],w0
	mov	w1,[w0]
.L89:
.LM142:
	mov	[w14],w0
.LM143:
	ulnk	
	return	
	.set ___PA___,0
.LFE12:
	.align	2
	.global	_vector3_normalize	; export
	.type	_vector3_normalize,@function
_vector3_normalize:
.LFB13:
.LM144:
	.set ___PA___,1
	lnk	#6
	mov	w0,[w14+2]
	mov	w1,[w14+4]
.LM145:
	mov	[w14+4],w0
	add	w0,#4,w0
	mov	[w0],w2
	mov	[w14+4],w0
	inc2	w0,w0
	mov	[w0],w1
	mov	[w14+4],w0
	mov	[w0],w0
	call	_vector3_mag
	mov	w0,[w14]
.LM146:
	cp0	[w14]
	.set ___BP___,0
	bra	z,.L92
.LM147:
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
.LM148:
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
.LM149:
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
	bra	.L94
.L92:
.LM150:
	mov	[w14+2],w0
	inc2	w0,w2
	mov	[w14+2],w0
	add	w0,#4,w1
	clr	w0
	mov	w0,[w1]
	mov	[w1],w0
	mov	w0,[w2]
	mov	[w2],w1
	mov	[w14+2],w0
	mov	w1,[w0]
.L94:
.LM151:
	mov	[w14],w0
.LM152:
	ulnk	
	return	
	.set ___PA___,0
.LFE13:
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
	.asciz	"mathlibNAV.c"
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
	.byte	0x3e
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
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM5
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM6
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM7
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM8
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM9
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM10
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM11
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM12
	.byte	0x1b
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM13
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM14
	.byte	0x15
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
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM18
	.byte	0x18
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
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM22
	.byte	0x18
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
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM26
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM27
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM28
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM29
	.byte	0x3
	.sleb128 -15
	.byte	0x1
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM30
	.byte	0x25
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM31
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM32
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM33
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM34
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM35
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM36
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM37
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM38
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM39
	.byte	0x15
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
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM43
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM44
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM45
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM46
	.byte	0x1a
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM47
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM48
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM49
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM50
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM51
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM52
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM53
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM54
	.byte	0x1f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM55
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM56
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM57
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM58
	.byte	0x15
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
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM62
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM63
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM64
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM65
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM66
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM67
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM68
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM69
	.byte	0x1c
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM70
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM71
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM72
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM73
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM74
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM75
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM76
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM77
	.byte	0x1f
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM78
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM79
	.byte	0x16
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
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM85
	.byte	0xd
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM86
	.byte	0x1d
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM87
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM88
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM89
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM90
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM91
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM92
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM93
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM94
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM95
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM96
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM97
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM98
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM99
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM100
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM101
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM102
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM103
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM104
	.byte	0x15
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
	.4byte	.LM107
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM108
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM109
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM110
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM111
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM112
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM113
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM114
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM115
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM116
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM117
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM118
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM119
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM120
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM121
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM122
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM123
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM124
	.byte	0x11
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM125
	.byte	0x19
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM126
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM127
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM128
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM129
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM130
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM131
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM132
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM133
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM134
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM135
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM136
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM137
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM138
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM139
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM140
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM141
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM142
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM143
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM144
	.byte	0x17
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM145
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM146
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM147
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM148
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM149
	.byte	0x15
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM150
	.byte	0x18
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM151
	.byte	0x16
	.byte	0x0
	.uleb128 0x5
	.byte	0x2
	.4byte	.LM152
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
	.4byte	0x68a
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.Ldebug_line0
	.4byte	.Letext0
	.4byte	.Ltext0
	.asciz	"GNU C 4.0.3 (dsPIC30, Microchip v3_30) (A) Build date: Jun  2 2011"
	.byte	0x1
	.asciz	"../../libDCM/mathlibNAV.c"
	.ascii	"/home/markw/Dropbox/autopilots/UAVdevboard/UDB4/mplabx_projects/MPQp"
	.asciz	"id400/MatrixPilotQuad/MPQpid400.X"
	.uleb128 0x2
	.asciz	"unsigned char"
	.byte	0x1
	.byte	0x8
	.uleb128 0x3
	.4byte	0x113
	.asciz	"ww"
	.byte	0x4
	.byte	0x1
	.byte	0x1d
	.uleb128 0x4
	.asciz	"W0"
	.byte	0x1
	.byte	0x1d
	.4byte	0x113
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"W1"
	.byte	0x1
	.byte	0x1d
	.4byte	0x113
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
	.4byte	0x149
	.asciz	"longww"
	.byte	0x4
	.byte	0x1
	.byte	0x21
	.uleb128 0x6
	.asciz	"WW"
	.byte	0x1
	.byte	0x21
	.4byte	0x11a
	.uleb128 0x6
	.asciz	"_"
	.byte	0x1
	.byte	0x21
	.4byte	0xed
	.byte	0x0
	.uleb128 0x7
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
	.uleb128 0x3
	.4byte	0x1af
	.asciz	"relative2D"
	.byte	0x4
	.byte	0x7
	.byte	0x1b
	.uleb128 0x4
	.asciz	"x"
	.byte	0x7
	.byte	0x1b
	.4byte	0x113
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x4
	.asciz	"y"
	.byte	0x7
	.byte	0x1b
	.4byte	0x113
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x2
	.asciz	"float"
	.byte	0x4
	.byte	0x4
	.uleb128 0x8
	.4byte	0x1f7
	.byte	0x1
	.asciz	"sine"
	.byte	0x9
	.byte	0x2b
	.byte	0x1
	.4byte	0x113
	.4byte	.LFB2
	.4byte	.LFE2
	.byte	0x1
	.byte	0x5e
	.uleb128 0x9
	.4byte	.LASF1
	.byte	0x9
	.byte	0x29
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xa
	.asciz	"angle_int"
	.byte	0x9
	.byte	0x2c
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0x2
	.asciz	"signed char"
	.byte	0x1
	.byte	0x6
	.uleb128 0x8
	.4byte	0x274
	.byte	0x1
	.asciz	"arcsine"
	.byte	0x9
	.byte	0x4c
	.byte	0x1
	.4byte	0x1f7
	.4byte	.LFB3
	.4byte	.LFE3
	.byte	0x1
	.byte	0x5e
	.uleb128 0xb
	.asciz	"y"
	.byte	0x9
	.byte	0x48
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xc
	.4byte	.LASF1
	.byte	0x9
	.byte	0x4d
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 3
	.uleb128 0xa
	.asciz	"doubleangle"
	.byte	0x9
	.byte	0x4e
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xa
	.asciz	"step"
	.byte	0x9
	.byte	0x4f
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 1
	.uleb128 0xa
	.asciz	"sign"
	.byte	0x9
	.byte	0x50
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0x8
	.4byte	0x2a1
	.byte	0x1
	.asciz	"cosine"
	.byte	0x9
	.byte	0x74
	.byte	0x1
	.4byte	0x113
	.4byte	.LFB4
	.4byte	.LFE4
	.byte	0x1
	.byte	0x5e
	.uleb128 0x9
	.4byte	.LASF1
	.byte	0x9
	.byte	0x73
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xd
	.4byte	0x327
	.byte	0x1
	.asciz	"rotate"
	.byte	0x9
	.byte	0x7a
	.byte	0x1
	.4byte	.LFB5
	.4byte	.LFE5
	.byte	0x1
	.byte	0x5e
	.uleb128 0xb
	.asciz	"xy"
	.byte	0x9
	.byte	0x79
	.4byte	0x327
	.byte	0x2
	.byte	0x7e
	.sleb128 12
	.uleb128 0x9
	.4byte	.LASF1
	.byte	0x9
	.byte	0x79
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 14
	.uleb128 0xa
	.asciz	"cosang"
	.byte	0x9
	.byte	0x7d
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0xa
	.asciz	"sinang"
	.byte	0x9
	.byte	0x7d
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xa
	.asciz	"newx"
	.byte	0x9
	.byte	0x7d
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xa
	.asciz	"newy"
	.byte	0x9
	.byte	0x7d
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.uleb128 0xa
	.asciz	"accum"
	.byte	0x9
	.byte	0x7e
	.4byte	0x126
	.byte	0x2
	.byte	0x7e
	.sleb128 8
	.byte	0x0
	.uleb128 0xe
	.byte	0x2
	.4byte	0x183
	.uleb128 0x8
	.4byte	0x3aa
	.byte	0x1
	.asciz	"rect_to_polar"
	.byte	0x9
	.byte	0x8c
	.byte	0x1
	.4byte	0x1f7
	.4byte	.LFB6
	.4byte	.LFE6
	.byte	0x1
	.byte	0x5e
	.uleb128 0xb
	.asciz	"xy"
	.byte	0x9
	.byte	0x8b
	.4byte	0x327
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0xa
	.asciz	"theta"
	.byte	0x9
	.byte	0x92
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 5
	.uleb128 0xc
	.4byte	.LASF2
	.byte	0x9
	.byte	0x93
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xc
	.4byte	.LASF3
	.byte	0x9
	.byte	0x94
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 3
	.uleb128 0xa
	.asciz	"steps"
	.byte	0x9
	.byte	0x95
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xc
	.4byte	.LASF4
	.byte	0x9
	.byte	0x96
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0x8
	.4byte	0x43b
	.byte	0x1
	.asciz	"rect_to_polar16"
	.byte	0x9
	.byte	0xb9
	.byte	0x1
	.4byte	0x113
	.4byte	.LFB7
	.4byte	.LFE7
	.byte	0x1
	.byte	0x5e
	.uleb128 0xb
	.asciz	"xy"
	.byte	0x9
	.byte	0xb8
	.4byte	0x327
	.byte	0x2
	.byte	0x7e
	.sleb128 8
	.uleb128 0xc
	.4byte	.LASF4
	.byte	0x9
	.byte	0xbf
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0xa
	.asciz	"theta16"
	.byte	0x9
	.byte	0xc0
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xa
	.asciz	"theta"
	.byte	0x9
	.byte	0xc1
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 3
	.uleb128 0xc
	.4byte	.LASF2
	.byte	0x9
	.byte	0xc2
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xc
	.4byte	.LASF3
	.byte	0x9
	.byte	0xc3
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 1
	.uleb128 0xa
	.asciz	"steps"
	.byte	0x9
	.byte	0xc4
	.4byte	0x1f7
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0x8
	.4byte	0x494
	.byte	0x1
	.asciz	"sqrt_int"
	.byte	0x9
	.byte	0xec
	.byte	0x1
	.4byte	0x149
	.4byte	.LFB8
	.4byte	.LFE8
	.byte	0x1
	.byte	0x5e
	.uleb128 0xb
	.asciz	"sqr"
	.byte	0x9
	.byte	0xeb
	.4byte	0x149
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0xc
	.4byte	.LASF5
	.byte	0x9
	.byte	0xee
	.4byte	0x149
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0xc
	.4byte	.LASF6
	.byte	0x9
	.byte	0xef
	.4byte	0x149
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0xc
	.4byte	.LASF7
	.byte	0x9
	.byte	0xf1
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xf
	.4byte	0x4f3
	.byte	0x1
	.asciz	"sqrt_long"
	.byte	0x9
	.2byte	0x106
	.byte	0x1
	.4byte	0x149
	.4byte	.LFB9
	.4byte	.LFE9
	.byte	0x1
	.byte	0x5e
	.uleb128 0x10
	.asciz	"sqr"
	.byte	0x9
	.2byte	0x105
	.4byte	0x16e
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x11
	.4byte	.LASF5
	.byte	0x9
	.2byte	0x108
	.4byte	0x149
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x11
	.4byte	.LASF6
	.byte	0x9
	.2byte	0x109
	.4byte	0x149
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x11
	.4byte	.LASF7
	.byte	0x9
	.2byte	0x10b
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xf
	.4byte	0x544
	.byte	0x1
	.asciz	"vector2_mag"
	.byte	0x9
	.2byte	0x121
	.byte	0x1
	.4byte	0x149
	.4byte	.LFB10
	.4byte	.LFE10
	.byte	0x1
	.byte	0x5e
	.uleb128 0x10
	.asciz	"x"
	.byte	0x9
	.2byte	0x120
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x10
	.asciz	"y"
	.byte	0x9
	.2byte	0x120
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x12
	.asciz	"magsqr"
	.byte	0x9
	.2byte	0x122
	.4byte	0x16e
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xf
	.4byte	0x5a2
	.byte	0x1
	.asciz	"vector3_mag"
	.byte	0x9
	.2byte	0x128
	.byte	0x1
	.4byte	0x149
	.4byte	.LFB11
	.4byte	.LFE11
	.byte	0x1
	.byte	0x5e
	.uleb128 0x10
	.asciz	"x"
	.byte	0x9
	.2byte	0x127
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x10
	.asciz	"y"
	.byte	0x9
	.2byte	0x127
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 6
	.uleb128 0x10
	.asciz	"z"
	.byte	0x9
	.2byte	0x127
	.4byte	0x113
	.byte	0x2
	.byte	0x7e
	.sleb128 8
	.uleb128 0x12
	.asciz	"magsqr"
	.byte	0x9
	.2byte	0x129
	.4byte	0x16e
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xf
	.4byte	0x5fc
	.byte	0x1
	.asciz	"vector2_normalize"
	.byte	0x9
	.2byte	0x12f
	.byte	0x1
	.4byte	0x149
	.4byte	.LFB12
	.4byte	.LFE12
	.byte	0x1
	.byte	0x5e
	.uleb128 0x13
	.4byte	.LASF6
	.byte	0x9
	.2byte	0x12e
	.4byte	0x5fc
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x10
	.asciz	"input"
	.byte	0x9
	.2byte	0x12e
	.4byte	0x5fc
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x11
	.4byte	.LASF8
	.byte	0x9
	.2byte	0x130
	.4byte	0x149
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0xe
	.byte	0x2
	.4byte	0x113
	.uleb128 0xf
	.4byte	0x65c
	.byte	0x1
	.asciz	"vector3_normalize"
	.byte	0x9
	.2byte	0x13f
	.byte	0x1
	.4byte	0x149
	.4byte	.LFB13
	.4byte	.LFE13
	.byte	0x1
	.byte	0x5e
	.uleb128 0x13
	.4byte	.LASF6
	.byte	0x9
	.2byte	0x13e
	.4byte	0x5fc
	.byte	0x2
	.byte	0x7e
	.sleb128 2
	.uleb128 0x10
	.asciz	"input"
	.byte	0x9
	.2byte	0x13e
	.4byte	0x5fc
	.byte	0x2
	.byte	0x7e
	.sleb128 4
	.uleb128 0x11
	.4byte	.LASF8
	.byte	0x9
	.2byte	0x140
	.4byte	0x149
	.byte	0x2
	.byte	0x7e
	.sleb128 0
	.byte	0x0
	.uleb128 0x14
	.4byte	0x66c
	.4byte	0x113
	.uleb128 0x15
	.4byte	0x66c
	.byte	0x40
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF0
	.byte	0x2
	.byte	0x7
	.uleb128 0x16
	.asciz	"sintab"
	.byte	0x9
	.byte	0x1f
	.4byte	0x688
	.byte	0x1
	.byte	0x5
	.byte	0x3
	.4byte	_sintab
	.uleb128 0x17
	.4byte	0x65c
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
	.uleb128 0x9
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
	.uleb128 0xa
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
	.uleb128 0xe
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
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
	.uleb128 0x10
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
	.uleb128 0x11
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
	.uleb128 0x12
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
	.uleb128 0x13
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
	.uleb128 0x14
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x1
	.uleb128 0x13
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
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
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x17
	.uleb128 0x26
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,info
	.4byte	0xd1
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x68e
	.4byte	0x1b8
	.asciz	"sine"
	.4byte	0x206
	.asciz	"arcsine"
	.4byte	0x274
	.asciz	"cosine"
	.4byte	0x2a1
	.asciz	"rotate"
	.4byte	0x32d
	.asciz	"rect_to_polar"
	.4byte	0x3aa
	.asciz	"rect_to_polar16"
	.4byte	0x43b
	.asciz	"sqrt_int"
	.4byte	0x494
	.asciz	"sqrt_long"
	.4byte	0x4f3
	.asciz	"vector2_mag"
	.4byte	0x544
	.asciz	"vector3_mag"
	.4byte	0x5a2
	.asciz	"vector2_normalize"
	.4byte	0x602
	.asciz	"vector3_normalize"
	.4byte	0x673
	.asciz	"sintab"
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
	.asciz	"angle"
.LASF2:
	.asciz	"delta_theta"
.LASF3:
	.asciz	"theta_rot"
.LASF4:
	.asciz	"scaleShift"
.LASF6:
	.asciz	"result"
.LASF7:
	.asciz	"iterations"
.LASF8:
	.asciz	"magnitude"
.LASF5:
	.asciz	"binary_point"
.LASF0:
	.asciz	"unsigned int"

	.section __c30_signature, info, data
	.word 0x0001
	.word 0x0000
	.word 0x0000

	.set ___PA___,0
	.end
