#include "vga.h"
#include "arch/x86/instructions.h"

int width;
int height;
int cursor_pos;
uint8_t *video_memory;

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

void VGA_move_up(){
    cursor_pos = 0;
    for(int i = 0; i < height - 1; i++){
        for(int j = 0; j <width; j++){
            video_memory[cursor_pos * 2 + 0] = video_memory[(cursor_pos + width) * 2 + 0];
            video_memory[cursor_pos * 2 + 1] = video_memory[(cursor_pos + width) * 2 + 1];
            cursor_pos++;
        }
    }
    for(int j = 0; j < width; j++){
        video_memory[cursor_pos * 2 + 0] = 0;
        video_memory[cursor_pos * 2 + 1] = 0;
        cursor_pos++;
    }
    cursor_pos = width * (height - 1);
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
        VGA_move_up();
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
	x86_outb(0x3D4, 0x0A);
	x86_outb(0x3D5, (x86_inb(0x3D5) & 0xC0) | cursor_start);
 
	x86_outb(0x3D4, 0x0B);
	x86_outb(0x3D5, (x86_inb(0x3D5) & 0xE0) | cursor_end);
}

void disable_cursor() {
	x86_outb(0x3D4, 0x0A);
	x86_outb(0x3D5, 0x20);
}

void VGA_set_cursor(int x, int y) {
	uint16_t pos = y * width + x;
 
	x86_outb(0x3D4, 0x0F);
	x86_outb(0x3D5, (uint8_t) (pos & 0xFF));
	x86_outb(0x3D4, 0x0E);
	x86_outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
