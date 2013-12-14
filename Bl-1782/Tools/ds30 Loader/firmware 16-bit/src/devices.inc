;------------------------------------------------------------------------------
;
; Product:			ds30 Loader
;
; File description:	device constants for PIC24 and dsPIC
;
; Copyright: 		Copyright 2011 MG Digital Solutions
;
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Includes
;------------------------------------------------------------------------------
        .include	"devices_PIC24F.inc"
        .include	"devices_PIC24FJ.inc"
        .include	"devices_PIC24HJ.inc"
        .include	"devices_dsPIC30F.inc"
        .include	"devices_dsPIC33FJ.inc"
        
        
;------------------------------------------------------------------------------
; Family specific defines
;------------------------------------------------------------------------------		
		.ifdef IS_24F
			.equiv 	FL_ER_CODE,		0x4058
			.equiv 	FL_WR_CODE,		0x4004
			.equiv 	EE_ER_CODE,		0x4058
			.equiv 	EE_WR_CODE,		0x4004

			.equiv	PAGESIZER,		1				/*pagesize [rows]*/
			.equiv	ROWSIZEW,		32				/*rowsize [words]*/	
		.endif
		
		.ifdef IS_24FJ
			.equiv 	FL_ER_CODE,		0x4042
			.equiv 	FL_WR_CODE,		0x4001
			.equiv 	HAS_PAGES,		1

			.equiv	PAGESIZER,		8				/*pagesize [rows]*/
			.equiv	ROWSIZEW,		64				/*rowsize [words]*/	
		.endif
		
		.ifdef IS_24HJ
			.equiv 	FL_ER_CODE,		0x4042
			.equiv 	FL_WR_CODE,		0x4001
			.equiv 	CFG_ER_CODE,	0x4040
			.equiv 	CFG_WR_CODE,	0x4000
			.equiv	CFG_TYPE2,		1
			.equiv 	HAS_PAGES,		1

			.equiv	PAGESIZER,		8				/*pagesize [rows]*/
			.equiv	ROWSIZEW,		64				/*rowsize [words]*/	
		.endif	
		
		.ifdef IS_30F
			.equiv 	FL_ER_CODE,		0x4041
			.equiv 	FL_WR_CODE,		0x4001
			.equiv 	EE_ER_CODE,		0x4044
			.equiv 	EE_WR_CODE,		0x4004
			.equiv 	CFG_WR_CODE,	0x4008
			.equiv 	CFG_TYPE1,		1

			.equiv	PAGESIZER,		1				/*pagesize [rows]*/
			.equiv	ROWSIZEW,		32				/*rowsize [words]*/	
		.endif	
		
		.ifdef IS_33FJ
			.equiv 	FL_ER_CODE,		0x4042
			.equiv 	FL_WR_CODE,		0x4001
			.equiv 	CFG_ER_CODE,	0x4040
			.equiv 	CFG_WR_CODE,	0x4000
			.equiv 	CFG_TYPE2,		1
			.equiv 	HAS_PAGES,		1

			.equiv	PAGESIZER,		8				/*pagesize [rows]*/
			.equiv	ROWSIZEW,		64				/*rowsize [words]*/	
		.endif	
		
		.ifndef VALID_DEV
			.error "Unknown device specified"
		.endif
		
		        
;------------------------------------------------------------------------------
; End of file
;------------------------------------------------------------------------------		
