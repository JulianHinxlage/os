#include "isr.hpp"
#include "../../io/printf.h"
#include "../../io/vga.h"
#include "panic.h"
#include "pic.hpp"
#include "ps2.h"

void ISR_interrupt_service_routine(ISR_Registers *regs){
    if(regs->interrupt >= PIC_IRQ_OFFSET && regs->interrupt < PIC_IRQ_OFFSET + 0x10){
        //hardware interrupt
        int irq = regs->interrupt - PIC_IRQ_OFFSET;
        if(irq == 0){
            //timer
            //printf(".");
        }
        else if(irq == 1){
            //keyboard
            int value = PS2_get_key();
            if(value & 0x80){
                //released
            }else {
                //pressed
                VGA_putc(PS2_key_to_char(value));
            }

            //printf("Key: %i", PS2_get_key());
        }
        else{
            //other
            printf("IRQ: %i\n", irq);
        }

        PIC_send_eoi(irq);
        return;
    }else if(regs->interrupt <= 0x20){
        //exception
        kernal_panic(regs);
    }else{
        //software interrupt
        printf("ISR: 0x%x\n", regs->interrupt);
        return;
    }
}
