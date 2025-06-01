#ifndef PCI_H
#define PCI_H

#define BUSSES 256
#define SLOTS 32
#define FUNCS 8

typedef struct {
    word vendorID;
    word deviceID;
    byte bus;
    byte slot;
    byte function;
} PCI_Device;


extern word PCI_Index[BUSSES][SLOTS][FUNCS];

//initializes the PCI_Index which says which slots, busses, functions are connected
void PCI_Init(void);

//reads a word from a pci function of a slot of a bus at an offset of offset
word pciConfigReadWord(byte bus, byte slot, byte func, byte offset);

//returns the vendor id of the pci slot and bus
word pciCheckVendor(byte bus, byte slot);

//returns the device id of the pci Slot and Bus
word pciCheckDevice(byte bus, byte slot);

//Prints all connected PCI devices with their bus slot and function info
void Print_All_Connected_PCI_Devices(void);

#endif