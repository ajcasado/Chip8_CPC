;.globl _cdtc_screen_basic__table_hl
;_cdtc_screen_basic__table_hl:
;    .dw 0x5000 ;z80 endian
; put table just after screen, because why not
;_screen_first_byte_address == 0xC000
;_crtc_r1_disp_width_chars == 32    ; 32 chars, 64 bytes, 256-pixel wide screen (standard is 40)
;_crtc_r6_disp_height_chars == 22   ; 32 chars but of height 6, 192-pixel high screen
;_crtc_r9_plus_1_disp_lines_per_char_height == 8 ; unusual value 6 instead of standard value 8, r9 == 5
; Don't change lines below
;_screen_width_in_bytes == _crtc_r1_disp_width_chars * 2
;_screen_total_lines == _crtc_r6_disp_height_chars * _crtc_r9_plus_1_disp_lines_per_char_height
;R1=32, R2=42, R4=50, R6=32, R7=40, R9=5
;defecto: R1 = 40 ; R2 = 46 ; R6 = 25 ; R7 = 30
;     C8(0): R1 = 32 ; R2 = 42 ; R6 = 22 ; R7 = 29
;     C8(1): R1 = 46 ; R2 = 49 ; R6 = 22 ; R7 = 29