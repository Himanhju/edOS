[org 0x7C00]          ; Bootloader starts at 0x7C00


; --------- Bootloader: Load kernel into memory ---------

mov ah, 0x02          ; BIOS function: read sectors
mov al, 127           ; Read 127 sectors
mov ch, 0x00          ; Cylinder 0
mov cl, 0x02          ; Start reading from sector 2
mov dh, 0x00          ; Head 0
mov dl, 0x80          ; Drive (HDD or floppy)
mov bx, 0x7E0         ; Destination address for kernel (0x7E00)
mov es, bx            ; Set ES:BX = 0x7E00:0000
xor bx, bx            ; Clear BX, so ES:BX = 0x7E00:0000 = 0x7E00
int 0x13              ; BIOS interrupt to read sectors

;mov ah, 0x02          ; BIOS function: read sectors
;mov al, 127           ; Read 127 sectors
;mov ch, 0x00          ; Cylinder 0
;mov cl, 0x81          ; Start reading from sector 129
;mov dh, 0x00          ; Head 0
;mov dl, 0x80          ; Drive (HDD or floppy)
;mov bx, 0x17C0        ; Destination address for kernel (0x17C00)
;mov es, bx            ; Set ES:BX = 0x7E00:0000
;xor bx, bx            ; Clear BX, so ES:BX = 0x7E00:0000 = 0x7E00
;int 0x13              ; BIOS interrupt to read sectors

; --------- Setup Stack ---------

cli                   ; Disable interrupts
mov ax, 0x0E00        ; Stack segment at 0xE000
mov ss, ax
mov sp, 0xFFFF        ; Stack pointer at top of segment (0xFFFF)
sti                   ; Enable interrupts

; --------- Set Video Mode ---------

vid:
; Set VBE mode 0x118 with linear framebuffer (bit 14 = LFB)
mov ax, 0x4F02          ; VESA function: Set video mode
mov bx, 0x4118          ; Mode 0x118 | 0x4000 (bit 14 enables LFB)
int 0x10

test ah, ah
jnz vid             ; Check for errors

; --------- Get the vram address ---------

mov ax, 0x4F01            ; VESA function - Get Mode Info
mov cx, 0x118             ; Mode number (for 1024x768x32bpp)
mov di, 0x0028            ; Offset of buffer
mov bx, 0x0500
mov es, bx            ; Segment of buffer (buffer at 0x5000)
int 0x10

; --------- Jump to kernel ---------

jmp 0x7E00            ; Jump to kernel (address 0x7E00)

; --------- Bootloader padding and signature ---------
times 510 - ($ - $$) db 0   ; Fill the rest of the bootloader with zeroes
dw 0xAA55                   ; Bootloader signature (required by BIOS)

;cli                 ; Disable interrupts
;mov ax, cs          ; Get current code segment
;mov [0x0000 + 4*0x27 + 2], ax     ; Set segment part of int 0x27
;mov word [0x0000 + 4*0x27], h27 ; Set offset
;sti                 ; Enable interrupts
;
;
;mov ax, 'h'
;int 0x27 ;prints h
;
;jmp $
;
;
;h27:
;    mov ah, 0x0E
;    int 0x10
;    iret
;
;; --------- Bootloader padding and signature ---------
;times 510 - ($ - $$) db 0   ; Fill the rest of the bootloader with zeroes
;dw 0xAA55                   ; Bootloader signature (required by BIOS)
;
;; ax = ah al
;; bx = bh bl
;; cx = ch cl
;; dx = dh dl
