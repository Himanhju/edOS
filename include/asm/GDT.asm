; Global Descriptor Table (GDT) definition
; File: /home/himanhju/Code/OS/C/casm/edOS/include/GDT.asm

[bits 16]
;section .data
align 8
gdt_start:
    ; Null Descriptor (mandatory)
    dd 0x0                ; Null base
    dd 0x0                ; Null limit

    ; Kernel Code Segment Descriptor
    dw 0xFFFF             ; Limit (16 bits)
    dw 0x0000             ; Base (16 bits)
    db 0x00               ; Base (8 bits)
    db 0x9A               ; Access byte: 1 00 1 1 0 1 0 (present, ring 0, executable, readable)
    db 0xCF               ; Flags: 1100 1111 (4-bit limit, granularity, 32-bit)
    db 0x00               ; Base (8 bits)

    ; Kernel Data Segment Descriptor
    dw 0xFFFF             ; Limit (16 bits)
    dw 0x0000             ; Base (16 bits)
    db 0x00               ; Base (8 bits)
    db 0x92               ; Access byte: 1 00 1 0 0 1 0 (present, ring 0, writable)
    db 0xCF               ; Flags: 1100 1111 (4-bit limit, granularity, 32-bit)
    db 0x00               ; Base (8 bits)

    ; User Code Segment Descriptor
    dw 0xFFFF             ; Limit (16 bits)
    dw 0x0000             ; Base (16 bits)
    db 0x00               ; Base (8 bits)
    db 0xFA               ; Access byte: 1 11 1 1 0 1 0 (present, ring 3, executable, readable)
    db 0xCF               ; Flags: 1100 1111 (4-bit limit, granularity, 32-bit)
    db 0x00               ; Base (8 bits)

    ; User Data Segment Descriptor
    dw 0xFFFF             ; Limit (16 bits)
    dw 0x0000             ; Base (16 bits)
    db 0x00               ; Base (8 bits)
    db 0xF2               ; Access byte: 1 11 1 0 0 1 0 (present, ring 3, writable)
    db 0xCF               ; Flags: 1100 1111 (4-bit limit, granularity, 32-bit)
    db 0x00               ; Base (8 bits)

gdt_end:

;Segment Size = (Limit + 1) × Granularity (4096)

; GDT Descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Size of GDT (limit)
    dd gdt_start              ; Address of GDT (base)