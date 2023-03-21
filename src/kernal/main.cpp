#include <stdint.h>
#include "arch/x86/instructions.h"
#include "io/vga.h"
#include "hal/hal.h"
#include "io/printf.h"

#include "driver/fat.h"
#include "memory/mm.h"
#include "util/memory.h"

extern uint8_t __entry_start;
extern uint8_t __text_start;
extern uint8_t __data_start;
extern uint8_t __rodata_start;
extern uint8_t __bss_start;
extern uint8_t __end;


extern "C" void kernal_main(uint8_t bootDrive){
    //initialize bss section
    for(int i = (int)&__bss_start; i < (int)&__end; i++){
        *((uint8_t*)(i)) = 0;
    }

    VGA_init();
    VGA_clear();

    printf("Kernal loaded (addr: 0x%x, size: %i byte)\n", (int)&__entry_start, (int)&__bss_start - (int)&__entry_start);
    printf("boot drive: 0x%x\n", (int)bootDrive);
    printf("kernal in memory size: %i bytes\n", (int)&__end - (int)&__entry_start);

    HAL_init();

    int buffer_size = 1024 * 30;
    char *buffer = (char*)MM_map(buffer_size);

    if(FAT_read_file("kernal.bin", (uint8_t*)buffer, buffer_size)){
        int start = 0x3620;
        for(int i = start; i < start + 200; i++){
            printf("%c", buffer[i]);
        }
        printf("\n");
    }else{
        printf("file not found\n");
    }

    if(FAT_read_file("test2/test123456789.txt", (uint8_t*)buffer, buffer_size)){
        printf("file: %s\n", buffer);
    }else{
        printf("file not found\n");
    }

    MM_unmap(buffer);
    buffer = 0;
    buffer_size = 0;

    printf("memory usage: %i bytes\n", ((int)&__end - (int)&__entry_start) + MM_get_bytes_used());


    while(true){
        x86_hlt();
    }
}
