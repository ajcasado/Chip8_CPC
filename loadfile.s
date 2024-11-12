.globl _loadfile
.globl _disc_buf
.globl _file_name
.globl _file_name_length
.globl _memory

_loadfile::

;; This example shows how to read a file byte by byte.
;;
;; A file without a header must be read this way, it can't be
;; read using CAS IN DIRECT (unless the in-memory header is patched)
;;
;; This example doesn't have any error checking.

cas_in_open .equ #0xbc77
cas_in_close .equ #0xbc7a
cas_in_char .equ #0xbc80

;ld hl,#_disc_buf					;; address to load file data to (example)
;push hl

;; open file for reading
ld hl,#_file_name_length
ld b,(hl)
ld hl,#_file_name
ld de,#_disc_buf
call cas_in_open
push hl ;; cualquier camino a eof debe dejar la pila igual
jr nc,eof
pop hl
;; If a file is opened without a header:
;; - the filetype will be ASCII (&16)
;; - the length and load address will be undefined.
;;
;; If a file is opened with a header, the 
;; - the filetype will be taken from the header
;; - the length and load address will be taken from the header
;;
;; A file without a header can't be read with CAS IN DIRECT
;; and must be read using CAS IN CHAR.

;pop hl
;ld hl,#_disc_buf

;; read a char from the file, character is returned in A register
ld hl,#_memory+#0x200
ld de,#_memory+#0x1000
next_byte:
call cas_in_char
;jr nc,eof
;jr nz,not_eof

;; could be end of file
;; test for hard end of file byte

;not_eof:
;; write byte to memory
ld (hl),a
inc hl
push hl
ccf
sbc hl,de
jr z,eof
pop hl
jr next_byte
eof:
pop hl
call cas_in_close
ret

;;-------------------------------------------------------------------
;; name of the file to read

;.filename
;defb "datafile.bin"
;.end_filename

;;-------------------------------------------------------------------
;; this buffer is filled with data from the file
;.two_k_buffer defs 2048

