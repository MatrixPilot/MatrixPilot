;------------------------------------------------------------------------------
;
; Product:			ds30 loader
;
; File description:	device constants for PIC24FJ
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
.ifdef	__24FJ128GB206
	.include	"p24FJ128GB206.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		9
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GB210
	.include	"p24FJ128GB210.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		17
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GB206
	.include	"p24FJ256GB206.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		18
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GB210
	.include	"p24FJ256GB210.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		19
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256DA206
	.include	"p24FJ256DA206.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		191
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256DA210
	.include	"p24FJ256DA210.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		192
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ16GA002
	.include	"p24FJ16GA002.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		206
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ16GA004
	.include	"p24FJ16GA004.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		4096
	.equiv		DEVICEID,		207
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ32GA002
	.include	"p24FJ32GA002.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		208
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ32GA004
	.include	"p24FJ32GA004.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		209
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ48GA002
	.include	"p24FJ48GA002.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x8400
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		210
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ48GA004
	.include	"p24FJ48GA004.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x8400
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		211
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA002
	.include	"p24FJ64GA002.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		212
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA004
	.include	"p24FJ64GA004.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		213
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA006
	.include	"p24FJ64GA006.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		214
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ64GA008
	.include	"p24FJ64GA008.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		215
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ64GA010
	.include	"p24FJ64GA010.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		216
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ64GB106
	.include	"p24FJ64GB106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		217
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GB108
	.include	"p24FJ64GB108.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		218
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GB110
	.include	"p24FJ64GB110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		219
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ96GA006
	.include	"p24FJ96GA006.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x10000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		220
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ96GA008
	.include	"p24FJ96GA008.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x10000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		221
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ96GA010
	.include	"p24FJ96GA010.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x10000
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		222
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ128GA006
	.include	"p24FJ128GA006.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		223
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ128GA008
	.include	"p24FJ128GA008.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		224
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ128GA010
	.include	"p24FJ128GA010.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		225
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24FJ128GA106
	.include	"p24FJ128GA106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		226
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GA108
	.include	"p24FJ128GA108.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		227
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GA110
	.include	"p24FJ128GA110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		228
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GB106
	.include	"p24FJ128GB106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		229
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GB108
	.include	"p24FJ128GB108.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		230
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GB110
	.include	"p24FJ128GB110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		231
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ192GA106
	.include	"p24FJ192GA106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x20C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		232
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ192GA108
	.include	"p24FJ192GA108.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x20C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		233
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ192GA110
	.include	"p24FJ192GA110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x20C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		234
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ192GB106
	.include	"p24FJ192GB106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x20C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		235
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ192GB108
	.include	"p24FJ192GB108.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x20C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		236
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ192GB110
	.include	"p24FJ192GB110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x20C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		237
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GA106
	.include	"p24FJ256GA106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		238
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GA108
	.include	"p24FJ256GA108.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		239
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GA110
	.include	"p24FJ256GA110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		240
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GB106
	.include	"p24FJ256GB106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		241
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GB108
	.include	"p24FJ256GB108.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		242
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256GB110
	.include	"p24FJ256GB110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		16384
	.equiv		DEVICEID,		243
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ32GB002
	.include	"p24FJ32GB002.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		244
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ32GB004
	.include	"p24FJ32GB004.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		245
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GB002
	.include	"p24FJ64GB002.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		246
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GB004
	.include	"p24FJ64GB004.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xAC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		247
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ32GA102
	.include	"p24FJ32GA102.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		248
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ32GA104
	.include	"p24FJ32GA104.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		249
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA102
	.include	"p24FJ64GA102.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xac00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		633
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA104
	.include	"p24FJ64GA104.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xac00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		634
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128DA106
	.include	"p24FJ128DA106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		24576
	.equiv		DEVICEID,		250
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128DA110
	.include	"p24FJ128DA110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		24576
	.equiv		DEVICEID,		251
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128DA206
	.include	"p24FJ128DA206.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		252
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128DA210
	.include	"p24FJ128DA210.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		98304
	.equiv		DEVICEID,		253
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256DA106
	.include	"p24FJ256DA106.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		24576
	.equiv		DEVICEID,		254
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ256DA110
	.include	"p24FJ256DA110.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2AC00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		24576
	.equiv		DEVICEID,		255
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ16MC101
	.include	"p24FJ16MC101.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		449
	.equiv		EESIZEB,		0
.endif
.ifdef	__24FJ16MC102
	.include	"p24FJ16MC102.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1024
	.equiv		DEVICEID,		450
	.equiv		EESIZEB,		0
.endif
.ifdef	__24FJ128GA306
	.include	"p24FJ128GA306.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		615
	.equiv		EESIZEB,		0
	.equiv		CONFIGCNT,		4
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GA308
	.include	"p24FJ128GA308.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		616
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ128GA310
	.include	"p24FJ128GA310.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x15800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		617
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA306
	.include	"p24FJ64GA306.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xac00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		618
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA308
	.include	"p24FJ64GA308.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xac00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		619
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
.ifdef	__24FJ64GA310
	.include	"p24FJ64GA310.inc"
	.equiv		IS_24FJ,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xac00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		8192
	.equiv		DEVICEID,		620
	.equiv		EESIZEB,		0
	.equiv		HAS_UART2,		1
	.equiv		HAS_UART3,		1
	.equiv		HAS_UART4,		1
	.equiv		HAS_PPS, 		1
.endif
