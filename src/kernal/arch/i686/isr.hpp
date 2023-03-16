#pragma once
#include <stdint.h>

#pragma pack(1)
struct ISR_Registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pusha
    uint32_t interrupt; // pushed by ISR stub
    uint32_t error; // pushed by CPU
    uint32_t eip, cs, eflags, user_esp, user_ss; // pushed by CPU
};

extern "C" void ISR_interrupt_service_routine(ISR_Registers *regs);
