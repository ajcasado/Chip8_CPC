/*
    CHIP-8 and Super-CHIP interpreter library
    https://github.com/AlfonsoJLuna/chip8swemu
*/


#include "chip8lib.h"
#include <stdlib.h>
#include <stdio.h>
#include "cpcrslib.h"
#include "asm.h"


uint8_t* const videotable[200] = {
    0xC000 , 0xC800 , 0xD000 , 0xD800 , 0xE000 , 0xE800 , 0xF000 , 0xF800 ,
    0xC050 , 0xC850 , 0xD050 , 0xD850 , 0xE050 , 0xE850 , 0xF050 , 0xF850 ,
    0xC0A0 , 0xC8A0 , 0xD0A0 , 0xD8A0 , 0xE0A0 , 0xE8A0 , 0xF0A0 , 0xF8A0 ,
    0xC0F0 , 0xC8F0 , 0xD0F0 , 0xD8F0 , 0xE0F0 , 0xE8F0 , 0xF0F0 , 0xF8F0 ,
    0xC140 , 0xC940 , 0xD140 , 0xD940 , 0xE140 , 0xE940 , 0xF140 , 0xF940 ,
    0xC190 , 0xC990 , 0xD190 , 0xD990 , 0xE190 , 0xE990 , 0xF190 , 0xF990 ,
    0xC1E0 , 0xC9E0 , 0xD1E0 , 0xD9E0 , 0xE1E0 , 0xE9E0 , 0xF1E0 , 0xF9E0 ,
    0xC230 , 0xCA30 , 0xD230 , 0xDA30 , 0xE230 , 0xEA30 , 0xF230 , 0xFA30 ,
    0xC280 , 0xCA80 , 0xD280 , 0xDA80 , 0xE280 , 0xEA80 , 0xF280 , 0xFA80 ,
    0xC2D0 , 0xCAD0 , 0xD2D0 , 0xDAD0 , 0xE2D0 , 0xEAD0 , 0xF2D0 , 0xFAD0 ,
    0xC320 , 0xCB20 , 0xD320 , 0xDB20 , 0xE320 , 0xEB20 , 0xF320 , 0xFB20 ,
    0xC370 , 0xCB70 , 0xD370 , 0xDB70 , 0xE370 , 0xEB70 , 0xF370 , 0xFB70 ,
    0xC3C0 , 0xCBC0 , 0xD3C0 , 0xDBC0 , 0xE3C0 , 0xEBC0 , 0xF3C0 , 0xFBC0 ,
    0xC410 , 0xCC10 , 0xD410 , 0xDC10 , 0xE410 , 0xEC10 , 0xF410 , 0xFC10 ,
    0xC460 , 0xCC60 , 0xD460 , 0xDC60 , 0xE460 , 0xEC60 , 0xF460 , 0xFC60 ,
    0xC4B0 , 0xCCB0 , 0xD4B0 , 0xDCB0 , 0xE4B0 , 0xECB0 , 0xF4B0 , 0xFCB0 ,
    0xC500 , 0xCD00 , 0xD500 , 0xDD00 , 0xE500 , 0xED00 , 0xF500 , 0xFD00 ,
    0xC550 , 0xCD50 , 0xD550 , 0xDD50 , 0xE550 , 0xED50 , 0xF550 , 0xFD50 ,
    0xC5A0 , 0xCDA0 , 0xD5A0 , 0xDDA0 , 0xE5A0 , 0xEDA0 , 0xF5A0 , 0xFDA0 ,
    0xC5F0 , 0xCDF0 , 0xD5F0 , 0xDDF0 , 0xE5F0 , 0xED50 , 0xF550 , 0xFD50 ,
    0xC640 , 0xCE40 , 0xD640 , 0xDE40 , 0xE640 , 0xEE40 , 0xF640 , 0xFE40 ,
    0xC690 , 0xCE90 , 0xD690 , 0xDE90 , 0xE690 , 0xEE90 , 0xF690 , 0xFE90 ,
    0xC6E0 , 0xCEE0 , 0xD6E0 , 0xDEE0 , 0xE6E0 , 0xEEE0 , 0xF6E0 , 0xFEE0 ,
    0xC730 , 0xCF30 , 0xD730 , 0xDF30 , 0xE730 , 0xEF30 , 0xF730 , 0xFF30 ,
    0xC780 , 0xCF80 , 0xD780 , 0xDF80 , 0xE780 , 0xEF80 , 0xF780 , 0xFF80 
};
uint8_t const penused[15] = {
    0B11000000,
    0B00001100,
    0B11001100,
    0B00110000,
    0B11110000,
    0B00111100,
    0B11111100,
    0B00000011,
    0B11000011,
    0B00001111,
    0B11001111,
    0B00110011,
    0B11110011,
    0B00111111,
    0B11111111  
}; //A0 B0 	A2 	B2 	A1 	B1 	A3 	B3 



uint8_t memory[0x1000];

uint16_t screen_offset;
uint16_t screen_first_byte_address;
uint16_t cdtc_screen_basic__table_hl;
uint16_t cdtc_screen_basic__table_hl_source;
uint16_t cdtc_screen_basic__table_hl_dest;
uint8_t changeVideoBuffer;
uint8_t activepen;
uint8_t clsline;
uint8_t lineparity;
uint8_t background;
uint8_t foreground;

