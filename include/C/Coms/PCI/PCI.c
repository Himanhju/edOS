/*

To access a PCI device's configuration space:

; Build configuration address:
; Bits 31: Enable bit (must be 1)
; Bits 23-16: Bus number
; Bits 15-11: Device number
; Bits 10-8 : Function number
; Bits 7-2  : Register number (offset >> 2)
; Bits 1-0  : Always 0 (DWORD access)

mov eax, 0x80000000      ; Bus 0, Device 0, Function 0, Register 0
out dx, eax              ; Write to 0xCF8

mov dx, 0xCFC            ; Read result from 0xCFC
in eax, dx               ; EAX contains vendor/device ID

Example: Identify if a device is present:
cmp ax, 0xFFFF           ; If AX == 0xFFFF, no device present


Register Format: Building a Config Address

A 32-bit value written to 0xCF8:


Bits 31:       Enable bit (must be 1)
Bits 30-24:    Reserved (0)
Bits 23-16:    Bus number
Bits 15-11:    Device number
Bits 10-8:     Function number
Bits 7-2:      Register number (DWORD aligned)
Bits 1-0:      Must be 0
*/

#include "../../Types/Common.h"
#include "../../Memory/Opcodes/Opcodes.h"
#include "../../Memory/Operations/Operations.h"
#include "../../Graphics/VESA.h"

#include "PCI.h"

//256 busses 32 slots per bus 8 functions per slot
word PCI_Index[BUSSES][SLOTS][FUNCS];

unsigned int PCI_count = 0;

/*
word pciConfigRead_Word(byte bus, byte slot, byte func, byte offset) {
    dword address;
    dword lbus  = (dword)bus;
    dword lslot = (dword)slot;
    dword lfunc = (dword)func;
    word tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (dword)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((dword)0x80000000));
  
    // Write out the address
    outl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (word)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}
*/

// Function to read from PCI config space
word pciConfigRead_Word(byte bus, byte slot, byte func, byte offset) {
    dword address;
    word tmp = 0;

    address = (dword)((bus << 16) | (slot << 11) |
              (func << 8) | (offset & 0xFC) | ((dword)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    tmp = (word)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

dword pciConfigRead_Dword(byte bus, byte slot, byte func, byte offset) {
    dword address;
    dword tmp = 0;

    address = (dword)((bus << 16) | (slot << 11) |
              (func << 8) | (offset & 0xFC) | ((dword)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address);
    tmp = (dword)((inl(PCI_CONFIG_DATA) >> ((offset & 2) * 8)));
    return tmp;
}

void pciConfigWrite_Word(byte bus, byte slot, byte func, byte offset, word data) {
    dword address = (1U << 31) | (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC);
    dword value;

    outl(PCI_CONFIG_ADDRESS, address);
    value = inl(PCI_CONFIG_DATA);

    if (offset & 2) {
        value = (value & 0x0000FFFF) | ((dword)data << 16);
    } else {
        value = (value & 0xFFFF0000) | data;
    }

    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}

void pciConfigWrite_Dword(byte bus, byte slot, byte func, byte offset, dword data) {
    dword address;

    address = (dword)((bus << 16) | (slot << 11) |
              (func << 8) | (offset & 0xFC) | ((dword)0x80000000));

    outl(PCI_CONFIG_ADDRESS, address); // write the address
    outl(PCI_CONFIG_DATA, data);
}

word pciCheckVendor(byte bus, byte slot) {
    return pciConfigRead_Word(bus, slot, 0, 0);
}

word pciCheckDevice(byte bus, byte slot){
    return pciConfigRead_Word(bus, slot, 0, 2);
}

// PCI device information structure
void PCI_Init(void) {
    for (word bus = 0; bus < BUSSES; bus++) {
        for (byte slot = 0; slot < SLOTS; slot++) {
            for (byte func = 0; func < FUNCS; func++) {
                word vendor = pciConfigRead_Word((byte)bus, slot, func, 0);
                if (vendor != 0xFFFF) {
                    word device = pciConfigRead_Word((byte)bus, slot, func, 2);

                    PCI_Index[bus][slot][func] = device;
                    
                    PCI_count++;

                    // Output information (you can replace printf with your own kernel print function)
                    //printf("PCI Device Found - Bus: %u, Slot: %u, Function: %u, Vendor ID: 0x%04X, Device ID: 0x%04X\n",
                    //    bus, slot, func, vendor, device);
                }else{
                    PCI_Index[bus][slot][func] = 0xFFFF;
                }
            }
        }
    }
}

void Print_All_Connected_PCI_Devices(){
    char HEX[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(word b = 0; b < BUSSES; b++){
        for(byte s = 0; s < SLOTS; s++){
            for(byte f = 0; f < FUNCS; f++){
                if(PCI_Index[b][s][f] != 0xFFFF){
                    pint_8x8(b, Get_RGB(0xffffff));
                    pchar_8x8(',', Get_RGB(0xffffff));
                    pint_8x8(s, Get_RGB(0xffffff));
                    pchar_8x8(',', Get_RGB(0xffffff));
                    pint_8x8(f, Get_RGB(0xffffff));
                    newline();
                    pstr_8x8("vendor: ", Get_RGB(0xffffff));
                    Dec_To_Hex(pciConfigRead_Word((byte)b, s, f, 0), HEX);
                    pstr_8x8(HEX, Get_RGB(0xffffff));
                    memset(HEX, 0, 9);
                    newline();
                    pstr_8x8("Device: ", Get_RGB(0xffffff));
                    Dec_To_Hex(pciConfigRead_Word((byte)b, s, f, 2), HEX);
                    pstr_8x8(HEX, Get_RGB(0xffffff));
                    memset(HEX, 0, 9);
                    newline();
                    newline();
                }
            }
        }
    }
}

dword read_bar(byte bus, byte dev, byte func, byte bar_num) {
    dword address = (1U << 31) | (bus << 16) | (dev << 11) | (func << 8) | (dword)(0x10 + bar_num * 4);
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}