#pragma once
#include <stdint.h>

bool PS2_probe();
bool PS2_init();
uint8_t PS2_get_key();
char PS2_key_to_char(uint8_t key);

