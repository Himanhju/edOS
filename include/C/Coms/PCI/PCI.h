#ifndef PCI_H
#define PCI_H

#include <Types/Common.h>

#define BUSSES 256
#define SLOTS 32
#define FUNCS 8

#define MAX_BARS 6

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// the PCI command offset
#define PCICMD_OFFSET   0x04

// the bits of the command offset
#define PCICMD_IO       (1)
#define PCICMD_MEM      (1 << 1)
#define PCICMD_MASTER   (1 << 2)

typedef struct {
    word bus;
    byte slot;
    byte func;
} PCI_Device;


typedef enum PCI_IDENTIFIER{
    VENDOR,
    DEVICE
}PCI_ID;

extern unsigned int PCI_count;

extern word PCI_Index[BUSSES][SLOTS][FUNCS][2];

extern PCI_Device Highs;

//initializes the PCI_Index which says which slots, busses, functions are connected
void PCI_Init(void);

//reads a word from a pci function of a slot of a bus at an offset of offset
word pciConfigRead_Word(byte bus, byte slot, byte func, byte offset);

dword pciConfigRead_Dword(byte bus, byte slot, byte func, byte offset);

void pciConfigWrite_Word(byte bus, byte slot, byte func, byte offset, word data);

void pciConfigWrite_Dword(byte bus, byte slot, byte func, byte offset, dword data);

//returns the vendor id of the pci slot and bus
word pciCheckVendor(byte bus, byte slot);

//returns the device id of the pci Slot and Bus
word pciCheckDevice(byte bus, byte slot);

//Prints all connected PCI devices with their bus slot and function info
void Print_All_Connected_PCI_Devices(void);

//returns the BAR given the parameters
dword read_bar(byte bus, byte dev, byte func, byte bar_num);

//writes Data into the bar specified
void write_bar(byte bus, byte dev, byte func, byte bar_num, dword Data);

//enables bus mastering on a device
byte Enable_Bus_Mastering(byte bus, byte slot, byte func);

#endif