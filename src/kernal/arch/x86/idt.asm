
global x86_IDT_load
x86_IDT_load:
    push ebp
    mov ebp, esp

    ; load idt
    mov bx, [esp + 8]
    lidt [bx]

    mov esp, ebp
    pop ebp
    ret