#pragma once
#include <stdint.h>

bool ATA_init();

void ATA_read_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);

void ATA_write_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer);

void ATA_interrupt(int irq);
