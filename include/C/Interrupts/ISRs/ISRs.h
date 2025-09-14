#ifndef ISRS_H
#define ISRS_H

#include <Types/Common.h>

#define ESP_MIN         0x5090 // decided by creators friend because he was thinking of the rtx 5090

#define TICKS_MAX_SIZE  1999

extern volatile unsigned int   PIT_Ticks;

extern volatile unsigned int   PIT_Seconds;

extern volatile byte Scancode;

extern volatile byte Key_Pressed;

extern volatile unsigned int tick_count;

//the isr for when div 0 occurs
extern void ISR_divide_error(void);

//the timer isr which runs every millisecond
extern void ISR_timer(void); // Interrupt 0x20 - IRQ0 - Timer Interrupt

// IRQ1 - Keyboard Interrupt
extern void ISR_keyboard(void); // Interrupt 0x21 - IRQ1 - Keyboard Interrupt

// IRQ2 - Cascade Interrupt
extern void ISR_cascade(void); // Interrupt 0x22 - IRQ2 - Cascade Interrupt
// Typically used to handle IRQs 8–15 from the Slave PIC.

// IRQ3 - COM2 Interrupt
extern void ISR_com2(void); // Interrupt 0x23 - IRQ3 - COM2 Interrupt

// IRQ4 - COM1 Interrupt
extern void ISR_com1(void); // Interrupt 0x24 - IRQ4 - COM1 Interrupt

// IRQ5 - LPT2/Sound Card Interrupt
extern void ISR_lpt2(void); // Interrupt 0x25 - IRQ5 - LPT2/Sound Card Interrupt

// IRQ6 - Floppy Disk Interrupt
extern void ISR_floppy(void); // Interrupt 0x26 - IRQ6 - Floppy Disk Interrupt

// IRQ7 - LPT1 Interrupt
extern void ISR_lpt1(void); // Interrupt 0x27 - IRQ7 - LPT1 Interrupt

// IRQ8 - Real-Time Clock (RTC) Interrupt
extern void ISR_rtc(void); // Interrupt 0x28 - IRQ8 - RTC Interrupt

// IRQ9 - ACPI Interrupt
extern void ISR_acpi(void); // Interrupt 0x29 - IRQ9 - ACPI Interrupt

// IRQ10 - General Use Interrupt
extern void ISR_irq10(void); // Interrupt 0x2A - IRQ10 - General Use Interrupt

// IRQ11 - General Use Interrupt
extern void ISR_irq11(void); // Interrupt 0x2B - IRQ11 - General Use Interrupt

// IRQ12 - PS/2 Mouse Interrupt
extern void ISR_mouse(void); // Interrupt 0x2C - IRQ12 - PS/2 Mouse Interrupt

// IRQ13 - FPU Interrupt
extern void ISR_fpu(void); // Interrupt 0x2D - IRQ13 - FPU Interrupt

// IRQ14 - Primary ATA Interrupt
extern void ISR_primary_ata(void); // Interrupt 0x2E - IRQ14 - Primary ATA Interrupt

// IRQ15 - Secondary ATA Interrupt
extern void ISR_secondary_ata(void); // Interrupt 0x2F - IRQ15 - Secondary ATA Interrupt
// Reserved Interrupts(void);;// Interrupts 0x30 - 0x3F - Reservedextern void ISR_reserved(extern void);
// They may be used by future hardware or software standards.

//OS specific interrupts
extern void ISR_syscall(void); // Interrupt 0x80 - Syscall

//Empty Initializer
extern void ISR_empty(void);

//if you divide by 0 the computer will halt
void ISR_Handler_divide_error(void);
    
void ISR_Handler_debug(void);

void ISR_Handler_nmi(void);

void ISR_Handler_breakpoint(void);

void ISR_Handler_overflow(void);

void ISR_Handler_bound_range_exceeded(void);

void ISR_Handler_invalid_opcode(void);

void ISR_Handler_device_not_available(void);

void ISR_Handler_double_fault(void);

void ISR_Handler_coprocessor_segment_overrun(void);

void ISR_Handler_invalid_tss(void);

void ISR_Handler_segment_not_present(void);

void ISR_Handler_stack_segment_fault(void);

void ISR_Handler_general_protection_fault(void);

void ISR_Handler_page_fault(void);

// do not use int 0x0F as if is reserved for intel and should not be edited
void ISR_Handler_x87_fpu_error(void);

void ISR_Handler_alignment_check(void);

void ISR_Handler_machine_check(void);

void ISR_Handler_simd_fpu_error(void);

void ISR_Handler_virtualization_exception(void);

void ISR_Handler_security_exception(void);

// Hardware Interrupt Handlers (IRQs)

// IRQ0 - Timer Interrupt
void ISR_Handler_timer(void);

// IRQ1 - Keyboard Interrupt
void ISR_Handler_keyboard(void);

// IRQ2 - Cascade Interrupt
void ISR_Handler_cascade(void);

// IRQ3 - COM2 Interrupt
void ISR_Handler_com2(void);

// IRQ4 - COM1 Interrupt
void ISR_Handler_com1(void);

// IRQ5 - LPT2/Sound Card Interrupt
void ISR_Handler_lpt2(void);

// IRQ6 - Floppy Disk Interrupt
void ISR_Handler_floppy(void);

// IRQ7 - LPT1 Interrupt
void ISR_Handler_lpt1(void);

// IRQ8 - Real-Time Clock (RTC) Interrupt
void ISR_Handler_rtc(void);

// IRQ9 - ACPI Interrupt
void ISR_Handler_acpi(void);

// IRQ10 - General Use Interrupt
void ISR_Handler_irq10(void);

// IRQ11 - General Use Interrupt
void ISR_Handler_irq11(void);

// IRQ12 - PS/2 Mouse Interrupt
void ISR_Handler_mouse(void);

// IRQ13 - FPU Interrupt
void ISR_Handler_fpu(void);

// IRQ14 - Primary ATA Interrupt
void ISR_Handler_primary_ata(void);

// IRQ15 - Secondary ATA Interrupt
void ISR_Handler_secondary_ata(void);

//OS Specific interrupts

//syscall interrupt
void ISR_Handler_syscall(void);

#endif