align 8

IDT: times 256 dq 0 ; Allocate space for 256 IDT entries (8 bytes each)

IDT_end: 

IDTDescriptor:
    dw IDT_end - IDT - 1 ; Limit (size of IDT - 1)
    dd IDT              ; Base address of the IDT

; Macro to define an IDT entry
%macro set_idt_entry 4
    ; %1 = Entry index
    ; %2 = Offset (handler address)
    ; %3 = Selector (code segment selector)
    ; %4 = Type and attributes (e.g., 0x8E for interrupt gate)
    mov eax, %2
    mov word [IDT + %1 * 8], ax          ; Offset bits 0-15
    mov word [IDT + %1 * 8 + 2], %3      ; Selector
    mov byte [IDT + %1 * 8 + 4], 0       ; Reserved
    mov byte [IDT + %1 * 8 + 5], %4      ; Type and attributes
    shr eax, 16
    mov word [IDT + %1 * 8 + 6], ax      ; Offset bits 16-31
%endmacro

; Example: Setting up an IDT entry for interrupt 0x20 (IRQ0 - Timer)
%include "include/asm/ISR.asm" ; Include ISR definitions

global setup_idt
setup_idt:
    ; Set up IDT entries here

    ; Interrupt 0x00, handler isr_divide_error
    set_idt_entry 0x00, isr_divide_error, 0x08, 0x8E
    ; Interrupt 0x01, handler isr_debug
    set_idt_entry 0x01, isr_debug, 0x08, 0x8E
    ; Interrupt 0x02, handler isr_nmi
    set_idt_entry 0x02, isr_nmi, 0x08, 0x8E
    ; Interrupt 0x03, handler isr_breakpoint
    set_idt_entry 0x03, isr_breakpoint, 0x08, 0x8E
    ; Interrupt 0x04, handler isr_overflow
    set_idt_entry 0x04, isr_overflow, 0x08, 0x8E
    ; Interrupt 0x05, handler isr_bound_range_exceeded
    set_idt_entry 0x05, isr_bound_range_exceeded, 0x08, 0x8E
    ; Interrupt 0x06, handler isr_invalid_opcode
    set_idt_entry 0x06, isr_invalid_opcode, 0x08, 0x8E
    ; Interrupt 0x07, handler isr_device_not_available
    set_idt_entry 0x07, isr_device_not_available, 0x08, 0x8E
    ; Interrupt 0x08, handler isr_double_fault
    set_idt_entry 0x08, isr_double_fault, 0x08, 0x8E
    ; Interrupt 0x09, handler isr_coprocessor_segment_overrun
    set_idt_entry 0x09, isr_coprocessor_segment_overrun, 0x08, 0x8E
    ; Interrupt 0x0A, handler isr_invalid_tss
    set_idt_entry 0x0A, isr_invalid_tss, 0x08, 0x8E
    ; Interrupt 0x0B, handler isr_segment_not_present
    set_idt_entry 0x0B, isr_segment_not_present, 0x08, 0x8E
    ; Interrupt 0x0C, handler isr_stack_segment_fault
    set_idt_entry 0x0C, isr_stack_segment_fault, 0x08, 0x8E
    ; Interrupt 0x0D, handler isr_general_protection_fault
    set_idt_entry 0x0D, isr_general_protection_fault, 0x08, 0x8E
    ; Interrupt 0x0E, handler isr_page_fault
    set_idt_entry 0x0E, isr_page_fault, 0x08, 0x8E
    ; Interrupt 0x10, handler isr_x87_fpu_error
    set_idt_entry 0x10, isr_x87_fpu_error, 0x08, 0x8E
    ; Interrupt 0x11, handler isr_alignment_check
    set_idt_entry 0x11, isr_alignment_check, 0x08, 0x8E
    ; Interrupt 0x12, handler isr_machine_check
    set_idt_entry 0x12, isr_machine_check, 0x08, 0x8E
    ; Interrupt 0x13, handler isr_simd_fpu_error
    set_idt_entry 0x13, isr_simd_fpu_error, 0x08, 0x8E
    ; Interrupt 0x14, handler isr_virtualization_exception
    set_idt_entry 0x14, isr_virtualization_exception, 0x08, 0x8E
    ; Interrupt 0x15, handler isr_security_exception
    set_idt_entry 0x15, isr_security_exception, 0x08, 0x8E

    ; Reserved entries (0x16 to 0x1F) are typically not used or reserved for future use
    set_idt_entry 0x16, isr_reserved, 0x08, 0x8E ; Interrupt 0x16, reserved
    set_idt_entry 0x17, isr_reserved, 0x08, 0x8E ; Interrupt 0x17, reserved
    set_idt_entry 0x18, isr_reserved, 0x08, 0x8E ; Interrupt 0x18, reserved
    set_idt_entry 0x19, isr_reserved, 0x08, 0x8E ; Interrupt 0x19, reserved
    set_idt_entry 0x1A, isr_reserved, 0x08, 0x8E ; Interrupt 0x1A, reserved
    set_idt_entry 0x1B, isr_reserved, 0x08, 0x8E ; Interrupt 0x1B, reserved
    set_idt_entry 0x1C, isr_reserved, 0x08, 0x8E ; Interrupt 0x1C, reserved
    set_idt_entry 0x1D, isr_reserved, 0x08, 0x8E ; Interrupt 0x1D, reserved
    set_idt_entry 0x1E, isr_reserved, 0x08, 0x8E ; Interrupt 0x1E, reserved
    set_idt_entry 0x1F, isr_reserved, 0x08, 0x8E ; Interrupt 0x1F, reserved

    ; Set up IRQs (Interrupt Requests)
    ; These are typically used for hardware interrupts
    ; IRQs are mapped to specific hardware devices, and they use the same IDT entry format
    ; IRQ0 - Timer
    set_idt_entry 0x20, isr_timer, 0x08, 0x8E
    ; IRQ1 - Keyboard
    set_idt_entry 0x21, isr_keyboard, 0x08, 0x8E
    ; IRQ2 - Cascade
    set_idt_entry 0x22, isr_cascade, 0x08, 0x8E
    ; IRQ3 - COM2
    set_idt_entry 0x23, isr_com2, 0x08, 0x8E
    ; IRQ4 - COM1
    set_idt_entry 0x24, isr_com1, 0x08, 0x8E
    ; IRQ5 - LPT2/Sound Card
    set_idt_entry 0x25, isr_lpt2, 0x08, 0x8E
    ; IRQ6 - Floppy Disk
    set_idt_entry 0x26, isr_floppy, 0x08, 0x8E
    ; IRQ7 - LPT1
    set_idt_entry 0x27, isr_lpt1, 0x08, 0x8E
    ; IRQ8 - RTC
    set_idt_entry 0x28, isr_rtc, 0x08, 0x8E
    ; IRQ9 - ACPI
    set_idt_entry 0x29, isr_acpi, 0x08, 0x8E
    ; IRQ10 - General Use
    set_idt_entry 0x2A, isr_irq10, 0x08, 0x8E
    ; IRQ11 - General Use
    set_idt_entry 0x2B, isr_irq11, 0x08, 0x8E
    ; IRQ12 - PS/2 Mouse
    set_idt_entry 0x2C, isr_mouse, 0x08, 0x8E
    ; IRQ13 - FPU
    set_idt_entry 0x2D, isr_fpu, 0x08, 0x8E
    ; IRQ14 - Primary ATA
    set_idt_entry 0x2E, isr_primary_ata, 0x08, 0x8E
    ; IRQ15 - Secondary ATA
    set_idt_entry 0x2F, isr_secondary_ata, 0x08, 0x8E

    ; Load the IDT
    lidt [IDTDescriptor]
    ret

; ISR definitions for the IDT entries
; These are the Interrupt Service Routines (ISRs) that will be called when an interrupt occurs.


;3rd Parameter (%3): Selector
;This specifies the code segment selector in the GDT (Global Descriptor Table) or LDT (Local Descriptor Table) that the CPU will use when handling the interrupt. Typically, this is the selector for the kernel's code segment (e.g., 0x08 in your code).

;4th Parameter (%4): Type and Attributes
;This specifies the type and attributes of the IDT entry. It defines the type of gate (e.g., interrupt gate, trap gate) and its privilege level. For example:
;
;0x8E represents an interrupt gate with DPL=0 (Descriptor Privilege Level 0), meaning it can only be accessed by kernel-level code.
;The breakdown of 0x8E:
;0x8: Indicates it's a 32-bit interrupt gate.
;E: Sets the present bit and privilege level.