[org 0x7E00]

section .text
[bits 32]

%include "include/asm/GDT.asm"
%include "include/asm/IDT.asm"

; Switch to protected mode
cli                     ; Clear interrupts

; Enable A20 Gate (Fast Method)
mov al, 0x02            ; Enable A20 using the fast method
out 0x92, al            ; Write to port 0x92

call load_gdt           ; Load the Global Descriptor Table (GDT)

mov eax, cr0            ; Read control register 0
or eax, 1               ; Set the PE (Protection Enable) bit
mov cr0, eax            ; Write back to control register 0

call setup_idt          ; Set up the IDT

mov ax, 0x10            ; Set the data segment selector (kernel data segment)
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

mov esp, 0x70000        ; Set the stack pointer to a safe location
sti                     ; Enable interrupts

jmp 0x08:0xA400         ; Jump to the kernel entry point (0xA400:0000)

; remember to update the mkbin function in ~/.bashrc

;kernel spreads from sector 21 to sector 120


times 10240 - ($ - $$) db 0 ; fill the rest of the mode switch sectors with zeroes
; replace with a jump to the kernel entry point when ready