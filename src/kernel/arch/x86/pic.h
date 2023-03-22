#pragma once
#include "instructions.h"

#define PIC_IRQ_OFFSET 0x20

bool PIC_probe();
void PIC_init(bool autoEOI);
void PIC_set_mask(uint16_t mask);
void PIC_disable();
void PIC_enable();
// disable interrupt request
void PIC_disbale_irq(int irq);
// enable interrupt request
void PIC_enable_irq(int irq);
//send end of interrupt
void PIC_send_eoi(int irq);
// read interrupt request register
uint16_t PIC_get_irr();
// read interrupt service register
uint16_t PIC_get_isr();
