;------------------------------------------------------------------------------
;
; Product:			ds30 loader
;
; File description:	device constants for PIC24F
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
.ifdef	__24F16KA301
	.include	"p24F16KA301.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		29
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F32KA301
	.include	"p24F32KA301.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0X5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		35
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F16KA302
	.include	"p24F16KA302.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		36
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F32KA302
	.include	"p24F32KA302.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0X5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		37
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F16KA304
	.include	"p24F16KA304.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		38
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F32KA304
	.include	"p24F32KA304.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0X5800
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		2048
	.equiv		DEVICEID,		39
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F04KA200
	.include	"p24F04KA200.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0xB00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		512
	.equiv		DEVICEID,		200
	.equiv		EESIZEB,		0
.endif
.ifdef	__24F04KA201
	.include	"p24F04KA201.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x0B00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		512
	.equiv		DEVICEID,		201
	.equiv		EESIZEB,		0
.endif
.ifdef	__24F08KA101
	.include	"p24F08KA101.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x1600
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1536
	.equiv		DEVICEID,		202
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F08KA102
	.include	"p24F08KA102.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x1600
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1536
	.equiv		DEVICEID,		203
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F16KA101
	.include	"p24F16KA101.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1536
	.equiv		DEVICEID,		204
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
.ifdef	__24F16KA102
	.include	"p24F16KA102.inc"
	.equiv		IS_24F,			1
	.equiv		VALID_DEV,		1
	.equiv		FLASHSIZE,		0x2C00
	.equiv		RAM_START,		0x800
	.equiv		RAM_SIZEB,		1536
	.equiv		DEVICEID,		205
	.equiv		EESIZEB,		512
	.equiv		HAS_UART2,		1
.endif
