;;from: https://cpctech.cpcwiki.de/source/sampplay.html

.globl _write_ay


_write_ay::
;; Write data to an AY register
;; H = register
;; L = data
write_ay_reg:
;; write register index
ld b,#0xf4
out (c),h
;; "write register index"
ld bc,#0xf6c0
out (c),c
;; "inactive"
ld c,#0x00
out (c),c
;; write register data
ld b,#0xf4
out (c),l
;; "write data"
ld bc,#0xf680
out (c),c
;; "inactive"
ld c,#0x00
out (c),c
ret