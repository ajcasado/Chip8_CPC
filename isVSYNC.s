.globl _isVSYNC
_isVSYNC::
   ld l,#0
   ld  b, #0xf5;; [2] B = F5h ==> B has the address of PPI Port B, where we get information from VSYNC
   in  a,(c)         ;; [4] A = Status register got from PPI port B
   rra               ;; [1] Move bit 0 of A to Carry (bit 0 contains VSYNC status)
   jr  nc, bye  ;; [2/3] No Carry means No VSYNC
   ld l,#1
bye:
   ret               ;;l has VSYNC status