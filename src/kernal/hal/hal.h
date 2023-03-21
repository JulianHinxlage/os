
//HAL - Hardware Abstraction Layer

#pragma once
#include <stdint.h>

void HAL_init();

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);

void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);

void disk_read_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);

void disk_write_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);