void chip8ResetCpu()
{
    memset(&cpu,0,sizeof(cpu_t));
    screen_offset = 0;
  // Programs start at memory address 0x200
    cpu.PC = 0x200;
}


 void fetchInstruction()
{
    cpu.opcode = memory[cpu.PC] << 8 | memory[cpu.PC + 1];
}


#define OPCODE_X ((cpu.opcode & 0x0F00) >> 8)
#define OPCODE_Y ((cpu.opcode & 0x00F0) >> 4)
#define OPCODE_N (cpu.opcode & 0x000F)
#define OPCODE_KK (cpu.opcode & 0x00FF)
#define OPCODE_NNN (cpu.opcode & 0x0FFF)



// 00BN: Scroll display N lines up
 void exec00BN() // esto se puede optimizar usando una pantalla de 256 pixeles de ancho
                    //tocando los registros del CRTC
/*Hay que hacer dos bucles, uno que recorra cada linea de caracteres 
    y otro que recorra todas las lineas de caracteres para respetar la distribución de 
    memoria de video:
        | Character   |  Pixel |  Pixel |  Pixel |  Pixel |  Pixel |  Pixel |  Pixel |  Pixel |
    |   Line      | Line 0 | Line 1 | Line 2 | Line 3 | Line 4 | Line 5 | Line 6 | Line 7 |
    ---------------------------------------------------------------------------------------
    |      1      | 0xC000 | 0xC800 | 0xD000 | 0xD800 | 0xE000 | 0xE800 | 0xF000 | 0xF800 |
    |      2      | 0xC050 | 0xC850 | 0xD050 | 0xD850 | 0xE050 | 0xE850 | 0xF050 | 0xF850 |
    |      3      | 0xC0A0 | 0xC8A0 | 0xD0A0 | 0xD8A0 | 0xE0A0 | 0xE8A0 | 0xF0A0 | 0xF8A0 |
    |      4      | 0xC0F0 | 0xC8F0 | 0xD0F0 | 0xD8F0 | 0xE0F0 | 0xE8F0 | 0xF0F0 | 0xF8F0 |
    |      5      | 0xC140 | 0xC940 | 0xD140 | 0xD940 | 0xE140 | 0xE940 | 0xF140 | 0xF940 |
    |      6      | 0xC190 | 0xC990 | 0xD190 | 0xD990 | 0xE190 | 0xE990 | 0xF190 | 0xF990 |
    |      7      | 0xC1E0 | 0xC9E0 | 0xD1E0 | 0xD9E0 | 0xE1E0 | 0xE9E0 | 0xF1E0 | 0xF9E0 |
    |      8      | 0xC230 | 0xCA30 | 0xD230 | 0xDA30 | 0xE230 | 0xEA30 | 0xF230 | 0xFA30 |
    |      9      | 0xC280 | 0xCA80 | 0xD280 | 0xDA80 | 0xE280 | 0xEA80 | 0xF280 | 0xFA80 |
    |     10      | 0xC2D0 | 0xCAD0 | 0xD2D0 | 0xDAD0 | 0xE2D0 | 0xEAD0 | 0xF2D0 | 0xFAD0 |
    |     11      | 0xC320 | 0xCB20 | 0xD320 | 0xDB20 | 0xE320 | 0xEB20 | 0xF320 | 0xFB20 |
    |     12      | 0xC370 | 0xCB70 | 0xD370 | 0xDB70 | 0xE370 | 0xEB70 | 0xF370 | 0xFB70 |
    |     13      | 0xC3C0 | 0xCBC0 | 0xD3C0 | 0xDBC0 | 0xE3C0 | 0xEBC0 | 0xF3C0 | 0xFBC0 |
    |     14      | 0xC410 | 0xCC10 | 0xD410 | 0xDC10 | 0xE410 | 0xEC10 | 0xF410 | 0xFC10 |
    |     15      | 0xC460 | 0xCC60 | 0xD460 | 0xDC60 | 0xE460 | 0xEC60 | 0xF460 | 0xFC60 |
    |     16      | 0xC4B0 | 0xCCB0 | 0xD4B0 | 0xDCB0 | 0xE4B0 | 0xECB0 | 0xF4B0 | 0xFCB0 |
    |     17      | 0xC500 | 0xCD00 | 0xD500 | 0xDD00 | 0xE500 | 0xED00 | 0xF500 | 0xFD00 |
    |     18      | 0xC550 | 0xCD50 | 0xD550 | 0xDD50 | 0xE550 | 0xED50 | 0xF550 | 0xFD50 |
    |     19      | 0xC5A0 | 0xCDA0 | 0xD5A0 | 0xDDA0 | 0xE5A0 | 0xEDA0 | 0xF5A0 | 0xFDA0 |
    |     20      | 0xC5F0 | 0xCDF0 | 0xD5F0 | 0xDDF0 | 0xE5F0 | 0xED50 | 0xF550 | 0xFD50 |
    |     21      | 0xC640 | 0xCE40 | 0xD640 | 0xDE40 | 0xE640 | 0xEE40 | 0xF640 | 0xFE40 |
    |     22      | 0xC690 | 0xCE90 | 0xD690 | 0xDE90 | 0xE690 | 0xEE90 | 0xF690 | 0xFE90 |
    |     23      | 0xC6E0 | 0xCEE0 | 0xD6E0 | 0xDEE0 | 0xE6E0 | 0xEEE0 | 0xF6E0 | 0xFEE0 |
    |     24      | 0xC730 | 0xCF30 | 0xD730 | 0xDF30 | 0xE730 | 0xEF30 | 0xF730 | 0xFF30 |
    |     25      | 0xC780 | 0xCF80 | 0xD780 | 0xDF80 | 0xE780 | 0xEF80 | 0xF780 | 0xFF80 |
    | spare start | 0xC7D0 | 0xCFD0 | 0xD7D0 | 0xDFD0 | 0xE7D0 | 0xEFD0 | 0xF7D0 | 0xFFD0 |
    | spare end   | 0xC7FF | 0xCFFF | 0xD7FF | 0xDFFF | 0xE7FF | 0xEFFF | 0xF7FF | 0xFFFF |
    ---------------------------------------------------------------------------------------
              Table 1 - Video memory starting locations for all pixel lines
*/
{ // copiar la lógica de scroll 00CN
    uint8_t bY = OPCODE_N * 2;
    if (bY < 1) return;
    for (uint8_t nY = 1; nY < (127 - bY); nY=nY+2){
        memcpy(videotable[nY], videotable[nY + bY], 64);
        memcpy(videotable[nY+1], videotable[nY + bY], 64);
        memset(videotable[nY + bY], 0, 64 );
        memset(videotable[nY + bY + 1], 0, 64 );
    }
    cpu.PC += 2;cpc_SetColour(0 , cpct_getHWColour(background));
}

