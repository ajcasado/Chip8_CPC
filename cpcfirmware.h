#ifndef _cpcfirmware_H_
#define _cpcfirmware_H_

/** Small wrappers (implemented in assembly) to firmware routines.

    The thin assembly wrapping is to pass arguments and return value
    between compiler convention and actual routine practices. */

extern uint16_t km_test_key (uint8_t);
#endif /* _cpcfirmware_H_ */
