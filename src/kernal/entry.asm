bits 32

extern kernal_main
global _start 
global _entry
global entry

extern __entry_start
extern __text_start
extern __data_start
extern __rodata_start
extern __bss_start

section .entry
entry:
_entry:
_start:
    jmp kernal_main
