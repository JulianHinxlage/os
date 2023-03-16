#include "vga.h"
#include "../arch/i686/instructions.hpp"

int width;
int height;
int cursor_pos;
uint8_t *video_memory;

//uint8_t inb(uint16_t port){
//    return i686_inb(port);
//}
//
//void outb(uint16_t port, uint8_t value){
//    i686_outb(port, value);
//}

void VGA_init() {
    width = 80;
    height = 25;
    cursor_pos = 0;
    video_memory = (uint8_t*)0xb8000;
}

void VGA_puts(const char *str) {
    while(*str){
        VGA_putc(*str);
        str++;
    }
    VGA_set_cursor(cursor_pos % width, cursor_pos / width);
}

void VGA_putc(char c){
    if(c == '\n'){
        cursor_pos = cursor_pos / width;
        cursor_pos++;
        cursor_pos *= width;
    }else{
        video_memory[cursor_pos * 2 + 0] = c;
        video_memory[cursor_pos * 2 + 1] = 0x0f;
        cursor_pos++;
    }

    if(cursor_pos >= width * height){
        VGA_clear();
    }

    VGA_set_cursor(cursor_pos % width, cursor_pos / width);
}

void VGA_clear() {
    cursor_pos = 0;
    for(int i = 0; i < width * height; i++){
            video_memory[cursor_pos * 2 + 0] = ' ';
            video_memory[cursor_pos * 2 + 1] = 0x0f;
            cursor_pos++;
    }
    cursor_pos = 0;
    VGA_set_cursor(0, 0);
}

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	i686_outb(0x3D4, 0x0A);
	i686_outb(0x3D5, (i686_inb(0x3D5) & 0xC0) | cursor_start);
 
	i686_outb(0x3D4, 0x0B);
	i686_outb(0x3D5, (i686_inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor() {
	i686_outb(0x3D4, 0x0A);
	i686_outb(0x3D5, 0x20);
}

void VGA_set_cursor(int x, int y) {
	uint16_t pos = y * width + x;
 
	i686_outb(0x3D4, 0x0F);
	i686_outb(0x3D5, (uint8_t) (pos & 0xFF));
	i686_outb(0x3D4, 0x0E);
	i686_outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
