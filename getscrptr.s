
  
  .globl _cdtc_screen_basic__table_hl
  .globl _getscrptr ;;METER ESTO EN UN .c COMO INLINE ASM (__asm __endasm)

_getscrptr:: 
        ;ld d, #_cdtc_screen_basic__table_hl >> 8 ; MSB table
        ld de, (#_cdtc_screen_basic__table_hl) ; >> 8 ; MSB table
        ld e, h                 ; y
        ld a,(de)               ; read MSB
        ld h,a
        inc d                   ; LSB table
        ld a,(de)               ; read LSB
        srl l
        add l                   ; add x
        jr nc, cdtc_screen_basic__plot_no_carry
        inc h
cdtc_screen_basic__plot_no_carry:
        ld l,a
        ret