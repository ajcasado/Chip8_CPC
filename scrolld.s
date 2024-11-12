;; scroll down 

.globl _scrolld
.globl _cdtc_screen_basic__table_hl_source
.globl _cdtc_screen_basic__table_hl_dest
.globl _cdtc_screen_basic__table_hl
.globl _screen_offset

;;Pensarse seriamente usar como destino un backbuffer y usar doble buffer. A ello voy.
_scrolld:: 

    ld a,l
    or a
    ret z   ;;if OPCODE_N =0 return
    ld a,l
    ;add a,a     ;;bY = OPCODE_N * 2
    ld h,#128                ;; i = 128
    ld c,#0
    ;ld iy,#_cdtc_screen_basic__table_hl_dest
    ;ld iy,#_cdtc_screen_basic__table_hl_source
    ;ld de,1(iy)
loop:
    dec h
    cp h
    jp z,loop1
    ld l,h 
    ld iy,#_cdtc_screen_basic__table_hl_dest
    ld h,1(iy)
    ;ld h, #_cdtc_screen_basic__table_hl >> 8 ; MSB table
    ld d,(hl)               ; read MSB
    inc h                   ; LSB table
    ld e,(hl)               ; read LSB || DE tiene comienzo linea destino
    ld h,l
    push hl
    ld b,a
    sbc hl,bc
    ;;ld (#_sadd1+1),hl
    ld l,h
    ld iy,#_cdtc_screen_basic__table_hl_source
    ld h,1(iy)
    ;ld h, #_cdtc_screen_basic__table_hl >> 8 ; MSB table
    push de
    ld d,(hl)               ; read MSB
    inc h                   ; LSB table
    ld e,(hl)               ; read LSB || DE tiene comienzo linea origen
    ex de,hl                ;HL tiene comienzo linea origen
    pop de                  ;DE tiene comienzo linea destino
    push hl
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    pop de
    pop hl
    push hl
    dec h
    ld l,h
    ld iy,#_cdtc_screen_basic__table_hl_dest
    ld h,1(iy)
    ;ld h, #_cdtc_screen_basic__table_hl >> 8 ; MSB table
    push de
    ld d,(hl)               ; read MSB
    inc h                   ; LSB table
    ld e,(hl) ; read LSB || DE tiene comienzo linea destino ¿¿mejor calculando linea destino como |set 3,d|, según https://www.cpcwiki.eu/forum/programming/sprites-tiles-and-other-superfast-screen-output-approaches/msg225567/#msg225567
    pop hl
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    ldi
    pop hl
    dec h
    cp h
    jp nz,loop
loop1:
    dec a
    ;ret z
    ld l,a
    ld iy,#_cdtc_screen_basic__table_hl_dest
    ld h,1(iy)
    ;ld h, #_cdtc_screen_basic__table_hl >> 8 ; MSB table
    ld d,(hl)               ; read MSB
    inc h                   ; LSB table
    ld e,(hl) ; read LSB || DE tiene comienzo linea destino
    ex de,hl
    ld bc,#0 
    ld de,#64
    ld   (#_stack+1), sp 
    add  hl, de                   
    di   ; esto mete lag y/o jiter a la interrupción?
    ld   sp, hl       
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc            
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc
    cp #0
    jp z,out
    dec a
    ld l,a
    ld iy,#_cdtc_screen_basic__table_hl_source ;;¿haría falta borrar la linea de origen?
    ld h,1(iy)
    ;ld h, #_cdtc_screen_basic__table_hl >> 8 ; MSB table
    ld d,(hl)               ; read MSB
    inc h                   ; LSB table
    ld e,(hl)               ; read LSB || DE tiene comienzo linea origen
    ex de,hl                ;HL tiene comienzo linea origen
    ;ld bc,#0
    ld de,#64
    add  hl, de                 
    ld   sp, hl       
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc            
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc           
    push bc
out:
_stack:  
    ld sp,#0 
    ei
    cp #0
    ret z
    jp loop1
