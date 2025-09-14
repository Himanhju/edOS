[org 0x7C00]          ; Bootloader starts at 0x7C00

; --------- FAT32 Info ---------

BPB:
jmp short Code
nop
;    times 8 db ' '
;    dw 512          ; 512 byte per sector
;    db 8            ; 8 sectors per cluster
;    dw 260          ; start the FS at sector 260 (260 reserved sectors)
;    db 2            ; 2 FATs for backups
;    dw 0            ; 0 entries in the root dir (fat32 needs it to be 0)
;    dw 0            ; use the 32 bit version
;    db 0xf8         ; fixed drive
;    dw 0            ; sectors per fat (not for fat32)
;    dw 63           ; 63 sectors per track (i dont think its used)
;    dw 255          ; 255 sides/heads of the drive (also not used to my knowledge)
;    dd 0            ; this partition starts at 0
;    dd 0x200000     ; partition is 1 gb in size
;
;EBPB:
;    ; fat 32 extended boot record
;    dd 16385        ; 16385 sectors per FAT (formula is ((sectors in partition + 2) * 4) / sector size)
;    dw 0            ; set the flags to 0 so it writes to both fats
;    dw 0            ; version: 0.0
;    dd 2            ; root dir starts at cluster 2 (0 and 1 are reserved)
;    dw 257          ; the FSInfo Sector is 258 sectors into the partitions
;    dw 258          ; the Backup boot sector is 258 sectors into the partition
;    times 12 db 0   ; reserved
;    db 0x80         ; assume the device is a hard disk (will get replaced with the actual number at runtime)
;    db 0            ; reserved for windows NT
;    db 0x28         ; signature
;    dd 0            ; Serial number (currently ignored)
;    times 11 db ' ' ; padded with spaces
;    db "FAT32   "   ; string ID


times 127 - ($ - $$) db 0

; --------- Bootloader: Load kernel into memory ---------

Code:

mov [BPB + 0x40], dl  ; store the drive number to the ebpb

mov ah, 0x02          ; BIOS function: read sectors
mov al, 61            ; Read 61 sectors
mov ch, 0x00          ; Cylinder 0
mov cl, 0x02          ; Start reading from sector 2
mov dh, 0x00          ; Head 0
;mov dl, [BPB + 0x40] ; Drive (HDD or floppy)
mov bx, 0x7E0         ; Destination address for kernel (0x7E00)
mov es, bx            ; Set ES:BX = 0x07E0:0000
xor bx, bx            ; Clear BX, so ES:BX = 0x07E0:0000 = 0x7E00
int 0x13              ; BIOS interrupt to read sectors

mov ah, 0x02          ; BIOS function: read sectors
mov al, 127           ; Read 127 sectors
mov cl, 63            ; Start reading from sector 63
;mov dl, [BPB + 0x40] ; Drive (HDD or floppy)
mov bx, 0xf80         ; Destination address for kernel (0xf800)
mov es, bx            ; Set ES:BX = 0x0f80:0000
xor bx, bx            ; Clear BX, so ES:BX = 0x0f80:0000 = 0xf800
int 0x13              ; BIOS interrupt to read sectors again

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


;
;; ax = ah al
;; bx = bh bl
;; cx = ch cl
;; dx = dh dl