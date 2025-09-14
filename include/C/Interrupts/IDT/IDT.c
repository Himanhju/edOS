#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43

// Channel 0, Access Mode: lobyte/hibyte, Mode 3 (square wave)
#define PIT_MODE 0x36


#include <Interrupts/ISRs/ISRs.h>
#include <Memory/Opcodes/Opcodes.h>
#include <Types/Common.h>
#include <Input/Keyboard/Keyboard.h>

#include "IDT.h"

typedef struct InterruptDescriptor32 {
    unsigned short offset_1;        // offset bits 0..15
    unsigned short selector;        // a code segment selector in GDT or LDT
    unsigned char  zero;            // unused, set to 0
    unsigned char  type_attributes; // gate type, dpl, and p fields
    unsigned short offset_2;        // offset bits 16..31
}__attribute__((packed))ID32;

typedef struct {
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed)) IDT_desc;

volatile ID32 IDT[256];

volatile IDT_desc loaded;

static void PIC_remap(void) {
    // ICW1: Start initialization of both PICs
    outb(0x20, 0x11); // Master PIC command port
    outb(0xA0, 0x11); // Slave PIC command port

    // ICW2: Set vector offsets for the PICs
    outb(0x21, 0x20); // Master PIC vector offset (0x20)
    outb(0xA1, 0x28); // Slave PIC vector offset (0x28)

    // ICW3: Configure Master and Slave PICs
    outb(0x21, 0x04); // Tell Master PIC that there is a Slave PIC at IRQ2 (0000 0100)
    outb(0xA1, 0x02); // Tell Slave PIC its cascade identity (0000 0010)

    // ICW4: Set PICs to 8086/88 mode
    outb(0x21, 0x01); // Master PIC
    outb(0xA1, 0x01); // Slave PIC

    // Mask all interrupts (optional, can be adjusted later)
    outb(0x21, 0x00); // Enable all IRQs on Master PIC
    outb(0xA1, 0x00); // Enable all IRQs on Slave PIC
}

static void Keyboard_Init(void){
    // Enable the keyboard device
    outb(0x64, 0xAE); // Command to enable the keyboard

    // Enable keyboard interrupts
    outb(0x60, 0xF4); // Command to enable scanning
}

static void PIT_Set_Interval(unsigned int interval){
    unsigned short divisor = (unsigned short)(1193182 / interval);
    
    // Send command byte
    outb(PIT_COMMAND, PIT_MODE);
    
    // Send low byte of divisor
    outb(PIT_CHANNEL0, (byte) (divisor & 0xFF) );
    
    // Send high byte of divisor
    outb(PIT_CHANNEL0, (byte) ((divisor >> 8) & 0xFF) );
}

static void Set_IDT_Entry(byte entry, void *func, unsigned short selector, byte types_and_attributes){
    IDT[entry].offset_1             =   (unsigned short)((unsigned int) func & 0x0000FFFF);
    IDT[entry].selector             =   selector;
    IDT[entry].zero                 =   0x00;
    IDT[entry].type_attributes      =   types_and_attributes;
    IDT[entry].offset_2             =   (unsigned short)((((unsigned int) func) >> 16) & 0xFFFF);
}

static void load_IDT(void){
    loaded.limit = sizeof(IDT) - 1;
    loaded.base = (unsigned int)IDT;
    asm(
    "lidt (%0)"
    :
    : "r"(&loaded)
    : "memory"
    );
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

void IDT_Init(void){
    pstr_8x8("Initializing IDT\n", Get_RGB(0xffffff));

    PIT_Set_Interval(1000); // Set PIT interval to 100 Hz (10 ms per tick)

    for (int i = 0; i < 256; i++) {
        Set_IDT_Entry((byte)i, ISR_empty, 0x08, 0x8E);
    }

    // traps
    Set_IDT_Entry(0x00, ISR_divide_error, 0x08, 0x8E);
    Set_IDT_Entry(0x01, ISR_empty, 0x08, 0x8F);
    Set_IDT_Entry(0x03, ISR_empty, 0x08, 0x8F);
    Set_IDT_Entry(0x04, ISR_empty, 0x08, 0x8F);

    // load IRQs
    Set_IDT_Entry(0x20, ISR_timer, 0x08, 0x8E);
    Set_IDT_Entry(0x21, ISR_keyboard, 0x08, 0x8E);
    Set_IDT_Entry(0x22, ISR_cascade, 0x08, 0x8E);
    Set_IDT_Entry(0x23, ISR_com2, 0x08, 0x8E);
    Set_IDT_Entry(0x24, ISR_com1, 0x08, 0x8E);
    Set_IDT_Entry(0x25, ISR_lpt2, 0x08, 0x8E);
    Set_IDT_Entry(0x26, ISR_floppy, 0x08, 0x8E);
    Set_IDT_Entry(0x27, ISR_lpt1, 0x08, 0x8E);
    Set_IDT_Entry(0x28, ISR_rtc, 0x08, 0x8E); 
    Set_IDT_Entry(0x29, ISR_acpi, 0x08, 0x8E);
    Set_IDT_Entry(0x2A, ISR_irq10, 0x08, 0x8E);
    Set_IDT_Entry(0x2B, ISR_irq11, 0x08, 0x8E);
    Set_IDT_Entry(0x2C, ISR_mouse, 0x08, 0x8E);
    Set_IDT_Entry(0x2D, ISR_fpu, 0x08, 0x8E);
    Set_IDT_Entry(0x2E, ISR_primary_ata, 0x08, 0x8E);
    Set_IDT_Entry(0x2F, ISR_secondary_ata, 0x08, 0x8E);

    //OS Specific
    Set_IDT_Entry(0x80, ISR_syscall, 0x08, 0xEE);

    PIC_remap();

    Keyboard_Init();
    
    load_IDT();

    asm("sti");

    pstr_8x8("IDT Initialized Successfully\n\n", Get_RGB(0x7CFC00));
}

#pragma GCC diagnostic pop