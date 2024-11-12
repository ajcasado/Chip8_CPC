	;; Call KM_TEST_KEY firmware routine
	;; and put return value where C compiler expects it.

	.globl _km_test_key
	

;	---------------------------------
; Function km_test_key
; ---------------------------------

_km_test_key:
	ld 	a,l
        call    0xBB1E  ; KM TEST KEY
	ld	l,c
        ld      h,#0
	ret	z
        dec 	h
        ret