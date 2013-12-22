;------------------------------------------------------------------------------
;
; Product:			ds30 loader
;
; File description:	device constants for dsPIC30F
;
; Copyright: 		Copyright 2009-2011 Mikael Gustafsson
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
.ifdef	__30F3014
	.include	"p30F3014.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x4000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		1
	.equiv		EESIZEB,		1024
	.equiv		HAS_UART2,		1
.endif
.ifdef	__30F4013
	.include	"p30F4013.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x8000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		2
	.equiv		EESIZEB,		1024
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__30F5011
	.include	"p30F5011.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xB000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		3
	.equiv		EESIZEB,		1024
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6011
	.include	"p30F6011.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x16000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		6144
	.equiv		DEVICEID,		4
	.equiv		EESIZEB,		2048
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6012
	.include	"p30F6012.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x18000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		5
	.equiv		EESIZEB,		4096
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F5013
	.include	"p30F5013.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xB000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		6
	.equiv		EESIZEB,		1024
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6013
	.include	"p30F6013.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x16000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		6144
	.equiv		DEVICEID,		7
	.equiv		EESIZEB,		2048
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6014
	.include	"p30F6014.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x18000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		8
	.equiv		EESIZEB,		4096
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F2010
	.include	"p30F2010.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		512
	.equiv		DEVICEID,		10
	.equiv		EESIZEB,		1024
.endif
.ifdef	__30F3010
	.include	"p30F3010.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x4000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		11
	.equiv		EESIZEB,		1024
.endif
.ifdef	__30F4012
	.include	"p30F4012.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x8000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		12
	.equiv		EESIZEB,		1024
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__30F3011
	.include	"p30F3011.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x4000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		13
	.equiv		EESIZEB,		1024
	.equiv		HAS_UART2,		1
.endif
.ifdef	__30F4011
	.include	"p30F4011.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x8000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		14
	.equiv		EESIZEB,		1024
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__30F5015
	.include	"p30F5015.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xB000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		15
	.equiv		EESIZEB,		1024
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__30F6010
	.include	"p30F6010.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x18000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		16
	.equiv		EESIZEB,		4096
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F2011
	.include	"p30F2011.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		20
	.equiv		EESIZEB,		0
.endif
.ifdef	__30F3012
	.include	"p30F3012.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x4000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		21
	.equiv		EESIZEB,		1024
.endif
.ifdef	__30F2012
	.include	"p30F2012.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		22
	.equiv		EESIZEB,		0
.endif
.ifdef	__30F3013
	.include	"p30F3013.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x4000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		23
	.equiv		EESIZEB,		1024
	.equiv		HAS_UART2,		1
.endif
.ifdef	__30F6010A
	.include	"p30F6010A.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x18000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		24
	.equiv		EESIZEB,		4096
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6011A
	.include	"p30F6011A.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x16000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		6144
	.equiv		DEVICEID,		25
	.equiv		EESIZEB,		2048
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6012A
	.include	"p30F6012A.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x18000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		26
	.equiv		EESIZEB,		4096
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6013A
	.include	"p30F6013A.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x16000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		6144
	.equiv		DEVICEID,		27
	.equiv		EESIZEB,		2048
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F6014A
	.include	"p30F6014A.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x18000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		28
	.equiv		EESIZEB,		4096
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
	.equiv		HAS_CAN2,		1
.endif
.ifdef	__30F1010
	.include	"p30F1010.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x1000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		256
	.equiv		DEVICEID,		30
	.equiv		EESIZEB,		0
.endif
.ifdef	__30F2020
	.include	"p30F2020.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		512
	.equiv		DEVICEID,		31
	.equiv		EESIZEB,		0
.endif
.ifdef	__30F2023
	.include	"p30F2023.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		512
	.equiv		DEVICEID,		32
	.equiv		EESIZEB,		0
.endif
.ifdef	__30F5016
	.include	"p30F5016.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xB000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		33
	.equiv		EESIZEB,		1024
	.equiv		HAS_CAN1,		1
.endif
.ifdef	__30F6015
	.include	"p30F6015.inc"
	.equiv		IS_30F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x18000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		34
	.equiv		EESIZEB,		4096
	.equiv		HAS_UART2,		1
	.equiv		HAS_CAN1,		1
.endif
