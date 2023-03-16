#include "instructions.hpp"

void halt(){
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

void i686_iowait(){
    // write to unused port
    i686_outb(0x80, 0);
}

void i686_sti(){
    asm("sti\n");
}

void i686_cli(){
    asm("cli\n");
}