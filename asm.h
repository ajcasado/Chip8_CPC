
#ifndef _asm_H_
#define _asm_H_

/** asm routines */

extern void scrolld (uint8_t nY) __z88dk_fastcall;
extern void write_ay (uint16_t reg_data) __z88dk_fastcall;
extern void cpct_setInterruptHandler ( void (*intHandler)(void) ) __z88dk_fastcall;
extern void cpct_setCRTCReg (uint8_t regnum, uint8_t newval) __z88dk_callee;
extern void docat(void);
extern void loadfile(void);
extern void cls(void);
extern inline uint8_t* getscrptr(uint16_t coords) __z88dk_fastcall __preserves_regs(b, c, iyl, iyh);
extern uint8_t cpct_getHWColour (uint16_t firmware_colour) __z88dk_fastcall __preserves_regs(a, b, c, iyl, iyh);
extern void cpct_waitVSYNC (void);
extern uint8_t isVSYNC(void) __z88dk_fastcall __preserves_regs(d, e, h, iyl, iyh);
#endif /* _asm_H_ */