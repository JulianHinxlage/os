#include "panic.h"
#include "instructions.h"
#include "io/printf.h"

const char *exceptions_names[32] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
};

void kernal_panic(ISR_Registers *regs){
        //kernal panic
    printf("##### Kernal Panic #####\n");
    printf("Interrupt: 0x%x\n", (int)regs->interrupt);

    if(regs->interrupt <= 31){
        const char *name = exceptions_names[regs->interrupt];
        printf("Exception: ");
        printf(name);
        printf("\n");
    }

    printf("eax=0x%x, ebx=0x%x, ecx=0x%x, edx=0x%x\n", (int)regs->eax, (int)regs->ebx, (int)regs->ecx, (int)regs->edx);
    printf("esi=0x%x, edi=0x%x, esp=0x%x, ebp=0x%x\n", (int)regs->esi, (int)regs->edi, (int)regs->esp, (int)regs->ebp);
    printf("ds=0x%x, eip=0x%x, cs=0x%x, eflags=0x%x\n", (int)regs->ds, (int)regs->eip, (int)regs->cs, (int)regs->eflags);
    printf("error=0x%x, user_esp=0x%x, user_ss=0x%x\n", (int)regs->error, (int)regs->user_esp, (int)regs->user_ss);

    x86_cli();
    x86_hlt();
}
