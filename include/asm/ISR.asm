; for questions consult https://wiki.osdev.org/Interrupt_Descriptor_Table

; Interrupt Service Routines (ISRs) for CPU exceptions and hardware interrupts
; This file defines handlers for various interrupts, including CPU exceptions
; and hardware interrupts (IRQs). It also includes definitions of key terms.

; Definitions:
; 1. PIC (Programmable Interrupt Controller):
;    - A hardware component that manages and prioritizes hardware interrupts.
;    - It connects devices to the CPU and ensures interrupts are handled efficiently.
;    - In x86 systems, there are two PICs: Master PIC (handles IRQ0–IRQ7) and
;      Slave PIC (handles IRQ8–IRQ15), connected via IRQ2 (cascade).
;    - The PIC is typically remapped in protected mode to avoid conflicts with CPU exceptions.

; 2. IRQ (Interrupt Request):
;    - A signal sent by hardware devices to request the CPU's attention.
;    - Each IRQ is mapped to an interrupt vector in the IDT (Interrupt Descriptor Table).
;    - Examples: IRQ0 (Timer), IRQ1 (Keyboard), IRQ12 (Mouse).

; 3. IDT (Interrupt Descriptor Table):
;    - A data structure used by the CPU to map interrupt vectors to their corresponding
;      handlers (ISRs). It is used in protected mode to handle both hardware and software interrupts.

; 4. ISR (Interrupt Service Routine):
;    - A function or routine that handles a specific interrupt.
;    - When an interrupt occurs, the CPU jumps to the corresponding ISR.

; 5. Cascade Interrupt (IRQ2):
;    - Used to connect the Slave PIC to the Master PIC.
;    - Allows the Master PIC to delegate handling of IRQ8–IRQ15 to the Slave PIC.

; CPU Exception Handlers
global isr_divide_error
isr_divide_error: ; Interrupt 0x00 - Divide Error
    pusha
    cli 
    hlt
    ;if you divide by 0 the computer will halt
global isr_debug
isr_debug: ; Interrupt 0x01 - Debug Exception
    pusha
    ; Handle the debug exception here
    popa
    iret
global isr_nmi
isr_nmi: ; Interrupt 0x02 - Non-Maskable Interrupt
    pusha
    ; Handle the non-maskable interrupt here
    popa
    iret
global isr_breakpoint
isr_breakpoint: ; Interrupt 0x03 - Breakpoint Exception
    pusha
    ; Handle the breakpoint exception here
    popa
    iret
global isr_overflow
isr_overflow: ; Interrupt 0x04 - Overflow Exception
    pusha
    ; Handle the overflow exception here
    popa
    iret
global isr_bound_range_exceeded
isr_bound_range_exceeded: ; Interrupt 0x05 - Bound Range Exceeded
    pusha
    ; Handle the bound range exceeded exception here
    popa
    iret
global isr_invalid_opcode
isr_invalid_opcode: ; Interrupt 0x06 - Invalid Opcode Exception
    pusha
    ; Handle the invalid opcode exception here
    popa
    iret
global isr_device_not_available
isr_device_not_available: ; Interrupt 0x07 - Device Not Available Exception
    pusha
    ; Handle the device not available exception here
    popa
    iret
global isr_double_fault
isr_double_fault: ; Interrupt 0x08 - Double Fault Exception
    pusha
    ; Handle the double fault exception here
    popa
    iret
global isr_coprocessor_segment_overrun
isr_coprocessor_segment_overrun: ; Interrupt 0x09 - Coprocessor Segment Overrun
    pusha
    ; Handle the coprocessor segment overrun exception here
    popa
    iret
global isr_invalid_tss
isr_invalid_tss: ; Interrupt 0x0A - Invalid TSS Exception
    pusha
    ; Handle the invalid TSS exception here
    popa
    iret
global isr_segment_not_present
isr_segment_not_present: ; Interrupt 0x0B - Segment Not Present Exception
    pusha
    ; Handle the segment not present exception here
    popa
    iret
global isr_stack_segment_fault
isr_stack_segment_fault: ; Interrupt 0x0C - Stack Segment Fault
    pusha
    ; Handle the stack segment fault exception here
    popa
    iret
global isr_general_protection_fault
isr_general_protection_fault: ; Interrupt 0x0D - General Protection Fault
    pusha
    ; Handle the general protection fault here
    popa
    iret
global isr_page_fault
isr_page_fault: ; Interrupt 0x0E - Page Fault Exception
    pusha
    ; Handle the page fault exception here
    popa
    iret
; do not use int 0x0F as if is reserved for intel and should not be edited;
global isr_x87_fpu_error
isr_x87_fpu_error: ; Interrupt 0x10 - x87 FPU Error
    pusha
    ; Handle the x87 FPU error here
    popa
    iret
global isr_alignment_check
isr_alignment_check: ; Interrupt 0x11 - Alignment Check Exception
    pusha
    ; Handle the alignment check exception here
    popa
    iret
global isr_machine_check
isr_machine_check: ; Interrupt 0x12 - Machine Check Exception
    pusha
    ; Handle the machine check exception here
    popa
    iret
global isr_simd_fpu_error
isr_simd_fpu_error: ; Interrupt 0x13 - SIMD FPU Error
    pusha
    ; Handle the SIMD FPU error here
    popa
    iret
global isr_virtualization_exception
isr_virtualization_exception: ; Interrupt 0x14 - Virtualization Exception
    pusha
    ; Handle the virtualization exception here
    popa
    iret
