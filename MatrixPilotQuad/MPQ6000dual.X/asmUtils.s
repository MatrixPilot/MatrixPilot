.include "p33Fxxxx.inc"

.text
.global _getNewIPL
; C passes 2 input arg's in registers w0 and w1
; single result arg is expected in w0

_getNewIPL:
    mov.w   [w15], w0   ; SR is high byte of TOS
    lsr     w0, #13, w0 ; shift IPL[2:0] from [15:13] to [2:0]
    return
.end
