org 0x7c00
bits 16


; FAT HEADER
jmp short main
nop
bdb_oem:					db "MSWIN4.1"			; 8 byte
bdb_bytes_per_sector:		dw 512
bdb_sector_per_cluster: 	db 1
bdb_reserved_sectors: 		dw 1
bdb_fat_count: 				db 2
bdb_dir_entries_count:		dw 0E0h
bdb_total_sectors:			dw 2880
bdb_media_descriptor_type:	db 0F0h
bdb_sectors_per_fat:		dw 9
bdb_sectors_per_track:		dw 0x3f 				; 0x12 for floppy and 0x3f for disk
bdb_heads:					dw 0x10					; 0x02 for floppy and 0x10 for disk
bdb_hidden_sectors:			dd 0
bdb_large_sectors_count:	dd 0
ebr_drive_number:			db 0
							db 0 					; reserved
ebr_signature:				db 29h
ebr_volume_id:				db 01h, 02h, 03h, 04h	; 4 byte
ebr_volume_label:			db "OS         " 		; 11 byte
ebr_system_id:				db "FAT12   " 			; 8 byte

; GDT
gdt_begin:
gdt_null:
	dd 0
	dd 0
gdt_code:
	dw 0xffff
	dw 0
	db 0
	db 10011010b
	db 11001111b
	db 0
gdt_data:
	dw 0xffff
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0
gdt_end:
gdt_descriptor:
	dw gdt_end - gdt_begin - 1	; size
	dd gdt_begin				; ptr

seg_code equ gdt_code - gdt_begin
seg_data equ gdt_data - gdt_begin

drive_number: db 0
drive_heads: dw 0
drive_sectors_per_track: dw 0

halt:
    cli
    hlt
    jmp halt
    ret


main:
	; save boot brive from bios
	mov byte [drive_number], dl

	; init segment registers
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; init stack
	mov sp, 0xffff
	mov bp, sp

	call read_drive_parameters
    call load_stage2
    call switch_to_pmode

	bits 32
	call execute_stage_2
	bits 16
    call halt


; si: address of str to print
puts:
	mov bl, 0
	mov bh, 0
.loop:
	lodsb
	or al, al
	jz .done
	
	mov ah, 0Eh
	int 10h
	jmp .loop
.done:
	ret


read_drive_parameters:
	mov dl, byte [drive_number]
	mov ah, 08h
	stc
	int 13h
	jc .error

	mov byte [drive_heads], dh
	inc word [drive_heads]
	and cl, 0x3f
	mov byte [drive_sectors_per_track], cl

	mov ax, 0
	mov es, ax
	ret
.error:
    mov si, msg_read_error
	call halt


; ax: lba address
lba_to_chs:
	push ax
	push dx

	mov dx, 0
	div word [drive_sectors_per_track]
	inc dx
	mov cx, dx

	mov dx, 0
	div word [drive_heads]

	mov dh, dl
	mov ch, al
	shl ah, 6
	or cl, ah
	
	pop ax
	mov dl, al
	pop ax
	ret


; ax: lba address
; dl: drive
; cl: sectors to read  
; bx: destination address
read_disk:
	pusha
	push cx
	call lba_to_chs
	pop ax
	mov ah, 02h
	stc
	int 13h
	jc .error
	popa
	ret
.error:
    mov si, msg_read_error
    call puts 
	call halt


load_stage2:
	mov ch, 0
    mov di, stage2_sector_table
    mov bx, word [stage2_destination]

.loop:
    mov eax, dword [di]
    add di, 4
    mov cl, byte [di]
    add di, 1
    mov dl, byte [drive_number]

    cmp cl, 0
    je .skip_read

    call read_disk
    mov ch, 0
    imul cx, [bdb_bytes_per_sector]
    add bx, cx

.skip_read:
    cmp di, stage2_sector_table_end
    jb .loop
    ret


switch_to_pmode:
	cli

	; load GDT
	lgdt [gdt_descriptor]

	; set pmode bit
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	
	sti
	jmp seg_code:.pmode
	

bits 32
.pmode:
	; enable A20 Gate
	in al, 0x92
	or al, 2
	out 0x92, al

	mov ax, seg_data
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	ret


execute_stage_2:
	mov eax, 0
	mov ebx, 0
	mov ecx, 0
	mov edx, 0
	mov esi, 0
	mov edi, 0

	; push drive number on stack
	mov al, byte [drive_number]
	push eax

	mov ax, word [stage2_destination]
	call eax
	jmp halt


msg_read_error: db "Disk read failed", 0



sector_table_size equ 60

; padding
; =   512 	 (boot sector size)
;	- ($-$$) (size of code above)
;	- 2 	 (stage 2 destination address)
;	- 60 	 (size of sector table) 
;	- 2 	 (boot magic)
times 512-($-$$)-2-sector_table_size-2 db 0

stage2_destination: dw 0x0500

; table of sectors to load for stage 2 of bootloader
; stating at 32 bytes off the end of the boot sector (480, 0x1e0)
; array with 6 entries, each entry has:
;   4 byte LBA sector address
;   1 byte sector count
stage2_sector_table: times sector_table_size db 0
stage2_sector_table_end:

; boot sector magic end
dw 0xAA55
