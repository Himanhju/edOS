#include "../../Memory/Opcodes/Opcodes.h"
#include "../../Graphics/VESA.h"
#include "../Syscall/Syscall.h"
#include "../../Coms/IO/IO.h"

#include "ISRs.h"

volatile unsigned int   PIT_Ticks = 0;

volatile unsigned int   PIT_Seconds = 0;

volatile byte Scancode;

volatile byte Key_Pressed = 0; // Global variable to track if a key is pressed

volatile unsigned int tick_count = 0; // Global variable to track ticks

// file for handlers

void ISR_Handler_divide_error(void){ // Interrupt 0x00 - Divide Error
    
    // Handle the divide error here
    clr_scrn();
    pstr_8x8("ERR: DIV 0: HALTING", Get_RGB(0xFF0000));
    asm("cli\n" "hlt");
}
    //if you divide by 0 the computer will halt
void ISR_Handler_debug(void){ // Interrupt 0x01 - Debug Exception
    
    // Handle the debug exception here
    
    
}
void ISR_Handler_nmi(void){ // Interrupt 0x02 - Non-Maskable Interrupt
    
    // Handle the non-maskable interrupt here
    
    
}
void ISR_Handler_breakpoint(void){ // Interrupt 0x03 - Breakpoint Exception
    
    // Handle the breakpoint exception here
    
    
}
void ISR_Handler_overflow(void){ // Interrupt 0x04 - Overflow Exception
    
    // Handle the overflow exception here
    
    
}
void ISR_Handler_bound_range_exceeded(void){ // Interrupt 0x05 - Bound Range Exceeded
    
    // Handle the bound range exceeded exception here
    
    
}
void ISR_Handler_invalid_opcode(void){ // Interrupt 0x06 - Invalid Opcode Exception
    
    // Handle the invalid opcode exception here
    clr_scrn();
    pstr_8x8("ERR: INVALID OPCODE: HALTING", Get_RGB(0xff0000));
    asm("cli\n" "hlt");
    
}
void ISR_Handler_device_not_available(void){ // Interrupt 0x07 - Device Not Available Exception
    
    // Handle the device not available exception here
    
    
}
void ISR_Handler_double_fault(void){ // Interrupt 0x08 - Double Fault Exception
    
    // Handle the double fault exception here
    clr_scrn();
    pstr_8x8("ERR: DOUBLE FAULT: HALTING", Get_RGB(0xFF0000));
    asm("cli\n" "hlt");
    
}
void ISR_Handler_coprocessor_segment_overrun(void){ // Interrupt 0x09 - Coprocessor Segment Overrun
    
    // Handle the coprocessor segment overrun exception here
    
    
}
void ISR_Handler_invalid_tss(void){ // Interrupt 0x0A - Invalid TSS Exception
    
    // Handle the invalid TSS exception here
    
    
}
void ISR_Handler_segment_not_present(void){ // Interrupt 0x0B - Segment Not Present Exception
    
    // Handle the segment not present exception here
    
    
}
void ISR_Handler_stack_segment_fault(void){ // Interrupt 0x0C - Stack Segment Fault
    
    // Handle the stack segment fault exception here
    
    
}
void ISR_Handler_general_protection_fault(void){ // Interrupt 0x0D - General Protection Fault
    
    // Handle the general protection fault here
    
    
}
void ISR_Handler_page_fault(void){ // Interrupt 0x0E - Page Fault Exception
    
    // Handle the page fault exception here
    
    
}
// do not use int 0x0F as if is reserved for intel and should not be edited
void ISR_Handler_x87_fpu_error(void){ // Interrupt 0x10 - x87 FPU Error
    
    // Handle the x87 FPU error here
    
    
}
void ISR_Handler_alignment_check(void){ // Interrupt 0x11 - Alignment Check Exception
    
    // Handle the alignment check exception here
    
    
}
void ISR_Handler_machine_check(void){ // Interrupt 0x12 - Machine Check Exception
    
    // Handle the machine check exception here
    
    
}
void ISR_Handler_simd_fpu_error(void){ // Interrupt 0x13 - SIMD FPU Error
    
    // Handle the SIMD FPU error here
    
    
}
void ISR_Handler_virtualization_exception(void){ // Interrupt 0x14 - Virtualization Exception
    
    // Handle the virtualization exception here
    
    
}
void ISR_Handler_security_exception(void){ // Interrupt 0x15 - Security Exception
    
    // Handle the security exception here
    
    
}

// Hardware Interrupt Handlers (IRQs)

// IRQ0 - Timer Interrupt
void ISR_Handler_timer(void){ // Interrupt 0x20 - IRQ0 - Timer Interrupt
    // ========== actual timer ==========
    tick_count++;
    if (tick_count % 1000 == 0) {
        PIT_Seconds++;
        // Debugging: Increment PIT_Seconds and print a visual indicator
        // Uncomment the following line if debugging is needed
        // pchar_8x8(Graphics_A_8x8, Get_RGB(0xFFFFFF));
    }
    tick_count %= 2000;
    PIT_Ticks = tick_count; // Update the global PIT_Ticks for external use
    //===================================

    int esp;

    asm volatile(
        "movl %%esp, %0"
        : "=m"(esp)
        :
        :   
    );

    if(esp < ESP_MIN){
        esp = 0;
        pstr_8x8("\nSTACK OVERFLOW: RESETTING TO ORIGINAL\n", Get_RGB(0xffffff));
        asm volatile(
            "movl $0x70000, %%esp"
            :
            :
            :
        );
        Read_IO();
    }
}

