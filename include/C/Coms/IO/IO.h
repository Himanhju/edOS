#ifndef IO_H
#define IO_H

#include <Types/Common.h>

#include <Coms/PCI/PCI.h>

typedef     void*    mmio;      // memory mapped IO port type (pointer to a region in memory) 
typedef     word     IO_port;   // IO Ports for the out and in opcodess

// The type of IO that is done to communicate with the device
enum IO_Type {
    MMIO,
    PORT
};

// The type of devices (internet, graphics, ect.)
enum IO_Dev {
    INET,
    DISC,
    PMC,
    ACPI,
    IDE,
    ISA,
    GRAPH
};

// A General IO struct for all devices
typedef struct IO{
    PCI_Device Device;
    //dword bar[MAX_BARS];
}IO;

#define INTEL_VENDOR_ID     0x8086
#define REDHAT_VENDOR_ID    0x1B36
#define BOCH_VENDOR_ID      0x1234


#define Get_Device(Vendor, Device) ((Vendor << (16)) | Device)

// returns the type of IO device
enum IO_Type check_IO_type(byte bus, byte dev, byte func, byte bar_num);

// initializes IO_RAM to store IO info
void IO_Init(void);

dword IO_Identify(void);

#endif