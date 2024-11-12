;;FROM http://www.cpcmania.com/Docs/Programming/Ficheros.htm
;;  //Based on http://cpctech.cpc-live.com/source/loader.html

        .globl _setupdos

_setupdos::

    ld l, 2 (ix) ;stack return address
    ld h, 3 (ix) ;stack return address
    ld (_stack+1), hl

    ;;------------------------------------------------------------------------
    ;; store the drive number the loader was run from //hope that this is not corrupted
    ld hl,(#0xbe7d)
    ld a,(hl)                  
    ld (_drive+1),a

    ;;------------------------------------------------------------------------
    ld c,#0xff          ;; disable all roms
    ld hl, #_start222         ;; execution address for program
    call #0xbd16  ;;mc_start_program    ;; start it

    _start222:: nop

    call #0xbccb  ;;kl_rom_walk     ;; enable all roms 

    ;;------------------------------------------------------------------------
    ;; when AMSDOS is enabled, the drive reverts back to drive 0!
    ;; This will restore the drive number to the drive the loader was run from
    _drive: ld a, #0x00
    ld hl,(#0xbe7d)
    ld (hl),a     

    _stack: ld hl, #0x0000
    push hl ;//one for sdcc ix pop
    push hl ;//two for return address
    ret
