#include <stdint.h>
#include "isr_stub.h"

#pragma pack(1)
struct IDT_Entry {
    uint16_t offset_low;
    uint16_t segment;
    uint8_t reserved;
    uint8_t flags;
    uint16_t offset_high;
};

#pragma pack(1)
struct IDT_Descriptor {
    uint16_t size;
    IDT_Entry *ptr;
};

typedef enum{
    IDT_FLAG_TASK = 0x05,
    IDT_FLAG_16BIT_INT = 0x06,
    IDT_FLAG_16BIT_TRAP = 0x07,
    IDT_FLAG_32BIT_INT = 0x0e,
    IDT_FLAG_32BIT_TRAP = 0x0f,
    IDT_FLAG_RING0 = 0x00,
    IDT_FLAG_RING1 = 0x20,
    IDT_FLAG_RING2 = 0x40,
    IDT_FLAG_RING3 = 0x60,
    IDT_FLAG_PRESENT = 0x80,
} IDT_FLAG;

IDT_Entry IDT_entry(void (*isr)(), uint16_t segment, uint8_t flags){
    IDT_Entry entry;
    uint32_t address =  (uint32_t)isr;
    entry.offset_low =  (address & 0x0000ffff) >> 0;
    entry.offset_high = (address & 0xffff0000) >> 16;
    entry.segment =     segment;
    entry.flags =       flags;
    entry.reserved =    0;
    return entry;
}

IDT_Entry idt_table[256];

IDT_Descriptor idt_descriptor;

extern "C" void x86_IDT_load(IDT_Descriptor *descriptor);

void IDT_init(){
    ISR_stub_table_init();

    for(int i = 0; i < 256; i++){
        idt_table[i] = IDT_entry((void(*)())isr_stub_table[i], 0x08, IDT_FLAG_PRESENT | IDT_FLAG_32BIT_INT | IDT_FLAG_RING0);
    }

    idt_descriptor.size = sizeof(IDT_Entry) * 256 - 1;
    idt_descriptor.ptr = idt_table;

    x86_IDT_load(&idt_descriptor);
}
