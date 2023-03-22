// MM - Memory Manager

#pragma once
#include <stdint.h>

void MM_init();
void *MM_map(uint32_t size);
void *MM_map(uint32_t size, uint32_t alignement);
void MM_unmap(void *ptr);
uint32_t MM_get_bytes_used();
