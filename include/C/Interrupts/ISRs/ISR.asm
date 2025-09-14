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
; Add Handlers
extern ISR_Handler_divide_error ; Handler - Interrupt 0x00 - Divide Error
    
    

extern ISR_Handler_debug ; Handler - Interrupt 0x01 - Debug Exception
    
    
    
extern ISR_Handler_nmi ; Handler - Interrupt 0x02 - Non-Maskable Interrupt
    
    
    
extern ISR_Handler_breakpoint ; Handler - Interrupt 0x03 - Breakpoint Exception
    
    
extern ISR_Handler_overflow ; Handler - Interrupt 0x04 - Overflow Exception
    
    
    
extern ISR_Handler_bound_range_exceeded ; Handler - Interrupt 0x05 - Bound Range Exceeded
    
    
    
extern ISR_Handler_invalid_opcode ; Handler - Interrupt 0x06 - Invalid Opcode Exception
    
    
extern ISR_Handler_device_not_available ; Handler - Interrupt 0x07 - Device Not Available Exception
    
    
extern ISR_Handler_double_fault ; Handler - Interrupt 0x08 - Double Fault Exception
    
    
    
extern ISR_Handler_coprocessor_segment_overrun ; Handler - Interrupt 0x09 - Coprocessor Segment Overrun
    
    
    
extern ISR_Handler_invalid_tss ; Handler - Interrupt 0x0A - Invalid TSS Exception
    
    
    
extern ISR_Handler_segment_not_present ; Handler - Interrupt 0x0B - Segment Not Present Exception
    
    
extern ISR_Handler_stack_segment_fault ; Handler - Interrupt 0x0C - Stack Segment Fault
    
    
    
extern ISR_Handler_general_protection_fault ; Handler - Interrupt 0x0D - General Protection Fault
    
    
    
extern ISR_Handler_page_fault ; Handler - Interrupt 0x0E - Page Fault Exception
    
    
; Handler - do not use int 0x0F as if is reserved for intel and should not be edited
extern ISR_Handler_x87_fpu_error ; Handler - Interrupt 0x10 - x87 FPU Error
    
    
    
extern ISR_Handler_alignment_check ; Handler - Interrupt 0x11 - Alignment Check Exception

    
    
extern ISR_Handler_machine_check ; Handler - Interrupt 0x12 - Machine Check Exception
    

    
extern ISR_Handler_simd_fpu_error ; Handler - Interrupt 0x13 - SIMD FPU Error
    
    
    
extern ISR_Handler_virtualization_exception ; Handler - Interrupt 0x14 - Virtualization Exception
    
    
    
extern ISR_Handler_security_exception ; Handler - Interrupt 0x15 - Security Exception
    
    
    

; Handler - Hardware Interrupt Handlers (IRQs)
; Handler - IRQ0 - Timer Interrupt
extern ISR_Handler_timer ; Handler - Interrupt 0x20 - IRQ0 - Timer Interrupt
    
    
    

; Handler - IRQ1 - Keyboard Interrupt
extern ISR_Handler_keyboard ; Handler - Interrupt 0x21 - IRQ1 - Keyboard Interrupt
    
    
    

; Handler - IRQ2 - Cascade Interrupt
extern ISR_Handler_cascade ; Handler - Interrupt 0x22 - IRQ2 - Cascade Interrupt
    
    
    

; Handler - IRQ3 - COM2 Interrupt
extern ISR_Handler_com2 ; Handler - Interrupt 0x23 - IRQ3 - COM2 Interrupt
    
    
    

; Handler - IRQ4 - COM1 Interrupt
extern ISR_Handler_com1 ; Handler - Interrupt 0x24 - IRQ4 - COM1 Interrupt
    
    
    

; Handler - IRQ5 - LPT2/Sound Card Interrupt
extern ISR_Handler_lpt2 ; Handler - Interrupt 0x25 - IRQ5 - LPT2/Sound Card Interrupt
    
    
    

; Handler - IRQ6 - Floppy Disk Interrupt
extern ISR_Handler_floppy ; Handler - Interrupt 0x26 - IRQ6 - Floppy Disk Interrupt
    
    
    

