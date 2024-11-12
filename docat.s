;; This example shows how to perform a directory of a disc
;; and extract the filenames. 
;;
;; This code is public domain and can freely be used in your
;; own programs.
;;
;; Written by Kevin Thacker. 2002

.globl _docat
.globl _disc_buf

_docat::

;;------------------------------------------------------------------

;; firmware function to catalog a disc or cassette
cas_catalog .equ #0xbc9b
;; firmware function to disable text output
txt_vdu_enable .equ #0xbb54
;; firmware function to enable text output
txt_vdu_disable .equ #0xbb57
;; firmware function to output a character to the screen
txt_output .equ #0xbb5a
;; firmware function to find a RSX 
kl_find_command .equ #0xbcd4

;;------------------------------------------------------------------



;;------------------------------------------------------------------
;; find BIOS SET MESSAGE command
;; this is used to disable disc messages.
;; this is compatible with other DOSs that also provide this command

ld hl,#cmd_bios_set_message
call kl_find_command
ret nc							;; if carry flag is clear, then command has not been found

;; command found

;; store address of command
ld (bios_set_message),hl
ld a,c
;; store "rom select" of command
ld (bios_set_message+2),a

;;------------------------------------------------------------------

;; do CAT
call fetch_directory

ret


;;------------------------------------------------------------------
;; perform a CAT command

fetch_directory:
;; disable disc messages. Error messages will not be displayed.
ld a,#0xff
rst #0x18						;; KL FAR CALL
.dw bios_set_message

;; disable text output
call txt_vdu_disable

ld de,#_disc_buf

;; initialise in case of an error
xor a
ld (de),a

;; do catalog
call cas_catalog

;; enable text output
call txt_vdu_enable

;; enable disc messages. Error messages will be displayed
ld a,#0
rst #0x18						;; KL FAR CALL
.dw bios_set_message
ret


;;------------------------------------------------------------------

;; this is initialised when the "BIOS SET MESSAGE" RSX has been found.
bios_set_message:
.dw 0                    ;; address of function
.db 0                    ;; "rom select" for function


cmd_bios_set_message:
.db 1+0x80				;; this is the "BIOS SET MESSAGE" RSX

;;------------------------------------------------------------------

;; used by CAS CATALOG function to store list of files in directory
;;two_k_buffer .equ #_disc_buf