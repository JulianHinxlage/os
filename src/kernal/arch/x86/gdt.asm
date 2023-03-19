
global x86_GDT_load
x86_GDT_load:
    push ebp
    mov ebp, esp

    ; load gdt
    mov bx, [esp + 8]
    lgdt [bx]

    ; load data segments
    mov ax, [esp + 12]
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
    
    ; load code segment
    mov eax, [esp + 16]
    push eax
    push .done
    retf ; hack to perform long jump to value on stack

.done:
    mov esp, ebp
    pop ebp
    ret