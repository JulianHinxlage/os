#include "vga.h"
#include <stdarg.h>
#include <stdbool.h>

void printNum(int num, int base){
    char buf[64];
    buf[63] = '\0';
    char *str = &buf[63];

    while(num > 0){
        uint8_t digit = num % base;
        num /= base;
        char c = "0123456789abcdef"[digit % 16];
        str--;
        *str = c;
    }

    if(str == &buf[63]){
        str--;
        *str = '0';
    }

    VGA_puts(str);
}

void printf(const char *fmt, ...){
    va_list args;
    va_start(args, fmt);

    bool escaped = false;
    while(*fmt){
        char c = *fmt;
        if(escaped){
            if(c == '%'){
                VGA_putc(c);
            }else if(c == 'i'){
                printNum(va_arg(args, int), 10);
            }else if(c == 'c'){
                VGA_putc((char)va_arg(args, int));
            }else if(c == 'x'){
                printNum(va_arg(args, int), 16);
            }else if(c == 's'){
                const char *str = va_arg(args, char*);
                VGA_puts(str);
            }
            escaped = false;
        }else if(c == '%'){
            escaped = true;
        }else{
            VGA_putc(c);
        }
        fmt++;
    }

    va_end(args);
}