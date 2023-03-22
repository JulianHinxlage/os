#include "instructions.h"

void x86_hlt(){
    asm("hlt\n");
}

void halt_all(){
    while(true){
        asm(
            "cli\n"
            "hlt\n"
        );
    }
}

void x86_iowait(){
    // write to unused port
    x86_outb(0x80, 0);
}

void x86_sti(){
    asm("sti\n");
}

void x86_cli(){
    asm("cli\n");
}