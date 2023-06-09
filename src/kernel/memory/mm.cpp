#include "mm.h"
#include <stdint.h>

uint8_t *memory_stack_start;
uint8_t *memory_stack;
void *last_map_ptr = 0;

void MM_init(){
    memory_stack_start = (uint8_t*)0x100000;//1 MB
    memory_stack = memory_stack_start;
}

void *MM_map(uint32_t size){
    return MM_map(size, 1);
}

void *MM_map(uint32_t size, uint32_t alignement){
    memory_stack = (uint8_t*)(((((uint32_t)memory_stack - 1) / alignement) + 1) * alignement);
    void *ptr = memory_stack;
    memory_stack += size;
    last_map_ptr = ptr;
    return ptr;
}

void MM_unmap(void *ptr){
    if(ptr == last_map_ptr && ptr != 0){
        memory_stack = (uint8_t*)ptr;
    }
}

uint32_t MM_get_bytes_used(){
    return memory_stack - memory_stack_start;
}