// 00BN: Scroll display N lines up
 /*void exec00BN()
{
    memcpy(cpu.screen, &cpu.screen[16 * OPCODE_N], 1024 - 16 * OPCODE_N);
    memset(&cpu.screen[1023 - 16 * OPCODE_N], 0, 16 * OPCODE_N);
    cpu.PC += 2;
}*/
// 00CN: Scroll display N lines down
 void exec00CN(){
    switch (screen_first_byte_address){//falta tener en cuenta el offset->ver scrool lateral en chip8lib
        case FIRST_SCREEN_BUFFER:
            screen_first_byte_address=SECOND_SCREEN_BUFFER;
            cdtc_screen_basic__table_hl=SECOND_SCREEN_BTABLE;
            cdtc_screen_basic__table_hl_source = FIRST_SCREEN_BTABLE;
            cdtc_screen_basic__table_hl_dest = SECOND_SCREEN_BTABLE;
            break;
        case SECOND_SCREEN_BUFFER:
            screen_first_byte_address=FIRST_SCREEN_BUFFER;
            cdtc_screen_basic__table_hl=FIRST_SCREEN_BTABLE;
            cdtc_screen_basic__table_hl_source = SECOND_SCREEN_BTABLE;
            cdtc_screen_basic__table_hl_dest = FIRST_SCREEN_BTABLE;
    }
    scrolld(OPCODE_N * 2);
    changeVideoBuffer = 1;
    /*
    uint8_t bY = OPCODE_N * 2;
    if (bY < 1) return;
    for (uint8_t nY = 127; nY > bY; nY=nY-2){
        //memcpy(videotable[nY], videotable[nY - bY], 64);
        //memcpy(videotable[nY-1], videotable[nY - bY], 64);
        //memset(videotable[nY - bY], 0, 64 );
        //memset(videotable[nY - bY - 1], 0, 64 );
        void* paux = getscrptr((uint16_t)((nY -bY) << 8));
        memcpy(getscrptr((uint16_t)(nY << 8)), paux, 64);
        memcpy(getscrptr((uint16_t)((nY - 1) << 8)), paux, 64);
    }
    for (uint8_t nY = 1; nY < bY; nY=nY+2){
        memset(getscrptr((uint16_t)((nY) << 8)), 0, 64 );
        memset(getscrptr((uint16_t)((nY-1) << 8)), 0, 64 );
    }
    */
    cpu.PC += 2;
 }

// 00E0: Clear the display - ESTO SE PUEDE Y DEBE ACELERAR YA QUE HAY JUEGOS QUE HACEN USO INTENSIVO (ALIEN)
//VALORAR EL USO DE DOBLE BUFFER (CAMBIO DE BUFFER Y LIMPIEZA A TROZOS POR INTERRUPCIONES)
//Estamos en mode 0 monocromo -> acelerar esto *15 usando el truco de Vector Vault https://www.cpcwiki.eu/forum/programming/best-way-to-update-screen-vector-graphics/
 void exec00E0()
{   
    screen_offset = 0;
    /*switch (screen_first_byte_address){//falta tener en cuenta el offset->ver scroll lateral en chip8lib
        case FIRST_SCREEN_BUFFER:
            screen_first_byte_address=SECOND_SCREEN_BUFFER;
            cdtc_screen_basic__table_hl=SECOND_SCREEN_BTABLE;
            break;
        case SECOND_SCREEN_BUFFER:
            screen_first_byte_address=FIRST_SCREEN_BUFFER;
            cdtc_screen_basic__table_hl=FIRST_SCREEN_BTABLE;
    }*/
    //cls();
    uint8_t linecount = 9;
    if (clsline > 125) linecount = 2;
    for (uint8_t line = 0; line < linecount; line++){
        memset (getscrptr((clsline++)<<8),0,64);
    }
    if (clsline > 127) clsline = 0;
    cpc_SetColour(activepen + 1 , cpct_getHWColour(background));
    if (++activepen > 14) activepen = 0;
    cpc_SetColour(activepen + 1 , cpct_getHWColour(foreground));
    //changeVideoBuffer = 1;
    cpu.PC += 2;
}


