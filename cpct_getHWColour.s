;;-----------------------------LICENSE NOTICE------------------------------------
;;  This file is part of CPCtelera: An Amstrad CPC Game Engine 
;;  Copyright (C) 2014-2015 ronaldo / Fremos / Cheesetea / ByteRealms (@FranGallegoBR)
;;
;;  This program is free software: you can redistribute it and/or modify
;;  it under the terms of the GNU Lesser General Public License as published by
;;  the Free Software Foundation, either version 3 of the License, or
;;  (at your option) any later version.
;;
;;  This program is distributed in the hope that it will be useful,
;;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;  GNU Lesser General Public License for more details.
;;
;;  You should have received a copy of the GNU Lesser General Public License
;;  along with this program.  If not, see <http://www.gnu.org/licenses/>.
;;-------------------------------------------------------------------------------
;#####################################################################
;### MODULE: SetVideoMode                                          ###
;#####################################################################
;### Routines to establish and control video modes                 ###
;#####################################################################
;
;.module cpct_video
  
;.include /videomode.s/

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Function: cpct_getHWColour
;;
;;    Converts a firmware colour value into its equivalent hardware one.
;;
;; C Definition:
;;    <u8> <cpct_getHWColour> (<u16> *firmware_colour*)
;;
;; Input Parameters (2 Bytes):
;;    (2B HL) firmware_colour - [0-26] Firmware colour value to be converted (Similar to BASIC's INK value)
;;
;; Assembly call (Input parameters on registers):
;;    > call cpct_getHWColour_asm
;;
;; Parameter Restrictions:
;;    * *firmware_colour* must be in the range [0-26], otherwise, return value will be undefined.
;;
;; Return Value:
;;    <u8> - [0-31] Hardware colour value corresponding to *firmware_colour* provided.
;;
;; Details:
;;    Uses the *firmware_colour* as index in a conversion table to get its equivalent hardware
;; value. You can find this equivalences in the table 1 of the <cpct_setPalette> explanation.
;;
;; Destroyed Register values:
;;    HL, DE
;;
;; Required memory:
;;      6 bytes
;;   + 27 bytes cpct_firmware2hw_colour conversion table
;;
;; Time Measures:
;; (start code)
;;     Case   | microSecs(us) | CPU Cycles
;; -----------------------------------------
;;     Any    |      11       |     44
;; -----------------------------------------
;; (end code)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;.globl cpct_firmware2hw_colour ;; colour conversion table defined in cpct_getHWColour.s

.globl _cpct_getHWColour

_cpct_getHWColour::
;cpct_getHWColour_asm::       ;; Assembly entry point
   ld   de, #_cpct_firmware2hw_colour ;; [3] DE points to the start of the colour table
   add  hl, de                       ;; [3] HL += DE, HL points to the exact hardware color value to return

   ld    l, (hl)            ;; [2] L = Return value (hardware colour for firmware colour supplied)
   ret                      ;; [3] Return

_cpct_firmware2hw_colour:
  .db 0x14, 0x04, 0x15, 0x1C, 0x18, 0x1D, 0x0C, 0x05, 0x0D
  .db 0x16, 0x06, 0x17, 0x1E, 0x00, 0x1F, 0x0E, 0x07, 0x0F
  .db 0x12, 0x02, 0x13, 0x1A, 0x19, 0x1B, 0x0A, 0x03, 0x0B