global isr_security_exception
isr_security_exception: ; Interrupt 0x15 - Security Exception
    pusha
    ; Handle the security exception here
    popa
    iret

; Hardware Interrupt Handlers (IRQs)

; IRQ0 - Timer Interrupt
global isr_timer
isr_timer: ; Interrupt 0x20 - IRQ0 - Timer Interrupt
    pusha
    ; Handle the timer interrupt here
    popa
    iret

; IRQ1 - Keyboard Interrupt
global isr_keyboard
isr_keyboard: ; Interrupt 0x21 - IRQ1 - Keyboard Interrupt
    pusha
    ; Handle the keyboard interrupt here
    popa
    iret

; IRQ2 - Cascade Interrupt
global isr_cascade
isr_cascade: ; Interrupt 0x22 - IRQ2 - Cascade Interrupt
    pusha
    ; This interrupt is triggered when the Slave PIC signals the Master PIC.
    ; Typically used to handle IRQs 8–15 from the Slave PIC.
    popa
    iret

; IRQ3 - COM2 Interrupt
global isr_com2
isr_com2: ; Interrupt 0x23 - IRQ3 - COM2 Interrupt
    pusha
    ; Handle the COM2 (Serial Port) interrupt here
    popa
    iret

; IRQ4 - COM1 Interrupt
global isr_com1
isr_com1: ; Interrupt 0x24 - IRQ4 - COM1 Interrupt
    pusha
    ; Handle the COM1 (Serial Port) interrupt here
    popa
    iret

; IRQ5 - LPT2/Sound Card Interrupt
global isr_lpt2
isr_lpt2: ; Interrupt 0x25 - IRQ5 - LPT2/Sound Card Interrupt
    pusha
    ; Handle the LPT2 (Printer) or Sound Card interrupt here
    popa
    iret

; IRQ6 - Floppy Disk Interrupt
global isr_floppy
isr_floppy: ; Interrupt 0x26 - IRQ6 - Floppy Disk Interrupt
    pusha
    ; Handle the floppy disk controller interrupt here
    popa
    iret

; IRQ7 - LPT1 Interrupt
global isr_lpt1
isr_lpt1: ; Interrupt 0x27 - IRQ7 - LPT1 Interrupt
    pusha
    ; Handle the LPT1 (Printer) interrupt here
    popa
    iret

; IRQ8 - Real-Time Clock (RTC) Interrupt
global isr_rtc
isr_rtc: ; Interrupt 0x28 - IRQ8 - RTC Interrupt
    pusha
    ; Handle the Real-Time Clock (RTC) interrupt here
    popa
    iret

; IRQ9 - ACPI Interrupt
global isr_acpi
isr_acpi: ; Interrupt 0x29 - IRQ9 - ACPI Interrupt
    pusha
    ; Handle the ACPI (Advanced Configuration and Power Interface) interrupt here
    popa
    iret

; IRQ10 - General Use Interrupt
global isr_irq10
isr_irq10: ; Interrupt 0x2A - IRQ10 - General Use Interrupt
    pusha
    ; Handle the general-purpose interrupt for IRQ10 here
    popa
    iret

; IRQ11 - General Use Interrupt
global isr_irq11
isr_irq11: ; Interrupt 0x2B - IRQ11 - General Use Interrupt
    pusha
    ; Handle the general-purpose interrupt for IRQ11 here
    popa
    iret

; IRQ12 - PS/2 Mouse Interrupt
global isr_mouse
isr_mouse: ; Interrupt 0x2C - IRQ12 - PS/2 Mouse Interrupt
    pusha
    ; Handle the PS/2 mouse interrupt here
    popa
    iret

; IRQ13 - FPU Interrupt
global isr_fpu
isr_fpu: ; Interrupt 0x2D - IRQ13 - FPU Interrupt
    pusha
    ; Handle the Floating Point Unit (FPU) interrupt here
    popa
    iret

; IRQ14 - Primary ATA Interrupt
global isr_primary_ata
isr_primary_ata: ; Interrupt 0x2E - IRQ14 - Primary ATA Interrupt
    pusha
    ; Handle the primary ATA (hard disk) interrupt here
    popa
    iret

; IRQ15 - Secondary ATA Interrupt
global isr_secondary_ata
isr_secondary_ata: ; Interrupt 0x2F - IRQ15 - Secondary ATA Interrupt
    pusha
    ; Handle the secondary ATA (hard disk) interrupt here
    popa
    iret

; Reserved Interrupts
global isr_reserved
isr_reserved: ; Interrupts 0x30 - 0x3F - Reserved
    pusha
    ; These interrupts are reserved and should not be used.
    ; They may be used by future hardware or software standards.
    popa
    iret
; End of Interrupt Service Routines (ISRs)
; Note: Each ISR should be defined with the 'global' directive to make it accessible
; from other files, such as the IDT setup code.
; Ensure that each ISR ends with 'iret' to return from the interrupt properly.
; The 'pusha' instruction saves all general-purpose registers on the stack,
; and 'popa' restores them before returning from the interrupt.
; This allows the ISR to preserve the state of the CPU registers across the interrupt.
; The 'cli' instruction disables interrupts to prevent nested interrupts during critical sections.
; The 'hlt' instruction halts the CPU until the next interrupt occurs, which is useful
; in cases where the ISR cannot handle the interrupt immediately (e.g., divide by zero).
; The 'iret' instruction is used to return from the interrupt, restoring the CPU state
; from the stack and allowing the interrupted code to continue execution