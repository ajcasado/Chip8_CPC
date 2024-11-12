#include "chip8.h"
#include "chip8lib.h"
#include "cpcrslib.h"
#include "asm.h"
//#include "cfwi/cfwi.h"
#include "cdtc/cdtc_screen_basic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TONEH 0B00000010 //110Hz 
#define TONEL 0B00111000 //110Hz 
#define ENVFREQH 0B00000100 //50HZ
#define ENVFREQL 0B11100010 //50HZ
#define GAME 4


__at (0xbe7d)uint8_t fw_drive_number;
//uint8_t chipram[];
extern uint8_t* memory[];
extern uint8_t* videotable[];
extern uint16_t screen_first_byte_address;
extern uint16_t cdtc_screen_basic__table_hl;
extern uint8_t changeVideoBuffer;
extern uint8_t background;
extern uint8_t foreground;
extern uint8_t activepen;
extern uint8_t penused[];
extern uint8_t clsline;
extern uint8_t lineparity;
uint8_t drive_number;
static uint16_t current_time = 0;
static uint16_t last_time = 0;
static uint8_t count5 = 0;
static uint8_t count6 = 0;
static uint8_t soundon = 0;
uint8_t disc_buf[2048];
uint8_t file_name[13];
uint8_t file_name_length;
uint8_t nfiles = 0;
uint8_t reset_chip8 = 0;
uint8_t load = 0;
uint8_t menuconf = 0;
uint8_t border = 13;

void initemu(){
    if (background == foreground){
        background = 0;
        foreground = 26;
        border = 13;
    }
    drive_number = fw_drive_number;
    cpc_SetInk(0, background);
    cpc_SetInk(1, foreground);
    activepen = 0;
    for (uint8_t ind = 2; ind < 16; ind++) cpc_SetInk(ind, background);
    cpc_SetBorder(border);
    cpc_SetModo(0);
    cpc_DisableFirmware();
	chip8ResetCpu();
    setsound();
    cpct_setInterruptHandler(&interrupt_handler);
    cdtc_screen_basic__configure_crtc();
    screen_first_byte_address=SECOND_SCREEN_BUFFER;
    cdtc_screen_basic__table_hl=SECOND_SCREEN_BTABLE;
    cdtc_screen_basic__table_hl__fill();
    screen_first_byte_address=FIRST_SCREEN_BUFFER;
    cdtc_screen_basic__table_hl=FIRST_SCREEN_BTABLE;
    cdtc_screen_basic__table_hl__fill();
    changeVideoBuffer = 0;
    memset(FIRST_SCREEN_BUFFER, 0, 0x4000);
    memset(SECOND_SCREEN_BUFFER, 0, 0x4000);
    cpct_setCRTCReg(3 , 136);
    lineparity = 0;
    clsline = 0;
}

void main ()
{
    load_menu();
    initemu();
    uint8_t framerate = 1; // fr/10
    uint8_t ne = 1;
    memcpy (&memory[40],charset8,180); //Este 40 me escama, canónicamente debería ser 80. (0x50)
    //for (uint8_t i = 0 ; i < 180 ; i++) memory[i]=charset8[i];
    //uint8_t frarr[16] = {0};
    //uint8_t nearr[16] = {0};
    //uint8_t aind = 0;

	while (1)
    {   
        if (load){
            load = 0;
            nfiles = 0;
            cpct_setCRTCReg(12 , 48);
            cpct_setCRTCReg(13 , 0);
            cpc_EnableFirmware();
            load_menu();
            initemu();
        }
        if (reset_chip8){
            reset_chip8 = 0;
            chip8ResetCpu();
            cls();
            }
        if (menuconf){
            confmenu();
            if (background == foreground){
                background = 0;
                foreground = 26;
                border = 13;
                menuconf = 1;
            }
        }
        //uint16_t avgfr = 0;
        //uint16_t avgnr = 0;
        //nearr[aind & 15] = ne;
        //frarr[aind++ & 15] = framerate;
        //for (uint8_t i = 0; i < 16; i++){
        //    avgfr += frarr[i];
            //avgnr += nearr[i];
        //}
        //avgfr /= 16u;
        //avgnr /= 16;
        uint16_t elapsed_time = (current_time - last_time); // 1/300 seg
        if ( elapsed_time >= 30 / framerate )
        {
            last_time = current_time;
            chip8StepCpu(ne);
            if (framerate > 1) framerate--;
            if (framerate < 2) 
                if (ne > 1) ne--;
            if (framerate > 3) 
                if (ne < 255) ne++;
        }
        else if (framerate < 50) framerate++;
    }
}

