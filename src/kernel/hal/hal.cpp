#include "hal.h"
#include "io/printf.h"
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "arch/x86/pic.h"
#include "arch/x86/ps2.h"

#include "driver/ata.h"
#include "driver/fat.h"

#include "memory/mm.h"

void HAL_init_x86(){
    GDT_init();
    printf("GDT initialized\n");
    IDT_init();
    printf("IDT initialized\n");
    MM_init();
    printf("MM  initialized\n");

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

void HAL_init_drivers() {
    if(ATA_init()){
        printf("ATA initialized\n");
    }else{
        printf("ATA initialization failed\n");
    }

    if(FAT_init(0x80)){
        printf("FAT initialized\n");
    }else{
        printf("FAT initialization failed\n");
    }
}

void HAL_init(){
    HAL_init_x86();
    HAL_init_drivers();
}

void outb(uint16_t port, uint8_t value){
    x86_outb(port, value);
}

uint8_t inb(uint16_t port){
    return x86_inb(port);
}

void outw(uint16_t port, uint16_t value){
    x86_outw(port, value);
}

uint16_t inw(uint16_t port){
    return x86_inw(port);
}

void disk_read_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer){
    if(drive >= 0x80){
        ATA_read_sectors(drive - 0x80, sector, count, buffer);
    }else{
        printf("disk: drive 0x%x not supported\n");
    }
}

void disk_write_sectors(uint8_t drive, uint32_t sector, uint8_t count, uint8_t *buffer){
    if(drive >= 0x80){
        ATA_write_sectors(drive - 0x80, sector, count, buffer);
    }else{
        printf("disk: drive 0x%x not supported\n");
    }
}