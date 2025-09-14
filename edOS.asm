[org 0x7E00]

[bits 16]

jmp short start

;%include "include/asm/IDT.asm"


start:
; Switch to protected mode

cli                     ; Clear interrupts

; Enable A20 Gate (Fast Method)
mov al, 0x02            ; Enable A20
out 0x92, al            ; Write to port 0x92


mov ax, 0xB800          ; Video memory segment
mov es, ax

lgdt [gdt_descriptor]   ; Load GDT

mov eax, cr0            ; Read control register 0
or eax, 1               ; Set the PE (Protection Enable) bit
mov cr0, eax            ; Write back to control register 0

jmp 0x08:protected_mode ; long jump to change segment

[bits 32]

protected_mode:
mov ax, 0x10            ; Kernel data segment selector
mov ds, ax              ; Set data segment
mov es, ax              ; Set extra segment
mov fs, ax              ; Set FS
mov gs, ax              ; Set GS
mov ss, ax              ; Set stack segment

nop

mov esp, 0x7C000        ; Set stack pointer to a safe location

jmp 0x08:0x8000

%include "include/asm/GDT.asm"

times 512 - ($ - $$) db 0 ; fill the rest of the mode switch sectors with zeroes