; Handler - IRQ7 - LPT1 Interrupt
extern ISR_Handler_lpt1 ; Handler - Interrupt 0x27 - IRQ7 - LPT1 Interrupt
    
    
    

; Handler - IRQ8 - Real-Time Clock (RTC) Interrupt
extern ISR_Handler_rtc ; Handler - Interrupt 0x28 - IRQ8 - RTC Interrupt
    
    
    

; Handler - IRQ9 - ACPI Interrupt
extern ISR_Handler_acpi ; Handler - Interrupt 0x29 - IRQ9 - ACPI Interrupt
    
    
    

; Handler - IRQ10 - General Use Interrupt
extern ISR_Handler_irq10 ; Handler - Interrupt 0x2A - IRQ10 - General Use Interrupt
    
    
    

; Handler - IRQ11 - General Use Interrupt
extern ISR_Handler_irq11 ; Handler - Interrupt 0x2B - IRQ11 - General Use Interrupt
    
    
    

; Handler - IRQ12 - PS/2 Mouse Interrupt
extern ISR_Handler_mouse ; Handler - Interrupt 0x2C - IRQ12 - PS/2 Mouse Interrupt
    
    
    

; Handler - IRQ13 - FPU Interrupt
extern ISR_Handler_fpu ; Handler - Interrupt 0x2D - IRQ13 - FPU Interrupt
    
    
    

; Handler - IRQ14 - Primary ATA Interrupt
extern ISR_Handler_primary_ata ; Handler - Interrupt 0x2E - IRQ14 - Primary ATA Interrupt
    
    
    

; Handler - IRQ15 - Secondary ATA Interrupt
extern ISR_Handler_secondary_ata ; Handler - Interrupt 0x2F - IRQ15 - Secondary ATA Interrupt
    
    
    

; Handler - IRQ15 - Secondary ATA Interrupt
extern ISR_Handler_syscall ; Handler - Interrupt 0x80 - Syscall Interrupt
    

; CPU Exceptions
global ISR_divide_error
ISR_divide_error: ; Interrupt 0x00 - Divide Error
    pusha
    ; Handle the divide error here
    call ISR_Handler_divide_error
    popa
    iret
global ISR_debug
ISR_debug: ; Interrupt 0x01 - Debug Exception
    pusha
    ; Handle the debug exception here
    popa
    iret
global ISR_nmi
ISR_nmi: ; Interrupt 0x02 - Non-Maskable Interrupt
    pusha
    ; Handle the non-maskable interrupt here
    popa
    iret
global ISR_breakpoint
ISR_breakpoint: ; Interrupt 0x03 - Breakpoint Exception
    pusha
    ; Handle the breakpoint exception here
    popa
    iret
global ISR_overflow
ISR_overflow: ; Interrupt 0x04 - Overflow Exception
    pusha
    ; Handle the overflow exception here
    popa
    iret
global ISR_bound_range_exceeded
ISR_bound_range_exceeded: ; Interrupt 0x05 - Bound Range Exceeded
    pusha
    ; Handle the bound range exceeded exception here
    popa
    iret
global ISR_invalid_opcode
ISR_invalid_opcode: ; Interrupt 0x06 - Invalid Opcode Exception
    pusha
    ; Handle the invalid opcode exception here
    call ISR_Handler_invalid_opcode
    popa
    iret
global ISR_device_not_available
ISR_device_not_available: ; Interrupt 0x07 - Device Not Available Exception
    pusha
    ; Handle the device not available exception here
    popa
    iret
global ISR_double_fault
ISR_double_fault: ; Interrupt 0x08 - Double Fault Exception
    pusha
    ; Handle the double fault exception here
    call ISR_Handler_double_fault
    popa
    iret
global ISR_coprocessor_segment_overrun
ISR_coprocessor_segment_overrun: ; Interrupt 0x09 - Coprocessor Segment Overrun
    pusha
    ; Handle the coprocessor segment overrun exception here
    popa
    iret
global ISR_invalid_tss
ISR_invalid_tss: ; Interrupt 0x0A - Invalid TSS Exception
    pusha
    ; Handle the invalid TSS exception here
    popa
    iret
