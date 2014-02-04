
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	.section .libdsp, code



	.global	_FindFirstBitChangeFromLeft	; export
_FindFirstBitChangeFromLeft:

	FBCL	W0 , W0
	return

	.global	_FindFirstBitFromLeft	; export
_FindFirstBitFromLeft:

	FF1L	W0 , W0
	return