void interrupt_handler() 
{   
    if(!menuconf){
        //count6++; 
        count5++;
        if (isVSYNC()){
            //count6 = 0;
            //cpc_SetMode(0);
            //cpc_SetColour(0 , cpct_getHWColour(background));
            //cpc_SetColour(1 , cpct_getHWColour(foreground));
            if (changeVideoBuffer == 1){
                changeVideoBuffer = 0;
                switch (screen_first_byte_address){//falta tener en cuenta el offset->ver scroll horiz. en chip8lib
                    case SECOND_SCREEN_BUFFER:
                        cpct_setCRTCReg(12 , 16);
                        //cpct_setCRTCReg(13 , 0);
                        break;
                    case FIRST_SCREEN_BUFFER:
                        cpct_setCRTCReg(12 , 48);
                        //cpct_setCRTCReg(13 , 0);
                }
            }
            cpc_ScanKeyboard();
            if (cpc_TestKeyF(16)){
                reset_chip8 = 1;
                while (cpc_TestKeyF(16)) cpc_ScanKeyboard();
            }
            if (cpc_TestKeyF(22)){
                load = 1;
                while (cpc_TestKeyF(22)) cpc_ScanKeyboard();
            }
            if (cpc_TestKeyF(21)){
                menuconf = 1;
                while (cpc_TestKeyF(21)) cpc_ScanKeyboard();
            }
            cpu.keyboard[0] = cpc_TestKeyF(0);
            cpu.keyboard[1] = cpc_TestKeyF(1);
            cpu.keyboard[2] = cpc_TestKeyF(2);
            cpu.keyboard[3] = cpc_TestKeyF(3);
            cpu.keyboard[4] = cpc_TestKeyF(4);
            cpu.keyboard[5] = cpc_TestKeyF(5);
            cpu.keyboard[6] = cpc_TestKeyF(6);
            cpu.keyboard[7] = cpc_TestKeyF(7);
            cpu.keyboard[8] = cpc_TestKeyF(8);
            cpu.keyboard[9] = cpc_TestKeyF(9);
            cpu.keyboard[10] = cpc_TestKeyF(10);
            cpu.keyboard[11] = cpc_TestKeyF(11);
            cpu.keyboard[12] = cpc_TestKeyF(12);
            cpu.keyboard[13] = cpc_TestKeyF(13);
            cpu.keyboard[14] = cpc_TestKeyF(14);
            cpu.keyboard[15] = cpc_TestKeyF(15);
        }
        //cpc_SetColour(17,cpct_getHWColour(count6));
        /*if(changeVideoBuffer == 1){//rápida y suciamente, sin tener en cuenta como afecta esto a delay y sound timer
        changeVideoBuffer++;
            cpct_waitVSYNC();
            count5 = 0; //Sincronizamos la 3ª interrupción con el periodo vsync, habrá que ver sin con
                    //tanto deshabilitar las interrupciones hay que repetir esto más.
        }*/
        if (!(++current_time)) last_time = 0;
        if (count5 == 5){
            //cpc_SetMode(1); 
            //cpc_SetColour(0 , cpct_getHWColour(foreground));
            //cpc_SetColour(1 , cpct_getHWColour(background));
            count5 = 0;
            if (cpu.delay_timer) (cpu.delay_timer)--;
            if (cpu.sound_timer){
                (cpu.sound_timer)--;
                if (!soundon){
                    write_ay((7<<8)|0B11111000);
                    soundon = 1;
                }
            }
            else{
                if (soundon){
                    write_ay((7<<8)|0B11111111);
                    soundon = 0;
                }
            }
        }
    }
}