// IRQ1 - Keyboard Interrupt
void ISR_Handler_keyboard(void) { // Interrupt 0x21 - IRQ1 - Keyboard Interrupt
    Scancode = inb(0x60); // Read the scancode from the keyboard controller

    if ((Scancode & 0x80) == 0) { // Key press event (highest bit is 0)
        Key_Pressed = 1; // Set key_pressed to indicate a key is being pressed
    } else {
        Key_Pressed = 0; // Reset key_pressed on key release
    }
}

// IRQ2 - Cascade Interrupt
void ISR_Handler_cascade(void){ // Interrupt 0x22 - IRQ2 - Cascade Interrupt
    
    // This interrupt is triggered when the Slave PIC signals the Master PIC.
    // Typically used to handle IRQs 8–15 from the Slave PIC.
    
    
}

// IRQ3 - COM2 Interrupt
void ISR_Handler_com2(void){ // Interrupt 0x23 - IRQ3 - COM2 Interrupt
    
    // Handle the COM2 (Serial Port) interrupt here
    
    
}

// IRQ4 - COM1 Interrupt
void ISR_Handler_com1(void){ // Interrupt 0x24 - IRQ4 - COM1 Interrupt
    
    // Handle the COM1 (Serial Port) interrupt here
    
    
}

// IRQ5 - LPT2/Sound Card Interrupt
void ISR_Handler_lpt2(void){ // Interrupt 0x25 - IRQ5 - LPT2/Sound Card Interrupt
    
    // Handle the LPT2 (Printer) or Sound Card interrupt here
    
    
}

// IRQ6 - Floppy Disk Interrupt
void ISR_Handler_floppy(void){ // Interrupt 0x26 - IRQ6 - Floppy Disk Interrupt
    
    // Handle the floppy disk controller interrupt here
    
    
}

// IRQ7 - LPT1 Interrupt
void ISR_Handler_lpt1(void){ // Interrupt 0x27 - IRQ7 - LPT1 Interrupt
    
    // Handle the LPT1 (Printer) interrupt here
    
    
}

// IRQ8 - Real-Time Clock (RTC) Interrupt
void ISR_Handler_rtc(void){ // Interrupt 0x28 - IRQ8 - RTC Interrupt
    
    // Handle the Real-Time Clock (RTC) interrupt here
    
}

// IRQ9 - ACPI Interrupt
void ISR_Handler_acpi(void){ // Interrupt 0x29 - IRQ9 - ACPI Interrupt
    
    // Handle the ACPI (Advanced Configuration and Power Interface) interrupt here
    
    
}

// IRQ10 - General Use Interrupt
void ISR_Handler_irq10(void){ // Interrupt 0x2A - IRQ10 - General Use Interrupt
    
    // Handle the general-purpose interrupt for IRQ10 here
    
    
}

// IRQ11 - General Use Interrupt
void ISR_Handler_irq11(void){ // Interrupt 0x2B - IRQ11 - General Use Interrupt
    
    // Handle the general-purpose interrupt for IRQ11 here
    
    
}

// IRQ12 - PS/2 Mouse Interrupt
void ISR_Handler_mouse(void){ // Interrupt 0x2C - IRQ12 - PS/2 Mouse Interrupt
    
    // Handle the PS/2 mouse interrupt here
    
    
}

// IRQ13 - FPU Interrupt
void ISR_Handler_fpu(void){ // Interrupt 0x2D - IRQ13 - FPU Interrupt
    
    // Handle the Floating Point Unit (FPU) interrupt here
    
    
}

// IRQ14 - Primary ATA Interrupt
void ISR_Handler_primary_ata(void){ // Interrupt 0x2E - IRQ14 - Primary ATA Interrupt
    
    // Handle the primary ATA (hard disk) interrupt here
    
    
}

// IRQ15 - Secondary ATA Interrupt
void ISR_Handler_secondary_ata(void){ // Interrupt 0x2F - IRQ15 - Secondary ATA Interrupt
    
    // Handle the secondary ATA (hard disk) interrupt here
    
    
}

//OS Specific interrupts

//syscall interrupt
void ISR_Handler_syscall(void){
    //call the syscall function

    cli();
    
    //TODO change ss, esp and other registers


    //dont push the GP registers as they are parameters
    int EAX, EBX, ECX, EDX, EDI, ESI;
    asm(
        "movl %%eax, %0\n" // copy all the registers into the variable
        "movl %%ebx, %1\n"
        "movl %%ecx, %2\n"
        "movl %%edx, %3\n"
        "movl %%edi, %4\n"
        "movl %%esi, %5\n"
        : "=m"(EAX), "=m"(EBX), "=m"(ECX), "=m"(EDX), "=m"(EDI), "=m"(ESI)
        :
        : "eax", "ebx", "ecx", "edx", "edi", "esi"
    );

    EAX = syscall(EAX, EBX, ECX, EDX, EDI, ESI);

    asm(
        "movl %0, %%eax" // copy the variable to the register
        :
        : "r"(EAX)
        : "eax"
    );

    sti();
}