#include "ps2.h"
#include "instructions.hpp"

#define PS2_DATA_PORT 0x60
#define PS2_CMD_PORT 0x64

void PS2_wait_for_write(){
    uint8_t status = 0;
    do{
        status = i686_inb(PS2_CMD_PORT);
    }while(status & 0x02);
}

void PS2_wait_for_read(){
    uint8_t status = 0;
    do{
        status = i686_inb(PS2_CMD_PORT);
    }while(!(status & 0x01));
}

bool PS2_probe(){
    return true;
}

bool PS2_init(){
    //disable
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0xAD);
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0xA7);

    //flush output buffer
    i686_inb(PS2_DATA_PORT);

    //read controller configuration byte
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0x20);
    PS2_wait_for_read();
    uint8_t ccb = i686_inb(PS2_DATA_PORT);

    //write ccb back
    ccb &= ~(0b00100011);
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0x60);
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, ccb);

    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0xAA);
    PS2_wait_for_read();
    uint8_t test = i686_inb(PS2_DATA_PORT);
    if(test != 0x55){
        //test failed
        return false;
    }

    //enable
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0xAE);
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0xA8);

    //reset device
    PS2_wait_for_write();
    i686_outb(PS2_CMD_PORT, 0xFF);

    return true;
}

uint8_t PS2_get_key(){
    return i686_inb(PS2_DATA_PORT);
}

unsigned char ps2_keymap[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '+', /*'´' */0, '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '<',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '-',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,  '<',
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

char PS2_key_to_char(uint8_t key){
    return ps2_keymap[key & 0x7f];
}
