
/*
    CHIP-8 and Super-CHIP interpreter library
    https://github.com/AlfonsoJLuna/chip8swemu
*/


#ifndef CHIP8_H
#define CHIP8_H


//#include <stduint8_t.h>
#include <stdint.h>
#include <string.h>

//#include <cpctelera.h>
/*
#define CPC6128
#ifdef CPC464
    #define RANDOM_SEED_ADDRESS 0xB187
#endif
#ifdef CPC6128
    #define RANDOM_SEED_ADDRESS 0xB8B4
#endif
#define VECTOR_ZERO_16 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define VECTOR_ZERO_8 {0,0,0,0,0,0,0,0}

/**
 * Resets all registers and buffers.
 */
void chip8ResetCpu();


/**
 * Get the status of a pixel of the screen buffer.
 *
 * @param row The row to be accessed.
 * @param col The column to be accessed.
 *
 * @return 0 if the pixel is clear, 1 if the pixel is set.
 */
//static inline uint8_t chip8GetPixel(uint8_t row, uint8_t col);


/**
 * Steps the cpu.
 *
 * @param cycles The number of instructions to execute.
 *
 * @return 0 if success, 1 if an unsupported opcode has been processed.
 */
uint8_t chip8StepCpu(uint8_t cycles);


/**
 * Decreases the timers.
 * This function needs to be called 60 times per second.
 */
/*void chip8UpdateTimers(uint8_t cyxles_60);


/**
 * Updates a key status.
 * This function needs to be called when a key has been pressed or released.
 *
 * @param key The key pressed or released (from 0x0 to 0xF).
 * @param status 1 if the key has been pressed, 0 if the key has been released.
 */
/*void chip8UpdateKey(uint8_t key, uint8_t status);


/**
 * Get the buzzer status.
 *
 * @return 0 if the buzzer should be silent, 1 if should beep.
 */
/*uint8_t chip8GetAudio();


/**
 * Get the screen buffer.
 *
 * @return A pointer to the 128x64 pixels = 1024 bytes screen buffer.
*/
/*uint8_t* chip8GetScreen();


/**
 * Change 8XY6, 8XYE, FX55, FX65 behavior between
 * CHIP-8 (compatibility mode enabled) or Super-CHIP (disabled).
 *
 * @param Enable or disable the compatibility mode.
 */
void chip8CompatibilityMode(uint8_t enabled);


/**
 * Configure Vertical Wrap
 *
 * @param Enable or disable the vertical wrap.
 */
void chip8VerticalWrap(uint8_t enabled);

//static uint8_t global_delay_timer;
//static uint8_t global_sound_timer;
typedef struct
{
    uint16_t PC;            // Program counter
    uint16_t I;             // Index register
    uint8_t  V[16];         // General purpose registers
    uint8_t  R[8];          // User editable registers
    uint8_t  SP;            // Stack pointer
    uint16_t stack[16];
    uint8_t  extended;      // Extended screen (128x64 mode)
    uint16_t opcode;
    uint8_t  delay_timer;
    uint8_t  sound_timer;
    uint8_t compatibility_mode;
    uint8_t vertical_wrap;
    uint8_t  keyboard[16];
//    uint8_t  screen[1024];  // Screen buffer needs 128x64 pixels = 8192 bits = 1024 bytes
} cpu_t;
extern cpu_t cpu;

#define FIRST_SCREEN_BUFFER 0xC000
#define SECOND_SCREEN_BUFFER 0X4000
#define FIRST_SCREEN_BTABLE 0X9E00
#define SECOND_SCREEN_BTABLE 0X9900

	



#endif  // CHIP8_H