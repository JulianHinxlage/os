#pragma once
#include <stdint.h>

bool FAT_init(uint32_t drive);
void FAT_deinit();

bool FAT_read_file(const char *path, uint8_t *buffer, uint32_t bufferSize);
void FAT_write_file(const char *path, const uint8_t *buffer, uint32_t bufferSize);
uint32_t FAT_file_size(const char *path);

void FAT_create_directory(const char *path);
void FAT_list_directory(const char *path, uint8_t *buffer, uint32_t bufferSize);

void FAT_remove_file(const char *path);
void FAT_remove_directory(const char *path);
