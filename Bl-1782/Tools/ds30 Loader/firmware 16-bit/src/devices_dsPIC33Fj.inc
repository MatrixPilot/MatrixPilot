;------------------------------------------------------------------------------
;
; Product:			ds30 loader
;
; File description:	device constants for dsPIC33FJ
;
; Copyright: 		Copyright © 09-11, Mikael Gustafsson
;                                          
;------------------------------------------------------------------------------

;-----------------------------------------------------------------------------
;    This file is part of ds30 Loader.
;
;    ds30 Loader is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation.
;
;    ds30 Loader is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with ds30 Loader. If not, see <http:;www.gnu.org/licenses/>.
;------------------------------------------------------------------------------
.ifdef	__33FJ128GP310A
	.include	"p33FJ128GP310A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		76
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ128GP706A
	.include	"p33FJ128GP706A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		77
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128GP708A
	.include	"p33FJ128GP708A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		78
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128GP710A
	.include	"p33FJ128GP710A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		79
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128MC506A
	.include	"p33FJ128MC506A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		80
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ128MC510A
	.include	"p33FJ128MC510A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		81
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ128MC706A
	.include	"p33FJ128MC706A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		82
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ128MC708A
	.include	"p33FJ128MC708A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		83
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128MC710A
	.include	"p33FJ128MC710A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		84
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ256GP506A
	.include	"p33FJ256GP506A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		85
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ256GP510A
	.include	"p33FJ256GP510A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		86
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ256GP710A
	.include	"p33FJ256GP710A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		30720
	.equiv		DEVICEID,		87
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ256MC510A
	.include	"p33FJ256MC510A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		88
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ256MC710A
	.include	"p33FJ256MC710A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		30720
	.equiv		DEVICEID,		89
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64GP206A
	.include	"p33FJ64GP206A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		90
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GP306A
	.include	"p33FJ64GP306A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		91
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GP310A
	.include	"p33FJ64GP310A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		92
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GP708A
	.include	"p33FJ64GP708A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		93
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64GP710A
	.include	"p33FJ64GP710A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		94
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64MC506A
	.include	"p33FJ64MC506A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		95
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC508A
	.include	"p33FJ64MC508A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		96
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC706A
	.include	"p33FJ64MC706A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		97
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC710A
	.include	"p33FJ64MC710A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		98
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ06GS101
	.include	"p33FJ06GS101.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x1000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		256
	.equiv		DEVICEID,		100
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ06GS102
	.include	"p33FJ06GS102.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x1000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		256
	.equiv		DEVICEID,		101
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ06GS202
	.include	"p33FJ06GS202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x1000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1028
	.equiv		DEVICEID,		102
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ12GP201
	.include	"p33FJ12GP201.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		110
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ12GP202
	.include	"p33FJ12GP202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		111
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ12MC201
	.include	"p33FJ12MC201.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		112
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ12MC202
	.include	"p33FJ12MC202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		113
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ16GP304
	.include	"p33FJ16GP304.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		120
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ16GS402
	.include	"p33FJ16GS402.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		121
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ16GS404
	.include	"p33FJ16GS404.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		122
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ16GS502
	.include	"p33FJ16GS502.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		123
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ16GS504
	.include	"p33FJ16GS504.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		124
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ16MC304
	.include	"p33FJ16MC304.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		125
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32GP202
	.include	"p33FJ32GP202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		130
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32GP204
	.include	"p33FJ32GP204.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		131
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32GP302
	.include	"p33FJ32GP302.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		132
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32GP304
	.include	"p33FJ32GP304.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		133
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32MC202
	.include	"p33FJ32MC202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		134
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32MC204
	.include	"p33FJ32MC204.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		135
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32MC302
	.include	"p33FJ32MC302.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		136
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32MC304
	.include	"p33FJ32MC304.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		137
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64GP202
	.include	"p33FJ64GP202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		140
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64GP204
	.include	"p33FJ64GP204.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		141
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64GP206
	.include	"p33FJ64GP206.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		142
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GP306
	.include	"p33FJ64GP306.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		143
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GP310
	.include	"p33FJ64GP310.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		144
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GP706
	.include	"p33FJ64GP706.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		145
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64GP706A
	.include	"p33FJ64GP706A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		146
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64GP708
	.include	"p33FJ64GP708.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		147
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64GP710
	.include	"p33FJ64GP710.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		148
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64GP802
	.include	"p33FJ64GP802.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		149
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64GP804
	.include	"p33FJ64GP804.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		150
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64MC202
	.include	"p33FJ64MC202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		151
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64MC204
	.include	"p33FJ64MC204.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		152
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64MC506
	.include	"p33FJ64MC506.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		153
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC508
	.include	"p33FJ64MC508.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		154
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC510
	.include	"p33FJ64MC510.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		155
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC510A
	.include	"p33FJ64MC510A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		156
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC706
	.include	"p33FJ64MC706.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		157
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64MC710
	.include	"p33FJ64MC710.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		158
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ64MC802
	.include	"p33FJ64MC802.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		159
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ64MC804
	.include	"p33FJ64MC804.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		160
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ32GS406
	.include	"p33FJ32GS406.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		161
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ32GS606
	.include	"p33FJ32GS606.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		162
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ32GS608
	.include	"p33FJ32GS608.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		163
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ32GS610
	.include	"p33FJ32GS610.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		164
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GS406
	.include	"p33FJ64GS406.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		165
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ64GS606
	.include	"p33FJ64GS606.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		9216
	.equiv		DEVICEID,		166
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64GS608
	.include	"p33FJ64GS608.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		9216
	.equiv		DEVICEID,		167
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ64GS610
	.include	"p33FJ64GS610.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		9216
	.equiv		DEVICEID,		168
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ128GP202
	.include	"p33FJ128GP202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		170
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ128GP204
	.include	"p33FJ128GP204.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		171
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ128GP206
	.include	"p33FJ128GP206.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		172
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ128GP206A
	.include	"p33FJ128GP206A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		173
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ128GP306
	.include	"p33FJ128GP306.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		174
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ128GP306A
	.include	"p33FJ128GP306A.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		175
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ128GP310
	.include	"p33FJ128GP310.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		176
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__33FJ128GP706
	.include	"p33FJ128GP706.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		177
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128GP708
	.include	"p33FJ128GP708.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		178
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128GP710
	.include	"p33FJ128GP710.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		179
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128GP802
	.include	"p33FJ128GP802.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		180
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ128GP804
	.include	"p33FJ128GP804.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		181
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ128MC202
	.include	"p33FJ128MC202.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		182
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ128MC204
	.include	"p33FJ128MC204.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		183
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ128MC506
	.include	"p33FJ128MC506.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		184
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ128MC510
	.include	"p33FJ128MC510.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		185
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ128MC706
	.include	"p33FJ128MC706.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		186
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ128MC708
	.include	"p33FJ128MC708.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		187
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128MC710
	.include	"p33FJ128MC710.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		188
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ128MC802
	.include	"p33FJ128MC802.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		189
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ128MC804
	.include	"p33FJ128MC804.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		190
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__33FJ256GP506
	.include	"p33FJ256GP506.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		195
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ256GP510
	.include	"p33FJ256GP510.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		196
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ256GP710
	.include	"p33FJ256GP710.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		30720
	.equiv		DEVICEID,		197
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ256MC510
	.include	"p33FJ256MC510.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		198
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__33FJ256MC710
	.include	"p33FJ256MC710.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		30720
	.equiv		DEVICEID,		199
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__33FJ16GP101
	.include	"p33FJ16GP101.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		445
	.equiv		EESIZEB,		0
.endif
.ifdef	__33FJ16GP102
	.include	"p33FJ16GP102.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		446
	.equiv		EESIZEB,		0
.endif
.ifdef	__33FJ16MC101
	.include	"p33FJ16MC101.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		447
	.equiv		EESIZEB,		0
.endif
.ifdef	__33FJ16MC102
	.include	"p33FJ16MC102.inc"
	.equiv		IS_33FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		448
	.equiv		EESIZEB,		0
.endif
