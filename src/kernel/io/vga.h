#pragma once

#include <stdint.h>

void VGA_init();
void VGA_puts(const char *str);
void VGA_putc(char c);
void VGA_clear();
void VGA_set_cursor(int x, int y);