void setsound(){
    write_ay(TONEL);
    write_ay((1<<8)|TONEH);
    write_ay((2<<8)|TONEL);
    write_ay((3<<8)|TONEH);
    write_ay((4<<8)|TONEL);
    write_ay((5<<8)|TONEH);
    write_ay((7<<8)|0B11111111);
    write_ay((8<<8)|0B00010000);
    write_ay((9<<8)|0B00010000);
    write_ay((10<<8)|0B00010000);
    write_ay((11<<8)|ENVFREQL);
    write_ay((12<<8)|ENVFREQH);
    write_ay((13<<8)|0B00001101);
}

void load_menu(){
    __asm
    ;;------------------------------------------------------------------------
    ld (#_stack+1),SP ;save program stack pointer
    pop hl
    ld (#_ret_add+1),hl ;save return address
    ld c,#0xff          ;; disable all roms
    ld hl, #_start22         ;; execution address for program
    call #0xbd16  ;;mc_start_program    ;; start it
    _start22:: nop
    call #0xbccb  ;;kl_rom_walk     ;; enable all roms 
    _stack: 
    ld sp,#0 ;restore program stack pointer
    pop hl
    _ret_add:
    ld hl,#0 ;
    push hl ;restore return address
    ;;------------------------------------------------------------------------
    __endasm;
    cpct_setCRTCReg(1,40);
    cpct_setCRTCReg(2,46);
    //cpct_setCRTCReg(4,38);
    cpct_setCRTCReg(6,25);
    cpct_setCRTCReg(7,30);
    //cpct_setCRTCReg(9,7);
    cpc_SetModo(1);
    cpc_SetInk(0,26);
    cpc_SetInk(1,6);
    cpc_SetInk(2,4);
    cpc_SetInk(3,5);
    cpc_SetBorder(13);
    docat();
    printcat();
    selectfile();
    loadfile();
 }
 void printcat(){
    for (uint16_t i = 1 ; i < 2036; i += 14){ // según esto podría guardar ~146 nombres en disc_buf
        uint8_t name[9];                //pero en el ancho de pantalla sólo caben hasta 99  
        uint8_t ext[4];
        memcpy(name, disc_buf + i, 8);
        memcpy(ext ,disc_buf + i + 8, 3);
        for (uint8_t c = 0; c < 8; c++) 
            if (name[c] < 33)
                name[c] = 0;
        for (uint8_t c = 0; c < 3; c++)
            if (ext[c] < 33)
                ext[c] = 0;
        uint8_t c1 = 0;
        while((name[c1] > 0) && (c1 < 8))
            file_name[c1] = name[c1++];
        uint8_t c2 = 0;
        file_name[c1++]=':';
        while((ext[c2] > 0) && (c2 < 3))
            file_name[c1++] = ext[c2++];
        file_name[c1] = 0;
        file_name_length = c1;
        if (c1 > 1){
            file_name_length = c1;
            char aux[5];
            nfiles++;
            aux[0] = ':';
            aux[1] = (nfiles / (uint8_t)10) + 48;
            aux[2] = (nfiles % (uint8_t)10) + 48;
            aux[3] = ':';
            aux[4] = 0;
            uint8_t x = ((nfiles - (uint8_t)1) % (uint8_t)5) * (uint8_t)16;
            uint8_t y = ((nfiles - (uint8_t)1) / (uint8_t)5) * (uint8_t)8;
            cpc_PrintGphStrXYM1(aux, x , y);
            cpc_PrintGphStrXYM1(file_name, x + 4, y);
            uint8_t ci = 0;
            while(ci++ < file_name_length)
                if (file_name[ci] == ':')
                    file_name[ci] = '.';
            memcpy(disc_buf + ((nfiles - 1) * (uint8_t)13), file_name, file_name_length);
            disc_buf[12 + (nfiles - 1) * (uint8_t)13] = file_name_length;
        }
    }
 }
 void selectfile(){
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t x0 = 0;
    uint8_t y0 = 0;    
    uint8_t exit = 0;
    uint8_t sfile = 0;
    uint8_t *p = videotable[7];
    for (uint8_t i =0; i < 16; i++){
        *(p + i) = 0xf0;
    }
    while (!exit){
        cpc_ScanKeyboard();
        exit = cpc_TestKeyF(21);
        if (y > 0){
            y -= cpc_TestKeyF(17);
            while (cpc_TestKeyF(17)) cpc_ScanKeyboard();
        }
        if ((y < (nfiles / (uint8_t)5)) && (sfile < (nfiles - 5))){
            y += cpc_TestKeyF(18);
            while (cpc_TestKeyF(18)) cpc_ScanKeyboard();
        }
        if ((x < 4) && (sfile < (nfiles - 1))){
            x += cpc_TestKeyF(20);
            while (cpc_TestKeyF(20)) cpc_ScanKeyboard();
        }
        if (x > 0){
            x -= cpc_TestKeyF(19);
            while (cpc_TestKeyF(19)) cpc_ScanKeyboard();
        }
        if ((x != x0) || (y != y0)){
            p = videotable[(8 * (uint8_t)(y + 1)) - 1]; 
            for (uint8_t i =0; i < 16; i++){
                *(p + (x * (uint8_t)16) + i) = 0xf0;
            }
            p  = videotable[(8 * (uint8_t)(y0 + 1)) - 1];
            for (uint8_t i0 =0; i0 < 16; i0++){
                *(p + (x0 * (uint8_t)16) + i0) = 0;
            }
            x0 = x;
            y0 = y;
        }
        sfile = (y * (uint8_t)5) + x;
    }
    file_name_length = disc_buf[12 + (sfile * (uint8_t)13)];
    memcpy(file_name, disc_buf + (sfile * (uint8_t)13), file_name_length);
    file_name[file_name_length] = 0;
    cpc_PrintGphStrXYM1(file_name, 0, 14 * 8);
}
void confmenu(){
    uint8_t exit = 0;
        while (!exit){
            cpc_ScanKeyboard();
            if (cpc_TestKeyF(21)){
                exit = 1;
                while (cpc_TestKeyF(21)) cpc_ScanKeyboard();
            }
            if (background > 0){
                background -= cpc_TestKeyF(17);
                while (cpc_TestKeyF(17)) cpc_ScanKeyboard();
            }
            if (background < 27){
                background += cpc_TestKeyF(18);
                while (cpc_TestKeyF(18)) cpc_ScanKeyboard();
            }
            if (foreground < 27){
                foreground += cpc_TestKeyF(20);
                while (cpc_TestKeyF(20)) cpc_ScanKeyboard();
            }
            if (foreground > 0){
                foreground -= cpc_TestKeyF(19);
                while (cpc_TestKeyF(19)) cpc_ScanKeyboard();
            }
            if (border < 27){
                border += cpc_TestKeyF(1);
                while (cpc_TestKeyF(1)) cpc_ScanKeyboard();
            }
            if (border > 0){
                border -= cpc_TestKeyF(2);
                while (cpc_TestKeyF(2)) cpc_ScanKeyboard();
            }
            //border = foreground;
            cpc_SetMode(0);
            //cpc_SetColour(0 , cpct_getHWColour(background));
            for (uint8_t ind = 0; ind < (activepen + 1); ind++) cpc_SetColour(ind, cpct_getHWColour(background));
            cpc_SetColour(activepen + 1, cpct_getHWColour(foreground));
            for (uint8_t ind = (activepen + 2); ind < 16; ind++) cpc_SetColour(ind, cpct_getHWColour(background));
            cpc_SetColour(17 , cpct_getHWColour(border));
        }
    menuconf = 0;
}


