#pragma once
#include <stdint.h>

void halt();
void halt_all();

extern "C" {
    void i686_outb(uint16_t port, uint8_t value);
    uint8_t i686_inb(uint16_t port);
}

void i686_iowait();
void i686_sti();
void i686_cli();
