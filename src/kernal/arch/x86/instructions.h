#pragma once
#include <stdint.h>

void x86_hlt();

extern "C" {
    void x86_outb(uint16_t port, uint8_t value);
    uint8_t x86_inb(uint16_t port);

    void x86_outw(uint16_t port, uint16_t value);
    uint16_t x86_inw(uint16_t port);
}

void x86_iowait();
void x86_sti();
void x86_cli();