// 00EE: Return from a subroutine
 void exec00EE()
{
    cpu.SP--;
    cpu.PC = cpu.stack[cpu.SP];
    cpu.PC += 2;
}


// 00FB: Scroll display 4 pixels right
 void exec00FB()
{
    /*for (int row = 0; row < 64; row++)
        for (int col = 15; col >= 0; col--)
        {
            cpu.screen[16 * row + col] >>= 4;
            if (col > 0)
                cpu.screen[16 * row + col] |= (cpu.screen[16 * row + col - 1] & 0x0F) << 4;
        }*/
    cpc_RLI (0xc000, 127, 63);
    cpc_RLI (0xc000, 127, 63);

    cpu.PC += 2;
}


//c2m returns screen byte address of given x,y coordinates. x in chars, y in lines
uint8_t * c2m(uint8_t x, uint8_t y){
    uint8_t char_line = y / 8;
    uint8_t offset_from_start = char_line * 32; //R1=32
    uint8_t total_offset = offset_from_start + screen_offset;
    uint8_t offset_from_start_of_line = x / 2;
    total_offset = total_offset + offset_from_start_of_line;
    total_offset = total_offset & 0x3ff;
    uint8_t total_byte_offset = total_offset * 2;
    uint8_t scan_line = y & 7;
    total_byte_offset = total_byte_offset + (scan_line * 0x800);
    if (x & 1) ++total_byte_offset;
    return screen_first_byte_address + total_byte_offset;
}
// 00FC: Scroll display 4 pixels left
void exec00FC()
{
    /*for (int row = 0; row < 64; row++)
        for (int col = 0; col < 16; col++)
        {
            cpu.screen[16 * row + col] <<= 4;
            if (col < 15)
                cpu.screen[16 * row + col] |= (cpu.screen[16 * row + col + 1] & 0xF0) >> 4;
        }*/
    //cpc_RRI (0xc000, 127, 63);
    //cpc_RRI (0xc000, 127, 63);
    for (uint8_t i = 0 ; i < 128 ; i++){
        //uint8_t *pi = videotable[i] + screen_offset;
        uint8_t *pi = getscrptr((uint16_t)(i << 8));
        //*pi=255;
        *(pi + screen_offset * 2) = 0;
        *(pi + screen_offset * 2 + 1) = 0;
        //*(pi + (screen_offset) + 2) = 255;
        //*(pi + (screen_offset) + 3) = 255;
    }
    screen_offset = ((screen_offset + 1) & 0x1f/*0x3ff*/); 
    cpct_setCRTCReg (12, (screen_offset >> 8) | (0x30)); //0x30 no funciona con doble buffer
    cpct_setCRTCReg (13, (screen_offset & 0xff));
     //incremento módulo 256 &30 OR ((ma%\256) AND 3)
    //cpct_setCRTCReg (12, 0x30 | ((screen_offset/256) & 3));
    /*
    char aux[5];
    aux[0] = ' ';
    aux[1] = (screen_offset / (uint8_t)10) + 48;
    aux[2] = (screen_offset % (uint8_t)10) + 48;
    aux[3] = ' ';
    aux[4] = 0;
    cpc_PrintGphStrXYM12X(aux, screen_offset * 2, 140);
    */
    
    cpu.PC += 2;
}


// 00FD: Exit CHIP interpreter
 void exec00FD()
{
    // Stop incrementing the PC
}


// 00FE: Disable extended screen mode
 void exec00FE()
{
    cpu.extended = 0;
    cpu.PC += 2;
}


// 00FF: Enable extended screen mode
 void exec00FF()
{
    cpu.extended = 1;
    cpu.PC += 2;
}


// 1NNN: Jump to location NNN
 void exec1NNN()
{
    cpu.PC = OPCODE_NNN;
}


// 2NNN: Call subroutine at NNN
 void exec2NNN()
{
    cpu.stack[cpu.SP] = cpu.PC;
    cpu.SP++;
    cpu.PC = OPCODE_NNN;
}


// 3XKK: Skip next instruction if VX = KK
 void exec3XKK()
{
    if (cpu.V[OPCODE_X] == OPCODE_KK)
        cpu.PC += 4;
    else
        cpu.PC += 2;
}


// 4XKK: Skip next instruction if VX != KK
 void exec4XKK()
{
    if (cpu.V[OPCODE_X] != OPCODE_KK)
        cpu.PC += 4;
    else
        cpu.PC += 2;
}


// 5XY0: Skip next instruction if VX = VY
 void exec5XY0()
{
    if (cpu.V[OPCODE_X] == cpu.V[OPCODE_Y])
        cpu.PC += 4;
    else
        cpu.PC += 2;
}


// 6XKK: Set VX = KK
 void exec6XKK()
{
    cpu.V[OPCODE_X] = OPCODE_KK;
    cpu.PC += 2;
}


// 7XKK: Adds KK to VX
 void exec7XKK()
{
    cpu.V[OPCODE_X] += OPCODE_KK;
    cpu.PC += 2;
}


