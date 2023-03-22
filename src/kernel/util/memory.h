
#pragma once

#include <stdint.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

bool mem_cmp(const char *a, const char *b, int size);
bool str_cmp(const char *a, const char *b);
int mem_find(const char *mem, char search);
int mem_find(const char *mem, int size, char search);
int str_len(const char *str);

void memset(char *mem, int size, uint8_t value);

char to_upper(char c);
char to_lower(char c);