global ISR_segment_not_present
ISR_segment_not_present: ; Interrupt 0x0B - Segment Not Present Exception
    pusha
    ; Handle the segment not present exception here
    popa
    iret
global ISR_stack_segment_fault
ISR_stack_segment_fault: ; Interrupt 0x0C - Stack Segment Fault
    pusha
    ; Handle the stack segment fault exception here
    popa
    iret
global ISR_general_protection_fault
ISR_general_protection_fault: ; Interrupt 0x0D - General Protection Fault
    pusha
    ; Handle the general protection fault here
    popa
    iret
global ISR_page_fault
ISR_page_fault: ; Interrupt 0x0E - Page Fault Exception
    pusha
    ; Handle the page fault exception here
    popa
    iret
; do not use int 0x0F as if is reserved for intel and should not be edited;
global ISR_x87_fpu_error
ISR_x87_fpu_error: ; Interrupt 0x10 - x87 FPU Error
    pusha
    ; Handle the x87 FPU error here
    popa
    iret
global ISR_alignment_check
ISR_alignment_check: ; Interrupt 0x11 - Alignment Check Exception
    pusha
    ; Handle the alignment check exception here
    popa
    iret
global ISR_machine_check
ISR_machine_check: ; Interrupt 0x12 - Machine Check Exception
    pusha
    ; Handle the machine check exception here
    popa
    iret
global ISR_simd_fpu_error
ISR_simd_fpu_error: ; Interrupt 0x13 - SIMD FPU Error
    pusha
    ; Handle the SIMD FPU error here
    popa
    iret
global ISR_virtualization_exception
ISR_virtualization_exception: ; Interrupt 0x14 - Virtualization Exception
    pusha
    ; Handle the virtualization exception here
    popa
    iret
global ISR_security_exception
ISR_security_exception: ; Interrupt 0x15 - Security Exception
    pusha
    ; Handle the security exception here
    popa
    iret
; Hardware Interrupt Handlers (IRQs)
; IRQ0 - Timer Interrupt
global ISR_timer
ISR_timer: ; Interrupt 0x20 - IRQ0 - Timer Interrupt
    pusha
    sti
    ; Handle the timer interrupt here
    call ISR_Handler_timer
    ; Acknowledge the interrupt to the PIC
    ; Send an End Of Interrupt (EOI) signal to the PIC
    mov al, 0x20 ; EOI command for Master PIC
    out 0x20, al ; Send EOI to Master PIC
    ; If using a Slave PIC, send EOI to it as well
    ; mov al, 0x20 ; EOI command for Slave PIC
    ; out 0xA0, al ; Send EOI to Slave PIC
    popa
    iret
; IRQ1 - Keyboard Interrupt
global ISR_keyboard
ISR_keyboard: ; Interrupt 0x21 - IRQ1 - Keyboard Interrupt
    pusha
    sti
    ; Handle the keyboard interrupt here
    call ISR_Handler_keyboard
    ; Acknowledge the interrupt to the PIC
    mov al, 0x21 ; EOI command for Master PIC
    out 0x20, al ; Send EOI to Master PIC
    ; If using a Slave PIC, send EOI to it as well
    ; mov al, 0x21 ; EOI command for Slave PIC
    ; out 0xA0, al ; Send EOI to Slave PIC
    popa
    iret
; IRQ2 - Cascade Interrupt
global ISR_cascade
ISR_cascade: ; Interrupt 0x22 - IRQ2 - Cascade Interrupt
    pusha
    ; This interrupt is triggered when the Slave PIC signals the Master PIC.
    ; Typically used to handle IRQs 8–15 from the Slave PIC.
    popa
    iret
; IRQ3 - COM2 Interrupt
global ISR_com2
ISR_com2: ; Interrupt 0x23 - IRQ3 - COM2 Interrupt
    pusha
    ; Handle the COM2 (Serial Port) interrupt here
    popa
    iret
; IRQ4 - COM1 Interrupt
global ISR_com1
ISR_com1: ; Interrupt 0x24 - IRQ4 - COM1 Interrupt
    pusha
    ; Handle the COM1 (Serial Port) interrupt here
    popa
    iret
