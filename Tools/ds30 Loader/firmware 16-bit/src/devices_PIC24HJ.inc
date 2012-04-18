;------------------------------------------------------------------------------
;
; Product:			ds30 loader
;
; File description:	device constants for PIC24H
;
; Copyright: 		Copyright 2009-2012 Mikael Gustafsson
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

.ifdef	__24HJ12GP201
	.include	"p24HJ12GP201.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		40
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ12GP202
	.include	"p24HJ12GP202.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		41
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ16GP304
	.include	"p24HJ16GP304.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		42
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ32GP202
	.include	"p24HJ32GP202.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		43
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ32GP204
	.include	"p24HJ32GP204.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		44
	.equiv		EESIZEB,		0
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ32GP302
	.include	"p24HJ32GP302.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		45
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ32GP304
	.include	"p24HJ32GP304.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		46
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ64GP202
	.include	"p24HJ64GP202.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		47
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ64GP204
	.include	"p24HJ64GP204.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		48
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ64GP206
	.include	"p24HJ64GP206.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		49
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ64GP210
	.include	"p24HJ64GP210.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		50
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ64GP210A
	.include	"p24HJ64GP210A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		51
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ64GP502
	.include	"p24HJ64GP502.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		52
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ64GP504
	.include	"p24HJ64GP504.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		53
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ64GP506
	.include	"p24HJ64GP506.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		54
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ64GP506A
	.include	"p24HJ64GP506A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		55
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ64GP510
	.include	"p24HJ64GP510.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		56
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ64GP510A
	.include	"p24HJ64GP510A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		57
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP202
	.include	"p24HJ128GP202.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		58
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ128GP204
	.include	"p24HJ128GP204.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		59
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ128GP206
	.include	"p24HJ128GP206.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		60
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP210
	.include	"p24HJ128GP210.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		61
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP210A
	.include	"p24HJ128GP210A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		62
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP306
	.include	"p24HJ128GP306.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		63
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP310
	.include	"p24HJ128GP310.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		64
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP310A
	.include	"p24HJ128GP310A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		65
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP502
	.include	"p24HJ128GP502.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		66
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ128GP504
	.include	"p24HJ128GP504.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		67
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24HJ128GP506
	.include	"p24HJ128GP506.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		68
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ128GP506A
	.include	"p24HJ128GP506A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		69
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ128GP510
	.include	"p24HJ128GP510.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		70
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ128GP510A
	.include	"p24HJ128GP510A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		71
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ256GP206
	.include	"p24HJ256GP206.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		72
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ256GP210
	.include	"p24HJ256GP210.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		73
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ256GP610
	.include	"p24HJ256GP610.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		74
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__24HJ256GP610A
	.include	"p24HJ256GP610A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		451
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ256GP206A
	.include	"p24HJ256GP206A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		452
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ256GP210A
	.include	"p24HJ256GP210A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		453
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP306A
	.include	"p24HJ128GP306A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		454
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ128GP206A
	.include	"p24HJ128GP206A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		455
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24HJ64GP206A
	.include	"p24HJ64GP206A.inc"
	.equiv		IS_24HJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		500
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
