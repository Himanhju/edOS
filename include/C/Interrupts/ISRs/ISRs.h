#ifndef ISRS_H
#define ISRS_H

#include "../../Types/Common.h"

extern volatile unsigned int   PIT_Ticks;

extern volatile unsigned int   PIT_Seconds;

extern volatile byte Scancode;

extern volatile byte Key_Pressed;

extern volatile unsigned int tick_count;

//the isr for when div 0 occurs
extern void ISR_divide_error();

//the timer isr which runs every millisecond
extern void ISR_timer(); // Interrupt 0x20 - IRQ0 - Timer Interrupt

// IRQ1 - Keyboard Interrupt
extern void ISR_keyboard(); // Interrupt 0x21 - IRQ1 - Keyboard Interrupt

// IRQ2 - Cascade Interrupt
extern void ISR_cascade(); // Interrupt 0x22 - IRQ2 - Cascade Interrupt
// Typically used to handle IRQs 8–15 from the Slave PIC.

// IRQ3 - COM2 Interrupt
extern void ISR_com2(); // Interrupt 0x23 - IRQ3 - COM2 Interrupt

// IRQ4 - COM1 Interrupt
extern void ISR_com1(); // Interrupt 0x24 - IRQ4 - COM1 Interrupt

// IRQ5 - LPT2/Sound Card Interrupt
extern void ISR_lpt2(); // Interrupt 0x25 - IRQ5 - LPT2/Sound Card Interrupt

// IRQ6 - Floppy Disk Interrupt
extern void ISR_floppy(); // Interrupt 0x26 - IRQ6 - Floppy Disk Interrupt

// IRQ7 - LPT1 Interrupt
extern void ISR_lpt1(); // Interrupt 0x27 - IRQ7 - LPT1 Interrupt

// IRQ8 - Real-Time Clock (RTC) Interrupt
extern void ISR_rtc(); // Interrupt 0x28 - IRQ8 - RTC Interrupt

// IRQ9 - ACPI Interrupt
extern void ISR_acpi(); // Interrupt 0x29 - IRQ9 - ACPI Interrupt

// IRQ10 - General Use Interrupt
extern void ISR_irq10(); // Interrupt 0x2A - IRQ10 - General Use Interrupt

// IRQ11 - General Use Interrupt
extern void ISR_irq11(); // Interrupt 0x2B - IRQ11 - General Use Interrupt

// IRQ12 - PS/2 Mouse Interrupt
extern void ISR_mouse(); // Interrupt 0x2C - IRQ12 - PS/2 Mouse Interrupt

// IRQ13 - FPU Interrupt
extern void ISR_fpu(); // Interrupt 0x2D - IRQ13 - FPU Interrupt

// IRQ14 - Primary ATA Interrupt
extern void ISR_primary_ata(); // Interrupt 0x2E - IRQ14 - Primary ATA Interrupt

// IRQ15 - Secondary ATA Interrupt
extern void ISR_secondary_ata(); // Interrupt 0x2F - IRQ15 - Secondary ATA Interrupt
// Reserved Interrupts();// Interrupts 0x30 - 0x3F - Reservedextern void ISR_reserved(extern void);
// They may be used by future hardware or software standards.

//Empty Initializer
extern void ISR_empty();

#endif