; IRQ5 - LPT2/Sound Card Interrupt
global ISR_lpt2
ISR_lpt2: ; Interrupt 0x25 - IRQ5 - LPT2/Sound Card Interrupt
    pusha
    ; Handle the LPT2 (Printer) or Sound Card interrupt here
    popa
    iret
; IRQ6 - Floppy Disk Interrupt
global ISR_floppy
ISR_floppy: ; Interrupt 0x26 - IRQ6 - Floppy Disk Interrupt
    pusha
    ; Handle the floppy disk controller interrupt here
    popa
    iret
; IRQ7 - LPT1 Interrupt
global ISR_lpt1
ISR_lpt1: ; Interrupt 0x27 - IRQ7 - LPT1 Interrupt
    pusha
    ; Handle the LPT1 (Printer) interrupt here
    popa
    iret
; IRQ8 - Real-Time Clock (RTC) Interrupt
global ISR_rtc
ISR_rtc: ; Interrupt 0x28 - IRQ8 - RTC Interrupt
    pusha
    ; Handle the Real-Time Clock (RTC) interrupt here
    popa
    iret
; IRQ9 - ACPI Interrupt
global ISR_acpi
ISR_acpi: ; Interrupt 0x29 - IRQ9 - ACPI Interrupt
    pusha
    ; Handle the ACPI (Advanced Configuration and Power Interface) interrupt here
    popa
    iret
; IRQ10 - General Use Interrupt
global ISR_irq10
ISR_irq10: ; Interrupt 0x2A - IRQ10 - General Use Interrupt
    pusha
    ; Handle the general-purpose interrupt for IRQ10 here
    popa
    iret
; IRQ11 - General Use Interrupt
global ISR_irq11
ISR_irq11: ; Interrupt 0x2B - IRQ11 - General Use Interrupt
    pusha
    ; Handle the general-purpose interrupt for IRQ11 here
    popa
    iret
; IRQ12 - PS/2 Mouse Interrupt
global ISR_mouse
ISR_mouse: ; Interrupt 0x2C - IRQ12 - PS/2 Mouse Interrupt
    pusha
    ; Handle the PS/2 mouse interrupt here
    popa
    iret
; IRQ13 - FPU Interrupt
global ISR_fpu
ISR_fpu: ; Interrupt 0x2D - IRQ13 - FPU Interrupt
    pusha
    ; Handle the Floating Point Unit (FPU) interrupt here
    popa
    iret
; IRQ14 - Primary ATA Interrupt
global ISR_primary_ata
ISR_primary_ata: ; Interrupt 0x2E - IRQ14 - Primary ATA Interrupt
    pusha
    ; Handle the primary ATA (hard disk) interrupt here

    ; Acknowledge the interrupt to the PIC
    mov al, 0x2E ; EOI command for Master PIC
    out 0x20, al ; Send EOI to Master PIC
    ; If using a Slave PIC, send EOI to it as well
    ; mov al, 0x2E ; EOI command for Slave PIC
    ; out 0xA0, al ; Send EOI to Slave PIC
    popa
    iret
; IRQ15 - Secondary ATA Interrupt
global ISR_secondary_ata
ISR_secondary_ata: ; Interrupt 0x2F - IRQ15 - Secondary ATA Interrupt
    pusha
    ; Handle the secondary ATA (hard disk) interrupt here

    ; Acknowledge the interrupt to the PIC
    mov al, 0x2F ; EOI command for Master PIC
    out 0x20, al ; Send EOI to Master PIC
    ; If using a Slave PIC, send EOI to it as well
    ; mov al, 0x2F ; EOI command for Slave PIC
    ; out 0xA0, al ; Send EOI to Slave PIC
    popa
    iret
; Reserved Interrupts


;OS Specific interrupts

;syscall interrupt
global ISR_syscall
ISR_syscall:
    ; dont pop the registers as they are used for parameters and return values
    ; call the syscall handler
    
    call ISR_Handler_syscall

    iret

; empty initializers
global ISR_reserved
ISR_reserved: ; Interrupts 0x30 - 0x3F - Reserved
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
; Empty Initalizer
global ISR_empty
ISR_empty:
    iret

section .note.GNU-stack