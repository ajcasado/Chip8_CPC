;; clear screen

.globl _cls 
.globl _cdtc_screen_basic__table_hl 
;;Pensarse seriamente usar como destino un backbuffer y usar doble buffer

_cls:: 
    xor a ;
    ld h,#128;; Estamos en mode 0 monocromo -> acelerar esto *15 usando el truco de Vector Vault https://www.cpcwiki.eu/forum/programming/best-way-to-update-screen-vector-graphics/
loop:
    dec h
    push hl
    ld l,h
    ;ld h, #_cdtc_screen_basic__table_hl >> 8 ; MSB table
    ld bc, (#_cdtc_screen_basic__table_hl); >> 8 ; MSB table
    ld h,b
    ld d,(hl)               ; read MSB
    inc h                   ; LSB table
    ld e,(hl)               ; read LSB
    ex de,hl
    ld de,#0
    ld bc,#64
    ld   (#_stack1+1), sp 
    add  hl, bc                   
    di   ; esto mete lag y/o jiter a la interrupción?
    ld   sp, hl       
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de            
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de           
    push de
_stack1:         
    ld sp,#0               
    ei
    pop hl
    cp h
    ret z
    jr loop