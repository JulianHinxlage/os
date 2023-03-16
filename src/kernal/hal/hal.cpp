#include "hal.hpp"
#include "../io/printf.h"
#include "../arch/i686/gdt.hpp"
#include "../arch/i686/idt.hpp"
#include "../arch/i686/pic.hpp"
#include "../arch/i686/ps2.h"

void HAL_init_i686(){
    GDT_init();
    printf("GDT initialized\n");
    IDT_init();
    printf("IDT initialized\n");
    if(PIC_probe()){
        PIC_init(false);
        printf("PIC initialized\n");
    }else{
        printf("PIC not found\n");
    }

    if(!PS2_init()){
        printf("PS2 initialization failed\n");
    }else{
        printf("PS2 initialized\n");
    }
}

void HAL_init(){
    HAL_init_i686();
}
