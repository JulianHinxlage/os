global x86_PT_load
x86_PT_load:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    mov cr3, eax

    mov eax, cr4
    or eax, 0x00000010
    mov cr4, eax

    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    mov esp, ebp
    pop ebp
    ret
