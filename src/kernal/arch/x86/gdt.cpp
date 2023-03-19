#include <stdint.h>

#pragma pack(1)
struct GDT_Entry{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags_and_limit_high;
    uint8_t base_high;
};

#pragma pack(1)
struct GDT_Descriptor{
    uint16_t size;
    GDT_Entry *ptr;
};

typedef enum{
    GDT_ACCESS_ACCESSED = 0x01,
    GDT_ACCESS_DATA_WRITABLE = 0x02,
    GDT_ACCESS_CODE_READABLE = 0x02,
    GDT_ACCESS_DATA_DIRECTION_DOWN = 0x04,
    GDT_ACCESS_CODE_EXECUTABLE_FROM_LOWER_RING = 0x04,
    GDT_ACCESS_CODE_SEGMENT = 0x18,
    GDT_ACCESS_DATA_SEGMENT = 0x10,
    GDT_ACCESS_SYSTEM_SEGMENT = 0x00,
    GDT_ACCESS_RING0 = 0x00,
    GDT_ACCESS_RING1 = 0x20,
    GDT_ACCESS_RING2 = 0x40,
    GDT_ACCESS_RING3 = 0x60,
    GDT_ACCESS_PRESENT = 0x80,
} GDT_ACCESS;

typedef enum{
    GDT_FLAG_16BIT = 0x00,
    GDT_FLAG_32BIT = 0x04,
    GDT_FLAG_64BIT = 0x02,
    GDT_FLAG_GRANULARITY_1B = 0x00,
    GDT_FLAG_GRANULARITY_4KB = 0x08,
} GDT_FLAG;

GDT_Entry GDT_entry(uint32_t base, uint32_t limit, uint8_t flags, uint8_t access){
    GDT_Entry entry;
    entry.base_low =    (base  & 0x0000ffff) >> 0;
    entry.base_middle = (base  & 0x00ff0000) >> 16;
    entry.base_high =   (base  & 0xff000000) >> 24;
    entry.limit_low =   (limit & 0x0000ffff) >> 0;
    entry.flags_and_limit_high =    (limit & 0x000f0000) >> 16;
    entry.flags_and_limit_high |=   (flags & 0x0f) << 4;
    entry.access = access;
    return entry;
}

GDT_Entry gdt_table[16];

GDT_Descriptor gdt_descriptor;

extern "C" void x86_GDT_load(GDT_Descriptor *descriptor, uint8_t dataSegemnt, uint8_t codeSegment);

void GDT_init(){
    gdt_table[0] = GDT_entry(0, 0, 0, 0),

    gdt_table[1] = GDT_entry(0, 0x000fffff, 
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4KB, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE),

    gdt_table[2] = GDT_entry(0, 0x000fffff, 
        GDT_FLAG_32BIT | GDT_FLAG_GRANULARITY_4KB, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE),

    gdt_descriptor.size = sizeof(GDT_Entry) * 3 - 1;
    gdt_descriptor.ptr = gdt_table;

    x86_GDT_load(&gdt_descriptor, 2 * sizeof(GDT_Entry), 1 * sizeof(GDT_Entry));
}
