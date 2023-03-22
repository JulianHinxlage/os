#include "pic.h"

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

//initialisation word 1
typedef enum{
    PIC_ICW1_ICW4 = 0x01,
    PIC_ICW1_SINGLE = 0x02,
    PIC_ICW1_INTERVAL4 = 0x04,
    PIC_ICW1_LEVEL = 0x08,
    PIC_ICW1_INIT = 0x10,
} PIC_ICW1;

//initialisation word 4
typedef enum{
    PIC_ICW4_8086 = 0x01,
    PIC_ICW4_AUTO_EOI = 0x02,
    PIC_ICW4_BUFFERD_NON = 0x00,
    PIC_ICW4_BUFFERD_SLAVE = 0x08,
    PIC_ICW4_BUFFERD_MASTER = 0x0c,
    PIC_ICW4_FULLY_NESTED = 0x10,
    PIC_ICW4_INIT = 0x80,
} PIC_ICW4;

//command word
typedef enum{
    PIC_CMD_END_OF_INTERRUPT = 0x20,
    PIC_CMD_READ_IRR = 0x0A,
    PIC_CMD_READ_ISR = 0x0B,
}PIC_CMD;

uint16_t pic_mask = 0xffff;
uint16_t pic_cached_mask = 0xffff;

bool PIC_probe(){
    x86_outb(PIC2_DATA_PORT, 0xff);
    x86_outb(PIC1_DATA_PORT, 0xf5);
    bool found = x86_inb(PIC1_DATA_PORT) == 0xf5;
    x86_outb(PIC1_DATA_PORT, 0xff);
    return found;
}

void PIC_init(bool autoEOI){
    PIC_set_mask(0xffff);

    x86_outb(PIC1_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    x86_iowait();
    x86_outb(PIC2_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    x86_iowait();

    x86_outb(PIC1_DATA_PORT, PIC_IRQ_OFFSET);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, PIC_IRQ_OFFSET + 0x08);
    x86_iowait();

    x86_outb(PIC1_DATA_PORT, 0x04);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, 0x02);
    x86_iowait();

    uint8_t icw4 = PIC_ICW4_8086;
    if(autoEOI){
        icw4 |= PIC_ICW4_AUTO_EOI;
    }
    x86_outb(PIC1_DATA_PORT, icw4);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, icw4);
    x86_iowait();

    PIC_set_mask(0x0000);
    pic_mask = 0x0000;
    pic_cached_mask = 0x0000;
}

void PIC_set_mask(uint16_t mask) {
    pic_mask = mask;
    x86_outb(PIC1_DATA_PORT, (pic_mask & 0x00ff) >> 0);
    x86_iowait();
    x86_outb(PIC2_DATA_PORT, (pic_mask & 0xff00) >> 8);
    x86_iowait();
}

void PIC_disable() {
    pic_cached_mask = pic_mask;
    PIC_set_mask(0xffff);
}

void PIC_enable() {
    PIC_set_mask(pic_cached_mask);
}

void PIC_disbale_irq(int irq) {
    PIC_set_mask(pic_mask | (1 << irq));
}

void PIC_enable_irq(int irq) {
    PIC_set_mask(pic_mask & ~(1 << irq));
}

void PIC_send_eoi(int irq) {
    if(irq >= 8){
        x86_outb(PIC2_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
    }
    x86_outb(PIC1_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
}

uint16_t PIC_get_irr() {
    x86_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_IRR);
    x86_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_IRR);
    return (uint16_t)x86_inb(PIC1_COMMAND_PORT) | (uint16_t)x86_inb(PIC2_COMMAND_PORT) << 8;
}

uint16_t PIC_get_isr() {
    x86_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_ISR);
    x86_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_ISR);
    return (uint16_t)x86_inb(PIC1_COMMAND_PORT) | (uint16_t)x86_inb(PIC2_COMMAND_PORT) << 8;
}