// 8XY0: Set VX = VY
 void exec8XY0()
{
    cpu.V[OPCODE_X] = cpu.V[OPCODE_Y];
    cpu.PC += 2;
}


// 8XY1: Set VX = VX OR VY
 void exec8XY1()
{
    cpu.V[OPCODE_X] |= cpu.V[OPCODE_Y];
    cpu.PC += 2;
}


// 8XY2: Set VX = VX AND VY
 void exec8XY2()
{
    cpu.V[OPCODE_X] &= cpu.V[OPCODE_Y];
    cpu.PC += 2;
}


// 8XY3: Set VX = VX XOR VY
 void exec8XY3()
{
    cpu.V[OPCODE_X] ^= cpu.V[OPCODE_Y];
    cpu.PC += 2;
}


// 8XY4: Set VX = VX + VY, set VF = carry
 void exec8XY4()
{
    cpu.V[0xF] = (cpu.V[OPCODE_X] + cpu.V[OPCODE_Y]) > 0xFF;
    cpu.V[OPCODE_X] += cpu.V[OPCODE_Y];
    cpu.PC += 2;
}


// 8XY5: Set VX = VX - VY, set VF = NOT borrow
 void exec8XY5()
{
    // Borrow occurred if VY > VX, so NOT borrow occured if VX >= VY
    cpu.V[0xF] = cpu.V[OPCODE_X] >= cpu.V[OPCODE_Y];
    // VX = VX - VY
    cpu.V[OPCODE_X] -= cpu.V[OPCODE_Y];
    cpu.PC += 2;
}


// 8XY6: Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
// Note: The original implementation stores the value of register VY shifted right one bit in register VX.
 void exec8XY6()
{
    if (cpu.compatibility_mode)
    {
        cpu.V[0xF] = cpu.V[OPCODE_Y] & 0x01;
        cpu.V[OPCODE_X] = cpu.V[OPCODE_Y] >> 1;
    }
    else
    {
        cpu.V[0xF] = cpu.V[OPCODE_X] & 0x01;
        cpu.V[OPCODE_X] >>= 1;
    }

    cpu.PC += 2;
}


// 8XY7: Set VX = VY - VX, set VF = NOT borrow
 void exec8XY7()
{
    // Borrow occurred if VX > VY, so NOT borrow occured if VY >= VX
    cpu.V[0xF] = cpu.V[OPCODE_Y] >= cpu.V[OPCODE_X];
    // VX = VY - VX
    cpu.V[OPCODE_X] = cpu.V[OPCODE_Y] - cpu.V[OPCODE_X];
    cpu.PC += 2;
}


// 8XYE: Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
// Note: The original implementation stores the value of register VY shifted left one bit in register VX.
 void exec8XYE()
{
    if (cpu.compatibility_mode)
    {
        cpu.V[0xF] = cpu.V[OPCODE_Y] >> 7;
        cpu.V[OPCODE_X] = cpu.V[OPCODE_Y] << 1;
    }
    else
    {
        cpu.V[0xF] = cpu.V[OPCODE_X] >> 7;
        cpu.V[OPCODE_X] <<= 1;
    }

    cpu.PC += 2;
}


// 9XY0: Skip next instruction if VX != VY
 void exec9XY0()
{
    if (cpu.V[OPCODE_X] != cpu.V[OPCODE_Y])
        cpu.PC += 4;
    else
        cpu.PC += 2;
}


// ANNN: Set I = NNN
 void execANNN()
{
    cpu.I = OPCODE_NNN;
    cpu.PC += 2;
}


// BNNN: Jump to location NNN + V0
 void execBNNN()
{
    cpu.PC = OPCODE_NNN + cpu.V[0];
}


// CXKK: Set VX = random byte AND KK
 void execCXKK()
{
    cpu.V[OPCODE_X] = cpc_Random() & OPCODE_KK;
    cpu.PC += 2;
}


