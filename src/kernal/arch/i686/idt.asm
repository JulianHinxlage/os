
global i686_IDT_load
i686_IDT_load:
    push ebp
    mov ebp, esp

    ; load idt
    mov bx, [esp + 8]
    lidt [bx]

    mov esp, ebp
    pop ebp
    ret