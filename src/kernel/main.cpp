#include <stdint.h>
#include "arch/x86/instructions.h"
#include "io/vga.h"
#include "hal/hal.h"
#include "io/printf.h"

#include "driver/fat.h"
#include "memory/mm.h"
#include "memory/pt.h"
#include "util/memory.h"

extern uint8_t __entry_start;
extern uint8_t __text_start;
extern uint8_t __data_start;
extern uint8_t __rodata_start;
extern uint8_t __bss_start;
extern uint8_t __end;


extern "C" void kernel_main(uint8_t bootDrive){
    //initialize bss section
    for(int i = (int)&__bss_start; i < (int)&__end; i++){
        *((uint8_t*)(i)) = 0;
    }

    VGA_init();
    VGA_clear();

    printf("Kernel loaded (addr: 0x%x, size: %i byte)\n", (int)&__entry_start, (int)&__bss_start - (int)&__entry_start);
    printf("boot drive: 0x%x\n", (int)bootDrive);

    HAL_init();

    PageDirectory *pd = PT_create_directory();
    PT_create_identity_mapping(pd);
    PT_enable(pd);
    
    while(true){
        x86_hlt();
    }
}