static void execDXY0() 
{
    cpu.V[0xF] = 0;
    uint8_t iscollision=0;
    uint8_t VX=cpu.V[OPCODE_X];
    uint8_t VY=(cpu.V[OPCODE_Y] * 2);// + (128-(VX))>(screen_offset * 4)((screen_offset >> 7) * 8);//investigar porqué car sólo funciona sumando 1 //hay sitio para triplicar en vez de doblar
    if (VX > (120-screen_offset*4)%128) VY = (VY + (8 * ((screen_offset / 32)))) % 128;
    VX = (VX + screen_offset * 4) % 128;
    //if (VX > 120) VY += screen_offset / (uint8_t)4;
    //VY += 8 * ((screen_offset) / (uint8_t)64);
    //if (VX > (128 - (screen_offset * 4))) VY += 8;
    //VY += 8 * ((screen_offset) / (uint8_t)32);    //uint8_t VX=cpu.V[OPCODE_X] + screen_offset * 4;
    //uint8_t VY=(cpu.V[OPCODE_Y] * 2) + ((screen_offset >> 7) * 8) ; //hay sitio para triplicar en vez de doblar y con scanlines me ahorro pintar
    uint8_t nshift = VX & 1; // MOD 2
    //uint8_t nshift2 = VX & 7; //MOD 8
    uint16_t coord;
    coord = VX + (uint16_t)(VY << 8);
    uint8_t mask = penused[activepen];
    uint8_t maskl = mask & 0B10101010;
    uint8_t maskr = mask & 0B01010101;
        
    for (uint8_t row = 0; row < 16; row++)
    {	
        //uint8_t *pAddress1 =  videotable[(VY + row) & 127]+((VX & 127) / 2) + (screen_offset * 2);//0xC000 + (((VY + row) / 8) * 80) + (((VY + row) & 7) * 2048) + (VX / 2);
        uint8_t *pAddress1 = getscrptr(coord);// + (screen_offset * 2);
        //VY+=1;
        //uint8_t *pAddress2 =  videotable[(VY + row) & 127]+((VX & 127) / 2) + (screen_offset * 2);//(uint8_t *)0xC000 + (((VY + row) / 8) * 80) + (((VY + row) & 7) * 2048) + (VX / 2);
    	coord += 256;
        uint8_t *pAddress2 = getscrptr(coord);// + (screen_offset * 2);
        uint8_t dbl = memory[cpu.I + 2 * row];
    	uint8_t dbr = memory[cpu.I + 2 * row + 1];    	
    	if (nshift)
        {
            if (dbl & 128){ //un codigo automodificante obviaría estos condicionales, un asm rápido sería una secuencia se shifts con carry y salto si o no carry
            *(pAddress1) &= 0B10101010; //*(pAddress1) = (*(pAddress1) & 0B10101010) | (maskr * (!(dbl & 127)))
                if ((*(pAddress2) & 0B01010101) == maskr){ //iscollision = (*(pAddress2) & 0B01010101) * (!(dbl & 127));
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbl & 64){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbl & 32){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbl & 16){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbl & 8){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbl & 4){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbl & 2){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbl & 1){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 128){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 64){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 32){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 16){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 8){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 4){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 2){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 1){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            *(pAddress2) = *(pAddress1);
		}
   	    else {	
            if (dbl & 128){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbl & 64){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbl & 32){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbl & 16){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbl & 8){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbl & 4){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbl & 2){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbl & 1){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 128){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 64){ 
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 32){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 16){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 8){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 4){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (dbr & 2){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (dbr & 1){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2) = *(pAddress1);
		}
    coord += 256;
    }
    if (iscollision) cpu.V[0xF]=1;
    cpu.PC += 2;
}

 static void execDXYN() 
{
    cpu.V[0xF] = 0;
    uint8_t iscollision=0;
    uint8_t VX=cpu.V[OPCODE_X];
    uint8_t VY=(cpu.V[OPCODE_Y] * 2);// + (128-(VX))>(screen_offset * 4)((screen_offset >> 7) * 8);//investigar porqué car sólo funciona sumando 1 //hay sitio para triplicar en vez de doblar
    //if (VX > 120) VY += (screen_offset / (uint8_t)32) * 8;
    if (VX > (120-screen_offset*4)%128) VY = (VY + (8 * ((screen_offset / 32)))) % 128;
    VX = (VX + screen_offset * 4) % 128;
    //VY += 8 * ((screen_offset) / (uint8_t)64);
    uint8_t nshift = VX & 1; // MOD 2
    uint16_t coord;
    coord = VX + (uint16_t)(VY << 8);
    //uint8_t nshift2 = VX & 7; //MOD 8
    uint8_t mask = penused[activepen];
    uint8_t maskl = mask & 0B10101010;
    uint8_t maskr = mask & 0B01010101;
    
    for (uint8_t row = 0; row < OPCODE_N; row++)
    {	
        //uint8_t *pAddress1 =  videotable[(VY + row) & 127]+((VX & 127) / 2) + (screen_offset * 2);//(uint8_t *)0xC000 + (((VY + row) / 8) * 80) + (((VY + row) & 7) * 2048) + (VX / 2);
        uint8_t *pAddress1 = getscrptr(coord);// + (screen_offset * 2);
        //VY+=1;
        coord += 256;
        //uint8_t *pAddress2 =  videotable[(VY + row) & 127]+((VX & 127) / 2) + (screen_offset * 2);//(uint8_t *)0xC000 + (((VY + row) / 8) * 80) + (((VY + row) & 7) * 2048) + (VX / 2);
    	uint8_t *pAddress2 = getscrptr(coord);// + (screen_offset * 2);
        uint8_t db = memory[cpu.I + row];
    	if (nshift)
        {
            if (db & 128){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (db & 64){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (db & 32){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (db & 16){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (db & 8){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (db & 4){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (db & 2){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (db & 1){ 
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            *(pAddress2) = *(pAddress1);
		}            
   	    else {
            if (db & 128){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (db & 64){
                *(pAddress1) &= 0B10101010; 
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (db & 32){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (db & 16){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (db & 8){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (db & 4){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2++) = *(pAddress1++);
            if (db & 2){
                *(pAddress1) &= 0B01010101;
                if ((*(pAddress2) & 0B10101010) == maskl){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskl;
                }
            }
            if (db & 1){
                *(pAddress1) &= 0B10101010;
                if ((*(pAddress2) & 0B01010101) == maskr){
                    iscollision = 1;
                }
                else {
                    *(pAddress1) |= maskr;
                }
            }
            *(pAddress2) = *(pAddress1);
		}
    coord += 256;
    }
    if (iscollision) cpu.V[0xF]=1;
    cpu.PC += 2;
}


// EX9E: Skip next instruction if key with the value of VX is pressed
 void execEX9E()
{
    if (cpu.keyboard[cpu.V[OPCODE_X]])
        cpu.PC += 4;
    else
        cpu.PC += 2;
}


// EXA1: Skip next instruction if key with the value of VX is not pressed
 void execEXA1()
{
    if (!cpu.keyboard[cpu.V[OPCODE_X]])
        cpu.PC += 4;
    else
        cpu.PC += 2;
}


// FX07: Set VX = delay timer value
 void execFX07()
{
    cpu.V[OPCODE_X] = cpu.delay_timer;
    cpu.PC += 2;
}


// FX0A: Wait for a key press, store the value of the key in VX
 void execFX0A()
{
    // If no key was pressed, repeat the instruction until a key is pressed
    for (int key = 0; key < 16; key++)
    {
        if (cpu.keyboard[key])
        {
            cpu.V[OPCODE_X] = key;
            cpu.PC += 2;
        }
        cpu.keyboard[key] = 0;
    }
}


// FX15: Set delay timer = VX
 void execFX15()
{
    cpu.delay_timer = cpu.V[OPCODE_X];
    cpu.PC += 2;
}


// FX18: Set sound timer = VX
 void execFX18()
{
    cpu.sound_timer = cpu.V[OPCODE_X];
    cpu.PC += 2;
}


// FX1E: Set I = I + VX
 void execFX1E()
{
    cpu.I += cpu.V[OPCODE_X];
    cpu.V[0xF] = cpu.I >= 0x1000;
    cpu.PC += 2;
}


// FX29: Set I = location of low-res sprite for digit VX
 void execFX29()
{
    cpu.I = 0x50 + cpu.V[OPCODE_X] * (uint8_t)5;
    cpu.PC += 2;
}


// FX30: Set I = location of high-res sprite for digit VX
 void execFX30()
{
    cpu.I = 0xA0 + cpu.V[OPCODE_X] * (uint8_t)10;
    cpu.PC += 2;
}


// FX33: Store BCD representation of VX in memory locations I, I+1, and I+2
 void execFX33()
{
    memory[cpu.I & 0xFFF] = cpu.V[OPCODE_X] / (uint8_t)100;
    memory[(cpu.I + 1) & 0xFFF] = (cpu.V[OPCODE_X] / (uint8_t)10) % (uint8_t)10;
    memory[(cpu.I + 2) & 0xFFF] = cpu.V[OPCODE_X] % (uint8_t)10;
    cpu.PC += 2;
}


// FX55: Store registers V0 through VX in memory starting at location I
// Note: The original implementation also increments I by the number of registers written.
 void execFX55()
{
    for (int i = 0; i <= OPCODE_X; i++)
        memory[(cpu.I + i) & 0xFFF] = cpu.V[i];

    if (cpu.compatibility_mode)
        cpu.I += (OPCODE_X + 1);

    cpu.PC += 2;
}


// FX65: Read registers V0 through VX from memory starting at location I
// Note: The original implementation also increments I by the number of registers read.
 void execFX65()
{
    for (int i = 0; i <= OPCODE_X; i++)
        cpu.V[i] = memory[(cpu.I + i) & 0xFFF];

    if (cpu.compatibility_mode)
        cpu.I += (OPCODE_X + 1);

    cpu.PC += 2;
}


// FX75: Store V0..VX in RPL user flags (X <= 7)
 void execFX75()
{
    if (OPCODE_X <= 7)
        memcpy(cpu.R, cpu.V, OPCODE_X + 1);
    cpu.PC += 2;
}


// FX85: Read V0..VX from RPL user flags (X <= 7)
 void execFX85()
{
    if (OPCODE_X <= 7)
        memcpy(cpu.V, cpu.R, OPCODE_X + 1);
    cpu.PC += 2;
}


 uint8_t decodeInstruction()
{
    uint8_t unknown_opcode = 0;

    switch (cpu.opcode & 0xF000)
    {
        case 0x0000:
            switch (cpu.opcode & 0xFFF0)
            {
                case 0x00B0:            exec00BN();         break;
                case 0x00C0:            exec00CN();         break;
                case 0x00E0:
                    switch (cpu.opcode)
                    {
                        case 0x00E0:    exec00E0();         break;
                        case 0x00EE:    exec00EE();         break;
                        default:    unknown_opcode = 1;  break;
                    }
                break;
                case 0X00F0:
                    switch (cpu.opcode)
                    {
                        case 0x00FB:    exec00FB();         break;
                        case 0x00FC:    exec00FC();         break;
                        case 0x00FD:    exec00FD();         break;
                        case 0x00FE:    exec00FE();         break;
                        case 0x00FF:    exec00FF();         break;
                        default:    unknown_opcode = 1;  break;
                    }
                break;
                default:            unknown_opcode = 1;  break;
            }
        break;
        case 0x1000:                    exec1NNN();         break;
        case 0x2000:                    exec2NNN();         break;
        case 0x3000:                    exec3XKK();         break;
        case 0x4000:                    exec4XKK();         break;
        case 0x5000:                    exec5XY0();         break;
        case 0x6000:                    exec6XKK();         break;
        case 0x7000:                    exec7XKK();         break;
        case 0x8000:
            switch (cpu.opcode & 0x000F)
            {
                case 0x0000:            exec8XY0();         break;
                case 0x0001:            exec8XY1();         break;
                case 0x0002:            exec8XY2();         break;
                case 0x0003:            exec8XY3();         break;
                case 0x0004:            exec8XY4();         break;
                case 0x0005:            exec8XY5();         break;
                case 0x0006:            exec8XY6();         break;
                case 0x0007:            exec8XY7();         break;
                case 0x000E:            exec8XYE();         break;
                default:            unknown_opcode = 1;  break;
            }
        break;
        case 0x9000:                    exec9XY0();         break;
        case 0xA000:                    execANNN();         break;
        case 0xB000:                    execBNNN();         break;
        case 0xC000:                    execCXKK();         break;
        case 0xD000:
            switch(cpu.opcode & 0x000F)
            {
                case 0x0000:            execDXY0();         break;
                default:                execDXYN();         break;
            }
        break;
        case 0xE000:
            switch (cpu.opcode & 0x00FF)
            {
                case 0x009E:            execEX9E();         break;
                case 0x00A1:            execEXA1();         break;
                default:            unknown_opcode = 1;  break;
            }
        break;
        case 0xF000:
            switch (cpu.opcode & 0x00FF)
            {
                case 0x0007:            execFX07();         break;
                case 0x000A:            execFX0A();         break;
                case 0x0015:            execFX15();         break;
                case 0x0018:            execFX18();         break;
                case 0x001E:            execFX1E();         break;
                case 0x0029:            execFX29();         break;
                case 0x0030:            execFX30();         break;
                case 0x0033:            execFX33();         break;
                case 0x0055:            execFX55();         break;
                case 0x0065:            execFX65();         break;
                case 0x0075:            execFX75();         break;
                case 0x0085:            execFX85();         break;
                default:            unknown_opcode = 1;  break;
            }
        break;
        default:                    unknown_opcode = 1;  break;
    }

    return unknown_opcode;
}


uint8_t chip8StepCpu(uint8_t cycles)
{
    uint8_t unknown_opcode = 0;

    for (uint8_t i = 0; i < cycles; i++)
    {
        fetchInstruction();
        unknown_opcode |= decodeInstruction();
    }
    return unknown_opcode;
}



void chip8CompatibilityMode(uint8_t enabled)
{
    cpu.compatibility_mode = enabled;
}


void chip8VerticalWrap(uint8_t enabled)
{
    cpu.vertical_wrap = enabled;
}

/*
void chip8UpdateTimers(uint8_t cycles_60)
{
    if (cpu.delay_timer > cycles_60)
        cpu.delay_timer -= cycles_60;
    else 
        cpu.delay_timer = 0;

    if (cpu.sound_timer > cycles_60)
        cpu.sound_timer -= cycles_60;
    else
        cpu.sound_timer = 0;
}
*/
/*
void chip8UpdateKey(uint8_t key, uint8_t status)
{
    if (key <= 0xF)
        cpu.keyboard[key] = status;
}
*/
/*
uint8_t chip8GetAudio()
{
    return cpu.sound_timer > 0;
}
*/
/*
uint8_t* chip8GetScreen()
{
    return cpu.screen;
}
*/

 //uint8_t chip8GetPixel(uint8_t nY, uint8_t nX)
//{
    /*if (row >= 0 && row <= 63 && col >= 0 && col <= 127)
        return (cpu.screen[(128 * row + col) / 8] >> (7 - (col % 8))) & 1;
    else
        return 0;*/
    //return fw_gra_test_absolute(col,row);
//    uint8_t *pAddress = (uint8_t *)0xC000 + ((nY / 8) * 80) + ((nY % 8) * 2048) + (nX / 8);
 //   uint8_t nBit = 7 - nX % 8;
  //  return (*pAddress >> nBit) & 1;
//}


 // void setPixel(uint8_t nY, uint8_t nX)
//{
    //uint16_t_as_uint8_t_pair_t cxy;
    //cxy.low = (uint8_t)col;
    //cxy.high = (uint8_t)row;
    /*if (row >= 0 && row <= 63 && col >= 0 && col <= 127)
        cpu.screen[(128 * row + col) / 8] |= (1 << (7 - (col % 8)));*/
    //fw_gra_set_pen(1);
    //fw_gra_plot_absolute(col,row);
    //cdtc_screen_basic__plot( (row<<8)+col );
//    uint8_t *pAddress =  (uint8_t *)0xC000 + ((nY / 8) * 80) + ((nY % 8) * 2048) + (nX / 8);
 //   uint8_t nBit = 7 - nX % 8;
  //  uint8_t nAux = (1 << nBit);
   // *pAddress = *pAddress | nAux;
 // }


//  void clearPixel(uint8_t nY, uint8_t nX)
//{
    /*if (row >= 0 && row <= 63 && col >= 0 && col <= 127)
        cpu.screen[(128 * row + col) / 8] &= ~(1 << (7 - (col % 8)));*/
    //fw_gra_set_pen(0);
    //cdtc_screen_basic__plot( (row<<8)+col );
    //fw_gra_plot_absolute(col,row);
//    uint8_t *pAddress =  (uint8_t *)0xC000 + ((nY / 8) * 80) + ((nY % 8) * 2048) + (nX / 8);
//    uint8_t nBit = 7 - nX % 8;
//    uint8_t nAux = ~(1 << nBit);
//    *pAddress = *pAddress & nAux;
//}
