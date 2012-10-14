.include "p33Fxxxx.inc"

.text
.global _addSat
; C passes 2 input arg's in registers w0 and w1
; single result arg is expected in w0

_addSat:
    lac w0,#0,A ; store contents of w0, unshifted, into accA
    add w1,#0,A ; add contents of w1 to accA
    sac A,#0,w0 ; store accA into w0
    return
.end



